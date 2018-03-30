include_directories(
    .
    lamp
    )

if(ENABLE_INSTRUMENT AND ENABLE_PRESENCE_MOD)
	message("Enable ENABLE_PRESENCE_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

add_all_subdirectories()
