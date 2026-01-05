/* osalSemaphore.c - FreeRTOS Semaphore Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_semaphore.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <stddef.h>

/* Functions */

osal_semaphore_handle_t osal_semaphore_create(const osal_semaphore_attr_t *attr)
{
    SemaphoreHandle_t handle = NULL;
    uint32_t max_count = 1;
    uint32_t initial_count = 0;

    if (attr)
    {
        max_count = attr->max_count;
        initial_count = attr->initial_count;
    }

    if (attr && attr->cb_mem)
    {
        #if (configSUPPORT_STATIC_ALLOCATION == 1)
        if (attr->cb_size >= sizeof(StaticSemaphore_t))
        {
            handle = xSemaphoreCreateCountingStatic(max_count, initial_count, (StaticSemaphore_t *)attr->cb_mem);
        }
        #endif
    }
    else
    {
        #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
        handle = xSemaphoreCreateCounting(max_count, initial_count);
        #endif
    }

    return (osal_semaphore_handle_t)handle;
}

osal_status_t osal_semaphore_delete(osal_semaphore_handle_t sem)
{
    if (sem == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    vSemaphoreDelete((SemaphoreHandle_t)sem);
    return OSAL_SUCCESS;
}

osal_status_t osal_semaphore_take(osal_semaphore_handle_t sem, uint32_t timeout_ms)
{
    TickType_t ticks;

    if (sem == NULL)
    {
        return OSAL_ERROR_PARAMETER;
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

    if (xSemaphoreTake((SemaphoreHandle_t)sem, ticks) == pdTRUE)
    {
        return OSAL_SUCCESS;
    }
    else
    {
        return OSAL_ERROR_TIMEOUT;
    }
}

osal_status_t osal_semaphore_give(osal_semaphore_handle_t sem)
{
    if (sem == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (xPortIsInsideInterrupt())
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (xSemaphoreGiveFromISR((SemaphoreHandle_t)sem, &xHigherPriorityTaskWoken) == pdTRUE)
        {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            return OSAL_SUCCESS;
        }
        else
        {
            return OSAL_ERROR;
        }
    }
    else
    {
        if (xSemaphoreGive((SemaphoreHandle_t)sem) == pdTRUE)
        {
            return OSAL_SUCCESS;
        }
        else
        {
            return OSAL_ERROR;
        }
    }
}

uint32_t osal_semaphore_get_count(osal_semaphore_handle_t sem)
{
    if (sem == NULL)
    {
        return 0;
    }

    if (xPortIsInsideInterrupt())
    {
         return (uint32_t)uxSemaphoreGetCountFromISR((SemaphoreHandle_t)sem);
    }
    else
    {
        return (uint32_t)uxSemaphoreGetCount((SemaphoreHandle_t)sem);
    }
}
