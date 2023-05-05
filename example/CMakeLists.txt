cmake_minimum_required(VERSION 3.2)
project(main)

if(NOT DEFINED SHEEP_INSTALL_DIR)
    message(FATAL_ERROR "SHEEP_INSTALL_DIR is not defined. Please provide it as an argument.")
endif()

if (CMAKE_BUILD_TYPE)
else()
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

set(CMAKE_CXX_FLAGS -std=c++11)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O2")

set(ProtosPath ${PROJECT_SOURCE_DIR}/protos)
add_custom_command(
    OUTPUT gen_proto
    COMMAND ${SHEEP_INSTALL_DIR}/bin/protoc -I${ProtosPath}
            --grpc_out=${ProtosPath}
            --plugin=protoc-gen-grpc=${SHEEP_INSTALL_DIR}/bin/grpc_cpp_plugin
            ${ProtosPath}/helloworld.proto
    COMMAND ${SHEEP_INSTALL_DIR}/bin/protoc -I${ProtosPath}
            --cpp_out=${ProtosPath}
            ${ProtosPath}/helloworld.proto
    COMMENT "gen proto ${GenProtoPath}"
)
add_custom_target(gen_proto_target DEPENDS gen_proto)
    
link_directories(${SHEEP_INSTALL_DIR}/lib)

foreach (TARGET server client)
    aux_source_directory(${PROJECT_SOURCE_DIR}/${TARGET} ${TARGET}LIST_SRC)
    aux_source_directory(${PROJECT_SOURCE_DIR}/protos ${TARGET}LIST_SRC)
    add_executable(${TARGET} ${${TARGET}LIST_SRC})
    add_dependencies(${TARGET} gen_proto_target)
    
    target_include_directories(${TARGET} PRIVATE ${SHEEP_INSTALL_DIR}/include)
    target_link_libraries(${TARGET} sheep_cpp pthread dl)

    target_include_directories(${TARGET} PRIVATE ${PROJECT_SOURCE_DIR}/../test/small_server/${TARGET})
    target_include_directories(${TARGET} PRIVATE ${PROJECT_SOURCE_DIR}/protos)
    target_include_directories(${TARGET} PRIVATE ${PROJECT_SOURCE_DIR}/${TARGET})
endforeach()