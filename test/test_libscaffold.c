/* test_libscaffold.c - Unit tests for libscaffold */

/* All Rights Reserved */

/* Includes */

#include "libscaffold/libscaffold.h"
#include "unity.h"
#include <string.h>

/* Test Setup and Teardown */

void setUp(void)
{
    /* This is run before EACH test */
}

void tearDown(void)
{
    /* This is run after EACH test */
}

/* Test Cases for libscaffoldGetVersion */

void test_getVersion_should_returnVersionString(void)
{
    const char *version = libscaffold_get_version();
    TEST_ASSERT_NOT_NULL(version);
    TEST_ASSERT_EQUAL_STRING("1.0.0", version);
}

/* Test Cases for libscaffoldAdd */

void test_add_should_returnSum_when_addingPositiveNumbers(void)
{
    int32_t result = libscaffold_add(5, 3);
    TEST_ASSERT_EQUAL_INT32(8, result);
}

void test_add_should_returnSum_when_addingNegativeNumbers(void)
{
    int32_t result = libscaffold_add(-5, -3);
    TEST_ASSERT_EQUAL_INT32(-8, result);
}

void test_add_should_returnSum_when_addingMixedNumbers(void)
{
    int32_t result = libscaffold_add(10, -5);
    TEST_ASSERT_EQUAL_INT32(5, result);
}

void test_add_should_returnZero_when_addingZeros(void)
{
    int32_t result = libscaffold_add(0, 0);
    TEST_ASSERT_EQUAL_INT32(0, result);
}

/* Test Cases for libscaffoldMultiply */

void test_multiply_should_returnSuccess_when_multiplyingPositiveNumbers(void)
{
    int32_t result;
    libscaffold_status_t status = libscaffold_multiply(5, 3, &result);
    TEST_ASSERT_EQUAL(LIBSCAFFOLD_SUCCESS, status);
    TEST_ASSERT_EQUAL_INT32(15, result);
}

void test_multiply_should_returnSuccess_when_multiplyingByZero(void)
{
    int32_t result;
    libscaffold_status_t status = libscaffold_multiply(5, 0, &result);
    TEST_ASSERT_EQUAL(LIBSCAFFOLD_SUCCESS, status);
    TEST_ASSERT_EQUAL_INT32(0, result);
}

void test_multiply_should_returnErrorNull_when_resultPointerIsNull(void)
{
    libscaffold_status_t status = libscaffold_multiply(5, 3, NULL);
    TEST_ASSERT_EQUAL(LIBSCAFFOLD_ERROR_NULL, status);
}

/* Test Cases for libscaffoldFoo */

void test_foo_should_returnSuccess_when_processingValidInput(void)
{
    char output[100];
    libscaffold_status_t status = libscaffold_foo("test", output, sizeof(output));
    TEST_ASSERT_EQUAL(LIBSCAFFOLD_SUCCESS, status);
    TEST_ASSERT_EQUAL_STRING("Processed: test", output);
}

void test_foo_should_returnErrorNull_when_inputIsNull(void)
{
    char output[100];
    libscaffold_status_t status = libscaffold_foo(NULL, output, sizeof(output));
    TEST_ASSERT_EQUAL(LIBSCAFFOLD_ERROR_NULL, status);
}

void test_foo_should_returnErrorNull_when_outputIsNull(void)
{
    libscaffold_status_t status = libscaffold_foo("test", NULL, 100);
    TEST_ASSERT_EQUAL(LIBSCAFFOLD_ERROR_NULL, status);
}

void test_foo_should_returnErrorInvalid_when_outputSizeIsZero(void)
{
    char output[100];
    libscaffold_status_t status = libscaffold_foo("test", output, 0);
    TEST_ASSERT_EQUAL(LIBSCAFFOLD_ERROR_INVALID, status);
}

void test_foo_should_returnErrorInvalid_when_bufferTooSmall(void)
{
    char output[5];
    libscaffold_status_t status = libscaffold_foo("test", output, sizeof(output));
    TEST_ASSERT_EQUAL(LIBSCAFFOLD_ERROR_INVALID, status);
}

/* Test Cases for libscaffoldBar */

void test_bar_should_returnTrue_when_valueIsInRange(void)
{
    TEST_ASSERT_TRUE(libscaffold_bar(50));
    TEST_ASSERT_TRUE(libscaffold_bar(0));
    TEST_ASSERT_TRUE(libscaffold_bar(100));
}

void test_bar_should_returnFalse_when_valueIsOutOfRange(void)
{
    TEST_ASSERT_FALSE(libscaffold_bar(-1));
    TEST_ASSERT_FALSE(libscaffold_bar(101));
    TEST_ASSERT_FALSE(libscaffold_bar(-100));
    TEST_ASSERT_FALSE(libscaffold_bar(200));
}

/* Test Cases for libscaffoldFactorial */

void test_factorial_should_returnCorrectValue_when_inputIsZero(void)
{
    libscaffold_result_t result = libscaffold_factorial(0);
    TEST_ASSERT_EQUAL(LIBSCAFFOLD_SUCCESS, result.status);
    TEST_ASSERT_EQUAL_INT32(1, result.value);
}

void test_factorial_should_returnCorrectValue_when_inputIsOne(void)
{
    libscaffold_result_t result = libscaffold_factorial(1);
    TEST_ASSERT_EQUAL(LIBSCAFFOLD_SUCCESS, result.status);
    TEST_ASSERT_EQUAL_INT32(1, result.value);
}

void test_factorial_should_returnCorrectValue_when_inputIsFive(void)
{
    libscaffold_result_t result = libscaffold_factorial(5);
    TEST_ASSERT_EQUAL(LIBSCAFFOLD_SUCCESS, result.status);
    TEST_ASSERT_EQUAL_INT32(120, result.value);
}

void test_factorial_should_returnCorrectValue_when_inputIsTen(void)
{
    libscaffold_result_t result = libscaffold_factorial(10);
    TEST_ASSERT_EQUAL(LIBSCAFFOLD_SUCCESS, result.status);
    TEST_ASSERT_EQUAL_INT32(3628800, result.value);
}

void test_factorial_should_returnErrorInvalid_when_inputIsNegative(void)
{
    libscaffold_result_t result = libscaffold_factorial(-1);
    TEST_ASSERT_EQUAL(LIBSCAFFOLD_ERROR_INVALID, result.status);
    TEST_ASSERT_EQUAL_INT32(0, result.value);
}

void test_factorial_should_returnErrorInvalid_when_inputIsTooLarge(void)
{
    libscaffold_result_t result = libscaffold_factorial(13);
    TEST_ASSERT_EQUAL(LIBSCAFFOLD_ERROR_INVALID, result.status);
    TEST_ASSERT_EQUAL_INT32(0, result.value);
}

/* Main Test Runner */

int main(void)
{
    UNITY_BEGIN();

    /* Version tests */
    RUN_TEST(test_getVersion_should_returnVersionString);

    /* Add function tests */
    RUN_TEST(test_add_should_returnSum_when_addingPositiveNumbers);
    RUN_TEST(test_add_should_returnSum_when_addingNegativeNumbers);
    RUN_TEST(test_add_should_returnSum_when_addingMixedNumbers);
    RUN_TEST(test_add_should_returnZero_when_addingZeros);

    /* Multiply function tests */
    RUN_TEST(test_multiply_should_returnSuccess_when_multiplyingPositiveNumbers);
    RUN_TEST(test_multiply_should_returnSuccess_when_multiplyingByZero);
    RUN_TEST(test_multiply_should_returnErrorNull_when_resultPointerIsNull);

    /* Foo function tests */
    RUN_TEST(test_foo_should_returnSuccess_when_processingValidInput);
    RUN_TEST(test_foo_should_returnErrorNull_when_inputIsNull);
    RUN_TEST(test_foo_should_returnErrorNull_when_outputIsNull);
    RUN_TEST(test_foo_should_returnErrorInvalid_when_outputSizeIsZero);
    RUN_TEST(test_foo_should_returnErrorInvalid_when_bufferTooSmall);

    /* Bar function tests */
    RUN_TEST(test_bar_should_returnTrue_when_valueIsInRange);
    RUN_TEST(test_bar_should_returnFalse_when_valueIsOutOfRange);

    /* Factorial function tests */
    RUN_TEST(test_factorial_should_returnCorrectValue_when_inputIsZero);
    RUN_TEST(test_factorial_should_returnCorrectValue_when_inputIsOne);
    RUN_TEST(test_factorial_should_returnCorrectValue_when_inputIsFive);
    RUN_TEST(test_factorial_should_returnCorrectValue_when_inputIsTen);
    RUN_TEST(test_factorial_should_returnErrorInvalid_when_inputIsNegative);
    RUN_TEST(test_factorial_should_returnErrorInvalid_when_inputIsTooLarge);

    return UNITY_END();
}

