#include "ETLLib.hpp"
#include "interfacedefine.h"
#include "sipsender.h"
#include "h323sender.h"
#include "modvoip.h"
#include "devicelib/networkserver.h"
#include "configparser/modconfigparser.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#include "configiddefine.h"

#include <ylstring.h>
#include <ylstringalgorithm.h>


#include "service_sip_interface.h"
#include "talk/talklogic/include/talklogic_common.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "account/include/modaccount.h"

#include "service_h323_interface.h"
#include "service_h323_define.h"

#include "protocolhelper.h"

#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklistener/include/modtalklistener.h"
#include "talk/talkproxy/include/delaycalloutasyncproxy.h"

// 处理配置更改消息
static LRESULT OnProcessRegistMsg(msgObject& msg)
{
    if (MKIT_MSG_SRVREG == msg.message)
    {
        if (MKSRV_ID_SIPUA == msg.wParam)
        {
            // sip启动起来
            _SIPSender.InitSIP();
            return 0;
        }
#ifdef IF_FEATURE_H323
        else if (MKSRV_ID_H323 == msg.wParam)
        {
            _H323Sender.InitH323();
        }
#endif //#ifdef IF_FEATURE_H323
    }
#ifdef IF_FEATURE_H323
    else if (CONFIG_MSG_BCAST_CHANGED == msg.message)
    {
        if (ST_H323LINE == msg.wParam
                || (ST_CLOUD_CONFIG == msg.wParam
                    && 1 == msg.lParam))
        {
            // 云账号配置变更
            _H323Sender.SwitchH323();
        }
    }
#endif //#ifdef IF_FEATURE_H323

    return 1;
}

void modVoip_init()
{
    _ProtocolHelper.Init();

    // 初始化sip
    _SIPSender.InitSIP();

#ifdef IF_FEATURE_H323
    // 初始化H323
    _H323Sender.InitH323();

#endif //#ifdef IF_FEATURE_H323
    SingleMsgReg(MKIT_MSG_SRVREG, OnProcessRegistMsg);

    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, OnProcessRegistMsg);

    // 处理
}

void modVoip_StopService()
{
    _SIPSender.StopSip();

#ifdef IF_FEATURE_H323
    _H323Sender.StopH323();
#endif //#ifdef IF_FEATURE_H323
}

int modVoip_NewCallId()
{
    AUTO_SCOPED_TIMER;

    // 默认用sip的call id
    int iCallId = _SIPSender.RequestCallID();

    if (iCallId != 0)
    {
        return iCallId;
    }

    //return _H323Sender.RequestCallID();
    return rand();
}

bool modVoip_MakeCall(CallOutInfo& callInfo)
{
    //callInfo.m_iAccountID = modVoip_ReplaceAccount(callInfo.m_iAccountID);

    // 参数格式转换
    AccountProtocolType eProtocolType = acc_GetAccountProtocol(callInfo.m_iAccountID);

#ifdef IF_FEATURE_H323
    if (eProtocolType == PROTOCOL_H323)
    {
        return _H323Sender.CallInvite(&callInfo, acc_IsSupportH264(callInfo.m_iAccountID));
    }
#endif //#ifdef IF_FEATURE_H323

    SignalParams sigp(talklogic_GetCallInfoByCallId(callInfo.m_iCallID));
    sigp.wParam.iValue = callInfo.m_iAccountID;
    sigp.lParam.iValue = callInfo.m_iCallID;
    sigp.tParam.pValue = &callInfo;
    modtklsn_emit_sync(TALK_SIG_SESSION_MAKE_CALL, sigp);

    AUTO_SCOPED_TIMER;

    return _SIPSender.CallInvite(&callInfo, acc_IsSupportH264(callInfo.m_iAccountID));
}

bool modVoip_AnswerCall(int iAccountID, int iCallId, int iType, bool bMainStreamLimit)
{
#ifdef IF_FEATURE_H323
    if (acc_GetAccountProtocol(iAccountID) == PROTOCOL_H323)
    {
        return _H323Sender.AcceptCall(iAccountID, iCallId, iType, acc_IsSupportH264(iAccountID),
                                      bMainStreamLimit);
    }
#endif //#ifdef IF_FEATURE_H323

    SignalParams sigp(talklogic_GetCallInfoByCallId(iCallId));
    sigp.wParam.iValue = iAccountID;
    sigp.lParam.iValue = iType;
    sigp.tParam.iValue = bMainStreamLimit;
    modtklsn_emit_sync(TALK_SIG_SESSION_ANSWER, sigp);

    AUTO_SCOPED_TIMER;

    return _SIPSender.AcceptCall(iAccountID, iCallId, iType, acc_IsSupportH264(iAccountID),
                                 bMainStreamLimit);
}

bool modVoip_MuteCall(int iAccountID, int iCallId, bool bMute)
{
#ifdef IF_FEATURE_H323
    if (acc_GetAccountProtocol(iAccountID) == PROTOCOL_H323)
    {
        return _H323Sender.MuteCall(iAccountID, iCallId, bMute);
    }
#endif //#ifdef IF_FEATURE_H323

    AUTO_SCOPED_TIMER;

    return false;
}

bool modVoip_HangupCall(int iAccountID, int iCallId, int lParam/* = 0*/)
{
    COMMONUI_INFO("Hangup call[%d] type[%d]", iCallId, lParam);

#ifdef IF_FEATURE_H323
    if (acc_GetAccountProtocol(iAccountID) == PROTOCOL_H323)
    {
        return _H323Sender.CallHangUp(iAccountID, iCallId);
    }
#endif //#ifdef IF_FEATURE_H323
    SignalParams sigp(talklogic_GetCallInfoByCallId(iCallId));
    sigp.wParam.iValue = iAccountID;
    sigp.lParam.iValue = lParam;
    modtklsn_emit_sync(TALK_SIG_SESSION_HANDUP, sigp);

    AUTO_SCOPED_TIMER;

    return _SIPSender.CallHangUp(iAccountID, iCallId, lParam);
}

bool modVoip_RefuseCall(int iAccountID, int iCallId, int iCode, CallInUserInfo* pUserInfo/* = NULL*/)
{
#ifdef IF_FEATURE_H323
    if (acc_GetAccountProtocol(iAccountID) == PROTOCOL_H323)
    {
        return _H323Sender.RefuseCall(iAccountID, iCallId, iCode);
    }
#endif //#ifdef IF_FEATURE_H323

    SignalParams sigp(talklogic_GetCallInfoByCallId(iCallId));
    sigp.wParam.iValue = iAccountID;
    sigp.lParam.iValue = iCode;
    sigp.tParam.pValue = pUserInfo;
    modtklsn_emit_sync(TALK_SIG_SESSION_REFUSE_CALL, sigp);

    AUTO_SCOPED_TIMER;

    return _SIPSender.RefuseCall(iAccountID, iCallId, iCode);
}

bool modVoip_SendDtmf(int iAccountID, int iCallID, int iKey)
{
#ifdef IF_FEATURE_H323
    if (acc_GetAccountProtocol(iAccountID) == PROTOCOL_H323)
    {
        return _H323Sender.SendDTMF(iAccountID, iCallID, iKey);
    }
#endif //#ifdef IF_FEATURE_H323

    AUTO_SCOPED_TIMER;

    return _SIPSender.SendDTMF(iAccountID, iCallID, iKey);
}

bool modVoip_ResponseIncoming(int iAccountID, int iCallId, int iResponseCode)
{
    TALK_INFO("ResponseIncoming callid[%d] code[%d]", iCallId, iResponseCode);

#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return _H323Sender.ResponseIncoming(iAccountID, iCallId, iResponseCode);
    }
#endif //#ifdef IF_FEATURE_H323

    AUTO_SCOPED_TIMER;

    return _SIPSender.ResponseIncoming(iAccountID, iCallId, iResponseCode);
}

LINE_STATE modVoip_QueryLineState(int iAccountID)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return acc_MapH323State2Logic(_H323Sender.QueryLineState(iAccountID));
    }
#endif //#ifdef IF_FEATURE_H323

    AUTO_SCOPED_TIMER;

    return acc_MapState(_SIPSender.QueryLineState(iAccountID));
}

bool modVoip_RequestSrtpLifetime(int iAccountID, int iCallID)
{
#ifdef IF_FEATURE_H323
    if (acc_GetAccountProtocol(iAccountID) == PROTOCOL_H323)
    {
        // h323 不支持opus
        return false;
    }
#endif //#ifdef IF_FEATURE_H323

    AUTO_SCOPED_TIMER;

    return _SIPSender.SrtpLifetimeReinvite(iCallID, iAccountID);
}

// 需要IP直播的云账号
bool modVoip_NeedDirectIPCloudAccount(SipAccountType eType)
{
    return (eType == ACCOUNT_ZOOM
            || eType == ACCOUNT_MIND
            || eType == ACCOUNT_BLUEJEANS);
}

// int modVoip_ReplaceAccount(int iAccountID)
// {
//  if (modVoip_NeedDirectIPCloudAccount(acc_GetSipAccountType(iAccountID)))
//  {
//      return acc_GetDirectIPAccountID();
//  }
//
//  return iAccountID;
// }

bool modVoip_HoldCall(int iAccountID, int iCallId, bool bHold, bool bNeedNego/* = false*/)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return _H323Sender.SetHold(iAccountID, iCallId, bHold, bNeedNego);
    }
#endif //#ifdef IF_FEATURE_H323

    AUTO_SCOPED_TIMER;

    return _SIPSender.SetHold(iAccountID, iCallId, bHold);
}

#ifdef IF_SUPPORT_VIDEO
bool modVoip_SetCallMedia(int iAccountID, int iCallId, bool bVideo)
{
    COMMONAPI_INFO("set video call media[%d]", bVideo);

#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        //COMMONAPI_INFO("H323 protocol not deal hold");
        _H323Sender.SetCallMedia(iAccountID, iCallId, bVideo);
        return true;
    }
#endif //#ifdef IF_FEATURE_H323

    return _SIPSender.SetVideoCallMedia(iAccountID, iCallId, bVideo);
}

bool modVoip_VideoSharing(int iAccountID, int iCallID, bool bShare)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323

    return _SIPSender.VideoSharing(iAccountID, iCallID, bShare);
}

bool modVoip_NegoH264(int iAccountID, int iCallId, const VideoSizeInfo& videoSize, int iType,
                      bool bNegoNow, const VideoSizeInfo& extvideoSize)
{
#ifdef IF_FEATURE_H323
    if (acc_GetAccountProtocol(iAccountID) == PROTOCOL_H323)
    {
        return _H323Sender.CallNegoH264(iAccountID, iCallId, videoSize, extvideoSize, iType, bNegoNow);
    }
#endif //#ifdef IF_FEATURE_H323

    return _SIPSender.CallNegoH264(iAccountID, iCallId, videoSize, extvideoSize, iType, bNegoNow);
}

bool modVoip_RequestKeyFrame(int iAccountID, int iCallID, bool bShare)
{
#ifdef IF_FEATURE_H323
    if (acc_GetAccountProtocol(iAccountID) == PROTOCOL_H323)
    {
        return _H323Sender.ReqKeyframe(iAccountID, iCallID, bShare);
    }
#endif //#ifdef IF_FEATURE_H323

    return _SIPSender.ReqKeyframe(iAccountID, iCallID);
}

bool modVoip_RequestBitrate(int iAccountID, int iCallID, int iBandwidth, bool bShare)
{
#ifdef IF_FEATURE_H323
    if (acc_GetAccountProtocol(iAccountID) == PROTOCOL_H323)
    {
        return _H323Sender.ReqBitrate(iAccountID, iCallID, iBandwidth, bShare);
    }
#endif //#ifdef IF_FEATURE_H323

    return false;
}

bool modVoip_IsSupportVideo()
{
    if (devicelib_GetPhoneType() == PT_T49
            && configParser_GetConfigInt(kszVideoTalkEnable) != 0)
    {
        return true;
    }

    return false;
}

bool modVoip_CallNegoResume(int iAccountID, int iCallID)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323

    return _SIPSender.CallNegoResume(iAccountID, iCallID);
}
#endif //#ifdef IF_SUPPORT_VIDEO

bool modVoip_RequestTransfer(int iAccountID, int iTraneeId, int iTranerId, LPCSTR lpszTranTarget)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323

    SignalParams sigp(talklogic_GetSessionIdByCallID(iTraneeId), lpszTranTarget);
    sigp.wParam.iValue = iAccountID;
    sigp.lParam.iValue = iTranerId;
    modtklsn_emit_sync(TALK_SIG_SESSION_TRANSFER, sigp);

    AUTO_SCOPED_TIMER;

    return _SIPSender.RequestTransfer(iAccountID, iTraneeId, iTranerId, sigp.strData.c_str());
}

bool modVoip_EstablishNetworkConf(int iAccountID, int iCallID, int iCount, int* pConfCallIDList,
                                  int iMask /*= 0*/)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323

    AUTO_SCOPED_TIMER;

    return _SIPSender.EstablishNetworkConf(iAccountID, iCallID, iCount, pConfCallIDList, iMask);
}

bool modVoip_EstablishSCANetworkConf(int iAccountID, int iCallID, int iCount, int* pConfCallIDList,
                                     int iBlaIndex, int iMask /*= 0*/)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323

    AUTO_SCOPED_TIMER;

    return _SIPSender.EstablishSCANetworkConf(iAccountID, iCallID, iCount, pConfCallIDList, iBlaIndex,
            iMask);
}

bool modVoip_AddToNetworkConf(int iAccountID, int iSrcCallID, int iAddCallID)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323

    SignalParams sigp(talklogic_GetCallInfoByCallId(iSrcCallID));
    sigp.wParam.iValue = iAccountID;
    sigp.lParam.iValue = iAddCallID;
    modtklsn_emit_sync(TALK_SIG_SESSION_ADD_TO_NET_CONF, sigp);

    AUTO_SCOPED_TIMER;

    return _SIPSender.AddToNetworkConf(iAccountID, iSrcCallID, iAddCallID);
}

bool modVoip_RequestForward(int iAccountID, int iCallId, LPCSTR lpszForwardNumber,
                            int iForwardType /*= 0*/, CallInUserInfo* pUserInfo/* = NULL*/)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323

    SignalParams sigp(talklogic_GetCallInfoByCallId(iCallId), lpszForwardNumber);
    sigp.wParam.iValue = iAccountID;
    sigp.lParam.iValue = iForwardType;
    sigp.tParam.pValue = pUserInfo;
    modtklsn_emit_sync(TALK_SIG_SESSION_FWD, sigp);

    AUTO_SCOPED_TIMER;

    return _SIPSender.RequestForward(iAccountID, iCallId, lpszForwardNumber, iForwardType);
}

bool modVoip_SendOEMFlash(int iAccountID, int iCallID)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.SendOEMFlash(iAccountID, iCallID);
}

bool modVoip_BLAHold(int iAccountID, bool bPublic, int iCallId, bool bHold, int iBLAIndex/* = 0*/)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.BLAHold(iAccountID, bPublic, iCallId, bHold, iBLAIndex);
}

bool modVoip_DraftBLAHold(int iAccountID, bool bPublic, int iCallId, bool bHold)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.DraftBLAHold(iAccountID, bPublic, iCallId, bHold);
}

bool modVoip_StartURLRecord(int iAccountID, int iCallID, bool bStart)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.StartURLRecord(iAccountID, iCallID, bStart);
}

bool modVoip_GetCallInfo(int iAccountID, int iCallID, sip_call_proto_info2_t* lpszData)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.GetCallInfo(iAccountID, iCallID, lpszData);
}

bool modVoip_SendReportVQSession(int iAccountID, int iCallID, LPCSTR lpszData)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.SendReportVQSession(iAccountID, iCallID, lpszData);
}

int modVoip_SetOption(int option, void* value, unsigned int size, bool bSip)
{
    AUTO_SCOPED_TIMER;

    if (bSip)
    {
        _SIPSender.SetSipOption(option, value, size);
    }
#ifdef IF_FEATURE_H323
    else
    {
        _H323Sender.SetOption(option, value, size);
    }
#endif //#ifdef IF_FEATURE_H323

    return 1;
}

bool modVoip_DraftBLASeizeLine(int iAccountID, int iCallID, bool bSeize, int iIndex)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.DraftBLASeizeLine(iAccountID, iCallID, bSeize, iIndex);
}

bool modVoip_BLASeizeLine(int iAccountID, int iCallID, bool bSeize, int iIndex)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.BLASeizeLine(iAccountID, iCallID, bSeize, iIndex);
}

bool modVoip_SubscribCallCompletion(int iAccountID, int iCallID, BOOL bSubscribe,
                                    const char* pNumber)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.SubscribCallCompletion(iAccountID, iCallID, bSubscribe, pNumber);
}

bool modVoip_SendCSTAResponse(int iAccountID, int iCallID, int iTid, LPCSTR lpszData)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.SendCSTAResponse(iAccountID, iCallID, iTid, lpszData);
}

bool modVoip_SendCSTARequest(int iAccountID, int iCallID, LPCSTR lpszData)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.SendCSTARequest(iAccountID, iCallID, lpszData);
}

bool modVoip_SendCSTAAccptCall(int iAccountID, int iCallID, LPCSTR lpszData)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.SendCSTAAccptCall(iAccountID, iCallID, lpszData);
}

bool modVoip_SendAnsRequest(int iAccountID, int iCallID, LPCSTR lpszData)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.SendAnsRequest(iAccountID, iCallID, lpszData);
}

bool modVoip_FeatureKeySync(int iAccountID, int iTargetAccountID, LPCSTR lpszData)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.FeatureKeySync(iAccountID, iTargetAccountID, lpszData);
}

bool modVoip_SendAcdInfoReq(int iAccountID, int iCallID, LPCSTR lpszData)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.SendAcdInfoReq(iAccountID, iCallID, lpszData);
}

bool modVoip_ReqQueueStatus(int iAccountID, bool IsOn)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.ReqQueueStatus(iAccountID, IsOn);
}

bool modVoip_ACDAction(int iAccountID, int iKeyIndex, int iType, const char* pUser /*= NULL*/,
                       const char* pPwd /*= NULL*/)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.ACDAction(iAccountID, iKeyIndex, iType, pUser, pPwd);
}

bool modVoip_ReqHoteling(int iAccountID, int iStatus, const char* pUser, const char* pPwd)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.ReqHoteling(iAccountID, iStatus, pUser, pPwd);
}

bool modVoip_CallInvite(int iAccountID, int iCallId, LPCSTR lpszFullName)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.CallInvite(iAccountID, iCallId, lpszFullName);
}

bool modVoip_SendTextMessage(int iAccountID, const char* pText, const char* pTo)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.SendTextMessage(iAccountID, pText, pTo);
}

yl::string modVoip_GetUserAgent(int iAccountID, int iCallId)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return _H323Sender.GetUserAgent(iAccountID, iCallId);
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.GetUserAgent(iAccountID, iCallId);
}

bool modVoip_StartFortinetRecord(int iAccountID, int iCallID, BOOL bStart)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.StartFortinetRecord(iAccountID, iCallID, bStart);
}

bool modVoip_ReInviteRespose(int iAccountID, int iCallID, int iTID, bool bAccept)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.ReInviteRespose(iAccountID, iCallID, iTID, bAccept);
}

bool modVoip_SrtpLifetimeReinvite(int iAccountID, int iCallID)
{
#ifdef IF_FEATURE_H323
    if (PROTOCOL_H323 == acc_GetAccountProtocol(iAccountID))
    {
        // 确认h323不支持
        return false;
    }
#endif //#ifdef IF_FEATURE_H323
    AUTO_SCOPED_TIMER;

    return _SIPSender.SrtpLifetimeReinvite(iAccountID, iCallID);
}
#ifdef IF_FEATURE_EMERGENCY
bool modVoip_EmergencyCall_HangupReInvite(int iAccoutID, int iCallID)
{
    AUTO_SCOPED_TIMER;

    return _SIPSender.EmergencyCallHangup(iAccoutID, iCallID);
}
#endif

//删除网络会议与会者
bool modVoip_DeleteNetConfParty(int iAccountID, int iCallID, const char* pUserId)
{
    AUTO_SCOPED_TIMER;

    return _SIPSender.DeleteNetConfParty(iAccountID, iCallID, pUserId);
}


