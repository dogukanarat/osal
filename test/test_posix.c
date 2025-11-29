/* test_posix.c - Comprehensive Unit Tests for OSAL POSIX */

#include "osal/osalMutex.h"
#include "osal/osalSemaphore.h"
#include "osal/osalMessageQueue.h"
#include "osal/osalTime.h"
#include "osal/osalTypes.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>

/* --- MOCK CONTROL --- */
/* We use global variables to control the behavior of mocks */
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
    /* Advance mock time by sleep duration */
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
        /* We don't actually spawn a thread in unit tests usually, 
           unless we want to test concurrency. For now, just succeed. */
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

    /* 1. Create Success */
    osalMutexHandle_t m = osalMutexCreate(NULL);
    assert(m != NULL);

    /* 2. Lock Success */
    assert(osalMutexLock(m, OSAL_WAIT_FOREVER) == OSAL_SUCCESS);
    assert(osalMutexUnlock(m) == OSAL_SUCCESS);

    /* 3. Lock Fail (Simulated Error) */
    mock_mutex_lock_ret = EINVAL;
    assert(osalMutexLock(m, OSAL_WAIT_FOREVER) == OSAL_ERROR);
    mock_mutex_lock_ret = 0;

    /* 4. Delete */
    assert(osalMutexDelete(m) == OSAL_SUCCESS);

    /* 5. Create Fail */
    mock_mutex_init_ret = ENOMEM;
    assert(osalMutexCreate(NULL) == NULL);
    
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
    
    /* 1. Create Success */
    osalSemaphoreHandle_t s = osalSemaphoreCreate(&attr);
    assert(s != NULL);

    /* 2. Take Success */
    assert(osalSemaphoreTake(s, OSAL_WAIT_FOREVER) == OSAL_SUCCESS);

    /* 3. Give Success */
    assert(osalSemaphoreGive(s) == OSAL_SUCCESS);

    /* 4. Take Timeout */
    mock_sem_timedwait_ret = ETIMEDOUT;
    assert(osalSemaphoreTake(s, 100) == OSAL_ERROR_TIMEOUT);
    mock_sem_timedwait_ret = 0;

    /* 5. Delete */
    assert(osalSemaphoreDelete(s) == OSAL_SUCCESS);

    /* 6. Create Fail */
    mock_sem_init_ret = -1;
    assert(osalSemaphoreCreate(&attr) == NULL);

    printf("[Semaphore] Passed\n");
}

void test_queue(void)
{
    printf("[Queue] Running tests...\n");
    reset_mocks();

    /* 1. Create Success */
    osalQueueHandle_t q = osalQueueCreate(5, sizeof(int), NULL);
    assert(q != NULL);

    /* 2. Send Success */
    int val = 42;
    assert(osalQueueSend(q, &val, OSAL_NO_WAIT) == OSAL_SUCCESS);

    /* 3. Receive Success */
    int rxVal = 0;
    assert(osalQueueReceive(q, &rxVal, OSAL_WAIT_FOREVER) == OSAL_SUCCESS);
    /* Note: Since we mock cond_wait to return 0 immediately, and the queue logic 
       uses a real list (if implemented that way) or just mocks? 
       Wait, the POSIX implementation uses a linked list protected by mutex/cond.
       Since we mock mutex/cond to succeed, the logic *should* work if we don't interfere.
       However, `pthread_cond_wait` releases the mutex and waits. Our mock just returns.
       This might cause issues if the implementation expects the mutex to be re-acquired.
       Actually, `pthread_cond_wait` re-acquires mutex before returning.
       Our mock `pthread_cond_wait` does nothing. 
       The caller `osalQueueReceive` holds the mutex, calls `cond_wait` (which releases, waits, acquires).
       If our mock does nothing, the mutex is still held? No, `cond_wait` takes mutex as arg.
       If we do nothing, the mutex remains locked. The function proceeds.
       So it behaves as if the condition was signaled immediately.
       
       BUT: The queue implementation has a `while (count == 0)` loop.
       If we send first, count is 1. Receive should succeed immediately.
    */
    
    /* 4. Delete */
    assert(osalQueueDelete(q) == OSAL_SUCCESS);

    printf("[Queue] Passed\n");
}

void test_time(void)
{
    printf("[Time] Running tests...\n");
    reset_mocks();

    /* 1. Get Tick */
    /* Mock time is 1000s = 1,000,000 ms */
    assert(osalGetTickMs() == 1000000);

    /* 2. Delay */
    /* Delay 500ms */
    osalDelayMs(500);
    /* Mock nanosleep advances time. */
    assert(osalGetTickMs() == 1000500);

    printf("[Time] Passed\n");
}

int main(void)
{
    test_mutex();
    test_semaphore();
    test_queue();
    test_time();
    
    printf("All POSIX tests passed!\n");
    return 0;
}
