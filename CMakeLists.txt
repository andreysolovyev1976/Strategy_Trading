cmake_minimum_required(VERSION 3.19)
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
find_package (Boost REQUIRED filesystem)
if(Boost_FOUND)
    message(STATUS "MAIN: FOUND Boost, including and linking...")
    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREAD ON)
    include_directories(${BOOST_ROOT})
    #link_directories(${BOOST_ROOT}/libs) # add this before add_executable()
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


# CURL STARTS HERE-------------------
find_package (CURL REQUIRED)
if(CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    message(STATUS "MAIN: FOUND CURL, including and linking...")
else()
    message(STATUS "MAIN: NOT FOUND CURL")
endif()
# CURL FRAMEWORK ENDS HERE-------------------

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(primitives)
include_directories(strategy)
include_directories(json)
include_directories(curl_client)
include_directories(data_feed/tezos)
include_directories(data_feed/coinbase)
include_directories(data_feed/quipuswap)
include_directories(data_feed/tzkt)
include_directories(ini)
include_directories(tg_bot_ui)
include_directories(transaction)
include_directories(third_part/date_time/include)
include_directories(third_part/cds)
include_directories(third_part/tgbot/include)

#either PROD or EXPERIMENTS
SET(BUILD_BRANCH "PROD")

if(${BUILD_BRANCH} STREQUAL PROD)

    message(STATUS "Build Branch - selected PROD")

    add_executable(${PROJECT_NAME}
            main.cpp
            )
    add_subdirectory(primitives)
    add_subdirectory(strategy)
    add_subdirectory(json)
    add_subdirectory(data_feed)
    add_subdirectory(curl_client)
    add_subdirectory(tg_bot_ui)
    add_subdirectory(ini)
    add_subdirectory(transaction)

    target_link_libraries(${PROJECT_NAME}
            /usr/local/lib/libTgBot.a
            /usr/local/Cellar/openssl@3/3.0.0/lib/libssl.dylib
            /usr/local/Cellar/openssl@3/3.0.0/lib/libcrypto.dylib
            fmt::fmt
            ${Boost_LIBRARIES}
            ${CURL_LIBRARY}
            )

    enable_testing()
    add_subdirectory(tests)

    message(STATUS "STATUS ======== OK ========")

elseif(${BUILD_BRANCH} STREQUAL EXPERIMENTS)

    message(STATUS "Build Branch - selected EXPERIMENTS")

    include_directories (third_part/signalR/include)

    add_executable(${PROJECT_NAME}_tryouts tryouts.cpp)
    add_subdirectory(third_part/signalR)
    add_subdirectory(json)
    add_subdirectory(data_feed)
    add_subdirectory(curl_client)
    add_subdirectory(tg_bot_ui)
    add_subdirectory(ini)
    add_subdirectory(transaction)

    #add_executable(${PROJECT_NAME}_tryouts boost_ws.cpp)
    target_link_libraries(${PROJECT_NAME}_tryouts
            /usr/local/lib/libTgBot.a
            /usr/local/Cellar/openssl@3/3.0.0/lib/libssl.dylib
            /usr/local/Cellar/openssl@3/3.0.0/lib/libcrypto.dylib
            ${CURL_LIBRARY}
            ${Boost_LIBRARIES}
            )
    message(STATUS "STATUS ======== OK ========")
else()
    message(STATUS "STATUS ======= FAIL =======")
    message(STATUS "Error of selecting a Build Branch - see CMakeLists.txt for the details")
endif()