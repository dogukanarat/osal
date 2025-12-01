/* osalThread.c - Zephyr Thread Implementation */

/* All Rights Reserved */

/* Includes */
#include <osal/osalThread.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <string.h>
#include <stdlib.h>

/* Internal Types */
typedef struct
{
    struct k_thread thread;    /* Zephyr thread object */
    osalThreadFunc_t func;     /* User function */
    void *arg;                 /* User argument */
    void *allocated_stack;     /* Pointer to malloc'd stack (to free later) */
    bool isStatic;             /* Flag for static memory management */
} osalThreadControlBlock_t;

/* * Zephyr Entry Point Wrapper
 * Adapts the 3-argument Zephyr entry to the 1-argument OSAL entry.
 */
static void threadWrapper(void *p1, void *p2, void *p3)
{
    osalThreadControlBlock_t *cb = (osalThreadControlBlock_t *)p1;

    /* Store the CB handle in Zephyr's custom data for osalThreadGetId() */
    k_thread_custom_data_set(cb);

    if (cb && cb->func)
    {
        cb->func(cb->arg);
    }
}

/* Functions */

osalThreadHandle_t osalThreadCreate(osalThreadFunc_t func, void *arg, const osalThreadAttr_t *attr)
{
    osalThreadControlBlock_t *cb;
    k_thread_stack_t *stack;
    size_t stack_size;
    int priority;

    if (func == NULL)
    {
        return NULL;
    }

    /* ----------------------------------------------------------------
     * 1. Control Block Allocation
     * ---------------------------------------------------------------- */
    if (attr && attr->cbMem)
    {
        /* Static Allocation */
        if (attr->cbSize < sizeof(osalThreadControlBlock_t))
        {
            return NULL; /* Provided buffer too small */
        }
        cb = (osalThreadControlBlock_t *)attr->cbMem;
        cb->isStatic = true;
    }
    else
    {
        /* Dynamic Allocation (Requires CONFIG_HEAP_MEM_POOL_SIZE) */
        cb = (osalThreadControlBlock_t *)k_malloc(sizeof(osalThreadControlBlock_t));
        if (cb == NULL)
        {
            return NULL;
        }
        cb->isStatic = false;
    }

    cb->func = func;
    cb->arg = arg;
    cb->allocated_stack = NULL;

    /* ----------------------------------------------------------------
     * 2. Stack Allocation
     * ---------------------------------------------------------------- */
    /* Default Zephyr stack size if not specified (e.g., 1024 bytes) */
    size_t requested_size = (attr && attr->stackSize > 0) ? attr->stackSize : 1024;

    if (attr && attr->stackMem)
    {
        /* User provided stack - Must be aligned! */
        stack = (k_thread_stack_t *)attr->stackMem;
        stack_size = requested_size;
    }
    else
    {
        /* Dynamic stack - Must be aligned to Z_KERNEL_STACK_OBJ_ALIGN */
        /* Note: k_aligned_alloc is required for stacks */
        cb->allocated_stack = k_aligned_alloc(Z_KERNEL_STACK_OBJ_ALIGN,
                                              Z_KERNEL_STACK_SIZE_ADJUST(requested_size));

        if (cb->allocated_stack == NULL)
        {
            if (!cb->isStatic) k_free(cb);
            return NULL;
        }
        stack = (k_thread_stack_t *)cb->allocated_stack;
        stack_size = requested_size;
    }

    /* ----------------------------------------------------------------
     * 3. Priority Mapping
     * ---------------------------------------------------------------- */
    /* Zephyr: Lower number = Higher priority.
       Mapping OSAL priorities (assuming 0=Normal) to Zephyr Preemptible priorities.
       You may need to adjust this map based on your system design. */
    if (attr)
    {
        switch(attr->priority)
        {
            case OSAL_THREAD_PRIORITY_REALTIME: priority = K_PRIO_PREEMPT(1);  break;
            case OSAL_THREAD_PRIORITY_HIGH:     priority = K_PRIO_PREEMPT(5);  break;
            case OSAL_THREAD_PRIORITY_LOW:      priority = K_PRIO_PREEMPT(15); break;
            case OSAL_THREAD_PRIORITY_NORMAL:
            default:                            priority = K_PRIO_PREEMPT(10); break;
        }
    }
    else
    {
        priority = K_PRIO_PREEMPT(10);
    }

    /* ----------------------------------------------------------------
     * 4. Thread Creation
     * ---------------------------------------------------------------- */
    k_tid_t tid = k_thread_create(&cb->thread,
                                  stack, stack_size,
                                  threadWrapper,
                                  cb, NULL, NULL, /* Pass CB as p1 */
                                  priority,
                                  0, K_NO_WAIT);

    /* Set name if available (Requires CONFIG_THREAD_NAME=y) */
    if (attr && attr->name)
    {
        k_thread_name_set(tid, attr->name);
    }

    return (osalThreadHandle_t)cb;
}

osalStatus_t osalThreadDelete(osalThreadHandle_t thread)
{
    osalThreadControlBlock_t *cb = (osalThreadControlBlock_t *)thread;
    osalThreadControlBlock_t *self = (osalThreadControlBlock_t *)osalThreadGetId();

    if (cb == NULL)
    {
        cb = self;
    }

    if (cb == NULL) return OSAL_ERROR_PARAMETER;

    /* Abort the target thread */
    k_thread_abort(&cb->thread);

    /* * Resource Cleanup:
     * WARNING: If a thread deletes ITSELF, we cannot free the stack/cb
     * we are currently executing on.
     */
    if (cb != self && !cb->isStatic)
    {
        if (cb->allocated_stack)
        {
            k_free(cb->allocated_stack);
        }
        k_free(cb);
    }

    return OSAL_SUCCESS;
}

void osalThreadYield(void)
{
    k_yield();
}

osalThreadHandle_t osalThreadGetId(void)
{
    /* * Requires CONFIG_THREAD_CUSTOM_DATA=y in prj.conf
     * This retrieves the pointer we stored in threadWrapper.
     */
    return (osalThreadHandle_t)k_thread_custom_data_get();
}
