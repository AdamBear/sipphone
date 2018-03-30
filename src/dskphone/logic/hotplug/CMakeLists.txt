if(_USB)

if(ENABLE_INSTRUMENT AND ENABLE_HOTPLUG_MOD)
	message("Enable ENABLE_HOTPLUG_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()

add_mock_library(STATIC)
add_fast_test(
    ETLLibmock
    configparsermock
    etlmsghandlermock
)

endif()