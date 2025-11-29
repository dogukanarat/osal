/* osalMutex.c - FreeRTOS Mutex Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osalMutex.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <stddef.h>

/* Functions */

osalMutexHandle_t osalMutexCreate(const osalMutexAttr_t *attr)
{
    SemaphoreHandle_t handle = NULL;

    if (attr && attr->cbMem)
    {
        #if (configSUPPORT_STATIC_ALLOCATION == 1)
        if (attr->cbSize >= sizeof(StaticSemaphore_t))
        {
            handle = xSemaphoreCreateRecursiveMutexStatic((StaticSemaphore_t *)attr->cbMem);
        }
        #endif
    }
    else
    {
        #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
        handle = xSemaphoreCreateRecursiveMutex();
        #endif
    }

    return (osalMutexHandle_t)handle;
}

osalStatus_t osalMutexDelete(osalMutexHandle_t mutex)
{
    if (mutex == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    vSemaphoreDelete((SemaphoreHandle_t)mutex);
    return OSAL_SUCCESS;
}

osalStatus_t osalMutexLock(osalMutexHandle_t mutex, uint32_t timeoutMs)
{
    TickType_t ticks;

    if (mutex == NULL)
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

    if (xSemaphoreTakeRecursive((SemaphoreHandle_t)mutex, ticks) == pdTRUE)
    {
        return OSAL_SUCCESS;
    }
    else
    {
        return OSAL_ERROR_TIMEOUT;
    }
}

osalStatus_t osalMutexUnlock(osalMutexHandle_t mutex)
{
    if (mutex == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (xSemaphoreGiveRecursive((SemaphoreHandle_t)mutex) == pdTRUE)
    {
        return OSAL_SUCCESS;
    }
    else
    {
        return OSAL_ERROR;
    }
}
