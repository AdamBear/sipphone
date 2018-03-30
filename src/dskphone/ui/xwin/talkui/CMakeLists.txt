include_directories(
    include
    src
    )
    
if(ENABLE_INSTRUMENT AND ENABLE_TALKUI_MOD)
	message("Enable ENABLE_TALKUI_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()

