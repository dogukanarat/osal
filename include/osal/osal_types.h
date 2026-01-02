/* osalTypes.h - Common types and definitions */

/* All Rights Reserved */

#ifndef INC_OSAL_TYPES_H
#define INC_OSAL_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* Configurations */

/* Definitions */
#define OSAL_WAIT_FOREVER 0xFFFFFFFFU
#define OSAL_NO_WAIT      0x00000000U

/* Types */

/**
 * @brief OSAL Status Codes
 */
typedef enum
{
    OSAL_SUCCESS = 0,
    OSAL_ERROR,
    OSAL_ERROR_TIMEOUT,
    OSAL_ERROR_RESOURCE,
    OSAL_ERROR_PARAMETER,
    OSAL_ERROR_NOMEM,
    OSAL_ERROR_ISR,
    OSAL_ERROR_NOT_IMPLEMENTED,
    OSAL_ERROR_LAST,
} osal_status_t;

/* External Declarations */

#ifdef __cplusplus
}
#endif

#endif /* INC_OSAL_TYPES_H */
