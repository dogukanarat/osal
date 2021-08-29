#include <unistd.h>
#include "os_timer.h"

/**
 * @brief It cause a delay
 * 
 * @param d_microsecond_u32 
 */
void OS_wait_us( uint32_t d_microsecond_u32 )
{
    usleep( d_microsecond_u32 );
}