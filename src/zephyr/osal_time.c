/* osalTime.c - POSIX Time Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_time.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <string.h>
#include <stdlib.h>

/* Functions */

void osal_delay_ms(uint32_t ms) {
    k_msleep(ms);
}

uint32_t osal_get_tick_ms(void) {
    return (uint32_t)k_uptime_get();
}

osal_status_t osal_get_unix_time(osal_time_val_t *tv)
{
    if (tv == NULL) {
        return OSAL_ERROR_PARAMETER;
    }

    // k_uptime_get() returns time since boot in milliseconds (int64_t)
    int64_t uptime_ms = k_uptime_get();

    // Convert milliseconds to Seconds and Microseconds
    tv->sec = (uint32_t)(uptime_ms / 1000);
    tv->usec = (uint32_t)((uptime_ms % 1000) * 1000);

    return OSAL_SUCCESS;
}
