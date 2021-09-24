#include "os_memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace OSAL;

void *Memory::memAllocate(uint32_t d_size_u32)
{
    return malloc(d_size_u32);
}

void *Memory::memSet(void *p_base_address, uint32_t d_data_u32, uint32_t d_size_u32)
{
    return memset(p_base_address, d_data_u32, d_size_u32);
}

void *Memory::memCopy(void *p_dest_address, void *p_source_address, uint32_t d_size_u32)
{
    return memmove(p_dest_address, p_source_address, d_size_u32);
}

void *Memory::memFillWithZeros(void *p_base_address, uint32_t d_size_u32)
{
    return Memory::memSet(p_base_address, 0, d_size_u32);
}
