include(ExternalProject)

set(HIREDIS_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/hiredis)
set(HIREDIS_INCLUDE_DIR ${HIREDIS_INSTALL_DIR}/include)
set(HIREDIS_LIB_DIR ${HIREDIS_INSTALL_DIR}/lib)
ExternalProject_Add(hiredis_external_project
    SOURCE_DIR  ${CMAKE_SOURCE_DIR}/thirdparty/hiredis
    PREFIX      ${HIREDIS_INSTALL_DIR}
    INSTALL_DIR ${HIREDIS_INSTALL_DIR}
    CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX:PATH=${HIREDIS_INSTALL_DIR}
                -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
                )
include_directories(BEFORE SYSTEM ${HIREDIS_INCLUDE_DIR})
link_directories(${HIREDIS_LIB_DIR})
install(DIRECTORY ${HIREDIS_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})