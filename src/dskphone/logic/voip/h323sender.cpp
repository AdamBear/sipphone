#ifdef IF_FEATURE_H323

#include "ETLLib.hpp"
#include "interfacedefine.h"
#include "h323sender.h"
#include "devicelib/networkserver.h"
#include "configparser/modconfigparser.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/systemhelperapi.h"
#include "configiddefine.h"
#include <ylstring.h>
#include <ylstringalgorithm.h>
#include "talk/fwd_dnd/include/fwd_dnd_def.h"
#include "broadsoft/callcenter/include/cccommon.h"
#include "talk/talklogic/include/talklogic_common.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "account/include/modaccount.h"
#include "service_h323_interface.h"
#include "service_h323_define.h"
#include "commonfunctions.h"

#define MAIN_STREAM 1
#define SHARE_STREAM 2

static LRESULT OnTimerMsg(msgObject & msg)
{
    if (TM_TIMER != msg.message)
    {
        return 0;
    }

    return _H323Sender.OnTimer(msg.wParam) ? 1 : 0;
}

IMPLEMENT_GETINSTANCE(CH323Sender)

CH323Sender::CH323Sender()
{
    m_iStartServiceTimer = -1;
    m_iWaitAutopTimer = configParser_GetConfigInt(kszWaitAutopTime);
    if (m_iWaitAutopTimer < 0 || m_iWaitAutopTimer > 3600)
    {
        m_iWaitAutopTimer = 0;
    }
}

CH323Sender::~CH323Sender()
{

}

//  查询账号状态
int CH323Sender::QueryLineState(int iAccountID)
{
    return h323_query_reg_status(mkGetHandle());
}

int CH323Sender::RequestCallID()
{
    return h323_call_new_cid(mkGetHandle());
}

// 接起来电，eType代表接起方式
bool CH323Sender::AcceptCall(int iAccountID, int iCallId, int iType, bool bSupportH264,
                             bool bMainStreamLimit)
{
    COMMONUNITS_INFO("Accept call id[%d] type[%d]", iCallId, iType);
    // 设置带宽
    h323_call_set_bandwidth(mkGetHandle(), iCallId, 2 * 1024 * 1024);
    if (iType != 1)
    {
        // 关闭视频
        h323_call_set_video_media(mkGetHandle(), iCallId, false);
    }
    else
    {
        // 暂定不需要配置
        // 配置视频参数
//      h323_ext_params_t* ext_param;
//      h323_ext_params_init(&ext_param);
//
//
//      h323_call_set_call_param();
//
//      h323_ext_params_free(&ext_param);

        if (bSupportH264)
        {
            // 辅流要设置参数，限制最大fps为15
            SetVideoH264Param(iAccountID, iCallId, bMainStreamLimit ? VideoSizeInfo(1280, 720,
                              30) : VideoSizeInfo(1920, 1080, 30),
                              VideoSizeInfo(1920, 1080, 15), (SHARE_STREAM | MAIN_STREAM));
        }
    }

    // 接听通话
    h323_call_accept(mkGetHandle(), iCallId);
    return true;
}

// 拒接来电
bool CH323Sender::RefuseCall(int iAccountID, int iCallId, int iRefuseCode)
{
    COMMONUNITS_INFO("Refuse call [%d]", iRefuseCode);
    h323_call_reject(mkGetHandle(), iCallId);
    return true;
}

bool CH323Sender::SetVideoH264Param(int iAccountID, int iCallId, const VideoSizeInfo & videoSize,
                                    const VideoSizeInfo & extvideoSize, int iType)
{
    COMMONUNITS_INFO("set h264 w[%d] h[%d] fps[%d] type[%d]", videoSize.w, videoSize.h, videoSize.fps,
                     iType);
    COMMONUNITS_INFO("set ext h264 w[%d] h[%d] fps[%d] type[%d]", extvideoSize.w, extvideoSize.h,
                     extvideoSize.fps, iType);

    // 接听来电也设置是否要支持1080p
    h323_ext_params_t * params;
    if (0 != h323_ext_params_init(&params))
    {
        return false;
    }

    if (MAIN_STREAM & iType)
    {
        //主流参数
        h323_h264_param_t h264_param;
        set_h264_param(h264_param, videoSize.w, videoSize.h, videoSize.fps, false);
        h323_ext_params_add(&params, H323_MAKE_CALL_H264_PARAM, &h264_param, sizeof(h323_h264_param_t));
    }
    if (SHARE_STREAM & iType)
    {
        //辅流参数
        h323_h264_param_t ext_h264_param;
        set_h264_param(ext_h264_param, extvideoSize.w, extvideoSize.h, extvideoSize.fps, false);
        h323_ext_params_add(&params, H323_MAKE_CALL_EXT_H264_PARAM, &ext_h264_param,
                            sizeof(h323_h264_param_t));
    }
    // 辅流禁用
    //int bDisable = true;
    //sip_ext_params_add(ext_param, SIP_MAKE_CALL_DISABLE_BFCP, &bDisable, sizeof(int));

    h323_call_set_call_param(mkGetHandle(), iCallId, params);

    h323_ext_params_free(&params);
    return true;
}

yl::string CH323Sender::GetUserAgent(int iAccountID, int iCallId)
{
    char szUserAgent[128] = { 0 };
    h323_call_get_remote_useragent(mkGetHandle(), iCallId, szUserAgent, 128);
    return szUserAgent;
}

bool CH323Sender::CallNegoH264(int iAccountID, int iCallId, const VideoSizeInfo & videoSize,
                               const VideoSizeInfo & extvideoSize, int iType, bool bNegoNow)
{
    SetVideoH264Param(iAccountID, iCallId, videoSize, extvideoSize, iType);

    if (bNegoNow)
    {
        h323_call_renegotiate(mkGetHandle(), iCallId);
    }
    return true;
}

bool CH323Sender::CallInvite(CallOutInfo * pCallInfo, bool bSupportH264)
{
    if (NULL == pCallInfo)
    {
        return false;
    }

//  COMMONUNITS_INFO("H323 invite [%s]", pCallInfo->m_strNumber.c_str());
    h323_ext_params_t * ext_param;

    if (0 != h323_ext_params_init(&ext_param))
    {
        return false;
    }

    // 呼出带宽
    int iBandwidth = configParser_GetConfigInt(kszOutgoingBandwidth) * 1024;
    if (iBandwidth <= 0)
    {
        iBandwidth = 1 * 1280 * 1024;
    }

    // 设置带宽
    h323_ext_params_add(&ext_param, H323_MAKE_CALL_VIDEO_BANDWIDTH, &iBandwidth, sizeof(int));

    //设置视频参数，主叫时，暂时将对方默认识别为非Yealink设备
    //Yealink设备在接通后会进行重协商
    if (pCallInfo->m_iMask & SIP_CALL_MASK_VIDEO)
    {
        // 支持H264才设置参数，H263的由sip读取配置
        if (bSupportH264)
        {
            // 有辅流要限制主流的分辨率
            h323_h264_param_t h264_param;
            set_h264_param(h264_param, 1280, 720, 30, true);
            h323_ext_params_add(&ext_param, H323_MAKE_CALL_H264_PARAM, &h264_param, sizeof(h323_h264_param_t));

            // 辅流要设置参数，限制最大fps为15
            h323_h264_param_t ext_h264_param;
            set_h264_param(ext_h264_param, 1920, 1080, 15, true);
            h323_ext_params_add(&ext_param, H323_MAKE_CALL_EXT_H264_PARAM, &ext_h264_param,
                                sizeof(h323_h264_param_t));
        }

    }

    // 设置callmask
    h323_call_type_e maskCallType = (pCallInfo->m_iMask & SIP_CALL_MASK_VIDEO) ? H323_VIDEO_CALL :
                                    H323_AUDIO_CALL;
    h323_ext_params_add(&ext_param, H323_MAKE_CALL_MASK, &maskCallType, sizeof(h323_call_type_e));

    yl::string strURI = pCallInfo->m_strNumber;

    // 格式化呼出号码
    // 格式化呼出号码(ip##id)
    yl::string strExtNumber = pCallInfo->m_strExtNumber;
    // h323协议层已支持xxx@xxx格式，此部分由底层处理
    // 底层已经不处理h323：开头，因此GUI去除
    if (strURI.size() > 5
            && strURI.find("h323:", 0, 5) != yl::string::npos)
    {
        strURI = strURI.substr(5);
    }

    if (!strExtNumber.empty()
            && !strURI.empty())
    {
        strURI = strURI + "##" + strExtNumber;
    }
    COMMONUNITS_INFO("H323 invite [%s]", strURI.c_str());

    // 设置通话目标
    h323_ext_params_add(&ext_param, H323_MAKE_CALL_CALLEE, (char *)strURI.c_str(), (strURI.size() + 1));

    // 呼出
    int nResult = h323_call_invite2(mkGetHandle(), pCallInfo->m_iCallID, ext_param);

    h323_ext_params_free(&ext_param);

    return true;
}

// 通话挂断
bool CH323Sender::CallHangUp(int iAccountID, int iCallId, int lParam)
{
    h323_call_hangup(mkGetHandle(), iCallId);
    return true;
}

// 设置通话Hold，bHold代表是否Hold对方
bool CH323Sender::SetHold(int iAccountID, int iCallId, bool bHold, bool bNeedNego)
{
    if (!bHold && bNeedNego)
    {
        // H323 解hold的时候，发送重协商请求
        h323_call_renegotiate(mkGetHandle(), iCallId);
    }

    COMMONAPI_INFO("H323 protocol hold [%d]", bHold);
    return false;
}

// 给Sip发送DTMF按键消息,iKey为按键键值
bool CH323Sender::SendDTMF(int iAccountID, int iCallID, int iKey)
{
//  if (iKey == PHONE_KEY_STAR)
//  {
//      iKey = 10;
//  }
//  else if (iKey == PHONE_KEY_POUND)
//  {
//      iKey = 11;
//  }
//  else if (iKey >= PHONE_KEY_0 && iKey <= PHONE_KEY_9)
//  {
//      iKey -= PHONE_KEY_0;
//  }
//  else if (iKey >= PHONE_KEY_A && iKey <= PHONE_KEY_D)
//  {
//      iKey = iKey - PHONE_KEY_A + 12;
//  }
//  else
//  {
//      return FALSE;
//  }

    char szBuf[16] = {0};
    sprintf(szBuf, "%c", iKey);
    COMMONAPI_INFO("H323 send dtmf[%d][%c]", iKey, iKey);

    return 0 == h323_call_send_dtmf(mkGetHandle(), iCallID, szBuf);
}

bool CH323Sender::MuteCall(int iAccountID, int iCallId, bool bMute)
{
    COMMONAPI_INFO("H323 mute [%d]", bMute);

    bool bRet = false;
    if (bMute)
    {
        bRet = (h323_call_local_mute(mkGetHandle(), iCallId) == 0);
    }
    else
    {
        bRet = (h323_call_local_un_mute(mkGetHandle(), iCallId) == 0);
    }

    return bRet;
}

// 来电时响应SIP
bool CH323Sender::ResponseIncoming(int iAccountID, int iCallID, int iResponseCode)
{
    COMMONUNITS_INFO("H323 Response Incoming [%d]", iCallID);
    return 0 == h323_call_ringing(mkGetHandle(), iCallID);
}

// bool CH323Sender::GetCallInfo(int iAccountID, int iCallID, sip_call_proto_info2_t* lpszData)
// {
//  // 新版本的SIP, 使用该接口
//  if (strcmp(GetVersion("Interface").c_str(), "1.0.0.0") > 0)
//  {
//      return (0 == sip_call_get_info2(mkGetHandle(), iAccountID, iCallID, lpszData, sizeof(lpszData)));
//  }
//
//  // 旧版本的SIP, 使用该接口
//  sip_call_proto_info_t info;
//  if (0 == sip_call_get_info(mkGetHandle(), iAccountID, iCallID, &info))
//  {
//      strcpy(lpszData->call_id, info.call_id);
//      strcpy(lpszData->local_tag, info.local_tag);
//      strcpy(lpszData->remote_tag, info.remote_tag);
//      memset(lpszData->from, 0, sizeof(lpszData->from));
//      memset(lpszData->to, 0, sizeof(lpszData->to));
//      return true;
//  }
//
//  return false;
// }

// bool CH323Sender::IsSupportVideo()
// {
//  if (devicelib_GetPhoneType() == PT_T49
//      && configParser_GetConfigInt(kszVideoTalkEnable) != 0)
//  {
//      return true;
//  }
//  return false;
// }

// 启动H323 Server
int CH323Sender::StartH323()
{
    COMMONUNITS_INFO("Start h323 service");

    int iRet = h323_service_start(mkGetHandle());

    int iValue = IsSupportVideo() ? 1 : 0;
    SetOption(SIP_SERVICE_OPT_VIDEO_CAP, (void *) &iValue, sizeof(int));
    return iRet;
}

int CH323Sender::StopH323()
{
    COMMONUNITS_INFO("Stop h323 service");
    int iRet = h323_service_stop(mkGetHandle());
    return iRet;
}

int CH323Sender::SwitchH323()
{
    // H323 service支持多次开启
    if (acc_IsCloudEnable()
            && configParser_GetConfigInt(kszH323ProtocolSwitch) != 0)
    {
        return StartH323();
    }

    return StopH323();
}

bool CH323Sender::IsSupportVideo()
{
    // 暂时拷贝sip接口：此部分sip与h323可能不完全一致
    if (devicelib_GetPhoneType() == PT_T49
            && configParser_GetConfigInt(kszVideoTalkEnable) != 0)
    {
        return true;
    }
    return false;
}

int CH323Sender::SetOption(int option, void * value, unsigned int size)
{
    return h323_service_set_option(mkGetHandle(), option, value, size);
}

// int CH323Sender::SetVideoCallMedia(int iAccountID, int iCallID, bool bVideo)
// {
//  return sip_call_set_video_media(mkGetHandle(), iAccountID, iCallID, bVideo ? 1 : 0);
// }

// 初始化H323Server
bool CH323Sender::InitH323()
{
    if (mkservice_get_data(mkGetHandle(), MKSRV_ID_H323, 0) <= 0)
    {
        COMMONUNITS_INFO("Init h323 not running");
        return false;
    }

    if (m_iWaitAutopTimer > 0)
    {
        COMMONUNITS_INFO("wait autop [%d]", m_iWaitAutopTimer);
        SingleMsgReg(TM_TIMER, OnTimerMsg);
        // 3s检测一次等待autop更新完成
        timerSetThreadTimer((UINT)&m_iWaitAutopTimer, 3000 + 1);
        return false;
    }

    // 定时器已经启动则让定时器去启动
    if (m_iStartServiceTimer != -1)
    {
        return false;
    }

    int iRet = h323_request_notify(mkGetHandle(), H323_REQUEST_NOTIFY_ALL);
    COMMONUNITS_INFO("Init h323: request notify[%d]", iRet);
    int iDelayInitTime = configParser_GetConfigInt(kszNetworkRegSurgePrevention);
    static bool bInitService = false;
    if (bInitService
            || iDelayInitTime <= 0)
    {
        iRet = SwitchH323();

        COMMONUNITS_INFO("Init h323: start service[%d]", iRet);
    }
    else
    {
        COMMONUNITS_INFO("Delay %ds start service", iDelayInitTime);
        m_iStartServiceTimer = iDelayInitTime * 1000;
        SingleMsgReg(TM_TIMER, OnTimerMsg);
        timerSetThreadTimer((UINT)&m_iStartServiceTimer, m_iStartServiceTimer);
    }

    bInitService = true;
    return TRUE;
}

// 处理定时器
bool CH323Sender::OnTimer(UINT uTimeID)
{
    bool bHandle = FALSE;

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

            InitH323();
        }
        bHandle = TRUE;
    }
    else if (uTimeID == (UINT)&m_iStartServiceTimer)
    {
        m_iStartServiceTimer = -1;
        timerKillThreadTimer(uTimeID);
        etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnTimerMsg);

        int iRet = StartH323();
        COMMONUNITS_INFO("Init h323: start service by timer[%d]", iRet);
        bHandle = TRUE;
    }

    return bHandle;
}

bool CH323Sender::ReqKeyframe(int iAccountID, int iCallID, bool bShare)
{
    return (h323_call_video_frame_sync(mkGetHandle(), iCallID, bShare) == 0);
}

bool CH323Sender::ReqBitrate(int iAccountID, int iCallID, int iBandwidth, bool bShare)
{
    h323_flow_ctrl_para_t flowPara;
    flowPara.sid = bShare ? H323_SESSION_ID_EXT_VIDEO : H323_SESSION_ID_VIDEO;
    flowPara.bitrate = iBandwidth;
    return (h323_call_flow_ctrl_rate(mkGetHandle(), iCallID, &flowPara) == 0);
}

int CH323Sender::SetCallMedia(int iAccountID, int iCallID, bool bVideo)
{
    return (h323_call_set_video_media(mkGetHandle(), iCallID, bVideo ? 1 : 0) == 0);
}

#endif //#ifdef IF_FEATURE_H323

