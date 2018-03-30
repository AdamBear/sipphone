if(ENABLE_INSTRUMENT AND ENABLE_COMMONUNITS_MOD)
	message("Enable ENABLE_COMMONUNITS_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()

add_mock_library(STATIC)
