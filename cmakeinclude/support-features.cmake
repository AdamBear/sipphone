######################################
#define macro control program's functional
######################################

#include features-define
if(NOT market)
    set(market "null")
endif()

if(COLOR)
    set(FEATURE_BROADTOUCH_UC 1)
endif()

#市场类型
if (${market} EQUAL "smb")
    set(INCLUDE_FEATURE_H, features-define-smb.h)
elseif(${market} EQUAL "bsft")
    set(INCLUDE_FEATURE_H, features-define-bsft.h)
elseif(${market} EQUAL "cc")
    set(INCLUDE_FEATURE_H, features-define-cc.h)
else()
  if(DECT)
    set(INCLUDE_FEATURE_H features-define-dect.h)
  else()
    set(INCLUDE_FEATURE_H features-define.h)
  endif()
endif()

# 客户特制
if (custom MATCHES "IF_CUSTOMER_SUPPORT_[0-9]_*")
# 不使用cmake的替换机制、因为缓存不易清除，且容易引发错误
    message("Info: current customer is \"${custom}\" " )
    set(${custom} 1)
else()
    message("Info: current customer is neutral version" )
endif()

#宏定义
if(T46)
    add_definitions(-D_T4X -D_T46 -DIF_ATLINK_SPECIFIC=0 -DIF_NEW_FILE_PATH -DIF_SUPPORT_POWERSAVING)
    # T54 同时开了T46，需要单独再设置
    #if(T54)
    #    add_definitions(-D_HIGHLIGHT)
    #endif()
elseif(T69)
    add_definitions(-D_T4X -D_T69 -DIF_ATLINK_SPECIFIC=0 -DIF_NEW_FILE_PATH )
elseif(T48)
    add_definitions(-D_T4X -D_T48 -DIF_ATLINK_SPECIFIC=0 -DIF_NEW_FILE_PATH -DIF_SUPPORT_POWERSAVING)
elseif(T49)
    add_definitions(-D_T4X -D_T49 -DIF_ATLINK_SPECIFIC=0 -DIF_NEW_FILE_PATH -DIF_BT_SUPPORT_PHONE -DIF_SUPPORT_VIDEO -DIF_SUPPORT_HDMI -DIF_FEATURE_H323 -DIF_FEATURE_CLOUD -DIF_SUPPORT_POWERSAVING)
    if(ENABLE_OPENGL)
        add_definitions(-DIF_OPENGL_BACKEND_SUPPORT)
    endif()
elseif(T2XD)
    add_definitions(-DPHONE_TYPE=28 -DIF_ATLINK_SPECIFIC=0 -DIF_NEW_DRIVER -DIF_NEW_FILE_PATH -DIF_LCD_SIZE_132_64 -DIF_XML_TRANS_LANG -D_T21 -D_MSGKIT)
elseif(T23 OR T27 OR T27G OR T42)
    add_definitions(-DPHONE_TYPE=28 -DIF_ATLINK_SPECIFIC=0 -DIF_NEW_DRIVER -DIF_NEW_FILE_PATH -DIF_LCD_SIZE_132_64 -DIF_XML_TRANS_LANG -D_T23 -D_MSGKIT)
elseif(CP)
    add_definitions(-DPHONE_TYPE=28 -DIF_ATLINK_SPECIFIC=0 -DIF_NEW_DRIVER -DIF_NEW_FILE_PATH -DIF_LCD_SIZE_132_64 -DIF_XML_TRANS_LANG -D_CP -D_MSGKIT)
elseif(CP920)
	add_definitions(-DPHONE_TYPE=28 -D_CP920 -DIF_ATLINK_SPECIFIC=0 -DIF_NEW_DRIVER -DIF_NEW_FILE_PATH -DIF_LCD_SIZE_132_64 -DIF_XML_TRANS_LANG -D_CP -D_MSGKIT)
elseif(T2X)
    add_definitions(-DPHONE_TYPE=28 -DIF_ATLINK_SPECIFIC=0 -D_MAP_2_LIST_ -DIF_HAVE_MEMORY -DIF_HAVE_EXP -DIF_XML_TRANS_LANG)
elseif(T20)
    add_definitions(-DPHONE_TYPE=20 -DIF_ATLINK_SPECIFIC=0 -D_MAP_2_LIST_ -DIF_XML_TRANS_LANG)
elseif(DECT)
    add_definitions(-D_DECT -DIF_SUPPORT_PSTN_ACCOUNT -DIF_SUPPORT_FEATURE_SU)
endif()

#编译配置
configure_file("${CMAKE_SOURCE_DIR}/src/include/config.in.h" "${CMAKE_BINARY_DIR}/config.h" @ONLY)

include_directories(${CMAKE_BINARY_DIR})

force_include_header_file(config.h)
force_include_header_file(${INCLUDE_FEATURE_H})

if (custom MATCHES "IF_CUSTOMER_SUPPORT_[0-9]_*")
    unset(${custom} CACHE)
    unset(custom CACHE)
endif()

