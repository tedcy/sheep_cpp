cmake_minimum_required(VERSION 3.2)
project(small_watcher)

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
aux_source_directory(${PROJECT_SOURCE_DIR}/etcd LIST_SRC)
add_library(${TARGET} STATIC ${LIST_SRC})

include_directories(${TARGET} ${PROJECT_SOURCE_DIR}/)
include_directories(${TARGET} ${PROJECT_SOURCE_DIR}/etcd)

find_package(small_client REQUIRED)
include_directories(${TARGET} ${small_client_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${small_client_LIBRARIES})

find_package(net REQUIRED)
include_directories(${TARGET} ${net_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${net_LIBRARIES})

find_package(small_packages REQUIRED)
include_directories(${TARGET} ${small_packages_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${small_packages_LIBRARIES})

find_package(log REQUIRED)
include_directories(${TARGET} ${log_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${log_LIBRARIES})

target_link_libraries(${TARGET} pthread)
