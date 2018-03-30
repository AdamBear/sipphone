include_directories(
    include 
    src
  ../../../logic
	../../../uilogic
	../../../uilogic/idleuilogic/include
	../../../../include
)

if(ENABLE_INSTRUMENT AND ENABLE_IDLESCREENUI_MOD)
	message("Enable ENABLE_IDLESCREENUI_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})


if(T54)
configure_file(idlescreenuidefine.t54.h
               idlescreenuidefine.h
               COPYONLY)
elseif(T42S)
configure_file(idlescreenuidefine.t42.h
               idlescreenuidefine.h
               COPYONLY)
elseif(T27 OR T27G)
configure_file(idlescreenuidefine.t27.h
               idlescreenuidefine.h
               COPYONLY)
elseif(COLOR)
configure_file(idlescreenuidefine.color.h
               idlescreenuidefine.h
               COPYONLY)
else()
configure_file(idlescreenuidefine.black.h
               idlescreenuidefine.h
               COPYONLY)
endif()

collect()

