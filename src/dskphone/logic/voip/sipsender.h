#ifndef __SIP_SENDER_H__
#define __SIP_SENDER_H__

#include "singletonclassdefine.h"
#include "service_sip_define.h"
#include "media/commondefine.h"

struct CallOutInfo;



class CSIPSender
{
    DECLARE_SINGLETON_CLASS(CSIPSender)
public:
    //  查询
    int QueryLineState(int iAccountID);
    // 接起来电，eType代表接起方式
    BOOL AcceptCall(int iAccountID, int iCallId, int iType, bool bSupportH264, bool bMainStreamLimit);
    // 拒接来电
    BOOL RefuseCall(int iAccountID, int iCallId, int iRefuseCode);
    // 邀请通话目标，lpszFullName代表呼叫目标
    BOOL CallInvite(CallOutInfo * pCallInfo, bool bSupportH264);
    // 发送code使用
    BOOL CallInvite(int iAccountID, int iCallId, LPCSTR lpszFullName);
    // 通话挂断
    BOOL CallHangUp(int iAccountID, int iCallId, int lParam = 0);
    // 设置通话Hold，bHold代表是否Hold对方，bEnableQCIF为Video通话是否采用QCIF进行协商
    BOOL SetHold(int iAccountID, int iCallId, BOOL bHold);
    // 请求呼叫转移，lpszForwardNumber代表转移目标
    BOOL RequestForward(int iAccountID, int iCallId, LPCSTR lpszForwardNumber, int iForwardType = 0);
    // 请求呼叫转接，iTraneeId为被转方的CallId，iTranerId为转移方的CallId，lpszTranTarget代表转接目标
    BOOL RequestTransfer(int iAccountID, int iTraneeId, int iTranerId, LPCSTR lpszTranTarget);
    // 更新PBX button
    BOOL UpdatePBXButton(int iAccountID, LPCSTR lpszNotify);
    // ACD响应
    BOOL ACDAction(int iAccountID, int iKeyIndex, int iType, const char * pUser = NULL,
                   const char * pPwd = NULL);
    // ATS添加删除参与方
    BOOL ModifyATSPartner(int iAccountID, BOOL bAdd, int iCallID, LPCSTR lpszNumber);
    // BLA hold
    BOOL BLAHold(int iAccountID, BOOL bPublic, int iCallId, BOOL bHold, int iBLAIndex);
    // DRAFT BLA03 hold
    BOOL DraftBLAHold(int iAccountID, BOOL bPublic, int iCallId, BOOL bHold);
    // 建立网络会议
    BOOL EstablishNetworkConf(int iAccountID, int iCallID, int iCount, int * pConfCallIDList,
                              int iMask = 0);
    // 建立SCA网络会议
    BOOL EstablishSCANetworkConf(int iAccountID, int iCallID, int iCount, int * pConfCallIDList,
                                 int iBlaIndex, int iMask = 0);
    // 网络会议加人
    BOOL AddToNetworkConf(int iAccountID, int iSrcCallID, int iAddCallID);
    // 开始或关闭URLRecord
    BOOL StartURLRecord(int iAccountID, int iCallID, BOOL bStart);
#if IF_BUG_22457_Record
    // 开始或关闭FortinetRecord
    BOOL StartFortinetRecord(int iAccountID, int iCallID, BOOL bStart);
#endif //IF_BUG_22457_Record
    // 订阅与取消订阅CC
    BOOL SubscribCallCompletion(int iAccountID, int iCallID, BOOL bSubscribe, const char * pNumber);
    // 功能键同步
    BOOL FeatureKeySync(int iAccountID, int iTargetAccountID, LPCSTR lpszData);
    // 给Sip发送DTMF按键消息,iKey为按键键值
    BOOL SendDTMF(int iAccountID, int iCallID, int iKey);
    // 发送OEM FLASH
    BOOL SendOEMFlash(int iAccoutID, int iCallID);
    // BLA占线与释放占线
    BOOL BLASeizeLine(int iAccountID, int iCallID, BOOL bSeize, int iIndex);
    // DraftBLA占线与释放占线
    BOOL DraftBLASeizeLine(int iAccountID, int iCallID, BOOL bSeize, int iIndex);
    // 发送消息
    BOOL SendTextMessage(int iAccountID, const char * pText, const char * pTo);

    //BOOL SendCSTAInfoReq(int iAccountID, int iCallID, LPCSTR lpszData);

    BOOL SendAnsRequest(int iAccountID, int iCallID, LPCSTR lpszData);

    BOOL SendCSTAAccptCall(int iAccountID, int iCallID, LPCSTR lpszData);

    BOOL SendCSTARequest(int iAccountID, int iCallID, LPCSTR lpszData);

    BOOL SendCSTAResponse(int iAccountID, int iCallID, int iTid, LPCSTR lpszData);

#if IF_FEATURE_CTRIP_CTI
    BOOL SendCtripHWCTIRequest(int iAccountID, int iCallID, sip_uc_req_send_info_t * info);

    BOOL SendCtripHWCTIResponse(int iAccountID, int iCallID, int iTid, int iCode);

    BOOL SendCtripFSCTIResponse(int iAccountID, int iCallID, int iTid, int iCode);
#endif //IF_FEATURE_CTRIP_CTI

    BOOL SendReportVQSession(int iAccountID, int iCallID, LPCSTR lpszData);

    BOOL GetCallInfo(int iAccountID, int iCallID, sip_call_proto_info2_t * lpszData);

    // 来电时响应SIP
    BOOL ResponseIncoming(int iAccountID, int iCallID, int iResponseCode);

    // 向sip获取call id
    int RequestCallID();

    // 请求对方发送关键帧
    bool ReqKeyframe(int iAccountID, int iCallID);

    // 初始化sip
    BOOL InitSIP();

    bool IsSupportVideo();

    // 启动并配置sip
    int StartSIP();

    //设置参数
    bool SetSipOption(int option, void * value, unsigned int size);

#ifdef IF_SUPPORT_VIDEO
    bool CallNegoH264(int iAccountID, int iCallId, const VideoSizeInfo & videoSize,
                      const VideoSizeInfo & extvideoSize, int iType, bool bNegoNow);

	bool SetVideoCallMedia(int iAccountID, int iCallID, bool bVideo);

	// 音视频切换
	bool VideoSharing(int iAccountID, int iCallID, bool bShare);


	bool CallNegoResume(int iAccountID, int iCallID);
#endif //#ifdef IF_SUPPORT_VIDEO

    int StopSip();

    bool SrtpLifetimeReinvite(int iAccountID, int iCallID);

    // 处理定时器
    BOOL OnTimer(UINT uTimeID);

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    //Hoteling状态改变
    BOOL ReqHoteling(int iAccountID, int iStatus, const char * pUser, const char * pPwd);
    //ACD Trace,DispCode,EmergencyEscalation 功能消息
    BOOL SendAcdInfoReq(int iAccountID, int iCallID, LPCSTR lpszData);
    //订阅QueueStatus消息，或取消订阅
    BOOL ReqQueueStatus(int iAccountID, bool IsOn);
#endif //IF_FEATURE_BROADSOFT_CALL_CENTER

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
    bool BwCallRecord(int iAccountID, int iCallID, int option);
#endif

    yl::string GetUserAgent(int iAccountID, int iCallId);


    bool ReInviteRespose(int iAccountID, int iCallID, int iTID, bool bAccept);

#ifdef IF_FEATURE_EMERGENCY
    //紧急话路被dut挂断,隐藏时
    int  EmergencyCallHangup(int iAccountID, int iCallId);
#endif
#if IF_FEATURE_GENBAND_E911
    bool SetLocationID(int iAccountID, int nLocationID);
#endif

    bool DeleteNetConfParty(int iAccountID, int iCallID, const char * pUserId);

private:
    // 获取延迟初始化sip的时间
    int GetInitSIPTime();

#ifdef IF_SUPPORT_VIDEO
    bool SetVideoH264Param(int iAccountID, int iCallId, const VideoSizeInfo & videoSize,
                           const VideoSizeInfo & extvideoSize, int iType);
#endif //#ifdef IF_SUPPORT_VIDEO

    yl::string GetVersion(const char * pKey);


private:
    // 延迟启动sip service的定时器
    int m_iStartServiceTimer;
    // 等待autop更新完成
    int m_iWaitAutopTimer;
};

#define _SIPSender GET_SINGLETON_INSTANCE(CSIPSender)

#endif // __SIP_SENDER_H__
