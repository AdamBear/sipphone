#ifndef DSSKEY_COMMON_DEFINE_H
#define DSSKEY_COMMON_DEFINE_H

#include <ylstring.h>

#include <yllist.h>
#include <ETLLib.hpp>
#include "dsskey/include/dsskeystructure.h"
//#include "dsskey/include/dsskey_data_define.h"
#include "dsskey/include/dsskey_enumtypes.h"
#include "contacts/directory/include/commondata.h"
#define TRANS_NON 0
#define TRANS_ALL -1


enum DssKeyUI_LockType
{
    DSSKEYUI_LOCK_TYPE_BEGIN = -1,
    DSSKEYUI_LOCK_TYPE_DEFAULT,
    DSSKEYUI_LOCK_TYPE_FLOAT,
    DSSKEYUI_LOCK_TYPE_LOCK,
    DSSKEYUI_LOCK_TYPE_END,
};

// DsskeyFrame父窗口类型
enum DSS_KEY_FRAME_PARENT_TYPE
{
    DSS_KEY_FRAME_PARENT_IDLE, // 该DsskeyFrame是Idle界面的
};

enum DSS_KEY_LED_PRIORITY_TYPE
{
    DSS_KEY_LED_PRIORITY_HIGHEST = 0,
    DSS_KEY_LED_PRIORITY_SECOND_HIGH,
    DSS_KEY_LED_PRIORITY_HIGH,
    DSS_KEY_LED_PRIORITY_NORMAL,
    DSS_KEY_LED_PRIORITY_LOW,
    DSS_KEY_LED_PRIORITY_SECOND_LOW,
    DSS_KEY_LED_PRIORITY_LOWEST
};

// dsskey闪动得模式
enum DSS_KEY_FLUSH_TYPE
{
    REFRESH_TYPE_NONE = -2, //无类型不处理
    REFRESH_TYPE_OFF,       //LED不亮

    REFRESH_TYPE_NO_FLUSH,  // 不闪
    REFRESH_TYPE_SLOW_FLUSH,// 慢闪动 1000ms
    REFRESH_TYPE_FLUSH,     // 闪动 500ms
    REFRESH_TYPE_FAST_FLUSH,// 快速的闪动 300ms
    REFRESH_TYPE_FASTER_FLUSH,// 更快速的闪动 200ms
    REFRESH_TYPE_ALTERNATE_FLUSH,// 交替的闪动 200ms
};

//Dsskey Led灯的颜色
enum DSS_KEY_COLOR_TYPE
{
    LED_COLOR_OFF = -1,
    LED_COLOR_TYPE_RED,
    LED_COLOR_TYPE_GREEN,
    LED_COLOR_TYPE_ORANGE,
    LED_COLOR_GRAY,
    LED_COLOR_GRAY_CIRCLE,
    LED_COLOR_YELLOW,
    LED_COLOR_RED_GREEN,
};

enum DSS_KEY_CLICK_TYPE
{
    PRESS_TYPE_CLICK,
    PRESS_TYPE_LONG_PRESS,
};

// 判断是哪里的Dsskey
enum DSS_KEY_FRAME_TYPE
{
    DSS_KEY_FRAME_LINEKEY = 0,  // Linekey的数据
    DSS_KEY_FRAME_MEMKEY,       // Memkey的数据
};

// Dsskey显示Text的模式
enum DSS_KEY_TEXT_MODE
{
    TEXT_MODE_FIRST_ROW,    // 文字显示为一行
    TEXT_MODE_SEC_ROW,      // 文字显示为两行
};

//显示底色模式，临时添加
enum DSS_KEY_GROUNDPHOTO_MODE
{
    GROUNDPHOTO_MODE_DISALBE,   //显示disable状态的底色
    GROUNDPHOTO_MODE_HIGHLIGHT, //显示高亮状态的底色
    GROUNDPHOTO_MODE_NORMAL,    //显示一般状态的底色
    GROUNDPHOTO_MODE_USING,     //显示处理通话状态的底色
};

enum DSSKEY_DISPLAY_MODE
{
    //非法值：起始值
    DDM_FIRST = -1,

    //占满屏幕的宽度
    DDM_FULL,

    //比通常宽度略宽一点的宽度
    DDM_EXTEND,

    //通常宽度
    DDM_NORMAL,

    //T52使用，显示两个文字的宽度
    DDM_TWO_CHARS,

    //T52使用，只露出一小段，不显示文字和图标
    DDM_NO_LABEL,

    //完全隐藏
    DDM_HIDE,

    //非法值：结束值
    DDM_LAST,
};

enum DSSKEY_LABEL_TYLE
{
    DSS_LT_NORMAL = 1,
    DSS_LT_SHORT,
};

enum PRESENCE_STATUS
{
    PRESENCE_STATUS_NONE = 0,

    PRESENCE_STATUS_ONLINE,
    PRESENCE_STATUS_CHATTY,
    PRESENCE_STATUS_AWAY,
    PRESENCE_STATUS_EXTENDEDAWAY,
    PRESENCE_STATUS_BUSY,
    PRESENCE_STATUS_OFFLINE,
    PRESENCE_STATUS_UNKNOWN,

    PRESENCE_STATUS_MAX,
};

enum Dsskey_Icon_Type
{
    DK_ICON_None = 0,

    DK_ICON_Default,

    // common
    DK_ICON_UnRegistered,
    DK_ICON_Registering,
    DK_ICON_Registered,
    DK_ICON_Using,
//#ifdef _T42
    DK_ICON_Callout,
//#endif
    DK_ICON_Speeddial,
    DK_ICON_Remote_Offline,
    DK_ICON_Remote_Talking,
    DK_ICON_Remote_Online,
    DK_ICON_Remote_Ringing,

    // linekey
    DK_ICON_RemoteDir,

    // EXP39
    DK_ICON_DND,
    DK_ICON_Forward,
    DK_ICON_HOLD,
    DK_ICON_SMS,
    DK_ICON_Triangle,
    DK_ICON_Warning,

    //TO DO:
    // Call Center
    //DK_ICON_ACD,
    //DK_ICON_HOTELING,
    //DK_ICON_ACD_TRACE,
    //DK_ICON_DISPCODE,
    //DK_ICON_ESCALATE,
//#ifdef _T42
    DK_ICON_ACD_IDLE,
    DK_ICON_ACD_AVAILABLE,
    DK_ICON_ACD_UNAVAILABLE,
    DK_ICON_ACD_WRAPUP,
    DK_ICON_RINGING,
    DK_ICON_BLA,
//#endif

    // 页面编号
    DK_ICON_Page_1,
    DK_ICON_Page_2,
    DK_ICON_BLF_PARK,
    DK_ICON_BLF_CALLOUT,
    DK_ICON_BLF_IDLE,
    DK_ICON_BLF_UNKNOWN,
    DK_ICON_BLF_TALK,
    DK_ICON_BLF_RINGING,
    DK_ICON_BLF_HOLD,

    DK_ICON_BLA_PARK,
    DK_ICON_BLA_UNKNOWN,
    DK_ICON_BLA_SEIZED,
    DK_ICON_BLA_PROGRESSING,
    DK_ICON_BLA_ACTIVE,
    DK_ICON_BLA_HELD,
    DK_ICON_BLA_HELD_PRIVATE,
    DK_ICON_BLA_BRIDGE_ACTIVE,
    DK_ICON_BLA_BRIDGE_HELD,
    DK_ICON_BLA_IDLE,
    DK_ICON_BLA_ALERTING,

    DK_ICON_INTERCOM,
    DK_ICON_INTERCOM_TALKING,
    DK_ICON_INTERCOM_HOLD,
    DK_ICON_INTERCOM_RINGING,

    DK_ICON_LINE_FAIL,
    DK_ICON_LINE_FORWARD,
    DK_ICON_LINE_DND,

    DK_ICON_VOICE_MAIL,

    DK_ICON_PICK_UP,
    DK_ICON_GROUP_PICK_UP,
    DK_ICON_CALLPARK,
    DK_ICON_CALLPARK_FAIL,
    DK_ICON_CALLPARK_RINGING,
    DK_ICON_CALLPARK_AUTO_TALK,
    DK_ICON_CALLPARK_MUNUAL_TALK,

    DK_ICON_DTMF,
    DK_ICON_PREFIX_ICON,

    DK_ICON_LOCAL_GROUP,
    DK_ICON_DIRECTORY,
    DK_ICON_XMLBROWSER,

    DK_ICON_LDAP,
    DK_ICON_FAVORITES,

    DK_ICON_CONFERENCE,
    DK_ICON_TRANSFER,
    DK_ICON_REDIAL,

    DK_ICON_TALK,
    DK_ICON_CALLRETURN,

    DK_ICON_RECORD,
    DK_ICON_RECORDING,

    DK_ICON_PAGING,
    DK_ICON_GROUP_LISTENING,

    DK_ICON_COLOR_ACD_AVAILABLE,
    DK_ICON_COLOR_ACD_UNAVAILABLE,
    DK_ICON_COLOR_ACD_WRAPUP,
    DK_ICON_COLOR_ACD_TRACE,
    DK_ICON_ACD_LOGOUT,
    DK_ICON_ACD_DEFAULT,

    DK_ICON_ZERO_TOUCH,
    DK_ICON_MEET_ME,
    DK_ICON_HOTELING_LOGININ,
    DK_ICON_HOTELING_LOGINOUT,

    DK_ICON_DISPCODE,
    DK_ICON_ESCALATE,

    DK_ICON_RETRIEVE_PARK,
    DK_ICON_HOTDESKING,
    DK_ICON_BUDDIES,
    DK_ICON_STATUS,

    DK_ICON_EDK,
    DK_ICON_KEYPAD_LOCK,
    DK_ICON_GET_BALANCE,

    DK_ICON_UC_ONLINE,
    DK_ICON_UC_AWAY,
    DK_ICON_UC_EXTENDED_AWAY,
    DK_ICON_UC_BUSY,
    DK_ICON_UC_OFFLINE,

    DK_ICON_URL_GREEN,
    DK_ICON_URL_RED,
    DK_ICON_URL,

    DK_ICON_MOBILE_BLUE_GREEN,
    DK_ICON_MOBILE_BLUE_CONNETING,
    DK_ICON_MOBILE_BLUE_RED,

    DK_ICON_EDITING,

    DK_ICON_METASWITCH_MESSAGE_LIST,
    DK_ICON_METASWITCH_LOGOUT,

    //自定义图片类型
    DK_ICON_CUSTOM,
};

struct DsskeyDisplayData
{
    int                             iIndex;             // Dsskey对应的index
    DSS_KEY_TEXT_MODE               eTextMode;          // 文字显示的模式,分单行还是多行
    int                             iDsskeyID;          // DsskeyID包含了类型索引灯
    DSSKEY_MODULE_TYPE              eType;
    DSS_KEY_GROUNDPHOTO_MODE        eGroundPhoto;       // Btn的底色
    yl::string                      strIconPath;        // DsskeyIcon的路径
    yl::string                      strFirstRowText;    // 第一行显示的文字
    yl::string                      strPresenceIcon;    // 状态图标的路径
    yl::string                      strShortenText;     // 缩略显示时显示文本
    DssKey_Type                     eDssKeytype;        // Dsskey的类型
    Dsskey_Icon_Type                eIconType;          // 扩展台的ICON类型
    PRESENCE_STATUS         ePresenceType;  //状态图标类型
    YLList<int>                     listAuxiliaryState;     // 辅助状态链表

    //T46使用
    DSS_KEY_COLOR_TYPE              eLightColorType;    // dsskey灯颜色
    DSS_KEY_FLUSH_TYPE              eLightFlushType;    // dsskey灯闪烁速度
    BOOL                            bNeedRefresh;       // 是否需要在UI上重新刷新该dsskey
    BOOL                            bLockKeyed;         // 被锁定的dsskey
    // Page灯状态
    DSS_KEY_COLOR_TYPE              ePageColorType;     // 对应的page灯状态
    DSS_KEY_FLUSH_TYPE              ePageLightFlushType;// 对应的page灯状态
    int                             iPageIconState;     // page icon状态,1 page icon长亮
    unsigned int                    iTimestamp;         // 闪烁定时时间停止
    int                             iLightPriority;     // 对应灯优先级
    int                             iFlushuration;      // 定时时间到停止闪灯
    int                             iStatus;

    // T48
    DSS_KEY_FLUSH_TYPE              eRefreshType;       // Dsskey闪烁类型(如快闪慢闪)
    yl::string                      strSecRowText;      // 第二行显示的文字
    yl::string
    strDownIconPath;    // Dsskey按下状态Icon的路径-T48 反白效果使用
    yl::string                      strFlushIcon1;      // 闪烁Icon1的路径-T48-URL使用
    yl::string                      strFlushIcon2;      // 闪烁Icon2的路径-T48-URL使用

    DsskeyDisplayData()
    {
        clear();
    }
    void clear()
    {
        iIndex = -1;
        eRefreshType = REFRESH_TYPE_NO_FLUSH;
        eTextMode    = TEXT_MODE_FIRST_ROW;
        eGroundPhoto = GROUNDPHOTO_MODE_DISALBE;
        eType = DMT_NON;
        iTimestamp = 0;
        iDsskeyID = -1;
        bNeedRefresh = TRUE;
        eDssKeytype = DT_NA;
        eLightColorType = LED_COLOR_OFF;
        iLightPriority = DSS_KEY_LED_PRIORITY_LOWEST;
        eIconType = DK_ICON_None;
        ePageColorType = LED_COLOR_OFF;
        ePageLightFlushType = REFRESH_TYPE_NO_FLUSH;
        bLockKeyed = FALSE;
        iFlushuration = -1;
        iPageIconState = 0;
        iStatus = -1;
    }

    DsskeyDisplayData & operator = (const DsskeyDisplayData & data)
    {
        if (this == &data)
        {
            return *this;
        }

        iIndex = data.iIndex;
        eRefreshType = data.eRefreshType;
        eTextMode = data.eTextMode;
        eGroundPhoto = data.eGroundPhoto;
        eType = data.eType;
        iTimestamp = data.iTimestamp;
        iDsskeyID = data.iDsskeyID;
        bNeedRefresh = data.bNeedRefresh;
        eDssKeytype = data.eDssKeytype;
        eLightColorType = data.eLightColorType;
        iLightPriority = data.iLightPriority;
        eIconType = data.eIconType;
        ePageColorType = data.ePageColorType;
        ePageLightFlushType = data.ePageLightFlushType;
        bLockKeyed = data.bLockKeyed;
        iFlushuration = data.iFlushuration;
        iPageIconState = data.iPageIconState;
        iStatus = data.iStatus;
        strIconPath = data.strIconPath;
        strPresenceIcon = data.strPresenceIcon;
        strDownIconPath = data.strDownIconPath;
        strFlushIcon1 = data.strFlushIcon1;
        strFlushIcon2 = data.strFlushIcon2;
        strFirstRowText = data.strFirstRowText;
        strSecRowText = data.strSecRowText;
        strShortenText = data.strShortenText;

        return *this;
    }

    ~DsskeyDisplayData() {}
};


typedef YLList<DsskeyDisplayData>   ListDisplayData;

typedef YLList<DssKeyLogicData>     ListLogicData;
typedef YLList<DssKeyLogicData *>    ListpLogicData;


#endif // DSSKEY_COMMON_DEFINE_H
