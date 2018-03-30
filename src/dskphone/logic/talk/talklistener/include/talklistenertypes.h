#pragma once

#include "ylstring.h"
#include "talksignaliddefine.h"
#include "talk/talkproxy/include/scopedelapse.h"
#include "talk/talklogic/include/talklogic_common.h"
#include "talk/talklogic/include/uiandlogic_common.h"
#include "talk/talkproxy/include/scaasyncproxy.h"
#include "talk/talkproxy/include/noanswerfwdasyncproxy.h"
#include "talk/talkproxy/include/recordasyncproxy.h"
#include "talk/talkproxy/include/autoanswerasyncproxy.h"
#include "talk/talkproxy/include/autorefuseasyncproxy.h"
#include <talk/talkproxy/include/scopedtimerex.h>
#include "voice/include/voicecommon.h"
#include "talk/talkasync/include/talkasync_data.h"
class CCallInfo;
class CBaseSession;
class CSessionProxy;

// 考虑用SesssionProxy替代SesssionID
struct SignalParams
{
    typedef union
    {
        int                         iValue;
        bool                        bValue;
        size_t                      uValue;
        float                       fValue;
        const void*                 pValue;
    } SIG_PARAM;

    int                         iSessionID;
    int                         iCallID;
    SIG_PARAM                   wParam;
    SIG_PARAM                   lParam;
    SIG_PARAM                   tParam;
    mutable TALK_SIGNAL_TYPE    eSignal;
    mutable size_t              uReply;
    mutable yl::string          strData;

    SignalParams(int iSessionID = INVALID_SESSION_ID, const yl::string& str = yl::string());

    SignalParams(const CBaseSession* p, const yl::string& str = yl::string());

    SignalParams(const CCallInfo* p, const yl::string& str = yl::string());
};

// 语音通道信息结构体
struct VoiceInfo
{
    // Hold/Held提示音类型
    enum HOLD_ALERT_TYPE
    {
        HAT_NO_ALERT, // 无提示音
        HAT_HOLD_ALERT, // Hold提示音
        HAT_HELD_ALERT // Held提示音
    };


    bool                 m_bNeedProcessRtpMsg;    //接收到是否出了VPM的RTP消息
    PlayRingType         m_ePlayRingType;
    yl::string           m_strRingPath;      // 铃声的路径
    int                  m_iRingStatus;      // 是否正处于加载铃声
    /*部分通话流程的定时器*/
    CScopedTimerEx       timerAutoRedial;     // Autoredial
    TONE_TYPE            eCallWaitingTone;    // 保存来电等待提示音类型
    CScopedTimerEx       timerRing;           // 来电等待提示音定时器
    HOLD_ALERT_TYPE      eHoldAlertType;      // 当前播放Hold/Held提示音类型
    CScopedTimerEx       timerHoldAlert;      // Hold/Held提示音定时器
    CScopedTimerEx       timerMadiaSniffer;   // madiasniffer // Ringback定时器
    TONE_TYPE            m_eToneType;         // 保存finsh提示音类型
    CScopedTimerEx       timerBusyTone;       // finish Busy提示音定时器
    CScopedTimerEx       timerPlayMsgTone;    // play Msg
    CScopedTimerEx       timerDelayPlayTone;  // delayPlayTone
    CScopedTimerEx       timerToneDelay;      //sca失败忙音 dialroutine 忙音
    CScopedTimerEx       timerIntercomTone;  //intercomTone
    CScopedTimerEx       timerParkNotify;  // 播放ParkNotify提示音定时器

    // http://10.2.1.199/Bug.php?BugID=34704
    bool                 m_bCallParkFailedAlert;
    // 播放提示音重复次数
    int                  m_iTranFailedToneTimes;
    CScopedTimerEx       timerTranFailedAudioAlert; // 转移失败提示音定时器
    VoiceInfo():
        eHoldAlertType(HAT_NO_ALERT)
        , eCallWaitingTone(TONE_CALLWAIT)
    {
        m_strRingPath.clear();
        m_iRingStatus = TALK_RING_LS_UNKOWN;
        m_bNeedProcessRtpMsg = true;
    }
};

struct CListenInfo
{
    explicit CListenInfo(int iSessionID);

    bool        bLastHold;                // 上一次的Hold状态
    bool        bSeizeFailed;             // 是否占线失败
    bool        m_bProcessSeizeLine;      //
    bool        m_bBLASeizeLineSucced;    // SCA是否占线成功
    int         iBindSubSession;          // 绑定的子话路ID
    int         iLastFocusSession;        // 上一路焦点
    bool        m_bSaveCallLog;           // 是否保存历史记录
    bool        m_bReadySendedS2SDmtf;
    bool        m_bEstablish;             // 是否建立通话:诊断平台qoe功能使用,目前使用lync的代码，通话解耦之后，去掉此部分


    CHANNEL_MODE    m_eAAChannel;      // 自动应答所用通道
    bool        m_bDPickupDial;        // 是否为DPickup的呼出
#if IF_BUG_27033 || IF_BUG_30965
    bool        m_bPagingDial;         // 是否为Paging的呼出
    bool        m_bIntercomDial;       // 是否为Intercom的呼出
    bool        m_bUnParkDial;         // 是否为UNPark的呼出
#endif
    bool        m_bEnterBLARoutine;    // 标识是否要进入BLAconnect routine
    bool        m_bCallParkFailed;     // 是否callpark失败
    bool        m_bCallPushFailed;     // 是否callpush失败
    yl::string  m_strParkFiled;        // callpark 失败提示

    bool        m_bInPreCallPark;      // 是否处在callpark拨号状态
    bool        m_bInPreGroupCallPark; // 是否处在groupcallpark拨号状态
    bool        m_bShowCallInfo;       // 是否显示的CCInfo界面
    bool        m_bTransferFailed;     // trans失败
    time_t      m_tBornTime;           // 呼叫建立时间
    time_t      m_tEstblishTime;       // 通话建立时间

    void SetPark(bool bPark);
    void ReSetPark();
    void SetGPark(bool bGPark);
    void ReSetGPark();
    void SetCCInfo(bool bShow);
    void SetTransFailed(bool bFailed);
    inline bool IsPrePark() const
    {
        return m_bInPreCallPark;
    }
    inline bool IsPreGPark() const
    {
        return m_bInPreGroupCallPark;
    }
    inline bool IsCCShow() const
    {
        return m_bShowCallInfo;
    }
    inline bool IsTransFailed() const
    {
        return m_bTransferFailed;
    }


    //会议名称
    yl::string                  m_strConfName;
    //与会者列表
    YLList<DataNetConfUserInfo> m_listNetUserInfo;

    CSCAASyncProxy              m_BlaProxy;
    CNoAnswerFWDASyncProxy      m_NoAnwFwdProxy;
    CRecordASyncProxy           m_RecordProxy;
    CAutoAnswerASyncProxy       m_AutoAnserProxy;
    CAutoRefuseASyncProxy       m_AutoRefuseProxy;
    CScopedTimerEx              m_TimerS2S;

    VoiceInfo                   m_VoiceInfo;// 语音信箱

private:
    int m_iSessionID;
    void SendInfoSig(int iMask, bool bFlag);
};

ListenerName GetSignalName(int type);

ListenerName GetETLMSGName(int iIndex);
