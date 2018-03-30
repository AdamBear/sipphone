include_directories(
	include
	src
    )
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
if(CP920)
configure_file(include/diruilogicdefine.cp.h
               include/diruilogicdefine.h
               COPYONLY)
else()
configure_file(include/diruilogicdefine.normal.h
               include/diruilogicdefine.h
               COPYONLY)
endif()
collect()

