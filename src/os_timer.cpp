#include <unistd.h>
#include "osal/os_timer.h"

using namespace OSAL;

/**
 * @brief It cause a delay
 * 
 * @param d_microsecond_u32 
 */
void Timer::waitUs(uint32_t d_microsecond_u32)
{
    usleep(d_microsecond_u32);
}
