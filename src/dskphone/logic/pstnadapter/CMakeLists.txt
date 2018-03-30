if(CP OR CP920)

include_directories(
    include
    )

if(ENABLE_INSTRUMENT AND ENABLE_PSTNADAPTER_MOD)
	message("Enable ENABLE_PSTNADAPTER_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()

endif()
