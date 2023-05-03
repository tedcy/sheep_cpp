include(ExternalProject)

set(GLOG_INSTALL_DIR ${CMAKE_BINARY_DIR}/thirdparty/glog)
set(GLOG_INCLUDE_DIR ${GLOG_INSTALL_DIR}/include)
set(GLOG_LIB_DIR ${GLOG_INSTALL_DIR}/lib)
ExternalProject_Add(glog_external_project
    SOURCE_DIR  ${CMAKE_SOURCE_DIR}/thirdparty/glog
    PREFIX      ${GLOG_INSTALL_DIR}
    INSTALL_DIR ${GLOG_INSTALL_DIR}
    CMAKE_ARGS  -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                -DCMAKE_CXX_FLAGS=${EXTERNAL_PROJECT_CMAKE_CXX_FLAGS}
                -DCMAKE_INSTALL_PREFIX:PATH=${GLOG_INSTALL_DIR}
                -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
    DEPENDS     gflags_external_project
    )
include_directories(BEFORE SYSTEM ${GLOG_INCLUDE_DIR})
link_directories(${GLOG_LIB_DIR})
list(APPEND THIRDPARTY_LIBS thirdparty/glog/lib/libglog.a)
install(DIRECTORY ${GLOG_INCLUDE_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})