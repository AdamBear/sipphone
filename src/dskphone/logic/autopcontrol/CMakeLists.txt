if(ENABLE_INSTRUMENT AND ENABLE_AUTOPCONTROL_MOD)
	message("Enable ENABLE_AUTOPCONTROL_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()

include_directories(
    include 
    src
)