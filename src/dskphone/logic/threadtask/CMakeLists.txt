include_directories(
    include
    )


if(ENABLE_INSTRUMENT AND ENABLE_THTASKASS_MOD)
	message("Enable ENABLE_THTASKASS_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()