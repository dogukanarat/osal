/* osalMessageQueue.c - FreeRTOS Message Queue Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_message_queue.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include <stddef.h>

/* Functions */

osalQueueHandle_t osal_message_queue_create(uint32_t depth, uint32_t item_size, const osal_message_queue_attr_t *attr)
{
    QueueHandle_t handle = NULL;

    if (attr && attr->cb_mem && attr->mq_mem)
    {
        #if (configSUPPORT_STATIC_ALLOCATION == 1)
        if (attr->cb_size >= sizeof(StaticQueue_t) && attr->mq_size >= (depth * item_size))
        {
            handle = xQueueCreateStatic(depth, item_size, (uint8_t *)attr->mq_mem, (StaticQueue_t *)attr->cb_mem);
        }
        #endif
    }
    else
    {
        #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
        handle = xQueueCreate(depth, item_size);
        #endif
    }

    return (osalQueueHandle_t)handle;
}

osal_status_t osal_message_queue_delete(osalQueueHandle_t queue)
{
    if (queue == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    vQueueDelete((QueueHandle_t)queue);
    return OSAL_SUCCESS;
}

osal_status_t osal_message_queue_send(osalQueueHandle_t queue, const void *item, uint32_t timeout_ms)
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

osal_status_t osal_message_queue_receive(osalQueueHandle_t queue, void *item, uint32_t timeout_ms)
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
