/* osalSemaphore.c - FreeRTOS Semaphore Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osalSemaphore.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <stddef.h>

/* Functions */

osalSemaphoreHandle_t osalSemaphoreCreate(const osalSemaphoreAttr_t *attr)
{
    SemaphoreHandle_t handle = NULL;
    uint32_t maxCount = 1;
    uint32_t initialCount = 0;

    if (attr)
    {
        maxCount = attr->maxCount;
        initialCount = attr->initialCount;
    }

    if (attr && attr->cbMem)
    {
        #if (configSUPPORT_STATIC_ALLOCATION == 1)
        if (attr->cbSize >= sizeof(StaticSemaphore_t))
        {
            handle = xSemaphoreCreateCountingStatic(maxCount, initialCount, (StaticSemaphore_t *)attr->cbMem);
        }
        #endif
    }
    else
    {
        #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
        handle = xSemaphoreCreateCounting(maxCount, initialCount);
        #endif
    }

    return (osalSemaphoreHandle_t)handle;
}

osalStatus_t osalSemaphoreDelete(osalSemaphoreHandle_t sem)
{
    if (sem == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    vSemaphoreDelete((SemaphoreHandle_t)sem);
    return OSAL_SUCCESS;
}

osalStatus_t osalSemaphoreTake(osalSemaphoreHandle_t sem, uint32_t timeoutMs)
{
    TickType_t ticks;

    if (sem == NULL)
    {
        return OSAL_ERROR_PARAMETER;
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

    if (xSemaphoreTake((SemaphoreHandle_t)sem, ticks) == pdTRUE)
    {
        return OSAL_SUCCESS;
    }
    else
    {
        return OSAL_ERROR_TIMEOUT;
    }
}

osalStatus_t osalSemaphoreGive(osalSemaphoreHandle_t sem)
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

uint32_t osalSemaphoreGetCount(osalSemaphoreHandle_t sem)
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
