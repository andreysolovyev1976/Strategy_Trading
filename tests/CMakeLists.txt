cmake_minimum_required(VERSION 3.19)
set (PROJECT_NAME_TEST ${PROJECT_NAME}_tests)
project(${PROJECT_NAME_TEST}
        VERSION 0.1
        DESCRIPTION "Tezos Client in C++ - TEST"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS " -Wall -Wextra -Wpedantic -Werror -Wreturn-std-move -Wno-c99-extensions -Wno-missing-field-initializers -o")
set(CMAKE_CXX_FLAGS_DEBUG " -fsanitize=undefined -g")
set(CMAKE_CXX_FLAGS_DEBUG " -fsanitize=address -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_CXX_FLAGS_DEBUG " -fsanitize=thread -g")
set(CMAKE_CXX_FLAGS_RELEASE " -O3")

find_package(GTest REQUIRED)
if(GTest_FOUND)
    message(STATUS "TESTS: FOUND GTest, including and linking...")
    include_directories(${GTEST_INCLUDE_DIRS})
else()
    message(STATUS "TESTS: NOT FOUND GTest")
endif()

if (APPLE)
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE)
    add_definitions(-D__GLIBCXX__)
endif (APPLE)

add_executable(${PROJECT_NAME_TEST}
        tests_all.cpp
        tests_data_processor.cpp
        tests_signal.cpp
        tests_trade.cpp
        tests_rule.cpp
        tests_portfolio.cpp
        tests_value.cpp
        tests_indicator.cpp)
add_subdirectory(../primitives ${CMAKE_CURRENT_BINARY_DIR}_primitives)
add_subdirectory(../strategy ${CMAKE_CURRENT_BINARY_DIR}_strategy)

target_link_libraries(${PROJECT_NAME_TEST}
        ${GTEST_LIBRARIES} pthread
        )

# This is so you can do 'make test' to see all your tests run, instead of
# manually running the executable runUnitTests to see those specific tests.
#add_test(NAME that-test-I-made COMMAND ${PROJECT_NAME_TEST})

# You can also omit NAME and COMMAND. The second argument could be some other
# test executable.
#add_test(that-other-test-I-made runUnitTests)

