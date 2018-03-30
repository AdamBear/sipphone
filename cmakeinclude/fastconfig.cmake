#===============================================================================
# add_all_subdirectories()
#   Add all subdirectories that exist CMakeLists.txt
#
#
# add_fast_library/add_fast_executable([PARAMS...])
#   Build current source directory as executable or library, target name is current directory name.
#   INPUT:
#       PARAMS params pass to add_library os add_executable
#
#===============================================================================
# Functions of "Collection" Target Mode
#
# FAST_CONFIG_COLLECTION_SOURCE
#   use source file mode instead object mode
#
# collection_build_begin()
#   Start a "Collection" target, enter "Collection" mode, target name is current directory name.
#
# collect()
#   Add current directory to the "Collection" target.
#
# add_collection_library/add_collection_executable([PARAMS...])
#   Build "Collection" target as executable or library, then exit the "Collection" mode.
#   INPUT:
#       PARAMS params pass to add_library os add_executable
#
# fast_collect_library/fast_collect_executable(TARGET_NAME AGRV)
#   Add all sub directory sources and header into target name.
#=============================================================================
# Copyright (c) 2016, v2abcd
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================

include(CMakeParseArguments)

function(add_all_subdirectories)
    cmake_parse_arguments(SUBDIR "" "" "EXCLUDE" ${ARGN})
    file(GLOB ALL_FILES *)

    foreach(subdir ${ALL_FILES})
        if((IS_DIRECTORY ${subdir})
            AND (EXISTS ${subdir}/CMakeLists.txt))
            file(RELATIVE_PATH relPath ${CMAKE_CURRENT_SOURCE_DIR} ${subdir})
            if(SUBDIR_EXCLUDE)
                list(FIND SUBDIR_EXCLUDE ${relPath} _index)
                if (_index EQUAL -1)
                    add_subdirectory(${subdir})
                elseif(CMAKE_DEBUG)
                    message("Exclude [${relPath}] when add_all_subdirectories")
                endif()
            else()
                add_subdirectory(${subdir})
            endif()
        endif()
    endforeach()
endfunction()

# 设置VS工程根目录
macro(_WC_SET_FOLDER_ROOT_DIR)
    set(_WC_FOLDER_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
    if(CMAKE_DEBUG)
        message("_WC_FOLDER_ROOT_DIR: ${_WC_FOLDER_ROOT_DIR}")
    endif()
endmacro()

# 设置VS工程相对目录
function(set_target_folder)
    # MSVC环境下才需要设置工程相对目录
    if(MSVC)
        # 检查工程根目录是否设置
        if(NOT _WC_FOLDER_ROOT_DIR)
            message(FATAL_ERROR "Root of folders not found, invoke '_WC_SET_FOLDER_ROOT_DIR' first.")
        endif()
        # 获取当前目录与VS工程根目录的相对路径
        file(RELATIVE_PATH rel_path ${_WC_FOLDER_ROOT_DIR} ${CMAKE_CURRENT_LIST_DIR})
        # 给每一级父文件夹加上`Projects`后缀，避免与同名工程冲突
        string(REPLACE "/" " Projects/" rel_path ${rel_path})
        # 移除最后一级文件夹（已作为工程名`target_name`）
        get_filename_component(rel_path ${rel_path} DIRECTORY)
        if(NOT rel_path STREQUAL "")
            # 设置VS工程相对目录
            set_target_properties(${target_name}
                PROPERTIES FOLDER ${rel_path}
            )
            if(CMAKE_DEBUG)
                message("[${target_name}] FOLDER path: ${rel_path}")
            endif()
        endif()
    endif()
endfunction()

# 获取指定路径下的头文件/源码文件
macro(_WC_GET_SOURCE_FILES_IN_DIR SEARCH_PATH)
    set(HEADER_EXP
        ${SEARCH_PATH}/*.h
        ${SEARCH_PATH}/*.hxx
        ${SEARCH_PATH}/*.hpp
        ${SEARCH_PATH}/*.ixx
        )

    set(SOURCE_EXP
        ${SEARCH_PATH}/*.c
        ${SEARCH_PATH}/*.cc
        ${SEARCH_PATH}/*.cxx
        ${SEARCH_PATH}/*.cpp
        ${SEARCH_PATH}/*.cxx
        )

    if (MSVC)
        list(APPEND SOURCE_EXP ${SEARCH_PATH}/*.rc)
    endif()

    file(GLOB DIR_HEADER_FILES ${HEADER_EXP})
    file(GLOB DIR_SOURCE_FILES ${SOURCE_EXP})

    # 设置VS工程源码目录
    if(MSVC)
        # 当前目录的文件不需要设置路径
        if(NOT ${SEARCH_PATH} STREQUAL ".")
            # 头文件和源码文件都有的时候，需要细分
            if(DIR_HEADER_FILES AND DIR_SOURCE_FILES)
                source_group("${SEARCH_PATH}\\Header Files" FILES ${DIR_HEADER_FILES})
                source_group("${SEARCH_PATH}\\Source Files" FILES ${DIR_SOURCE_FILES})
            else()
                source_group(${SEARCH_PATH}
                    FILES ${DIR_HEADER_FILES} ${DIR_SOURCE_FILES})
            endif()
        endif()
    endif()
endmacro()

macro(_WC_GET_BUILD_DIRECTORY_VARS)
    set(HEADER_FILES)
    set(SOURCE_FILES)
    foreach(SOURCE_PATH . include src)
        _WC_GET_SOURCE_FILES_IN_DIR(${SOURCE_PATH})
        list(APPEND HEADER_FILES ${DIR_HEADER_FILES})
        list(APPEND SOURCE_FILES ${DIR_SOURCE_FILES})
    endforeach()

    # 默认包含include目录
    if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include)
        include_directories(include)
    endif()

    _WC_GENERATE_PROJECT_NAME()
endmacro()


function(add_fast_executable)
    _WC_GET_BUILD_DIRECTORY_VARS()
    _WC_FILTER_MSVC_PCH()
    add_executable(${target_name} ${ARGV}
        ${HEADER_FILES}
        ${SOURCE_FILES}
    )
    _WC_ADD_MSVC_PCH()
    _WC_RESTORE_DISPLAY_NAME()
    set_target_folder()
endfunction()

function(add_fast_library)
    _WC_GET_BUILD_DIRECTORY_VARS()
    _WC_FILTER_MSVC_PCH()
    add_library(${target_name} ${ARGV}
        ${HEADER_FILES}
        ${SOURCE_FILES})
    _WC_ADD_MSVC_PCH()
    _WC_RESTORE_DISPLAY_NAME()
    set_target_folder()
endfunction()

function(add_mock_library)
    if(ENABLE_TEST)
        _WC_GET_BUILD_DIRECTORY_VARS()
        set(target_name ${target_name}mock)
        if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/mock)
            _WC_GET_SOURCE_FILES_IN_DIR(mock)
            add_library(${target_name} ${ARGV}
                ${DIR_HEADER_FILES}
                ${DIR_SOURCE_FILES}
            )
        else()
            add_library(${target_name} ${ARGV}
                ${HEADER_FILES}
                ${SOURCE_FILES}
            )
        endif()
        set_target_folder()
        _WC_SET_TARGET_DISPLAY_NAME(PREFIX mock)
    endif()
endfunction()


function(collection_build_begin)
    cmake_parse_arguments(COLLECTION "" "TARGET_NAME" "" ${ARGN})
    if(NOT COLLECTION_TARGET_NAME)
        _WC_GET_BUILD_DIRECTORY_VARS()
    else()
        set(target_name ${COLLECTION_TARGET_NAME})
        if (MSVC)
            _WC_GET_BUILD_DIRECTORY_VARS()
            _WC_SET_TARGET_DISPLAY_NAME(NAME ${COLLECTION_TARGET_NAME})
        endif()
    endif()
    if(_WC_CURRENT_COLLECTION_TARGET_NAME)
        message(FATAL_ERROR "Called build_begin_collect twice.")
    endif()
    set(_WC_CURRENT_COLLECTION_TARGET_NAME ${target_name} PARENT_SCOPE)
    if(CMAKE_DEBUG)
        message("Collection target ${target_name} begin.")
    endif()
endfunction()

function(collect)
    if(NOT _WC_CURRENT_COLLECTION_TARGET_NAME)
        message(FATAL_ERROR "Current collection target not found.")
    endif()

    _WC_GET_BUILD_DIRECTORY_VARS()
    if(NOT FAST_CONFIG_COLLECTION_SOURCE)
        _WC_FILTER_MSVC_PCH()
        add_library(${target_name} OBJECT
            ${HEADER_FILES}
            ${SOURCE_FILES}
            ${ARGV}
        )
        _WC_ADD_MSVC_PCH()
        set_property(GLOBAL APPEND
            PROPERTY _WC_COLLECTION_${_WC_CURRENT_COLLECTION_TARGET_NAME} ${target_name}
        )

        set_target_folder()

        if(CMAKE_DEBUG)
            message("Collected [${target_name}]")
        endif()
    else()
        set_property(GLOBAL APPEND
            PROPERTY _WC_COLLECTION_${_WC_CURRENT_COLLECTION_TARGET_NAME}_HEADER ${HEADER_FILES}
        )
        set_property(GLOBAL APPEND
            PROPERTY _WC_COLLECTION_${_WC_CURRENT_COLLECTION_TARGET_NAME}_SOURCE ${SOURCE_FILES}
        )
        if(CMAKE_DEBUG)
            foreach(filename ${HEADER_FILES} ${SOURCE_FILES})
                message("Collected [${filename}]")
            endforeach()
        endif()
    endif()
    _WC_RESTORE_DISPLAY_NAME()
endfunction()

macro(_WC_GET_COLLECTION_OBJECTS)
    if(NOT FAST_CONFIG_COLLECTION_SOURCE)
        get_property(target_collection
            GLOBAL PROPERTY _WC_COLLECTION_${_WC_CURRENT_COLLECTION_TARGET_NAME})
        foreach(subtarget IN LISTS target_collection)
            list(APPEND OBJECT_LIST $<TARGET_OBJECTS:${subtarget}>)
        endforeach()
    else()
        get_property(target_collection_header
            GLOBAL PROPERTY _WC_COLLECTION_${_WC_CURRENT_COLLECTION_TARGET_NAME}_HEADER)
        get_property(target_collection_source
            GLOBAL PROPERTY _WC_COLLECTION_${_WC_CURRENT_COLLECTION_TARGET_NAME}_SOURCE)
        list(APPEND OBJECT_LIST ${target_collection_header} ${target_collection_source})
        foreach(header IN LISTS target_collection_header)
            get_filename_component(dirname ${header} PATH)
            string(REGEX MATCH "[^/]+$" dirname ${dirname})
            source_group("${dirname}\\Header Files" FILES ${header})
        endforeach()
        foreach(source IN LISTS target_collection_source)
            get_filename_component(dirname ${source} PATH)
            string(REGEX MATCH "[^/]+$" dirname ${dirname})
            source_group("${dirname}\\Source Files" FILES ${source})
        endforeach()
        set(target_collection ${target_collection_header} ${target_collection_source})
    endif()
    set(_WC_CURRENT_COLLECTION_TARGET_NAME "" PARENT_SCOPE)
    if(CMAKE_DEBUG)
        message("End Collection. Objects [${target_collection}]")
    endif()
endmacro()

macro(_WC_FINISH_COLLECTION)
    if(NOT FAST_CONFIG_COLLECTION_SOURCE)
        set_target_properties(${_WC_CURRENT_COLLECTION_TARGET_NAME}
            PROPERTIES FOLDER "Libraries"
        )
    endif()
    
    if (MSVC)
        set(target_name ${_WC_CURRENT_COLLECTION_TARGET_NAME})
        _WC_RESTORE_DISPLAY_NAME()
    endif()
endmacro()

function(add_collection_executable)
    _WC_GET_BUILD_DIRECTORY_VARS()
    _WC_GET_COLLECTION_OBJECTS()
    add_executable(${_WC_CURRENT_COLLECTION_TARGET_NAME} ${ARGV}
        ${OBJECT_LIST}
    )
    _WC_FINISH_COLLECTION()
endfunction()

function(add_collection_library)
    _WC_GET_BUILD_DIRECTORY_VARS()
    _WC_GET_COLLECTION_OBJECTS()
    add_library(${_WC_CURRENT_COLLECTION_TARGET_NAME} ${ARGV}
        ${OBJECT_LIST}
    )
    _WC_FINISH_COLLECTION()
endfunction()

macro(_WC_GET_SUB_DIRECTORY_SOURCES)
    set(HEADER_EXP *.h *.hxx *.hpp *.ixx)
    set(SOURCE_EXP *.c *.cc *.cxx *.cpp *.cxx)

    file(GLOB_RECURSE HEADER_FILES ${HEADER_EXP})
    file(GLOB_RECURSE SOURCE_FILES ${SOURCE_EXP})
    if(MSVC)
        #Set headers group
        foreach(filename ${HEADER_FILES})
            get_filename_component(dirname ${filename} PATH)
            file(RELATIVE_PATH dirname ${CMAKE_CURRENT_SOURCE_DIR} ${dirname})
            string(REPLACE "/" "\\" dirname "${dirname}")
            source_group("${dirname}\\Header Files" FILES ${filename})
        endforeach()

        #Set sources group
        foreach(filename ${SOURCE_FILES})
            get_filename_component(dirname ${filename} PATH)
            file(RELATIVE_PATH dirname ${CMAKE_CURRENT_SOURCE_DIR} ${dirname})
            string(REPLACE "/" "\\" dirname "${dirname}")
            source_group("${dirname}\\Source Files" FILES ${filename})
        endforeach()
    endif()
endmacro()

function(fast_collect_library)
    _WC_GET_SUB_DIRECTORY_SOURCES()
    add_library(${ARGV}
        ${HEADER_FILES}
        ${SOURCE_FILES})
endfunction()

function(fast_collect_executable)
    _WC_GET_SUB_DIRECTORY_SOURCES()
    add_executable(${ARGV}
        ${HEADER_FILES}
        ${SOURCE_FILES})
endfunction()

function(add_fast_test)
    if(ENABLE_TEST)
        # 默认包含src目录
        include_directories(src)
        # 获取${target_name}
        _WC_GET_BUILD_DIRECTORY_VARS()
        # 获取单元测试文件
        _WC_GET_SOURCE_FILES_IN_DIR(test)
        set(target_name ${target_name}test)
        # 生成测试程序
        add_executable(${target_name}
            ${HEADER_FILES}
            ${SOURCE_FILES}
            ${DIR_HEADER_FILES}
            ${DIR_SOURCE_FILES}
        )
        # 链接gmock与其他mock库
        target_link_libraries(${target_name}
            gmock
            gmock_main
            ${ARGV}
            )
        # 添加到测试目标中，便于通过`make test`运行命令
        add_test(${target_name} ${target_name})
        # 添加测试目标的依赖(通过`make check`执行编译和测试)
        if(TARGET check)
            add_dependencies(check ${target_name})
        else()
            add_custom_target(check
                COMMAND ${CMAKE_CTEST_COMMAND}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                DEPENDS ${target_name}
                )
            if(MSVC)
                set_target_properties(check
                    PROPERTIES FOLDER CMakePredefinedTargets
                )
            endif()
        endif()
        _WC_SET_TARGET_DISPLAY_NAME(PREFIX test)
    endif()
endfunction()

# 强制包含一个头文件
function(force_include_header_file target)
    if(MSVC)
        add_definitions(-FI${target})
    else()
        add_definitions(-include ${target})
    endif()
endfunction()

# 产生工程文件名，针对工程名重名的问题，通过添加机型目录解决
macro(_WC_GENERATE_PROJECT_NAME)
    if (MSVC)
        if (NOT target_name)
            string(REGEX MATCH "[^/]+$" target_name ${CMAKE_CURRENT_SOURCE_DIR})
        endif()
        _WC_GET_PATH_PHONE_TYPE()
        # 名字和host不一样时组合
        if (PHONE_HOST_NAME AND (NOT ${PHONE_HOST_NAME} STREQUAL ${target_name}))
            set(target_name "${PHONE_HOST_NAME}_${target_name}")
        endif()
        
        if(CMAKE_DEBUG)
            message("GENERATE_PROJECT_NAME host:${PHONE_HOST_NAME} target:${target_name}")
        endif()
    else()
        string(REGEX MATCH "[^/]+$" target_name ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
endmacro()

# 从路径中获取机型
macro(_WC_GET_PATH_PHONE_TYPE)
    if (NOT PHONE_HOST_NAME)
        file(RELATIVE_PATH rel_path_name ${_WC_FOLDER_ROOT_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
        # example: t54a t3xb t21d t19e t42s t27g t27p t18 t2x t56v cp960 dect w600 
        set(PHONE_TYPE_REGEX "((t|w|cp)[0-9]+x?[abdegpsv]?|(dect)|(xwin))")
        string(REGEX MATCH "/${PHONE_TYPE_REGEX}/" PHONE_HOST_NAME ${rel_path_name})
        if (NOT PHONE_HOST_NAME)
            # 最后一级目录
            string(REGEX MATCH "/${PHONE_TYPE_REGEX}$" PHONE_HOST_NAME ${rel_path_name})
        endif()
        if (PHONE_HOST_NAME)
            string(REGEX MATCH "[^/]+" PHONE_HOST_NAME ${PHONE_HOST_NAME})
        endif()
    endif()
endmacro()

# 还原IDE中工程名的显示名
macro(_WC_RESTORE_DISPLAY_NAME)
    if (MSVC)
        get_property(target_display_name GLOBAL PROPERTY ${target_name}_REL_NAME)
        if (target_display_name)
            set_target_properties(${target_name} PROPERTIES PROJECT_LABEL ${target_display_name})
        else()
            string(REGEX MATCH "[^/]+$" target_display_name ${CMAKE_CURRENT_SOURCE_DIR})
            set_target_properties(${target_name} PROPERTIES PROJECT_LABEL ${target_display_name})
        endif()
        
        if(CMAKE_DEBUG)
            message("RESTORE_DISPLAY_NAME ${target_name} -> ${target_display_name}")
        endif()
    endif()
endmacro()

# 保存IDE中工程名的显示名
macro(_WC_SET_TARGET_DISPLAY_NAME)
    if (MSVC)
        cmake_parse_arguments(OBJECT "" "PREFIX;NAME;SUFFIX" "" ${ARGN})   
        if (OBJECT_NAME)
            set_property(GLOBAL PROPERTY ${target_name}_REL_NAME ${OBJECT_NAME})    
        endif()
        
        if (OBJECT_SUFFIX)
            string(REGEX MATCH "[^/]+$" target_display_name ${CMAKE_CURRENT_SOURCE_DIR})
            set_property(GLOBAL PROPERTY ${target_name}_REL_NAME "${target_display_name}_${OBJECT_SUFFIX}")
        endif()
        
        if (OBJECT_PREFIX)
            string(REGEX MATCH "[^/]+$" target_display_name ${CMAKE_CURRENT_SOURCE_DIR})
            set_property(GLOBAL PROPERTY ${target_name}_REL_NAME "${OBJECT_PREFIX}_${target_display_name}")
        endif()
        
        if (OBJECT_SUFFIX OR OBJECT_PREFIX)
            _WC_RESTORE_DISPLAY_NAME()
            get_target_property(FOLDER_NAME ${target_name} FOLDER)
            if (NOT FOLDER_NAME)
                set_target_properties(${target_name}
                    PROPERTIES FOLDER "${OBJECT_PREFIX}${OBJECT_SUFFIX} Projects")
            endif()
        endif()
    endif()
endmacro()

# MSVC 预编译文件过滤
macro(_WC_FILTER_MSVC_PCH)
    if (MSVC AND ENABLE_PCH)
        cmake_parse_arguments(PCH "" "NAME_WE" "" ${ARGN})
        if(NOT PCH_NAME_WE)
            #默认的预编译头文件为precompile，默认需要超过5个才会开预编译
            set(PCH_NAME_WE precompile)
            set(${target_name}_use_pch OFF)
        else()
            #如果有指定预编译文件名，则强制开启预编译选项
            set(${target_name}_use_pch ON)
        endif()
        
        set(PCH_SOURCE_FILE ${PCH_PATH}/${PCH_NAME_WE}.cpp)
        set(PCH_HEADER_FILE ${PCH_PATH}/${PCH_NAME_WE}.h)
        list(LENGTH SOURCE_FILES SRC_FILE_COUNT)
        # 文件个数大于5个启用预编译提速
        if(${SRC_FILE_COUNT} GREATER 4)
            # c预编译头文件和c++预编译头文件不能混用
            set(${target_name}_use_pch ON)
            foreach(filename ${SOURCE_FILES})
                string(REGEX MATCH "[^.]+$" filename ${filename})
                string(TOLOWER ${filename} filename)
                string(REGEX MATCH "^c+$" filename ${filename})
                if(NOT (filename STREQUAL ""))
                    set(${target_name}_use_pch OFF)
                    break()
                endif()
            endforeach()
        endif()
        
        if (${target_name}_use_pch)
            list(APPEND SOURCE_FILES ${PCH_SOURCE_FILE})
            list(APPEND HEADER_FILES ${PCH_HEADER_FILE})
        endif()
         
        if(CMAKE_DEBUG)
            message("FILTER_MSVC_PCH ${PCH_FILE_NAME_WE} for target ${target_name}")
        endif()
    endif()
endmacro()

# MSVC 预编译文件处理
macro(_WC_ADD_MSVC_PCH)
    if(MSVC)
        if (ENABLE_PCH)
            if (${target_name}_use_pch)
                get_filename_component(PCH_NAME ${PCH_HEADER_FILE} NAME_WE)

                # Compute a custom name for the precompiled header.
                if(CMAKE_CONFIGURATION_TYPES)
                    set(PCH_DIR "${CMAKE_CURRENT_BINARY_DIR}/PCH/${CMAKE_CFG_INTDIR}")
                else(CMAKE_CONFIGURATION_TYPES)
                    set(PCH_DIR "${CMAKE_CURRENT_BINARY_DIR}/PCH")
                endif(CMAKE_CONFIGURATION_TYPES)

                file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/PCH)

                set(PCH_FILE /Fp${PCH_DIR}/${PCH_NAME}.pch)

                set_target_properties(${target_name} PROPERTIES COMPILE_FLAGS
                    "/Yu${PCH_HEADER_FILE} /FI${PCH_HEADER_FILE} ${PCH_FILE}")
                set_source_files_properties(${PCH_SOURCE_FILE} PROPERTIES COMPILE_FLAGS
                    "/Yc${PCH_HEADER_FILE}")

                set_directory_properties(PROPERTIES
                    ADDITIONAL_MAKE_CLEAN_FILES ${PCH_DIR}/${PCH_NAME}.pch)

                source_group("Pre-compiled" FILES ${PCH_HEADER_FILE} ${PCH_SOURCE_FILE})
            endif()
        else()
            #未启用预编译头文件时，直接包含文件
            #force_include_header_file(${PCH_HEADER_FILE})
        endif()
    endif(MSVC)
endmacro()

# 通过伪造一个目标，仅为了IDE环境显示需要，不参与实际编译
function(add_fake_target)
    if (MSVC AND ENABLE_IDE_MODE)
        # 参数解析
        list(LENGTH ARGV argv_len)
        if (${argv_len} GREATER 0)
            _WC_GENERATE_PROJECT_NAME()
            set(i 0)
            while( i LESS ${argv_len})  
                list(GET ARGV ${i} argv_value)
                _WC_GET_SOURCE_FILES_IN_DIR(${argv_value}) 
                list(APPEND HEADER_FILES ${DIR_HEADER_FILES})
                list(APPEND SOURCE_FILES ${DIR_SOURCE_FILES})
                # 第一项作为显示名
                if (i EQUAL 0)
                    set(NAME_PATH ${argv_value})
                endif()
                math(EXPR i "${i} + 1")
            endwhile() 
        else()
            _WC_GET_BUILD_DIRECTORY_VARS()
        endif()
        
        if ((NOT HEADER_FILES) AND (NOT SOURCE_FILES))
            return()
        endif()
        
        # target包含源代码不参与实际编译
        foreach(filename ${HEADER_FILES} ${SOURCE_FILES})
            set_source_files_properties(${filename} PROPERTIES 
                HEADER_FILE_ONLY TRUE)
        endforeach()
        set(target_display_name ${target_name})
        set(target_name ${target_name}_fake)
        # 伪造一个target
        add_custom_target(${target_name}
                COMMAND echo Build fake target ${target_display_name}
                COMMENT "Build a fake target for IDE will do nothing but waste of time"
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                SOURCES ${HEADER_FILES} ${SOURCE_FILES})  
        # 还原IDE环境中的显示名
        if (NAME_PATH)
            set_target_properties(${target_name} PROPERTIES 
                PROJECT_LABEL "${NAME_PATH}_${target_display_name}"
                FOLDER "${NAME_PATH} Projects")
        else()   
		    set_target_properties(${target_name} PROPERTIES 
                PROJECT_LABEL ${target_display_name})
            set_target_folder()
        endif()
        
        if(CMAKE_DEBUG)
            message("add fake target ${target_display_name}")
        endif()
    endif()
endfunction()

# 递归添加fake子目录
function(add_fake_subdirectory sub_path)
    if ((NOT MSVC) OR (NOT ENABLE_IDE_MODE))
        return()
    endif()
    
    # 扫描下级子目录是否可添加
    file(GLOB ALL_FILES ${sub_path}/*)
    foreach(subdir ${ALL_FILES})
        string(REGEX MATCH "[^/]+$" dirname ${subdir})
        if((IS_DIRECTORY ${subdir}) 
            AND (EXISTS ${subdir}/CMakeLists.txt)
            AND (NOT ${dirname} STREQUAL "src"))
            add_fake_subdirectory(${subdir})
        endif()
    endforeach()
    
    # 过滤条件
    set(HEADER_EXP *.h *.hxx *.hpp *.ixx)
    set(SOURCE_EXP *.c *.cc *.cxx *.cpp *.cxx *.rc *.py)
    
    # 过滤文件
    foreach(SINGLE_SUB_PATH . include src src/menu test)
        foreach(FILE_TYPE ${HEADER_EXP})
            # include使用递归
            if (${SINGLE_SUB_PATH} STREQUAL "include")
                include_directories(${sub_path}/include)
                file(GLOB_RECURSE DIR_HEADER_FILES ${sub_path}/${SINGLE_SUB_PATH}/${FILE_TYPE})
            else()
                file(GLOB DIR_HEADER_FILES ${sub_path}/${SINGLE_SUB_PATH}/${FILE_TYPE})
            endif()
            if(DIR_HEADER_FILES)
                list(APPEND HEADER_FILES ${DIR_HEADER_FILES})
                # include显示优化，只有include的情况下不显示include文件夹
                if(${SINGLE_SUB_PATH} STREQUAL "include")
                    source_group("${SINGLE_SUB_PATH}" FILES ${DIR_HEADER_FILES})
                elseif(NOT ${SINGLE_SUB_PATH} STREQUAL ".")
                    source_group("${SINGLE_SUB_PATH}\\Header Files" FILES ${DIR_HEADER_FILES})
                endif()
            endif()
        endforeach()
        foreach(FILE_TYPE ${SOURCE_EXP})
            file(GLOB DIR_SOURCE_FILES ${sub_path}/${SINGLE_SUB_PATH}/${FILE_TYPE})
            if(DIR_SOURCE_FILES)
                list(APPEND SOURCE_FILES ${DIR_SOURCE_FILES})
                if(NOT ${SINGLE_SUB_PATH} STREQUAL ".")
                    source_group("${SINGLE_SUB_PATH}\\Source Files" FILES ${DIR_SOURCE_FILES})
                endif()
            endif()
        endforeach()
    endforeach()
    
    # 没有源文件直接返回
    if (NOT SOURCE_FILES)
        return()
    endif()

    # 获取工程名称
    string(REGEX MATCH "[^/]+$" target_display_name ${sub_path})
    # 尝试获取机型名称
    set(target_name ${target_display_name})
    file(RELATIVE_PATH rel_path_name ${_WC_FOLDER_ROOT_DIR} ${sub_path})
    set(PHONE_TYPE_REGEX "((t|w|cp)[0-9]+x?[abdegpsv]?|(dect)|(xwin))")
    string(REGEX MATCH "/${PHONE_TYPE_REGEX}/" PHONE_HOST_NAME ${rel_path_name})
    if (NOT PHONE_HOST_NAME)
        # 最后一级目录
        string(REGEX MATCH "/${PHONE_TYPE_REGEX}$" PHONE_HOST_NAME ${rel_path_name})
    endif()
    if (PHONE_HOST_NAME)
        string(REGEX MATCH "[^/]+" PHONE_HOST_NAME ${PHONE_HOST_NAME})
    endif()
    
    # 名字和host不一样时组合
    if (PHONE_HOST_NAME AND (NOT ${PHONE_HOST_NAME} STREQUAL ${target_display_name}))
        set(target_name "${PHONE_HOST_NAME}_${target_display_name}")
    endif()
    
    # 子目录下包含的源代码不参与实际编译
    foreach(filename ${HEADER_FILES} ${SOURCE_FILES})
        set_source_files_properties(${filename} PROPERTIES 
            HEADER_FILE_ONLY TRUE)
    endforeach()
    
    set(target_name ${target_name}_fake)
    # 伪造一个target
    add_custom_target(${target_name}
            COMMAND echo Build fake target ${target_display_name}
            COMMENT "Build a fake target for IDE will do nothing but waste of time"
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            SOURCES ${HEADER_FILES} ${SOURCE_FILES})
 
    # 还原IDE环境中的显示名
    if (target_prefix)
        set_target_properties(${target_name} PROPERTIES 
            PROJECT_LABEL "${target_prefix}_${target_display_name}"
            FOLDER "${target_prefix} Projects")
    else()
        set_target_properties(${target_name} PROPERTIES 
            PROJECT_LABEL ${target_display_name})
            
        file(RELATIVE_PATH rel_path ${_WC_FOLDER_ROOT_DIR} ${sub_path})    
        string(REPLACE "/" " Projects/" rel_path ${rel_path})
        get_filename_component(rel_path ${rel_path} DIRECTORY)
        if(rel_path)
            # 设置VS工程相对目录
            set_target_properties(${target_name} PROPERTIES 
                FOLDER ${rel_path})
        endif()
    endif()
    
    if(CMAKE_DEBUG)
        message("add fake sub directory ${target_display_name}")
    endif()
endfunction()

# 递归添加fake子目录，允许排除或者单独指定某个目录
function(add_fake_directory)
    if (MSVC AND ENABLE_IDE_MODE)
        cmake_parse_arguments(PATH "" "EXCLUDE;TARGET;PREFIX" "" ${ARGN})
        file(GLOB ALL_FILES *)
        
        # 名称前缀
        if(PATH_PREFIX)
            set(target_prefix ${PATH_PREFIX})
        endif()      
        # 排除单个目录
        if(PATH_EXCLUDE)
            foreach(SUB_PATH ${ALL_FILES})
                if((IS_DIRECTORY ${SUB_PATH}) 
                    AND (EXISTS ${SUB_PATH}/CMakeLists.txt))
                    file(RELATIVE_PATH SUB_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${SUB_PATH})
                    list(FIND PATH_EXCLUDE ${SUB_PATH} _index)
                    if (_index EQUAL -1)
                        add_fake_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${SUB_PATH})
                    endif()
                endif()
            endforeach()
        # 仅包含单个目录
        elseif(PATH_TARGET)
            set(SUB_PATH ${PATH_TARGET})
            add_fake_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/${SUB_PATH})
        # 包含所有子目录
        else()
            foreach(SUB_PATH ${ALL_FILES})
                if((IS_DIRECTORY ${SUB_PATH}) 
                    AND (EXISTS ${SUB_PATH}/CMakeLists.txt))
                    add_fake_subdirectory(${SUB_PATH})
                endif()
            endforeach()
        endif()
        
        if(CMAKE_DEBUG)
            message("add fake path EXCLUDE:${PATH_EXCLUDE} TARGET:${PATH_TARGET}")
        endif()
    endif()
endfunction()
