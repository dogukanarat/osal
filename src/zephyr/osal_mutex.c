/* osalMutex.c - Zephyr Mutex Implementation */

/* All Rights Reserved */

/* Includes */
#include <osal/osal_mutex.h>
#include <zephyr/kernel.h>
#include <stdlib.h>

/* Internal Types */
typedef struct
{
    struct k_mutex mutex;
    bool is_static;
} osal_mutex_control_block_t;

/* Functions */

osal_mutex_handle_t osal_mutex_create(const osal_mutex_attr_t *attr)
{
    osal_mutex_control_block_t *cb;

    /* ----------------------------------------------------------------
     * 1. Control Block Allocation
     * ---------------------------------------------------------------- */
    if (attr && attr->cb_mem)
    {
        /* Static Allocation */
        if (attr->cb_size < sizeof(osal_mutex_control_block_t))
        {
            return NULL; /* Provided buffer too small */
        }
        cb = (osal_mutex_control_block_t *)attr->cb_mem;
        cb->is_static = true;
    }
    else
    {
        /* Dynamic Allocation (Requires CONFIG_HEAP_MEM_POOL_SIZE) */
        cb = (osal_mutex_control_block_t *)k_malloc(sizeof(osal_mutex_control_block_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->is_static = false;
    }

    /* ----------------------------------------------------------------
     * 2. Initialization
     * ---------------------------------------------------------------- */
    /* Zephyr k_mutex is recursive by default (reentrant).
     * It handles the OSAL_MUTEX_RECURSIVE requirement natively.
     */
    k_mutex_init(&cb->mutex);

    return (osal_mutex_handle_t)cb;
}

osal_status_t osal_mutex_delete(osal_mutex_handle_t mutex)
{
    osal_mutex_control_block_t *cb = (osal_mutex_control_block_t *)mutex;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    /* Zephyr mutexes do not need explicit destruction,
     * but we must free the memory if we allocated it.
     */
    if (!cb->is_static)
    {
        k_free(cb);
    }

    return OSAL_SUCCESS;
}

osal_status_t osal_mutex_lock(osal_mutex_handle_t mutex, uint32_t timeout_ms)
{
    osal_mutex_control_block_t *cb = (osal_mutex_control_block_t *)mutex;
    k_timeout_t timeout;
    int ret;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    /* Map OSAL timeout to Zephyr timeout */
    if (timeout_ms == OSAL_WAIT_FOREVER)
    {
        timeout = K_FOREVER;
    }
    else if (timeout_ms == OSAL_NO_WAIT)
    {
        timeout = K_NO_WAIT;
    }
    else
    {
        timeout = K_MSEC(timeout_ms);
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

osal_status_t osal_mutex_unlock(osal_mutex_handle_t mutex)
{
    osal_mutex_control_block_t *cb = (osal_mutex_control_block_t *)mutex;

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
