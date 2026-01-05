/* osalMemory.c - FreeRTOS Memory Management Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_memory.h"
#include "FreeRTOS.h"
#include <stddef.h>

/* Functions */

void *osal_memory_alloc(size_t size)
{
    return pvPortMalloc(size);
}

void osal_memory_free(void *ptr)
{
    vPortFree(ptr);
}
