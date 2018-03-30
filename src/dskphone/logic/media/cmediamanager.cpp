///////////////////////////////////////////////////////////
//  CMediaManager.cpp
//  Implementation of the Class CMediaManager
//  Created on:      06-二月-2015 10:10:26
//  Original author: huangwz
///////////////////////////////////////////////////////////

#ifdef _T49
#include "cmediamanager.h"
#include "caudiomanager.h"
#include "talk/talklogic/include/talklogic_common.h"
#include "dsklog/log.h"
#include "devicelib/phonedevice.h"
#include "service_ipvp.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "hotplug/commondefine.h"
#include "hotplug/modhotplug.h"
#include "record/include/recordmessagedefine.h"
#include "talk/talklogic/include/callinfo.h"
#include "talk/talklogic/include/talkmsgdefine.h"

LRESULT CMediaManager::OnMessage(msgObject & objMsg)
{
    // 测试使用
    //VOICE_INFO("Media receive msg [%d] w[%d] l[%d]", objMsg.message, objMsg.wParam, objMsg.lParam);

    if (DSK_MSG_GET_MEDIA_STATS == objMsg.message)
    {
        if (objMsg.lParam == 0)
        {
            TalkStatisticsInfo info;
            memset(&info, 0, sizeof(TalkStatisticsInfo));
            _MediaManager.GetTalkStatistics(info);
            objMsg.ReplyMessageEx(1, sizeof(TalkStatisticsInfo), (void *)&info);
        }
        else if (objMsg.lParam == 1)
        {
            MediaStatsInfo info;
            _MediaManager.GetStats(objMsg.wParam, info);
            objMsg.ReplyMessageEx(1, sizeof(MediaStatsInfo), (void *)&info);
        }
        return 1;
    }
    else if (TM_TIMER == objMsg.message)
    {
        return _MediaManager.OnTimer(objMsg.wParam);
    }

    if (VOICE_VPM_START == objMsg.message)
    {
        // vpm加载完成，暂定此处处理，后续音频加入再调整
        // 同时往下发初始化完成的事件
        _MediaManager.OnIpvpStarted();
    }

    MediaEventData data;
    if (_MediaManager.GetEventData(objMsg, data))
    {
        _MediaManager.OnEvent(&data);
    }

    return 1;
}

IMPLEMENT_GETINSTANCE(CMediaManager)

CMediaManager::CMediaManager()
{
    m_iDspMode = IPVP_POWER_INVAL;
    m_iTimerDelayAdjustDsp = 5 * 1000;
    m_iTimerUpdateStat = STOP_TIMER_VALUE;
}


CMediaManager::~CMediaManager()
{
    Uninit();
}


void CMediaManager::Init()
{
    CBaseMedia * pAudio = new CAudioManager;
    if (NULL != pAudio)
    {
        m_listProcessor.push_back(pAudio);
    }

#ifdef IF_SUPPORT_VIDEO
    // 加入视频
    if (IsSupportVideo())
    {
        CBaseMedia * pVideo = new CVideoManager;
        if (NULL != pVideo)
        {
            m_listProcessor.push_back(pVideo);
        }
    }
#endif //IF_SUPPORT_VIDEO

    RangeMsgReg(TALK_MESSAGE_BEGIN, TALK_MESSAGE_END, &CMediaManager::OnMessage);

    SingleMsgReg(VOICE_VPM_START, &CMediaManager::OnMessage);
    SingleMsgReg(DSK_MSG_GET_MEDIA_STATS, &CMediaManager::OnMessage);
    SingleMsgReg(CAMERA_MSG_STATUS_CHANGE, &CMediaManager::OnMessage);
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, &CMediaManager::OnMessage);

    SingleMsgReg(MEDIA_MSG_CAMERA_UPDATE, &CMediaManager::OnMessage);

    SingleMsgReg(TM_TIMER, &CMediaManager::OnMessage);

    // 监听ipvp的消息
    SingleMsgReg(IPVP_MSG_VIDEO_RREQ_KFRAME, &CMediaManager::OnMessage);

    SingleMsgReg(IPVP_MSG_VIDEO_RREQ_FCTRL, &CMediaManager::OnMessage);

    SingleMsgReg(IPVP_MSG_VIDEO_RECV_RESIZED, &CMediaManager::OnMessage);

    //
    //SingleMsgReg(IPVP_MSG_AUDIO_SRTP_REINVITE, &CMediaManager::OnMessage);

}

void CMediaManager::Uninit()
{
    MediaList::iterator it = m_listProcessor.begin();
    while (it != m_listProcessor.end())
    {
        if (NULL != *it)
        {
            delete *it;
            it = m_listProcessor.erase(it);
            continue;
        }
        ++it;
    }

    RangeMsgUnReg(TALK_MESSAGE_BEGIN, TALK_MESSAGE_END, &CMediaManager::OnMessage);
    SingleMsgUnReg(VOICE_VPM_START, &CMediaManager::OnMessage);
    SingleMsgUnReg(DSK_MSG_GET_MEDIA_STATS, &CMediaManager::OnMessage);
    SingleMsgUnReg(CAMERA_MSG_STATUS_CHANGE, &CMediaManager::OnMessage);
    SingleMsgUnReg(CONFIG_MSG_BCAST_CHANGED, &CMediaManager::OnMessage);

    SingleMsgUnReg(TM_TIMER, &CMediaManager::OnMessage);
    SingleMsgUnReg(IPVP_MSG_VIDEO_RREQ_KFRAME, &CMediaManager::OnMessage);
    SingleMsgUnReg(IPVP_MSG_VIDEO_RREQ_FCTRL, &CMediaManager::OnMessage);
    SingleMsgUnReg(IPVP_MSG_VIDEO_RECV_RESIZED, &CMediaManager::OnMessage);
    //SingleMsgUnReg(IPVP_MSG_AUDIO_SRTP_REINVITE, &CMediaManager::OnMessage);
}

// 定时器
bool CMediaManager::OnTimer(UINT uTimeID)
{
    bool bHandle = true;
    if (uTimeID == (UINT)&m_iTimerDelayAdjustDsp)
    {
        VOICE_INFO("media delay adjust dsp time over");
        timerKillThreadTimer((UINT)&m_iTimerDelayAdjustDsp);
        AdjustDspPower(false);
    }
    else if (uTimeID == (UINT)&m_iTimerUpdateStat)
    {
        UpdateStat();
    }
    else
    {
        bHandle = false;
    }

    return bHandle;
}

bool CMediaManager::GetEventData(msgObject & objMsg, MediaEventData & data)
{
    bool bHandled = true;
    switch (objMsg.message)
    {
    case TALK_MSG_RINGING_ENTER:
    case TALK_MSG_RINGING_EXIT:
    {
        data.eType = MET_INCOMING;
        data.wEventPara = objMsg.wParam;
    }
    break;
    case TALK_MSG_TALK_START:
    {
        data.eType = MET_TALK_START;
        data.wEventPara = talklogic_GetCallIdBySessionID(objMsg.wParam);
    }
    break;
    case TALK_MSG_TALK_STOP:
    {
        data.eType = MET_TALK_STOP;
        data.wEventPara = talklogic_GetCallIdBySessionID(objMsg.wParam);
    }
    break;
    case TALK_MSG_TALK_RESET:
    {
        data.eType = MET_TALK_RESET;
        data.wEventPara = talklogic_GetCallIdBySessionID(objMsg.wParam);
        data.lEventPara = 0;
    }
    break;
    case TALK_MSG_TALK_HOLD:
    {
        data.eType = MET_TALK_HOLD;
        data.wEventPara = objMsg.wParam;
        //data.pExt = objMsg.GetExtraData();
    }
    break;
    case TALK_MSG_VIDEO_MUTE:
    {
        data.eType = MET_TALK_VIDEO_MUTE;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
    }
    break;
    case TALK_MSG_STOP_VIDEO:
    {
        data.eType = MET_TALK_STOP_VIDEO;
        data.wEventPara = objMsg.wParam;
    }
    break;
    case TALK_MSG_VIDEO_KEY_FRAME_REQUEST:
    {
        data.eType = MET_TALK_VIDEO_KEYFRAME_SEND;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
        data.pExt = objMsg.GetExtraData();
    }
    break;
    case TALK_MSG_TALK_HIDE:
    {
        data.eType = MET_TALK_HIDE;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
    }
    break;
    case CAMERA_MSG_STATUS_CHANGE:
    {
        data.eType = MET_CAMERA_CHANGE;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
    }
    break;
    case IPVP_MSG_VIDEO_RREQ_KFRAME:
    {
        data.eType = MET_TALK_VIDEO_KEYFRAME_REQ;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
    }
    break;
    case IPVP_MSG_VIDEO_RREQ_FCTRL:
    {
        data.eType = MET_TALK_VIDEO_FCTRL_REQ;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
        data.pExt = objMsg.GetExtraData();
    }
    break;
    case VOICE_VPM_START:
    {
        data.eType = MET_IPVP_START;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
    }
    break;
    case CONFIG_MSG_BCAST_CHANGED:
    {
        data.eType = MET_CONFIG_CHANGE;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
    }
    break;
    case MEDIA_MSG_CAMERA_UPDATE:
    {
        data.eType = MET_CAMERA_UPDATE;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
    }
    break;
    case TALK_MSG_CONF_START:
    {
        data.eType = MET_CONF_START;
        data.wEventPara = objMsg.wParam;
    }
    break;
    case TALK_MSG_CONF_STOP:
    {
        data.eType = MET_CONF_STOP;
        data.wEventPara = objMsg.wParam;
    }
    break;
    case TALK_MSG_SESSION_DESTORY:
    {
        data.eType = MET_TALK_DESTORY;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
    }
    break;
    case TALK_MSG_SHARE_OPEN:
    {
        data.eType = MET_SHARE_OPEN;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
    }
    break;
    case TALK_MSG_SHARE_START:
    {
        data.eType = MET_SHARE_START;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
    }
    break;
    case TALK_MSG_SHARE_STOP:
    {
        data.eType = MET_SHARE_STOP;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
    }
    break;
    case TALK_MSG_BITRATE_RESET:
    {
        data.eType = MET_BITRATE_RESET;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
        data.pExt = objMsg.GetExtraData();
    }
    break;
    case IPVP_MSG_AUDIO_SRTP_REINVITE:
    {
        data.eType = MET_TALK_AUDIO_SRTP_REQ;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
        data.pExt = objMsg.GetExtraData();
    }
    break;
    case IPVP_MSG_VIDEO_RECV_RESIZED:
    {
        data.eType = MET_TALK_VIDEO_RECV_RESIZED;
        data.wEventPara = objMsg.wParam;
        data.lEventPara = objMsg.lParam;
        data.pExt = objMsg.GetExtraData();
    }
    break;
    default:
        bHandled = false;
        break;
    }

    return bHandled;
}

bool CMediaManager::OnEvent(MediaEventData * pData)
{
    if (pData == NULL
            || pData->eType == MET_NONE)
    {
        return false;
    }

    // 开启、停止通话，需要更新dsp运行模式
    if (pData->eType == MET_TALK_START
            || pData->eType == MET_TALK_RESET)
    {
        AdjustDspPower(true, pData->wEventPara);
        // 保证模式调整完成；暂定不需要延迟
        //etlSleep(50);
    }

    bool bHandled = false;
    for (MediaList::iterator it = m_listProcessor.begin(); it != m_listProcessor.end(); ++it)
    {
        CBaseMedia * pProcessor = *it;
        if (NULL != pProcessor)
        {
            bHandled |= pProcessor->OnEvent(pData);
        }
    }

    // 开启、停止通话，需要更新系统运行频率
    if (MET_TALK_STOP == pData->eType)
    {
        // 保证音视频已经停止结束;暂定延迟关闭
        timerSetThreadTimer((UINT)&m_iTimerDelayAdjustDsp, m_iTimerDelayAdjustDsp);
    }
    else if (MET_TALK_START == pData->eType)
    {
        SetUpdateStatTimer();
    }
    else if (MET_TALK_DESTORY == pData->eType)
    {
        //不存在视频话路了再去设置standby
        if (!talklogic_IsVideoSessionExist())
        {
            modhotplug_CtrlCamera(false, false);
        }

        //不存在话路了需要停止检测丢包率
        if (!talklogic_SessionExist())
        {
            CancleUpdateStatTimer();
        }
    }

    return bHandled;
}

bool CMediaManager::SetVideoLayout(DisplayMonitor & display)
{
    m_displayLayout = display;

    MediaEventData data;
    data.eType = MET_LAYOUT_CHANGE;
    data.pExt = &display;
    OnEvent(&data);
    return true;
}

bool CMediaManager::SetVideoLayout()
{
    MediaEventData data;
    data.eType = MET_LAYOUT_CHANGE;
    data.pExt = &m_displayLayout;
    OnEvent(&data);
    return true;
}

bool CMediaManager::PreviewVideo(DisplayMonitor & display, bool bOpen)
{
    MediaEventData data;
    data.eType = MET_PREVIEW_VIDEO;
    data.wEventPara = bOpen ? 1 : 0;
    data.pExt = &display;
    OnEvent(&data);
    return true;
}

bool CMediaManager::IsSupportVideo()
{
    if (devicelib_GetPhoneType() == PT_T49)
    {
        return true;
    }
    return false;
}

// ipvp启动后设置能力级，调整dsp
void CMediaManager::OnIpvpStarted()
{
    if (IsSupportVideo())
    {
        ipvp_capset_t capsetInfo;
        memset(&capsetInfo, 0, sizeof(ipvp_capset_t));

#ifdef _T49
        ipvp_get_capset(mkGetHandle(), &capsetInfo);
#endif
        VOICE_INFO("payload[%d] codec[%s] clock[%d] capacity[%s]", capsetInfo.payload_type,
                   capsetInfo.codec_name, capsetInfo.clock_rate, capsetInfo.capacity_string);

        // TODO 将capset写入到sipaccount中
        int iAccountNum = acc_AccountNum();
        for (int i = 0; i <= iAccountNum; i++)
        {
            yl::string strCapset = configParser_GetCfgItemStringValue(kszAccountMediaCapset, i);
            if (strCapset.compare(capsetInfo.capacity_string) != 0)
            {
                configParser_SetCfgItemStringValue(kszAccountMediaCapset, i, capsetInfo.capacity_string);
                msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, i);
            }
        }

#ifdef IF_FEATURE_H323
        // H323 私有能力集：从vmp读取，写到sip，sip协商使用；
        yl::string strCapset = configParser_GetConfigString(kszAccountH323MediaCapset);
        if (strCapset.compare(capsetInfo.capacity_string) != 0)
        {
            configParser_SetConfigString(kszAccountH323MediaCapset, capsetInfo.capacity_string);
            msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_H323LINE, 0);
        }
#endif //IF_FEATURE_H323

        AdjustDspPower();
    }

    return;
}


/************************************************************************/
/*
#define  IPVP_POWER_FULL        0 (全速模式，当前设计为MCU或者1080P分辨率通话)
#define  IPVP_POWER_SAVING      1 (一般省电，当前设计为Idle状态或单路720P分辨率通话)
#define  IPVP_POWER_DEEP        2 (深度省电，当前设计为待机并关闭显示输出)
#define  IPVP_POWER_SLEEP       3 (睡眠模式, 当前设计与深度省电完全等效)
*/
/************************************************************************/

/*
// 策略调整
1、要保证在第一路音频通话的开始前要保证完成power的设置为0，即full power
2、要保证在音频通话结束后再调整为power的设置为1，即power saving
3、因为音频和视频的power为同一个接口，因此音频或者视频都是做同样的处理
4、要保证在使用视频之前，开启摄像头打开，
包括预览和视频通话开始，单纯音频通话除外（这个要能保证，因为开启摄像头的功耗比较大，不需要时一定要保证关闭），
5、要在通话或者预览结束后再关闭摄像头，这个地方应该尽量避免没有必要的频繁开关。
6、延迟关闭power和摄像头的动作在安全可靠的情况下可以加进去，如果不能保证可靠则可以先不做，因为目前没有明显证据说明有相关性。
但如果要加入延迟关闭的话时间不宜设置过长，控制在5-10秒即可；也就是放置快速的切换导致反复开关就可以了。
*/

// 调整音视频芯片运行状态
void CMediaManager::AdjustDspPower(bool bStart/* = false*/, int iCallId/* = -1*/)
{
//  VOICE_INFO("media adjust dsp power[%d]", iCallId);
//  bool bExist1080p = false;
//  if (bStart)
//  {
//      // 开启通话先计算当前通话是否是1080p
//      CCallInfo *pInfo = talklogic_FindCallInfoByCallId(iCallId);
//      if (NULL != pInfo
//          && pInfo->IsVideoTalk())
//      {
//          // 解码分辨率
//          VideoResolutionParam resolution;
//          MediaComm_GetMaxVideoResolution(resolution, pInfo->m_tVPMParam.video, pInfo->m_stVideoExt,
//              pInfo->m_stMediaInfo.video, false);
//
//          bExist1080p = (resolution.m_width >= 1280 && resolution.m_height > 720)
//              || (resolution.m_width > 1280 && resolution.m_height >= 720);
//
//          VOICE_INFO("decode 1080[%d]", bExist1080p);
//
//          // 编码分辨率
//          if (!bExist1080p)
//          {
//              MediaComm_GetMaxVideoResolution(resolution, pInfo->m_tVPMParam.video, pInfo->m_stVideoExt,
//                  pInfo->m_stMediaInfo.video, true);
//
//              bExist1080p = (resolution.m_width >= 1280 && resolution.m_height > 720)
//                  || (resolution.m_width > 1280 && resolution.m_height >= 720);
//          }
//
//          VOICE_INFO("encode 1080[%d]", bExist1080p);
//      }
//  }
//
//  if (!bExist1080p)
//  {
//      // 不存在再到videomanager查找一次
//      for (MediaList::iterator it = m_listProcessor.begin(); it != m_listProcessor.end(); ++it)
//      {
//          CBaseMedia *pProcessor = *it;
//          CVideoManager *pVideo = dynamic_cast<CVideoManager *>(pProcessor);
//          if (NULL != pVideo)
//          {
//              bExist1080p = pVideo->IsVideoExist(VST_1080P);
//          }
//      }
//  }

    // 以下暂时从通话获取数据
    int iCallNum = talklogic_GetTalkingNumber();

//  int iVideoCallNum = talklogic_GetTalkingNumber(true);

    int iStatus = m_iDspMode;
    if (bStart || iCallNum > 0)
    {
        iStatus = IPVP_POWER_FULL;
    }
    else
    {
        iStatus = IPVP_POWER_SAVING;
    }

    if (iStatus != m_iDspMode)
    {
        m_iDspMode = iStatus;
#ifdef _T49
        ipvp_set_power(mkGetHandle(), m_iDspMode);
#endif
    }

    VOICE_INFO("Dsp run type[%d] talk[%d] start[%d]", m_iDspMode, iCallNum, bStart);
}

bool CMediaManager::GetStats(int iCallId, MediaStatsInfo & statsInfo)
{
    // 先清空数据
    memset(&statsInfo, 0, sizeof(MediaStatsInfo));

    VOICE_INFO("Get stats [%d]", iCallId);
    // 取通话信息，如果查找不到，则使用焦点话路，焦点如果是会议，取第一路
    CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoByCallId(iCallId);
    if (NULL == pCallInfo)
    {
        int iFocuseSession = talklogic_GetFocusedSessionID();

        VOICE_INFO("Not found call info, return focuse session");
        pCallInfo = (CCallInfo *)talklogic_GetCallInfoBySessionID(iFocuseSession);
    }

    if (NULL == pCallInfo)
    {
        statsInfo.iCallId = -1;

        VOICE_INFO("Get talk statistic error, not found call info");
        return false;
    }

    statsInfo.iCallId = pCallInfo->GetCallID();

    // V80 sp2新增
    statsInfo.iProtocol = (acc_GetAccountProtocol(pCallInfo->GetAccountID()) == PROTOCOL_H323 ?
                           DSK_PROTOCOL_H323 : DSK_PROTOCOL_SIP);
    strncpy(statsInfo.szUserAgent, pCallInfo->m_strUserAgent.c_str(),
            chMIN(pCallInfo->m_strUserAgent.size(), COMMON_STRING_SIZE - 1));

    MediaEventData data;
    data.eType = MET_STATS_INFO;
    data.wEventPara = pCallInfo->GetCallID();
    data.pExt = &statsInfo;
    return OnEvent(&data);
}

bool CMediaManager::ReduceVideoPower()
{
    MediaEventData data;
    data.eType = MET_ADJUST_POWER;
    data.wEventPara = 0;
    return OnEvent(&data);
}

bool CMediaManager::ResumeVideoPower()
{
    MediaEventData data;
    data.eType = MET_ADJUST_POWER;
    data.wEventPara = 1;
    return OnEvent(&data);
}

static const CCallInfo * GetCallInfoBySessionId(int nSessionId)
{
    SESSION_STATE eState = talklogic_GetSessionStateBySessionID(nSessionId);
    if (SESSION_LOCALCONF == eState
            || SESSION_BETRANSFERRED == eState
            || SESSION_DIALING == eState
            || SESSION_PREDIAL == eState
            || SESSION_PRETRAN == eState)
    {
        return NULL;
    }

    return (CCallInfo *)talklogic_GetCallInfoBySessionID(nSessionId);
}

bool CMediaManager::GetTalkStatistics(TalkStatisticsInfo & info)
{
    VOICE_INFO("Get talk statistics");
    // 获取所有通话
    YLVector<int> vecSessionId;
    talklogic_GetAllSessionID(vecSessionId, false);

    int iCount = 0;
    for (int i = 0; i < vecSessionId.size() && iCount < SESSIONSIZE; i++)
    {
        const CCallInfo * pCallInfo = GetCallInfoBySessionId(vecSessionId[i]);

        if (NULL == pCallInfo)
        {
            continue;
        }

        info.stSessionArr[iCount].iCallId = pCallInfo->GetCallID();
        strncpy(info.stSessionArr[iCount].szNumber, pCallInfo->m_tRemoteInfo.sSIPName.c_str(),
                chMIN(TALKNAMESIZE, pCallInfo->m_tRemoteInfo.sSIPName.size()));

        strncpy(info.stSessionArr[iCount].szDispName, pCallInfo->m_tRemoteInfo.sDisplayName.c_str(),
                chMIN(TALKNAMESIZE, pCallInfo->m_tRemoteInfo.sDisplayName.size()));

        strncpy(info.stSessionArr[iCount].szServer, pCallInfo->m_tRemoteInfo.sServerName.c_str(),
                chMIN(TALKNAMESIZE, pCallInfo->m_tRemoteInfo.sServerName.size()));

        VOICE_INFO("Session [%d][%d] number[%s] dis[%s], server[%s]", iCount, pCallInfo->GetCallID(),
                   info.stSessionArr[iCount].szNumber, info.stSessionArr[iCount].szDispName,
                   info.stSessionArr[iCount].szServer);
        ++iCount;
    }

    info.iSessionSize = iCount;

    return true;
}

bool CMediaManager::GetTalkStatistics(YLList<SessionInfo> & listInfo)
{
    VOICE_INFO("Get talk statistics");
    listInfo.clear();
    // 获取所有通话
    YLVector<int> vecSessionId;
    talklogic_GetAllSessionID(vecSessionId, false);

    SessionInfo stSession;
    for (int i = 0; i < vecSessionId.size(); i++)
    {
        const CCallInfo * pCallInfo = GetCallInfoBySessionId(vecSessionId[i]);

        if (NULL == pCallInfo)
        {
            continue;
        }

        memset(&stSession, 0, sizeof(stSession));

        stSession.iCallId = pCallInfo->GetCallID();
        strncpy(stSession.szNumber, pCallInfo->m_tRemoteInfo.sSIPName.c_str(),
                chMIN(TALKNAMESIZE, pCallInfo->m_tRemoteInfo.sSIPName.size()));

        strncpy(stSession.szDispName, pCallInfo->m_tRemoteInfo.sDisplayName.c_str(),
                chMIN(TALKNAMESIZE, pCallInfo->m_tRemoteInfo.sDisplayName.size()));

        strncpy(stSession.szServer, pCallInfo->m_tRemoteInfo.sServerName.c_str(),
                chMIN(TALKNAMESIZE, pCallInfo->m_tRemoteInfo.sServerName.size()));

        VOICE_INFO("[%d] number[%s] dis[%s], server[%s]", pCallInfo->GetCallID(),
                   stSession.szNumber, stSession.szDispName, stSession.szServer);

        listInfo.push_back(stSession);
    }

    return true;
}

void CMediaManager::CancleUpdateStatTimer()
{
    if (m_iTimerUpdateStat == STOP_TIMER_VALUE)
    {
        return ;
    }

    timerKillThreadTimer((UINT)&m_iTimerUpdateStat);
    m_iTimerUpdateStat = STOP_TIMER_VALUE;
}

void CMediaManager::SetUpdateStatTimer()
{
    if (m_iTimerUpdateStat != STOP_TIMER_VALUE)
    {
        return ;
    }

    m_iTimerUpdateStat = 1 * 1000;

    timerSetThreadTimer((UINT)&m_iTimerUpdateStat, m_iTimerUpdateStat);
}

void CMediaManager::UpdateStat()
{
    MediaEventData data;
    data.eType = MET_STATS_UPDATE;
    OnEvent(&data);
}

CVideoManager * CMediaManager::GetVideoProcessor()
{
    for (MediaList::iterator it = m_listProcessor.begin(); it != m_listProcessor.end(); ++it)
    {
        CBaseMedia * pProcessor = *it;
        CVideoManager * pVideo = dynamic_cast<CVideoManager *>(pProcessor);
        if (NULL != pVideo)
        {
            return pVideo;
        }
    }

    return NULL;
}

bool CMediaManager::ProcessStats(void * pInfo, const MediaStatsInfo & mediaStat,
                                 bool bVideo /*= true*/)
{
    // todo 此功能需要整改，建议放到vpm那边实现
    if (NULL == pInfo)
    {
        return false;
    }

    bool & bLostFlac = bVideo ? ((VideoInfo *) pInfo)->bLostFlac : ((AudioInfo *) pInfo)->bLostFlac;
    int & iCount = bVideo ? ((VideoInfo *) pInfo)->iConut : ((AudioInfo *) pInfo)->iConut;
    int iCallID = bVideo ? ((VideoInfo *) pInfo)->iCallId : ((AudioInfo *) pInfo)->iCallId;
    unsigned int uLostFlac = bVideo ? mediaStat.video.SendLostFrac : mediaStat.audio.SendLostFrac;

    //TALK_INFO("IsNeedNotifyLostFlac: bVideo= [%d] uLostFlac=[%d], pVideoInfo->bLostFlac =[%d], pVideoInfo->iConut=[%d]", bVideo, uLostFlac, bLostFlac, iCount);

#define  MAX_LOST_FLAC  5        //丢包率
#define  MAX_LOST_COUNT 3        //连续丢包次数

    bool bNotify = false;
    if (uLostFlac >= MAX_LOST_FLAC)
    {
        // 非丢包模式，连续丢包率大于数值，则需要通知，否则只计数。
        if (!bLostFlac)
        {
            if (iCount >= MAX_LOST_COUNT)
            {
                bNotify = true;
            }
            else
            {
                iCount++;
            }
        }
        else
        {
            iCount = 0;
        }
    }
    else
    {
        // 丢包模式，丢包率连续小于数值，则需要通知网络变好
        if (bLostFlac)
        {
            if (iCount >= MAX_LOST_COUNT)
            {
                bNotify = true;
            }
            else
            {
                iCount++;
            }
        }
        else
        {
            iCount = 0;
        }
    }

    if (bNotify)
    {
        bLostFlac = !bLostFlac;
        iCount = 0;
        int iSessionID = talklogic_GetSessionIdByCallID(iCallID);

        //Notify
        TALK_INFO("Notify Session[%d] bLostFalc[%d]", iSessionID, bLostFlac);
        etl_NotifyApp(FALSE, TALK_MSG_UPDATESTATS, iSessionID, bLostFlac);
    }

    return bNotify;
}

bool CMediaManager::GetVideoResolution(int iCallId, VideoSizeInfo & stInfo)
{
    CVideoManager * pVideo = GetVideoProcessor();

    if (pVideo != NULL)
    {
        VideoResolutionParam stRes;
        pVideo->GetRealVideoResolution(iCallId, stRes);

        stInfo.h = stRes.m_height;
        stInfo.w = stRes.m_width;
        stInfo.fps = stRes.m_fps;
    }

    return true;
}

bool CMediaManager::SetDisplay(int iWidth, int iHeight, int iFps, int videoDST)
{
    CVideoManager * pVideo = GetVideoProcessor();
    if (pVideo)
    {
        return pVideo->SetDisplay(iWidth, iHeight, iFps, videoDST);
    }
    return false;
}

void CMediaManager::StartConf(const YLList<int> & listCallID)
{
    CVideoManager * pVideo = GetVideoProcessor();
    if (pVideo)
    {
        pVideo->StartConf(listCallID);
    }
}

void CMediaManager::StopConf(const YLList<int> & listCallID)
{
    CVideoManager * pVideo = GetVideoProcessor();
    if (pVideo)
    {
        pVideo->StopConf(listCallID);
    }
}

#endif // 49
