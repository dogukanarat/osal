/* osalMemory.h - one line definition */

/* All Rights Reserved */

#ifndef INC_OSAL_MEMORY_H
#define INC_OSAL_MEMORY_H

/* Includes */

#include "osalTypes.h"

#ifdef __cplusplus
extern "C" {
#endif


/* Configurations */


/* Definitions */


/* Types */


/* External Declarations */

/**
 * @brief Allocates memory from the heap.
 *
 * @param size
 * @return void*
 */
extern void *osalMemoryAlloc(size_t size);

/**
 * @brief Frees previously allocated memory.
 *
 * @param ptr
 */
extern void osalMemoryFree(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* INC_OSAL_MEMORY_H */
