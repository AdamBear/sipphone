collect()

add_fast_test(
    configparsermock
    etlmsghandlermock
)
    
if (NOT ENABLE_TEST)
    add_fake_target(test include)
endif()
