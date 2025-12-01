/* osalTime.c - POSIX Time Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osalTime.h"
#include <osal/osal.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <string.h>
#include <stdlib.h>

/* Functions */

void osalDelayMs(uint32_t ms) {
    k_msleep(ms);
}

uint32_t osalGetTickMs(void) {
    return (uint32_t)k_uptime_get();
}

osalStatus_t osalGetUnixTime(osalTimeVal_t *tv)
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
