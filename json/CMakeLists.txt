cmake_minimum_required(VERSION 3.19)
project(${PROJECT_NAME})

set (SOURCES
        "json.cpp"
        "json_builder.cpp"
        )

if(${BUILD_BRANCH} STREQUAL PROD)
    message(STATUS "PROD: Linking " ${CMAKE_CURRENT_SOURCE_DIR})
    target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
elseif(${BUILD_BRANCH} STREQUAL EXPERIMENTS)
    message(STATUS "EXPERIMENTS: Linking " ${CMAKE_CURRENT_SOURCE_DIR})
    target_sources(${PROJECT_NAME}_tryouts  PRIVATE ${SOURCES})
else()
    message(STATUS "Error of selecting a Build Branch - see CMakeLists.txt for the details")
endif()
