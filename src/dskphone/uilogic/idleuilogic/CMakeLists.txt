
include_directories(
	include
	src
    )
    
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

if(T54)
configure_file(include/idledefine.t54.h
               include/idledefine.h
               COPYONLY)
elseif(T42S)
configure_file(include/idledefine.t42.h
               include/idledefine.h
               COPYONLY)
elseif(T27 OR T27G)
configure_file(include/idledefine.t27.h
               include/idledefine.h
               COPYONLY)
elseif(T23 OR T2XD)
configure_file(include/idledefine.smallscreen.h
               include/idledefine.h
               COPYONLY)
elseif(COLOR)
configure_file(include/idledefine.color.h
               include/idledefine.h
               COPYONLY)
else()
configure_file(include/idledefine.black.h
               include/idledefine.h
               COPYONLY)
endif()

collect()
