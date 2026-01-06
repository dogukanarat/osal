/* scaffold_project_types.h - Common types and definitions */

/* All Rights Reserved */

#ifndef INC_SCAFFOLD_PROJECT_TYPES_H
#define INC_SCAFFOLD_PROJECT_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes */

#include <stdint.h>

/* Configurations */

/* API visibility */
#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef SCAFFOLD_PROJECT_EXPORTS
#define SCAFFOLD_PROJECT_API __declspec(dllexport)
#else
#define SCAFFOLD_PROJECT_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#define SCAFFOLD_PROJECT_API __attribute__((visibility("default")))
#else
#define SCAFFOLD_PROJECT_API
#endif


/* Definitions */

#define SCAFFOLD_PROJECT_MAX_STRING_LEN 256
#define SCAFFOLD_PROJECT_VERSION_MAJOR 1
#define SCAFFOLD_PROJECT_VERSION_MINOR 0
#define SCAFFOLD_PROJECT_VERSION_PATCH 0

/* Types */

/**
 * @brief Return status codes for library functions
 */
typedef enum
{
    SCAFFOLD_PROJECT_SUCCESS = 0,     /**< Operation successful */
    SCAFFOLD_PROJECT_ERROR = -1,      /**< Generic error */
    SCAFFOLD_PROJECT_ERROR_NULL = -2, /**< Null pointer error */
    SCAFFOLD_PROJECT_ERROR_INVALID = -3 /**< Invalid parameter error */
} scaffold_project_status_t;

/**
 * @brief Operation result structure
 */
typedef struct
{
    int32_t value;               /**< Result value */
    scaffold_project_status_t status; /**< Operation status */
} scaffold_project_result_t;

/* External Declarations */


#ifdef __cplusplus
}
#endif

#endif /* INC_SCAFFOLD_PROJECT_TYPES_H */
