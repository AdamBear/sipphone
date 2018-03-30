include_directories(
    include
    src
    )
    

if(ENABLE_INSTRUMENT AND ENABLE_DSSKEYUI_MOD)
	message("Enable ENABLE_DSSKEYUI_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()

