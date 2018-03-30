include_directories(
		../
		../../
		../../common
)

if(ENABLE_INSTRUMENT AND ENABLE_GLOBALINPUTHOOK_MOD)
	message("Enable ENABLE_GLOBALINPUTHOOK_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()

