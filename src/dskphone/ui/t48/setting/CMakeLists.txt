include_directories(
    ../
	../../../logic
	../uicommon
	../include
    include
    src
)

if (MSVC)
	_WC_GET_SOURCE_FILES_IN_DIR(src/menu)
	add_fast_library(STATIC ${DIR_HEADER_FILES} ${DIR_SOURCE_FILES})
else()
	_WC_GET_SOURCE_FILES_IN_DIR(src/menu)
	add_fast_library(SHARED ${DIR_HEADER_FILES} ${DIR_SOURCE_FILES})

	#安装动态库
	_WC_GET_BUILD_DIRECTORY_VARS()

	install(TARGETS ${target_name} LIBRARY DESTINATION lib
                       RUNTIME DESTINATION bin)

endif()