cmake_minimum_required(VERSION 3.7)
project(main)

if (CMAKE_BUILD_TYPE)
else()
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_FLAGS -std=c++11)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Weffc++ ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 ${CMAKE_CXX_FLAGS}")

set(CMAKE_MODULE_PATH ${SHEEP_PROJECT_SOURCE_DIR}/cmake)

foreach (TARGET server client client_pool)
    aux_source_directory(${PROJECT_SOURCE_DIR}/${TARGET} ${TARGET}LIST_SRC)
    add_executable(${TARGET} ${${TARGET}LIST_SRC})

    include_directories(${PROJECT_SOURCE_DIR}/${TARGET})

    find_package(net REQUIRED)
    include_directories(${TARGET} ${net_INCLUDE_DIRS})
    target_link_libraries(${TARGET} ${net_LIBRARIES})
    
    target_link_libraries(${TARGET} pthread)
endforeach()