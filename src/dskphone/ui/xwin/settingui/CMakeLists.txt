include_directories(
    include 
    src
  ../../../logic
	../../../uilogic
	../../../uilogic/settinguilogic/include
	../../../../include
)

if(ENABLE_INSTRUMENT AND ENABLE_SETTINGUI_MOD)
	message("Enable ENABLE_SETTINGUI_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

if(CP920)
configure_file(include/settinguidefine.cp.h
               include/settinguidefine.h
               COPYONLY)
elseif(T42S)
configure_file(include/settinguidefine.t42.h
               include/settinguidefine.h
               COPYONLY)
elseif(T54)
configure_file(include/settinguidefine.t54.h
               include/settinguidefine.h
               COPYONLY)
elseif(COLOR)
configure_file(include/settinguidefine.color.h
               include/settinguidefine.h
               COPYONLY)
else()
configure_file(include/settinguidefine.black.h
               include/settinguidefine.h
               COPYONLY)
endif()

collect()

