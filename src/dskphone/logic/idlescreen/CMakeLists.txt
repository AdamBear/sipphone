if(ENABLE_INSTRUMENT AND ENABLE_IDLESCREEN_MOD)
	message("Enable ENABLE_IDLESCREEN_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()

add_mock_library(STATIC)

include_directories(
    include
    src
)


if(TEST_IDLESCREEN)
    add_executable(gtest_idlescreen
        test/gtest_idlescreen.cpp
        test/empty_functions.cpp
        $<TARGET_OBJECTS:account>
        $<TARGET_OBJECTS:idleScreen>)
        
    target_link_libraries(gtest_idlescreen
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
        
    install(TARGETS gtest_idlescreen RUNTIME DESTINATION bin)
endif()