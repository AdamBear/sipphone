include_directories(
    src     
	include 
	../../../include/extended
	)

set(TEST_FILES)

# WiFi测试代码
if(wifitest)
    _WC_GET_SOURCE_FILES_IN_DIR(test)
	list(APPEND TEST_FILES ${DIR_HEADER_FILES} ${DIR_SOURCE_FILES})

	add_definitions(-DIF_WIFI_TEST_DRIVER)
	message("*******wifi test code mode on********")
endif()

if(wifitestdriver)
    _WC_GET_SOURCE_FILES_IN_DIR(testdriver)
	list(APPEND TEST_FILES ${DIR_HEADER_FILES} ${DIR_SOURCE_FILES})

	add_definitions(-DIF_WIFI_SIMULATION_DRIVER)
	message("******wifi simulation driver mode on********")
endif()

collect(${TEST_FILES})
