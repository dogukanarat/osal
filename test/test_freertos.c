/* test_freertos.c - Unit Tests for OSAL FreeRTOS */

#include "osal/osal_mutex.h"
#include "osal/osal_semaphore.h"
#include "osal/osal_message_queue.h"
#include "osal/osal_time.h"
#include "osal/osal_types.h"
#include "unity/unity.h"
#include <stdio.h>

/* Mock Control Variables (extern from mock_freertos.c) */
extern int mock_rtos_fail_create;
extern int mock_rtos_fail_take;
extern int mock_rtos_fail_give;
extern int mock_rtos_inside_isr;

void reset_mocks(void)
{
    mock_rtos_fail_create = 0;
    mock_rtos_fail_take = 0;
    mock_rtos_fail_give = 0;
    mock_rtos_inside_isr = 0;
}

void test_mutex(void)
{
    printf("[FreeRTOS Mutex] Running tests...\n");
    reset_mocks();

    osal_mutex_handle_t m = osal_mutex_create(NULL);
    TEST_ASSERT_NOT_NULL(m);

    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osal_mutex_lock(m, 100));
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osal_mutex_unlock(m));

    /* Test Failure */
    mock_rtos_fail_take = 1;
    TEST_ASSERT_EQUAL(OSAL_ERROR_TIMEOUT, osal_mutex_lock(m, 100));

    osal_mutex_delete(m);
    printf("[FreeRTOS Mutex] Passed\n");
}

void test_isr_detection(void)
{
    printf("[FreeRTOS ISR] Running tests...\n");
    reset_mocks();

    osal_semaphore_attr_t attr = {0};
    osal_semaphore_handle_t s = osal_semaphore_create(&attr);
    TEST_ASSERT_NOT_NULL(s);

    /* Normal Context */
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osal_semaphore_give(s));

    /* ISR Context */
    mock_rtos_inside_isr = 1;
    /* This calls xSemaphoreGiveFromISR internally */
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osal_semaphore_give(s));

    printf("[FreeRTOS ISR] Passed\n");
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_mutex);
    RUN_TEST(test_isr_detection);

    printf("All FreeRTOS tests passed!\n");
    return UNITY_END();
}
