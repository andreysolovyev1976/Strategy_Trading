cmake_minimum_required(VERSION 3.21)
set (PROJECT_NAME "strategy_trading")

project(${PROJECT_NAME}
        VERSION 0.1
        DESCRIPTION "Trading Strategy in C++"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS " -Wall -Wextra -Wpedantic -Werror -Wreturn-std-move -Wno-c99-extensions -o")
set(CMAKE_CXX_FLAGS_DEBUG " -fsanitize=undefined -g")
set(CMAKE_CXX_FLAGS_DEBUG " -fsanitize=address -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_CXX_FLAGS_DEBUG " -fsanitize=thread -g")
set(CMAKE_CXX_FLAGS_RELEASE " -O3")

#Threads not found
IF(APPLE)
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
ENDIF()


# BOOST STARTS HERE-------------------
find_package (Boost 1.73 REQUIRED filesystem)
if(Boost_FOUND)
    message(STATUS "MAIN: FOUND Boost, including and linking...")
    include_directories(${BOOST_ROOT})
else()
    message(STATUS "MAIN: NOT FOUND Boost")
endif()
# BOOST ENDS HERE-------------------


# OPENSSL STARTS HERE-------------------
find_package(OpenSSL 3.0 REQUIRED)
if(OpenSSL_FOUND)
    message(STATUS "MAIN: FOUND OpenSSL, including and linking...")
    include_directories(/usr/local/Cellar/openssl@3/3.0.0/include)
else()
    message(STATUS "MAIN: NOT FOUND OpenSSL")
endif()
# OPEN SSL FRAMEWORK ENDS HERE-------------------


# FMT STARTS HERE-------------------
find_package(fmt)
if(fmt_FOUND)
    message(STATUS "MAIN: FOUND fmt, including and linking...")
else()
    message(STATUS "MAIN: NOT FOUND fmt")
endif()
# FMT FRAMEWORK ENDS HERE-------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(primitives)
include_directories(strategy)

add_executable(${PROJECT_NAME}
        main.cpp
        )
add_subdirectory(primitives)
add_subdirectory(strategy)

target_link_libraries(${PROJECT_NAME}
        fmt::fmt
        ${Boost_LIBRARIES}
        )

enable_testing()
add_subdirectory(tests)
