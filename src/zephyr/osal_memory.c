/* osalMemory.c - one line definition */

/* All Rights Reserved */

/* Includes */

#include <zephyr/kernel.h>

/* Imports */


/* Definitions */


/* Types */


/* Forward Declarations */


/* Variables */


/* Functions */

void *osal_memory_alloc(size_t size)
{
    return k_malloc(size);
}

void osal_memory_free(void *ptr)
{
    k_free(ptr);
}
