#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "os_memory.hpp"

void *OS_mem_allocate( uint32_t d_size_u32 )
{
    return malloc( d_size_u32 );
}

void OS_mem_set( void *p_base_address, uint32_t d_data_u32, uint32_t d_size_u32 )
{
    memset( p_base_address, d_data_u32, d_size_u32 );
}

void OS_mem_copy( void *p_dest_address, void *p_source_address, uint32_t d_size_u32 )
{
    memmove( p_dest_address, p_source_address, d_size_u32 );
}

void OS_mem_fill_with_zeros( void *p_base_address, uint32_t d_size_u32 )
{
    OS_mem_set( p_base_address, 0, d_size_u32);
}