include_directories(
    include
    )
    
if(ENABLE_INSTRUMENT AND ENABLE_UIMANAGER_MOD)
	message("Enable ENABLE_UIMANAGER_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()

