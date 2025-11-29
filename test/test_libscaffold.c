/* test_libscaffold.c - Unit tests for libscaffold */

/* All Rights Reserved */

/* Includes */

#include "libscaffold/libscaffold.h"
#include "unity.h"

/* Test Setup and Teardown */

void setUp(void)
{
    /* This is run before EACH test */
}

void tearDown(void)
{
    /* This is run after EACH test */
}

/* Test Cases */

void test_example_should_pass(void)
{
    TEST_ASSERT_TRUE(1);
}

void test_example_should_fail_when_disabled(void)
{
    TEST_IGNORE_MESSAGE("Remove this test or implement real tests");
}

/* Add your test functions here */
/* Example:
void test_myFunction_should_returnSuccess(void)
{
    int result = myFunction();
    TEST_ASSERT_EQUAL(0, result);
}
*/

/* Main Test Runner */

int main(void)
{
    UNITY_BEGIN();

    /* Register your tests here */
    RUN_TEST(test_example_should_pass);
    RUN_TEST(test_example_should_fail_when_disabled);

    return UNITY_END();
}
