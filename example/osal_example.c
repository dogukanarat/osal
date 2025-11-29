/* osal_example.c - OSAL Verification Example */

#include "osal/osalTime.h"
#include "osal/osalThread.h"
#include "osal/osalMutex.h"
#include "osal/osalSemaphore.h"
#include "osal/osalMessageQueue.h"
#include "osal/osalEventFlags.h"
#include <stdio.h>
#include <string.h>

/* Objects */
osalMutexHandle_t g_mutex;
osalSemaphoreHandle_t g_sem;
osalQueueHandle_t g_queue;
osalEventFlagsHandle_t g_flags;

/* Thread 1: Producer */
void thread1_func(void *arg)
{
    (void)arg;
    uint32_t count = 0;
    
    printf("Thread 1 started\n");

    while (count < 5)
    {
        osalDelayMs(100);
        
        /* Mutex test */
        osalMutexLock(g_mutex, OSAL_WAIT_FOREVER);
        printf("Thread 1: Locked mutex\n");
        osalDelayMs(50);
        printf("Thread 1: Unlocking mutex\n");
        osalMutexUnlock(g_mutex);

        /* Queue test */
        printf("Thread 1: Sending %d to queue\n", count);
        osalQueueSend(g_queue, &count, OSAL_WAIT_FOREVER);

        /* Semaphore test */
        printf("Thread 1: Giving semaphore\n");
        osalSemaphoreGive(g_sem);

        /* Event flags test */
        if (count == 4)
        {
            printf("Thread 1: Setting event flag 0x01\n");
            osalEventFlagsSet(g_flags, 0x01);
        }

        count++;
    }
    
    printf("Thread 1 finished\n");
}

/* Thread 2: Consumer */
void thread2_func(void *arg)
{
    (void)arg;
    uint32_t msg;
    uint32_t flags;
    
    printf("Thread 2 started\n");

    for (int i = 0; i < 5; i++)
    {
        /* Semaphore test */
        printf("Thread 2: Waiting for semaphore...\n");
        osalSemaphoreTake(g_sem, OSAL_WAIT_FOREVER);
        printf("Thread 2: Got semaphore\n");

        /* Queue test */
        if (osalQueueReceive(g_queue, &msg, 1000) == OSAL_SUCCESS)
        {
            printf("Thread 2: Received %d from queue\n", msg);
        }
        else
        {
            printf("Thread 2: Queue receive timeout\n");
        }
    }

    /* Event flags test */
    printf("Thread 2: Waiting for event flag 0x01...\n");
    flags = osalEventFlagsWait(g_flags, 0x01, OSAL_EVENT_WAIT_ALL, 2000);
    if (flags & 0x01)
    {
        printf("Thread 2: Got event flag 0x01\n");
    }
    else
    {
        printf("Thread 2: Event flag timeout\n");
    }
    
    printf("Thread 2 finished\n");
}

int main(void)
{
    printf("OSAL Verification Example\n");

    /* Create objects */
    osalMutexAttr_t mutexAttr = { .name = "TestMutex", .attrBits = OSAL_MUTEX_RECURSIVE };
    g_mutex = osalMutexCreate(&mutexAttr);
    if (!g_mutex) printf("Failed to create mutex\n");

    osalSemaphoreAttr_t semAttr = { .name = "TestSem", .maxCount = 10, .initialCount = 0 };
    g_sem = osalSemaphoreCreate(&semAttr);
    if (!g_sem) printf("Failed to create semaphore\n");

    osalQueueAttr_t queueAttr = { .name = "TestQueue" };
    g_queue = osalQueueCreate(10, sizeof(uint32_t), &queueAttr);
    if (!g_queue) printf("Failed to create queue\n");

    osalEventFlagsAttr_t flagsAttr = { .name = "TestFlags" };
    g_flags = osalEventFlagsCreate(&flagsAttr);
    if (!g_flags) printf("Failed to create event flags\n");

    /* Create threads */
    osalThreadAttr_t t1Attr = { .name = "Thread1", .stackSize = 4096, .priority = OSAL_THREAD_PRIORITY_NORMAL };
    osalThreadHandle_t t1 = osalThreadCreate(thread1_func, NULL, &t1Attr);
    if (!t1) printf("Failed to create Thread 1\n");

    osalThreadAttr_t t2Attr = { .name = "Thread2", .stackSize = 4096, .priority = OSAL_THREAD_PRIORITY_NORMAL };
    osalThreadHandle_t t2 = osalThreadCreate(thread2_func, NULL, &t2Attr);
    if (!t2) printf("Failed to create Thread 2\n");

    /* Main thread waits */
    osalDelayMs(3000);

    /* ISR API Test (Simulated) */
    printf("Testing ISR APIs (Abstracted)...\n");
    osalSemaphoreGive(g_sem);
    uint32_t val = 123;
    osalQueueSend(g_queue, &val, OSAL_NO_WAIT);
    osalEventFlagsSet(g_flags, 0x02);
    printf("ISR APIs called successfully\n");

    /* Clean up */
    osalThreadDelete(t1);
    osalThreadDelete(t2);
    osalMutexDelete(g_mutex);
    osalSemaphoreDelete(g_sem);
    osalQueueDelete(g_queue);
    osalEventFlagsDelete(g_flags);

    printf("Example finished\n");
    return 0;
}
