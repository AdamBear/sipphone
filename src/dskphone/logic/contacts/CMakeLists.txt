include_directories(
    ../broadsoft/xsiactionhelper/include
    )
 
if(ENABLE_INSTRUMENT AND ENABLE_CONTACTS_MOD)
	message("Enable ENABLE_CONTACTS_MOD instrument")
	add_definitions("${ENABLE_INSTRUMENT_CFG}")
endif()

add_all_subdirectories()
