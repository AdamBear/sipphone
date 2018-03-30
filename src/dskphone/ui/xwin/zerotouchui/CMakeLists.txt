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

if(ENABLE_INSTRUMENT AND ENABLE_ZEROTOUCHUI_MOD)
	message("Enable ENABLE_ZEROTOUCHUI_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

collect()

