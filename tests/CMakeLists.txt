cmake_minimum_required(VERSION 3.19)
set (PROJECT_NAME_TEST ${PROJECT_NAME}_tests)
project(${PROJECT_NAME_TEST}
        VERSION 0.1
        DESCRIPTION "Tezos Client in C++ - TEST"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
if (APPLE)
    set(CMAKE_CXX_FLAGS " -Wall -Wextra -Wpedantic -Werror -Wreturn-std-move -Wno-c99-extensions -Wno-missing-field-initializers -o")
else()
    set(CMAKE_CXX_FLAGS " -Wall -Wextra -Wpedantic -Werror -Wno-missing-field-initializers -Wno-attributes")
endif()

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
        tests_safe_ptr.cpp
        tests_api_coinbase.cpp
        tests_api_tzkt.cpp
        tests_data_processor.cpp
        tests_signal.cpp
        tests_trade.cpp
        tests_rule.cpp
        tests_portfolio.cpp
        tests_value.cpp
        tests_trading_contract.cpp
        tests_indicator.cpp
        tests_indicator_modifier.cpp
        )
add_subdirectory(../project_files/curl_client ${CMAKE_CURRENT_BINARY_DIR}_curl_client)
add_subdirectory(../project_files/dexes ${CMAKE_CURRENT_BINARY_DIR}_dexes)
add_subdirectory(../project_files/json ${CMAKE_CURRENT_BINARY_DIR}_json)
add_subdirectory(../project_files/pipe ${CMAKE_CURRENT_BINARY_DIR}_pipe)
add_subdirectory(../project_files/primitives ${CMAKE_CURRENT_BINARY_DIR}_primitives)
add_subdirectory(../project_files/robot_config ${CMAKE_CURRENT_BINARY_DIR}_robot_config)
add_subdirectory(../project_files/strategy ${CMAKE_CURRENT_BINARY_DIR}_strategy)
add_subdirectory(../project_files/app_controller ${CMAKE_CURRENT_BINARY_DIR}_app_controller)

if (APPLE)
    message(STATUS "Loading APPLE HOME COMPUTER Configuration...")
target_link_libraries(${PROJECT_NAME_TEST}
        ${GTEST_LIBRARIES} pthread
        /Users/elenasolovyeva/Documents/Cpp/Projects/StrategyTrading/third_part/tgbot/libTgBot.a
        /usr/local/Cellar/openssl@3/3.0.0/lib/libssl.dylib
        /usr/local/Cellar/openssl@3/3.0.0/lib/libcrypto.dylib
        fmt::fmt
        ${Boost_LIBRARIES}
        ${CURL_LIBRARY}
        )
else()
    message(STATUS "Loading DEBIAN REMOTE COMPUTER Configuration...")
    target_link_libraries(${PROJECT_NAME_TEST}
            libTgBot.a
            OpenSSL::SSL
            OpenSSL::Crypto
            ${Boost_LIBRARIES}
            ${CURL_LIBRARY}
            )
endif()

# This is so you can do 'make test' to see all your tests run, instead of
# manually running the executable runUnitTests to see those specific tests.
#add_test(NAME that-test-I-made COMMAND ${PROJECT_NAME_TEST})

# You can also omit NAME and COMMAND. The second argument could be some other
# test executable.
#add_test(that-other-test-I-made runUnitTests)

