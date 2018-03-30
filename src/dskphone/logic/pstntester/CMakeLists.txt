if(CP AND TEST_PSTN)

add_definitions(-D_TEST_PSTN)

include_directories(.)

collect()

endif()
