#ifdef IF_SUPPORT_VIDEO

#include "negohelper.h"
#include "talk/talklogic/include/modtalklogic.h"
//#include "commonunits/sipsender.h"
#include "talk/talklogic/include/callinfo.h"
#include "media/mediacommonfun.h"
#include "record/include/modrecord.h"
#include <interfacedefine.h>

#define DELAY_SHOW_TIP 500

IMPLEMENT_GETINSTANCE(CNegoHelper)

static LRESULT OnProcessVoipMessage(msgObject & msg)
{
    return _Negohelper.OnVoipMessage(msg);
}

CNegoHelper::CNegoHelper()
{
    listCallNegoInfo.clear();
    // 重协商超时时间设为6S（同VCS修改），原来的10S太长了
    m_iRenegoTime = 6 * 1000;

    m_iShowTipTimer = 0;
    // 注册SIP消息.
    SingleMsgReg(SIP_CALL_ESTABLISH, OnProcessVoipMessage);

#ifdef IF_FEATURE_H323
    SingleMsgReg(H323_CALL_OPEN_LOGIC_CHANNEL, OnProcessVoipMessage);
#endif  // IF_FEATURE_H323

    SingleMsgReg(TM_TIMER, OnProcessVoipMessage);
}

CNegoHelper::~CNegoHelper()
{
    etl_UnregisterMsgHandle(SIP_CALL_ESTABLISH, SIP_CALL_ESTABLISH, OnProcessVoipMessage);

    etl_UnregisterMsgHandle(H323_CALL_OPEN_LOGIC_CHANNEL, SIP_CALL_ESTABLISH, OnProcessVoipMessage);

    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnProcessVoipMessage);
}

LRESULT CNegoHelper::OnVoipMessage(msgObject & msg)
{
    // 处理消息.
    switch (msg.message)
    {
    case SIP_CALL_ESTABLISH:
        {
            if (NULL == msg.GetExtraData())
            {
                return 0;
            }

            // 不是视频通话不处理,没有重协商不处理
            if (!talklogic_IsVideoTalk(msg.lParam) || !ExistNego(msg.lParam))
            {
                return 0;
            }

            sip_channels_v20_t sipChannelInfo;
            Comm_GetSipChannelsData(sipChannelInfo, msg);

            // hold/held不处理
            if (SIP_CALL_HOLD_DEFAULT != sipChannelInfo.hold_state)
            {
                return 0;
            }

            return ProcessNegoBack(msg.lParam, sipChannelInfo);
        }
        break;
#ifdef IF_FEATURE_H323
    case H323_CALL_OPEN_LOGIC_CHANNEL:
        {
            h323_channel_t * pChannel = (h323_channel_t *)msg.GetExtraData();
            if (NULL == pChannel)
            {
                return 0;
            }

            if (pChannel->channel_type != H323_VIDEO_CHANNEL)
            {
                return 0;
            }

            // 不是视频通话不处理,没有重协商不处理
            if (!talklogic_IsVideoTalk(msg.lParam) || !ExistNego(msg.lParam))
            {
                return 0;
            }

            return ProcessNegoBack(msg.lParam, pChannel);

        }
        break;
#endif  // IF_FEATURE_H323
    case TM_TIMER:
        {
            return OnTimer(msg.wParam);
        }
        break;
    default:
        break;
    }

    return 1;
}

void CNegoHelper::RegisterCallBack(FunNegoHelperCallBack fun)
{
    m_CallBackFun = fun;
}

void CNegoHelper::NegoCallBack(NegoResParam & negoParam)
{
    if (NULL != m_CallBackFun)
    {
        m_CallBackFun(negoParam);
    }
}

LRESULT CNegoHelper::ProcessNegoBack(int iCallId, sip_channels_v20_t & sipChannelInfo)
{
    TALK_INFO("Process SIP Nego Back");
    ipvp_speech_t voiceParam;
    VideoExtInfo ExtInfo;
    AudioStreamInfo audioStream;
    MediaStreamInfo StreamInfo;
    if (talklogic_GetMediaInfoByCallId(iCallId, StreamInfo))
    {
        audioStream = StreamInfo.m_stAudio;
    }

    Comm_MediaParam_sip2ipvp(&sipChannelInfo, "", &voiceParam, &ExtInfo);
    Comm_AudioCodec_sip2ipvp(audioStream, &voiceParam);

    return ProcessRenegoResult(iCallId, voiceParam, ExtInfo, StreamInfo);
}

#ifdef IF_FEATURE_H323
LRESULT CNegoHelper::ProcessNegoBack(int iCallId, h323_channel_t * pChannelInfo)
{
    if (pChannelInfo->channel_type != H323_VIDEO_CHANNEL)
    {
        return 0;
    }

    TALK_INFO("Process H323 Nego Back");
    ipvp_speech_t voiceParam;
    VideoExtInfo ExtInfo;
    MediaStreamInfo StreamInfo;
    ipvp_speech_t speechInfo;
    memset(&voiceParam, 0, sizeof(ipvp_speech_t));
    Comm_MediaParam_H323ToIpvp(pChannelInfo, "", &voiceParam, &ExtInfo);
    if (talklogic_GetSpeechInfoByCallId(iCallId, speechInfo))
    {
        memcpy(&voiceParam.audio, &speechInfo.audio, sizeof(ipvp_audio_t));
        voiceParam.mode |= speechInfo.mode;
    }

    talklogic_GetMediaInfoByCallId(iCallId, StreamInfo);
    return ProcessRenegoResult(iCallId, voiceParam, ExtInfo, StreamInfo);
}
#endif  // IF_FEATURE_H323

// 处理重协商结果
LRESULT CNegoHelper::ProcessRenegoResult(int iCallId, ipvp_speech_t & voiceParam,
        VideoExtInfo & ExtInfo, MediaStreamInfo & StreamInfo)
{
    bool bFirst = true;
    VideoNegoInfo NegoInfo;
    NegoInfo.videoInfo = voiceParam.video;
    NegoInfo.videoExt = ExtInfo;
    NegoInfo.videoStream = StreamInfo.m_stVideo;

    while (listCallNegoInfo.size() > 0)
    {
        CallNegoInfo * pNegoInfo = GetFirstNegoInfo(iCallId);
        if (NULL == pNegoInfo
                || (bFirst && pNegoInfo->eState != STS_NEGOING && pNegoInfo->eState != STS_NEGO_INVALID))
        {
            TALK_INFO("Nego info is null[%d] First nego[%d]", NULL == pNegoInfo, bFirst);
            return 0;
        }

        timerKillThreadTimer((UINT)&pNegoInfo->iCallId);
        // 无效协商不处理
        if (pNegoInfo->eState == STS_NEGO_INVALID)
        {
            TALK_INFO("[%d][%d]Invalid renegoation", pNegoInfo->iCallId, pNegoInfo->eAction);
            DeleteCallNegoInfo(iCallId);
            bFirst = false;
            continue;
        }
        NegoResParam negoParam;
        negoParam.iCallId = iCallId;
        negoParam.eAction = pNegoInfo->eAction;
        memcpy(&negoParam.voiceParam, &voiceParam, sizeof(ipvp_speech_t));
        memcpy(&negoParam.videoinfo, &ExtInfo, sizeof(VideoExtInfo));
        int iRes = CAL_RES_OK;
        if (CaculateCurrentAbility(iCallId, NegoInfo, pNegoInfo->eAction, iRes) == NEGO_WAIT)
        {
            negoParam.eNegoRes = RES_NEGO_FAIL;
            if (!bFirst)
            {
                Renegotiation(pNegoInfo->iAccountId, iCallId, pNegoInfo->eAction, iRes, 3, true);
            }

            TALK_INFO("resolution not satisfy[%d][%d]", pNegoInfo->iCallId, pNegoInfo->eAction);
            // 回来的分辨率不满足，继续等待，超时才认为是失败
            return 1;
        }
        else
        {
            negoParam.eNegoRes = RES_NEGO_OK;
        }

        if (bFirst || RES_NEGO_OK == negoParam.eNegoRes)
        {
            DeleteCallNegoInfo(negoParam.iCallId);
            // 没有建立会议协商，则删除会议提示定时器
            if (!ExistAction(ACT_ESTABLISH_CONF) && m_iShowTipTimer != 0)
            {
                timerKillThreadTimer((UINT)&m_iShowTipTimer);
                m_iShowTipTimer = 0;
            }
            NegoCallBack(negoParam);
        }

        bFirst = false;
    }

    return 1;
}

// 根据当前情况计算操作是否需要等待重协商
NegoProcessType CNegoHelper::CaculateCurrentAbility(int iCallId, VideoNegoInfo & pVideoInfo,
        NegoAction eAction, int & iRes)
{
    VideoResolutionParam resolution;
    MediaComm_GetMaxVideoResolution(resolution, pVideoInfo.videoInfo, pVideoInfo.videoExt,
                                    pVideoInfo.videoStream, false);
    NegoProcessType eProcType = NEGO_NO_NEED;
    iRes = CAL_RES_OK;
    bool bConf = talklogic_IsLocalConf(iCallId);
    bool bRecord = modRecord_IsRecording(RECORD_VIDEO);
    bool bExtVideo = talklogic_IsExtVideoTalk(iCallId);
    if (bConf)
    {
        if (ACT_SPLIT_CONF == eAction)
        {
            // 按照新的方案，拆分会议时是Yealink设备并且没有辅流需要尝试提升
            if (talklogic_IsYealinkDevice(iCallId) && !bExtVideo)
            {
                iRes |= CAL_RES_RCV_1080P;
            }
            // 会议中收720P，发448P，拆分会议后，根据是否有录制和辅流提升收发的分辨率
//          if (bExtVideo)
//          {
//              iRes |= CAL_RES_RCV_720P;
//          }
//          else
//          {
//              iRes |= CAL_RES_RCV_1080P;
//          }
//
//          if (bRecord)
//          {
//              iRes |= CAL_RES_SEND_720P;
//          }
//          else
//          {
//              iRes |= CAL_RES_SEND_1080P;
//          }

            eProcType = NEGO_WITHOUT_WAIT;
        }

        return eProcType;
    }

    switch (eAction)
    {
    case ACT_START_RECORD:
        {
            iRes |= CAL_RES_SEND_720P;

            if (MediaComm_CompareResolution(resolution, SIZE_720P))
            {
                iRes |= CAL_RES_RCV_720P;
                eProcType = NEGO_WAIT;
            }
        }
        break;
    case ACT_STOP_RECORD:
        {
            iRes |= CAL_RES_SEND_1080P;
        }
        break;
    case ACT_ESTABLISH_CONF:
        {
            iRes |= CAL_RES_SEND_720P;

            if (MediaComm_CompareResolution(resolution, SIZE_720P))
            {
                iRes |= CAL_RES_RCV_720P;
                eProcType = NEGO_WAIT;
            }
        }
        break;
    case ACT_START_EXTVIDEO:
        {
            iRes |= CAL_RES_SEND_720P;
            if (MediaComm_CompareResolution(resolution, SIZE_720P))
            {
                // 开启辅流不需要协商，非Yealink不会超过720P，Yealink的对方会将发送
//              iRes |= CAL_RES_RCV_720P;
//              eProcType = NEGO_WAIT;
            }
        }
        break;
    case ACT_STOP_EXTVIDEO:
        {
            // 开启辅流时对方有可能协商收发720P，关闭时如果不尝试升到1080P可能出现新的发送分辨率上限是1080P，但是只发720P
            iRes |= CAL_RES_SEND_1080P;
//          if (!MediaComm_CompareResolution(resolution, SIZE_720P))
//          {
//              iRes |= CAL_RES_RCV_1080P;
//          }
        }
        break;
    case ACT_INCREASE_REMOTE:
        {
            if (!MediaComm_CompareResolution(resolution, SIZE_720P))
            {
                iRes |= CAL_RES_RCV_1080P;
            }
        }
        break;
    case ACT_REDUCE_REMOTE:
        {
            if (MediaComm_CompareResolution(resolution, SIZE_720P))
            {
                iRes |= CAL_RES_RCV_720P;
            }
        }
        break;
    default:
        break;
    }

    return eProcType;
}

// 根据计算结果进行重协商
bool CNegoHelper::Renegotiation(int iAccountID, int iCallId, NegoAction eAction, int iRes,
                                int iType, bool bNegoNow)
{
    if (iRes < CAL_RES_RCV_448P)
    {
        // 接收的分辨率没变化，不需要重协商
        TALK_INFO("Remote send no change");
        return false;
    }

    // 建立会议协商时要设置会议建立提示定时器
    if (eAction == ACT_ESTABLISH_CONF && m_iShowTipTimer == 0)
    {
        m_iShowTipTimer = DELAY_SHOW_TIP;
        timerSetThreadTimer((UINT)&m_iShowTipTimer, m_iShowTipTimer);
    }

    CallNegoInfo negoInfo;
    negoInfo.iAccountId = iAccountID;
    negoInfo.iCallId = iCallId;
    negoInfo.eAction = eAction;
    negoInfo.iRes = iRes;
    if (GetFirstNegoState(iCallId) == STS_NEGOING)
    {
        negoInfo.eState = STS_NEGO_WAITING;
        AddWaitingAction(negoInfo);
        return false;
    }
    // 如果是resume生效的，有新的重协商，则覆盖掉
    else if (GetFirstNegoState(iCallId) == STS_NEGO_WAIT)
    {
        DeleteCallNegoInfo(iCallId);
    }

    negoInfo.eState = bNegoNow ? STS_NEGOING : STS_NEGO_WAIT;
    listCallNegoInfo.push_back(negoInfo);
    // 直接协商需要定时器
    if (bNegoNow)
    {
        modVoip_NegoH264(iAccountID, iCallId, GetVideoSize(iRes), iType, bNegoNow);

        CallNegoInfo * pNegoInfo = GetFirstNegoInfo(negoInfo.iCallId);
        if (NULL != pNegoInfo)
        {
            SetRenegoTimer((UINT)&pNegoInfo->iCallId);
        }
    }

    return true;
}

bool CNegoHelper::DeleteCallNegoInfo(int iCallId, DeleteType eType /* = DLT_FIRST */)
{
    YLList<CallNegoInfo>::ListIterator iter = listCallNegoInfo.begin();
    while (iter != listCallNegoInfo.end())
    {
        if (iCallId == (*iter).iCallId)
        {
            if (eType == DLT_FIRST)
            {
                iter = listCallNegoInfo.erase(iter);
                return true;
            }

            if (eType == DLT_ALL)
            {
                iter = listCallNegoInfo.erase(iter);
                continue;
            }

            if (eType == DLT_WAIT && STS_NEGO_WAIT == (*iter).eState)
            {
                iter = listCallNegoInfo.erase(iter);
                return true;
            }

        }

        iter ++;
    }

    return true;
}

NegoState CNegoHelper::GetFirstNegoState(int iCallId)
{
    CallNegoInfo * pNegoInfo = GetFirstNegoInfo(iCallId);
    if (pNegoInfo != NULL)
    {
        return pNegoInfo->eState;
    }

    return STS_NONE;
}

CallNegoInfo * CNegoHelper::GetFirstNegoInfo(int iCallId)
{
    YLList<CallNegoInfo>::ListIterator iter = listCallNegoInfo.begin();
    for (; iter != listCallNegoInfo.end(); iter++)
    {
        if (iCallId == (*iter).iCallId)
        {
            return &(*iter);
        }
    }

    return NULL;
}

CallNegoInfo * CNegoHelper::GetNegoInfoByTimer(UINT uTimerId)
{
    if (listCallNegoInfo.size() == 0)
    {
        return NULL;
    }

    YLList<CallNegoInfo>::ListIterator iter = listCallNegoInfo.begin();
    for (; iter != listCallNegoInfo.end(); iter++)
    {
        if (uTimerId == (UINT) & (*iter).iCallId)
        {
            return &(*iter);
        }
    }

    return NULL;
}

static bool CompareActionType(NegoAction eAtionExist, NegoAction eActionNew)
{
    if (eAtionExist == eActionNew)
    {
        TALK_INFO("Same action[%d]", eActionNew);
        return true;
    }

    if ((ACT_START_RECORD == eAtionExist && ACT_STOP_RECORD == eActionNew)
            || (ACT_STOP_RECORD == eAtionExist && ACT_START_RECORD == eActionNew)
            || (ACT_ESTABLISH_CONF == eAtionExist && ACT_SPLIT_CONF == eActionNew)
            || (ACT_SPLIT_CONF == eAtionExist && ACT_ESTABLISH_CONF == eActionNew)
            || (ACT_START_EXTVIDEO == eAtionExist && ACT_STOP_EXTVIDEO == eActionNew)
            || (ACT_STOP_EXTVIDEO == eAtionExist && ACT_START_EXTVIDEO == eActionNew))
    {
        TALK_INFO("Reverse action[%d]", eActionNew);
        return true;
    }

    return false;
}

bool CNegoHelper::AddWaitingAction(CallNegoInfo & negoInfo)
{
    for (YLList<CallNegoInfo>::ListIterator iter = listCallNegoInfo.begin();
            iter != listCallNegoInfo.end(); iter++)
    {
        if (negoInfo.iCallId == (*iter).iCallId
                && STS_NEGO_WAITING == (*iter).eState
                // true的话表明是同一类操作（目前有3类：录制、会议、辅流）
                && (CompareActionType((*iter).eAction, negoInfo.eAction)))
        {
            if (negoInfo.eAction == (*iter).eAction)
            {
                // 一样的操作不需要添加，正常情况下是不会出现的
                return false;
            }

            // 相反的操作，相当于保持当前状态，要把前一个加入链表的操作删除
            listCallNegoInfo.erase(iter);
            return false;
        }
    }

    listCallNegoInfo.push_back(negoInfo);
    return true;
}

bool CNegoHelper::ExistAction(NegoAction eAction)
{
    YLList<CallNegoInfo>::ListIterator iter = listCallNegoInfo.begin();
    for (; iter != listCallNegoInfo.end(); iter++)
    {
        if (eAction == (*iter).eAction)
        {
            return true;
        }
    }

    return false;
}

bool CNegoHelper::RemoveAction(NegoAction eAction)
{
    YLList<CallNegoInfo>::ListIterator iter = listCallNegoInfo.begin();
    for (; iter != listCallNegoInfo.end(); iter++)
    {
        if (eAction == (*iter).eAction)
        {
            listCallNegoInfo.erase(iter);
            return true;
        }
    }

    return false;
}

LRESULT CNegoHelper::OnTimer(UINT iTimerID)
{
    if (iTimerID == (UINT)&m_iShowTipTimer)
    {
        timerKillThreadTimer((UINT)&m_iShowTipTimer);
        m_iShowTipTimer = 0;
        if (ExistAction(ACT_ESTABLISH_CONF))
        {
            TALK_INFO("ShowTipTimer[%d] time out", m_iShowTipTimer);
            NegoResParam negoParam;
            negoParam.iCallId = -1;
            negoParam.eAction = ACT_ESTABLISH_CONF;
            negoParam.eNegoRes = RES_NEGO_WAITING;
            NegoCallBack(negoParam);
        }

        return true;
    }

    CallNegoInfo * pNegoInfo = GetNegoInfoByTimer(iTimerID);
    if (NULL == pNegoInfo)
    {
        return false;
    }

    TALK_INFO("Renego time out[%d]", *(int *)iTimerID);
    timerKillThreadTimer(iTimerID);
    // 无效协商，超时也不处理
    if (pNegoInfo->eState == STS_NEGO_INVALID)
    {
        DeleteCallNegoInfo(pNegoInfo->iCallId);
        return true;
    }
    NegoResParam negoParam;
    negoParam.iCallId = pNegoInfo->iCallId;
    negoParam.eAction = pNegoInfo->eAction;
    negoParam.eNegoRes = RES_NEGO_FAIL;
    // 会议协商只要一路失败，则其它路没必要再处理
    if (ACT_ESTABLISH_CONF == pNegoInfo->eAction)
    {
        SetNegoActionState(ACT_ESTABLISH_CONF, STS_NEGO_INVALID);
    }

    // 删除当前协商信息，回调处理
    DeleteCallNegoInfo(negoParam.iCallId);
    NegoCallBack(negoParam);

    // 进行下一个协商
    pNegoInfo = GetFirstNegoInfo(pNegoInfo->iCallId);
    if (NULL != pNegoInfo)
    {
        Renegotiation(pNegoInfo->iAccountId, pNegoInfo->iCallId, pNegoInfo->eAction, pNegoInfo->iRes, 3,
                      true);
    }

    return true;
}

bool CNegoHelper::SetRenegoTimer(UINT iTimerID)
{
    if (m_iRenegoTime < 6000)
    {
        m_iRenegoTime = 6 * 1000;
    }

    return timerSetThreadTimer(iTimerID, m_iRenegoTime) != 0;
}

bool CNegoHelper::ExistNego(int iCallId)
{
    CallNegoInfo * pNegoInfo = GetFirstNegoInfo(iCallId);

    return (NULL == pNegoInfo) ? false : true;
}

bool CNegoHelper::StartFirstNego(int iCallId, int iAccountId)
{
    CallNegoInfo * pNegoInfo = GetFirstNegoInfo(iCallId);
    if (NULL != pNegoInfo && pNegoInfo->eState == STS_NEGO_WAIT)
    {
        pNegoInfo->eState = STS_NEGOING;
        modVoip_NegoH264(iAccountId, iCallId, GetVideoSize(pNegoInfo->iRes), 3, true);

        return SetRenegoTimer((UINT)&pNegoInfo->iCallId);
    }

    return false;
}

void CNegoHelper::SetNegoActionState(NegoAction eAction, NegoState eState)
{
    YLList<CallNegoInfo>::ListIterator iter = listCallNegoInfo.begin();
    for (; iter != listCallNegoInfo.end(); iter++)
    {
        if (eAction == (*iter).eAction)
        {
            (*iter).eState = eState;
        }
    }

    return;
}

VideoSizeInfo CNegoHelper::GetVideoSize(int iRes)
{
    if (iRes & CAL_RES_RCV_448P)
    {
        return VideoSizeInfo(704, 576, 30);
    }
    else if (iRes & CAL_RES_RCV_720P)
    {
        return VideoSizeInfo(1280, 720, 30);
    }

    return VideoSizeInfo(1920, 1080, 30);
}
#endif //#ifdef IF_SUPPORT_VIDEO
