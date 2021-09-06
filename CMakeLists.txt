file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true src/*.cpp)
file(GLOB_RECURSE HEADERS LIST_DIRECTORIES true inc/*.h)

add_library(osal ${SOURCES})
set_target_properties(osal PROPERTIES PUBLIC_HEADER "${HEADERS}")
target_include_directories(osal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

install(TARGETS osal
        PUBLIC_HEADER DESTINATION include/osal/
)