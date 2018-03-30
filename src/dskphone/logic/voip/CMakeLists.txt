
if(ENABLE_INSTRUMENT AND ENABLE_VOIP_MOD)
	message("Enable ENABLE_VOIP_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()
