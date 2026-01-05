/* osalThread.c - macOS Thread Implementation */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_thread.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>

/* Internal Types */
typedef struct
{
    pthread_t thread;
    osal_thread_func_t func;
    void *arg;
    const char *name;
    bool is_static;
} osal_thread_control_block_t;

/* Global Key for Thread Local Storage */
static pthread_key_t g_osal_thread_key;
static pthread_once_t g_osal_key_once = PTHREAD_ONCE_INIT;

static void init_thread_key(void)
{
    pthread_key_create(&g_osal_thread_key, NULL);
}

/* Internal wrapper to match pthread signature */
static void *thread_wrapper(void *arg)
{
    osal_thread_control_block_t *cb = (osal_thread_control_block_t *)arg;

    /* Store the CB handle in TLS so GetId() works correctly */
    pthread_once(&g_osal_key_once, init_thread_key);
    pthread_setspecific(g_osal_thread_key, cb);

    /* Set thread name from within the thread (macOS requirement) */
    if (cb && cb->name)
    {
        pthread_setname_np(cb->name);
    }

    if (cb && cb->func)
    {
        cb->func(cb->arg);
    }
    return NULL;
}

/* Functions */

osal_thread_handle_t osal_thread_create(osal_thread_func_t func, void *arg, const osal_thread_attr_t *attr)
{
    osal_thread_control_block_t *cb;
    pthread_attr_t thread_attr;
    int ret;

    if (func == NULL)
    {
        return NULL;
    }

    /* Initialize TLS key if not already done */
    pthread_once(&g_osal_key_once, init_thread_key);

    /* Allocate control block */
    if (attr && attr->cb_mem)
    {
        if (attr->cb_size < sizeof(osal_thread_control_block_t))
        {
            return NULL;
        }
        cb = (osal_thread_control_block_t *)attr->cb_mem;
        cb->is_static = true;
    }
    else
    {
        cb = (osal_thread_control_block_t *)malloc(sizeof(osal_thread_control_block_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->is_static = false;
    }

    cb->func = func;
    cb->arg = arg;
    cb->name = (attr && attr->name) ? attr->name : NULL;

    pthread_attr_init(&thread_attr);

    /* Handle stack */
    if (attr && attr->stack_mem && attr->stack_size > 0)
    {
        pthread_attr_setstack(&thread_attr, attr->stack_mem, attr->stack_size);
    }
    else if (attr && attr->stack_size > 0)
    {
        pthread_attr_setstacksize(&thread_attr, attr->stack_size);
    }

    /* Note: macOS doesn't support setting thread priority via pthread_attr_setschedparam
       as reliably as Linux. Priority handling is simplified here. */

    ret = pthread_create(&cb->thread, &thread_attr, thread_wrapper, cb);
    pthread_attr_destroy(&thread_attr);

    if (ret != 0)
    {
        if (!cb->is_static)
        {
            free(cb);
        }
        return NULL;
    }

    return (osal_thread_handle_t)cb;
}

osal_status_t osal_thread_delete(osal_thread_handle_t thread)
{
    osal_thread_control_block_t *cb = (osal_thread_control_block_t *)thread;

    if (cb == NULL)
    {
        /* If NULL, delete self */
        cb = (osal_thread_control_block_t *)osal_thread_get_id();
        if (cb == NULL)
        {
            return OSAL_ERROR_PARAMETER;
        }
    }

    pthread_cancel(cb->thread);
    pthread_join(cb->thread, NULL);

    if (!cb->is_static)
    {
        free(cb);
    }

    return OSAL_SUCCESS;
}

void osal_thread_yield(void)
{
    sched_yield();
}

osal_thread_handle_t osal_thread_get_id(void)
{
    pthread_once(&g_osal_key_once, init_thread_key);
    return (osal_thread_handle_t)pthread_getspecific(g_osal_thread_key);
}

osal_status_t osal_thread_join(osal_thread_handle_t thread)
{
    osal_thread_control_block_t *cb = (osal_thread_control_block_t *)thread;

    if (cb == NULL)
    {
        return OSAL_ERROR_PARAMETER;
    }

    pthread_join(cb->thread, NULL);

    return OSAL_SUCCESS;
}