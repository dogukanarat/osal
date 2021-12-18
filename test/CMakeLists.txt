enable_testing()

set(${TEST_NAME} ${PROJECT_NAME}}-test)

add_executable(
    ${TEST_NAME}
    main.cpp
    )

target_link_libraries(
    ${TEST_NAME}
    gtest_main
    osal
    )

add_test(
    NAME that-test-I-made
    COMMAND ${TEST_NAME}
    )

gtest_discover_tests(${TEST_NAME})
