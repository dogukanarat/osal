/* osalMutex.c - POSIX Mutex Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osalMutex.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

/* Internal Types */
typedef struct
{
    pthread_mutex_t mutex;
    bool isStatic;
} osalMutexControlBlock_t;

/* Functions */

osalMutexHandle_t osalMutexCreate(const osalMutexAttr_t *attr)
{
    osalMutexControlBlock_t *cb;
    pthread_mutexattr_t mutexAttr;
    int ret;

    /* Allocate control block */
    if (attr && attr->cbMem)
    {
        if (attr->cbSize < sizeof(osalMutexControlBlock_t))
        {
            return NULL;
        }
        cb = (osalMutexControlBlock_t *)attr->cbMem;
        cb->isStatic = true;
    }
    else
    {
        cb = (osalMutexControlBlock_t *)malloc(sizeof(osalMutexControlBlock_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->isStatic = false;
    }

    /* Initialize mutex attributes */
    pthread_mutexattr_init(&mutexAttr);
    
    /* Handle attributes */
    if (attr && (attr->attrBits & OSAL_MUTEX_RECURSIVE))
    {
        pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE);
    }
    else
    {
        /* Default to recursive for general robustness if not specified, 
           or normal if strictly following bits. Let's follow bits but 
           note that many OSALs default to recursive. */
        pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_NORMAL);
    }

    /* Initialize mutex */
    ret = pthread_mutex_init(&cb->mutex, &mutexAttr);
    pthread_mutexattr_destroy(&mutexAttr);

    if (ret != 0)
    {
        if (!cb->isStatic)
        {
            free(cb);
        }
        return NULL;
    }

    return (osalMutexHandle_t)cb;
}

osalStatus_t osalMutexDelete(osalMutexHandle_t mutex)
{
    osalMutexControlBlock_t *cb = (osalMutexControlBlock_t *)mutex;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    pthread_mutex_destroy(&cb->mutex);

    if (!cb->isStatic)
    {
        free(cb);
    }

    return OSAL_SUCCESS;
}

osalStatus_t osalMutexLock(osalMutexHandle_t mutex, uint32_t timeoutMs)
{
    osalMutexControlBlock_t *cb = (osalMutexControlBlock_t *)mutex;
    struct timespec ts;
    int ret;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (timeoutMs == OSAL_WAIT_FOREVER)
    {
        ret = pthread_mutex_lock(&cb->mutex);
    }
    else if (timeoutMs == OSAL_NO_WAIT)
    {
        ret = pthread_mutex_trylock(&cb->mutex);
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
        ret = pthread_mutex_timedlock(&cb->mutex, &ts);
    }

    if (ret == 0)
    {
        return OSAL_SUCCESS;
    }
    else if (ret == ETIMEDOUT || ret == EBUSY)
    {
        return OSAL_ERROR_TIMEOUT;
    }
    else
    {
        return OSAL_ERROR;
    }
}

osalStatus_t osalMutexUnlock(osalMutexHandle_t mutex)
{
    osalMutexControlBlock_t *cb = (osalMutexControlBlock_t *)mutex;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (pthread_mutex_unlock(&cb->mutex) != 0)
    {
        return OSAL_ERROR;
    }

    return OSAL_SUCCESS;
}
