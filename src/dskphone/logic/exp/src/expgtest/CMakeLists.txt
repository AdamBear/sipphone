if(ENABLE_TEST AND TEST_EXP)
message("gtest exp has been compiled")

include_directories(../expdrawer)
include_directories(../expscanner)
include_directories(../expdriveheader)

aux_source_directory(. SRC_FILES)
aux_source_directory(../expscanner SRC_FILES)


add_definitions(-DTEST_EXP)
add_executable(gtest_exp ${SRC_FILES})
target_link_libraries(  gtest_exp
                        gmock_main
                        xmlparser
        )
install(TARGETS gtest_exp RUNTIME DESTINATION bin)
else()

endif()

