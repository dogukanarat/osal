/* osalTime.c - FreeRTOS Time Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osalTime.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stddef.h>

/* Functions */

void osalDelayMs(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

uint32_t osalGetTickMs(void)
{
    return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

osalStatus_t osalGetUnixTime(osalTimeVal_t *tv)
{
    /* FreeRTOS doesn't have a standard RTC.
       Users should implement a hook or provide a weak function.
    */
    extern uint32_t osalGetUnixTimeHook(osalTimeVal_t *tv) __attribute__((weak));

    if (tv == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (osalGetUnixTimeHook)
    {
        return (osalStatus_t)osalGetUnixTimeHook(tv);
    }

    return OSAL_ERROR;
}
