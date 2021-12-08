cmake_minimum_required(VERSION 3.19)
project(${PROJECT_NAME})


if (${PROJECT_NAME} STREQUAL "strategy_trading")
    message(STATUS "MAIN: Linking " ${CMAKE_CURRENT_SOURCE_DIR})
else()
    message(STATUS "TEST: Linking " ${CMAKE_CURRENT_SOURCE_DIR})
endif()

target_sources(${PROJECT_NAME} PRIVATE
        curl_client/const_values.h
        curl_client/connection_data.cpp
        curl_client/curl_handler.cpp
        curl_client/data_structures.cpp
        curl_client/request.cpp
        curl_client/utils.cpp
        coinbase.cpp
        )
