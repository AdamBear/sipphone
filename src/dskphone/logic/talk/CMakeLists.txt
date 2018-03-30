if(ENABLE_INSTRUMENT AND ENABLE_TALK_MOD)
	message("Enable ENABLE_TALK_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

include_directories(
    .
    talk
    )

add_all_subdirectories()
