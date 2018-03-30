add_fast_executable()
_WC_GET_BUILD_DIRECTORY_VARS()

set_target_properties(${target_name}
    PROPERTIES OUTPUT_NAME "sync")

target_link_libraries(${target_name} 
winimp 
xmlparser 
ETL 
Devicelib
libiconv
ConfigParser
libmsgkit
libcfgservice
)
DEEP_STRIP(${target_name})
target_install(${target_name})
UPX_COMPRESS(${target_name})