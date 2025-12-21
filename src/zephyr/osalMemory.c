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

void *osalMemoryAlloc(size_t size)
{
    return k_malloc(size);
}

void osalMemoryFree(void *ptr)
{
    k_free(ptr);
}
