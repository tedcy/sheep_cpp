cmake_minimum_required(VERSION 3.7)
project(main)

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
add_executable(${TARGET} ${LIST_SRC})

find_package(small_net REQUIRED)
include_directories(${TARGET} ${small_net_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${small_net_LIBRARIES})

find_package(log REQUIRED)
include_directories(${TARGET} ${log_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${log_LIBRARIES})

find_package(Boost REQUIRED COMPONENTS system)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    target_link_libraries(${TARGET} ${Boost_LIBRARIES})
endif()

find_package(jsoncpp CONFIG REQUIRED)
include_directories(${TARGET} ${jsoncpp_INCLUDE_DIRS})
target_link_libraries(${TARGET} jsoncpp_lib_static)

find_package(glog CONFIG REQUIRED)
target_link_libraries(${TARGET} glog::glog)

target_link_libraries(${TARGET} pthread)
