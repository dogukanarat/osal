/* osalTime.h - Time management functions */

/* All Rights Reserved */

#ifndef INC_OSAL_TIME_H
#define INC_OSAL_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "osalTypes.h"

/* Configurations */

/* Definitions */

/* Types */

/**
 * @brief Time value structure for Unix time
 */
typedef struct
{
    uint32_t sec;  /**< Seconds */
    uint32_t usec; /**< Microseconds */
} osalTimeVal_t;

/* External Declarations */

/**
 * @brief Delay execution for a specified number of milliseconds.
 * @param ms Milliseconds to delay.
 */
void osalDelayMs(uint32_t ms);

/**
 * @brief Get the current system tick in milliseconds.
 * @return Current system tick.
 */
uint32_t osalGetTickMs(void);

/**
 * @brief Get current Unix time.
 * @param tv Pointer to time value structure to store result.
 * @return OSAL_SUCCESS on success, error code otherwise.
 */
osalStatus_t osalGetUnixTime(osalTimeVal_t *tv);

#ifdef __cplusplus
}
#endif

#endif /* INC_OSAL_TIME_H */