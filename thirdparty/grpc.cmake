include(ExternalProject)

set(GRPC_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/grpc)
set(GRPC_INCLUDE_DIR ${GRPC_INSTALL_DIR}/include)
set(GRPC_LIB_DIR ${GRPC_INSTALL_DIR}/lib)
set(GRPC_BIN_DIR ${GRPC_INSTALL_DIR}/bin)
ExternalProject_Add(grpc_external_project
    SOURCE_DIR  ${CMAKE_SOURCE_DIR}/thirdparty/grpc
    CONFIGURE_COMMAND   ${CMAKE_COMMAND} -DgRPC_INSTALL=ON 
                        -DCMAKE_BUILD_TYPE=Release
                        -DgRPC_BUILD_TESTS=OFF 
                        -DgRPC_ABSL_PROVIDER=module 
                        -DgRPC_CARES_PROVIDER=module
                        -DgRPC_PROTOBUF_PROVIDER=module
                        -DgRPC_SSL_PROVIDER=package
                        -DgRPC_ZLIB_PROVIDER=module 
                        -DCMAKE_INSTALL_PREFIX=${GRPC_INSTALL_DIR} ${CMAKE_SOURCE_DIR}/thirdparty/grpc
    BUILD_COMMAND       ${CMAKE_COMMAND} --build . -- -j12
    INSTALL_COMMAND     ${CMAKE_COMMAND} --install .
            COMMAND     ${CMAKE_COMMAND} -E copy ${GRPC_LIB_DIR}/libgrpc++.a ${GRPC_LIB_DIR}/libgrpcpp.a
    DEPENDS             gperftools_external_project
)
include_directories(BEFORE SYSTEM ${GRPC_INCLUDE_DIR})
link_directories(${GRPC_LIB_DIR})
list(APPEND THIRDPARTY_LIBS ${GRPC_LIB_DIR}/libgrpcpp.a)
list(APPEND THIRDPARTY_LIBS ${GRPC_LIB_DIR}/libgrpc.a)
list(APPEND THIRDPARTY_LIBS ${GRPC_LIB_DIR}/libgpr.a)
list(APPEND THIRDPARTY_LIBS ${GRPC_LIB_DIR}/libupb.a)
list(APPEND THIRDPARTY_LIBS ${GRPC_LIB_DIR}/libaddress_sorting.a)
list(APPEND THIRDPARTY_LIBS ${GRPC_LIB_DIR}/libcares.a)
list(APPEND THIRDPARTY_LIBS ${GRPC_LIB_DIR}/libabsl_base.a)
list(APPEND THIRDPARTY_LIBS ${GRPC_LIB_DIR}/libabsl_strings.a)
list(APPEND THIRDPARTY_LIBS ${GRPC_LIB_DIR}/libabsl_throw_delegate.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/librt.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/libz.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/libssl.a)
list(APPEND SYS_THIRDPARTY_LIBS /usr/lib/x86_64-linux-gnu/libcrypto.a)
install(DIRECTORY ${GRPC_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${GRPC_BIN_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX} USE_SOURCE_PERMISSIONS)

set(PROTOBUF_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/protobuf)
set(PROTOBUF_INCLUDE_DIR ${PROTOBUF_INSTALL_DIR}/include)
set(PROTOBUF_LIB_DIR ${PROTOBUF_INSTALL_DIR}/lib)
set(PROTOBUF_BIN_DIR ${PROTOBUF_INSTALL_DIR}/bin)
ExternalProject_Add(protobuf_external_project
    SOURCE_DIR  ${CMAKE_SOURCE_DIR}/thirdparty/grpc/third_party/protobuf
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       $(MAKE) -j12
    BUILD_IN_SOURCE     1
    INSTALL_COMMAND     $(MAKE) install prefix=${PROTOBUF_INSTALL_DIR}
    DEPENDS             grpc_external_project
)
ExternalProject_Add_Step(protobuf_external_project SetExecutablePermissions
    COMMAND chmod -R +x ${PROTOBUF_INSTALL_DIR}/bin
    DEPENDEES install
)
include_directories(BEFORE SYSTEM ${PROTOBUF_INCLUDE_DIR})
link_directories(${PROTOBUF_LIB_DIR})
list(APPEND THIRDPARTY_LIBS ${PROTOBUF_LIB_DIR}/libprotobuf.a)
install(DIRECTORY ${PROTOBUF_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${PROTOBUF_BIN_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX} USE_SOURCE_PERMISSIONS)