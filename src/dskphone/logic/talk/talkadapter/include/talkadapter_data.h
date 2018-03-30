#pragma once

#include "ylstring.h"
#include "ETLLib.hpp"
#include "talk/talklogic/include/uiandlogic_base.h"
#include "talk/talklogic/include/talklogic_common.h"
#include "talk/talklogic/include/uiandlogic_common.h"


// 更新界面的原因（UI据此更新不同区域）
enum UPDATE_REASON
{
    UPDATE_DEFAULT = 0, // 默认更新全屏
    UPDATE_MUTE,        // 更新mute相关区域
    UPDATE_HOLD,        // 更新hold相关区域
    UPDATE_ICON,
};


// logic发送给ui的数据
struct DataLogic2UI
{
    DataLogic2UI()
    {
        iSessionID = -1;
        eUpateReason = UPDATE_DEFAULT;
        eSessionState = SESSION_UNKNOWN;
        pData = NULL;
    }

    // Session标识
    int    iSessionID;
    // Session状态
    SESSION_STATE eSessionState;
    // 更新区域
    UPDATE_REASON eUpateReason;
    // 发送给小窗口的数据
    DataLogic2UIBase* pData;
};

// logic发送给ui的所有session数据链表
typedef YLList<DataLogic2UI> List_AllSession_Data;

class CCallInfo;
// 拨号过程发送给ui的数据(对应DataLogic2UI中的pData)
struct DataDial : public DataLogic2UIBase
{
    DataDial()
    {
        eDialUIType = DIAL_UI_DIALING;
        strNumber = "";
        pCallInfo = NULL;
        iKeyCode = PHONE_KEY_NONE;
        bEncrypt = false;
        listTranCallInfo.clear();
#if IF_FEATURE_DIAL_ON_HOLD
        bAllowTran = false;
#endif //
        bSeizeFailed = false;
    }

    //拨号界面类型
    DIAL_UI_TYPE eDialUIType;
    // 已拨号码
    yl::string       strNumber;
    // 通话信息
    CCallInfo*   pCallInfo;
    // 记录进入预拨号界面的按键键值
    int          iKeyCode;
    // 是否通话加密
    bool         bEncrypt;
    // transfer to穿梭的目标选择
    YLList<CCallInfo*> listTranCallInfo;
    // 会议的目标选择
    YLList<CCallInfo*> listConfCallInfo;

#if IF_FEATURE_DIAL_ON_HOLD
    bool        bAllowTran;
#endif //
    bool        bSeizeFailed;
};


// 来电振铃过程发送给ui的数据
struct DataRing : public DataLogic2UIBase
{
    DataRing()
        : pCallInfo(NULL), pDialData(NULL), pCCInfo(NULL)
    {
        bCallCenterInfoAvaliable = false;
        bShowCallCenterInfo = false;
        bShowSilence = false;
        bAllowJoin = false;
        bAllowFwd = false;
#ifdef IF_FEATURE_EMERGENCY
        bEmergencyCalled = false;
#endif
    }

    // 是否显示CallCenterInfo
    bool        bShowCallCenterInfo;
    // call info是否可用
    bool        bCallCenterInfoAvaliable;
    // 是否显示静音图标
    bool        bShowSilence;
    // call decline是否开启
    bool        bCallDeclineEnable;
    // 是否显示join按键
    bool        bAllowJoin;
    // 途径的中转者显示名
    yl::string  m_strViaName;
    // 途径的中转者号码
    yl::string  m_strViaNumber;
    // 途径的中转者完整信息
    yl::string  m_strViaFullContact;
    // 通话信息
    CCallInfo* pCallInfo;
    // 当处于forward呼出时
    DataDial*   pDialData;

    // 给UI的数据(Call Infomation)
    CallCenterInfo* pCCInfo;

    bool bAllowFwd;
#ifdef IF_FEATURE_EMERGENCY
    // 当前为紧急号码，并且回呼开关开启时
    bool bEmergencyCalled;
#endif
};

// 呼出回铃过程发送给ui的数据
struct DataConnect : public DataLogic2UIBase
{
    DataConnect()
    {
        bConnectByTran = false;
        pCallInfo = NULL;
        bEncrypt = false;
    }

    // 是否Tran之后的呼出
    bool       bConnectByTran;
    // 通话信息
    CCallInfo* pCallInfo;
    // 是否通话加密
    bool       bEncrypt;
};


// call Finished过程发送给ui的数据
struct DataCallFinished : public DataLogic2UIBase
{
    DataCallFinished()
    {
        bError = false;
        bInvalidAccount = false;
        strInfo = "";
        pCallInfo = NULL;
        bAllowNewCall = true;
        bAllowAutoRedial = false;
        bMaxSessionError = false;
    }

    // 是否显示错误提示(否则显示时长)
    bool       bError;
    // 是否非法账号
    bool       bInvalidAccount;
    // 是否最大话路错误
    bool       bMaxSessionError;
    // 提示信息(时长或错误原因)
    yl::string strInfo;
    // 通话信息
    CCallInfo* pCallInfo;
    // 是否允许显示new call的softkey
    bool       bAllowNewCall;
    // 是否允许自动重拨
    bool       bAllowAutoRedial;
    // 自动重拨数据
    AUTOREDIAL_DATA redialData;
};


// talking过程发送给ui的数据
struct DataTalking2UI : public DataLogic2UIBase
{
    DataTalking2UI()
        : pDialData(NULL)
    {
        eTalkUIType = TALKING_UI_NORMAL;
        bAllowTran = true;
        bAllowConf = true;
        bAllowSwap = false;
        bTranFailed = false;
        pCallInfo = NULL;
        bEncrypt = false;
        bCallParkFailed = false;
        strParkFiled = "";
        bOneSession = false;
        bAllowTranToAnother = false;
        bChannelLinked = true;
        bShowAoc = false;
        bCallPushFailed = false;
        bAllowNewCall = true;
        bAllowSecurity = false;
        bAllowHold = true;

        bAllowJoin = false;
        bAllowConfWithAnother = false;
        bInConfManager = false;
        strConfName = ""; //如果是旧服务器，UI要判断下为空则显示Conference
        listNetUserInfo.clear();
    }

    // 通话对应的UI类型
    TALKING_UI_TYPE eTalkUIType;
    // 是否允许tran
    bool       bAllowTran;
    // 是否允许conf
    bool       bAllowConf;
    // 是否允许swap
    bool       bAllowSwap;
    // 是否Tran失败
    bool       bTranFailed;
    // 是否callpark失败
    bool       bCallParkFailed;
    // callpark 失败提示
    yl::string   strParkFiled;
    // 通话信息
    CCallInfo* pCallInfo;
    // 是否通话加密
    bool       bEncrypt;
    // 当处于PreCallPark时
    DataDial*   pDialData;
    // 是否只有一路通话，用于New Call设置
    bool        bOneSession;
    // 是否允许穿梭
    bool        bAllowTranToAnother;
    // 声音通道是否在话机
    bool        bChannelLinked;
    // 是否显示Aoc
    bool        bShowAoc;
    bool        bCallPushFailed;
    bool        bAllowNewCall;
    bool        bAllowSecurity;
    // 是否允许hold
    bool        bAllowHold;

    bool        bAllowJoin;

    // 是否允许穿梭
    bool        bAllowConfWithAnother;

    // 是否confmanager中
    bool             bInConfManager;

    //网络会议名称
    yl::string strConfName;
    //网络会议成员列表
    YLList<DataNetConfUserInfo> listNetUserInfo;
};

// LocalConfSession发送给ui的数据
struct DataLocalConf : public DataLogic2UIBase
{
    DataLocalConf()
    {
        listCallInfo.clear();
        bEncrypt = false;
        bInConfManager = false;
        bAllowConf = true;
        bAllowNewCall = true;
        bAllowSplit = true;
        bAllowSecurity = false;
        bHoldConf = false;
    }

    // 加入会议的各话路CallInfo
    YLList<CCallInfo*> listCallInfo;
    // 是否通话加密
    bool             bEncrypt;
    // 是否confmanager中
    bool             bInConfManager;
    //是否允许会议
    bool bAllowConf;
    //是否允许new call
    bool bAllowNewCall;
    //是否允许split
    bool bAllowSplit;
    //是否显示安全等级
    bool bAllowSecurity;
    //是否会议被hold
    bool bHoldConf;
};
// AOC数据
struct DataAoc
{
    yl::string  strBill;            // 话费值
    yl::string  strMonetaryUnit;    // 货币单位
    yl::string  strSpendType;       // 花费类型:"total","subtotal"...
    yl::string  strBillingMode;     // 计费方式:"normal-charging"...
    int         iTimeDuration;      // 花费时间
};
