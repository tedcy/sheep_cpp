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
aux_source_directory(${PROJECT_SOURCE_DIR}/protos/test LIST_SRC)
add_executable(${TARGET} ${LIST_SRC})

include_directories(${PROJECT_SOURCE_DIR}/protos/test)
include_directories(${PROJECT_SOURCE_DIR}/)

find_package(small_flow REQUIRED)
include_directories(${TARGET} ${small_flow_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${small_flow_LIBRARIES})

target_link_libraries(${TARGET} pthread)
