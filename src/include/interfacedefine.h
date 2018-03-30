/************************************************************************/
/* 对外接口公共头文件，主要进行公共消息与数据结构定义                   */
/************************************************************************/

#ifndef __INTERFACEDEFINE_H__
#define __INTERFACEDEFINE_H__

// 包含model
#include "globalmodel.h"
#include "dsklog/log.h"
#include "path_define.h"
#include "keymap.h"
#include "common/common_data_define.h"
#include "setting/setting_data_define.h"
#include "service_sip_define.h"
#include "service_sip_interface.h"
#include "autop_msg_mk.h"
#include "service_dsk_interface.h"
#include "service_config.h"
#include "devicelib/phonedevice.h"
#include "translateiddefine.h"

//黑白屏绘图线程名
#define VP_SCREEN_THREAD_NAME       "app_vpScreen"
//GUI主线程名
#define VP_PHONE_THREAD_NAME        "app_vpPhone"
//彩屏EXP扩展台绘图线程名
#define VP_EXP_COLOR_THREAD_NAME "exp_drawer_color"
//看门狗线程名
#define WATCHDOG_THREAD_NAME        "app_WatchDog"
//Led线程名
#define LED_THREAD_NAME             "app_LedLogic"

#ifndef VP_AP_THREAD_NAME
#define VP_AP_THREAD_NAME           "autoServer"
#endif // VP_AP_THREAD_NAME
#define VP_BLFLIST_THREAD_NAME      "blflistname"

#define SELECT_BY_T46ANDT29(T46OBj, T29Obj) ((DEVICELIB_IS_T46(devicelib_GetPhoneType())) ? (T46OBj) : (T29Obj))
#define SELECT_BY_T46_T29_T69(T46Obj, T29Obj, T69Obj) ((DEVICELIB_IS_T46(devicelib_GetPhoneType()) || PT_T54S == devicelib_GetPhoneType()) ? (T46Obj) : ((PT_T29 == devicelib_GetPhoneType()) ? (T29Obj) : (T69Obj)))

template<typename T>
T SELECT_BY_T46_T29_T69_T54_T52(T T46Obj, T T29Obj, T T69Obj, T T54Obj, T T52Obj)
{
    PHONE_TYPE_DEF eType = devicelib_GetPhoneType();
    switch (eType)
    {
    case PT_T46:
    case PT_T46S:
        {
            return T46Obj;
        }
        break;
    case PT_T29:
        {
            return T29Obj;
        }
        break;
    case PT_T69:
    case PT_T69S:
        {
            return T69Obj;
        }
        break;
    case PT_T54S:
        {
            return T54Obj;
        }
        break;
    default:
        {
            return T52Obj;
        }
        break;
    }

    return T46Obj;
}

// 通话类型 使用者 vpPhone, appCallList
enum CALL_TYPE
{
    CT_UNKNOW = -1,
    CT_CALLALL = 0,
    CT_OUTGOING,
    CT_INCOMING,
    CT_MISSCALL,
    CT_FORWARDCALL,
    CT_MULTICAST_OUT,
    CT_MULTICAST_IN,
    CT_PARK_NOTIFIY,
    CT_AOC_NOTIFY,
    CL_CONTACT_TO_CALL = 0x1000
};

// Screen模块接收的消息
enum SCREEN_MESSAGE_CODE
{
    SCREEN_MSG_FIRST    = DSK_MSG(DSK_MSG_SCREEN, DSK_MSG_INDEX),
    SCREEN_MSG_WINDOW_REFRESH,       //01 通知ShareMemory有变化, 显示要更新
    SCREEN_MSG_GET_KEY,              //05 wParam:0                                  lParam:0
    SCREEN_MSG_GET_INITPARAM,        //09 wParam:0,lParam:0,返回值低8位bit0~bit7表示耳麦(T80)的状态，bit8~bit15表示手柄状态
    SCREEN_MSG_NETWORK_SCRIPT_RUN,   //0a wParam:0/1(0: After; 1: Begin)            lParam:0
    SCREEN_MSG_INSERT_EHS,           //0b wParam:EHS index插入无线耳麦的消息
    SCREEN_MSG_REMOVE_EHS,           //0c wParam:EHS index拔出无线耳麦的消息
    SCREEN_MSG_LAST
};

enum CALL_RELATION
{
    CR_NONE = -1,
    CR_BUSY,
    CR_REJECTED,
    CR_TRANS_TO,
    CR_FWD_TO,
    CR_TRANS_FROM,
    CR_FWD_FROM,
};

#endif //__INTERFACEDEFINE_H__
