/* osalSemaphore.c - macOS Semaphore Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_semaphore.h"
#include <dispatch/dispatch.h>
#include <stdlib.h>
#include <stdatomic.h>

/* Internal Types */
typedef struct
{
    dispatch_semaphore_t sem;
    atomic_uint count;
    bool is_static;
} osal_semaphore_control_block_t;

/* Functions */

osal_semaphore_handle_t osal_semaphore_create(const osal_semaphore_attr_t *attr)
{
    osal_semaphore_control_block_t *cb;
    uint32_t initial_count = 0;

    if (attr)
    {
        initial_count = attr->initial_count;
    }

    /* Allocate control block */
    if (attr && attr->cb_mem)
    {
        if (attr->cb_size < sizeof(osal_semaphore_control_block_t))
        {
            return NULL;
        }
        cb = (osal_semaphore_control_block_t *)attr->cb_mem;
        cb->is_static = true;
    }
    else
    {
        cb = (osal_semaphore_control_block_t *)malloc(sizeof(osal_semaphore_control_block_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->is_static = false;
    }

    /* Create GCD dispatch semaphore */
    cb->sem = dispatch_semaphore_create(initial_count);
    if (cb->sem == NULL)
    {
        if (!cb->is_static)
        {
            free(cb);
        }
        return NULL;
    }

    atomic_store(&cb->count, initial_count);

    return (osal_semaphore_handle_t)cb;
}

osal_status_t osal_semaphore_delete(osal_semaphore_handle_t sem)
{
    osal_semaphore_control_block_t *cb = (osal_semaphore_control_block_t *)sem;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (cb->sem != NULL)
    {
        dispatch_release(cb->sem);
    }

    if (!cb->is_static)
    {
        free(cb);
    }

    return OSAL_SUCCESS;
}

osal_status_t osal_semaphore_take(osal_semaphore_handle_t sem, uint32_t timeout_ms)
{
    osal_semaphore_control_block_t *cb = (osal_semaphore_control_block_t *)sem;
    dispatch_time_t timeout;
    long result;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (timeout_ms == OSAL_WAIT_FOREVER)
    {
        timeout = DISPATCH_TIME_FOREVER;
    }
    else if (timeout_ms == OSAL_NO_WAIT)
    {
        timeout = DISPATCH_TIME_NOW;
    }
    else
    {
        timeout = dispatch_time(DISPATCH_TIME_NOW, (int64_t)timeout_ms * 1000000);
    }

    result = dispatch_semaphore_wait(cb->sem, timeout);

    if (result == 0)
    {
        atomic_fetch_sub(&cb->count, 1);
        return OSAL_SUCCESS;
    }
    else
    {
        return OSAL_ERROR_TIMEOUT;
    }
}

osal_status_t osal_semaphore_give(osal_semaphore_handle_t sem)
{
    osal_semaphore_control_block_t *cb = (osal_semaphore_control_block_t *)sem;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    dispatch_semaphore_signal(cb->sem);
    atomic_fetch_add(&cb->count, 1);

    return OSAL_SUCCESS;
}

uint32_t osal_semaphore_get_count(osal_semaphore_handle_t sem)
{
    osal_semaphore_control_block_t *cb = (osal_semaphore_control_block_t *)sem;

    if (cb == NULL)
    {
        return 0;
    }

    return atomic_load(&cb->count);
}
