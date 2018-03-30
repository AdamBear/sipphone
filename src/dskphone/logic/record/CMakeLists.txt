if(_USB)

if(ENABLE_INSTRUMENT AND ENABLE_RECORD_MOD)
	message("Enable ENABLE_RECORD_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()

include_directories(
    include 
    src
)

add_mock_library(STATIC)
add_fast_test(
    ETLLibmock
    configparsermock
    etlmsghandlermock
)

endif()
