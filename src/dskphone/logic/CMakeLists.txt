include_directories(
	setting/include
	)

collection_build_begin()

add_all_subdirectories()

if((COLOR OR T42S) AND (NOT MSVC))
	#彩屏机编译为动态库
	set(LOGIC_LIB_TYPE SHARED)
else()
	#黑白屏编译为静态库
	set(LOGIC_LIB_TYPE STATIC)
endif()

add_collection_library(${LOGIC_LIB_TYPE})

_WC_GET_BUILD_DIRECTORY_VARS()
#解决main连接需要多次link的问题
if("${LOGIC_LIB_TYPE}" STREQUAL STATIC)
	if (NOT MSVC)
		target_link_libraries(${target_name}
			xwin
		)
	endif()
else() #安装动态库
	target_install(${target_name})
endif()
