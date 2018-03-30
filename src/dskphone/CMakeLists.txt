if(T48 OR T49)
    set(ENABLE_QT_LIB ON)
else()
	set(ENABLE_QT_LIB OFF)
endif()

if (ENABLE_QT_LIB)
	include(qtbuild.cmake)
endif()

if(ENABLE_MEMCHECK)
    force_include_header_file(${CMAKE_SOURCE_DIR}/src/3rdlib/memory/memory.h)
    add_definitions(-D_DEBUGMEM)
endif()

if(ENABLE_TESTABILITY)
    add_definitions(-DIF_ENABLE_TESTABILITY)
endif()

if(COLOR)
	add_definitions(-D_SCRRENSAVERSCOLOR)
endif()

# 使用的UI机型目录名称
if(T2X OR T2XD OR T23 OR T27 OR T27G OR CP)
    set(host_dir "xwin")
elseif(T42)
    set(host_dir "xwin")
elseif(T46 OR T69)
    set(host_dir "xwin")
elseif(T48)
    set(host_dir "t48")
elseif(CP920)
    set(host_dir "xwin")
elseif(DECT)
    set(host_dir "dect")
elseif(T49)
    set(host_dir "t49")
endif()

include_directories(
    logic
    extension
    ui/${host_dir}
    win32
    )

if(T49 OR XWIN)
include_directories(uilogic)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/uilogic)
endif()

if(XWIN)
include_directories(
	ui/common
	ui/xwin
	)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/ui/common)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/ui/xwin)
endif()

if(NOT ENABLE_TEST)
    _WC_SET_FOLDER_ROOT_DIR()
endif()

add_all_subdirectories()
