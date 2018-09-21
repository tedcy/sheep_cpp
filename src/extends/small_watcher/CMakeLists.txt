cmake_minimum_required(VERSION 3.7)
project(small_watcher)

set(TARGET ${CMAKE_PROJECT_NAME})

if (CMAKE_BUILD_TYPE)
else()
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_FLAGS -std=c++11)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Weffc++ ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 ${CMAKE_CXX_FLAGS}")

set(CMAKE_MODULE_PATH ${SHEEP_PROJECT_SOURCE_DIR}/cmake)

aux_source_directory(${PROJECT_SOURCE_DIR}/ LIST_SRC)
aux_source_directory(${PROJECT_SOURCE_DIR}/etcd LIST_SRC)
add_library(${TARGET} STATIC ${LIST_SRC})

include_directories(${TARGET} ${PROJECT_SOURCE_DIR}/)
include_directories(${TARGET} ${PROJECT_SOURCE_DIR}/etcd)

find_package(small_http_client REQUIRED)
include_directories(${TARGET} ${small_http_client_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${small_http_client_LIBRARIES})

find_package(small_timer REQUIRED)
include_directories(${TARGET} ${small_timer_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${small_timer_LIBRARIES})

find_package(small_net REQUIRED)
include_directories(${TARGET} ${small_net_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${small_net_LIBRARIES})

find_package(small_packages REQUIRED)
include_directories(${TARGET} ${small_packages_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${small_packages_LIBRARIES})

find_package(log REQUIRED)
include_directories(${TARGET} ${log_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${log_LIBRARIES})

find_package(jsoncpp CONFIG REQUIRED)
include_directories(${TARGET} ${jsoncpp_INCLUDE_DIRS})
target_link_libraries(${TARGET} jsoncpp_lib_static)

target_link_libraries(${TARGET} pthread)
