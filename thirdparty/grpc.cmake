include(ExternalProject)

set(GRPC_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/grpc)
set(GRPC_INCLUDE_DIR ${GRPC_INSTALL_DIR}/include)
set(GRPC_LIB_DIR ${GRPC_INSTALL_DIR}/lib)
set(GRPC_BIN_DIR ${GRPC_INSTALL_DIR}/bin)
ExternalProject_Add(grpc_external_project
    SOURCE_DIR  ${CMAKE_SOURCE_DIR}/thirdparty/grpc
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       $(MAKE) -j12
    BUILD_IN_SOURCE     1
    INSTALL_COMMAND     $(MAKE) install prefix=${GRPC_INSTALL_DIR}
            COMMAND     ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/thirdparty/grpc/libs/opt/protobuf/libprotobuf.a ${GRPC_LIB_DIR}/libprotobuf.a
            COMMAND     ${CMAKE_COMMAND} -E copy ${GRPC_LIB_DIR}/libgrpc++.a ${GRPC_LIB_DIR}/libgrpcpp.a
            COMMAND     ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/thirdparty/grpc/bins/opt/protobuf/protoc ${GRPC_BIN_DIR}/protoc
)
include_directories(BEFORE SYSTEM ${GRPC_INCLUDE_DIR})
link_directories(${GRPC_LIB_DIR})
list(APPEND THIRDPARTY_LIBS ${GRPC_LIB_DIR}/libgrpcpp.a)
list(APPEND THIRDPARTY_LIBS ${GRPC_LIB_DIR}/libgrpc.a)
list(APPEND THIRDPARTY_LIBS ${GRPC_LIB_DIR}/libprotobuf.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/librt.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/libz.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/libssl.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/libcrypto.a)
install(DIRECTORY ${GRPC_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${GRPC_BIN_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})