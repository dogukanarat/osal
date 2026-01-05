/* osalEventFlags.c - FreeRTOS Event Flags Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_event_flags.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include <stddef.h>

/* Functions */

osal_event_flags_handle_t osal_event_flags_create(const osal_event_flags_attr_t *attr)
{
    EventGroupHandle_t handle = NULL;

    if (attr && attr->cb_mem)
    {
        #if (configSUPPORT_STATIC_ALLOCATION == 1)
        if (attr->cb_size >= sizeof(StaticEventGroup_t))
        {
            handle = xEventGroupCreateStatic((StaticEventGroup_t *)attr->cb_mem);
        }
        #endif
    }
    else
    {
        #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
        handle = xEventGroupCreate();
        #endif
    }

    return (osal_event_flags_handle_t)handle;
}

osal_status_t osal_event_flags_delete(osal_event_flags_handle_t flags)
{
    if (flags == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    vEventGroupDelete((EventGroupHandle_t)flags);
    return OSAL_SUCCESS;
}

uint32_t osal_event_flags_set(osal_event_flags_handle_t flags, uint32_t flags_to_set)
{
    if (flags == NULL)
    {
        return 0;
    }

    if (xPortIsInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        BaseType_t result;
        
        result = xEventGroupSetBitsFromISR((EventGroupHandle_t)flags, (EventBits_t)flags_to_set, &xHigherPriorityTaskWoken);
        
        if (result == pdPASS)
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
        
        /* Note: FromISR set doesn't return the new flags immediately. Returning 0 as best effort. */
        return 0; 
    }
    else
    {
        /* Note: xEventGroupSetBits returns the value of the event group at the time 
           the call returned. */
        return (uint32_t)xEventGroupSetBits((EventGroupHandle_t)flags, (EventBits_t)flags_to_set);
    }
}

uint32_t osal_event_flags_clear(osal_event_flags_handle_t flags, uint32_t flags_to_clear)
{
    if (flags == NULL)
    {
        return 0;
    }

    if (xPortIsInsideInterrupt())
    {
        /* Clear from ISR returns the value of the event group BEFORE the bits were cleared */
        return (uint32_t)xEventGroupClearBitsFromISR((EventGroupHandle_t)flags, (EventBits_t)flags_to_clear);
    }
    else
    {
        return (uint32_t)xEventGroupClearBits((EventGroupHandle_t)flags, (EventBits_t)flags_to_clear);
    }
}

uint32_t osal_event_flags_wait(osal_event_flags_handle_t flags, uint32_t flags_to_wait, uint32_t options, uint32_t timeout_ms)
{
    TickType_t ticks;
    BaseType_t waitForAllBits = (options & OSAL_EVENT_WAIT_ALL) ? pdTRUE : pdFALSE;
    BaseType_t clearOnExit = (options & OSAL_EVENT_NO_CLEAR) ? pdFALSE : pdTRUE;

    if (flags == NULL)
    {
        return 0;
    }

    /* Cannot wait from ISR */
    if (xPortIsInsideInterrupt())
    {
        return 0;
    }

    if (timeout_ms == OSAL_WAIT_FOREVER)
    {
        ticks = portMAX_DELAY;
    }
    else if (timeout_ms == OSAL_NO_WAIT)
    {
        ticks = 0;
    }
    else
    {
        ticks = pdMS_TO_TICKS(timeout_ms);
    }

    return (uint32_t)xEventGroupWaitBits((EventGroupHandle_t)flags, (EventBits_t)flags_to_wait, clearOnExit, waitForAllBits, ticks);
}

uint32_t osal_event_flags_get(osal_event_flags_handle_t flags)
{
    if (flags == NULL)
    {
        return 0;
    }

    if (xPortIsInsideInterrupt())
    {
        return (uint32_t)xEventGroupGetBitsFromISR((EventGroupHandle_t)flags);
    }
    else
    {
        return (uint32_t)xEventGroupGetBits((EventGroupHandle_t)flags);
    }
}
