/* libscaffold_types.h - Common types and definitions */

/* All Rights Reserved */

#ifndef INC_LIBSCAFFOLD_TYPES_H
#define INC_LIBSCAFFOLD_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes */

#include <stdint.h>

/* Configurations */


/* Definitions */

#define LIBSCAFFOLD_MAX_STRING_LEN 256
#define LIBSCAFFOLD_VERSION_MAJOR 1
#define LIBSCAFFOLD_VERSION_MINOR 0
#define LIBSCAFFOLD_VERSION_PATCH 0

/* Types */

/**
 * @brief Return status codes for library functions
 */
typedef enum
{
    LIBSCAFFOLD_SUCCESS = 0,     /**< Operation successful */
    LIBSCAFFOLD_ERROR = -1,      /**< Generic error */
    LIBSCAFFOLD_ERROR_NULL = -2, /**< Null pointer error */
    LIBSCAFFOLD_ERROR_INVALID = -3 /**< Invalid parameter error */
} libscaffold_status_t;

/**
 * @brief Operation result structure
 */
typedef struct
{
    int32_t value;               /**< Result value */
    libscaffold_status_t status; /**< Operation status */
} libscaffold_result_t;

/* External Declarations */


#ifdef __cplusplus
}
#endif

#endif /* INC_LIBSCAFFOLD_TYPES_H */