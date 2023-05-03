set(TINY_TOML_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/tinytoml)
set(TINY_TOML_INCLUDE_DIR ${TINY_TOML_INSTALL_DIR}/include)

ExternalProject_Add(tinytoml_external_project
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/tinytoml"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
                  <SOURCE_DIR>/include ${TINY_TOML_INCLUDE_DIR}
)

include_directories(BEFORE SYSTEM ${TINY_TOML_INCLUDE_DIR})
install(DIRECTORY ${TINY_TOML_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})