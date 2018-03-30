if(ENABLE_INSTRUMENT AND ENABLE_MEDIA_MOD)
	message("Enable ENABLE_MEDIA_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()
