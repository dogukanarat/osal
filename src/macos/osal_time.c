/* osalTime.c - macOS Time Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_time.h"
#include <mach/mach_time.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

/* Variables */
static mach_timebase_info_data_t timebase_info;
static int timebase_initialized = 0;

/* Functions */

static void init_timebase(void)
{
    if (!timebase_initialized)
    {
        mach_timebase_info(&timebase_info);
        timebase_initialized = 1;
    }
}

void osal_delay_ms(uint32_t ms)
{
    usleep(ms * 1000);
}

uint32_t osal_get_tick_ms(void)
{
    init_timebase();

    uint64_t absTime = mach_absolute_time();
    uint64_t nanos = absTime * timebase_info.numer / timebase_info.denom;

    return (uint32_t)(nanos / 1000000);
}

osal_status_t osal_get_unix_time(osal_time_val_t *tv)
{
    struct timeval timeVal;

    if (tv == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    if (gettimeofday(&timeVal, NULL) != 0)
    {
        return OSAL_ERROR;
    }

    tv->sec = (uint32_t)timeVal.tv_sec;
    tv->usec = (uint32_t)timeVal.tv_usec;

    return OSAL_SUCCESS;
}
