if(COLOR OR T42S)

include_directories(
    include
    )
	
if(MSVC)
	collect()
else()

	if(ENABLE_INSTRUMENT AND ENABLE_PRESENCE_MOD)
		message("Enable ENABLE_LAMP_MOD instrument")
		add_definitions("${ENABLE_INSTRUMENT_CFG}")
	endif()

	add_fast_library(SHARED)

	_WC_GET_BUILD_DIRECTORY_VARS()
	
	install(TARGETS ${target_name} LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
endif()


else()

add_fake_target()

endif()