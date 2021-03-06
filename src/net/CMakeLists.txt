cmake_minimum_required(VERSION 3.2)
project(net)

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
aux_source_directory(${PROJECT_SOURCE_DIR}/common LIST_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/common/tcp LIST_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/common/poller LIST_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/common/timer LIST_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/common/asyncer LIST_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/client LIST_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/server LIST_SRC)

include_directories(${PROJECT_SOURCE_DIR}/)
include_directories(${PROJECT_SOURCE_DIR}/common)
include_directories(${PROJECT_SOURCE_DIR}/common/tcp)
include_directories(${PROJECT_SOURCE_DIR}/common/poller)
include_directories(${PROJECT_SOURCE_DIR}/common/timer)
include_directories(${PROJECT_SOURCE_DIR}/common/asyncer)
include_directories(${PROJECT_SOURCE_DIR}/client)
include_directories(${PROJECT_SOURCE_DIR}/server)

add_library(${TARGET} STATIC ${LIST_SRC})

find_package(small_packages REQUIRED)
include_directories(${TARGET} ${small_packages_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${small_packages_LIBRARIES})

target_link_libraries(${TARGET} pthread)
