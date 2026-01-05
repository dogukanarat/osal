/* osalTime.c - POSIX Time Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_time.h"
#include <time.h>
#include <errno.h>
#include <sys/time.h>

/* Functions */

void osal_delay_ms(uint32_t ms)
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

uint32_t osal_get_tick_ms(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
    {
        return 0;
    }
    return (uint32_t)((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000));
}

osal_status_t osal_get_unix_time(osal_time_val_t *tv)
{
    struct timespec ts;
    if (tv == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (clock_gettime(CLOCK_REALTIME, &ts) != 0)
    {
        return OSAL_ERROR;
    }

    tv->sec = (uint32_t)ts.tv_sec;
    tv->usec = (uint32_t)(ts.tv_nsec / 1000);

    return OSAL_SUCCESS;
}
