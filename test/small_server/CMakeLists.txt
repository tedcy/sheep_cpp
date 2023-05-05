cmake_minimum_required(VERSION 3.2)
project(main)

if (CMAKE_BUILD_TYPE)
else()
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_FLAGS -std=c++11)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wno-unused-parameter ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 ${CMAKE_CXX_FLAGS}")

foreach (TARGET server grpc_client)
    include_directories(${SHEEP_INSTALL_DIR}/include)
    link_directories(${SHEEP_INSTALL_DIR}/lib)

    aux_source_directory(${PROJECT_SOURCE_DIR}/${TARGET} ${TARGET}LIST_SRC)
    aux_source_directory(${PROJECT_SOURCE_DIR}/protos ${TARGET}LIST_SRC)
    add_executable(${TARGET} ${${TARGET}LIST_SRC})
    
    target_link_libraries(${TARGET} sheep_cpp pthread dl)

    include_directories(${PROJECT_SOURCE_DIR}/protos)
    include_directories(${PROJECT_SOURCE_DIR}/${TARGET})
endforeach()