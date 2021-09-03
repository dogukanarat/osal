#ifndef _OSAL_MEMORY_HPP
#define _OSAL_MEMORY_HPP

#include <stdint.h>

namespace OSAL
{
    class Memory
    {
        public:
        static void* memCopy( void *p_dest_address, void *p_source_address, uint32_t d_size_u32 );
        static void* memAllocate( uint32_t d_size_u32 );
        static void* memSet( void *p_base_address, uint32_t d_data_u32, uint32_t d_size_u32 );
        static void* memFillWithZeros( void *p_base_address, uint32_t d_size_u32 );
    };

}




#endif
