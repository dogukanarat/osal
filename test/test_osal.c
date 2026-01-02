/* test_osal.c - Cross-Platform Integration Tests for OSAL */

/* All Rights Reserved */

/* Includes */
#include "osal/osal_memory.h"
#include "osal/osal_time.h"
#include "osal/osal_mutex.h"
#include "osal/osal_semaphore.h"
#include "osal/osal_message_queue.h"
#include "osal/osal_event_flags.h"
#include "osal/osal_thread.h"
#include "unity/unity.h"
#include <string.h>
#include <stdio.h>

/* Test Helpers */
#define TEST_TIMEOUT_MS 5000
#define TEST_SHORT_DELAY_MS 10
#define TEST_QUEUE_DEPTH 10

/* Unity Setup/Teardown */
void setUp(void)
{
    /* Called before each test */
}

void tearDown(void)
{
    /* Called after each test */
}

/* ============================================================================
 * Memory Tests
 * ============================================================================ */

void test_memory_alloc_free(void)
{
    void *ptr = osal_memory_alloc(1024);
    TEST_ASSERT_NOT_NULL(ptr);

    /* Write to ensure it's usable */
    memset(ptr, 0xAA, 1024);

    osal_memory_free(ptr);
}

void test_memory_multiple_allocations(void)
{
    void *ptrs[10];

    for (int i = 0; i < 10; i++)
    {
        ptrs[i] = osal_memory_alloc(256);
        TEST_ASSERT_NOT_NULL(ptrs[i]);
    }

    for (int i = 0; i < 10; i++)
    {
        osal_memory_free(ptrs[i]);
    }
}

/* ============================================================================
 * Time Tests
 * ============================================================================ */

void test_time_delay(void)
{
    uint32_t start = osal_get_tick_ms();
    osal_delay_ms(50);
    uint32_t end = osal_get_tick_ms();

    uint32_t elapsed = end - start;
    /* Allow 20ms tolerance */
    TEST_ASSERT_UINT32_WITHIN(20, 50, elapsed);
}

void test_time_get_tick(void)
{
    uint32_t tick1 = osal_get_tick_ms();
    osal_delay_ms(10);
    uint32_t tick2 = osal_get_tick_ms();

    TEST_ASSERT_GREATER_THAN(tick1, tick2);
}

void test_time_get_unix_time(void)
{
    osal_time_val_t tv;
    osal_status_t status = osal_get_unix_time(&tv);

    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);
    /* Unix time should be > 2020-01-01 (1577836800) */
    TEST_ASSERT_GREATER_THAN(1577836800, tv.sec);
}

/* ============================================================================
 * Mutex Tests
 * ============================================================================ */

void test_mutex_create_delete(void)
{
    osal_mutex_handle_t mutex = osal_mutex_create(NULL);
    TEST_ASSERT_NOT_NULL(mutex);

    osal_status_t status = osal_mutex_delete(mutex);
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);
}

void test_mutex_lock_unlock(void)
{
    osal_mutex_handle_t mutex = osal_mutex_create(NULL);
    TEST_ASSERT_NOT_NULL(mutex);

    osal_status_t status = osal_mutex_lock(mutex, OSAL_WAIT_FOREVER);
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);

    status = osal_mutex_unlock(mutex);
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);

    osal_mutex_delete(mutex);
}

void test_mutex_lock_timeout(void)
{
    osal_mutex_handle_t mutex = osal_mutex_create(NULL);
    TEST_ASSERT_NOT_NULL(mutex);

    /* Try lock with no wait should succeed */
    osal_status_t status = osal_mutex_lock(mutex, OSAL_NO_WAIT);
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);

    osal_mutex_unlock(mutex);
    osal_mutex_delete(mutex);
}

static osal_mutex_handle_t g_sharedMutex;
static int g_sharedCounter = 0;

static void mutexThreadFunc(void *arg)
{
    (void)arg;

    for (int i = 0; i < 100; i++)
    {
        osal_mutex_lock(g_sharedMutex, OSAL_WAIT_FOREVER);
        g_sharedCounter++;
        osal_mutex_unlock(g_sharedMutex);
    }
}

void test_mutex_thread_safety(void)
{
    g_sharedMutex = osal_mutex_create(NULL);
    TEST_ASSERT_NOT_NULL(g_sharedMutex);
    g_sharedCounter = 0;

    osal_thread_attr_t attr = {
        .name = "TestThread",
        .stack_size = 4096,
        .priority = OSAL_THREAD_PRIORITY_NORMAL
    };

    osal_thread_handle_t t1 = osal_thread_create(mutexThreadFunc, NULL, &attr);
    osal_thread_handle_t t2 = osal_thread_create(mutexThreadFunc, NULL, &attr);

    TEST_ASSERT_NOT_NULL(t1);
    TEST_ASSERT_NOT_NULL(t2);

    /* Wait for threads to complete */
    osal_delay_ms(500);

    /* Counter should be exactly 200 if mutex works */
    TEST_ASSERT_EQUAL_INT(200, g_sharedCounter);

    osal_thread_delete(t1);
    osal_thread_delete(t2);
    osal_mutex_delete(g_sharedMutex);
}

/* ============================================================================
 * Semaphore Tests
 * ============================================================================ */

void test_semaphore_create_delete(void)
{
    osal_semaphore_attr_t attr = {
        .name = "TestSem",
        .max_count = 10,
        .initial_count = 5
    };

    osal_semaphore_handle_t sem = osal_semaphore_create(&attr);
    TEST_ASSERT_NOT_NULL(sem);

    osal_status_t status = osal_semaphore_delete(sem);
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);
}

void test_semaphore_give_take(void)
{
    osal_semaphore_attr_t attr = {
        .initial_count = 0
    };

    osal_semaphore_handle_t sem = osal_semaphore_create(&attr);
    TEST_ASSERT_NOT_NULL(sem);

    /* Give semaphore */
    osal_status_t status = osal_semaphore_give(sem);
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);

    /* Take semaphore - should succeed */
    status = osal_semaphore_take(sem, OSAL_NO_WAIT);
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);

    osal_semaphore_delete(sem);
}

void test_semaphore_timeout(void)
{
    osal_semaphore_attr_t attr = {
        .initial_count = 0
    };

    osal_semaphore_handle_t sem = osal_semaphore_create(&attr);
    TEST_ASSERT_NOT_NULL(sem);

    /* Take with no wait should timeout */
    osal_status_t status = osal_semaphore_take(sem, OSAL_NO_WAIT);
    TEST_ASSERT_EQUAL(OSAL_ERROR_TIMEOUT, status);

    /* Take with short timeout should timeout */
    status = osal_semaphore_take(sem, 10);
    TEST_ASSERT_EQUAL(OSAL_ERROR_TIMEOUT, status);

    osal_semaphore_delete(sem);
}

static osal_semaphore_handle_t g_testSem;

static void semProducerThread(void *arg)
{
    (void)arg;
    osal_delay_ms(50);
    osal_semaphore_give(g_testSem);
}

void test_semaphore_thread_signaling(void)
{
    osal_semaphore_attr_t attr = {
        .initial_count = 0
    };

    g_testSem = osal_semaphore_create(&attr);
    TEST_ASSERT_NOT_NULL(g_testSem);

    osal_thread_attr_t thread_attr = {
        .name = "Producer",
        .stack_size = 4096,
        .priority = OSAL_THREAD_PRIORITY_NORMAL
    };

    osal_thread_handle_t thread = osal_thread_create(semProducerThread, NULL, &thread_attr);
    TEST_ASSERT_NOT_NULL(thread);

    /* This will block until producer gives semaphore */
    uint32_t start = osal_get_tick_ms();
    osal_status_t status = osal_semaphore_take(g_testSem, TEST_TIMEOUT_MS);
    uint32_t elapsed = osal_get_tick_ms() - start;

    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);
    TEST_ASSERT_UINT32_WITHIN(30, 50, elapsed);

    osal_thread_delete(thread);
    osal_semaphore_delete(g_testSem);
}

/* ============================================================================
 * Message Queue Tests
 * ============================================================================ */

void test_queue_create_delete(void)
{
    osal_message_queue_handle_t queue = osal_message_queue_create(10, sizeof(int), NULL);
    TEST_ASSERT_NOT_NULL(queue);

    osal_status_t status = osal_message_queue_delete(queue);
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);
}

void test_queue_send_receive(void)
{
    osal_message_queue_handle_t queue = osal_message_queue_create(10, sizeof(int), NULL);
    TEST_ASSERT_NOT_NULL(queue);

    int sendValue = 42;
    osal_status_t status = osal_message_queue_send(queue, &sendValue, OSAL_NO_WAIT);
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);

    int recvValue = 0;
    status = osal_message_queue_receive(queue, &recvValue, OSAL_NO_WAIT);
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);
    TEST_ASSERT_EQUAL_INT(42, recvValue);

    osal_message_queue_delete(queue);
}

void test_queue_multiple_messages(void)
{
    osal_message_queue_handle_t queue = osal_message_queue_create(5, sizeof(int), NULL);
    TEST_ASSERT_NOT_NULL(queue);

    /* Send multiple messages */
    for (int i = 0; i < 5; i++)
    {
        osal_status_t status = osal_message_queue_send(queue, &i, OSAL_WAIT_FOREVER);
        TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);
    }

    /* Receive and verify */
    for (int i = 0; i < 5; i++)
    {
        int value;
        osal_status_t status = osal_message_queue_receive(queue, &value, OSAL_WAIT_FOREVER);
        TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);
        TEST_ASSERT_EQUAL_INT(i, value);
    }

    osal_message_queue_delete(queue);
}

void test_queue_count(void)
{
    osal_message_queue_handle_t queue = osal_message_queue_create(10, sizeof(int), NULL);
    TEST_ASSERT_NOT_NULL(queue);

    TEST_ASSERT_EQUAL_UINT32(0, osal_message_queue_get_count(queue));

    int value = 1;
    osal_message_queue_send(queue, &value, OSAL_NO_WAIT);
    TEST_ASSERT_EQUAL_UINT32(1, osal_message_queue_get_count(queue));

    osal_message_queue_send(queue, &value, OSAL_NO_WAIT);
    TEST_ASSERT_EQUAL_UINT32(2, osal_message_queue_get_count(queue));

    osal_message_queue_receive(queue, &value, OSAL_NO_WAIT);
    TEST_ASSERT_EQUAL_UINT32(1, osal_message_queue_get_count(queue));

    osal_message_queue_delete(queue);
}

/* ============================================================================
 * Event Flags Tests
 * ============================================================================ */

void test_eventflags_create_delete(void)
{
    osal_event_flags_handle_t flags = osal_event_flags_create(NULL);
    TEST_ASSERT_NOT_NULL(flags);

    osal_status_t status = osal_event_flags_delete(flags);
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);
}

void test_eventflags_set_clear(void)
{
    osal_event_flags_handle_t flags = osal_event_flags_create(NULL);
    TEST_ASSERT_NOT_NULL(flags);

    /* Set flags */
    uint32_t result = osal_event_flags_set(flags, 0x03);
    TEST_ASSERT_EQUAL_UINT32(0x03, result);

    /* Get flags */
    result = osal_event_flags_get(flags);
    TEST_ASSERT_EQUAL_UINT32(0x03, result);

    /* Clear flags */
    result = osal_event_flags_clear(flags, 0x01);
    TEST_ASSERT_EQUAL_UINT32(0x03, result); /* Returns previous value */

    result = osal_event_flags_get(flags);
    TEST_ASSERT_EQUAL_UINT32(0x02, result);

    osal_event_flags_delete(flags);
}

void test_eventflags_wait_any(void)
{
    osal_event_flags_handle_t flags = osal_event_flags_create(NULL);
    TEST_ASSERT_NOT_NULL(flags);

    osal_event_flags_set(flags, 0x05);

    /* Wait for any flag in 0x07 */
    uint32_t result = osal_event_flags_wait(flags, 0x07, OSAL_EVENT_WAIT_ANY, OSAL_NO_WAIT);
    TEST_ASSERT_EQUAL_UINT32(0x05, result);

    osal_event_flags_delete(flags);
}

void test_eventflags_wait_all(void)
{
    osal_event_flags_handle_t flags = osal_event_flags_create(NULL);
    TEST_ASSERT_NOT_NULL(flags);

    osal_event_flags_set(flags, 0x07);

    /* Wait for all flags */
    uint32_t result = osal_event_flags_wait(flags, 0x07, OSAL_EVENT_WAIT_ALL, OSAL_NO_WAIT);
    TEST_ASSERT_EQUAL_UINT32(0x07, result);

    osal_event_flags_delete(flags);
}

static osal_event_flags_handle_t g_testFlags;

static void eventFlagSetterThread(void *arg)
{
    (void)arg;
    osal_delay_ms(50);
    osal_event_flags_set(g_testFlags, 0x01);
}

void test_eventflags_thread_signaling(void)
{
    g_testFlags = osal_event_flags_create(NULL);
    TEST_ASSERT_NOT_NULL(g_testFlags);

    osal_thread_attr_t attr = {
        .name = "FlagSetter",
        .stack_size = 4096,
        .priority = OSAL_THREAD_PRIORITY_NORMAL
    };

    osal_thread_handle_t thread = osal_thread_create(eventFlagSetterThread, NULL, &attr);
    TEST_ASSERT_NOT_NULL(thread);

    /* Wait for flag to be set by thread */
    uint32_t start = osal_get_tick_ms();
    uint32_t result = osal_event_flags_wait(g_testFlags, 0x01, OSAL_EVENT_WAIT_ANY, TEST_TIMEOUT_MS);
    uint32_t elapsed = osal_get_tick_ms() - start;

    TEST_ASSERT_EQUAL_UINT32(0x01, result);
    TEST_ASSERT_UINT32_WITHIN(30, 50, elapsed);

    osal_thread_delete(thread);
    osal_event_flags_delete(g_testFlags);
}

/* ============================================================================
 * Thread Tests
 * ============================================================================ */

static volatile int g_threadExecuted = 0;

static void simpleThreadFunc(void *arg)
{
    (void)arg;
    g_threadExecuted = 1;
}

void test_thread_create_delete(void)
{
    g_threadExecuted = 0;

    osal_thread_attr_t attr = {
        .name = "TestThread",
        .stack_size = 4096,
        .priority = OSAL_THREAD_PRIORITY_NORMAL
    };

    osal_thread_handle_t thread = osal_thread_create(simpleThreadFunc, NULL, &attr);
    TEST_ASSERT_NOT_NULL(thread);

    osal_delay_ms(50);
    TEST_ASSERT_EQUAL_INT(1, g_threadExecuted);

    osal_status_t status = osal_thread_delete(thread);
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);
}

static void yieldThreadFunc(void *arg)
{
    (void)arg;
    for (int i = 0; i < 10; i++)
    {
        osal_thread_yield();
    }
}

void test_thread_yield(void)
{
    osal_thread_attr_t attr = {
        .name = "YieldThread",
        .stack_size = 4096,
        .priority = OSAL_THREAD_PRIORITY_NORMAL
    };

    osal_thread_handle_t thread = osal_thread_create(yieldThreadFunc, NULL, &attr);
    TEST_ASSERT_NOT_NULL(thread);

    osal_delay_ms(50);

    osal_thread_delete(thread);
}

/* ============================================================================
 * Main Test Runner
 * ============================================================================ */

int main(void)
{
    UNITY_BEGIN();

    /* Memory Tests */
    RUN_TEST(test_memory_alloc_free);
    RUN_TEST(test_memory_multiple_allocations);

    /* Time Tests */
    RUN_TEST(test_time_delay);
    RUN_TEST(test_time_get_tick);
    RUN_TEST(test_time_get_unix_time);

    /* Mutex Tests */
    RUN_TEST(test_mutex_create_delete);
    RUN_TEST(test_mutex_lock_unlock);
    RUN_TEST(test_mutex_lock_timeout);
    RUN_TEST(test_mutex_thread_safety);

    /* Semaphore Tests */
    RUN_TEST(test_semaphore_create_delete);
    RUN_TEST(test_semaphore_give_take);
    RUN_TEST(test_semaphore_timeout);
    RUN_TEST(test_semaphore_thread_signaling);

    /* Message Queue Tests */
    RUN_TEST(test_queue_create_delete);
    RUN_TEST(test_queue_send_receive);
    RUN_TEST(test_queue_multiple_messages);
    RUN_TEST(test_queue_count);

    /* Event Flags Tests */
    RUN_TEST(test_eventflags_create_delete);
    RUN_TEST(test_eventflags_set_clear);
    RUN_TEST(test_eventflags_wait_any);
    RUN_TEST(test_eventflags_wait_all);
    RUN_TEST(test_eventflags_thread_signaling);

    /* Thread Tests */
    RUN_TEST(test_thread_create_delete);
    RUN_TEST(test_thread_yield);

    return UNITY_END();
}
