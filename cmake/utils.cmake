function(GET_SUB_DIRS result curdir cache_file_prefix)
    # 检查缓存文件是否存在
    set(cache_file ${cache_file_prefix}/${curdir}.txt)
    if(EXISTS ${cache_file})
        # 从缓存文件读取子目录
        file(READ ${cache_file} subdirs)
        message(STATUS "reading from cache file: ${cache_file}")
    else()
        file(GLOB_RECURSE all_content LIST_DIRECTORIES true ${curdir} ${curdir}/*)

        set(subdirs ${curdir})
        foreach(item IN LISTS all_content)
            if(IS_DIRECTORY ${item})
                list(APPEND subdirs ${item})
            endif()
        endforeach()

        # 将子目录写入缓存文件
        file(WRITE ${cache_file} "${subdirs}")
        message(STATUS "writing to cache file: ${cache_file}")
    endif()

    set(${result} ${subdirs} PARENT_SCOPE)
    #message(STATUS "${curdir} temp dir: ${subdirs}")
endfunction()

list(APPEND IgnoreDirs "test")
list(APPEND IgnoreDirs "build")
function(IGNORE_DIR INPUT_SOURCE_FILES_VAR)
    set(SOURCE_FILES_TEMP ${${INPUT_SOURCE_FILES_VAR}})  # 获取传入变量的值
    foreach(Dir ${IgnoreDirs})
        list(FILTER SOURCE_FILES_TEMP EXCLUDE REGEX ${Dir})
    endforeach()
    set(${INPUT_SOURCE_FILES_VAR} ${SOURCE_FILES_TEMP} PARENT_SCOPE)  # 将修改的列表传回父作用域
endfunction()

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