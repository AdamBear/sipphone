#################################
#module:dskPhone.exx
#path:/bin/dskPhone.exx
#################################

qt_build(t48 "exe")

include_directories(
		../../ui/t48/uicommon
		../../ui/t48/include
)

if(ENABLE_MEMCHECK)
    target_link_libraries(${target_name}
       memory
       )
endif()

if (MSVC)
    set(LIB_NAME_PREFIX "${PHONE_HOST_NAME}_")
    set(LINK_LIB_UI 
        ${LIB_NAME_PREFIX}setting
        ${LIB_NAME_PREFIX}talkui
        ${LIB_NAME_PREFIX}idleui
        ${LIB_NAME_PREFIX}settingui
        ${LIB_NAME_PREFIX}BaseUI
        ${LIB_NAME_PREFIX}DirectoryUI
        ${LIB_NAME_PREFIX}CallManager
        ${LIB_NAME_PREFIX}MessageBox
        ${LIB_NAME_PREFIX}MainWnd
        ${LIB_NAME_PREFIX}MainWndHelp
        ${LIB_NAME_PREFIX}globalInputHook
        ${LIB_NAME_PREFIX}ImageManager
        ${LIB_NAME_PREFIX}InputMethod
        ${LIB_NAME_PREFIX}QtCommon
        ${LIB_NAME_PREFIX}UIKernel
        ${LIB_NAME_PREFIX}UIManager
        ${LIB_NAME_PREFIX}dsskeyui
        ${LIB_NAME_PREFIX}xmlbrowserui
        ${LIB_NAME_PREFIX}zerosptouchui
        ${LIB_NAME_PREFIX}messageui
        ${LIB_NAME_PREFIX}edkui
        ${LIB_NAME_PREFIX}VirtualVoiceMail
        ${LIB_NAME_PREFIX}WiFiUI
        ${LIB_NAME_PREFIX}ScreenSavers
        ${LIB_NAME_PREFIX}ylMouseDriverPlugin
        ${LIB_NAME_PREFIX}KmReview
        ${LIB_NAME_PREFIX}testmode
        ${LIB_NAME_PREFIX}MetaswitchUI
	${LIB_NAME_PREFIX}customtheme
        )
        
	set_target_properties(${target_name}
	   		 PROPERTIES OUTPUT_NAME "dskPhoneT48")
			 
	target_link_libraries(${target_name}
	    ${LINK_LIB_BASELIBS}
        ${LINK_LIB_3RD}
        ${LINK_LIB_UI}
        logic
	)
    
    add_target_link_property(${target_name})
else()
    set_target_properties(${target_name}
	   		 PROPERTIES OUTPUT_NAME "dskPhone.exx")
             
	target_link_libraries(${target_name}
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
	    ETLMsgHandler
	    CommonAPI
	    DeviceLib
	    xmlparser
	    ConfigParser
	    regexp
	    resource
	    presence

	    bluetooth
	    bt
	    dbus-1
	    setting    
	    talkui   
	    idleui
	    settingui 
	    BaseUI
	    DirectoryUI
	    CallManager
	    MainWnd
	    MainWndHelp
	    MessageBox
		  edkui
	    KmReview
	    globalInputHook
	    ImageManager
	    InputMethod    
	    QtCommon
	    UIKernel
	    UIManager
	    testmode
	    resource
	    logic
	    dsskeyui
	    zerosptouchui
	    WiFiUI
	    xmlbrowserui  
	    VirtualVoiceMail   
	    messageui  
	    YLSTL
	    ScreenSavers
		  customtheme
		
		freetype
	)
    
	
	if(ENABLE_TESTABILITY)
		target_link_libraries(${target_name}
			testability
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
            rt
            )
    endif()
    
    if(FEATURE_METASWITCH)
        target_link_libraries(${target_name}
            MetaswitchUI
            )
    endif()
endif()

if(ENABLE_JSON)
    target_link_libraries(${target_name}
        cjson
        )
    set(ENABLE_JSON OFF)
endif()

deep_strip(${target_name})

target_install(${target_name})