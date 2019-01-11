cmake_minimum_required(VERSION 3.2)
project(small_client)

set(TARGET ${CMAKE_PROJECT_NAME})

if (CMAKE_BUILD_TYPE)
else()
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_FLAGS -std=c++11)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Weffc++ ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 ${CMAKE_CXX_FLAGS}")

set(CMAKE_MODULE_PATH ${SHEEP_PROJECT_SOURCE_DIR}/cmake)

#aux_source_directory(${PROJECT_SOURCE_DIR}/ LIST_SRC)
#aux_source_directory(${PROJECT_SOURCE_DIR}/lb_policy LIST_SRC)
#aux_source_directory(${PROJECT_SOURCE_DIR}/resolver LIST_SRC)

#include_directories(${PROJECT_SOURCE_DIR}/)
#include_directories(${PROJECT_SOURCE_DIR}/lb_policy)
#include_directories(${PROJECT_SOURCE_DIR}/resolver)

#add_library(${TARGET} STATIC ${LIST_SRC})

#find_package(small_packages REQUIRED)
#include_directories(${TARGET} ${small_packages_INCLUDE_DIRS})
#target_link_libraries(${TARGET} ${small_packages_LIBRARIES})

#find_package(log REQUIRED)
#include_directories(${TARGET} ${log_INCLUDE_DIRS})
#target_link_libraries(${TARGET} ${log_LIBRARIES})

#find_package(small_watcher REQUIRED)
#include_directories(${TARGET} ${small_watcher_INCLUDE_DIRS})
#target_link_libraries(${TARGET} ${small_watcher_LIBRARIES})

#target_link_libraries(${TARGET} pthread)
