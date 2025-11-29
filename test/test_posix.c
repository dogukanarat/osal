/* test_posix.c - Comprehensive Unit Tests for OSAL POSIX (Unity Version) */

#include "osal/osalMutex.h"
#include "osal/osalSemaphore.h"
#include "osal/osalMessageQueue.h"
#include "osal/osalTime.h"
#include "osal/osalTypes.h"
#include "unity/unity.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>

/* --- MOCK CONTROL --- */
static int mock_mutex_init_ret = 0;
static int mock_mutex_lock_ret = 0;
static int mock_clock_gettime_ret = 0;
static int mock_sem_init_ret = 0;
static int mock_sem_wait_ret = 0;
static int mock_sem_timedwait_ret = 0;
static int mock_pthread_create_ret = 0;
static int mock_cond_init_ret = 0;
static int mock_cond_wait_ret = 0;
static int mock_cond_timedwait_ret = 0;

static struct timespec mock_current_time = {1000, 0}; /* Start at 1000s */

void reset_mocks(void)
{
    mock_mutex_init_ret = 0;
    mock_mutex_lock_ret = 0;
    mock_clock_gettime_ret = 0;
    mock_sem_init_ret = 0;
    mock_sem_wait_ret = 0;
    mock_sem_timedwait_ret = 0;
    mock_pthread_create_ret = 0;
    mock_cond_init_ret = 0;
    mock_cond_wait_ret = 0;
    mock_cond_timedwait_ret = 0;
    mock_current_time.tv_sec = 1000;
    mock_current_time.tv_nsec = 0;
}

/* --- MOCK IMPLEMENTATIONS --- */

int __wrap_clock_gettime(clockid_t clock_id, struct timespec *tp)
{
    (void)clock_id;
    if (mock_clock_gettime_ret == 0)
    {
        *tp = mock_current_time;
        return 0;
    }
    return -1;
}

int __wrap_nanosleep(const struct timespec *req, struct timespec *rem)
{
    (void)rem;
    mock_current_time.tv_sec += req->tv_sec;
    mock_current_time.tv_nsec += req->tv_nsec;
    if (mock_current_time.tv_nsec >= 1000000000)
    {
        mock_current_time.tv_sec += 1;
        mock_current_time.tv_nsec -= 1000000000;
    }
    return 0;
}

int __wrap_pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr)
{
    (void)mutex; (void)attr;
    return mock_mutex_init_ret;
}

int __wrap_pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    (void)mutex;
    return 0;
}

int __wrap_pthread_mutex_lock(pthread_mutex_t *mutex)
{
    (void)mutex;
    return mock_mutex_lock_ret;
}

int __wrap_pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    (void)mutex;
    return 0;
}

int __wrap_sem_init(sem_t *sem, int pshared, unsigned int value)
{
    (void)sem; (void)pshared; (void)value;
    return (mock_sem_init_ret == 0) ? 0 : -1;
}

int __wrap_sem_destroy(sem_t *sem)
{
    (void)sem;
    return 0;
}

int __wrap_sem_wait(sem_t *sem)
{
    (void)sem;
    if (mock_sem_wait_ret != 0)
    {
        errno = mock_sem_wait_ret;
        return -1;
    }
    return 0;
}

int __wrap_sem_timedwait(sem_t *sem, const struct timespec *abs_timeout)
{
    (void)sem; (void)abs_timeout;
    if (mock_sem_timedwait_ret != 0)
    {
        errno = mock_sem_timedwait_ret;
        return -1;
    }
    return 0;
}

int __wrap_sem_post(sem_t *sem)
{
    (void)sem;
    return 0;
}

int __wrap_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg)
{
    (void)thread; (void)attr;
    if (mock_pthread_create_ret == 0)
    {
        return 0;
    }
    return mock_pthread_create_ret;
}

int __wrap_pthread_join(pthread_t thread, void **retval)
{
    (void)thread; (void)retval;
    return 0;
}

int __wrap_pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
{
    (void)cond; (void)attr;
    return mock_cond_init_ret;
}

int __wrap_pthread_cond_destroy(pthread_cond_t *cond)
{
    (void)cond;
    return 0;
}

int __wrap_pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    (void)cond; (void)mutex;
    return mock_cond_wait_ret;
}

int __wrap_pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                                  const struct timespec *abstime)
{
    (void)cond; (void)mutex; (void)abstime;
    return mock_cond_timedwait_ret;
}

int __wrap_pthread_cond_signal(pthread_cond_t *cond)
{
    (void)cond;
    return 0;
}

int __wrap_pthread_cond_broadcast(pthread_cond_t *cond)
{
    (void)cond;
    return 0;
}

/* --- TEST CASES --- */

void test_mutex(void)
{
    printf("[Mutex] Running tests...\n");
    reset_mocks();

    osalMutexHandle_t m = osalMutexCreate(NULL);
    TEST_ASSERT_NOT_NULL(m);

    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osalMutexLock(m, OSAL_WAIT_FOREVER));
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osalMutexUnlock(m));

    mock_mutex_lock_ret = EINVAL;
    TEST_ASSERT_EQUAL(OSAL_ERROR, osalMutexLock(m, OSAL_WAIT_FOREVER));
    mock_mutex_lock_ret = 0;

    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osalMutexDelete(m));

    mock_mutex_init_ret = ENOMEM;
    TEST_ASSERT_NULL(osalMutexCreate(NULL));

    printf("[Mutex] Passed\n");
}

void test_semaphore(void)
{
    printf("[Semaphore] Running tests...\n");
    reset_mocks();

    osalSemaphoreAttr_t attr = {0};
    attr.name = "Sem";
    attr.maxCount = 10;
    attr.initialCount = 0;

    osalSemaphoreHandle_t s = osalSemaphoreCreate(&attr);
    TEST_ASSERT_NOT_NULL(s);

    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osalSemaphoreTake(s, OSAL_WAIT_FOREVER));
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osalSemaphoreGive(s));

    mock_sem_timedwait_ret = ETIMEDOUT;
    TEST_ASSERT_EQUAL(OSAL_ERROR_TIMEOUT, osalSemaphoreTake(s, 100));
    mock_sem_timedwait_ret = 0;

    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osalSemaphoreDelete(s));

    mock_sem_init_ret = -1;
    TEST_ASSERT_NULL(osalSemaphoreCreate(&attr));

    printf("[Semaphore] Passed\n");
}

void test_queue(void)
{
    printf("[Queue] Running tests...\n");
    reset_mocks();

    osalQueueHandle_t q = osalQueueCreate(5, sizeof(int), NULL);
    TEST_ASSERT_NOT_NULL(q);

    int val = 42;
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osalQueueSend(q, &val, OSAL_NO_WAIT));

    int rxVal = 0;
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osalQueueReceive(q, &rxVal, OSAL_WAIT_FOREVER));

    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osalQueueDelete(q));

    printf("[Queue] Passed\n");
}

void test_time(void)
{
    printf("[Time] Running tests...\n");
    reset_mocks();

    TEST_ASSERT_EQUAL(1000000, osalGetTickMs());

    osalDelayMs(500);
    TEST_ASSERT_EQUAL(1000500, osalGetTickMs());

    printf("[Time] Passed\n");
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_mutex);
    RUN_TEST(test_semaphore);
    RUN_TEST(test_queue);
    RUN_TEST(test_time);

    printf("All POSIX tests passed!\n");
    return UNITY_END();
}
