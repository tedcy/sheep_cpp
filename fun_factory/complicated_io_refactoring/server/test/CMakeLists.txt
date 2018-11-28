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

foreach (TARGET acceptor_test)
    aux_source_directory(${PROJECT_SOURCE_DIR}/../ ${TARGET}LIST_SRC)
    aux_source_directory(${PROJECT_SOURCE_DIR}/../../common/tcp ${TARGET}LIST_SRC)
    aux_source_directory(${PROJECT_SOURCE_DIR}/../../common ${TARGET}LIST_SRC)
    set(${TARGET}LIST_SRC ${${TARGET}LIST_SRC} ${TARGET}.cpp)
    add_executable(${TARGET} ${${TARGET}LIST_SRC})

    include_directories(${PROJECT_SOURCE_DIR}/../)
    include_directories(${PROJECT_SOURCE_DIR}/../../common/tcp)
    include_directories(${PROJECT_SOURCE_DIR}/../../common)

    find_package(small_packages REQUIRED)
    include_directories(${TARGET} ${small_packages_INCLUDE_DIRS})
    target_link_libraries(${TARGET} ${small_packages_LIBRARIES})
    
    find_package(log REQUIRED)
    include_directories(${TARGET} ${log_INCLUDE_DIRS})
    target_link_libraries(${TARGET} ${log_LIBRARIES})

    target_link_libraries(${TARGET} pthread)
endforeach()
