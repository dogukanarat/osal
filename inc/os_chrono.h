#ifndef _OSAL_CHRONO_H
#define _OSAL_CHRONO_H

#include "stdint.h"
#include <chrono>
#include <thread>

namespace OSAL
{
    class ThChrono
    {
        public:
        typedef std::chrono::system_clock::time_point TimePoint;
        typedef std::chrono::milliseconds MilliSecond;

        static TimePoint Now()
        {
            return std::chrono::system_clock::now();
        }

        static inline void SleepUntil( TimePoint time )
        {
            std::this_thread::sleep_until( time );
        }

        
    };
}

#endif