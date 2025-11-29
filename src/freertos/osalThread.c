/* osalThread.c - FreeRTOS Thread Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osalThread.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include <stddef.h>
#include "FreeRTOSConfig.h"

/* Functions */

osalThreadHandle_t osalThreadCreate(osalThreadFunc_t func, void *arg, const osalThreadAttr_t *attr)
{
    TaskHandle_t handle = NULL;
    const char *name = "Thread";
    uint32_t stackSize = configMINIMAL_STACK_SIZE;
    UBaseType_t priority = tskIDLE_PRIORITY + 1;

    if (func == NULL)
    {
        return NULL;
    }

    if (attr)
    {
        if (attr->name) name = attr->name;
        if (attr->stackSize > 0) stackSize = attr->stackSize;
        if (attr->priority > 0) priority = attr->priority;
    }

    if (attr && attr->cbMem && attr->stackMem)
    {
        #if (configSUPPORT_STATIC_ALLOCATION == 1)
        if (attr->cbSize >= sizeof(StaticTask_t))
        {
            handle = xTaskCreateStatic(func, name, stackSize, arg, priority, (StackType_t *)attr->stackMem, (StaticTask_t *)attr->cbMem);
        }
        #endif
    }
    else
    {
        #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
        if (xTaskCreate(func, name, stackSize, arg, priority, &handle) != pdPASS)
        {
            handle = NULL;
        }
        #endif
    }

    return (osalThreadHandle_t)handle;
}

osalStatus_t osalThreadDelete(osalThreadHandle_t thread)
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

void osalThreadYield(void)
{
    taskYIELD();
}

osalThreadHandle_t osalThreadGetId(void)
{
    return (osalThreadHandle_t)xTaskGetCurrentTaskHandle();
}
