cmake_minimum_required(VERSION 3.2)
project(main)

set(TARGET ${CMAKE_PROJECT_NAME})

if (CMAKE_BUILD_TYPE)
else()
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_FLAGS -std=c++11)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wno-unused-parameter ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 ${CMAKE_CXX_FLAGS}")

set(CMAKE_MODULE_PATH ${SHEEP_PROJECT_SOURCE_DIR}/cmake)

aux_source_directory(${PROJECT_SOURCE_DIR}/ LIST_SRC)
add_executable(${TARGET} ${LIST_SRC})

find_package(small_http_parser REQUIRED)
include_directories(${TARGET} ${small_http_parser_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${small_http_parser_LIBRARIES})

find_package(log REQUIRED)
include_directories(${TARGET} ${log_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${log_LIBRARIES})

target_link_libraries(${TARGET} pthread)
