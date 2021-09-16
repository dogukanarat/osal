#ifndef _OSAL_MUTEX_H
#define _OSAL_MUTEX_H

#include "consts.h"
#include "stdint.h"
#include <pthread.h>

namespace OSAL
{
    class Mutex
    {
        friend class CondVar;
        pthread_mutex_t m_mutex;

        public:
        Mutex() 
        {
            pthread_mutex_init(&m_mutex, NULL); 
        };
        virtual ~Mutex()
        {
            pthread_mutex_unlock(&m_mutex);
            pthread_mutex_destroy(&m_mutex);
        };
        
        inline int Lock()
        {
            return pthread_mutex_lock(&m_mutex);
        }
        inline int TryLock()
        {
            return pthread_mutex_trylock(&m_mutex);
        }
        inline int Unlock() 
        {
            return pthread_mutex_unlock(&m_mutex);
        }   
    };
}

#endif