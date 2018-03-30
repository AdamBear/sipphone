# post-build操作

# deep strip
macro(DEEP_STRIP target)
    if((NOT DEBUG_MODE) AND ENABLE_STRIP AND (NOT MSVC))
        get_target_property(TARGET_TYPE ${target} TYPE)
        #使用Backtrace时，不对可执行程序strip
        if(NOT (ENABLE_BACKTRACE AND ("${TARGET_TYPE}" STREQUAL "EXECUTABLE")))
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${BUILDTOOL_PREFIX}-strip -x --strip-unneeded -R .pdr -R .comment -R .note -R .mdebug.abi32 $<TARGET_FILE:${target}>
                COMMENT "Deep strip ${target}."
            )
            message("deep strip for target[${target}]")
        endif()
    endif()
endmacro()

# UPX压缩
function(UPX_COMPRESS target)
    if(NOT DEBUG_MODE AND ENABLE_UPX AND LATTICE)
        get_target_property(TARGET_FILE_NAME ${target} OUTPUT_NAME)
        set(UPX_FILE_PATH ${CMAKE_INSTALL_PREFIX}/bin/${TARGET_FILE_NAME})
        message("upx for target[${TARGET_FILE_NAME}] in path[${UPX_FILE_PATH}]")
        install(CODE "execute_process(COMMAND echo upx processing ${TARGET_FILE_NAME} ...)")
        install(CODE "execute_process(COMMAND upx --brute ${UPX_FILE_PATH})")
    endif()
endfunction()

# objcopy优化
macro(STRIP_COMMENT target)
    if(NOT DEBUG_MODE AND ENABLE_STRIPCOMMENT AND (NOT MSVC))
        add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -R .comment $<TARGET_FILE:${target}>
                COMMENT "strip comment ${target}.")
        message("objcopy comment for target[${target}]")
    endif()
endmacro()
