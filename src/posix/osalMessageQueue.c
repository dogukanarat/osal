/* osalMessageQueue.c - POSIX Message Queue Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osalMessageQueue.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

/* Internal Types */
typedef struct
{
    uint8_t *buffer;
    uint32_t depth;
    uint32_t itemSize;
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    pthread_mutex_t mutex;
    pthread_cond_t notEmpty;
    pthread_cond_t notFull;
    bool isStaticCb;
    bool isStaticBuf;
} osalQueueControlBlock_t;

/* Functions */

osalMessageQueueHandle_t osalMessageQueueCreate(uint32_t depth, uint32_t itemSize, const osalMessageQueueAttr_t *attr)
{
    osalQueueControlBlock_t *cb;
    uint8_t *buffer;

    if (depth == 0 || itemSize == 0)
    {
        return NULL;
    }

    /* Allocate control block */
    if (attr && attr->cbMem)
    {
        if (attr->cbSize < sizeof(osalQueueControlBlock_t))
        {
            return NULL;
        }
        cb = (osalQueueControlBlock_t *)attr->cbMem;
        cb->isStaticCb = true;
    }
    else
    {
        cb = (osalQueueControlBlock_t *)malloc(sizeof(osalQueueControlBlock_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->isStaticCb = false;
    }

    /* Allocate buffer */
    if (attr && attr->mqMem)
    {
        if (attr->mqSize < (depth * itemSize))
        {
            if (!cb->isStaticCb) free(cb);
            return NULL;
        }
        buffer = (uint8_t *)attr->mqMem;
        cb->isStaticBuf = true;
    }
    else
    {
        buffer = (uint8_t *)malloc(depth * itemSize);
        if (buffer == NULL)
        {
            if (!cb->isStaticCb) free(cb);
            return NULL;
        }
        cb->isStaticBuf = false;
    }

    /* Initialize control block */
    cb->buffer = buffer;
    cb->depth = depth;
    cb->itemSize = itemSize;
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;

    pthread_mutex_init(&cb->mutex, NULL);
    pthread_cond_init(&cb->notEmpty, NULL);
    pthread_cond_init(&cb->notFull, NULL);

    return (osalMessageQueueHandle_t)cb;
}

osalStatus_t osalMessageQueueDelete(osalMessageQueueHandle_t queue)
{
    osalQueueControlBlock_t *cb = (osalQueueControlBlock_t *)queue;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    pthread_mutex_destroy(&cb->mutex);
    pthread_cond_destroy(&cb->notEmpty);
    pthread_cond_destroy(&cb->notFull);

    if (!cb->isStaticBuf)
    {
        free(cb->buffer);
    }

    if (!cb->isStaticCb)
    {
        free(cb);
    }

    return OSAL_SUCCESS;
}

osalStatus_t osalMessageQueueSend(osalMessageQueueHandle_t queue, const void *item, uint32_t timeoutMs)
{
    osalQueueControlBlock_t *cb = (osalQueueControlBlock_t *)queue;
    struct timespec ts;
    int ret = 0;

    if (cb == NULL || item == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    pthread_mutex_lock(&cb->mutex);

    while (cb->count >= cb->depth)
    {
        if (timeoutMs == OSAL_NO_WAIT)
        {
            pthread_mutex_unlock(&cb->mutex);
            return OSAL_ERROR_TIMEOUT;
        }
        else if (timeoutMs == OSAL_WAIT_FOREVER)
        {
            pthread_cond_wait(&cb->notFull, &cb->mutex);
        }
        else
        {
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += timeoutMs / 1000;
            ts.tv_nsec += (timeoutMs % 1000) * 1000000;
            if (ts.tv_nsec >= 1000000000)
            {
                ts.tv_sec++;
                ts.tv_nsec -= 1000000000;
            }
            ret = pthread_cond_timedwait(&cb->notFull, &cb->mutex, &ts);
            if (ret == ETIMEDOUT)
            {
                pthread_mutex_unlock(&cb->mutex);
                return OSAL_ERROR_TIMEOUT;
            }
        }
    }

    memcpy(&cb->buffer[cb->tail * cb->itemSize], item, cb->itemSize);
    cb->tail = (cb->tail + 1) % cb->depth;
    cb->count++;

    pthread_cond_signal(&cb->notEmpty);
    pthread_mutex_unlock(&cb->mutex);

    return OSAL_SUCCESS;
}

osalStatus_t osalMessageQueueReceive(osalMessageQueueHandle_t queue, void *item, uint32_t timeoutMs)
{
    osalQueueControlBlock_t *cb = (osalQueueControlBlock_t *)queue;
    struct timespec ts;
    int ret = 0;

    if (cb == NULL || item == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    pthread_mutex_lock(&cb->mutex);

    while (cb->count == 0)
    {
        if (timeoutMs == OSAL_NO_WAIT)
        {
            pthread_mutex_unlock(&cb->mutex);
            return OSAL_ERROR_TIMEOUT;
        }
        else if (timeoutMs == OSAL_WAIT_FOREVER)
        {
            pthread_cond_wait(&cb->notEmpty, &cb->mutex);
        }
        else
        {
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += timeoutMs / 1000;
            ts.tv_nsec += (timeoutMs % 1000) * 1000000;
            if (ts.tv_nsec >= 1000000000)
            {
                ts.tv_sec++;
                ts.tv_nsec -= 1000000000;
            }
            ret = pthread_cond_timedwait(&cb->notEmpty, &cb->mutex, &ts);
            if (ret == ETIMEDOUT)
            {
                pthread_mutex_unlock(&cb->mutex);
                return OSAL_ERROR_TIMEOUT;
            }
        }
    }

    memcpy(item, &cb->buffer[cb->head * cb->itemSize], cb->itemSize);
    cb->head = (cb->head + 1) % cb->depth;
    cb->count--;

    pthread_cond_signal(&cb->notFull);
    pthread_mutex_unlock(&cb->mutex);

    return OSAL_SUCCESS;
}

uint32_t osalMessageQueueGetCount(osalMessageQueueHandle_t queue)
{
    osalQueueControlBlock_t *cb = (osalQueueControlBlock_t *)queue;
    uint32_t count;

    if (cb == NULL)
    {
        return 0;
    }

    pthread_mutex_lock(&cb->mutex);
    count = cb->count;
    pthread_mutex_unlock(&cb->mutex);

    return count;
}

osalStatus_t osalMessageQueueReset(osalMessageQueueHandle_t queue)
{
    osalQueueControlBlock_t *cb = (osalQueueControlBlock_t *)queue;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    pthread_mutex_lock(&cb->mutex);
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    pthread_cond_broadcast(&cb->notFull); /* Wake up any senders waiting for space */
    pthread_mutex_unlock(&cb->mutex);

    return OSAL_SUCCESS;
}
