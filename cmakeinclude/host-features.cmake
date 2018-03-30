# 设置机型特性

# 屏幕类型 LATTICE COLOR
if(T2XD OR T23 OR T27 OR T27G OR CP)
    set(LATTICE ON)
endif()
if(T46 OR T48 OR T49 OR T69)
    set(COLOR ON)
endif()

# 是否去除STD库
if(LATTICE AND (NOT ENABLE_TEST))
    set(NOSTD ON)
else()
    set(NOSTD OFF)
endif()

if(LATTICE)
    add_definitions(-D_NOTSTL_)
endif()

add_definitions(-D_MSGKIT)

# 是否使用OpenGL
if(NOT opengl)
    set(opengl "null")
else()
    string(TOLOWER "${opengl}" opengl)
endif()
if(T49)
    if(${opengl} MATCHES "disable")
        set(ENABLE_OPENGL OFF)
    else()
        set(ENABLE_OPENGL ON)
    endif()
else()
    set(ENABLE_OPENGL OFF)
endif()

# 编译宏定义(config.in.h)
if(LATTICE)
    set(IS_LATTICE 1)
endif()
if(COLOR)
    set(IS_COLOR 1)
endif()
if(ENABLE_MEMCHECK)
    set(IS_MEMCHECK 1)
endif()
#模式测试定义
if(ENABLE_TEST)
    enable_testing()
    set(IS_CODE_TEST 1)
endif()
if(ENABLE_COVER_TEST)
    add_definitions("-fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
    set(IS_COVER_TEST 1)
endif()

# USB功能
if(T46 OR T48 OR T49 OR T69 OR CP OR T27G OR CP920)
    set(_USB ON)
    add_definitions(-DIF_USB_SUPPORT)
endif()

# USB录音功能
if(T46 OR T48 OR T49 OR T69 OR CP OR T27G OR CP920)
    set(_USB_RECORD ON)
    add_definitions(-DIF_USB_RECORD)
endif()

# USB耳机功能
if(T54 OR T42S OR T46S OR T48S)
    set(_USB_AUDIO ON)
    add_definitions(-DIF_SUPPORT_USB_AUDIO)
endif()

# 蓝牙/WiFi功能
if(T46 OR T48 OR T69 OR T49 OR T27G OR CP920)
    set(_WIFI ON)
    set(_BLUETOOTH ON)
    add_definitions(-DIF_SUPPORT_BLUETOOTH)
    add_definitions(-DIF_SUPPORT_WIFI)
    
	# 开启蓝牙手机
	# 开启蓝牙手机联系人
	add_definitions(-DIF_BT_SUPPORT_PHONE)
	add_definitions(-DIF_SUPPORT_BLUETOOTH_CONTACT)
	
	if(CP920)
		add_definitions(-DIF_BT_SUPPORT_A2DP)
	else()
		add_definitions(-DIF_BT_SUPPORT_HEADSET)
	endif()
endif()

# 配置存储方式 INI DBX
string(TOLOWER "${CFG_BACKEND}" CFG_BACKEND)
set(CFG_BACKEND_OPTIONS "dbx" "ini")
if(CFG_BACKEND)
    list(FIND CFG_BACKEND_OPTIONS "${CFG_BACKEND}" _index)
    if (_index EQUAL -1)
        message(FATAL_ERROR "Config backend '${CFG_BACKEND}' not supported! Supported: ${CFG_BACKEND_OPTIONS}")
    endif()
endif()
if(T40G OR T42S OR T46S OR T48S OR T54 OR T27G OR T49 OR CP920 OR CFG_BACKEND STREQUAL "dbx")
    set(ENABLE_NEW_CFG_SERVICE ON)
    add_definitions(-D_NEW_CFG_SERVICE)
    set(CFG_BACKEND "dbx")
else()
    set(ENABLE_NEW_CFG_SERVICE OFF)
    set(CFG_BACKEND "ini")
endif()
message("Config backend: [${CFG_BACKEND}]")

#设备管理平台
if(T46S OR T48S OR T42S OR T27G OR T54 OR CP920 OR ENABLE_DIAGNOSE)
	add_definitions(-DIF_FEATURE_DIAGNOSE)
	message("enable diagnose")
endif()