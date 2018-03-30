include_directories(
    include
    )
    
if(ENABLE_INSTRUMENT AND ENABLE_ACCOUNT_MOD)
	message("Enable ENABLE_ACCOUNT_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()

if(ENABLE_TEST AND TEST_ACCOUNT)
	add_executable(gtest_account
        test/empty_functions.cpp
		test/gtest_account.cpp
        $<TARGET_OBJECTS:account>
        )
	    
	target_link_libraries(gtest_account
		ETL
		pthread
		m
		xmlparser
		mkservice
        sysservice
		DeviceLib
		ETLMsgHandler
		ConfigParser
		CommonAPI
        YLSTL
	    
		gtest
		)
	    
	install(TARGETS gtest_account RUNTIME DESTINATION bin)
endif()