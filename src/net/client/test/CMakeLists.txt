cmake_minimum_required(VERSION 3.2)
project(main)

if (CMAKE_BUILD_TYPE)
else()
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_FLAGS -std=c++11)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wno-unused-parameter ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 ${CMAKE_CXX_FLAGS}")

set(CMAKE_MODULE_PATH ${SHEEP_PROJECT_SOURCE_DIR}/cmake)

set(SHEEP_NET_PROJECT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/../..)
foreach (TARGET main)
    aux_source_directory(${PROJECT_SOURCE_DIR}/ LIST_SRC)
    aux_source_directory(${SHEEP_NET_PROJECT_SOURCE_DIR}/client LIST_SRC)
    aux_source_directory(${SHEEP_NET_PROJECT_SOURCE_DIR}/common LIST_SRC)
    aux_source_directory(${SHEEP_NET_PROJECT_SOURCE_DIR}/common/tcp LIST_SRC)
    aux_source_directory(${SHEEP_NET_PROJECT_SOURCE_DIR}/common/timer LIST_SRC)
    aux_source_directory(${SHEEP_NET_PROJECT_SOURCE_DIR}/common/asyncer LIST_SRC)
    aux_source_directory(${SHEEP_NET_PROJECT_SOURCE_DIR}/common/poller LIST_SRC)
    add_executable(${TARGET} ${LIST_SRC})

    include_directories(${SHEEP_NET_PROJECT_SOURCE_DIR}/client)
    include_directories(${SHEEP_NET_PROJECT_SOURCE_DIR}/common)
    include_directories(${SHEEP_NET_PROJECT_SOURCE_DIR}/common/tcp)
    include_directories(${SHEEP_NET_PROJECT_SOURCE_DIR}/common/timer)
    include_directories(${SHEEP_NET_PROJECT_SOURCE_DIR}/common/asyncer)
    include_directories(${SHEEP_NET_PROJECT_SOURCE_DIR}/common/poller)

    find_package(small_packages REQUIRED)
    include_directories(${TARGET} ${small_packages_INCLUDE_DIRS})
    target_link_libraries(${TARGET} ${small_packages_LIBRARIES})
    
    find_package(small_test REQUIRED)
    include_directories(${TARGET} ${small_test_INCLUDE_DIRS})
    target_link_libraries(${TARGET} ${small_test_LIBRARIES})

    find_package(log REQUIRED)
    include_directories(${TARGET} ${log_INCLUDE_DIRS})
    target_link_libraries(${TARGET} ${log_LIBRARIES})

    target_link_libraries(${TARGET} pthread)
endforeach()
