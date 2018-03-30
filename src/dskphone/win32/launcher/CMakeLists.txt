add_fast_executable()
_WC_GET_BUILD_DIRECTORY_VARS()

set_target_properties(${target_name}
    PROPERTIES OUTPUT_NAME "launcher")

target_link_libraries(${target_name}
    winimp ETL xmlparser DeviceLib)

DEEP_STRIP(${target_name})
target_install(${target_name})
UPX_COMPRESS(${target_name})