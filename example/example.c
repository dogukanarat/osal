/* osal_example.c - OSAL Verification Example */

#include "osal/osal_event_flags.h"
#include "osal/osal_message_queue.h"
#include "osal/osal_mutex.h"
#include "osal/osal_semaphore.h"
#include "osal/osal_thread.h"
#include "osal/osal_time.h"
#include <stdio.h>
#include <string.h>

/* Objects */
osal_mutex_handle_t Mutex;
osal_semaphore_handle_t Sem;
osal_message_queue_handle_t Queue;
osal_event_flags_handle_t Flags;

/* Thread 1: Producer */
void thread1Func(void *arg)
{
    (void)arg;
    uint32_t count = 0;

    printf("Thread 1 started\n");

    while (count < 5)
    {
        osal_delay_ms(100);

        /* Mutex test */
        osal_mutex_lock(Mutex, OSAL_WAIT_FOREVER);
        printf("Thread 1: Locked mutex\n");
        osal_delay_ms(50);
        printf("Thread 1: Unlocking mutex\n");
        osal_mutex_unlock(Mutex);

        /* Queue test */
        printf("Thread 1: Sending %d to queue\n", count);
        osal_message_queue_send(Queue, &count, OSAL_WAIT_FOREVER);

        /* Semaphore test */
        printf("Thread 1: Giving semaphore\n");
        osal_semaphore_give(Sem);

        /* Event flags test */
        if (count == 4)
        {
            printf("Thread 1: Setting event flag 0x01\n");
            osal_event_flags_set(Flags, 0x01);
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
        osal_semaphore_take(Sem, OSAL_WAIT_FOREVER);
        printf("Thread 2: Got semaphore\n");

        /* Queue test */
        if (osal_message_queue_receive(Queue, &msg, 1000) == OSAL_SUCCESS)
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
    flags = osal_event_flags_wait(Flags, 0x01, OSAL_EVENT_WAIT_ALL, 2000);
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
    osal_mutex_attr_t mutex_attr = {.name = "TestMutex", .attr_bits = OSAL_MUTEX_RECURSIVE};
    Mutex = osal_mutex_create(&mutex_attr);
    if (!Mutex)
    {
        printf("Failed to create mutex\n");
    }

    osal_semaphore_attr_t semAttr = {.name = "TestSem", .max_count = 10, .initial_count = 0};
    Sem = osal_semaphore_create(&semAttr);
    if (!Sem)
    {
        printf("Failed to create semaphore\n");
    }

    osal_message_queue_attr_t queueAttr = {.name = "TestQueue"};
    Queue = osal_message_queue_create(10, sizeof(uint32_t), &queueAttr);
    if (!Queue)
    {
        printf("Failed to create queue\n");
    }

    osal_event_flags_attr_t flagsAttr = {.name = "TestFlags"};
    Flags = osal_event_flags_create(&flagsAttr);
    if (!Flags)
    {
        printf("Failed to create event flags\n");
    }

    /* Create threads */
    osal_thread_attr_t t1Attr = {
        .name = "Thread1",
        .stack_size = 4096,
        .priority = OSAL_THREAD_PRIORITY_NORMAL};
    osal_thread_handle_t t1 = osal_thread_create(thread1Func, NULL, &t1Attr);
    if (!t1)
    {
        printf("Failed to create Thread 1\n");
    }

    osal_thread_attr_t t2Attr = {
        .name = "Thread2",
        .stack_size = 4096,
        .priority = OSAL_THREAD_PRIORITY_NORMAL};
    osal_thread_handle_t t2 = osal_thread_create(thread2Func, NULL, &t2Attr);
    if (!t2)
    {
        printf("Failed to create Thread 2\n");
    }

    /* Main thread waits */
    osal_delay_ms(3000);

    /* ISR API Test (Simulated) */
    printf("Testing ISR APIs (Abstracted)...\n");
    osal_semaphore_give(Sem);
    uint32_t val = 123;
    osal_message_queue_send(Queue, &val, OSAL_NO_WAIT);
    osal_event_flags_set(Flags, 0x02);
    printf("ISR APIs called successfully\n");

    /* Clean up */
    osal_thread_delete(t1);
    osal_thread_delete(t2);
    osal_mutex_delete(Mutex);
    osal_semaphore_delete(Sem);
    osal_message_queue_delete(Queue);
    osal_event_flags_delete(Flags);

    printf("Example finished\n");
    return 0;
}
