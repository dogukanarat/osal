#include "stdint.h"

void OS_mem_copy( void *p_dest_address, void *p_source_address, uint32_t d_size_u32 );

void *OS_mem_allocate( uint32_t d_size_u32 );

void OS_mem_set( void *p_base_address, uint32_t d_data_u32, uint32_t d_size_u32 );

void OS_mem_fill_with_zeros( void *p_base_address, uint32_t d_size_u32 );
