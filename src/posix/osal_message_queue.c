/* osalMessageQueue.c - POSIX Message Queue Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_message_queue.h"
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
    uint32_t item_size;
    uint32_t head;
    uint32_t tail;
    uint32_t count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    bool is_static_cb;
    bool is_static_buf;
} osal_queue_control_block_t;

/* Functions */

osal_message_queue_handle_t osal_message_queue_create(uint32_t depth, uint32_t item_size, const osal_message_queue_attr_t *attr)
{
    osal_queue_control_block_t *cb;
    uint8_t *buffer;

    if (depth == 0 || item_size == 0)
    {
        return NULL;
    }

    /* Allocate control block */
    if (attr && attr->cb_mem)
    {
        if (attr->cb_size < sizeof(osal_queue_control_block_t))
        {
            return NULL;
        }
        cb = (osal_queue_control_block_t *)attr->cb_mem;
        cb->is_static_cb = true;
    }
    else
    {
        cb = (osal_queue_control_block_t *)malloc(sizeof(osal_queue_control_block_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->is_static_cb = false;
    }

    /* Allocate buffer */
    if (attr && attr->mq_mem)
    {
        if (attr->mq_size < (depth * item_size))
        {
            if (!cb->is_static_cb) free(cb);
            return NULL;
        }
        buffer = (uint8_t *)attr->mq_mem;
        cb->is_static_buf = true;
    }
    else
    {
        buffer = (uint8_t *)malloc(depth * item_size);
        if (buffer == NULL)
        {
            if (!cb->is_static_cb) free(cb);
            return NULL;
        }
        cb->is_static_buf = false;
    }

    /* Initialize control block */
    cb->buffer = buffer;
    cb->depth = depth;
    cb->item_size = item_size;
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;

    pthread_mutex_init(&cb->mutex, NULL);
    pthread_cond_init(&cb->not_empty, NULL);
    pthread_cond_init(&cb->not_full, NULL);

    return (osal_message_queue_handle_t)cb;
}

osal_status_t osal_message_queue_delete(osal_message_queue_handle_t queue)
{
    osal_queue_control_block_t *cb = (osal_queue_control_block_t *)queue;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    pthread_mutex_destroy(&cb->mutex);
    pthread_cond_destroy(&cb->not_empty);
    pthread_cond_destroy(&cb->not_full);

    if (!cb->is_static_buf)
    {
        free(cb->buffer);
    }

    if (!cb->is_static_cb)
    {
        free(cb);
    }

    return OSAL_SUCCESS;
}

osal_status_t osal_message_queue_send(osal_message_queue_handle_t queue, const void *item, uint32_t timeout_ms)
{
    osal_queue_control_block_t *cb = (osal_queue_control_block_t *)queue;
    struct timespec ts;
    int ret = 0;

    if (cb == NULL || item == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    pthread_mutex_lock(&cb->mutex);

    while (cb->count >= cb->depth)
    {
        if (timeout_ms == OSAL_NO_WAIT)
        {
            pthread_mutex_unlock(&cb->mutex);
            return OSAL_ERROR_TIMEOUT;
        }
        else if (timeout_ms == OSAL_WAIT_FOREVER)
        {
            pthread_cond_wait(&cb->not_full, &cb->mutex);
        }
        else
        {
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += timeout_ms / 1000;
            ts.tv_nsec += (timeout_ms % 1000) * 1000000;
            if (ts.tv_nsec >= 1000000000)
            {
                ts.tv_sec++;
                ts.tv_nsec -= 1000000000;
            }
            ret = pthread_cond_timedwait(&cb->not_full, &cb->mutex, &ts);
            if (ret == ETIMEDOUT)
            {
                pthread_mutex_unlock(&cb->mutex);
                return OSAL_ERROR_TIMEOUT;
            }
        }
    }

    memcpy(&cb->buffer[cb->tail * cb->item_size], item, cb->item_size);
    cb->tail = (cb->tail + 1) % cb->depth;
    cb->count++;

    pthread_cond_signal(&cb->not_empty);
    pthread_mutex_unlock(&cb->mutex);

    return OSAL_SUCCESS;
}

osal_status_t osal_message_queue_receive(osal_message_queue_handle_t queue, void *item, uint32_t timeout_ms)
{
    osal_queue_control_block_t *cb = (osal_queue_control_block_t *)queue;
    struct timespec ts;
    int ret = 0;

    if (cb == NULL || item == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    pthread_mutex_lock(&cb->mutex);

    while (cb->count == 0)
    {
        if (timeout_ms == OSAL_NO_WAIT)
        {
            pthread_mutex_unlock(&cb->mutex);
            return OSAL_ERROR_TIMEOUT;
        }
        else if (timeout_ms == OSAL_WAIT_FOREVER)
        {
            pthread_cond_wait(&cb->not_empty, &cb->mutex);
        }
        else
        {
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += timeout_ms / 1000;
            ts.tv_nsec += (timeout_ms % 1000) * 1000000;
            if (ts.tv_nsec >= 1000000000)
            {
                ts.tv_sec++;
                ts.tv_nsec -= 1000000000;
            }
            ret = pthread_cond_timedwait(&cb->not_empty, &cb->mutex, &ts);
            if (ret == ETIMEDOUT)
            {
                pthread_mutex_unlock(&cb->mutex);
                return OSAL_ERROR_TIMEOUT;
            }
        }
    }

    memcpy(item, &cb->buffer[cb->head * cb->item_size], cb->item_size);
    cb->head = (cb->head + 1) % cb->depth;
    cb->count--;

    pthread_cond_signal(&cb->not_full);
    pthread_mutex_unlock(&cb->mutex);

    return OSAL_SUCCESS;
}

uint32_t osal_message_queue_get_count(osal_message_queue_handle_t queue)
{
    osal_queue_control_block_t *cb = (osal_queue_control_block_t *)queue;
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

osal_status_t osal_message_queue_reset(osal_message_queue_handle_t queue)
{
    osal_queue_control_block_t *cb = (osal_queue_control_block_t *)queue;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    pthread_mutex_lock(&cb->mutex);
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    pthread_cond_broadcast(&cb->not_full); /* Wake up any senders waiting for space */
    pthread_mutex_unlock(&cb->mutex);

    return OSAL_SUCCESS;
}
