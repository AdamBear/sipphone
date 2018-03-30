
if(ENABLE_INSTRUMENT AND ENABLE_EXTENDED_MOD)
	message("Enable ENABLE_EXTENDED_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

add_all_subdirectories()
