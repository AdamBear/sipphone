include_directories(
    include 
    src
	../../../uilogic/diruilogic/include
)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

if(ENABLE_INSTRUMENT AND ENABLE_DIRECTORYUI_MOD)
	message("Enable ENABLE_DIRECTORYUI_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

if(COLOR)
configure_file(include/directorydefine.color.h
               include/directorydefine.h
               COPYONLY)
elseif(T23 OR T2XD)
configure_file(include/directorydefine.smallscreen.h
               include/directorydefine.h
               COPYONLY)
else()
configure_file(include/directorydefine.black.h
               include/directorydefine.h
               COPYONLY)
endif()

collect()

