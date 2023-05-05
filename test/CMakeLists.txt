cmake_minimum_required(VERSION 3.0)

project(${PROJECT_NAME})

set(SheepLibs ${FINAL_OUTPUT} pthread dl gtest gmock)

list(APPEND GenProtoPaths ${CMAKE_SOURCE_DIR}/test/small_server/protos)
list(APPEND GenProtoPaths ${CMAKE_SOURCE_DIR}/test/small_flow/protos)
foreach(GenProtoPath ${GenProtoPaths})
    add_custom_command(
        OUTPUT gen_proto_${GenProtoPath}
        COMMAND ${CMAKE_BINARY_DIR}/thirdparty/grpc/bin/protoc -I${GenProtoPath}/proto
                --grpc_out=${GenProtoPath}
                --plugin=protoc-gen-grpc=${CMAKE_BINARY_DIR}/thirdparty/grpc/bin/grpc_cpp_plugin
                ${GenProtoPath}/proto/helloworld.proto
        COMMENT "gen proto ${GenProtoPath}"
        DEPENDS final_target
    )
    string(REPLACE "/" "_" TestTarget ${GenProtoPath})
    set(TestTarget ${TestTarget}_target) 
    add_custom_target(${TestTarget} DEPENDS gen_proto_${GenProtoPath})
    list(APPEND GenProtoTargets ${TestTarget})
endforeach()

list(APPEND TestTargetsPath log)
list(APPEND TestTargetsPath net/client)
list(APPEND TestTargetsPath net/server)
list(APPEND TestTargetsPath net/common)
list(APPEND TestTargetsPath net/go_cpp/client)
list(APPEND TestTargetsPath net/go_cpp/client_pool)
list(APPEND TestTargetsPath net/go_cpp/server)
list(APPEND TestTargetsPath small_server/grpc_client)
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
    add_executable(${TestTarget} ${TestTargetPath}/main.cpp)
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