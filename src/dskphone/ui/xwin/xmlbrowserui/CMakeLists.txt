include_directories(
    include 
    src
  ../../../logic
	../../../uilogic
	../../../uilogic/settinguilogic/include
	../../../../include
	../settingui/include
	../settingui/src
)

if(ENABLE_INSTRUMENT AND ENABLE_XMLBROWSERUI_MOD)
	message("Enable ENABLE_XMLBROWSERUI_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

if(COLOR)
configure_file(src/xmlbrowseruidefine.color.h
               src/xmlbrowseruidefine.h
               COPYONLY)
else()
configure_file(src/xmlbrowseruidefine.black.h
               src/xmlbrowseruidefine.h
               COPYONLY)
endif()

collect()

