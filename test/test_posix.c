/* test_posix.c - Comprehensive Unit Tests for OSAL POSIX */

#include "osal/osal_mutex.h"
#include "osal/osal_semaphore.h"
#include "osal/osal_message_queue.h"
#include "osal/osal_time.h"
#include "osal/osal_types.h"
#include "unity.h" /* */
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

static struct timespec mock_current_time = {1000, 0};

void setUp(void)
{
    /* Reset mocks before each test */
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

void tearDown(void)
{
    /* Cleanup if needed */
}

/* --- MOCK IMPLEMENTATIONS (Wrappers) --- */

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

int __wrap_pthread_mutex_destroy(pthread_mutex_t *mutex) { (void)mutex; return 0; }

int __wrap_pthread_mutex_lock(pthread_mutex_t *mutex)
{
    (void)mutex;
    return mock_mutex_lock_ret;
}

int __wrap_pthread_mutex_unlock(pthread_mutex_t *mutex) { (void)mutex; return 0; }

int __wrap_sem_init(sem_t *sem, int pshared, unsigned int value)
{
    (void)sem; (void)pshared; (void)value;
    return (mock_sem_init_ret == 0) ? 0 : -1;
}

int __wrap_sem_destroy(sem_t *sem) { (void)sem; return 0; }

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

int __wrap_sem_post(sem_t *sem) { (void)sem; return 0; }

int __wrap_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine) (void *), void *arg)
{
    (void)thread; (void)attr; (void)start_routine; (void)arg;
    return mock_pthread_create_ret;
}

int __wrap_pthread_join(pthread_t thread, void **retval) { (void)thread; (void)retval; return 0; }
int __wrap_pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) { (void)cond; (void)attr; return mock_cond_init_ret; }
int __wrap_pthread_cond_destroy(pthread_cond_t *cond) { (void)cond; return 0; }
int __wrap_pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) { (void)cond; (void)mutex; return mock_cond_wait_ret; }
int __wrap_pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime) { (void)cond; (void)mutex; (void)abstime; return mock_cond_timedwait_ret; }
int __wrap_pthread_cond_signal(pthread_cond_t *cond) { (void)cond; return 0; }
int __wrap_pthread_cond_broadcast(pthread_cond_t *cond) { (void)cond; return 0; }


/* --- TEST CASES --- */

void test_mutex(void)
{
    /* 1. Create Success */
    osal_mutex_handle_t m = osal_mutex_create(NULL);
    TEST_ASSERT_NOT_NULL(m);

    /* 2. Lock Success */
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osal_mutex_lock(m, OSAL_WAIT_FOREVER));
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osal_mutex_unlock(m));

    /* 3. Lock Fail (Simulated Error) */
    mock_mutex_lock_ret = ETIMEDOUT;
    TEST_ASSERT_EQUAL(OSAL_ERROR_TIMEOUT, osal_mutex_lock(m, OSAL_WAIT_FOREVER));

    /* 4. Delete */
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osal_mutex_delete(m));
}

void test_semaphore(void)
{
    osal_semaphore_attr_t attr = { .name = "Sem", .max_count = 10, .initial_count = 0 };

    /* 1. Create Success */
    osal_semaphore_handle_t s = osal_semaphore_create(&attr);
    TEST_ASSERT_NOT_NULL(s);

    /* 2. Take Success */
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osal_semaphore_take(s, OSAL_WAIT_FOREVER));

    /* 3. Give Success */
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osal_semaphore_give(s));

    /* 4. Take Timeout */
    mock_sem_timedwait_ret = ETIMEDOUT;
    TEST_ASSERT_EQUAL(OSAL_ERROR_TIMEOUT, osal_semaphore_take(s, 100));

    /* 5. Delete */
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osal_semaphore_delete(s));
}

void test_queue(void)
{
    /* 1. Create Success */
    osal_message_queue_handle_t q = osal_message_queue_create(5, sizeof(int), NULL);
    TEST_ASSERT_NOT_NULL(q);

    /* 2. Send Success */
    int val = 42;
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osal_message_queue_send(q, &val, OSAL_NO_WAIT));

    /* 3. Receive Success */
    int rxVal = 0;
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osal_message_queue_receive(q, &rxVal, OSAL_WAIT_FOREVER));

    /* 4. Delete */
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osal_message_queue_delete(q));
}

void test_time(void)
{
    /* 1. Get Tick */
    TEST_ASSERT_EQUAL_UINT32(1000000, osal_get_tick_ms());

    /* 2. Delay */
    osal_delay_ms(500);
    TEST_ASSERT_EQUAL_UINT32(1000500, osal_get_tick_ms());
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_mutex);
    RUN_TEST(test_semaphore);
    RUN_TEST(test_queue);
    RUN_TEST(test_time);
    return UNITY_END();
}
