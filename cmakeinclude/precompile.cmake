# 预编译头文件(Precompiled Header Files, PCH Files)相关函数

function(precompile_header HEADER addition_flags)
    if(CMAKE_BUILD_TYPE)
        string(TOUPPER ${CMAKE_BUILD_TYPE} BUILDTYPE_UPPER)
        string(TOLOWER ${CMAKE_BUILD_TYPE} BUILDTYPE_LOWER)
        set(BUILDTYPE_SUBFIX "_${BUILDTYPE_UPPER}")
    endif()

    #设置输出目录
    set(pch_output_dir ${CMAKE_CURRENT_BINARY_DIR}/${BUILDTYPE_LOWER})
    set(output_header ${pch_output_dir}/${HEADER})

    configure_file(${HEADER} ${output_header})

    SET(_CMAKE_CURRENT_BINARY_DIR_included_before_path FALSE)

    #获取编译参数
    GET_DIRECTORY_PROPERTY(_directory_flags INCLUDE_DIRECTORIES)
    FOREACH(item ${_directory_flags})
        LIST(APPEND include_flags "-I${item}")
    ENDFOREACH(item)
    #defines, no -include
    get_directory_property(_all_define_flags DEFINITIONS)
    string(REGEX REPLACE "-include +[^ ]+"
         "" define_flags
         ${_all_define_flags})

    #检查gch是否存在
    set(GCH_FILE_NAME "${output_header}.gch")

    #message("precompile ${HEADER} header...")
    add_custom_command(
        OUTPUT ${GCH_FILE_NAME}
        COMMAND ${CMAKE_CXX_COMPILER} ${output_header}
                 ${CMAKE_CXX_FLAGS${BUILDTYPE_SUBFIX}}
                 ${addition_flags}
                 ${include_flags}
                 ${define_flags}
        MAIN_DEPENDENCY ${output_header}
        IMPLICIT_DEPENDS C ${output_header}
        WORKING_DIRECTORY ${pch_output_dir}
        COMMENT "precompile ${HEADER} header..."
        )

    string(REPLACE "." "_" pch_target "${HEADER}")
    add_custom_target(${pch_target} ALL
        DEPENDS ${GCH_FILE_NAME})

    ADD_DEPENDENCIES(logic ${pch_target})

    add_custom_target(${pch_target}_clean
         COMMAND rm ${GCH_FILE_NAME})
    #导出pch路径
    set(ENABLE_PRECOMPILE ${output_header} PARENT_SCOPE)
endfunction()

function(target_use_pch target)
    if((NOT MSVC) AND ENABLE_PRECOMPILE)
        set_target_properties(${target}
                PROPERTIES COMPILE_DEFINITIONS "ENABLE_PRECOMPILE -Winvalid-pch -include ${ENABLE_PRECOMPILE}")
    else()
        message(AUTHOR_WARNING "Target:${target} is using precompiled header, but no header was precompiled.")
    endif()
endfunction()

function(dirctory_use_pch target)
    if((NOT MSVC) AND ENABLE_PRECOMPILE)
        set_property(DIRECTORY ${target}
                APPEND_STRING PROPERTY COMPILE_DEFINITIONS "ENABLE_PRECOMPILE -Winvalid-pch -include ${ENABLE_PRECOMPILE}")
    else()
        message(AUTHOR_WARNING "Directory:${target} is using precompiled header, but no header was precompiled.")
    endif()
endfunction()
