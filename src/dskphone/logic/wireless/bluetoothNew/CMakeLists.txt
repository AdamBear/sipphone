if(ENABLE_NEW_BLUETOOTH)

message("*******Use new version bluetooth********")

#添加包含路径
include_directories(
	include 
	src
	../../../include/extended/
	)

if (ENABLE_BT_TEST_LOGIC OR ENABLE_BT_SIMULATION_DRIVER)
	message("----*******bt test code on********--------")
	
	if(ENABLE_BT_SIMULATION_DRIVER)
		message("******bt simulation driver mode on********")
	endif()	
	
    _WC_GET_SOURCE_FILES_IN_DIR(testdriver)
endif()

collect(${DIR_HEADER_FILES} ${DIR_SOURCE_FILES})

add_mock_library(STATIC)

endif()
