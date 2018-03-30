collect()
add_mock_library(STATIC)

include_directories(
    include
    src
)

if(ENABLE_INSTRUMENT AND ENABLE_DSSKEY_MOD)
	message("Enable ENABLE_DSSKEY_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

if(ENABLE_TEST AND TEST_DSSKEY)
	add_executable(gtest_dsskey
		test/main.cpp
        test/dsskey_test.cpp
        $<TARGET_OBJECTS:dsskey>
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

	install(TARGETS gtest_dsskey RUNTIME DESTINATION bin)
endif()
