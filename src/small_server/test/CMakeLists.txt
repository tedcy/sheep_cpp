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

foreach (TARGET server grpc_client)
    aux_source_directory(${PROJECT_SOURCE_DIR}/${TARGET} ${TARGET}LIST_SRC)
    aux_source_directory(${PROJECT_SOURCE_DIR}/protos/test ${TARGET}LIST_SRC)
    add_executable(${TARGET} ${${TARGET}LIST_SRC})

    include_directories(${PROJECT_SOURCE_DIR}/protos/test)
    include_directories(${PROJECT_SOURCE_DIR}/${TARGET})

    find_package(small_client REQUIRED)
    include_directories(${TARGET} ${small_client_INCLUDE_DIRS})
    target_link_libraries(${TARGET} ${small_client_LIBRARIES})

    find_package(small_server REQUIRED)
    include_directories(${TARGET} ${small_server_INCLUDE_DIRS})
    target_link_libraries(${TARGET} ${small_server_LIBRARIES})

    find_package(small_watcher REQUIRED)
    include_directories(${TARGET} ${small_watcher_INCLUDE_DIRS})
    target_link_libraries(${TARGET} ${small_watcher_LIBRARIES})
    target_link_libraries(${TARGET} pthread)
endforeach()
