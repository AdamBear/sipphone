include_directories(
	include
	src
	../
	../../../logic
	../../../../include
    )
    
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
    
if(ENABLE_INSTRUMENT AND ENABLE_ACCOUNT_MOD)
	message("Enable ENABLE_ACCOUNT_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

if(T54)
configure_file(include/baseuidefine.t54.h
               include/baseuidefine.h
               COPYONLY)
elseif(COLOR)
configure_file(include/baseuidefine.t46.h
               include/baseuidefine.h
               COPYONLY)
elseif(T23 OR T2XD)
configure_file(include/baseuidefine.smallscreen.h
               include/baseuidefine.h
               COPYONLY)
elseif(T42S)
configure_file(include/baseuidefine.t42.h
               include/baseuidefine.h
               COPYONLY)
elseif(CP920)
configure_file(include/baseuidefine.cp920.h
               include/baseuidefine.h
               COPYONLY)
else()
configure_file(include/baseuidefine.black.h
               include/baseuidefine.h
               COPYONLY)
endif()

collect()

