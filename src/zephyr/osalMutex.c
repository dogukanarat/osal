/* osalMutex.c - Zephyr Mutex Implementation */

/* All Rights Reserved */

/* Includes */
#include <osal/osalMutex.h>
#include <zephyr/kernel.h>
#include <stdlib.h>

/* Internal Types */
typedef struct
{
    struct k_mutex mutex;
    bool isStatic;
} osalMutexControlBlock_t;

/* Functions */

osalMutexHandle_t osalMutexCreate(const osalMutexAttr_t *attr)
{
    osalMutexControlBlock_t *cb;

    /* ----------------------------------------------------------------
     * 1. Control Block Allocation
     * ---------------------------------------------------------------- */
    if (attr && attr->cbMem)
    {
        /* Static Allocation */
        if (attr->cbSize < sizeof(osalMutexControlBlock_t))
        {
            return NULL; /* Provided buffer too small */
        }
        cb = (osalMutexControlBlock_t *)attr->cbMem;
        cb->isStatic = true;
    }
    else
    {
        /* Dynamic Allocation (Requires CONFIG_HEAP_MEM_POOL_SIZE) */
        cb = (osalMutexControlBlock_t *)k_malloc(sizeof(osalMutexControlBlock_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->isStatic = false;
    }

    /* ----------------------------------------------------------------
     * 2. Initialization
     * ---------------------------------------------------------------- */
    /* Zephyr k_mutex is recursive by default (reentrant).
     * It handles the OSAL_MUTEX_RECURSIVE requirement natively.
     */
    k_mutex_init(&cb->mutex);

    return (osalMutexHandle_t)cb;
}

osalStatus_t osalMutexDelete(osalMutexHandle_t mutex)
{
    osalMutexControlBlock_t *cb = (osalMutexControlBlock_t *)mutex;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    /* Zephyr mutexes do not need explicit destruction,
     * but we must free the memory if we allocated it.
     */
    if (!cb->isStatic)
    {
        k_free(cb);
    }

    return OSAL_SUCCESS;
}

osalStatus_t osalMutexLock(osalMutexHandle_t mutex, uint32_t timeoutMs)
{
    osalMutexControlBlock_t *cb = (osalMutexControlBlock_t *)mutex;
    k_timeout_t timeout;
    int ret;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    /* Map OSAL timeout to Zephyr timeout */
    if (timeoutMs == OSAL_WAIT_FOREVER)
    {
        timeout = K_FOREVER;
    }
    else if (timeoutMs == OSAL_NO_WAIT)
    {
        timeout = K_NO_WAIT;
    }
    else
    {
        timeout = K_MSEC(timeoutMs);
    }

    /* Attempt to lock */
    ret = k_mutex_lock(&cb->mutex, timeout);

    if (ret == 0)
    {
        return OSAL_SUCCESS;
    }
    else if (ret == -EAGAIN || ret == -EBUSY)
    {
        return OSAL_ERROR_TIMEOUT;
    }
    else
    {
        /* Other errors (e.g. general failure) */
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

    /* Unlock the mutex.
     * k_mutex_unlock returns 0 on success, or non-zero if caller
     * doesn't own the mutex or if the count is already 0.
     */
    if (k_mutex_unlock(&cb->mutex) != 0)
    {
        return OSAL_ERROR;
    }

    return OSAL_SUCCESS;
}
