aux_source_directory(src SRC_FILES)

add_library(example OBJECT
    ${SRC_FILES}
    )
    
include_directories(
    include 
    src
)

if(ENABLE_INSTRUMENT AND ENABLE_EXAMPLE_MOD)
	message("Enable ENABLE_EXAMPLE_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()



if(ENABLE_TEST AND TEST_EXAMPLE)
	      add_executable(gtest_dsskey
		    test/main.cpp
        test/dsskey_test.cpp
        $<TARGET_OBJECTS:example>
        )
	    
	target_link_libraries(gtest_dsskey
        logic
        pthread
        m
        ssl
        crypt
        crypto
        download
        ldap
        mkservice
        service
        TaskAction
        ETL
        ETLMsgHandler
        CommonAPI
        DeviceLib
        xmlparser
        trace
        ConfigParser
        YLSTL
        resource
        regexp
        gtest
		)
	    
	install(TARGETS gtest_example RUNTIME DESTINATION bin)
endif()
