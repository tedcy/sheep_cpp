cmake_minimum_required(VERSION 3.0)

project(sheep_cpp_core)
set(FINAL_OUTPUT "sheep_cpp")

enable_language(C ASM)

set(BasePath "${CMAKE_SOURCE_DIR}/src")

set(CMAKE_INSTALL_PREFIX "/root/sheep_cpp-version/sheep_cpp-1.0.0")

#init Dirs
list(APPEND Dirs ${BasePath}/small_server)
list(APPEND Dirs ${BasePath}/small_client)
list(APPEND Dirs ${BasePath}/net)
list(APPEND Dirs ${BasePath}/log)
list(APPEND Dirs ${BasePath}/extends)

#init IgnoreDirs
list(APPEND IgnoreDirs "test")
list(APPEND IgnoreDirs "build")
function(IGNORE_DIR INPUT_SOURCE_FILES_VAR)
    set(SOURCE_FILES_TEMP ${${INPUT_SOURCE_FILES_VAR}})  # 获取传入变量的值
    foreach(Dir ${IgnoreDirs})
        list(FILTER SOURCE_FILES_TEMP EXCLUDE REGEX ${Dir})
    endforeach()
    set(${INPUT_SOURCE_FILES_VAR} ${SOURCE_FILES_TEMP} PARENT_SCOPE)  # 将修改的列表传回父作用域
endfunction()

#get all source files
foreach(Dir ${Dirs})
    file(GLOB_RECURSE SOURCE_FILES_TEMP ${Dir} ${Dir}/*.cpp)
    IGNORE_DIR(SOURCE_FILES_TEMP)
    list(APPEND SOURCE_FILES ${SOURCE_FILES_TEMP})
endforeach()

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

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})

#merge all static libraries
function(combine_archives output_archive list_of_input_archives dependencies)
    set(mri_file ${CMAKE_BINARY_DIR}/${output_archive}.mri)
    set(FULL_OUTPUT_PATH ${output_archive})
    file(WRITE ${mri_file} "create lib${FULL_OUTPUT_PATH}.a\n")
    FOREACH(in_archive ${list_of_input_archives})
        file(APPEND ${mri_file} "addlib ${in_archive}\n")
    ENDFOREACH()
    file(APPEND ${mri_file} "save\n")
    file(APPEND ${mri_file} "end\n")

    set(output_archive_dummy_file ${CMAKE_BINARY_DIR}/${output_archive}.dummy.cpp)
    add_custom_command(
        OUTPUT ${output_archive_dummy_file}
        COMMAND touch ${output_archive_dummy_file}
        DEPENDS ${dependencies}
    )

    add_library(${output_archive} STATIC ${output_archive_dummy_file})
    add_custom_command(
        TARGET ${output_archive}
        POST_BUILD
        COMMAND ar -M < ${mri_file}
        DEPENDS ${dependencies}
        COMMENT "Merging all static libraries into: ${output_archive}"
    )
endfunction(combine_archives)

list(APPEND FINAL_PATHS ${CMAKE_BINARY_DIR}/lib${PROJECT_NAME}.a)
list(APPEND FINAL_PATHS ${THIRDPARTY_LIBS})
list(APPEND FINAL_PATHS ${SYS_THIRDPARTY_LIBS})
combine_archives(${FINAL_OUTPUT} "${FINAL_PATHS}" ${PROJECT_NAME})
#不能install targets，因为custom_target不会产生实际文件
install(FILES ${CMAKE_BINARY_DIR}/lib${FINAL_OUTPUT}.a DESTINATION "lib") 

#add dependencies
add_dependencies(${PROJECT_NAME}
    tinytoml_external_project
    rapidjson_external_project
    http-parser_external_project
    hiredis_external_project
    gflags_external_project
    glog_external_project
    grpc_external_project
    libunwind_external_project
)

#add Dirs include
function(GET_SUB_DIRS result curdir)
    file(GLOB_RECURSE all_content LIST_DIRECTORIES true ${curdir} ${curdir}/*)

    set(subdirs ${curdir})
    foreach(item IN LISTS all_content)
        if(IS_DIRECTORY ${item})
            list(APPEND subdirs ${item})
        endif()
    endforeach()
    set(${result} ${subdirs} PARENT_SCOPE)
    #message(STATUS "${curdir} temp dir: ${subdirs}")
endfunction()

target_include_directories(${PROJECT_NAME} PRIVATE ${BasePath})
foreach(Dir ${Dirs})
    GET_SUB_DIRS(SUBDIRS ${Dir})
    IGNORE_DIR(SUBDIRS)
    foreach(SUBDIR IN LISTS SUBDIRS)
        #message(STATUS "include dir: ${SUBDIR}")
        target_include_directories(${PROJECT_NAME} PRIVATE ${SUBDIR})
    endforeach()
endforeach()

install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION "lib")

#install headers
file(GLOB_RECURSE HEADER_FILES ${BasePath} ${BasePath}/*.h)
foreach(HEADER_FILE ${HEADER_FILES})
    get_filename_component(FILE_PATH "${HEADER_FILE}" PATH)
    string(REPLACE ${BasePath} "include" TARGET_PATH "${FILE_PATH}")
    install(FILES "${HEADER_FILE}" DESTINATION "${TARGET_PATH}")
endforeach()