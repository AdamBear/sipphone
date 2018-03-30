collect()

add_fast_test(
    ETLLibmock
    configparsermock
    etlmsghandlermock
    sysservicemock
)

if (NOT ENABLE_TEST)
    add_fake_target(test include)
endif()