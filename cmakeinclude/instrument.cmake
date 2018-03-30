#############################################################################
#											 检测多线程函数优化配置
#										目前仅T23机型 ARMDSPG编译器支持
#############################################################################
#功能开关
option(ENABLE_INSTRUMENT "插入分析函数功能" OFF)

if(ENABLE_INSTRUMENT)
    message("++++++++Enable use instrument functions++++++++")

    option(ENABLE_FILTER_FILE "过滤文件列表功能" ON)
    option(ENABLE_FILTER_FUN "过滤函数接口功能" ON)

    #logic层开关定义
    option(ENABLE_ACCOUNT_MOD "account模块插入分析函数功能" ON)
    option(ENABLE_ADAPT_MOD "adapterbox模块插入分析函数功能" OFF)
    option(ENABLE_BLF_MOD "blf模块插入分析函数功能" ON)
    option(ENABLE_BROADSOFT_MOD "broadsoft模块插入分析函数功能" ON)
    option(ENABLE_COMMONUNITS_MOD "commonunits模块插入分析函数功能" OFF)
    option(ENABLE_CONTACTS_MOD "contacts模块插入分析函数功能" ON)
    option(ENABLE_DSSKEY_MOD "dsskey模块插入分析函数功能" OFF)
    option(ENABLE_EXTENDED_MOD "extended模块插入分析函数功能" ON)
    option(ENABLE_FEATURE_MOD "feature模块插入分析函数功能" ON)
    option(ENABLE_IDLESCREEN_MOD "idleScreen模块插入分析函数功能" ON)
    option(ENABLE_KEYGUARD_MOD "keyguard模块插入分析函数功能" ON)
    option(ENABLE_LAMP_MOD "lamp模块插入分析函数功能" ON)
    option(ENABLE_PRESENCE_MOD "Presence模块插入分析函数功能" OFF)
    option(ENABLE_TALK_MOD "talk模块插入分析函数功能" ON)
    option(ENABLE_VOICE_MOD "voice模块插入分析函数功能" OFF)

    #t2x  ui层开关定义
    option(ENABLE_COMMONUI_MOD "commonui插入分析函数功能" OFF)
    option(ENABLE_DIRECTORYUI_MOD "directoryui插入分析函数功能" ON)
    option(ENABLE_DOTWINDOWS_MOD "dotwindows插入分析函数功能" OFF)
    option(ENABLE_DSSKEYUI_MOD "dsskeyui插入分析函数功能" OFF)
    option(ENABLE_HOTDESKINGUI_MOD "hotdeskingui插入分析函数功能" OFF)
    option(ENABLE_IDLESCREENUI_MOD "idlescreenui插入分析函数功能" ON)
    option(ENABLE_KOREANIME_MOD "koreanime插入分析函数功能" OFF)
    option(ENABLE_POPUPBOX_MOD "popupbox插入分析函数功能" OFF)
    option(ENABLE_RMTSTATUSUI_MOD "remotestatusui插入分析函数功能" ON)
    option(ENABLE_SCREEN_MOD "screen插入分析函数功能" OFF)
    option(ENABLE_SETTING_MOD "setting插入分析函数功能" ON)
    option(ENABLE_SETTINGUI_MOD "settingui插入分析函数功能" ON)
    option(ENABLE_TALKUI_MOD "talkui插入分析函数功能" ON)
    option(ENABLE_TESTMODE_MOD "testmode插入分析函数功能" OFF)
    option(ENABLE_UIMANAGER_MOD "uimanager插入分析函数功能" ON)
    option(ENABLE_XMLBROWSERUI_MOD "xmlbrowserui插入分析函数功能" OFF)


    #############################################################################
    #											配置编译选项
    #############################################################################

    #配置插入分析函数编译选项
    set(ENABLE_INSTRUMENT_CFG "-finstrument-functions")


    #过滤编译选项配置
    set(FILTER_FILE_CFG "-finstrument-functions-exclude-file-list=")


    #过滤编译选项配置
    set(FILTER_FUN_CFG "-finstrument-functions-exclude-function-list=")


    #############################################################################
    #											配置文件过滤列表
    #############################################################################

    #默认过滤底层模块
    #set(FILTER_FILE_CFG "${FILTER_FILE_CFG}${CMAKE_SOURCE_DIR}/src/baselibs")
    #set(FILTER_FILE_CFG "${FILTER_FILE_CFG},${CMAKE_SOURCE_DIR}/src/include")
    #set(FILTER_FILE_CFG "${FILTER_FILE_CFG},${CMAKE_SOURCE_DIR}/src/3rdlib")

    set(FILTER_FILE_CFG "${FILTER_FILE_CFG}src/baselibs")
    set(FILTER_FILE_CFG "${FILTER_FILE_CFG},src/include")
    set(FILTER_FILE_CFG "${FILTER_FILE_CFG},src/3rdlib")
    set(FILTER_FILE_CFG "${FILTER_FILE_CFG},new,dsskeystructure.h")

    #模式过滤文件列表配置
    #logic各个模块需要过滤文件
    #if(ENABLE_ACCOUNT_MOD)
    #	set(FILTER_FILE_CFG "${FILTER_FILE_CFG},${CMAKE_SOURCE_DIR}/src/dskphone/logic/XXXXX.h,yyyy.h,ppp.cpp")
    #endif()
    #		...

    if(ENABLE_CONTACTS_MOD)
        set(FILTER_FILE_CFG "${FILTER_FILE_CFG},commondata.h")
    endif()


    if(ENABLE_IDLESCREEN_MOD)
        set(FILTER_FILE_CFG "${FILTER_FILE_CFG},idlescreen_def.h")
    endif()

    #UI各个模块需要过滤文件
    #if(ENABLE_COMMONUI_MOD)
    #	set(FILTER_FILE_CFG "${FILTER_FILE_CFG},${CMAKE_SOURCE_DIR}/src/dskphone/ui/t2x/XXXXX.h,yyyy.h,ppp.cpp")
    #endif()
    #	  ...

    if(ENABLE_SETTINGUI_MOD)
        set(FILTER_FILE_CFG "${FILTER_FILE_CFG},datafactory.h,common.h")
    endif()

    #配置过滤文件列表功能
    if(ENABLE_INSTRUMENT AND ENABLE_FILTER_FILE)
        add_definitions("${FILTER_FILE_CFG}")
    endif()


    #############################################################################
    #											配置接口函数过滤列表
    #############################################################################

    #默认过滤接口
    set(FILTER_FUN_CFG "${FILTER_FUN_CFG}main")

    #logic各个模块需要过滤接口
    #if(ENABLE_ACCOUNT_MOD)
    #		set(FILTER_FUN_CFG "${FILTER_FUN_CFG}xxxx,yyyy,kkk,lll")
    #endif()
    #  ...

    if(ENABLE_EXTENDED_MOD)
        set(FILTER_FUN_CFG "${FILTER_FUN_CFG},OnHotDeskingTimer")
    endif()

    if(ENABLE_FEATURE_MOD)
        set(FILTER_FUN_CFG "${FILTER_FUN_CFG},FeatureOnTimer")
    endif()

    if(ENABLE_EXTENDED_MOD)
        set(FILTER_FUN_CFG "${FILTER_FUN_CFG},OnHotDeskingTimer")
    endif()

    if(ENABLE_TALK_MOD)
        set(FILTER_FUN_CFG "${FILTER_FUN_CFG},CancelTimer")
    endif()

    #UI各个模块需要过滤文件
    #if(ENABLE_COMMONUI_MOD)
    #	set(FILTER_FUN_CFG "${FILTER_FUN_CFG}jjj,ggg,kkk,lll")
    #endif()
    #	 ...

    #配置过滤文件列表功能
    if(ENABLE_INSTRUMENT AND ENABLE_FILTER_FUN)
        add_definitions("${FILTER_FUN_CFG}")
    endif()

    #定义一个宏开关 声明C++函数不插入分析函数功能
    if(ENABLE_INSTRUMENT AND ENABLE_FILTER_FUN)
        add_definitions(-D_FILTER_FUNCTIONS_)
    endif()

endif()
