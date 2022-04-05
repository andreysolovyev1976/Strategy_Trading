cmake_minimum_required(VERSION 3.19)
project(${PROJECT_NAME})

set (SOURCES
        "json.cpp"
        "json_builder.cpp"
        "json_handler.cpp"
        )

message(STATUS "PROD: Linking " ${CMAKE_CURRENT_SOURCE_DIR})
target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
