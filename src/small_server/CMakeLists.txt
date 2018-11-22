cmake_minimum_required(VERSION 3.7)
project(small_server)

set(TARGET ${CMAKE_PROJECT_NAME})

if (CMAKE_BUILD_TYPE)
else()
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_FLAGS -std=c++11)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Weffc++ ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 ${CMAKE_CXX_FLAGS}")

#aux_source_directory(${PROJECT_SOURCE_DIR}/ LIST_SRC)

include_directories(${PROJECT_SOURCE_DIR}/)

#add_library(${TARGET} STATIC ${LIST_SRC})

#target_link_libraries(${TARGET} pthread)
