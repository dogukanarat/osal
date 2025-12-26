/* osalMutex.c - FreeRTOS Mutex Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_mutex.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <stddef.h>

/* Functions */

osal_mutex_handle_t osal_mutex_create(const osal_mutex_attr_t *attr)
{
    SemaphoreHandle_t handle = NULL;

    if (attr && attr->cb_mem)
    {
        #if (configSUPPORT_STATIC_ALLOCATION == 1)
        if (attr->cb_size >= sizeof(StaticSemaphore_t))
        {
            handle = xSemaphoreCreateRecursiveMutexStatic((StaticSemaphore_t *)attr->cb_mem);
        }
        #endif
    }
    else
    {
        #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
        handle = xSemaphoreCreateRecursiveMutex();
        #endif
    }

    return (osal_mutex_handle_t)handle;
}

osal_status_t osal_mutex_delete(osal_mutex_handle_t mutex)
{
    if (mutex == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    vSemaphoreDelete((SemaphoreHandle_t)mutex);
    return OSAL_SUCCESS;
}

osal_status_t osal_mutex_lock(osal_mutex_handle_t mutex, uint32_t timeout_ms)
{
    TickType_t ticks;

    if (mutex == NULL)
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

    if (xSemaphoreTakeRecursive((SemaphoreHandle_t)mutex, ticks) == pdTRUE)
    {
        return OSAL_SUCCESS;
    }
    else
    {
        return OSAL_ERROR_TIMEOUT;
    }
}

osal_status_t osal_mutex_unlock(osal_mutex_handle_t mutex)
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
