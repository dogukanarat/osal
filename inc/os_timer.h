#ifndef _OSAL_TIMER_HPP
#define _OSAL_TIMER_HPP

#include "stdint.h"

namespace OSAL
{
    class Timer
    {
        public:
        static void waitUs( uint32_t d_microsecond_u32 );
    };
}

#endif