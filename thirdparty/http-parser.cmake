include(ExternalProject)

set(HTTP_PARSER_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/http-parser)
set(HTTP_PARSER_INCLUDE_DIR ${HTTP_PARSER_INSTALL_DIR}/include)
set(HTTP_PARSER_LIB_DIR ${HTTP_PARSER_INSTALL_DIR}/lib)
ExternalProject_Add(http-parser_external_project
    SOURCE_DIR  ${CMAKE_SOURCE_DIR}/thirdparty/http-parser
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       $(MAKE) -j12 package
    BUILD_IN_SOURCE     1
    INSTALL_COMMAND     $(MAKE) install PREFIX=${HTTP_PARSER_INSTALL_DIR}
)
include_directories(BEFORE SYSTEM ${HTTP_PARSER_INCLUDE_DIR})
link_directories(${HTTP_PARSER_LIB_DIR})
list(APPEND THIRDPARTY_LIBS ${CMAKE_SOURCE_DIR}/thirdparty/http-parser/libhttp_parser.a)
install(DIRECTORY ${HTTP_PARSER_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})