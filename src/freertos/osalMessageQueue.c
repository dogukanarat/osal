/* osalMessageQueue.c - FreeRTOS Message Queue Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osalMessageQueue.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include <stddef.h>

/* Functions */

osalQueueHandle_t osalMessageQueueCreate(uint32_t depth, uint32_t itemSize, const osalMessageQueueAttr_t *attr)
{
    QueueHandle_t handle = NULL;

    if (attr && attr->cbMem && attr->mqMem)
    {
        #if (configSUPPORT_STATIC_ALLOCATION == 1)
        if (attr->cbSize >= sizeof(StaticQueue_t) && attr->mqSize >= (depth * itemSize))
        {
            handle = xQueueCreateStatic(depth, itemSize, (uint8_t *)attr->mqMem, (StaticQueue_t *)attr->cbMem);
        }
        #endif
    }
    else
    {
        #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
        handle = xQueueCreate(depth, itemSize);
        #endif
    }

    return (osalQueueHandle_t)handle;
}

osalStatus_t osalMessageQueueDelete(osalQueueHandle_t queue)
{
    if (queue == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    vQueueDelete((QueueHandle_t)queue);
    return OSAL_SUCCESS;
}

osalStatus_t osalMessageQueueSend(osalQueueHandle_t queue, const void *item, uint32_t timeoutMs)
{
    TickType_t ticks;

    if (queue == NULL || item == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (xPortIsInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (xQueueSendFromISR((QueueHandle_t)queue, item, &xHigherPriorityTaskWoken) == pdTRUE)
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            return OSAL_SUCCESS;
        }
        else
        {
            return OSAL_ERROR_TIMEOUT;
        }
    }
    else
    {
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

        if (xQueueSend((QueueHandle_t)queue, item, ticks) == pdTRUE)
        {
            return OSAL_SUCCESS;
        }
        else
        {
            return OSAL_ERROR_TIMEOUT;
        }
    }
}

osalStatus_t osalMessageQueueReceive(osalQueueHandle_t queue, void *item, uint32_t timeoutMs)
{
    TickType_t ticks;

    if (queue == NULL || item == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (xPortIsInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (xQueueReceiveFromISR((QueueHandle_t)queue, item, &xHigherPriorityTaskWoken) == pdTRUE)
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            return OSAL_SUCCESS;
        }
        else
        {
            return OSAL_ERROR_TIMEOUT;
        }
    }
    else
    {
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

        if (xQueueReceive((QueueHandle_t)queue, item, ticks) == pdTRUE)
        {
            return OSAL_SUCCESS;
        }
        else
        {
            return OSAL_ERROR_TIMEOUT;
        }
    }
}
