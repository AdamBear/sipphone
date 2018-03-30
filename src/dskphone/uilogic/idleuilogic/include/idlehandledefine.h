#ifndef _IDLE_HANDLE_DEFINE_H_
#define _IDLE_HANDLE_DEFINE_H_
#include <ylstring.h>
#include <ylvector.h>
#include <yllist.h>
#include <ETLLib.hpp>
#include <common/common_data_define.h>
#include "idlescreen/include/idlescreen_def.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include <idleuilogic/include/idledefine.h>
#include "extended/xmlbrowser/include/logicanduicommon.h"

#define MSGBOX_ID_NOTE      MSGBOX_ID_HOTDESK_WAITING + 100
#define MIN_IDLE_MESSAGE_BOX_SOFTKEY_NUM              4

typedef YLList<IdleStatusItem>  LIST_IDLE_STATUS_ITEM;
typedef YLList<ICON_TYPE>       LIST_ICON_TYPE;
typedef YLVector<SoftKey_TYPE>  VEC_SOFTKET_TYPE;
typedef YLVector<int>           VEC_ID;
typedef YLVector<StatusItem>    VEC_STATUS_ITEM;
typedef YLList<CustomNotify>    VEC_CUSTOM_NOTIFY;

struct VoiceMailData
{
    int  m_nCountInfo;
    int  m_nOnlyAccountId;
    bool m_bIsSetVoiceMailNumber;
    int  m_nUnReadCount;
    VoiceMailData()
    {
        ReSet();
    }

    void ReSet()
    {
        m_nCountInfo = -1;
        m_nOnlyAccountId = -1;
        m_nUnReadCount = 0; //未读信息数据
        m_bIsSetVoiceMailNumber = false;
    }
};

struct TextMessageData
{
    int m_nCountInfo;
    int m_nUnReadCount;//未读信息数据
    TextMessageData()
    {
        ReSet();
    }
    void ReSet()
    {
        m_nCountInfo = -1;
        m_nUnReadCount = -1;
    }
};

struct PopupBoxData
{
    int                     m_iType;      //弹窗类型
    int                     m_nId;        //弹窗id
    yl::string              m_strIcon;    //提示图标
    yl::string              m_strTitle;   //提示标题
    yl::string              m_strNote;    //提示内容
    VEC_SOFTKET_TYPE        m_vecSoftkey; //softkey
    int                     m_nTimes;     //退出时间
    PopupBoxData()
    {
        ReSet();
    }

    void ReSet()
    {
        m_iType = -1; //默认tips类型
        m_nId = -1;  //默认无效的弹窗ID
        m_strIcon = "";
        m_strTitle = "";
        m_strNote = "";
        m_nTimes =
            -1;//默认不设置时间，即采用弹出框模块默认设置 为0时表示不自动退出
        m_vecSoftkey.clear();
        m_vecSoftkey.push_back(ST_EMPTY);
        m_vecSoftkey.push_back(ST_EMPTY);
        m_vecSoftkey.push_back(ST_EMPTY);
        m_vecSoftkey.push_back(ST_EMPTY);
    }
};

struct ST_TextNotifyData
{
    bool                    m_bFlashIcon; // 图标是否闪烁
    bool                    m_bFlashNote; // 文本是否闪烁
    yl::string              m_strNote;    // 提示文本
    LIST_ICON_TYPE          m_listIconType; // 闪烁使用的图标

    ST_TextNotifyData()
    {
        ReSet();
    }

    void ReSet()
    {
        m_bFlashIcon = false; //默认tips类型
        m_bFlashNote = false;  //默认无效的弹窗ID
        m_strNote = "";
        m_listIconType.clear();
    }
};

// 话机状态数据结构.
struct IdleIconInfo
{
    bool        m_bFlash;           // 图标闪烁类型
    int         m_eIconType;        // 图片类型
    yl::string  m_strHint;          // 提示内容.

    IdleIconInfo()
    {
        Reset();
    }

    void Reset()
    {
        m_bFlash = false;
        m_eIconType = IT_UNKNOWN;
        m_strHint.clear();
    }
};

#define POPUP_BOX_SHOWING_SHORT_TIME    2000
#define POPUP_BOX_SHOWING_LONG_TIME     3000

enum IDLE_ENTER_PAGE_TYPE_E
{
    IEPT_NONE = -1,

    IEPT_BROADSOFT_CALLLOG_PLACEED = 0,
    IEPT_BROADSOFT_CALLLOG_MISSED,
    IEPT_LOCAL_CALLLOG_PLACEED,
    IEPT_LOCAL_CALLLOG_CALL_FORWARD,
    IEPT_LOCAL_CALLLOG_MISSED,

    IEPT_PICKUP_MENU_EXIT,
    IEPT_PICKUP_MENU_UPDATE,

    IEPT_UNLOCK_CHECK = 20,
    IEPT_HOT_DESK,
    IEPT_MAIN_MENU,
    IEPT_HOTELING_LOGIN,
    IEPT_HOTELING_LOGIN_ACD_OFF,
    IEPT_ACD_UNAVAILABLE_REASON_CODE,
    IEPT_ACD_DISPCODE,
    IEPT_GUEST_UNLOCK_CHECK,
    IEPT_BALANCE,
    IEPT_HOT_DESK_BASE_DATA,
    IEPT_PSTN_ACCOUNT_LIST,
    IEPT_TEXTMESSAGE_VIEW_INMSG,
    IEPT_TEXTMESSAGE_INBOX,
    IEPT_USB_VIEW_CAMERA,
    IEPT_BLUE_TOOTH_MENU,
    IEPT_VIEW_VOICE_MAIL,
    IEPT_WIFI_MENU,

#if IF_FEATURE_METASWITCH_VOICEMAIL
    IEPT_MTSW_VOICE_MAIL_LOGIN,
    IEPT_MTSW_VOICE_MAIL_LIST,
#endif

#if IF_FEATURE_METASWITCH_ACD
    IEPT_MTSW_ACD_MYSTATUS,
#endif

#if IF_FEATURE_METASWITCH_CALLLOG
    IEPT_MTSW_CALLLOG_PLACEED,
#endif

#if IF_FEATURE_GENBAND_E911
    IEPT_E911_SET_LOCATION,
#endif

#if IF_SUPPORT_BLUETOOTH
    IEPT_BLUE_TOOTH,
#endif

#if IF_SUPPORT_WIFI
    IEPT_WIFI,
#endif

};

typedef YLVector<IdleIconInfo>  VEC_ICON_INFO;

#endif //end of _IDLE_HANDLE_DEFINE_H_

