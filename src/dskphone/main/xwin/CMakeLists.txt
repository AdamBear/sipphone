#################################
#module:Screen.exe
#path:/bin/Screen.exe
#################################

if (MSVC)
	set(target_name xwin_exe)
	add_fast_executable()
	_WC_GET_BUILD_DIRECTORY_VARS()

	target_link_libraries(${target_name} 
		${LINK_LIB_BASELIBS}
    	${LINK_LIB_3RD}
    	xcommon
    	xwin
		logic
		uilogic
	)
	
	add_target_link_property(${target_name})

else()

if(COLOR)
	aux_source_directory(. SRC_FILES)

	add_executable(main
		${SRC_FILES})
		
	set(target_name main)

    set_target_properties(${target_name}
	   		 PROPERTIES OUTPUT_NAME "dskPhone.exx")
             
	target_link_libraries(${target_name}
	xcommon
	  xwin
	  uilogic
	  logic    
    pthread
    m
    ssl
    crypt
    crypto
    durl
    expat
    strophe
    xmpp
    ldap
    mkservice
    sipservice
    sysservice
    ipvpservice
    
    TaskAction
    ETL
    dsklog
    dskthread
    ETLMsgHandler
    CommonAPI
    DeviceLib
    xmlparser
    ConfigParser
    regexp
    YLSTL
    resource
    presence

    bluetooth
    dl
    bt
    dbus-1
    jpeg
    png16
    freetype
)

	if(ENABLE_MEMCHECK)
	    target_link_libraries(${target_name}
	       memory
	       )
	endif()

	if(ENABLE_TESTABILITY)
		target_link_libraries(${target_name}
			testability
			)
    endif()

    if(ENABLE_PROF)
        target_link_libraries(${target_name}
            prof
            rt
            )
    endif()
    
    if(_USB_AUDIO)
        target_link_libraries(${target_name}
            usbheadset
            )
    endif()

#    if(FEATURE_METASWITCH)
#        target_link_libraries(${target_name}
#            MetaswitchUI
#            )
#    endif()


	if(ENABLE_JSON)
	    target_link_libraries(${target_name}
	        cjson
	        )
	    set(ENABLE_JSON OFF)
	endif()

deep_strip(${target_name})

target_install(${target_name})


else()

aux_source_directory(. SRC_FILES)

add_executable(main
    ${SRC_FILES})
    
set(target_name main)


	set_target_properties(${target_name}
	    PROPERTIES OUTPUT_NAME "Screen.exx"
	)

	target_link_libraries(${target_name}
	xcommon
	  xwin 
	  uilogic
    logic    
    pthread
    m
    ssl
    crypt
    crypto
    durl
    ldap
    mkservice
    sipservice
    sysservice
    ipvpservice
    TaskAction
    ETL
    dsklog
    dskthread
    ETLMsgHandler
    CommonAPI
    DeviceLib
    xmlparser
    ConfigParser
    YLSTL
	resource
	regexp
    dl
	curl
	)

    if(_BLUETOOTH)
        target_link_libraries(${target_name}
            bluetooth
            bt
            dbus-1
            )
    endif()

    if(_USB_AUDIO)
        target_link_libraries(${target_name}
            usbheadset
            )
    endif()

    if(ENABLE_PROF)
        target_link_libraries(${target_name}
            prof
            )
    endif()


	if(_BLUETOOTH)
	    target_link_libraries(${target_name}
	        bt
	        bluetooth
	        dbus-1
	   			expat
	        )
	endif()
	
    if(T42S)
        target_link_libraries(${target_name}
           xmpp
           presence
           )
    endif()

	if(ENABLE_PROF)
	    target_link_libraries(${target_name}
	        prof
	        )
	endif()
	
	if(ENABLE_MEMCHECK)
	    target_link_libraries(${target_name}
	        memory
	        )
	endif()
	
	if(_USB_AUDIO)
	    target_link_libraries(${target_name}
	        usbheadset
	        )
	endif()
	
	if(ENABLE_JSON)
	    target_link_libraries(${target_name}
	        cjson
	        )
	    set(ENABLE_JSON OFF)
	endif()

DEEP_STRIP(${target_name})

target_install(${target_name})
UPX_COMPRESS(${target_name})

STRIP_COMMENT(${target_name})

endif()

endif()

