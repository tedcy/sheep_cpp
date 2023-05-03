include(ExternalProject)

set(GPRC_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/grpc)
set(GPRC_INCLUDE_DIR ${GPRC_INSTALL_DIR}/include)
set(GPRC_LIB_DIR ${GPRC_INSTALL_DIR}/lib)
set(GPRC_BIN_DIR ${GPRC_INSTALL_DIR}/bin)
ExternalProject_Add(grpc_external_project
    SOURCE_DIR  ${CMAKE_SOURCE_DIR}/thirdparty/grpc
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       $(MAKE) -j12
    BUILD_IN_SOURCE     1
    INSTALL_COMMAND     $(MAKE) install prefix=${GPRC_INSTALL_DIR}
)
include_directories(BEFORE SYSTEM ${GPRC_INCLUDE_DIR})
link_directories(${GPRC_LIB_DIR})
list(APPEND THIRDPARTY_LIBS thirdparty/grpc/lib/libgrpc++.a)
list(APPEND THIRDPARTY_LIBS thirdparty/grpc/lib/libgrpc.a)
list(APPEND THIRDPARTY_LIBS thirdparty/grpc/lib/libprotobuf.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/librt.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/libssl.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/libcrypto.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/libz.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/libpthread.a)
install(DIRECTORY ${GPRC_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${GPRC_BIN_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})
#install(PROGRAMS ${GPRC_BIN_DIR_EXTERNAL}/grpc_cpp_plugin DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)