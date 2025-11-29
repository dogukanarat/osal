/* osalEventFlags.c - FreeRTOS Event Flags Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osalEventFlags.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include <stddef.h>

/* Functions */

osalEventFlagsHandle_t osalEventFlagsCreate(const osalEventFlagsAttr_t *attr)
{
    EventGroupHandle_t handle = NULL;

    if (attr && attr->cbMem)
    {
        #if (configSUPPORT_STATIC_ALLOCATION == 1)
        if (attr->cbSize >= sizeof(StaticEventGroup_t))
        {
            handle = xEventGroupCreateStatic((StaticEventGroup_t *)attr->cbMem);
        }
        #endif
    }
    else
    {
        #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
        handle = xEventGroupCreate();
        #endif
    }

    return (osalEventFlagsHandle_t)handle;
}

osalStatus_t osalEventFlagsDelete(osalEventFlagsHandle_t flags)
{
    if (flags == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    vEventGroupDelete((EventGroupHandle_t)flags);
    return OSAL_SUCCESS;
}

uint32_t osalEventFlagsSet(osalEventFlagsHandle_t flags, uint32_t flagsToSet)
{
    if (flags == NULL)
    {
        return 0;
    }

    if (xPortIsInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        BaseType_t result;
        
        result = xEventGroupSetBitsFromISR((EventGroupHandle_t)flags, (EventBits_t)flagsToSet, &xHigherPriorityTaskWoken);
        
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
        return (uint32_t)xEventGroupSetBits((EventGroupHandle_t)flags, (EventBits_t)flagsToSet);
    }
}

uint32_t osalEventFlagsClear(osalEventFlagsHandle_t flags, uint32_t flagsToClear)
{
    if (flags == NULL)
    {
        return 0;
    }

    if (xPortIsInsideInterrupt())
    {
        /* Clear from ISR returns the value of the event group BEFORE the bits were cleared */
        return (uint32_t)xEventGroupClearBitsFromISR((EventGroupHandle_t)flags, (EventBits_t)flagsToClear);
    }
    else
    {
        return (uint32_t)xEventGroupClearBits((EventGroupHandle_t)flags, (EventBits_t)flagsToClear);
    }
}

uint32_t osalEventFlagsWait(osalEventFlagsHandle_t flags, uint32_t flagsToWait, uint32_t options, uint32_t timeoutMs)
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

    if (timeoutMs == OSAL_WAIT_FOREVER)
    {
        ticks = portMAX_DELAY;
    }
    else if (timeoutMs == OSAL_NO_WAIT)
    {
        ticks = 0;
    }
    else
    {
        ticks = pdMS_TO_TICKS(timeoutMs);
    }

    return (uint32_t)xEventGroupWaitBits((EventGroupHandle_t)flags, (EventBits_t)flagsToWait, clearOnExit, waitForAllBits, ticks);
}

uint32_t osalEventFlagsGet(osalEventFlagsHandle_t flags)
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
