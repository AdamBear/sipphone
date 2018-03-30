if(ENABLE_INSTRUMENT AND ENABLE_ADAPT_MOD)
	message("Enable ENABLE_ADAPT_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()
    
include_directories(
    include 
    src
)

add_mock_library(STATIC)
