/* osalEventFlags.c - Zephyr Event Flags Implementation */

/* All Rights Reserved */

/* Includes */
#include <osal/osal_event_flags.h>
#include <zephyr/kernel.h>
#include <stdlib.h>

/* Internal Types */
typedef struct
{
    struct k_event event;
    bool is_static;
} osal_event_flags_control_block_t;

/* Functions */

osal_event_flags_handle_t osal_event_flags_create(const osal_event_flags_attr_t *attr)
{
    osal_event_flags_control_block_t *cb;

    /* ----------------------------------------------------------------
     * 1. Control Block Allocation
     * ---------------------------------------------------------------- */
    if (attr && attr->cb_mem)
    {
        /* Static Allocation */
        if (attr->cb_size < sizeof(osal_event_flags_control_block_t))
        {
            return NULL;
        }
        cb = (osal_event_flags_control_block_t *)attr->cb_mem;
        cb->is_static = true;
    }
    else
    {
        /* Dynamic Allocation (Requires CONFIG_HEAP_MEM_POOL_SIZE) */
        cb = (osal_event_flags_control_block_t *)k_malloc(sizeof(osal_event_flags_control_block_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->is_static = false;
    }

    /* ----------------------------------------------------------------
     * 2. Initialization
     * ---------------------------------------------------------------- */
    k_event_init(&cb->event);

    return (osal_event_flags_handle_t)cb;
}

osal_status_t osal_event_flags_delete(osal_event_flags_handle_t flags)
{
    osal_event_flags_control_block_t *cb = (osal_event_flags_control_block_t *)flags;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    /* Zephyr k_event does not need explicit destruction,
     * but we must free the memory if we allocated it.
     */
    if (!cb->is_static)
    {
        k_free(cb);
    }

    return OSAL_SUCCESS;
}

uint32_t osal_event_flags_set(osal_event_flags_handle_t flags, uint32_t flags_to_set)
{
    osal_event_flags_control_block_t *cb = (osal_event_flags_control_block_t *)flags;
    uint32_t previousFlags;

    if (cb == NULL)
    {
        return 0;
    }

    /* k_event_post performs a logical OR and returns the PREVIOUS value */
    previousFlags = k_event_post(&cb->event, flags_to_set);

    /* Return current value (Previous | Set) to match OSAL API */
    return previousFlags | flags_to_set;
}

uint32_t osal_event_flags_clear(osal_event_flags_handle_t flags, uint32_t flags_to_clear)
{
    osal_event_flags_control_block_t *cb = (osal_event_flags_control_block_t *)flags;
    uint32_t current_flags;

    if (cb == NULL)
    {
        return 0;
    }

    /* Get current state before clearing to satisfy return value requirement */
    /* Note: There is a tiny race condition here compared to a mutex-locked
       implementation, but k_event operations are atomic in isolation. */
    current_flags = k_event_test(&cb->event, 0xFFFFFFFF);

    /* Clear specific bits */
    k_event_clear(&cb->event, flags_to_clear);

    return current_flags;
}

uint32_t osal_event_flags_wait(osal_event_flags_handle_t flags, uint32_t flags_to_wait, uint32_t options, uint32_t timeout_ms)
{
    osal_event_flags_control_block_t *cb = (osal_event_flags_control_block_t *)flags;
    k_timeout_t timeout;
    uint32_t retFlags;
    bool resetMode;

    if (cb == NULL)
    {
        return 0;
    }

    /* Timeout Mapping */
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

    /* Determine if we should clear (reset) flags after waiting */
    /* OSAL_EVENT_NO_CLEAR (bit set) -> reset = false */
    resetMode = (options & OSAL_EVENT_NO_CLEAR) == 0;

    /* Wait Logic */
    if (options & OSAL_EVENT_WAIT_ALL)
    {
        /* Wait for ALL bits to be set */
        retFlags = k_event_wait_all(&cb->event, flags_to_wait, resetMode, timeout);
    }
    else
    {
        /* Wait for ANY bit to be set (Default) */
        retFlags = k_event_wait(&cb->event, flags_to_wait, resetMode, timeout);
    }

    return retFlags;
}

uint32_t osal_event_flags_get(osal_event_flags_handle_t flags)
{
    osal_event_flags_control_block_t *cb = (osal_event_flags_control_block_t *)flags;

    if (cb == NULL)
    {
        return 0;
    }

    /* k_event_test checks the state without modifying it */
    return k_event_test(&cb->event, 0xFFFFFFFF);
}
