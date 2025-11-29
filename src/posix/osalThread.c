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

/* Global Key for Thread Local Storage */
static pthread_key_t g_osal_thread_key;
static pthread_once_t g_osal_key_once = PTHREAD_ONCE_INIT;

static void init_thread_key(void)
{
    pthread_key_create(&g_osal_thread_key, NULL);
}

/* Internal wrapper to match pthread signature */
static void *threadWrapper(void *arg)
{
    osalThreadControlBlock_t *cb = (osalThreadControlBlock_t *)arg;

    /* Store the CB handle in TLS so GetId() works correctly */
    pthread_once(&g_osal_key_once, init_thread_key);
    pthread_setspecific(g_osal_thread_key, cb);

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

    /* Initialize TLS key if not already done */
    pthread_once(&g_osal_key_once, init_thread_key);

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

    /* Handle priority */
    if (attr && attr->priority > 0)
    {
        pthread_attr_setschedpolicy(&threadAttr, SCHED_FIFO);
        param.sched_priority = attr->priority;
        pthread_attr_setschedparam(&threadAttr, &param);
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

    /* Set name if available */
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
        /* If NULL, delete self */
        cb = (osalThreadControlBlock_t *)osalThreadGetId();
        if (cb == NULL) return OSAL_ERROR_PARAMETER;
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
    pthread_once(&g_osal_key_once, init_thread_key);
    return (osalThreadHandle_t)pthread_getspecific(g_osal_thread_key);
}
