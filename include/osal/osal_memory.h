/* osalMemory.h - one line definition */

/* All Rights Reserved */

#ifndef INC_OSAL_MEMORY_H
#define INC_OSAL_MEMORY_H

/* Includes */

#include "osal_types.h"

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
extern void *osal_memory_alloc(size_t size);

/**
 * @brief Frees previously allocated memory.
 *
 * @param ptr
 */
extern void osal_memory_free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* INC_OSAL_MEMORY_H */
