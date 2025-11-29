/* osalEventFlags.c - POSIX Event Flags Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osalEventFlags.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

/* Internal Types */
typedef struct
{
    uint32_t flags;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    bool isStatic;
} osalEventFlagsControlBlock_t;

/* Functions */

osalEventFlagsHandle_t osalEventFlagsCreate(const osalEventFlagsAttr_t *attr)
{
    osalEventFlagsControlBlock_t *cb;

    /* Allocate control block */
    if (attr && attr->cbMem)
    {
        if (attr->cbSize < sizeof(osalEventFlagsControlBlock_t))
        {
            return NULL;
        }
        cb = (osalEventFlagsControlBlock_t *)attr->cbMem;
        cb->isStatic = true;
    }
    else
    {
        cb = (osalEventFlagsControlBlock_t *)malloc(sizeof(osalEventFlagsControlBlock_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->isStatic = false;
    }

    cb->flags = 0;
    pthread_mutex_init(&cb->mutex, NULL);
    pthread_cond_init(&cb->cond, NULL);

    return (osalEventFlagsHandle_t)cb;
}

osalStatus_t osalEventFlagsDelete(osalEventFlagsHandle_t flags)
{
    osalEventFlagsControlBlock_t *cb = (osalEventFlagsControlBlock_t *)flags;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    pthread_mutex_destroy(&cb->mutex);
    pthread_cond_destroy(&cb->cond);

    if (!cb->isStatic)
    {
        free(cb);
    }

    return OSAL_SUCCESS;
}

uint32_t osalEventFlagsSet(osalEventFlagsHandle_t flags, uint32_t flagsToSet)
{
    osalEventFlagsControlBlock_t *cb = (osalEventFlagsControlBlock_t *)flags;
    uint32_t currentFlags;

    if (cb == NULL)
    {
        return 0;
    }

    pthread_mutex_lock(&cb->mutex);
    cb->flags |= flagsToSet;
    currentFlags = cb->flags;
    pthread_cond_broadcast(&cb->cond); /* Wake up all waiters to check their conditions */
    pthread_mutex_unlock(&cb->mutex);

    return currentFlags;
}

uint32_t osalEventFlagsClear(osalEventFlagsHandle_t flags, uint32_t flagsToClear)
{
    osalEventFlagsControlBlock_t *cb = (osalEventFlagsControlBlock_t *)flags;
    uint32_t currentFlags;

    if (cb == NULL)
    {
        return 0;
    }

    pthread_mutex_lock(&cb->mutex);
    currentFlags = cb->flags;
    cb->flags &= ~flagsToClear;
    pthread_mutex_unlock(&cb->mutex);

    return currentFlags;
}

uint32_t osalEventFlagsWait(osalEventFlagsHandle_t flags, uint32_t flagsToWait, uint32_t options, uint32_t timeoutMs)
{
    osalEventFlagsControlBlock_t *cb = (osalEventFlagsControlBlock_t *)flags;
    struct timespec ts;
    uint32_t rflags = 0;
    int ret = 0;
    bool waitAll = (options & OSAL_EVENT_WAIT_ALL) != 0;
    bool clear = (options & OSAL_EVENT_NO_CLEAR) == 0;

    if (cb == NULL)
    {
        return 0;
    }

    pthread_mutex_lock(&cb->mutex);

    while (1)
    {
        uint32_t currentFlags = cb->flags & flagsToWait;
        bool conditionMet = false;

        if (waitAll)
        {
            if (currentFlags == flagsToWait)
            {
                conditionMet = true;
            }
        }
        else
        {
            if (currentFlags != 0)
            {
                conditionMet = true;
            }
        }

        if (conditionMet)
        {
            rflags = currentFlags;
            if (clear)
            {
                cb->flags &= ~currentFlags;
            }
            break;
        }

        if (timeoutMs == OSAL_NO_WAIT)
        {
            ret = ETIMEDOUT;
            break;
        }
        else if (timeoutMs == OSAL_WAIT_FOREVER)
        {
            pthread_cond_wait(&cb->cond, &cb->mutex);
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
            ret = pthread_cond_timedwait(&cb->cond, &cb->mutex, &ts);
            if (ret == ETIMEDOUT)
            {
                break;
            }
        }
    }

    pthread_mutex_unlock(&cb->mutex);

    if (ret == ETIMEDOUT)
    {
        return 0; /* Or error code, but standard typically returns 0 flags */
    }

    return rflags;
}

uint32_t osalEventFlagsGet(osalEventFlagsHandle_t flags)
{
    osalEventFlagsControlBlock_t *cb = (osalEventFlagsControlBlock_t *)flags;
    uint32_t currentFlags;

    if (cb == NULL)
    {
        return 0;
    }

    pthread_mutex_lock(&cb->mutex);
    currentFlags = cb->flags;
    pthread_mutex_unlock(&cb->mutex);

    return currentFlags;
}
