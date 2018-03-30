#ifndef DSSKEY_COMMON_DEFINE_H
#define DSSKEY_COMMON_DEFINE_H

#include <ylstring.h>

#include <yllist.h>
#include <ETLLib.hpp>
#include "dsskey/include/dsskeystructure.h"
//#include "dsskey/include/dsskey_data_define.h"
#include "dsskey/include/dsskey_enumtypes.h"

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
enum DSS_KEY_FLASH_TYPE
{
    REFRESH_TYPE_NO_FLASH,          // 不闪
    REFRESH_TYPE_SLOW_FLASH,        // 慢闪动 1000ms
    REFRESH_TYPE_FLASH,             // 闪动 500ms
    REFRESH_TYPE_FAST_FLASH,        // 快速的闪动 300ms
    REFRESH_TYPE_FASTER_FLASH,      // 更快速的闪动 200ms
    REFRESH_TYPE_ALTERNATE_FLASH,   // 交替的闪动 200ms
    REFRESH_TYPE_END,               // End
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
    GROUNDPHOTO_MODE_RED,       //需要亮红灯
    GROUNDPHOTO_MODE_GREEN,     //需要亮绿灯
    GROUNDPHOTO_MODE_ORANGE,    //需要亮橙色灯
};

enum DSSKEY_DISPLAY_MODE
{
    DDM_FIRST = -1,
    DDM_FULL,
    DDM_NORMAL,
    DDM_LAST,
};

enum Dsskey_Icon_Type
{
    DK_ICON_None,

    DK_ICON_Default,

    // common
    DK_ICON_UnRegistered,
    DK_ICON_Registering,
    DK_ICON_Registered,
    DK_ICON_Using,
#ifdef _T42
    DK_ICON_Callout,
#endif
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
    DK_ICON_ACD,
    DK_ICON_HOTELING,
    DK_ICON_ACD_TRACE,
    DK_ICON_DISPCODE,
    DK_ICON_ESCALATE,
#ifdef _T42
    DK_ICON_ACD_IDLE,
    DK_ICON_ACD_AVAILABLE,
    DK_ICON_ACD_UNAVAILABLE,
    DK_ICON_ACD_WRAPUP,
    DK_ICON_RINGING,
    DK_ICON_BLA,
#endif

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

};


struct DsskeyLabelData
{
    DSS_KEY_TEXT_MODE               eTextMode;          // 文字显示的模式,分单行还是多行
    yl::string                      strFirstRowText;    // 第一行显示的文字
    yl::string                      strSecRowText;      // 第二行显示的文字

    DsskeyLabelData()
    {
        clear();
    }

    void clear()
    {
        eTextMode = TEXT_MODE_FIRST_ROW;
        strFirstRowText = "";
        strSecRowText = "";
    }

};

struct DsskeyIconData
{
    yl::string                      strIconPath;        // DsskeyIcon的路径
    yl::string
    strDownIconPath;    // Dsskey按下状态Icon的路径-T48 反白效果使用
    yl::string                      strFlashIcon1;      // 闪烁Icon1的路径-T48-URL使用
    yl::string                      strFlashIcon2;      // 闪烁Icon2的路径-T48-URL使用
    DSS_KEY_FLASH_TYPE              eRefreshType;       // Dsskey闪烁类型(如快闪慢闪)
    Dsskey_Icon_Type                eIconType;          // 扩展台的ICON类型
    yl::string                      strPresenceIcon;    // 状态图标的路径

    DsskeyIconData()
    {
        clear();
    }

    void clear()
    {
        eRefreshType = REFRESH_TYPE_NO_FLASH;
        strIconPath = "";
        strDownIconPath = "";
        strFlashIcon1 = "";
        strFlashIcon2 = "";
        eIconType = DK_ICON_None;
        strPresenceIcon = "";
    }
};

struct DsskeyLightData
{
//  DSS_KEY_FLASH_TYPE              eRefreshType;       // Dsskey闪烁类型(如快闪慢闪)
    DSS_KEY_COLOR_TYPE              eLightColorType;    // dsskey灯颜色
    DSS_KEY_FLASH_TYPE              eLightFlashType;    // dsskey灯闪烁速度
    // Page状态 liuqy
    DSS_KEY_COLOR_TYPE              ePageColorType;     // 对应的page灯状态
    DSS_KEY_FLASH_TYPE              ePageLightFlashType;// 对应的page灯状态
    int                             iLightPriority;     // 对应灯优先级

    DsskeyLightData()
    {
        clear();
    }

    void clear()
    {
        eLightColorType = LED_COLOR_OFF;
        eLightFlashType = REFRESH_TYPE_NO_FLASH;
        iLightPriority = DSS_KEY_LED_PRIORITY_LOWEST;
        ePageColorType = LED_COLOR_OFF;
        ePageLightFlashType = REFRESH_TYPE_NO_FLASH;
    }
};

struct DsskeyBkgroundData
{
    DSS_KEY_GROUNDPHOTO_MODE        eGroundPhoto;       // Btn的底色模式

    DsskeyBkgroundData()
    {
        clear();
    }

    void clear()
    {
        eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;
    }
};

struct DsskeyDisplayData
{
    int                             iIndex;             // Dsskey对应的index
    DssKey_Type                     eDssKeytype;        // Dsskey的类型
    int                             iStatus;
    int                             iDsskeyID;          // DsskeyID包含了类型索引灯
    BOOL                            bNeedRefresh;       // 是否需要在UI上重新刷新该dsskey
    BOOL                            bLockKeyed;         // 被锁定的dsskey
    YLList<int>                     listAuxiliaryState;     // 辅助状态链表
    // Page状态 liuqy
    DSS_KEY_COLOR_TYPE              eLightColorType;    // dsskey灯颜色
    DSS_KEY_FLASH_TYPE              eLightFlashType;    // dsskey灯闪烁速度
    DSS_KEY_COLOR_TYPE              ePageColorType;     // 对应的page灯状态
    DSS_KEY_FLASH_TYPE              ePageLightFlashType;// 对应的page灯状态
    int                             iLightPriority;     // 对应灯优先级
    int                             iPageIconState;     // page icon状态,1 page icon长亮
    unsigned int                    iTimestamp;         // 闪烁定时时间停止
    int                             iFlashuration;      // 定时时间到停止闪灯

    DSSKEY_MODULE_TYPE              eType;
    DSS_KEY_GROUNDPHOTO_MODE        eGroundPhoto;       // Btn的底色

    yl::string                      strIconPath;        // DsskeyIcon的路径
    yl::string
    strDownIconPath;    // Dsskey按下状态Icon的路径-T48 反白效果使用
    yl::string                      strFlashIcon1;      // 闪烁Icon1的路径-T48-URL使用
    yl::string                      strFlashIcon2;      // 闪烁Icon2的路径-T48-URL使用
    DSS_KEY_FLASH_TYPE              eRefreshType;       // Dsskey闪烁类型(如快闪慢闪)
    Dsskey_Icon_Type                eIconType;          // 扩展台的ICON类型

    DSS_KEY_TEXT_MODE               eTextMode;          // 文字显示的模式,分单行还是多行
    yl::string                      strFirstRowText;    // 第一行显示的文字
    yl::string                      strSecRowText;      // 第二行显示的文字

    yl::string                      strPresenceIcon;    // 状态图标的路径



    DsskeyDisplayData()
    {
        clear();
    }

    void clear()
    {
        iIndex = -1;
        eDssKeytype = DT_NA;
        iStatus = DS_NON;
        iDsskeyID = -1;
        bNeedRefresh = FALSE;
        bLockKeyed = FALSE;

        eRefreshType = REFRESH_TYPE_NO_FLASH;
        strIconPath = "";
        strDownIconPath = "";
        strFlashIcon1 = "";
        strFlashIcon2 = "";
        eIconType = DK_ICON_None;


        eTextMode = TEXT_MODE_FIRST_ROW;
        strFirstRowText = "";
        strSecRowText = "";

        eGroundPhoto = GROUNDPHOTO_MODE_NORMAL;
#ifdef _T46
        eType = DMT_NON;
#endif
        iTimestamp = 0;
        iFlashuration = -1;
        iPageIconState = 0;

        eLightColorType = LED_COLOR_OFF;
        eLightFlashType = REFRESH_TYPE_NO_FLASH;
        iLightPriority = DSS_KEY_LED_PRIORITY_LOWEST;
        ePageColorType = LED_COLOR_OFF;
        ePageLightFlashType = REFRESH_TYPE_NO_FLASH;

    }

    ~DsskeyDisplayData() {}
};


typedef YLList<DsskeyDisplayData>   ListDisplayData;

typedef YLList<DssKeyLogicData>     ListLogicData;
typedef YLList<DssKeyLogicData *>    ListpLogicData;


#endif // DSSKEY_COMMON_DEFINE_H
