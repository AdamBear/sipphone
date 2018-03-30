#ifndef __CALL_INFO_H__
#define __CALL_INFO_H__

#include "account/include/modaccount.h"
#include "talk/ippaging/include/ippaging.h"
#if IF_FEATURE_EVENT_MODULE
#include "extended/event/include/event_def.h"
#else
#include "extended/actionuri_url/include/action_def.h"
#endif
#include "service_ipvp.h"
#include "sipstructmapfunc.h"
#include "uiandlogic_common.h"
#include "media/commondefine.h"
#include "voice/include/voicecommon.h"
#include "talk/talkhelp/include/talkhelp_data.h"

#include "talk/talkproxy/include/noanswerfwdasyncproxy.h"

// 通话对方信息，Talk模块使用
struct SipUserInfo
{
    yl::string sSIPName;         // 号码
    yl::string sDisplayName;     // 名称
    yl::string sServerName;      // 服务器地址
    yl::string sFullContactInfo; // 号码名称的组合信息
    yl::string sFromSIPName;     //From头域号码(username)
};

class CCallInfo;
class CSessionProxy;

struct CBaseInfoTraver
{
    virtual bool operator()(CCallInfo* p)
    {
        return false;
    }
};

struct CBaseSessionTraver
{
    virtual bool operator()(CSessionProxy& proxy)
    {
        return false;
    }
};

enum INF_RECORD_TYPE
{
    INF_RECORD_NONE    = 0,
    INF_RECORD_SIP     = 0x01,
    INF_RECORD_URL     = 0x01 << 1,
    INF_RECORD_VOICE   = 0x01 << 2,
    INF_RECORD_ALL     = 0xff,
};

enum ShareState
{
    SHARE_STATE_CLOSE,
    SHARE_STATE_OPENING,
    SHARE_STATE_OPENED,
    SHARE_STATE_RESTART_WAITING,    // 盲转时会被stop,盲转失败后recall要重新开启辅流，或者盲转前没有辅流，盲转失败recall时开启
    SHARE_STATE_CLOSE_WAITING,      // 忙转失败是会recall，如果这时关闭辅流，设置为该状态
};

enum PlayRingType
{
    PRT_PLAY_NONE,
    PRT_PLAY_LOCAL_RING,
    PRT_PLAY_COLOR_RING,
};

enum AUDIO_CODEC_TYPE
{
    ACT_NONE = 0,
    ACT_HD   = 0x01,
    ACT_AMR  = 0x01 << 1,
    ACT_OPUS = 0x01 << 2,
    ACT_ALL  = ACT_HD | ACT_AMR | ACT_OPUS,
};

enum
{
    TALK_CALL_INFO_TRNSFER,
    TALK_CALL_INFO_TRNS_FAILED,
    TALK_CALL_INFO_FWD,
    TALK_CALL_INFO_PARK,
    TALK_CALL_INFO_GPARK,
    TALK_CALL_INFO_CCINFO,
    TALK_CALL_INFO_RING_MUTE,
    TALK_CALL_CONF_MANAGER,
};

//VIDEO_REDUCED 表示已经降低的通道
#define VIDEO_REDUCED_NONE               0               //非Yealink不能调整Status
#define VIDEO_REDUCED_RECV               1
#define VIDEO_REDUCED_SEND               (1 << 1)
#define VIDEO_REDUCED_SHARE              (1 << 2)
#define VIDEO_REDUCED_RECORD             (1 << 3)

#define VIDEO_PENDING_RECV_SHARE         (1  << 18)  //辅流重协商后准备中
#define VIDEO_PENDING_SEND_SHARE         (1  << 19)  //辅流重协商后准备中
#define VIDEO_PENDING_RECORD             (1  << 20)  //录像准备

#define VIDEO_REDUCED_NEGOTIATION        (1 << 25)       //协商中及协商完成
#define VIDEO_REDUCED_NEGO_FAIL          (1 << 26)       //协商失败
#define VIDEO_INIT_NEGOTIATION           (1 << 31)       //初始重协商

// 转换sip传递的通话标志:Lync与sip中性定义要兼容
#define CALL_FEATURE_NONE                   0
#define CALL_FEATURE_BTOE_SWITCH            (1)         // 联动设备切换
#define CALL_TARGET_HAVE_DELEGATE           (1<<1)      // 呼叫对方有设置代理
#define CALL_TARGET_ANSWER_BY_DELEGATE      (1<<2)      // 呼叫对方有设置代理，代理接起
#define CALL_FEATURE_MEETING_CALL           (1<<3)      // 在meeting呼叫通话中

#define CALL_FEATURE_ICE_CALL               (1<<10)     // 通话采用ICE协商

class CSingleSession;
class DataDial;

#define CONFCALLNUM 12

// 基本通话信息
class CCallInfo : public DataLogic2UIBase
{
public:
    CCallInfo(CSingleSession* pSession, int nCallId = -1) DECLARE_ATTR;
    ~CCallInfo();

    // 刚呼出时对方的信息，在发生通话转移或CallReplace之前(在connect和talk记录)
    void SetOriginalRemoteInfo(bool bForced = false);
    void SetOriginalRemoteInfo(const SipUserInfo& userInfo);

    // 获取账号对应的BLA类型
    BLA_TYPE GetBLAType();

    // 获取账号对应的会议类型
    CONF_TYPE GetConfType();
    // 通话是否TLS加密
    bool IsTLSEncrypt();

    // 通话是否SRTP加密
    bool IsSRTPEncrypt();

    // 是否音频加密
    bool IsAudioSrtp();

    // 给对方发送Hold消息
    void SendHoldMsg(bool bHold = true);
    // 保存通话时长
    void SetCallDuration(int iSec);

    // 查找联系人信息 是否搜索名字
    // false：通过搜索、联系人、历史记录带名字呼出,不再搜索名字且取头像需要同时匹配号码与名字
    void SetContactInfo(bool bSearchDisp = true);

    // 是否HD通话
    bool IsHDTalking();

    AUDIO_CODEC_TYPE GetAudioCodecType();

    yl::string GetRemoteSipURL() const;

    // 是否音频通话
    bool IsAudioTalk();

#ifdef IF_BT_SUPPORT_PHONE
    //是否蓝牙话路
    bool IsBtTalk();
#endif // IF_BT_SUPPORT_PHONE

    // 对应录音类型是否开启
    inline bool IsRecordOn(size_t eType)
    {
        return (m_eRecordType & eType);
    }

    // 根据MissCallType，判断该次Misscall记录是否需要保存
    bool IsNeedSaveMissCall();


#if IF_FEATURE_PAGING
    // 是否是Multicast Page通话
    bool BeMulticastPage();
#endif

    void SetRelationInfo(CALL_RELATION eRelation = CR_NONE, const yl::string& strRelation = "");

public:
    inline int GetCallID() const
    {
        return m_iCallId;
    }

    inline int GetAccountID() const
    {
        return m_iSipAccount;
    }

    inline bool IsLocalHold() const
    {
        return m_bLocalHold;
    }

    inline bool IsRemoteHold() const
    {
        return m_bRemoteHold;
    }

    inline bool IsFarMute() const
    {
        return m_bFarMute;
    }

    inline bool IsFarHold() const
    {
        return m_bFarHold;
    }

    inline bool IsTalkStart() const
    {
        return m_bStartTalk;
    }

    // 获取通话线路类型
    inline CALL_LINE_TYPE GetCallLineType() const
    {
        return m_eCallLineType;
    }

    inline const SipUserInfo& GetRemoteInfo() const
    {
        return m_tRemoteInfo;
    }

    inline ipvp_speech_t& GetIPVPParam()
    {
        return m_tIPVPParam;
    }

    // falg标志
    inline bool IsCallMask(unsigned mask) const
    {
        return m_sipChangnels.call_flag & CALL_FEATURE_ICE_CALL;
    }

    inline const yl::string& GetDialText() const
    {
        return m_strDialedNumber;
    }

    inline bool IsRingMute() const
    {
        return m_bRingMute;
    }

    inline bool IsPreForword() const
    {
        return m_bInForward;
    }

    inline bool IsTransferred() const
    {
        return m_bTransferred;
    }

    inline bool IsConfManager() const
    {
        return m_bInConfManager;
    }

    ROUTINE_STATE GetRoutine() const;

    SESSION_STATE GetSession() const;

    int GetSessionID() const;

    bool IsInherits(ROUTINE_STATE eRoutine) const;

    void SetCallID(int iCallID);

    void SetAccountID(int iAccountID);

    void SetHold(bool bHold);

    void SetHeld(bool bHeld);

    void SetFarMute(bool bHold);

    void SetFarHold(bool bMute);

    bool SetIPVPParam(const ipvp_speech_t& pm);

    void SetTalkStart(bool bTalk);

    // 设置对方的信息(参数为不带扩展数据的sip info)
    void SetLocalInfo(const BasicUserInfo& userInfo);
    void SetRemoteInfo(const BasicUserInfo& userInfo);

    // 设置通话线路类型
    void SetCallLineType(CALL_LINE_TYPE eType);

    // 设置来电对方头像的Url
    void SetCallerImageURL(const yl::string& strURL);

    bool SetSipChannels(const sip_channels_v20_t& channels);

    void SetDialedNumber(const yl::string& strText);
    void ResetDialedNumber();

    void SetRoutineState(ROUTINE_STATE eRoutine);

    void SetTransfer(bool bTrans);
    void SetFWD(bool bFWD);
    void SetRingMute(bool bMute);

    void SetConfManager(bool bOn);

public:
    void CheckHoldStatus();

    CallCenterInfo& GetCCInfo();

    DataDial& GetDialData();

    CallOut_Param& GetCallOutParam();

    CallInUserInfo& GetRingUserInfo();

private:
    void SendInfoSig(int iMask, bool bFlag);

private:

    int             m_iCallId;             // 通话ID
    int             m_iSipAccount;         // 通话所使用的帐号ID
    bool            m_bLocalHold;          // 本方hold状态
    bool            m_bRemoteHold;         // 对方hold状态
    bool            m_bFarHold;            // 远端Hold
    bool            m_bFarMute;            // 远端静音
    bool            m_bStartTalk;          // 是否开启了通话
    CALL_LINE_TYPE  m_eCallLineType;       // 线路类型（SIP、PSTN）

    bool            m_bTransferred;        // 是否等待转接结果
    bool            m_bInForward;          // 是否处在来电FWD拨号状态
    bool            m_bRingMute;           // 单次来电静音
    CallCenterInfo* m_pCCInfo;              // cc info
    DataDial*       m_pDialData;
    CallOut_Param*  m_pCallOutParam;
    CallInUserInfo* m_pRingUserInfo;
    CSingleSession* m_pSession;            // session指针
    bool            m_bInConfManager;      // conf Manager

    yl::string      m_strDialedNumber;     // 已经输入的字符（多Routine复用）

    ipvp_speech_t   m_tIPVPParam;          // 音频参数
    sip_channels_v20_t  m_sipChangnels;        // sip

public:
    TALK_STATE      m_eTalkState;           // 通话的状态
    int             m_iUnregSipAccount;     // 当以无账呼出时,保存此账号的指针
    SipUserInfo     m_tRemoteInfo;          // 通话对方的信息
    SipUserInfo     m_tOriginalRemoteInfo;  // 刚呼出时对方的信息
    SipUserInfo     m_tLocalInfo;           // 通话本方的信息
    ROUTINE_STATE   m_eStateOfRoutine;      // routine状态
    bool            m_bAutoAnswerNow;       // 马上自动接起标志
    bool            m_bNeedBindDsskey;      // 是否绑定Dsskey
    const ipvp_speech_t&   m_tVPMParam;     // 通话的VPM参数
    int             m_iConfCallID[CONFCALLNUM];
    bool            m_bJoinConf;            // 是否加入会议(不影响其他已经在会议的通话)

public:

    CALL_TYPE       m_eCallType;        // 类型（呼入、呼出）
    bool            m_bIsColorRingback; // 是否在响彩铃
    int             m_iCallDuration;    // 通话时长(秒)，用于保存到呼叫记录中
    size_t          m_eRecordType;      // 录音类型
    URL_RECORD_ERROR m_eURLRecordError; // URL录音错误类型
    int             m_iBLAIndex;        // 话路对应的BLAIndex
    bool            m_bBLASeizeLine;    // BLA占线的状态
    bool            m_bNeedToSeizeLine; // 该话路是否需要去占线
    bool            m_bSilentAnswer;    // 是否Silence应答，且对方挂机不响忙音
    bool            m_bIsIntercom;      // 该路通话是否是Intercom来电
    bool            m_bAddToNetworkConf;// 是否要加入网络会议的话路
    bool            m_bConferenceDirect;// 对方接起来后直接进入会议
    bool            m_bTransferAfterRingBack;
#if IF_FEATURE_PAGING
    // paging params
    PAGING_PARAM*   m_pPageParam;       // 组播参数
    STOPPAGE        m_eStopPageType;    // 停止Paging呼入的方式
#endif

    TALK_CALLOUT_TYPE   m_eTalkCallOutType;     // 呼出类型，如是否为Pickup呼出
    bool            m_bFinishedError;   // 是否呼出失败
    yl::string      m_strNumberBeforeEncrypt; // 密码拨号：加密前的号码
    bool            m_bPlay182Tone;     // 确认是否播放182回铃音,BugID=12701
    yl::string      m_strAutoDTMF;      // 进入通话界面后需要预先输入的DTMF
    yl::string      m_strRemotePhoto;   // 通话对方的大头贴
    yl::string      m_strRemoteImageURL;    // 通话对方的头像URL
    bool            m_bMissCallInTalk;  //是否是通话中产生的Misscall
    int             m_iTransferId;      //转出CallId
    yl::string      m_strFeatureAccessCodes; //拨出去的特征码

    yl::string      m_strConfId;

    bool            m_bIsSingleExitCof;
    bool            m_bIsInConf;
    yl::string      m_strForwardTarget;
#if IF_BUG_27033 || IF_BUG_30965
    DIAL_DSSKEY_TYPE m_eDialDsskeyType; // 拨号的Dsskey类型
#endif

#if IF_BUG_22457_Record
    SIP_RECORD_STATUS   m_eSIPRecordStatus;
#endif

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    CALL_RECORD_STATUE m_eRecordStatus; //录音状态
    CALL_RECORD_ACTION_RESULT m_eRecordResult;  //录音操作
#endif
#if IF_FEATURE_DIAL_ON_HOLD
    bool            m_bDialOnHold;
#endif
#if IF_BUG_30961
    bool            m_bTranByBLF;
#endif

#if IF_BUG_27560
    bool            m_bHideInfo;             //隐藏呼出或呼入号码标志
#endif

#if IF_BUG_31481
    yl::string      m_strCallTextInfo; // 存储sip发送过来的text info内容
#endif
    // todo test
#if IF_BUG_35381
    SipUserInfo     m_tRealRemoteInfoByAnonymous; //anonymous来电（Privacy ID）真实的信息
#endif //

    bool            m_bConfWithOnePress; //是否呼出直接加入会议

///////////////////////////////视频////////////////////////////////////////
    VideoExtInfo    m_stVideoExt;       // 视频扩展参数
    MediaStreamInfo m_stMediaInfo;      // 保存可用的视频codec参数


    bool            m_bIsAutoAnswerCall; //是否为自动应答


    bool            m_bHoldAvailable;       // hold是否可用

    yl::string      m_strUserAgent;         // 对方终端

    yl::string      m_strDiversionNumber;


//////////////////////////辅流/////////////////////////////////
    SipReinviteInfo m_tSipReinviteInfo;

    CALL_RELATION   m_eCallRelation;
    yl::string      m_strRelationNum;

#if IF_FEATURE_LOOPBACK
    bool            m_bIsLoopBackCall;
#endif

    //网络会议的conf_uri
    yl::string      m_strNetConfUri;


    /**********************************routine*********************************/
    /* routine 转移出来的数据 */
    /* ring */
    bool            m_bAnswered;
    //
    bool            m_bPassiveRingMute;

    /* dial */
    int             m_iDigitKey;

    /* finish */
    TalkErrorInfo   m_errorInfo;

    /* ParkNotify */
    yl::string      m_strNumber;
    yl::string      m_strDiaplayName;
};

#endif // __CALL_INFO_H__
