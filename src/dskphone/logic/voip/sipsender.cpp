#include "ETLLib.hpp"
#include "interfacedefine.h"
#include "sipsender.h"
#include "devicelib/networkserver.h"
#include "configparser/modconfigparser.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#include "configiddefine.h"
#include <ylstring.h>
#include <ylstringalgorithm.h>
#include "broadsoft/callcenter/include/cccommon.h"
#include "service_sip_interface.h"
#include "talk/talklogic/include/talklogic_common.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "account/include/modaccount.h"
#include "talk/emergency/include/modemergency.h"
#include "talk/fwd_dnd/include/fwddndtypes.h"
#include "commonfunctions.h"
#include <winimp/winimp.h>
#include "talk/talkproxy/include/delaycalloutasyncproxy.h"

//#define MAIN_STREAM 1
//#define SHARE_STREAM 2
extern BOOL netIsValidIPV6(LPCSTR lpszIpAddress);
extern bool Comm_IsValidIP(LPCSTR lpszIpAddress);

// 将发送给sip消息的IPV6地址加上"[]"
static void AddIPV6Flag(yl::string & strURI)
{
    // 如果呼出的号码IP是IPV6,则要加上"[]"标示呼出的是IPV6,例如:"sip:123456@[2001:250:1800::1]"
    const char * pServer = strchr(strURI.c_str(), '@');
    if (pServer != NULL)
    {
        if (netIsValidIPV6(pServer + 1))
        {
            yl::string strIPV6 = commonAPI_FormatString("@[%s]", pServer + 1);
            string_replace(strURI, pServer, strIPV6);
        }
    }
    else
    {
        // sip:2003:1::123 or 2003:2::123
        yl::string strNum = strURI;
        int iPos = strURI.find("sip:");
        if (iPos != yl::string::npos)
        {
            strNum = strURI.substr(iPos + 4, strURI.size() - iPos);
        }

        if (!netIsValidIPV6(strNum.c_str()))
        {
            return;
        }

        strURI.clear();
        if (iPos != yl::string::npos)
        {
            strURI = "sip:";
        }

        strURI = strURI + "[" + strNum + "]";
    }
}

static LRESULT OnTimerMsg(msgObject & msg)
{
    if (TM_TIMER != msg.message)
    {
        return 0;
    }

    return _SIPSender.OnTimer(msg.wParam) ? 1 : 0;
}

IMPLEMENT_GETINSTANCE(CSIPSender)

CSIPSender::CSIPSender()
{
    m_iStartServiceTimer = -1;
    m_iWaitAutopTimer = configParser_GetConfigInt(kszWaitAutopTime);
    if (m_iWaitAutopTimer < 0 || m_iWaitAutopTimer > 3600)
    {
        m_iWaitAutopTimer = 0;
    }
}

CSIPSender::~CSIPSender()
{

}

//  查询
int CSIPSender::QueryLineState(int iAccountID)
{
    return sip_line_get_state(mkGetHandle(), iAccountID);
}

// 接起来电，eType代表接起方式
BOOL CSIPSender::AcceptCall(int iAccountID, int iCallId, int iType, bool bSupportH264,
                            bool bMainStreamLimit)
{
    //COMMONUNITS_INFO("Accept Call [%d]", iType);
#ifdef IF_SUPPORT_VIDEO
    if (iType == 1)
    {
        // todo h323
        int iBandwidth = configParser_GetConfigInt(kszIncomingBandwidth) * 1000;
        COMMONUNITS_INFO("AcceptCall Set Bandwidth[%d]", iBandwidth);
        if (iBandwidth <= 0)
        {
            iBandwidth = 2 * 1024 * 1024;
        }

        sip_call_set_video_bandwidth(mkGetHandle(), iAccountID, iCallId, iBandwidth);
    }
    else
    {
        COMMONUNITS_INFO("Set incoming video[%d]", iType);
        // 接听音视频通话控制sip要求GUI改成调接口
        SetVideoCallMedia(iAccountID, iCallId, 0);
    }

    if (bSupportH264)
    {
        // 设置辅流参数为1080P*15fps
        SetVideoH264Param(iAccountID, iCallId, bMainStreamLimit ? VideoSizeInfo(1280, 720,
                          30) : VideoSizeInfo(1920, 1080, 30),
                          VideoSizeInfo(1920, 1080, 15), (SHARE_STREAM | MAIN_STREAM));
    }
#endif //#ifdef IF_SUPPORT_VIDEO

    return 0 == sip_call_accept(mkGetHandle(), iAccountID, iCallId, iType);
}

// 拒接来电
BOOL CSIPSender::RefuseCall(int iAccountID, int iCallId, int iRefuseCode)
{
    COMMONUNITS_INFO("Refuse call");

    return 0 == sip_call_response(mkGetHandle(), iAccountID, iCallId, iRefuseCode);
}

// 邀请通话目标 lpszFullName代表呼叫目标
BOOL CSIPSender::CallInvite(int iAccountID, int iCallId, LPCSTR lpszFullName)
{

    yl::string strURI = lpszFullName;
    AddIPV6Flag(strURI);

    COMMONUNITS_INFO("CallInvite account[%d] CallId[%d] num[%s]", iAccountID, iCallId, strURI.c_str());
    return 0 == sip_call_invite(mkGetHandle(), iAccountID, iCallId, 0, strURI.c_str(), NULL);

}

#ifdef IF_SUPPORT_VIDEO
bool CSIPSender::SetVideoH264Param(int iAccountID, int iCallId, const VideoSizeInfo & videoSize,
                                   const VideoSizeInfo & extvideoSize, int iType)
{
#ifdef _T49
    COMMONUNITS_INFO("set h264 w[%d] h[%d] fps[%d] type[%d]", videoSize.w, videoSize.h, videoSize.fps,
                     iType);
    COMMONUNITS_INFO("set ext h264 w[%d] h[%d] fps[%d] type[%d]", extvideoSize.w, extvideoSize.h,
                     extvideoSize.fps, iType);

    //主流
    // 接听来电也设置是否要支持1080p
    sip_ext_params_t * params;
    if (0 != sip_ext_params_init(&params))
    {
        return FALSE;
    }

    if (MAIN_STREAM & iType)
    {
        //主流参数
        sip_h264_param_t h264_param;
        set_h264_param(h264_param, videoSize.w, videoSize.h, videoSize.fps, false);
        sip_ext_params_add(&params, SIP_MAKE_CALL_H264_PARAM, &h264_param, sizeof(sip_h264_param_t));
    }

    if (SHARE_STREAM & iType)
    {
        //辅流参数
        sip_h264_param_t ext_h264_param;
        set_h264_param(ext_h264_param, extvideoSize.w, extvideoSize.h, extvideoSize.fps, false);
        sip_ext_params_add(&params, SIP_MAKE_CALL_EXT_H264_PARAM, &ext_h264_param,
                           sizeof(sip_h264_param_t));
    }

    // 辅流禁用
    //int bDisable = true;
    //sip_ext_params_add(ext_param, SIP_MAKE_CALL_DISABLE_BFCP, &bDisable, sizeof(int));

    sip_call_set_call_param(mkGetHandle(), iAccountID, iCallId, params);

    sip_ext_params_free(&params);
#endif // T49
    return true;
}

bool CSIPSender::CallNegoH264(int iAccountID, int iCallId, const VideoSizeInfo & videoSize,
                              const VideoSizeInfo & extvideoSize, int iType, bool bNegoNow)
{
    SetVideoH264Param(iAccountID, iCallId, videoSize, extvideoSize, iType);

    if (bNegoNow)
    {
        sip_call_resume(mkGetHandle(), iAccountID, iCallId);
    }
    return true;
}
#endif //#ifdef IF_SUPPORT_VIDEO

BOOL CSIPSender::CallInvite(CallOutInfo * pCallInfo, bool bSupportH264)
{
    if (NULL == pCallInfo)
    {
        return FALSE;
    }


    yl::string strURI = pCallInfo->m_strNumber;

    bool bIsValidIP = Comm_IsValidIP(strURI.c_str());

    // ip##id
    if (!pCallInfo->m_strExtNumber.empty())
    {
        strURI = pCallInfo->m_strExtNumber + "@" + pCallInfo->m_strNumber;
    }

    // ip直播及uri要加上sip：短号不加
    if (!strURI.empty()
            && strURI.find("@") != yl::string::npos
            && strURI.substr(0, 4) != "sip:"
            || bIsValidIP)
    {
        strURI = "sip:" + strURI;
    }

    AddIPV6Flag(strURI);

    sip_ext_params_t * params;
    if (0 != sip_ext_params_init(&params))
    {
        return FALSE;
    }

    if (pCallInfo->m_iBLFIndex != -1)
    {
        sip_ext_params_add(&params, SIP_MAKE_CALL_PICKUP_INDEX, &(pCallInfo->m_iBLFIndex), sizeof(int));
    }

    if (pCallInfo->m_iBLAIndex != -1
            && pCallInfo->m_eBlaType == (int)BLA_BROADSOFT_SCA)
    {
        sip_ext_params_add(&params, SIP_MAKE_CALL_SCA_INDEX, &(pCallInfo->m_iBLAIndex), sizeof(int));
    }
    else if (pCallInfo->m_iBLAIndex != -1
             && pCallInfo->m_eBlaType == (int)BLA_DRAFT_BLA)
    {
        // DraftBLA index 从0开始逻辑层为了和sca保持一致，从1开始
        int iBlaIndex = pCallInfo->m_iBLAIndex > 0 ? pCallInfo->m_iBLAIndex - 1 : 0;
        sip_ext_params_add(&params, SIP_MAKE_CALL_BLA_INDEX, &iBlaIndex, sizeof(int));
    }

    if (!pCallInfo->m_strCode.empty())
    {
        sip_ext_params_add(&params, SIP_BSFT_DISPOSITION_CODE, (void *)pCallInfo->m_strCode.c_str(),
                           pCallInfo->m_strCode.size());
    }

    if (pCallInfo->m_eCallOutType == TCT_TIPTEL_AUTO_CALLOUT)
    {
        sip_ext_params_add(&params, SIP_MAKE_CALL_TIPTEL_INVOKE_ID,
                           (void *)pCallInfo->m_strInvokeID.c_str(), pCallInfo->m_strInvokeID.size());
    }

#ifdef IF_FEATURE_EMERGENCY
    COMMONUNITS_INFO("EmergencyIndex [%d]", pCallInfo->m_iEmergencyIndex);
    if (pCallInfo->m_iEmergencyIndex >= 1)
    {
        LPCSTR lpAssertedId = Emergency_GetAssertId();
        // 紧急呼出标志位
        if (!textEMPTY(lpAssertedId))
        {
            yl::string strAssertedId = lpAssertedId;
            sip_ext_params_add(&params, SIP_MESSAGE_HEADERS, (void *)strAssertedId.c_str(), strAssertedId.size());
        }

        const yl::string & strEmergencyServerList = Emergency_GetServerPriority(pCallInfo->m_iEmergencyIndex);
        sip_ext_params_add(&params, SIP_MAKE_CALL_EMERGENCY_SRV_PRI, (void *)strEmergencyServerList.c_str(), strEmergencyServerList.size());
    }
#endif //IF_FEATURE_EMERGENCY

#if IF_FEATURE_CTRIP_CTI
    if (!pCallInfo->m_strXUUI.empty())
    {
        yl::string strXUUI = "X-UUI:" + pCallInfo->m_strXUUI + "\r\n";
        int i = sip_ext_params_add(&params, SIP_MESSAGE_HEADERS, (void *)strXUUI.c_str(), strXUUI.size());
    }
#endif //IF_FEATURE_CTRIP_CTI

    if (pCallInfo->m_iMask & SIP_CALL_MASK_VIDEO)
    {
        // 呼出带宽
        int iBandwidth = configParser_GetConfigInt(kszOutgoingBandwidth) * 1024;
        if (iBandwidth <= 0)
        {
            iBandwidth = 1 * 1280 * 1024;
        }
        sip_ext_params_add(&params, SIP_MAKE_CALL_VIDEO_BANDWIDTH, &iBandwidth, sizeof(int));

        // 配置H263时由sip读取配置
        if (bSupportH264)
        {
            // 有辅流要限制主流的分辨率
            sip_h264_param_t h264_param;
            set_h264_param(h264_param, 1280, 720, 30, false);
            COMMONUNITS_INFO("Set Video Resolution");
            sip_ext_params_add(&params, SIP_MAKE_CALL_H264_PARAM, &h264_param, sizeof(sip_h264_param_t));

            // 辅流要设置参数，限制最大fps为15
            sip_h264_param_t ext_h264_param;
            set_h264_param(ext_h264_param, 1920, 1080, 15, false);
            sip_ext_params_add(&params, SIP_MAKE_CALL_EXT_H264_PARAM, &ext_h264_param,
                               sizeof(sip_h264_param_t));
        }

#if 0 //http://bugfree.yealink.com/Bug.php?BugID=77952
        //默认呼出由SIP 读取配置
        sip_h264_param_t h264_param;
        yl::string strProfileID = acc_GetVideoParam(pCallInfo->m_iAccountID, 0, ACCOUNT_VIDEO_PROFILE_LEVEL,
                                  "profile-level-id");

        if (strProfileID.empty()
                || !set_h264_param_by_config(h264_param, strProfileID))
        {
            set_h264_param(h264_param, 1280, 720, 30);
        }

        sip_ext_params_add(&params, SIP_MAKE_CALL_H264_PARAM, &h264_param, sizeof(sip_h264_param_t));
#endif // 0
    }

    COMMONUNITS_INFO("CallInvite:AccountID[%d] strURI[%s]", pCallInfo->m_iAccountID, strURI.c_str());
    int iRet = sip_call_invite(mkGetHandle(), pCallInfo->m_iAccountID, pCallInfo->m_iCallID,
                               pCallInfo->m_iMask, strURI.c_str(), params);
    sip_ext_params_free(&params);
    return iRet == 0;
}

// 通话挂断
BOOL CSIPSender::CallHangUp(int iAccountID, int iCallId, int lParam)
{
    /*
    lparam为0的话  或者默认的话就传递NULL
    lparam为1的话，就传递字符串："SIP;cause=200;text=\"Call completed elsewhere\""
    lparam为2的话，就传递字符串： "pcc;transfer"
    */

    const char * pInfo = NULL;
    switch (lParam)
    {
    case 1:
        pInfo = "SIP;cause=200;text=\"Call completed elsewhere\"";
        break;
    case 2:
        pInfo = "pcc;transfer";
        break;
    case 3:
        pInfo = "Call timeout";
        break;
    default:
        break;
    }
    return 0 == sip_call_release(mkGetHandle(), iAccountID, iCallId, pInfo);

}

// 设置通话Hold，bHold代表是否Hold对方
BOOL CSIPSender::SetHold(int iAccountID, int iCallId, BOOL bHold)
{
    COMMONUNITS_INFO("sip hold[%d][%d]", iCallId, bHold);

    if (bHold)
    {
        return 0 == sip_call_hold(mkGetHandle(), iAccountID, iCallId);
    }
    else
    {
        return 0 == sip_call_resume(mkGetHandle(), iAccountID, iCallId);
    }

}

// BLA hold
BOOL CSIPSender::BLAHold(int iAccountID, BOOL bPublic, int iCallId, BOOL bHold, int iBLAIndex)
{
    if (bPublic)
    {
        return bHold ? sip_call_sca_public_hold_ex(mkGetHandle(), iAccountID, iCallId,
                iBLAIndex) : sip_call_sca_public_resume_ex(mkGetHandle(), iAccountID, iCallId, iBLAIndex);
        //return bHold ? sip_call_sca_public_hold(mkGetHandle(), iAccountID, iCallId) : sip_call_sca_public_resume(mkGetHandle(), iAccountID, iCallId);
    }
    else
    {
        return bHold ? sip_call_sca_private_hold(mkGetHandle(), iAccountID,
                iCallId) : sip_call_sca_private_resume(mkGetHandle(), iAccountID, iCallId);
    }
}

// 请求呼叫转移，lpszForwardNumber代表转移目标
BOOL CSIPSender::RequestForward(int iAccountID, int iCallId, LPCSTR lpszForwardNumber,
                                int iForwardType/* = 0*/)
{

    yl::string strURI = lpszForwardNumber;

    AddIPV6Flag(strURI);

    int iType = 0;
    switch (iForwardType)
    {
    case FWD_TYPE_MANUAL:
        iType = 0;
        break;
    case FWD_TYPE_ALWAYS:
        iType = 1;
        break;
    case FWD_TYPE_BUSY:
        iType = 2;
        break;
    case FWD_TYPE_NO_ANSWER:
        iType = 3;
        break;
    default:
        iType = 0;
        break;
    }

    COMMONUNITS_INFO("RequestForward: strURI = [%s], Type = [%d]", strURI.c_str(), iType);
    return 0 == sip_call_forward(mkGetHandle(), iAccountID, iCallId, iType, strURI.c_str());
}

// 请求呼叫转接，iTraneeId为被转方的CallId，iTranerId为转移方的CallId，lpszTranTarget代表转接目标
BOOL CSIPSender::RequestTransfer(int iAccountID, int iTraneeId, int iTranerId,
                                 LPCSTR lpszTranTarget)
{
    yl::string strURI = lpszTranTarget;
    AddIPV6Flag(strURI);

    COMMONUNITS_INFO("RequestTransfer[%s] TraneeId[%d] TranerId[%d] ", strURI.c_str(), iTraneeId,
                     iTranerId);
    return 0 == sip_call_transfer(mkGetHandle(), iAccountID, iTraneeId, iTranerId, strURI.c_str());
}

// 更新PBX button
BOOL CSIPSender::UpdatePBXButton(int iAccountID, LPCSTR lpszNotify)
{
    return false;
    // yzh sip未实现
//  return 0 == sip_send_notify_tiptel_pbx_button(mkGetHandle(), iAccountID, 0, 0, (char *)lpszNotify);
}

// ACD响应
BOOL CSIPSender::ACDAction(int iAccountID, int iKeyIndex, int iType, const char * pUser,
                           const char * pPwd)
{
    int iAction = SIP_ACD_ACTION_LOGIN;
    switch (iType)
    {
    case AA_LOGIN:
        iAction = SIP_ACD_ACTION_LOGIN;
        break;
    case AA_AVAIL:
        iAction = SIP_ACD_ACTION_AVAIL;
        break;
    case AA_UNAVAIL:
        iAction = SIP_ACD_ACTION_UNAVAIL;
        break;
    case AA_LOGOUT:
        iAction = SIP_ACD_ACTION_LOGOUT;
        break;
    case AA_WRAPUP:
        iAction = SIP_ACD_ACTION_DETACH;
        break;
    default:
        break;
    }

    COMMONUNITS_INFO("CSIPSender::ACDAction account=[%d] type=[%d] send=[%d] ", iAccountID, iType,
                     iAction);
    return 0 == sip_acd_control(mkGetHandle(), iAccountID, iAction, iKeyIndex, pUser, pPwd);
}

// ATS添加删除参与方
BOOL CSIPSender::ModifyATSPartner(int iAccountID, BOOL bAdd, int iCallID, LPCSTR lpszNumber)
{
    return sip_call_conference_ats_add_user(mkGetHandle(), iAccountID, iCallID, lpszNumber);
}

// 建立网络会议
BOOL CSIPSender::EstablishNetworkConf(int iAccountID, int iCallID, int iCount,
                                      int * pConfCallIDList, int iMask/* = 0*/)
{
    if (pConfCallIDList == NULL)
    {
        return FALSE;
    }

    return 0 == sip_call_conference(mkGetHandle(), iAccountID, iCallID, iMask, iCount, pConfCallIDList,
                                    NULL, NULL);
}

// 建立SCA网络会议
BOOL CSIPSender::EstablishSCANetworkConf(int iAccountID, int iCallID, int iCount,
        int * pConfCallIDList, int iBlaIndex, int iMask/* = 0*/)
{
    if (pConfCallIDList == NULL)
    {
        return FALSE;
    }
    sip_ext_params_t * params;
    sip_ext_params_init(&params);
    sip_ext_params_add(&params, SIP_MAKE_CALL_SCA_INDEX, &iBlaIndex, sizeof(int));

    int iRet = sip_call_conference(mkGetHandle(), iAccountID, iCallID, iMask, iCount, pConfCallIDList,
                                   NULL, params);

    sip_ext_params_free(&params);
    return iRet;
}

// 网络会议加人
BOOL CSIPSender::AddToNetworkConf(int iAccountID, int iSrcCallID, int iAddCallID)
{
    return 0 == sip_call_conference_add_user(mkGetHandle(), iAccountID, iAddCallID, iSrcCallID);
}

// 开始或关闭URLRecord
BOOL CSIPSender::StartURLRecord(int iAccountID, int iCallID, BOOL bStart)
{
    if (bStart)
    {
        return 0 == sip_call_send_info_record_on(mkGetHandle(), iAccountID, iCallID, 0);
    }
    else
    {
        return 0 == sip_call_send_info_record_off(mkGetHandle(), iAccountID, iCallID, 0);
    }
}

#if IF_BUG_22457_Record
// 开始或关闭FortinetRecord
BOOL CSIPSender::StartFortinetRecord(int iAccountID, int iCallID, BOOL bStart)
{
    if (bStart)
    {
        return 0 == sip_call_send_info_fortinet_record_on(mkGetHandle(), iAccountID, iCallID, 0);
    }
    else
    {
        return 0 == sip_call_send_info_fortinet_record_off(mkGetHandle(), iAccountID, iCallID, 0);
    }
}
#endif //IF_BUG_22457_Record

// 订阅与取消订阅CC
BOOL CSIPSender::SubscribCallCompletion(int iAccountID, int iCallID, BOOL bSubscribe,
                                        const char * pNumber)
{
    if (bSubscribe)
    {
        return 0 == sip_call_completion_subscribe(mkGetHandle(), iAccountID, pNumber);
    }
    else
    {
        return 0 == sip_call_completion_unsubscribe(mkGetHandle(), iAccountID, pNumber);
    }
}

// 功能键同步
BOOL CSIPSender::FeatureKeySync(int iAccountID, int iTargetAccountID, LPCSTR lpszData)
{
    COMMONUNITS_INFO("Send sync:%d, %s", iAccountID, lpszData);
    //此处长度不能+1
    return 0 == sip_send_feature_key(mkGetHandle(), iAccountID, lpszData);
}

// 给Sip发送DTMF按键消息,iKey为按键键值
BOOL CSIPSender::SendDTMF(int iAccountID, int iCallID, int iKey)
{
    if (iKey == PHONE_KEY_STAR)
    {
        iKey = 10;
    }
    else if (iKey == PHONE_KEY_POUND)
    {
        iKey = 11;
    }
    else if (iKey >= PHONE_KEY_0 && iKey <= PHONE_KEY_9)
    {
        iKey -= PHONE_KEY_0;
    }
#if IF_FEATURE_DTMF_FLASH
    else if (iKey >= PHONE_KEY_A && iKey <= PHONE_KEY_E)
#else
    else if (iKey >= PHONE_KEY_A && iKey <= PHONE_KEY_D)
#endif //IF_FEATURE_DTMF_FLASH
    {
        iKey = iKey - PHONE_KEY_A + 12;
    }
    else
    {
        return FALSE;
    }

    return 0 == sip_call_send_dtmf(mkGetHandle(), iAccountID, iCallID, iKey);
}

// 发送OEM FLASH
BOOL CSIPSender::SendOEMFlash(int iAccoutID, int iCallID)
{
    return 0 == sip_call_send_info_broadsoft_flash(mkGetHandle(), iAccoutID, iCallID, 0);
}

// BLA占线与释放占线
BOOL CSIPSender::BLASeizeLine(int iAccountID, int iCallID, BOOL bSeize, int iIndex)
{
    if (bSeize)
    {
        return 0 == sip_sca_seize_line(mkGetHandle(), iAccountID, iIndex);
    }
    else
    {
        return 0 == sip_sca_unseize_line(mkGetHandle(), iAccountID, iIndex);
    }
}

// 发送消息
BOOL CSIPSender::SendTextMessage(int iAccountID, const char * pText, const char * pTo)
{
    if (NULL == pText
            || NULL == pTo)
    {
        return FALSE;
    }
    yl::string strSendData = pText;
    strSendData += pTo;

    //发送信息
    return 0 == sip_send_sms(mkGetHandle(), iAccountID, 0, pTo, pText);
}

// 向sip获取call id
int CSIPSender::RequestCallID()
{
    return sip_call_new_call_id(mkGetHandle());
}

// 来电时响应SIP
BOOL CSIPSender::ResponseIncoming(int iAccountID, int iCallID, int iResponseCode)
{
    COMMONUNITS_INFO("post SUA_MSG_CALL_RESPONSE");
    return 0 == sip_call_response(mkGetHandle(), iAccountID, iCallID, iResponseCode);
}

//BOOL CSIPSender::SendCSTAInfoReq(int iAccountID, int iCallID, LPCSTR lpszData)
//{
// yzh sip未实现
//  return PostMSGToSIPThreadEx(iAccountID,SIP_MSG_SEND_INFO_REQUEST,iCallID,7,strlen(lpszData)+1, lpszData);
//  return FALSE;
//}

BOOL CSIPSender::SendAnsRequest(int iAccountID, int iCallID, LPCSTR lpszData)
{
    // yzh sip未实现
//  return PostMSGToSIPThreadEx(iAccountID,SIP_MSG_ANSWER_REQUEST,iCallID,1,strlen(lpszData)+1, lpszData);
    return FALSE;
}

BOOL CSIPSender::SendCSTAAccptCall(int iAccountID, int iCallID, LPCSTR lpszData)
{
    return FALSE;
//  return PostMSGToSIPThreadEx(iAccountID,SIP_MSG_ANSWER_CALL,iCallID,3,strlen(lpszData)+1, lpszData);
    // yzh sip未实现
//  return 0 == sip_call_accept(mkGetHandle(), iAccountID, iCallID, 0);
}


BOOL CSIPSender::SendCSTARequest(int iAccountID, int iCallID, LPCSTR lpszData)
{
    return (0 == sip_uacsta_send_request(mkGetHandle(), iAccountID, iCallID, 0, lpszData));
}

BOOL CSIPSender::SendCSTAResponse(int iAccountID, int iCallID, int iTid, LPCSTR lpszData)
{
    return (0 == sip_uacsta_send_response(mkGetHandle(), iAccountID, iCallID, iTid, lpszData));
}

#if IF_FEATURE_CTRIP_CTI
BOOL CSIPSender::SendCtripHWCTIRequest(int iAccountID, int iCallID, sip_uc_req_send_info_t * info)
{
    return (0 == sip_uc_send_request(mkGetHandle(), iAccountID, iCallID, info,
                                     sizeof(sip_uc_req_send_info_t)));
}

BOOL CSIPSender::SendCtripHWCTIResponse(int iAccountID, int iCallID, int iTid, int iCode)
{
    return (0 == sip_uc_send_response(mkGetHandle(), iAccountID, iCallID, iTid, iCode));
}
BOOL CSIPSender::SendCtripFSCTIResponse(int iAccountID, int iCallID, int iTid, int iCode)
{
    COMMONUNITS_INFO("Send CtripCtiCmdResult: %d, %d, %d, %d", iAccountID, iCallID, iTid, iCode);
    return (0 == sip_uc_send_notify_response(mkGetHandle(), iAccountID, iCallID, iTid, iCode));
}
#endif

BOOL CSIPSender::SendReportVQSession(int iAccountID, int iCallID, LPCSTR lpszData)
{
    return (0 == sip_call_send_rtcp_xr(mkGetHandle(), iAccountID, iCallID, 0, lpszData));
}

BOOL CSIPSender::GetCallInfo(int iAccountID, int iCallID, sip_call_proto_info2_t * lpszData)
{
    // 新版本的SIP, 使用该接口
    if (strcmp(GetVersion("Interface").c_str(), "1.0.0.0") > 0)
    {
        return (0 == sip_call_get_info2(mkGetHandle(), iAccountID, iCallID, lpszData, sizeof(sip_call_proto_info2_t)));
    }

    // 旧版本的SIP, 使用该接口
    sip_call_proto_info_t info;
    if (0 == sip_call_get_info(mkGetHandle(), iAccountID, iCallID, &info))
    {
        strcpy(lpszData->call_id, info.call_id);
        strcpy(lpszData->local_tag, info.local_tag);
        strcpy(lpszData->remote_tag, info.remote_tag);
        memset(lpszData->from, 0, sizeof(lpszData->from));
        memset(lpszData->to, 0, sizeof(lpszData->to));
        return true;
    }
    return false;
}

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

BOOL CSIPSender::ReqHoteling(int iAccountID, int iStatus, const char * pUser, const char * pPwd)
{
    if (HOTELING_STATUS_LOGIN == iStatus)
    {
        return 0 == sip_hoteling_login(mkGetHandle(), iAccountID, pUser, pPwd);
    }
    else
    {
        return 0 == sip_hoteling_logout(mkGetHandle(), iAccountID);
    }
}

BOOL CSIPSender::SendAcdInfoReq(int iAccountID, int iCallID, LPCSTR lpszData)
{
    return 0 == sip_call_send_info_bw_service(mkGetHandle(), iAccountID, iCallID, 0, (char *)lpszData);
}

BOOL CSIPSender::ReqQueueStatus(int iAccountID, bool IsOn)
{
    if (IsOn)
    {
        return 0 == sip_broadsoft_call_center_subscribe(mkGetHandle(), iAccountID);
    }
    else
    {
        return 0 == sip_broadsoft_call_center_unsubscribe(mkGetHandle(), iAccountID);
    }
}
#endif //IF_FEATURE_BROADSOFT_CALL_CENTER

bool CSIPSender::IsSupportVideo()
{
    if (devicelib_GetPhoneType() == PT_T49
            && configParser_GetConfigInt(kszVideoTalkEnable) != TVO_DISABLE)
    {
        return true;
    }
    return false;
}

int CSIPSender::StopSip()
{
    static BOOL bStopService = FALSE;

    if (bStopService)
    {
        return 0;
    }

    int iRet = sip_service_stop(mkGetHandle());
    bStopService = TRUE;
    return iRet;
}

// 启动sip
int CSIPSender::StartSIP()
{
    int iRet = sip_service_start(mkGetHandle());

    int iValue = IsSupportVideo() ? 1 : 0;
    SetSipOption(SIP_SERVICE_OPT_VIDEO_CAP, (void *) &iValue, sizeof(int));
    return iRet;
}

bool CSIPSender::SetSipOption(int option, void * value, unsigned int size)
{
#ifdef _T49
    return (sip_service_set_option(mkGetHandle(), option, value, size) == 0);
#else
    return false;
#endif
}

#ifdef IF_SUPPORT_VIDEO
bool CSIPSender::SetVideoCallMedia(int iAccountID, int iCallID, bool bVideo)
{
    COMMONUNITS_INFO("SetVideoCallMedia acc=[%d] callid=[%d] video=[%d]", iAccountID, iCallID, bVideo);
    return (sip_call_set_video_media(mkGetHandle(), iAccountID, iCallID, bVideo ? 1 : 0) == 0);
}

bool CSIPSender::VideoSharing(int iAccountID, int iCallID, bool bShare)
{
    COMMONUNITS_INFO("StartSharing acc[%d] callid[%d] share[%d]", iAccountID, iCallID, bShare);

    if (bShare)
    {
        //return sip_call_start_shareling_video(mkGetHandle(), iAccountID, iCallID) == 0;
    }
    else
    {
        //return sip_call_stop_shareling_video(mkGetHandle(), iAccountID, iCallID) == 0;
    }
    return true;
}

bool CSIPSender::CallNegoResume(int iAccountID, int iCallID)
{
    COMMONUNITS_INFO("CallNegoResume acc[%d] callid[%d]", iAccountID, iCallID);
    return sip_call_resume(mkGetHandle(), iAccountID, iCallID) == 0;
}

#endif //#ifdef IF_SUPPORT_VIDEO

// 初始化sip
BOOL CSIPSender::InitSIP()
{
    if (mkservice_get_data(mkGetHandle(), MKSRV_ID_SIPUA, 0) <= 0)
    {
        COMMONUNITS_INFO("Init sip: not running!");
        return FALSE;
    }

    if (m_iWaitAutopTimer > 0)
    {
        COMMONUNITS_INFO("wait autop [%d]", m_iWaitAutopTimer);
        SingleMsgReg(TM_TIMER, OnTimerMsg);
        // 3s检测一次等待autop更新完成
        timerSetThreadTimer((UINT)&m_iWaitAutopTimer, 3000);
        return FALSE;
    }

    // 定时器已经启动则让定时器去启动SIP
    if (m_iStartServiceTimer != -1)
    {
        return FALSE;
    }

    int iRet = sip_request_notify(mkGetHandle(), SIP_REQUEST_NOTIFY_ALL);
    COMMONUNITS_INFO("Init sip: request notify[%d]", iRet);
    int iDelayInitTime = GetInitSIPTime();
    static BOOL bInitService = FALSE;
    if (bInitService
            || iDelayInitTime <= 0)
    {
        iRet = StartSIP();

        COMMONUNITS_INFO("Init sip: start service[%d]", iRet);
    }
    else
    {
        COMMONUNITS_INFO("Delay %ds start service", iDelayInitTime);
        m_iStartServiceTimer = iDelayInitTime * 1000;
        SingleMsgReg(TM_TIMER, OnTimerMsg);
        timerSetThreadTimer((UINT)&m_iStartServiceTimer, m_iStartServiceTimer);
    }

    bInitService = TRUE;
    return TRUE;
}

// 获取延迟初始化sip的时间
int CSIPSender::GetInitSIPTime()
{
    int iDelayTime = configParser_GetConfigInt(kszDelayInitSipService);
    int iInitRandNum = configParser_GetConfigInt(kszNetworkRegSurgePrevention);

    if (iInitRandNum > 0)
    {
        FILE * fp = NULL;
        unsigned int rand_num = 0;

        fp = fopen("/dev/urandom", "r");
        if (fp != NULL)
        {
            int i;
            unsigned int r;
            for (i = 0; i < 512; i++)
            {
                if (fread(&r, 1, sizeof(r), fp) <= 0)
                {
                    break;
                }

                rand_num += r;
            }
            fclose(fp);
        }

        if (rand_num == 0)
        {
            struct timeval now_time;
            gettimeofday(&now_time, NULL);
            rand_num = now_time.tv_usec;
        }

        srand((unsigned) rand_num);
        iDelayTime += (rand() % iInitRandNum);
    }

    return iDelayTime;
}

// 处理定时器
BOOL CSIPSender::OnTimer(UINT uTimeID)
{
    BOOL bHandle = FALSE;

    if (uTimeID == (UINT)&m_iWaitAutopTimer)
    {
        m_iWaitAutopTimer = m_iWaitAutopTimer - 3;

        int iRet = msgSendMsgToThreadTimeoutEx(msgGetThreadByName(VP_AP_THREAD_NAME),
                                               ATP_MSG_REQ_GET_POWER_ON_STATUS, 0, 0, 0, NULL, 500);
        if (iRet == 1
                || m_iWaitAutopTimer <= 0)
        {
            m_iWaitAutopTimer = -1;
            timerKillThreadTimer(uTimeID);
            etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnTimerMsg);
            InitSIP();
        }
        bHandle = TRUE;
    }
    else if (uTimeID == (UINT)&m_iStartServiceTimer)
    {
        m_iStartServiceTimer = -1;
        timerKillThreadTimer(uTimeID);
        etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnTimerMsg);

        int iRet = StartSIP();
        COMMONUNITS_INFO("Init sip: start service by timer[%d]", iRet);
        bHandle = TRUE;
    }

    return bHandle;
}

bool CSIPSender::ReqKeyframe(int iAccountID, int iCallID)
{
    const char * pInfo =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?><media_control><vc_primitive><to_encoder><picture_fast_update/></to_encoder></vc_primitive></media_control>";

    return (sip_call_send_info_frame_sync(mkGetHandle(), iAccountID, iCallID, 0, (char *)pInfo) == 0);
}

// Draft BLA hold
BOOL CSIPSender::DraftBLAHold(int iAccountID, BOOL bPublic, int iCallId, BOOL bHold)
{
    COMMONUNITS_INFO("DraftBLAHold acc=[%d] pub=[%d] callid=[%d] hold=[%d]", iAccountID, bPublic,
                     iCallId, bHold);
    return bHold ? sip_call_draft_bla_public_hold(mkGetHandle(), iAccountID, iCallId) == 0
           : sip_call_draft_bla_public_resume(mkGetHandle(), iAccountID, iCallId) == 0;
}

// BLA占线与释放占线
BOOL CSIPSender::DraftBLASeizeLine(int iAccountID, int iCallID, BOOL bSeize, int iIndex)
{
    COMMONUNITS_INFO("DraftBLASeizeLine acc=[%d] callid=[%d] seize=[%d] index=[%d]", iAccountID,
                     iCallID, bSeize, iIndex);
    return bSeize ? 0 == sip_draft_bla_seize_line(mkGetHandle(), iAccountID, iIndex)
           : 0 == sip_draft_bla_unseize_line(mkGetHandle(), iAccountID, iIndex);
}

yl::string CSIPSender::GetVersion(const char * pKey)
{
    if (NULL == pKey
            || 0 == strlen(pKey))
    {
        return "";
    }

    char buf[1024] = {0};
    sip_service_get_version(mkGetHandle(), buf, sizeof(buf));

    yl::string strContent = buf;
    yl::string strSplit = "\r\n";
    yl::string::size_type nFound = strContent.find(strSplit);

    while (nFound != yl::string::npos)
    {
        yl::string strTmp = strContent.substr(0, nFound);
        yl::string::size_type nTmp = strTmp.find(":");
        if (nTmp != yl::string::npos && strTmp.substr(0, nTmp).compare(pKey) == 0)
        {
            return strTmp.substr(nTmp + 1);
        }

        strContent = strContent.substr(nFound + strSplit.length());
        nFound = strContent.find(strSplit);
    }

    return "";
}

yl::string CSIPSender::GetUserAgent(int iAccountID, int iCallId)
{
    char szUserAgent[128] = { 0 };
    sip_call_get_remote_user_agent(mkGetHandle(), iAccountID, iCallId, szUserAgent, 128);
    return szUserAgent;
}

bool CSIPSender::SrtpLifetimeReinvite(int iAccountID, int iCallID)
{
    COMMONUNITS_INFO("Srtp reinvite call[%d] accout[%d]", iCallID, iAccountID);
    return sip_call_srtp_lifetime_reinvite(mkGetHandle(), iAccountID, iCallID);
}

bool CSIPSender::ReInviteRespose(int iAccountID, int iCallID, int iTID, bool bAccept)
{
    int iCode = bAccept ? 200 : configParser_GetConfigInt(kszReinviteRespondIgnoreCode);
    COMMONUNITS_INFO("ReInviteRespose acc[%d] callid[%d] tid[%d] code[%d]", iAccountID, iCallID, iTID,
                     iCode);
    return sip_call_reinvite_response(mkGetHandle(), iAccountID, iCallID, iTID, iCode) == 0;
}

#ifdef  IF_FEATURE_EMERGENCY
int CSIPSender::EmergencyCallHangup(int iAccountID, int iCallID)
{
    COMMONUNITS_INFO("EmergencyCallHangup acc[%d] callid[%d]", iAccountID, iCallID);
    return sip_call_bw_emergency_hangup(mkGetHandle(), iAccountID, iCallID);
}
#endif

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
bool CSIPSender::BwCallRecord(int iAccountID, int iCallID, int option)
{
    return 0 == sip_bw_call_recording(mkGetHandle(), iAccountID, iCallID, option);
}
#endif

#if IF_FEATURE_GENBAND_E911
bool CSIPSender::SetLocationID(int iAccountID, int nLocationID)
{
    return 0 == sip_e911_set_location(mkGetHandle(), iAccountID, nLocationID);
}
#endif

bool CSIPSender::DeleteNetConfParty(int iAccountID, int iCallID, const char * pUserId)
{
    return 0 == sip_call_conference_del_user(mkGetHandle(), iAccountID, iCallID, pUserId);
}

