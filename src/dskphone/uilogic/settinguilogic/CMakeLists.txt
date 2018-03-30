include_directories(
	include
	src
    )
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
if(T23 OR T2XD)
configure_file(include/settingdefine.smallscreen.h
               include/settingdefine.h
               COPYONLY)
elseif(T42S)
configure_file(include/settingdefine.t42.h
               include/settingdefine.h
               COPYONLY)
elseif(COLOR)
configure_file(include/settingdefine.color.h
               include/settingdefine.h
               COPYONLY)
else()
configure_file(include/settingdefine.black.h
               include/settingdefine.h
               COPYONLY)
endif()
collect()

