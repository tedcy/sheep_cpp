cmake_minimum_required(VERSION 3.0)

project(sheep_cpp_core)
set(FINAL_OUTPUT "sheep_cpp")

enable_language(C ASM)

include(${CMAKE_SOURCE_DIR}/cmake/utils.cmake)

set(BasePath ${CMAKE_SOURCE_DIR}/src)
set(SubDirsCachePath ${CMAKE_BINARY_DIR}/subdirs_cache)
set(SourceFilesCachePath ${CMAKE_BINARY_DIR}/source_files_cache)
set(HeadFilesCachePath ${CMAKE_BINARY_DIR}/head_files_cache)

set(CMAKE_INSTALL_PREFIX "/root/sheep_cpp-version/sheep_cpp-1.0.1")

#init Dirs
list(APPEND Dirs ${BasePath}/small_server)
list(APPEND Dirs ${BasePath}/small_client)
list(APPEND Dirs ${BasePath}/net)
list(APPEND Dirs ${BasePath}/log)
list(APPEND Dirs ${BasePath}/extends)

#get all source files
if (EXISTS ${SourceFilesCachePath})
    file(READ ${SourceFilesCachePath} SOURCE_FILES)
    message(STATUS "read source files from cache ${SourceFilesCachePath}")
else()
    foreach(Dir ${Dirs})
        file(GLOB_RECURSE SOURCE_FILES_TEMP ${Dir} ${Dir}/*.cpp)
        IGNORE_DIR(SOURCE_FILES_TEMP)
        list(APPEND SOURCE_FILES ${SOURCE_FILES_TEMP})
    endforeach()
    file(WRITE ${SourceFilesCachePath} "${SOURCE_FILES}")
    message(STATUS "write source files to cache ${SourceFilesCachePath}")
endif()


#add thirdparty
set (THIRDPARTY_LIBS "")
set (SYS_THIRDPARTY_LIBS "")
include(${CMAKE_SOURCE_DIR}/thirdparty/rapidjson.cmake)
include(${CMAKE_SOURCE_DIR}/thirdparty/http-parser.cmake)
include(${CMAKE_SOURCE_DIR}/thirdparty/hiredis.cmake)
include(${CMAKE_SOURCE_DIR}/thirdparty/gflags.cmake)
include(${CMAKE_SOURCE_DIR}/thirdparty/glog.cmake)
include(${CMAKE_SOURCE_DIR}/thirdparty/grpc.cmake)
include(${CMAKE_SOURCE_DIR}/thirdparty/tinytoml.cmake)
include(${CMAKE_SOURCE_DIR}/thirdparty/libunwind.cmake)
include(${CMAKE_SOURCE_DIR}/thirdparty/googletest.cmake)
include(${CMAKE_SOURCE_DIR}/thirdparty/gperftools.cmake)
include(${CMAKE_SOURCE_DIR}/thirdparty/gperftools-httpd.cmake)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

#merge all static libraries
list(APPEND FINAL_PATHS ${CMAKE_BINARY_DIR}/lib${PROJECT_NAME}.a)
list(APPEND FINAL_PATHS ${THIRDPARTY_LIBS})
list(APPEND FINAL_PATHS ${SYS_THIRDPARTY_LIBS})
combine_archives(${FINAL_OUTPUT} "${FINAL_PATHS}" ${PROJECT_NAME})
add_custom_target(final_target DEPENDS ${FINAL_OUTPUT})
#不能install targets，因为custom_target不会产生实际文件
install(FILES ${CMAKE_BINARY_DIR}/lib${FINAL_OUTPUT}.a DESTINATION "lib") 

add_dependencies(${PROJECT_NAME}
    tinytoml_external_project
    rapidjson_external_project
    http-parser_external_project
    hiredis_external_project
    gflags_external_project
    glog_external_project
    grpc_external_project
    libunwind_external_project
    googletest_external_project
    gperftools_external_project
    gperftools-httpd_external_project
)

#add Dirs include
target_include_directories(${PROJECT_NAME} PRIVATE ${BasePath})
foreach(Dir ${Dirs})
    GET_SUB_DIRS(SUBDIRS ${Dir} ${SubDirsCachePath})
    IGNORE_DIR(SUBDIRS)
    foreach(SUBDIR IN LISTS SUBDIRS)
        #message(STATUS "include dir: ${SUBDIR}")
        target_include_directories(${PROJECT_NAME} PRIVATE ${SUBDIR})
    endforeach()
endforeach()

install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION "lib")

#install headers
if (EXISTS ${HeadFilesCachePath}) 
    file(READ ${HeadFilesCachePath} HEADER_FILES)
    message(STATUS "read header files from cache ${HeadFilesCachePath}")
else()
    file(GLOB_RECURSE HEADER_FILES ${BasePath} ${BasePath}/*.h)
    file(WRITE ${HeadFilesCachePath} "${HEADER_FILES}")
    message(STATUS "write header files to cache ${HeadFilesCachePath}")
endif()

foreach(HEADER_FILE ${HEADER_FILES})
    get_filename_component(FILE_PATH "${HEADER_FILE}" PATH)
    string(REPLACE ${BasePath} "include" TARGET_PATH "${FILE_PATH}")
    install(FILES "${HEADER_FILE}" DESTINATION "${TARGET_PATH}")
endforeach()

add_subdirectory(test)