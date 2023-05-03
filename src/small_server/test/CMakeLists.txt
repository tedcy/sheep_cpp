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

set(SHEEP_INSTALL_DIR "/root/sheep_cpp-version/sheep_cpp-1.0.0")
foreach (TARGET server grpc_client)
    include_directories(${SHEEP_INSTALL_DIR}/include)
    link_directories(${SHEEP_INSTALL_DIR}/lib)

    aux_source_directory(${PROJECT_SOURCE_DIR}/${TARGET} ${TARGET}LIST_SRC)
    aux_source_directory(${PROJECT_SOURCE_DIR}/protos/test ${TARGET}LIST_SRC)
    add_executable(${TARGET} ${${TARGET}LIST_SRC})
    
    target_link_libraries(${TARGET} /root/sheep_cpp/build/libsheep_cpp.a)
    target_link_libraries(${TARGET} /root/sheep_cpp/build/thirdparty/glog/lib/libglog.a)
    target_link_libraries(${TARGET} /root/sheep_cpp/build/thirdparty/gflags/lib/libgflags.a)
    target_link_libraries(${TARGET} /root/sheep_cpp/thirdparty/http-parser/libhttp_parser.a)
    target_link_libraries(${TARGET} z)
    target_link_libraries(${TARGET} ssl)
    target_link_libraries(${TARGET} crypto)
    target_link_libraries(${TARGET} /root/taf-version/taf-3.4.5.8-coro-leak-check/lib/libunwind.a)
    #target_link_libraries(${TARGET} libsheep_cpp.a)
    target_link_libraries(${TARGET} pthread)

    include_directories(${PROJECT_SOURCE_DIR}/protos/test)
    include_directories(${PROJECT_SOURCE_DIR}/${TARGET})
endforeach()