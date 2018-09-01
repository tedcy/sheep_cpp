cmake_minimum_required(VERSION 3.11)
# Usage by vcpkg (CMAKE_TOOLCHAIN_FILE):
#   mkdir build && cd build && cmake .. -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE
#   mkdir build && cd build && cmake .. -DCMAKE_TOOLCHAIN_FILE="/home/ubuntu/git-third/vcpkg/scripts/buildsystems/vcpkg.cmake"
#   cmake .. -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE
#   cmake .. -DCMAKE_TOOLCHAIN_FILE="/home/ubuntu/git-third/vcpkg/scripts/buildsystems/vcpkg.cmake"

project(testServer)
set(TARGET ${CMAKE_PROJECT_NAME})

# flags
if (CMAKE_BUILD_TYPE)
else()
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

message("------------ Options -------------")
message("  CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("  CMAKE_COMMAND: ${CMAKE_COMMAND}")

set(CMAKE_CXX_FLAGS -std=c++11)
set(CMAKE_CXX_FLAGS_DEBUG "-g -pg ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 ${CMAKE_CXX_FLAGS}")

message("------------ Cxx flags -------------")
message("  CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}: ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}}")
message("------------------------------------")

# source
aux_source_directory(${PROJECT_SOURCE_DIR}/../server_test LIST_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/../server_test/service LIST_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/../server LIST_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/../server/event_loop LIST_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/../server/service LIST_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/../server/buffer LIST_SRC)
add_executable(${TARGET} ${LIST_SRC})

# include
include_directories(${PROJECT_SOURCE_DIR}/../common)
include_directories(${PROJECT_SOURCE_DIR}/../server)
include_directories(${PROJECT_SOURCE_DIR}/../server/event_loop)
include_directories(${PROJECT_SOURCE_DIR}/../server/service)
include_directories(${PROJECT_SOURCE_DIR}/../server/buffer)
include_directories(${PROJECT_SOURCE_DIR}/../server_test/service)

# link
target_link_libraries(${TARGET} "")

# libgo
#find_package(libgo REQUIRED)
#find_package(libgo)
#if (LIBGO_FOUND)
#    include_directories(${LIBGO_INCLUDE_DIRS})
#    target_link_libraries(${TARGET} ${LIBGO_LIBRARIES})
#endif()

# boost
#find_package(Boost REQUIRED context thread system date_time chrono regex)
#find_package(Boost REQUIRED context thread system)
#if (Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS})
#    link_directories(${Boost_LIBRARY_DIRS})
#    target_link_libraries(${TARGET} ${Boost_LIBRARIES})
#endif()

# target
target_link_libraries(${TARGET} -pthread -ldl -static)
