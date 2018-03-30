collect()

include_directories(
    include 
    src
)

if(ENABLE_INSTRUMENT AND ENABLE_DSSKEY_MOD)
	message("Enable ENABLE_DSSKEY_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()
