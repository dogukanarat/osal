/* osalSemaphore.c - POSIX Semaphore Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osalSemaphore.h"
#include <semaphore.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

/* Internal Types */
typedef struct
{
    sem_t sem;
    bool isStatic;
} osalSemaphoreControlBlock_t;

/* Functions */

osalSemaphoreHandle_t osalSemaphoreCreate(const osalSemaphoreAttr_t *attr)
{
    osalSemaphoreControlBlock_t *cb;
    uint32_t initialCount = 0;
    int ret;

    if (attr)
    {
        initialCount = attr->initialCount;
    }

    /* Allocate control block */
    if (attr && attr->cbMem)
    {
        if (attr->cbSize < sizeof(osalSemaphoreControlBlock_t))
        {
            return NULL;
        }
        cb = (osalSemaphoreControlBlock_t *)attr->cbMem;
        cb->isStatic = true;
    }
    else
    {
        cb = (osalSemaphoreControlBlock_t *)malloc(sizeof(osalSemaphoreControlBlock_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->isStatic = false;
    }

    /* Initialize semaphore */
    ret = sem_init(&cb->sem, 0, initialCount);

    if (ret != 0)
    {
        if (!cb->isStatic)
        {
            free(cb);
        }
        return NULL;
    }

    return (osalSemaphoreHandle_t)cb;
}

osalStatus_t osalSemaphoreDelete(osalSemaphoreHandle_t sem)
{
    osalSemaphoreControlBlock_t *cb = (osalSemaphoreControlBlock_t *)sem;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    sem_destroy(&cb->sem);

    if (!cb->isStatic)
    {
        free(cb);
    }

    return OSAL_SUCCESS;
}

osalStatus_t osalSemaphoreTake(osalSemaphoreHandle_t sem, uint32_t timeoutMs)
{
    osalSemaphoreControlBlock_t *cb = (osalSemaphoreControlBlock_t *)sem;
    struct timespec ts;
    int ret;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (timeoutMs == OSAL_WAIT_FOREVER)
    {
        ret = sem_wait(&cb->sem);
    }
    else if (timeoutMs == OSAL_NO_WAIT)
    {
        ret = sem_trywait(&cb->sem);
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
        ret = sem_timedwait(&cb->sem, &ts);
    }

    if (ret == 0)
    {
        return OSAL_SUCCESS;
    }
    else if (errno == ETIMEDOUT || errno == EAGAIN)
    {
        return OSAL_ERROR_TIMEOUT;
    }
    else
    {
        return OSAL_ERROR;
    }
}

osalStatus_t osalSemaphoreGive(osalSemaphoreHandle_t sem)
{
    osalSemaphoreControlBlock_t *cb = (osalSemaphoreControlBlock_t *)sem;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (sem_post(&cb->sem) != 0)
    {
        return OSAL_ERROR;
    }

    return OSAL_SUCCESS;
}

uint32_t osalSemaphoreGetCount(osalSemaphoreHandle_t sem)
{
    osalSemaphoreControlBlock_t *cb = (osalSemaphoreControlBlock_t *)sem;
    int val;

    if (cb == NULL)
    {
        return 0;
    }

    if (sem_getvalue(&cb->sem, &val) != 0)
    {
        return 0;
    }

    return (uint32_t)val;
}
