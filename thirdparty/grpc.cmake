include(ExternalProject)

set(GPRC_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/grpc)
set(GPRC_INCLUDE_DIR ${GPRC_INSTALL_DIR}/include)
set(GPRC_LIB_DIR ${GPRC_INSTALL_DIR}/lib)
set(GPRC_BIN_DIR ${GPRC_INSTALL_DIR}/bin)
ExternalProject_Add(grpc_external_project_core
    SOURCE_DIR  ${CMAKE_SOURCE_DIR}/thirdparty/grpc
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       $(MAKE) -j12
    BUILD_IN_SOURCE     1
    INSTALL_COMMAND     $(MAKE) install prefix=${GPRC_INSTALL_DIR}
)
include_directories(BEFORE SYSTEM ${GPRC_INCLUDE_DIR})
link_directories(${GPRC_LIB_DIR})
list(APPEND THIRDPARTY_LIBS ${GPRC_LIB_DIR}/libgrpcpp.a)
list(APPEND THIRDPARTY_LIBS ${GPRC_LIB_DIR}/libgrpc.a)
list(APPEND THIRDPARTY_LIBS ${CMAKE_SOURCE_DIR}/thirdparty/grpc/libs/opt/protobuf/libprotobuf.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/librt.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/libz.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/libssl.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/libcrypto.a)
install(DIRECTORY ${GPRC_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${GPRC_BIN_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})
add_custom_command(
    OUTPUT rename_grpcpp
    COMMAND ${CMAKE_COMMAND} -E rename
        ${GPRC_LIB_DIR}/libgrpc++.a
        ${GPRC_LIB_DIR}/libgrpcpp.a || true
        COMMENT "Copying 'libgrpc.a' library to 'libgrpcpp.a'"
    DEPENDS grpc_external_project_core
)
add_custom_target(grpc_external_project DEPENDS rename_grpcpp)