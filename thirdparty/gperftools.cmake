include(ExternalProject)

set(GPERFTOOLS_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/gperftools)
set(GPERFTOOLS_INCLUDE_DIR ${GPERFTOOLS_INSTALL_DIR}/include)
set(GPERFTOOLS_LIB_DIR ${GPERFTOOLS_INSTALL_DIR}/lib)
ExternalProject_Add(gperftools_external_project
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/gperftools/
    PREFIX ${GPERFTOOLS_INSTALL_DIR}
    CONFIGURE_COMMAND ./autogen.sh && ./configure 
        --prefix=${GPERFTOOLS_INSTALL_DIR}
        --enable-libunwind
        CPPFLAGS=-I${CMAKE_BINARY_DIR}/thirdparty/libunwind/include
        LDFLAGS=-L${CMAKE_BINARY_DIR}/thirdparty/libunwind/lib
        --enable-shared=no
        --with-pic
    BUILD_COMMAND       $(MAKE) -j12
    BUILD_IN_SOURCE     1
    INSTALL_COMMAND     $(MAKE) install
    DEPENDS     libunwind_external_project
)
include_directories(BEFORE SYSTEM ${GPERFTOOLS_INCLUDE_DIR})
link_directories(${GPERFTOOLS_LIB_DIR})
list(APPEND THIRDPARTY_LIBS ${GPERFTOOLS_LIB_DIR}/libtcmalloc_and_profiler.a)
install(DIRECTORY ${GPERFTOOLS_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})