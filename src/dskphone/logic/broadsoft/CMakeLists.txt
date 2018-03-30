include_directories(
    ./xsiactionhelper/include
    )
    
if(ENABLE_INSTRUMENT AND ENABLE_BROADSOFT_MOD)
	message("Enable ENABLE_BROADSOFT_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

add_all_subdirectories()
