# OSAL Testing Strategy

## Testing Philosophy

OSAL uses **integration testing** rather than unit testing with mocks for the following reasons:

### Why Integration Tests?

1. **OSAL is a thin abstraction layer** - It wraps OS primitives, not complex business logic
2. **Platform-specific behavior matters** - Testing real OS behavior catches actual bugs
3. **Cross-platform compatibility** - Works on Linux, macOS, FreeRTOS without mocking frameworks
4. **More valuable tests** - Verifies actual functionality, not mocked behavior
5. **Simpler maintenance** - No complex mocking infrastructure to maintain

### What We Test

✅ **Real behavior on each platform**
✅ **Actual threading, synchronization, timing**
✅ **Cross-thread communication**
✅ **Timeout handling**
✅ **Resource management**

## Test Coverage

### Memory Management
- Allocation and deallocation
- Multiple allocations
- Memory usability

### Time Functions
- Delays (with tolerance for OS scheduling)
- Tick counter monotonicity
- Unix time retrieval

### Mutex
- Basic create/delete
- Lock/unlock operations
- Timeout handling
- **Thread safety** - Concurrent access protection

### Semaphore
- Binary and counting semaphores
- Give/take operations
- Timeout behavior
- **Thread signaling** - Producer/consumer pattern

### Message Queue
- FIFO ordering
- Send/receive operations
- Queue count tracking
- Full/empty handling

### Event Flags
- Set/clear/get operations
- Wait ANY vs WAIT ALL
- **Thread synchronization** - Event-based signaling

### Threads
- Creation and deletion
- Thread execution verification
- Yielding

## Running Tests

### Build and Run

```bash
mkdir build && cd build
cmake -DBUILD_TESTS=ON ..
make
ctest --verbose
```

### Platform-Specific Execution

**Linux (POSIX)**
```bash
cmake -DOSAL_PLATFORM=POSIX -DBUILD_TESTS=ON ..
make
./test/test_osal
```

**macOS**
```bash
cmake -DOSAL_PLATFORM=MACOS -DBUILD_TESTS=ON ..
make
./test/test_osal
```

**FreeRTOS**
- Tests require FreeRTOS simulator or hardware
- See FreeRTOS-specific test documentation

## Test Structure

Each test follows this pattern:

```c
void test_feature_scenario(void)
{
    // 1. Setup - Create OSAL objects
    osalMutexHandle_t mutex = osalMutexCreate(NULL);
    TEST_ASSERT_NOT_NULL(mutex);

    // 2. Execute - Perform operations
    osalStatus_t status = osalMutexLock(mutex, OSAL_WAIT_FOREVER);
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);

    // 3. Verify - Check results
    status = osalMutexUnlock(mutex);
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);

    // 4. Cleanup - Delete OSAL objects
    osalMutexDelete(mutex);
}
```

## Test Timeouts

Tests use generous timeouts to account for:
- OS scheduling variability
- CI/CD system load
- Debug builds

Typical tolerances:
- Timing tests: ±20ms
- Thread synchronization: ±30ms
- Total test timeout: 5 seconds

## Adding New Tests

When adding new OSAL functionality:

1. Add integration test to `test_osal.c`
2. Test actual behavior, not implementation
3. Use generous timeouts for reliability
4. Test error conditions (NULL pointers, timeouts)
5. Clean up all resources

Example:

```c
void test_new_feature(void)
{
    // Create
    osalFeatureHandle_t feature = osalFeatureCreate(NULL);
    TEST_ASSERT_NOT_NULL(feature);

    // Test success case
    osalStatus_t status = osalFeatureDoSomething(feature);
    TEST_ASSERT_EQUAL(OSAL_SUCCESS, status);

    // Test error case
    status = osalFeatureDoSomething(NULL);
    TEST_ASSERT_EQUAL(OSAL_ERROR_PARAMETER, status);

    // Cleanup
    osalFeatureDelete(feature);
}
```

## CI/CD Integration

Tests are designed to run reliably in CI environments:
- No hardcoded timing assumptions
- Tolerant of system load
- Clear pass/fail criteria
- Minimal external dependencies

## Debugging Failed Tests

If a test fails:

1. **Check platform** - Some behavior varies by OS
2. **Run locally** - CI environment may have different timing
3. **Increase timeout** - System may be under load
4. **Check logs** - Unity provides clear failure messages
5. **Run single test** - Isolate the failing test

```bash
# Run specific test
./test/test_osal --verbose
```

## Test Maintenance

Tests should be:
- ✅ **Reliable** - Pass consistently
- ✅ **Fast** - Complete in seconds
- ✅ **Clear** - Easy to understand failures
- ✅ **Isolated** - No dependencies between tests
- ✅ **Platform-agnostic** - Work on all supported platforms

## Legacy Tests

`test_posix.c` contains the old mocked unit tests and is **deprecated**. Use `test_osal.c` for all new tests.
