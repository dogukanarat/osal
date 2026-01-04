# Testing Guide

This directory contains unit tests for ScaffoldProject using the [Unity Test Framework](https://github.com/ThrowTheSwitch/Unity).

## Unity Test Framework

Unity is a lightweight unit testing framework for C, perfect for embedded systems and libraries. It's automatically downloaded via CMake FetchContent when you build with tests enabled.

## Building and Running Tests

### Build with Tests

```bash
mkdir build
cd build
cmake -DBUILD_TESTS=ON ..
make
```

### Run Tests

```bash
# Run all tests
ctest

# Run with verbose output
ctest --verbose

# Run specific test
./test_scaffold_project
```

## Writing Tests

### Test File Structure

```c
#include "scaffold_project/scaffold_project.h"
#include "unity.h"

void setUp(void)
{
    /* Run before each test */
}

void tearDown(void)
{
    /* Run after each test */
}

void test_my_function_should_return_success(void)
{
    int result = my_function();
    TEST_ASSERT_EQUAL(0, result);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_my_function_should_return_success);
    return UNITY_END();
}
```

### Common Unity Assertions

```c
/* Basic assertions */
TEST_ASSERT_TRUE(condition);
TEST_ASSERT_FALSE(condition);
TEST_ASSERT(condition);

/* Equality assertions */
TEST_ASSERT_EQUAL(expected, actual);
TEST_ASSERT_EQUAL_INT(expected, actual);
TEST_ASSERT_EQUAL_UINT(expected, actual);
TEST_ASSERT_EQUAL_HEX(expected, actual);

/* Pointer assertions */
TEST_ASSERT_NULL(pointer);
TEST_ASSERT_NOT_NULL(pointer);
TEST_ASSERT_EQUAL_PTR(expected, actual);

/* String assertions */
TEST_ASSERT_EQUAL_STRING(expected, actual);
TEST_ASSERT_EQUAL_MEMORY(expected, actual, length);

/* Floating point */
TEST_ASSERT_EQUAL_FLOAT(expected, actual);
TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual);

/* Array assertions */
TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, count);
TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, actual, count);

/* Range assertions */
TEST_ASSERT_INT_WITHIN(delta, expected, actual);
TEST_ASSERT_GREATER_THAN(threshold, actual);
TEST_ASSERT_LESS_THAN(threshold, actual);

/* Special */
TEST_FAIL_MESSAGE("Custom failure message");
TEST_IGNORE_MESSAGE("Test not implemented yet");
```

## Test Naming Convention

Use descriptive test names following this pattern:
```
test_<function_name>_should_<expected_behavior>_when_<condition>
```

Examples:
```c
void test_init_should_return_success_when_called_first_time(void);
void test_process_should_return_error_when_buffer_is_null(void);
void test_cleanup_should_free_resources_when_initialized(void);
```

## Adding New Test Files

1. Create new test file: `test/test_mymodule.c`
2. Update `test/CMakeLists.txt`:
   ```cmake
   add_executable(test_mymodule test_mymodule.c)
   target_link_libraries(test_mymodule PRIVATE scaffold_project::scaffold_project unity)
   add_test(NAME test_mymodule COMMAND test_mymodule)
   ```

## Best Practices

1. **One assertion per test** - Makes failures easier to diagnose
2. **Test one thing** - Each test should verify one specific behavior
3. **Use setUp/tearDown** - Initialize and cleanup test state
4. **Descriptive names** - Test names should clearly describe what they test
5. **Test edge cases** - NULL pointers, boundary values, error conditions
6. **Keep tests fast** - Unit tests should run quickly

## Example Test Suite

```c
void test_init_should_return_success_when_called_first_time(void)
{
    int result = scaffold_project_init();
    TEST_ASSERT_EQUAL(0, result);
}

void test_init_should_return_error_when_called_twice(void)
{
    scaffold_project_init();
    int result = scaffold_project_init();
    TEST_ASSERT_NOT_EQUAL(0, result);
}

void test_process_should_return_error_when_buffer_is_null(void)
{
    int result = scaffold_project_process(NULL, 0);
    TEST_ASSERT_NOT_EQUAL(0, result);
}
```

## Resources

- [Unity Documentation](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityGettingStartedGuide.md)
- [Unity Assertions Reference](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityAssertionsReference.md)
