/* osalTime.c - FreeRTOS Time Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_time.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stddef.h>

/* Functions */

void osal_delay_ms(uint32_t ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}

uint32_t osal_get_tick_ms(void)
{
    return (uint32_t)(xTaskGetTickCount() * portTICK_PERIOD_MS);
}

osal_status_t osal_get_unix_time(osal_time_val_t *tv)
{
    /* FreeRTOS doesn't have a standard RTC.
       Users should implement a hook or provide a weak function.
    */
    extern uint32_t osal_get_unix_timeHook(osal_time_val_t *tv) __attribute__((weak));

    if (tv == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (osal_get_unix_timeHook)
    {
        return (osal_status_t)osal_get_unix_timeHook(tv);
    }

    return OSAL_ERROR;
}
