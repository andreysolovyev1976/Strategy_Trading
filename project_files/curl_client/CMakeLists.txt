cmake_minimum_required(VERSION 3.19)
project(${PROJECT_NAME})


if (${PROJECT_NAME} STREQUAL "strategy_trading")
    message(STATUS "MAIN: Linking " ${CMAKE_CURRENT_SOURCE_DIR})
else()
    message(STATUS "TEST: Linking " ${CMAKE_CURRENT_SOURCE_DIR})
endif()

target_sources(${PROJECT_NAME} PRIVATE
        curl_const_values.h
        connection_data.cpp
        curl_handler.cpp
        curl_data_structures.cpp
        request.cpp
        utils.cpp
        )
