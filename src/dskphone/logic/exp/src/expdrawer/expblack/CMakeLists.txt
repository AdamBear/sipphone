
include_directories(../expbase)
include_directories(../../expdriveheader)

if(BUILD_EXPCOLOR)
	add_fake_target()
else()
	collect()
endif()

