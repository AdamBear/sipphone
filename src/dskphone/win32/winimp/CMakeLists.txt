add_fast_library(SHARED)
_WC_GET_BUILD_DIRECTORY_VARS()

target_link_libraries(${target_name}
	libcfgservice libsysservice)

target_install(${target_name})

file(GLOB HEAD_FILES winimp.h getopt.h unicode.h sha256.h compress.h linux.h template.hpp)
header_install("${HEAD_FILES}" ${target_name})
