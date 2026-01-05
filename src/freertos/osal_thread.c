/* osalThread.c - FreeRTOS Thread Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_thread.h"
#include "FreeRTOS.h"
#include "osal/osal_types.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include <stddef.h>
#include "FreeRTOSConfig.h"

/* Functions */

osal_thread_handle_t osal_thread_create(osal_thread_func_t func, void *arg, const osal_thread_attr_t *attr)
{
    TaskHandle_t handle = NULL;
    const char *name = "Thread";
    uint32_t stack_size = configMINIMAL_STACK_SIZE;
    UBaseType_t priority = tskIDLE_PRIORITY + 1;

    if (func == NULL)
    {
        return NULL;
    }

    if (attr)
    {
        if (attr->name) name = attr->name;
        if (attr->stack_size > 0) stack_size = attr->stack_size;
        if (attr->priority > 0) priority = attr->priority;
    }

    if (attr && attr->cb_mem && attr->stack_mem)
    {
        #if (configSUPPORT_STATIC_ALLOCATION == 1)
        if (attr->cb_size >= sizeof(StaticTask_t))
        {
            handle = xTaskCreateStatic(func, name, stack_size, arg, priority, (StackType_t *)attr->stack_mem, (StaticTask_t *)attr->cb_mem);
        }
        #endif
    }
    else
    {
        #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
        if (xTaskCreate(func, name, stack_size, arg, priority, &handle) != pdPASS)
        {
            handle = NULL;
        }
        #endif
    }

    return (osal_thread_handle_t)handle;
}

osal_status_t osal_thread_delete(osal_thread_handle_t thread)
{
    if (thread == NULL)
    {
        /* Delete self */
        vTaskDelete(NULL);
    }
    else
    {
        vTaskDelete((TaskHandle_t)thread);
    }

    return OSAL_SUCCESS;
}

void osal_thread_yield(void)
{
    taskYIELD();
}

osal_thread_handle_t osal_thread_get_id(void)
{
    return (osal_thread_handle_t)xTaskGetCurrentTaskHandle();
}

osal_status_t osal_thread_join(osal_thread_handle_t thread)
{
    /* FreeRTOS does not support joining threads */
    (void)thread;
    return OSAL_ERROR_NOT_IMPLEMENTED;
}