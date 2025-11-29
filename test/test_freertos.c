/* test_freertos.c - Unit Tests for OSAL FreeRTOS */

#include "osal/osalMutex.h"
#include "osal/osalSemaphore.h"
#include "osal/osalMessageQueue.h"
#include "osal/osalTime.h"
#include "osal/osalTypes.h"
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

    osalMutexHandle_t m = osalMutexCreate(NULL);
    TEST_ASSERT_NOT_NULL(m);

    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osalMutexLock(m, 100));
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osalMutexUnlock(m));

    /* Test Failure */
    mock_rtos_fail_take = 1;
    TEST_ASSERT_EQUAL(OSAL_ERROR_TIMEOUT, osalMutexLock(m, 100));

    osalMutexDelete(m);
    printf("[FreeRTOS Mutex] Passed\n");
}

void test_isr_detection(void)
{
    printf("[FreeRTOS ISR] Running tests...\n");
    reset_mocks();

    osalSemaphoreAttr_t attr = {0};
    osalSemaphoreHandle_t s = osalSemaphoreCreate(&attr);
    TEST_ASSERT_NOT_NULL(s);

    /* Normal Context */
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osalSemaphoreGive(s));

    /* ISR Context */
    mock_rtos_inside_isr = 1;
    /* This calls xSemaphoreGiveFromISR internally */
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, osalSemaphoreGive(s));

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
