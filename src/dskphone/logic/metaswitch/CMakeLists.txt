include_directories(
    ./mtswcontrol/include
    )

if(ENABLE_INSTRUMENT AND ENABLE_METASWITCH_MOD)
	message("Enable ENABLE_METASWITCH_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

add_all_subdirectories()
