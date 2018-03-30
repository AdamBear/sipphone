include_directories(
		../
		../../
		../../common
)
if(ENABLE_INSTRUMENT AND ENABLE_MESSAGEBOX_MOD)
	message("Enable ENABLE_MESSAGEBOX_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()

