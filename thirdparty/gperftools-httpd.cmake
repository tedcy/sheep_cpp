include(ExternalProject)

set(GPERFTOOLS_HTTPD_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/gperftools-httpd)
set(GPERFTOOLS_HTTPD_INCLUDE_DIR ${GPERFTOOLS_HTTPD_INSTALL_DIR}/include)
set(GPERFTOOLS_HTTPD_LIB_DIR ${GPERFTOOLS_HTTPD_INSTALL_DIR}/lib)
ExternalProject_Add(gperftools-httpd_external_project
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/gperftools-httpd/
    PREFIX ${GPERFTOOLS_HTTPD_INSTALL_DIR}
    INSTALL_DIR ${GPERFTOOLS_HTTPD_INSTALL_DIR}
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       $(MAKE) -j12 libghttpd.a
    BUILD_IN_SOURCE     1
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/thirdparty/gperftools-httpd/libghttpd.a
                ${GPERFTOOLS_HTTPD_LIB_DIR}/libghttpd.a
            COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/thirdparty/gperftools-httpd/gperftools-httpd.h
                ${GPERFTOOLS_HTTPD_INCLUDE_DIR}/gperftools-httpd.h
    DEPENDS     gperftools_external_project
)
include_directories(BEFORE SYSTEM ${GPERFTOOLS_HTTPD_INCLUDE_DIR})
link_directories(${GPERFTOOLS_HTTPD_LIB_DIR})
list(APPEND THIRDPARTY_LIBS ${GPERFTOOLS_HTTPD_LIB_DIR}/libghttpd.a)
install(DIRECTORY ${GPERFTOOLS_HTTPD_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})