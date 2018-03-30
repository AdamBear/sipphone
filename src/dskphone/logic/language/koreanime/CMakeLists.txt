if(ENABLE_INSTRUMENT AND ENABLE_KOREANIME_MOD)
	message("Enable ENABLE_KOREANIME_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()
