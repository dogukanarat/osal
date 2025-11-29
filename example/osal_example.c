/* osal_example.c - OSAL Verification Example */

#include "osal/osalEventFlags.h"
#include "osal/osalMessageQueue.h"
#include "osal/osalMutex.h"
#include "osal/osalSemaphore.h"
#include "osal/osalThread.h"
#include "osal/osalTime.h"
#include <stdio.h>
#include <string.h>

/* Objects */
osalMutexHandle_t Mutex;
osalSemaphoreHandle_t Sem;
osalQueueHandle_t Queue;
osalEventFlagsHandle_t Flags;

/* Thread 1: Producer */
void thread1Func(void *arg)
{
    (void)arg;
    uint32_t count = 0;

    printf("Thread 1 started\n");

    while (count < 5)
    {
        osalDelayMs(100);

        /* Mutex test */
        osalMutexLock(Mutex, OSAL_WAIT_FOREVER);
        printf("Thread 1: Locked mutex\n");
        osalDelayMs(50);
        printf("Thread 1: Unlocking mutex\n");
        osalMutexUnlock(Mutex);

        /* Queue test */
        printf("Thread 1: Sending %d to queue\n", count);
        osalQueueSend(Queue, &count, OSAL_WAIT_FOREVER);

        /* Semaphore test */
        printf("Thread 1: Giving semaphore\n");
        osalSemaphoreGive(Sem);

        /* Event flags test */
        if (count == 4)
        {
            printf("Thread 1: Setting event flag 0x01\n");
            osalEventFlagsSet(Flags, 0x01);
        }

        count++;
    }

    printf("Thread 1 finished\n");
}

/* Thread 2: Consumer */
void thread2Func(void *arg)
{
    (void)arg;
    uint32_t msg;
    uint32_t flags;

    printf("Thread 2 started\n");

    for (int i = 0; i < 5; i++)
    {
        /* Semaphore test */
        printf("Thread 2: Waiting for semaphore...\n");
        osalSemaphoreTake(Sem, OSAL_WAIT_FOREVER);
        printf("Thread 2: Got semaphore\n");

        /* Queue test */
        if (osalQueueReceive(Queue, &msg, 1000) == OSAL_SUCCESS)
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
    flags = osalEventFlagsWait(Flags, 0x01, OSAL_EVENT_WAIT_ALL, 2000);
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
    osalMutexAttr_t mutexAttr = {.name = "TestMutex", .attrBits = OSAL_MUTEX_RECURSIVE};
    Mutex = osalMutexCreate(&mutexAttr);
    if (!Mutex)
    {
        printf("Failed to create mutex\n");
    }

    osalSemaphoreAttr_t semAttr = {.name = "TestSem", .maxCount = 10, .initialCount = 0};
    Sem = osalSemaphoreCreate(&semAttr);
    if (!Sem)
    {
        printf("Failed to create semaphore\n");
    }

    osalQueueAttr_t queueAttr = {.name = "TestQueue"};
    Queue = osalQueueCreate(10, sizeof(uint32_t), &queueAttr);
    if (!Queue)
    {
        printf("Failed to create queue\n");
    }

    osalEventFlagsAttr_t flagsAttr = {.name = "TestFlags"};
    Flags = osalEventFlagsCreate(&flagsAttr);
    if (!Flags)
    {
        printf("Failed to create event flags\n");
    }

    /* Create threads */
    osalThreadAttr_t t1Attr = {
        .name = "Thread1",
        .stackSize = 4096,
        .priority = OSAL_THREAD_PRIORITY_NORMAL};
    osalThreadHandle_t t1 = osalThreadCreate(thread1Func, NULL, &t1Attr);
    if (!t1)
    {
        printf("Failed to create Thread 1\n");
    }

    osalThreadAttr_t t2Attr = {
        .name = "Thread2",
        .stackSize = 4096,
        .priority = OSAL_THREAD_PRIORITY_NORMAL};
    osalThreadHandle_t t2 = osalThreadCreate(thread2Func, NULL, &t2Attr);
    if (!t2)
    {
        printf("Failed to create Thread 2\n");
    }

    /* Main thread waits */
    osalDelayMs(3000);

    /* ISR API Test (Simulated) */
    printf("Testing ISR APIs (Abstracted)...\n");
    osalSemaphoreGive(Sem);
    uint32_t val = 123;
    osalQueueSend(Queue, &val, OSAL_NO_WAIT);
    osalEventFlagsSet(Flags, 0x02);
    printf("ISR APIs called successfully\n");

    /* Clean up */
    osalThreadDelete(t1);
    osalThreadDelete(t2);
    osalMutexDelete(Mutex);
    osalSemaphoreDelete(Sem);
    osalQueueDelete(Queue);
    osalEventFlagsDelete(Flags);

    printf("Example finished\n");
    return 0;
}
