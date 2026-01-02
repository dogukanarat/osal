/* osalMemory.c - macOS Memory Management Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_memory.h"
#include <stdlib.h>

/* Functions */

void *osal_memory_alloc(size_t size)
{
    return malloc(size);
}

void osal_memory_free(void *ptr)
{
    free(ptr);
}
