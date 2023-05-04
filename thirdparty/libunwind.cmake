include(ExternalProject)

set(LIBUNWIND_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/libunwind)
set(LIBUNWIND_INCLUDE_DIR ${LIBUNWIND_INSTALL_DIR}/include)
set(LIBUNWIND_LIB_DIR ${LIBUNWIND_INSTALL_DIR}/lib)
ExternalProject_Add(libunwind_external_project
    PREFIX ${LIBUNWIND_INSTALL_DIR}
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/libunwind/
    CONFIGURE_COMMAND ./autogen.sh && ./configure
        --prefix=${LIBUNWIND_INSTALL_DIR}
        --enable-shared=no
        --disable-minidebuginfo
        "CFLAGS=-fPIC -g -O2"
    BUILD_COMMAND       $(MAKE) -j12
    BUILD_IN_SOURCE     1
    INSTALL_COMMAND     $(MAKE) install
)
include_directories(BEFORE SYSTEM ${LIBUNWIND_INCLUDE_DIR})
link_directories(${LIBUNWIND_LIB_DIR})
list(APPEND THIRDPARTY_LIBS ${LIBUNWIND_LIB_DIR}/libunwind.a)
install(DIRECTORY ${LIBUNWIND_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})