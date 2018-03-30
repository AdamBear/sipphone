if(XWIN)

collection_build_begin(TARGET_NAME xcommon)

add_subdirectory(xwindow-kernel)
add_subdirectory(xwindow)
add_subdirectory(fbscreen)
add_subdirectory(imeedit)
add_subdirectory(animation)

if(COLOR AND (NOT MSVC))
	#彩屏机编译为动态库
	set(LOGIC_LIB_TYPE SHARED)
else()
	#黑白屏编译为静态库
	set(LOGIC_LIB_TYPE STATIC)
endif()

add_collection_library(${LOGIC_LIB_TYPE})

_WC_GET_BUILD_DIRECTORY_VARS()

#解决main连接需要多次link的问题
if("${LOGIC_LIB_TYPE}" STREQUAL SHARED)
    install(TARGETS xcommon LIBRARY DESTINATION lib
                           RUNTIME DESTINATION bin)
endif()
endif()
