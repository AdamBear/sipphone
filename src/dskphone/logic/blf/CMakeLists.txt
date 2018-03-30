include_directories(
    include
    )
    
if(ENABLE_INSTRUMENT AND ENABLE_BLF_MOD)
	message("Enable ENABLE_BLF_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()
