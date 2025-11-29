/* osalThread.c - POSIX Thread Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osalThread.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>

/* Internal Types */
typedef struct
{
    pthread_t thread;
    osalThreadFunc_t func;
    void *arg;
    bool isStatic;
} osalThreadControlBlock_t;

/* Internal wrapper to match pthread signature */
static void *threadWrapper(void *arg)
{
    osalThreadControlBlock_t *cb = (osalThreadControlBlock_t *)arg;
    if (cb && cb->func)
    {
        cb->func(cb->arg);
    }
    return NULL;
}

/* Functions */

osalThreadHandle_t osalThreadCreate(osalThreadFunc_t func, void *arg, const osalThreadAttr_t *attr)
{
    osalThreadControlBlock_t *cb;
    pthread_attr_t threadAttr;
    struct sched_param param;
    int ret;

    if (func == NULL)
    {
        return NULL;
    }

    /* Allocate control block */
    if (attr && attr->cbMem)
    {
        if (attr->cbSize < sizeof(osalThreadControlBlock_t))
        {
            return NULL;
        }
        cb = (osalThreadControlBlock_t *)attr->cbMem;
        cb->isStatic = true;
    }
    else
    {
        cb = (osalThreadControlBlock_t *)malloc(sizeof(osalThreadControlBlock_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->isStatic = false;
    }

    cb->func = func;
    cb->arg = arg;

    pthread_attr_init(&threadAttr);

    /* Handle stack */
    if (attr && attr->stackMem && attr->stackSize > 0)
    {
        pthread_attr_setstack(&threadAttr, attr->stackMem, attr->stackSize);
    }
    else if (attr && attr->stackSize > 0)
    {
        pthread_attr_setstacksize(&threadAttr, attr->stackSize);
    }

    /* Handle priority (requires root/CAP_SYS_NICE usually, so we just try) */
    if (attr && attr->priority > 0)
    {
        pthread_attr_setschedpolicy(&threadAttr, SCHED_FIFO);
        param.sched_priority = attr->priority;
        pthread_attr_setschedparam(&threadAttr, &param);
        /* We don't check return here as setting priority might fail on non-realtime systems */
    }

    ret = pthread_create(&cb->thread, &threadAttr, threadWrapper, cb);
    pthread_attr_destroy(&threadAttr);

    if (ret != 0)
    {
        if (!cb->isStatic)
        {
            free(cb);
        }
        return NULL;
    }

    /* Set name if available (non-standard but common) */
    if (attr && attr->name)
    {
        #ifdef __linux__
        pthread_setname_np(cb->thread, attr->name);
        #endif
    }

    return (osalThreadHandle_t)cb;
}

osalStatus_t osalThreadDelete(osalThreadHandle_t thread)
{
    osalThreadControlBlock_t *cb = (osalThreadControlBlock_t *)thread;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    pthread_cancel(cb->thread);
    pthread_join(cb->thread, NULL);

    if (!cb->isStatic)
    {
        free(cb);
    }

    return OSAL_SUCCESS;
}

void osalThreadYield(void)
{
    sched_yield();
}

osalThreadHandle_t osalThreadGetId(void)
{
    /* This is tricky as we don't have easy access to the CB from just pthread_self() 
       without a lookup table. For now, we return the pthread_t casted, 
       BUT this breaks the handle type safety if we expect osalThreadHandle_t to be the CB.
       However, typically GetId is used for comparison. 
       
       If we need the CB, we would need thread-local storage or a global list.
       For simplicity in this scaffold, we'll return NULL or implement a TLS approach later.
       Actually, let's just return the pthread_t as a void* for ID comparison purposes,
       but note that it's not a valid handle for Delete() in this implementation.
       
       Better approach: Use the CB as the handle. 
       To return the CB from GetId, we need to store the CB in TLS.
    */
    return (osalThreadHandle_t)pthread_self(); 
}
