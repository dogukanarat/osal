#ifndef _OSAL_THREAD_HPP
#define _OSAL_THREAD_HPP

#include "pthread.h"

namespace OSAL
{
    class Thread
    {
        public:
        typedef pthread_t thread_t;

        static int create ( thread_t *p_thread_handler_st, void *(*p_thread)(void *) );
        static int join   ( thread_t d_thread_handler_st );
    };
}


#endif