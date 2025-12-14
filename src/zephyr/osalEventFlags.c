/* osalEventFlags.c - Zephyr Event Flags Implementation */

/* All Rights Reserved */

/* Includes */
#include <osal/osalEventFlags.h>
#include <zephyr/kernel.h>
#include <stdlib.h>

/* Internal Types */
typedef struct
{
    struct k_event event;
    bool isStatic;
} osalEventFlagsControlBlock_t;

/* Functions */

osalEventFlagsHandle_t osalEventFlagsCreate(const osalEventFlagsAttr_t *attr)
{
    osalEventFlagsControlBlock_t *cb;

    /* ----------------------------------------------------------------
     * 1. Control Block Allocation
     * ---------------------------------------------------------------- */
    if (attr && attr->cbMem)
    {
        /* Static Allocation */
        if (attr->cbSize < sizeof(osalEventFlagsControlBlock_t))
        {
            return NULL;
        }
        cb = (osalEventFlagsControlBlock_t *)attr->cbMem;
        cb->isStatic = true;
    }
    else
    {
        /* Dynamic Allocation (Requires CONFIG_HEAP_MEM_POOL_SIZE) */
        cb = (osalEventFlagsControlBlock_t *)k_malloc(sizeof(osalEventFlagsControlBlock_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->isStatic = false;
    }

    /* ----------------------------------------------------------------
     * 2. Initialization
     * ---------------------------------------------------------------- */
    k_event_init(&cb->event);

    return (osalEventFlagsHandle_t)cb;
}

osalStatus_t osalEventFlagsDelete(osalEventFlagsHandle_t flags)
{
    osalEventFlagsControlBlock_t *cb = (osalEventFlagsControlBlock_t *)flags;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    /* Zephyr k_event does not need explicit destruction,
     * but we must free the memory if we allocated it.
     */
    if (!cb->isStatic)
    {
        k_free(cb);
    }

    return OSAL_SUCCESS;
}

uint32_t osalEventFlagsSet(osalEventFlagsHandle_t flags, uint32_t flagsToSet)
{
    osalEventFlagsControlBlock_t *cb = (osalEventFlagsControlBlock_t *)flags;
    uint32_t previousFlags;

    if (cb == NULL)
    {
        return 0;
    }

    /* k_event_post performs a logical OR and returns the PREVIOUS value */
    previousFlags = k_event_post(&cb->event, flagsToSet);

    /* Return current value (Previous | Set) to match OSAL API */
    return previousFlags | flagsToSet;
}

uint32_t osalEventFlagsClear(osalEventFlagsHandle_t flags, uint32_t flagsToClear)
{
    osalEventFlagsControlBlock_t *cb = (osalEventFlagsControlBlock_t *)flags;
    uint32_t currentFlags;

    if (cb == NULL)
    {
        return 0;
    }

    /* Get current state before clearing to satisfy return value requirement */
    /* Note: There is a tiny race condition here compared to a mutex-locked
       implementation, but k_event operations are atomic in isolation. */
    currentFlags = k_event_test(&cb->event, 0xFFFFFFFF);

    /* Clear specific bits */
    k_event_clear(&cb->event, flagsToClear);

    return currentFlags;
}

uint32_t osalEventFlagsWait(osalEventFlagsHandle_t flags, uint32_t flagsToWait, uint32_t options, uint32_t timeoutMs)
{
    osalEventFlagsControlBlock_t *cb = (osalEventFlagsControlBlock_t *)flags;
    k_timeout_t timeout;
    uint32_t retFlags;
    bool resetMode;

    if (cb == NULL)
    {
        return 0;
    }

    /* Timeout Mapping */
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

    /* Determine if we should clear (reset) flags after waiting */
    /* OSAL_EVENT_NO_CLEAR (bit set) -> reset = false */
    resetMode = (options & OSAL_EVENT_NO_CLEAR) == 0;

    /* Wait Logic */
    if (options & OSAL_EVENT_WAIT_ALL)
    {
        /* Wait for ALL bits to be set */
        retFlags = k_event_wait_all(&cb->event, flagsToWait, resetMode, timeout);
    }
    else
    {
        /* Wait for ANY bit to be set (Default) */
        retFlags = k_event_wait(&cb->event, flagsToWait, resetMode, timeout);
    }

    return retFlags;
}

uint32_t osalEventFlagsGet(osalEventFlagsHandle_t flags)
{
    osalEventFlagsControlBlock_t *cb = (osalEventFlagsControlBlock_t *)flags;

    if (cb == NULL)
    {
        return 0;
    }

    /* k_event_test checks the state without modifying it */
    return k_event_test(&cb->event, 0xFFFFFFFF);
}
