#ifndef __EDK_DEF_H__
#define __EDK_DEF_H__


#include <ylstl/yllist.h>
#include <ylstl/ylstring.h>
#include <ylstl/ylhashmap.h>
#include "interfacedefine.h"
#include <ETLLib.hpp>
#include <dsklog/log.h>
#include "dsskey/include/dsskey_enumtypes.h"

enum EDKSOFTKEYTYPE
{
    EDK_SOFTKEY_NON = -1,
#if IF_FEATURE_EDK
    EDK_SOFTKEY_1 = ST_EDK_1,
    EDK_SOFTKEY_2 = ST_EDK_2,
    EDK_SOFTKEY_3 = ST_EDK_3,
    EDK_SOFTKEY_4 = ST_EDK_4,
    EDK_SOFTKEY_5 = ST_EDK_5,
    EDK_SOFTKEY_6 = ST_EDK_6,
    EDK_SOFTKEY_7 = ST_EDK_7,
    EDK_SOFTKEY_8 = ST_EDK_8,
    EDK_SOFTKEY_9 = ST_EDK_9,
    EDK_SOFTKEY_10 = ST_EDK_10,
#endif
};

enum EDKACTIONTYPE
{
    EDK_TYPE_NON = -1,
    EDK_DIGIT,
    EDK_INVITE,
    EDK_REFER,
    EDK_DTMF,
    EDK_POPUP,
    EDK_WC,
    EDK_PAUSE,
    EDK_HOLD,
    EDK_HANG,
    EDK_URL,
    EDK_FUNCTION,
    EDK_SOFTKEY,
    EDK_ENTER_MENU,
    EDK_HARDKEY,
    EDK_LABEL,
    EDK_LED_CONTROL,
    EDK_INTERCOM
};

enum EDKACTIONSTATUS
{
    EDK_STATUS_NON = -1,
    EDK_EXCUTING,
    EDK_FINISH,
};

enum EDKUITYPE
{
    EDK_UI_NON = -1,
    EDK_IDLE = 1,
    EDK_INCOMING_CALL,
    EDK_CONNECTING,
    EDK_TRANSFER_CONNECTING,
    EDK_ON_TALK,
    EDK_CALL_FAILED,
    EDK_RING_BACK,
    EDK_TRANFER_RING_BACK,
    EDK_TALK_HOLD,
    EDK_TALK_HELD,
    EDK_CONFERENCED,
    EDK_DIALTONE,
    EDK_DIALING,
};

// 执行类型
enum EDKEXCUTETYPE
{
    EDK_EXCUTENON = -1,
    EDK_EXCUTEPROMPT,
    EDK_EXCUTETALK,
    EDK_EXCUTETIMER,
    EDK_EXCUTECOMMON,
    EDK_EXCUTEENTERMENU,
    EDK_EXCUTEKEY,
    EDK_EXCUTELED,
};

// 弹出框默认输入法
enum EDKINPUTTYPE
{
    EDK_INPUT_NON = -1,
    EDK_INPUT_NUMERIC,      // 默认输入法为123
    EDK_INPUT_TEXT,         // 默认输入法为abc
};

// 触发类型
enum EDKTRIGGERTYPE
{
    EDK_TRIGGER = 0,
    BLF_TRIGGER,
    EXP_FLIP_TRIGGER,
};

// 执行命令数据结构
struct edk_action_data_t
{
    edk_action_data_t()
    {
        clear();
    }

    EDKACTIONTYPE m_eActionType;        // 命令
    int m_iDsskeyId;                    // dsskey的id
    yl::string m_strAction;             // 携带数据

    void clear()
    {
        m_eActionType = EDK_TYPE_NON;
        m_strAction.clear();
        m_iDsskeyId = -1;
    }

    bool operator==(const edk_action_data_t & data) const
    {
        if (this != &data)
        {
            if (m_eActionType == data.m_eActionType
                    && m_strAction == data.m_strAction
                    && m_iDsskeyId == data.m_iDsskeyId)
            {
                return true;
            }

            return false;
        }
        return true;
    }
};
typedef YLList<edk_action_data_t> listEdkAction;
typedef YLList<edk_action_data_t>::ListIterator LISTEDKACTIONITER;

// 预处理命令数据结构
struct edk_preaction_data_t
{
    edk_preaction_data_t()
    {
        clear();
    }

    int m_iIndex;
    edk_action_data_t m_sActionData;

    bool operator==(const edk_preaction_data_t & data) const
    {
        if (this != &data)
        {
            if (m_sActionData == data.m_sActionData
                    && m_iIndex == data.m_iIndex)
            {
                return true;
            }

            return false;
        }
        return true;
    }

    void clear()
    {
        m_iIndex = -1;
    }
};
typedef YLList<edk_preaction_data_t> listEdkPreAction;
typedef YLList<edk_preaction_data_t>::ListIterator LISTEDKPREACTIONITER;

// 宏类型数据结构体
struct edk_macro_data_t
{
    edk_macro_data_t()
    {
        clear();
    }

    bool m_bEnable;
    int m_iIndex;
    yl::string m_strAction;
    yl::string m_strName;

    void clear()
    {
        m_bEnable = false;
        m_iIndex = -1;
        m_strAction.clear();
        m_strName = "edk_macro";
    }
};
typedef YLList<edk_macro_data_t> listEdkMacro;
typedef YLList<edk_macro_data_t>::ListIterator LISTEDKMACROITER;

// 弹窗类型数据结构体
struct edk_prompt_data_t
{
    edk_prompt_data_t()
    {
        clear();
    }

    bool m_bEnable;
    bool m_bMasked;
    int m_iIndex;
    int m_iMaxInput;
    EDKINPUTTYPE m_eInputType;
    yl::string m_strLabel;
    yl::string m_strTitle;

    void clear()
    {
        m_bEnable = false;
        m_bMasked = false;
        m_iIndex = -1;
        m_iMaxInput = -1;
        m_eInputType = EDK_INPUT_TEXT;
        m_strLabel.clear();
        m_strTitle = TRID_EDK_PROMPT;
    }
};
typedef YLList<edk_prompt_data_t> listEdkPrompt;
typedef YLList<edk_prompt_data_t>::ListIterator LISTEDKPROMPTITER;

// SoftKey类型数据结构体
struct edk_softkey_data_t
{
    edk_softkey_data_t()
    {
        clear();
    }

    bool m_bEnable;
    EDKSOFTKEYTYPE m_eType;
    int m_iKeyPos;
    yl::string m_strAction;
    yl::string m_strLabel;

    yl::string m_strSoftkeyID;

public:
    void clear()
    {
        m_bEnable = false;
        m_iKeyPos = 0;
        m_eType = EDK_SOFTKEY_NON;
        m_strAction.clear();
        m_strLabel.clear();
        m_strSoftkeyID.clear();
    }
};

struct SoftKey_Data
{
    int iPos;
    EDKSOFTKEYTYPE eType;
};

// 命令结构体
struct EdkActionInfo
{
    int m_iActionId;
    listEdkPreAction m_listPreAction;
    listEdkAction m_listAction;
    int m_iExcuteActionIndex;       // Action   链表当前执行的位置

    void clear()
    {
        m_iActionId = -1;
        m_listAction.clear();
        m_listPreAction.clear();
        m_iExcuteActionIndex = 1;
    }

    EdkActionInfo()
    {
        clear();
    }
};

// 灯控制数据
enum COLOR_TYPE
{
    COLOR_NONE = 0,
    COLOR_RED,
    COLOR_GREEN,
    COLOR_ORANGE,
    COLOR_RED_PIC_IN,
    COLOR_RED_PIC_OUT,
    COLOR_GREEN_PIC_IN,
    COLOR_GREEN_PIC_OUT,
    COLOR_ORANGE_PIC_OUT,
    COLOR_ORANGE_PIC_IN,
    COLOR_LOOP, // 循环亮灯
};

// 背景类型
enum PHONE_BACKGROUND_TYPE
{
    PHONE_BACKGROUND_NONE = -1,
    PHONE_BACKGROUND_RED,
    PHONE_BACKGROUND_GREEN,
};

struct LedRuleData
{
    COLOR_TYPE m_eColorType;
    int m_iDurTime;
    bool m_bFlashed;

    LedRuleData()
    {
        m_eColorType = COLOR_NONE;
        m_iDurTime = -1;
        m_bFlashed = false;
    }

    void ReSet()
    {
        m_bFlashed = false;
    }
};

struct LedActionData
{
    int m_iActionId;
    int m_iDsskeyId;
    yl::string m_strLabel;
    YLList<LedRuleData> m_listLedRule;
    bool m_bTimerRunning;
    int m_iTimer;
    DsskeyExpRefresh m_eFlushType;

    void clear()
    {
        m_iActionId = -1;
        m_iDsskeyId = -1;
        m_strLabel = "";
        m_bTimerRunning = false;
        m_iTimer = -1;
        m_eFlushType = DSS_KEY_REFRESH_ALL;
    }

    void ReSet()
    {
        m_iTimer = -1;
        for (YLList<LedRuleData>::ListIterator iter = m_listLedRule.begin();
                iter != m_listLedRule.end(); ++iter)
        {
            (*iter).ReSet();
        }
    }

    LedActionData()
    {
        clear();
    }
};

typedef YLList<LedRuleData>::ListIterator LIST_LED_RULE_TIER;

typedef YLList<edk_softkey_data_t> listEdkSoftKey;
typedef YLList<edk_softkey_data_t>::ListIterator LISTEDKSOFTKEYITER;
typedef YLHashMap<EDKUITYPE, listEdkSoftKey> mapEdkSoftKey;
typedef YLHashMap<EDKUITYPE, listEdkSoftKey>::iterator MAPSOFTKEYITER;


// 彩屏换图标数据
struct EdkCtrlDsskeyDispData
{
    COLOR_TYPE      m_eColorType;           // 小图标类型
    PHONE_BACKGROUND_TYPE m_eBackGroundType;        // 背景类型
    yl::string      m_strLable;             // label显示名称
};
#endif // __EDK_DEF_H__
