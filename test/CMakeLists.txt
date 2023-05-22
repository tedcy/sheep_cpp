cmake_minimum_required(VERSION 3.0)

project(${PROJECT_NAME})

set(SheepLibs ${FINAL_OUTPUT} pthread dl gtest gmock)

list(APPEND GenProtoPaths ${CMAKE_SOURCE_DIR}/test/small_server/protos)
list(APPEND GenProtoPaths ${CMAKE_SOURCE_DIR}/test/small_flow/protos)
foreach(GenProtoPath ${GenProtoPaths})
    set(GRPC_OUTPUT_FILE "${GenProtoPath}/helloworld.grpc.pb.cc")
    set(PROTO_OUTPUT_FILE "${GenProtoPath}/helloworld.pb.cc")
    if(NOT EXISTS ${GRPC_OUTPUT_FILE} OR NOT EXISTS ${PROTO_OUTPUT_FILE})
        add_custom_command(
            OUTPUT gen_proto_${GenProtoPath}
            COMMAND ${CMAKE_BINARY_DIR}/thirdparty/protobuf/bin/protoc -I${GenProtoPath}
                    --grpc_out=${GenProtoPath}
                    --plugin=protoc-gen-grpc=${CMAKE_BINARY_DIR}/thirdparty/grpc/bin/grpc_cpp_plugin
                    ${GenProtoPath}/helloworld.proto
            COMMAND ${CMAKE_BINARY_DIR}/thirdparty/protobuf/bin/protoc -I${GenProtoPath}
                    --cpp_out=${GenProtoPath}
                    ${GenProtoPath}/helloworld.proto
            COMMENT "gen proto ${GenProtoPath}"
            DEPENDS final_target
        )
        string(REPLACE "/" "_" TestTarget ${GenProtoPath})
        set(TestTarget ${TestTarget}_target) 
        add_custom_target(${TestTarget} DEPENDS gen_proto_${GenProtoPath})
        list(APPEND GenProtoTargets ${TestTarget})
    endif()
endforeach()

list(APPEND TestTargetsPath log)
list(APPEND TestTargetsPath context)
list(APPEND TestTargetsPath net/client)
list(APPEND TestTargetsPath net/server)
list(APPEND TestTargetsPath net/common)
list(APPEND TestTargetsPath net/go_cpp/client)
list(APPEND TestTargetsPath net/go_cpp/client_pool)
list(APPEND TestTargetsPath net/go_cpp/server)
list(APPEND TestTargetsPath small_server/client)
list(APPEND TestTargetsPath small_server/server)
list(APPEND TestTargetsPath small_client/bench/bench_redis_client)
list(APPEND TestTargetsPath small_client/http_client)
list(APPEND TestTargetsPath small_client/redis_client)
list(APPEND TestTargetsPath small_watcher)
list(APPEND TestTargetsPath small_grpc)
list(APPEND TestTargetsPath small_http_parser)
list(APPEND TestTargetsPath small_config)
list(APPEND TestTargetsPath small_test)
list(APPEND TestTargetsPath small_flow)
list(APPEND TestTargetsPath small_pprof)

foreach(TestTargetPath ${TestTargetsPath})
    string(REPLACE "/" "_" TestTarget ${TestTargetPath})
    set(TestTarget ${TestTarget}_target) 
    set(TEMP_LIST_SRC "")
    aux_source_directory(${TestTargetPath}/../protos TEMP_LIST_SRC)
    aux_source_directory(${TestTargetPath}/protos TEMP_LIST_SRC)
    add_executable(${TestTarget} ${TestTargetPath}/main.cpp ${TEMP_LIST_SRC})
    target_include_directories(${TestTarget} PRIVATE ${TestTargetPath}/../protos)
    target_include_directories(${TestTarget} PRIVATE ${TestTargetPath}/protos)
    target_include_directories(${TestTarget} PRIVATE ${TestTargetPath})
    add_dependencies(${TestTarget} final_target ${GenProtoTargets})
    target_include_directories(${TestTarget} PRIVATE ${BasePath})
    target_include_directories(${TestTarget} PRIVATE ${CMAKE_SOURCE_DIR}/test)
    foreach(Dir ${Dirs})
        GET_SUB_DIRS(SUBDIRS ${Dir} ${SubDirsCachePath})
        IGNORE_DIR(SUBDIRS)
        foreach(SUBDIR IN LISTS SUBDIRS)
            #message(STATUS "${TestTarget} include dir: ${SUBDIR}")
            target_include_directories(${TestTarget} PRIVATE ${SUBDIR})
        endforeach()
    endforeach()
    target_link_libraries(${TestTarget} ${SheepLibs})
endforeach()

execute_process(COMMAND chmod -R +x ${CMAKE_INSTALL_PREFIX}/bin)