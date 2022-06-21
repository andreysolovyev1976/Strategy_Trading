cmake_minimum_required(VERSION 3.19)
set (PROJECT_NAME "strategy_trading")

project(${PROJECT_NAME}
        VERSION 0.1
        DESCRIPTION "Trading Strategy in C++"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
if (APPLE)
    set(CMAKE_CXX_FLAGS " -Wall -Wextra -Wpedantic -Werror -Wreturn-std-move -Wno-c99-extensions -Wno-missing-field-initializers")
else()
    set(CMAKE_CXX_FLAGS " -Wall -Wextra -Wpedantic -Werror -Wno-missing-field-initializers -Wno-attributes")
endif()

set(CMAKE_CXX_FLAGS_DEBUG " -fsanitize=undefined -g")
set(CMAKE_CXX_FLAGS_DEBUG " -fsanitize=address -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")
set(CMAKE_CXX_FLAGS_DEBUG " -fsanitize=thread -g")
set(CMAKE_CXX_FLAGS_RELEASE " -O3")

#Threads not found
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)


# BOOST STARTS HERE-------------------
find_package (Boost COMPONENTS filesystem thread REQUIRED)
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
if (APPLE)
    find_package(OpenSSL 3.0 REQUIRED)
else()
    find_package(OpenSSL 3.0.2 REQUIRED)
endif()

if(OpenSSL_FOUND)
    message(STATUS "MAIN: FOUND OpenSSL, including and linking...")
    include_directories(/usr/local/Cellar/openssl@3/3.0.0/include)
else()
    message(STATUS "MAIN: NOT FOUND OpenSSL")
endif()
# OPEN SSL FRAMEWORK ENDS HERE-------------------


# FMT STARTS HERE-------------------
if (APPLE)
    find_package(fmt)
    if(fmt_FOUND)
        message(STATUS "MAIN: FOUND fmt, including and linking...")
    else()
        message(STATUS "MAIN: NOT FOUND fmt")
    endif()
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
include_directories(project_files)
include_directories(project_files/primitives)
include_directories(project_files/strategy)
include_directories(project_files/strategy/market_data)
include_directories(project_files/json)
include_directories(project_files/curl_client)
include_directories(project_files/dexes)
include_directories(project_files/dexes/tezos)
include_directories(project_files/dexes/coinbase)
include_directories(project_files/dexes/quipuswap)
include_directories(project_files/dexes/tzkt)
include_directories(project_files/dexes/transaction)
include_directories(project_files/robot_config)
include_directories(project_files/pipe)
include_directories(project_files/app_controller)
include_directories(project_files/ui)
include_directories(third_part/date_time/include)
include_directories(third_part/cds)
include_directories(third_part/tgbot/include)


add_executable(${PROJECT_NAME}
        main.cpp
        )
add_subdirectory(project_files/primitives)
add_subdirectory(project_files/strategy)
add_subdirectory(project_files/json)
add_subdirectory(project_files/dexes)
add_subdirectory(project_files/curl_client)
add_subdirectory(project_files/robot_config)
add_subdirectory(project_files/pipe)
add_subdirectory(project_files/app_controller)

if (APPLE)
    message(STATUS "Loading APPLE HOME COMPUTER Configuration...")
    target_link_libraries(${PROJECT_NAME}
            /Users/elenasolovyeva/Documents/Cpp/Projects/StrategyTrading/third_part/tgbot/libTgBot.a
            /usr/local/Cellar/openssl@3/3.0.0/lib/libssl.dylib
            /usr/local/Cellar/openssl@3/3.0.0/lib/libcrypto.dylib
            fmt::fmt
            ${Boost_LIBRARIES}
            ${CURL_LIBRARY}
            )
else()
    message(STATUS "Loading DEBIAN REMOTE COMPUTER Configuration...")
    target_link_libraries(${PROJECT_NAME}
            libTgBot.a
            OpenSSL::SSL
            OpenSSL::Crypto
            ${Boost_LIBRARIES}
            ${CURL_LIBRARY}
            )
endif()

message(STATUS "Loading Testing Environment...")
enable_testing()
add_subdirectory(tests)

message(STATUS "STATUS ======== OK ========")

