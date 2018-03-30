#ifdef IF_SUPPORT_VIDEO
///////////////////////////////////////////////////////////
//  CVideoManager.cpp
//  Implementation of the Class CVideoManager
//  Created on:      05-二月-2015 21:05:04
//  Original author: huangwz
///////////////////////////////////////////////////////////

#include "cvideomanager.h"
#include "cmediamanager.h"
#include "commondefine.h"
#include "mediacommonfun.h"
#include "lamp/ledlamp/include/modledlamp.h"
#include "hotplug/modhotplug.h"
#include "adapterbox/include/modadapterbox.h"
#include "hotplug/modhotplug.h"
#include "voip/modvoip.h"
#include "record/include/modrecord.h"

#ifndef IS_MEDIA_TEST
#include "service_ipvp.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/include/callinfo.h"
#else
#include "mediatest.h"
/////////////////////////////测试代码使用/////////////////////////////////////////////
static CCallInfo * talklogic_FindCallInfoByCallId(int iCallId)
{
    VOICE_INFO("Test find call info[%d]", iCallId);
    return _mediaTest.GetCallInfo(iCallId);
}

static bool talklogic_GetMuteStatus(bool bVideo = false)
{
    return _mediaTest.m_bVideoMute;
}

static bool talklogic_IsRingSessionExist()
{
    return _mediaTest.m_bHaveVideoRing;
}

static bool talklogic_BeHaveHoldVideoSession()
{
    return _mediaTest.m_bHaveVideoHold;
}

static bool talklogic_IsVideoSessionExist()
{
    return _mediaTest.m_bIsVideoTalking;
}

static bool talklogic_IsLocalConf(int nCallId)
{
    return _mediaTest.m_bIsLocalConf;
}
//////////////////////////////////////////////////////////////////////////
#endif // IS_MEDIA_TEST

// 定义最大视频窗口个数
#define VIDEOLAYOUTSIZE  4

CVideoManager::CVideoManager()
{
    m_ePower = VPT_FULL;
}


CVideoManager::~CVideoManager()
{
    ipvp_cancel_notify(mkGetHandle(), IPVP_MSG_VIDEO_RREQ_KFRAME);
}


void CVideoManager::Init()
{
    // 初始灯状态
    UpdateLight();
}

bool CVideoManager::OnEvent(MediaEventData * pData)
{
    if (NULL == pData)
    {
        return false;
    }

    bool bHandled = true;
    switch (pData->eType)
    {
    case MET_INCOMING:
    case MET_TALK_HIDE:
        {
            UpdateLight();
        }
        break;
    case MET_CAMERA_CHANGE:
        {
            UpdateLight();
            if (pData->wEventPara == CST_NOT_CONNECTED
                    && talklogic_IsVideoSessionExist()
                    && talklogic_GetMuteStatus(true))
            {
                // 强制设置一次PG
                modhotplug_CtrlCamera(false, true, true);
            }
            else
            {
                AdjustCameraMode();
            }
        }
        break;
    case MET_CAMERA_UPDATE:
        {
            UpdateLight();
            AdjustCameraMode();
        }
        break;
    case MET_TALK_START:
        {
            OnStarVideo(pData->wEventPara);
        }
        break;
    case MET_TALK_STOP:
        {
            OnStopVideo(pData->wEventPara);
        }
        break;
    case MET_TALK_RESET:
        {
            OnResetVideo(pData->wEventPara);
        }
        break;
    case MET_CONF_START:
        {
            OnStartConf();
        }
        break;
    case MET_CONF_STOP:
        {
            OnStopConf(pData->wEventPara);
        }
        break;
    case MET_TALK_HOLD:
        {
            OnHold(pData->wEventPara);
        }
        break;
    case MET_TALK_VIDEO_MUTE:
        {
            OnMuteVideo(pData->wEventPara, pData->lEventPara);
        }
        break;
    case MET_TALK_STOP_VIDEO:
        {
            OnStopVideoEx(pData->wEventPara);
        }
        break;
    case MET_TALK_VIDEO_KEYFRAME_SEND:
        {
            OnSendKeyframe(pData->wEventPara, pData->lEventPara, (char *)pData->pExt);
        }
        break;
    case MET_LAYOUT_CHANGE:
        {
            if (NULL != pData->pExt)
            {
                SetVideoLayout((DisplayMonitor *)pData->pExt);
            }
        }
        break;
    case MET_PREVIEW_VIDEO:
        {
            if (NULL != pData->pExt)
            {
                PreviewVideo((DisplayMonitor *)pData->pExt, pData->wEventPara == 1);
            }
        }
        break;
    case MET_STATS_INFO:
        {
            if (NULL != pData->pExt)
            {
                GetStats(pData->wEventPara, *(MediaStatsInfo *)pData->pExt);
            }
        }
        break;
    case MET_TALK_VIDEO_KEYFRAME_REQ:
        {
            OnReqKeyframe(pData->wEventPara, pData->lEventPara);
        }
        break;
    case MET_IPVP_START:
        {
            //
            VOICE_INFO("video register notify ipvp");
            // 注册监听关键帧消息，ipvp的消息需要GUI注册实例到ipvp进程
            ipvp_request_notify(mkGetHandle(), IPVP_MSG_VIDEO_RREQ_KFRAME);

            ipvp_request_notify(mkGetHandle(), IPVP_MSG_VIDEO_RREQ_FCTRL);

            // todo
            ipvp_request_notify(mkGetHandle(), IPVP_MSG_VIDEO_RECV_RESIZED);

            SetVideoBandwidth(configParser_GetConfigInt(kszIncomingBandwidth) * 1024,
                              configParser_GetConfigInt(kszOutgoingBandwidth) * 1024) ;
        }
        break;
    case MET_CONFIG_CHANGE:
        {
            if (pData->wEventPara == ST_VIDEO_BANDWIDTH)
            {
                SetVideoBandwidth(configParser_GetConfigInt(kszIncomingBandwidth) * 1024,
                                  configParser_GetConfigInt(kszOutgoingBandwidth) * 1024);
            }
        }
        break;
    case MET_ADJUST_POWER:
        {
            OnAdjustPower(pData->wEventPara);
        }
        break;
    case MET_STATS_UPDATE:
        {
            UpdateStats();
        }
        break;
    case MET_TALK_VIDEO_FCTRL_REQ:
        {
            if (NULL != pData->pExt)
            {
                OnReqBitrate(pData->wEventPara, pData->lEventPara, *((int *)pData->pExt));
            }
        }
        break;
    case MET_BITRATE_RESET:
        {
            if (NULL != pData->pExt)
            {
                ResetBitrate(pData->wEventPara, pData->lEventPara, *((int *)pData->pExt));
            }
        }
        break;
    case MET_SHARE_START:
        {
            OnStartShareVideo(pData->wEventPara);
        }
        break;
    case MET_SHARE_STOP:
        {
            OnStopShareVideo(pData->wEventPara);
        }
        break;
    case MET_TALK_VIDEO_RECV_RESIZED:
        {
            VOICE_INFO("IPVP_MSG_VIDEO_RECV_RESIZED [%d]", pData->wEventPara);
            // 对端视频分辨率
            OnVideoResized(pData->wEventPara, VD_REC, (ipvp_resize_t *)pData->pExt);
        }
        break;
    default:
        bHandled = false;
        break;
    }
    return bHandled;
}

static void PrintStats(VideoStatsInfo & videoStats)
{
    VOICE_INFO("Video stats info:");
    VOICE_INFO("	Video codec Rec[%s] Send[%s]", videoStats.RecCodec, videoStats.SendCodec);
    VOICE_INFO("	Video bitrate Rec[%d] Send[%d]", videoStats.RecBitrate, videoStats.SendBitrate);
    VOICE_INFO("	Video frmrate Rec[%d] Send[%d]", videoStats.RecFramerate, videoStats.SendFramerate);
    VOICE_INFO("	Video width Rec[%d] Send[%d]", videoStats.RecWidth, videoStats.SendWidth);
    VOICE_INFO("	Video height Rec[%d] Send[%d]", videoStats.RecHeight, videoStats.SendHeight);
    VOICE_INFO("	Video lostfrac Rec[%d] Send[%d]", videoStats.RecLostFrac, videoStats.SendLostFrac);
    VOICE_INFO("	Video losttotal Rec[%d] Send[%d]", videoStats.RecLostTotal, videoStats.SendLostTotal);
    VOICE_INFO("	Video jitCur Rec[%d] Send[%d]", videoStats.RecJitCur, videoStats.SendJitCur);
    VOICE_INFO("	Video jitAvg Rec[%d] Send[%d]", videoStats.RecJitAvg, videoStats.SendJitAvg);
    VOICE_INFO("	Video jitMax Rec[%d] Send[%d]", videoStats.RecJitMax, videoStats.SendJitMax);
    VOICE_INFO("	Video Delay Rec[%d] Send[%d]", videoStats.RecDelay, videoStats.SendDelay);
}

bool CVideoManager::GetIpvpStats(int iCallId, VideoStatsInfo & statsInfo, bool bShare)
{
    ipvp_vstats_t videoStats;
    memset(&videoStats, 0, sizeof(ipvp_vstats_t));
    videoStats.flags = 0;
    videoStats.size = sizeof(ipvp_vstats_t);

    // 怀疑出现卡住问题
    VOICE_WARN("get video stats[%d]", bShare);

    if (bShare)
    {
        ipvp_get_share_stats(mkGetHandle(), iCallId, &videoStats);
    }
    else
    {
        ipvp_get_video_stats(mkGetHandle(), iCallId, &videoStats);
    }

    //http://bugfree.yealink.com/Bug.php?BugID=78233
#define H264HP_PROFILE     100
#define H264HP_CODECNAME   "H264HP"
#define H264_CODECNAME     "H264"
//  TALK_INFO("rx_codec[%s]profile[%d] tx_codec[%s]profile[%d]",videoStats.rx_codec,
//  videoStats.rx_profile,videoStats.tx_codec,videoStats.tx_profile);

    if (strcmp(videoStats.rx_codec, H264_CODECNAME) == 0
            && videoStats.rx_profile == H264HP_PROFILE)
    {
        strncpy(statsInfo.RecCodec, H264HP_CODECNAME, IPVP_MAX_CODECNAME);
    }
    else
    {
        strncpy(statsInfo.RecCodec, videoStats.rx_codec, IPVP_MAX_CODECNAME);
    }
    statsInfo.RecCodec[IPVP_MAX_CODECNAME - 1] = '\0';

    if (strcmp(videoStats.tx_codec, H264_CODECNAME) == 0
            && videoStats.tx_profile == H264HP_PROFILE)
    {
        strncpy(statsInfo.SendCodec, H264HP_CODECNAME, IPVP_MAX_CODECNAME);
    }
    else
    {
        strncpy(statsInfo.SendCodec, videoStats.tx_codec, IPVP_MAX_CODECNAME);
    }
    statsInfo.SendCodec[IPVP_MAX_CODECNAME - 1] = '\0';

    statsInfo.RecBitrate = videoStats.rx_bitrate;
    statsInfo.SendBitrate = videoStats.tx_bitrate;
    statsInfo.RecFramerate = videoStats.rx_framerate;
    statsInfo.SendFramerate = videoStats.tx_framerate;

    statsInfo.RecJitCur = videoStats.rx_jit_cur;
    statsInfo.SendJitCur = videoStats.tx_jit_cur;
    statsInfo.RecJitAvg = videoStats.rx_jit_avg;
    statsInfo.SendJitAvg = videoStats.tx_jit_avg;
    statsInfo.RecJitMax = videoStats.rx_jit_max;
    statsInfo.SendJitMax = videoStats.tx_jit_max;

    statsInfo.RecDelay = videoStats.rx_delay_rt;
    statsInfo.SendDelay = videoStats.tx_delay_rt;

    statsInfo.RecLostFrac = videoStats.rx_lost_frac;
    statsInfo.SendLostFrac = videoStats.tx_lost_frac;
    statsInfo.RecLostTotal = videoStats.rx_lost_total;
    statsInfo.SendLostTotal = videoStats.tx_lost_total;

    statsInfo.RecWidth = videoStats.rx_width;
    statsInfo.SendWidth = videoStats.tx_width;
    statsInfo.RecHeight = videoStats.rx_height;
    statsInfo.SendHeight = videoStats.tx_height;

#if 0
    PrintStats(statsInfo);
#endif

    return true;
}

bool CVideoManager::GetStats(int iCallId, MediaStatsInfo & statsInfo)
{
    VOICE_INFO("Video get stats");
    if (iCallId < 0)
    {
        int iFocuseSession = talklogic_GetFocusedSessionID();
        CCallInfo * pCallInfo = (CCallInfo *)talklogic_GetCallInfoBySessionID(iFocuseSession);
        if (NULL == pCallInfo)
        {
            return false;
        }
        iCallId = pCallInfo->GetCallID();
    }

    GetIpvpStats(iCallId, statsInfo.video, false);
    GetIpvpStats(iCallId, statsInfo.share, true);

    return true;
}

int CVideoManager::GetVideoNum()
{
    return m_listVideo.size();
}

void CVideoManager::GetVideSize(VideoSizeType eType, int & iWidth, int & iHeight)
{
    switch (eType)
    {
    case VST_1080P:
        {
            iWidth = 1920;
            iHeight = 1080;
        }
        break;
    case VST_720P:
        {
            iWidth = 1280;
            iHeight = 720;
        }
        break;
    case VST_4CIF:
        {
            iWidth = 704;
            iHeight = 576;
        }
        break;
    case VST_CIF:
        {
            iWidth = 352;
            iHeight = 288;
        }
        break;
    case VST_SXGA:
        {
            iWidth = 1280;
            iHeight = 1024;
        }
        break;
    case VST_WXGA_5_3:
        {
            iWidth = 1280;
            iHeight = 768;
        }
        break;
    case VST_XGA:
        {
            iWidth = 1024;
            iHeight = 768;
        }
        break;
    case VST_SVGA:
        {
            iWidth = 800;
            iHeight = 600;
        }
        break;
    case VST_VGA:
        {
            iWidth = 640;
            iHeight = 480;
        }
        break;
    case VST_W448P:
        {
            iWidth = 768;
            iHeight = 448;
        }
        break;
    case VST_W576P:
        {
            iWidth = 1024;
            iHeight = 576;
        }
        break;
    case VST_WQVGA:
        {
            iWidth = 400;
            iHeight = 240;
        }
        break;
    default:
        break;
    }
}

static bool IsResolution(ipvp_video_t & video, int iWidth, int iHeight)
{
    if (video.enc_list[0].win_width == iWidth
            && video.enc_list[0].win_height == iHeight
            || video.dec_list[0].win_width == iWidth
            && video.dec_list[0].win_height == iHeight)
    {
        return true;
    }

    return false;
}
// 是否存在1080p的通话
bool CVideoManager::IsVideoExist(VideoSizeType eType)
{
    if (m_listVideo.size() == 0)
    {
        return false;
    }

    int iWidth = 0;
    int iHeight = 0;
    GetVideSize(eType, iWidth, iHeight);

    YLList<VideoInfo>::iterator it = m_listVideo.begin();
    for (; it != m_listVideo.end(); ++it)
    {
        VideoInfo & info = (*it);
        if (IsResolution(info.videoInfo, iWidth, iHeight))
        {
            return true;
        }
    }

    return false;
}

void CVideoManager::GetRealVideoResolution(int iCallId, VideoResolutionParam & stInfo)
{
#if DEBUG_MEDIA
    VOICE_INFO("Get video resolution[%d]", iCallId);
#endif //DEBUG_MEDIA
    VideoInfo * pInfo = GetVideoInfo(iCallId);
    if (pInfo != NULL)
    {
        stInfo.m_height = pInfo->stRecvRes.m_height;
        stInfo.m_width = pInfo->stRecvRes.m_width;
        stInfo.m_fps = pInfo->stRecvRes.m_fps;
    }

#if DEBUG_MEDIA
    VOICE_INFO("resolution h[%d] w[%d] f[%d]", stInfo.m_height, stInfo.m_width, stInfo.m_fps);
#endif //DEBUG_MEDIA

    return ;
}

bool CVideoManager::GetIpvpVideoInfo(ipvp_video_t & video, CCallInfo * pCallInfo)
{
    if (NULL == pCallInfo)
    {
        return false;
    }

    memcpy(&video, &pCallInfo->m_tVPMParam.video, sizeof(ipvp_video_t));

    VideoResolutionParam resDecodeLimit;
    VideoResolutionParam resEncodeLimit;
    GetMaxResolution(resEncodeLimit, true, pCallInfo->GetAccountID(), pCallInfo->GetCallID());
    GetMaxResolution(resDecodeLimit, false, pCallInfo->GetAccountID(), pCallInfo->GetCallID());

    // 调整分辨率
    VOICE_INFO("Max send w[%d] h[%d] rec w[%d] h[%d]", resEncodeLimit.m_width, resEncodeLimit.m_height,
               resDecodeLimit.m_width, resDecodeLimit.m_height);

    // list[0]的扩展数据特殊处理 注意:remote 编码 local 解码
    VOICE_INFO("Parse encodec 0:");
    FetchResolution(video.enc_list[0], pCallInfo->m_stVideoExt.m_stRemote, resEncodeLimit);

    VOICE_INFO("Parse decodec 0:");
    FetchResolution(video.dec_list[0], pCallInfo->m_stVideoExt.m_stLocal, resDecodeLimit);

    VOICE_INFO("Parse encodec list:");
    VideoStreamInfo & videoStream = pCallInfo->m_stMediaInfo.m_stVideo;

    VideoCodecList::iterator iter = videoStream.listRemoteCodec.begin();
    // 因为第1个codec是通话建立的消息携带，目前codec从1开始，以后合并统一处理
    int iCodec = 1;
    int iBitrate = video.enc_list[0].codec_bitrate;
    for (; iter != videoStream.listRemoteCodec.end() && iCodec < IPVP_MAX_CODEC; ++iter)
    {
        ipvp_vcodec_t & codec = (*iter).codec;
        VideoCodecExtParam & ext = (*iter).extParam;
        video.enc_list[iCodec] = codec;
        video.enc_list[iCodec].codec_bitrate = iBitrate;
        FetchResolution(video.enc_list[iCodec], ext, resEncodeLimit);
        iCodec++;
    }
    video.enc_max = iCodec;

    VOICE_INFO("Parse decodec list:");
    iter = videoStream.listLocalCodec.begin();
    // 因为第1个codec是通话建立的消息携带，目前可用codec从1开始，以后合并统一处理
    iCodec = 1;
    iBitrate = video.dec_list[0].codec_bitrate;
    for (; iter != videoStream.listLocalCodec.end() && iCodec < IPVP_MAX_CODEC; ++iter)
    {
        ipvp_vcodec_t & codec = (*iter).codec;
        VideoCodecExtParam & ext = (*iter).extParam;
        video.dec_list[iCodec] = codec;
        video.dec_list[iCodec].codec_bitrate = iBitrate;
        FetchResolution(video.dec_list[iCodec], ext, resDecodeLimit);
        iCodec++;
    }
    video.dec_max = iCodec;

    // capset
    strncpy(video.priv_capset, pCallInfo->m_stMediaInfo.m_stVideo.strCapset.c_str(),
            chMIN(IPVP_MAX_CODEC_ATTR, pCallInfo->m_stMediaInfo.m_stVideo.strCapset.size()));
    return true;
}

bool CVideoManager::GetIpvpShareInfo(ipvp_video_t & video, CCallInfo * pCallInfo)
{
    if (NULL == pCallInfo)
    {
        return false;
    }

    memcpy(&video, &pCallInfo->m_stShareInfo, sizeof(ipvp_video_t));
    VideoResolutionParam resDecodeLimit;
//  VideoResolutionParam resEncodeLimit;
//  GetMaxResolution(resEncodeLimit, true, pCallInfo->GetAccountID(), pCallInfo->GetCallID());
    GetMaxResolution(resDecodeLimit, false, pCallInfo->GetAccountID(), pCallInfo->GetCallID(), true);

    // 调整分辨率
//  VOICE_INFO("Max send w[%d] h[%d] rec w[%d] h[%d]", resEncodeLimit.m_width, resEncodeLimit.m_height,
//      resDecodeLimit.m_width, resDecodeLimit.m_height);

// list[0]的扩展数据特殊处理 注意:remote 编码 local 解码
//  VOICE_INFO("Parse encodec 0:");
//  FetchResolution(video.enc_list[0], pCallInfo->m_stShareVideoExt.m_stRemote, resEncodeLimit);

    VOICE_INFO("Parse decodec 0:");
    FetchResolution(video.dec_list[0], pCallInfo->m_stShareVideoExt.m_stLocal, resDecodeLimit, false);

    VOICE_INFO("Parse encodec list:");
    VideoStreamInfo & videoStream = pCallInfo->m_stMediaInfo.m_stShare;

//  VideoCodecList::iterator iter = videoStream.listRemoteCodec.begin();
//  // 因为第1个codec是通话建立的消息携带，目前codec从1开始，以后合并统一处理
//  int iCodec = 1;
//  int iBitrate = video.enc_list[0].codec_bitrate;
//  for (; iter != videoStream.listRemoteCodec.end() && iCodec < IPVP_MAX_CODEC; ++iter)
//  {
//      ipvp_vcodec_t& codec = (*iter).codec;
//      VideoCodecExtParam& ext = (*iter).extParam;
//      video.enc_list[iCodec] = codec;
//      video.enc_list[iCodec].codec_bitrate = iBitrate;
//      FetchResolution(video.enc_list[iCodec], ext, resEncodeLimit);
//      iCodec++;
//  }
//  video.enc_max = iCodec;

    VOICE_INFO("Parse decodec list:");
    VideoCodecList::iterator iter = videoStream.listLocalCodec.begin();
    // 因为第1个codec是通话建立的消息携带，目前可用codec从1开始，以后合并统一处理
    int iCodec = 1;
    int iBitrate =  video.dec_list[0].codec_bitrate;
    for (; iter != videoStream.listLocalCodec.end() && iCodec < IPVP_MAX_CODEC; ++iter)
    {
        ipvp_vcodec_t & codec = (*iter).codec;
        VideoCodecExtParam & ext = (*iter).extParam;
        video.dec_list[iCodec] = codec;
        video.dec_list[iCodec].codec_bitrate = iBitrate;
        FetchResolution(video.dec_list[iCodec], ext, resDecodeLimit);
        iCodec++;
    }
    video.dec_max = iCodec;

    return true;
}

void CVideoManager::GetIpvpOsdParam(int iPosx, int iPosy, const yl::string & strOsdFile,
                                    ipvp_osd_t & osdParam)
{
    osdParam.display_mode = IPVP_OSD_MODE_FIXED;
    osdParam.width = 352;
    osdParam.height = 288;
    osdParam.x = iPosx;
    osdParam.y = iPosy;
    osdParam.alpha = 0x30;
    strncpy(osdParam.file, strOsdFile.c_str(), IPVP_MAX_PATH);
    osdParam.file[IPVP_MAX_PATH - 1] = '\0';
}

bool CVideoManager::OnReqBitrate(int iCallId, int iVideoType, int iBandwidth)
{
    VOICE_INFO("Ipvp request bitrate id[%d] type[%d] rate[%d]", iCallId, iVideoType, iBandwidth);
    CCallInfo * pCallInfo = talklogic_FindCallInfoByCallId(iCallId);
    if (NULL == pCallInfo)
    {
        return false;
    }

    return modVoip_RequestBitrate(pCallInfo->GetAccountID(), iCallId, iBandwidth,
                                  (iVideoType == IPVP_VIDEO_ID_SHARED));
}

bool CVideoManager::ResetBitrate(int iCallId, int iVideoType, int iBitrate)
{
    VOICE_INFO("Set bitrate id[%d] type[%d] rate[%d]", iCallId, iVideoType, iBitrate);
    CCallInfo * pCallInfo = talklogic_FindCallInfoByCallId(iCallId);
    if (NULL == pCallInfo)
    {
        return false;
    }

    ipvp_quality_t  quality;
    memset(&quality, 0, sizeof(quality));
    quality.size = sizeof(ipvp_quality_t);
    quality.flag |= IPVP_QUALITY_BITRATE;
    quality.bitrate = iBitrate;
    return !ipvp_video_set_quality(mkGetHandle(), iCallId,
                                   iVideoType == 0 ? IPVP_VIDEO_ID_SPEECH : IPVP_VIDEO_ID_SHARED,
                                   &quality, 0);
}

bool CVideoManager::OnAdjustPower(int iAction)
{
    VOICE_INFO("Adjust video power [%d]", iAction);

    if (0 != iAction && 1 != iAction)
    {
        return false;
    }

    // 设置编码能力
    m_ePower = (iAction == 1) ? VPT_FULL : VPT_SAVING_720;

    // 开始录制视频需要降低编码分辨率
    YLList<VideoInfo>::iterator it = m_listVideo.begin();
    for (; it != m_listVideo.end(); ++it)
    {
        VideoInfo & info = (*it);
        if (talklogic_IsLocalConf(info.iCallId))
        {
            continue;
        }

        // 存在单路视频通话中，需要重置视频参数
        if (ResetVideoSize(info.iCallId))
        {
            m_ePower = VPT_FULL;
            break;
        }
    }

    return true;
}

bool CVideoManager::OnHold(int iCallId)
{
    VOICE_INFO("Video hold callid[%d]", iCallId);
    CCallInfo * pInfo = talklogic_FindCallInfoByCallId(iCallId);
    if (NULL == pInfo)
    {
        return false;
    }

    VideoInfo * pVideoInfo = GetVideoInfo(iCallId);
    if (NULL == pVideoInfo)
    {
        if (pInfo->IsVideoTalk()
                && !pInfo->IsLocalHold())
        {
            // 解hold时，还没有开启视频通话，需要先开启
            return OnStarVideo(iCallId);
        }
        return false;
    }

    // 需要处理参数变更
    if (!pInfo->IsLocalHold())
    {
        ResetVideo(pVideoInfo, pInfo);
    }

    VOICE_INFO("Video hold [%d]", pInfo->IsLocalHold());
    // call id相同的情况下，音频hold，视频就不需要再hold
    //ipvp_video_hold(mkGetHandle(), pInfo->GetCallID(), pInfo->m_bLocalHold ? 1 : 0, 0);
    // 有辅流的通话需要同步Hold辅流
    if (SHARE_STATE_OPENED == pInfo->m_eShareState)
    {
        VOICE_INFO("EXTVideo hold [%d]", pInfo->IsLocalHold());
        ipvp_shared_hold(mkGetHandle(), pInfo->GetCallID(), pInfo->IsLocalHold() ? 1 : 0, 0);
    }
    UpdateLight();

    return true;
}

bool CVideoManager::OnMuteVideo(int iCallId, bool bNeedSleep)
{

#define VIDEO_OSD_POSX 796
#define VIDEO_OSD_POSY 402

    // 注意：Mute与视频话路无关，可任意开启
    int iMute = talklogic_GetMuteStatus(true) ? 1 : 0;
    yl::string strPicture = AdapterBox_GetMutePicture(VST_1080P);
    VOICE_INFO("Video mute callid[%d] mute[%d] picture[%s]", iCallId, iMute, strPicture.c_str());

#ifdef _T49
    if (iMute)
    {
        ipvp_osd_t osdParam;
        memset(&osdParam, 0, sizeof(ipvp_osd_t));
        GetIpvpOsdParam(VIDEO_OSD_POSX, VIDEO_OSD_POSY, strPicture, osdParam);

        ipvp_OSD_setup(mkGetHandle(), IPVP_VIDEO_ID_CAMERA, 0, &osdParam, false);

        UpdateLight();

        AdjustCameraMode();
    }
    else
    {
        UpdateLight();

        ipvp_OSD_clear(mkGetHandle(), IPVP_VIDEO_ID_CAMERA, 0, false);

        AdjustCameraMode();
    }
#endif

    return true;
}

bool CVideoManager::OnOsdChange(OsdLayoutInfo * pInfo)
{
#ifdef _T49
    if (NULL == pInfo)
    {
        return false;
    }

    VOICE_INFO("Osd change id[%d] x[%d] y[%d] type[%d] path[%s]", pInfo->m_iId,
               pInfo->m_iOffsetX, pInfo->m_iOffsetY, pInfo->m_eLayoutType, pInfo->m_strPath.c_str());

    if (pInfo->m_iId > 0
            && !pInfo->m_strPath.empty())
    {
        ipvp_osd_t osdParam;
        memset(&osdParam, 0, sizeof(ipvp_osd_t));
        GetIpvpOsdParam(0, 0, pInfo->m_strPath, osdParam);

        ipvp_OSD_setup(mkGetHandle(), IPVP_VIDEO_ID_CAMERA, pInfo->m_iId, &osdParam, false);
    }
    else
    {
        ipvp_OSD_reset(mkGetHandle(), 0);
    }
#endif

    return true;
}

bool CVideoManager::OnStartConf()
{
    // 暂不处理
//  VOICE_INFO("On start conf");
//  YLList<VideoInfo>::iterator it = m_listVideo.begin();
//  for (; it != m_listVideo.end(); ++it)
//  {
//      VideoInfo& info = (*it);
//
//      if (talklogic_IsLocalConf(info.iCallId))
//      {
//          // 开启会议时，当前通话中的话路要尝试重置分辨率
//          OnResetVideo(info.iCallId);
//      }
//  }
    return true;
}

// 取最大发送分辨率
void CVideoManager::GetMaxResolution(VideoResolutionParam & resolution, bool bEncode, int iAccount,
                                     int iCallId, bool bShare /* = false*/)
{
    // 1 默认分辨率限制
    resolution.m_width = 1920;
    resolution.m_height = 1080;

    // 2 帧率限制
    if (bShare)
    {
        // test
        // 辅流限制15帧以内: 之前版本辅流帧率有误，现在技术支持版本先不动，暂定辅流也30帧
        resolution.m_fps = 15;
    }
    else
    {
        // 主流限制由配置限制，但是不低于10帧
        resolution.m_fps = configParser_GetConfigInt(kszVideoMinFramerate);
        if (resolution.m_fps < 10
                || resolution.m_fps > 30)
        {
            resolution.m_fps = 30;
        }
    }

    if (!bEncode)
    {
        // 解码不限制
        return;
    }

    bool bConf = talklogic_IsLocalConf(iCallId);

    bool bRecording = modRecord_IsRecording(RECORD_VIDEO);

    bool bExtVideo = talklogic_IsExtVideoTalk(iCallId);

    // 限制最大编码分辨率即可 // 按照辅流不协商的方案，辅流存在时限制发送的分辨率为720P
    if (bEncode
            && (bConf || bRecording || m_ePower == VPT_SAVING_720 || bExtVideo))
    {
        // 会议或者录音中，编码不能超过720p
        VOICE_INFO("Encode is limited: conf[%d] Recording[%d] Power[%d] ExtVideo[%d]", bConf, bRecording,
                   m_ePower == VPT_SAVING_720, bExtVideo);
        resolution.m_width = 1280;
        resolution.m_height = 720;
    }

    if (acc_GetAccountProtocol(iAccount) != PROTOCOL_SIP)
    {
        return;
    }

    // T49 sp2 sip账号限制最大发送分辨率
    yl::string strInfo = configParser_GetCfgItemStringValue(kszAccountSendMaxResolution, iAccount);
    VOICE_INFO("send max reslution[%s]", strInfo.c_str());
    int width = 0;
    int height = 0;
    //int frame = 0;    // 暂不需要支持帧率
    sscanf(strInfo.c_str(), "%dx%d", &width, &height);

    if (width == 0
            || height == 0)
    {
        // 合法判断：不限制配置里面的分辨率的比率，方便扩展
        return;
    }

    if (resolution.m_width > width
            || resolution.m_height > height)
    {
        resolution.m_width = width;
        resolution.m_height = height;
    }

    return;
}

bool CVideoManager::FetchResolution(ipvp_vcodec_t & codec, const VideoCodecExtParam & videoExt,
                                    VideoResolutionParam resLimit, bool bfpsPrior/* = true*/)
{
    VideoResolutionParam resolutionArr[12];
    int iArrSize = 12; // 最多支持10个分辨率方案，

    int iRet = MediaComm_ParseVideoParam(codec.codec_name, videoExt, resolutionArr, iArrSize);

    if (iRet > 0)
    {
        int i = 0;
        for (; i < iRet; i++)
        {
            if (i == iRet - 1)
            {
                // 如果已经找到最后一个，那就使用最后一个
                break;
            }

            // 主流要求帧率比较高，辅流要求分辨率比较高
            if (bfpsPrior)
            {
                if (resolutionArr[i].m_width <= resLimit.m_width
                        && resolutionArr[i].m_height <= resLimit.m_height
                        && resolutionArr[i].m_fps >= resLimit.m_fps)
                {
                    // http://10.3.5.199/Bug.php?BugID=71383 要求30帧
                    break;
                }
                continue;
            }


            if (resolutionArr[i].m_width >= resLimit.m_width
                    && resolutionArr[i].m_height >= resLimit.m_height
                    && resolutionArr[i].m_fps <= resLimit.m_fps)
            {
                break;
            }
        }
        //VOICE_INFO("codec list index[%d]", i);

        resLimit.m_fps = resolutionArr[i].m_fps;
        resLimit.m_width = resolutionArr[i].m_width;
        resLimit.m_height = resolutionArr[i].m_height;
    }

    codec.codec_frmrate = resLimit.m_fps;
    codec.win_width = resLimit.m_width;
    codec.win_height = resLimit.m_height;

    if (strcmp("H264", codec.codec_name) == 0)
    {
        codec.codec_profile =
            videoExt.m_iProfileIDC; // ipvp: 0x42表示base profile 0x66 表示high profile
        codec.codec_pmode   = videoExt.m_iPacketizationMode == 1 ? IPVP_VIDEO_PM_MSLICES :
                              IPVP_VIDEO_PM_SSLICES;
    }

#if 1
    VOICE_INFO("~~~~~~~~~Video codec param~~~~~~~~~~~~");
    VOICE_INFO("name[%s] ptype[%d] fec[%d] bitrate [%d]", codec.codec_name, codec.codec_ptype,
               codec.codec_fec,
               codec.codec_bitrate);
    VOICE_INFO("frmrate[%d] profile[%d] pmode[%d] pmode[%d]", codec.codec_frmrate, codec.codec_profile,
               codec.codec_profile,
               codec.codec_pmode);
    VOICE_INFO("clock[%d] event_ptype[%d] event_clock[%d] width[%d] height[%d] fps[%d]",
               codec.codec_clock, codec.event_ptype,
               codec.event_clock, codec.win_width, codec.win_height, codec.codec_frmrate);
#endif
    return true;
}

bool CVideoManager::OnStarVideo(int iCallId)
{
    VOICE_INFO("Video on start video [%d]", iCallId);
    CCallInfo * pInfo = talklogic_FindCallInfoByCallId(iCallId);
    if (NULL == pInfo)
    {
        VOICE_INFO("Video not found call info");
        return false;
    }

    if (!pInfo->IsVideoTalk())
    {
        // http://10.2.1.199/Bug.php?BugID=93654
        // SP2：hold情况下也可以开通话
        VOICE_INFO("not video");
        return false;
    }

    VideoInfo * pVideoInfo = GetVideoInfo(pInfo->GetCallID());
    if (NULL != pVideoInfo)
    {
        VOICE_ERR("Video started");
        return true;
    }

    VideoInfo info;
    info.iCallId = pInfo->GetCallID();
    info.videoInfo = pInfo->m_tVPMParam.video;

    GetIpvpVideoInfo(info.videoInfo, pInfo);

    m_listVideo.push_back(info);
#if 1
    TALK_INFO("~~~~~~~~~~~~Video stream param:~~~~~~~~~~~");
    TALK_INFO("	flags [%d]", info.videoInfo.flags);
    TALK_INFO("	size [%d]", info.videoInfo.size);
    TALK_INFO("	dec_max [%d]", info.videoInfo.dec_max);
    TALK_INFO("	enc_max [%d]", info.videoInfo.enc_max);
    TALK_INFO("	enc_key [%s]", info.videoInfo.enc_key);
    TALK_INFO("	dec_key [%s]", info.videoInfo.dec_key);
    TALK_INFO("	RTCP_params [%s]", info.videoInfo.RTCP_params);
    TALK_INFO("	H460_params [%s]", info.videoInfo.H460_params);
    TALK_INFO("	priv_capset [%s]", info.videoInfo.priv_capset);
#endif

    // 先开启摄像头
    AdjustCameraMode();
#ifdef _T49
    ipvp_video_start(mkGetHandle(), pInfo->GetCallID(), &info.videoInfo, 0);
#endif

    // 开启视频更新灯
    UpdateLight();
    return true;
}

bool CVideoManager::OnStopConf(int iSession)
{
    return true;
}

bool CVideoManager::OnStopVideo(int iCallId)
{
    bool bDelete = DeleteVideo(iCallId);

    if (bDelete)
    {
        // H323 会出现先开视频，在开音频：如果只开视频，再关闭，视频需要自己关闭
        ipvp_video_stop(mkGetHandle(), iCallId, 0);
    }

    // 停止视频时，会出现当前还在talkroutine状态，此处延迟更新
    etl_NotifyApp(false, MEDIA_MSG_CAMERA_UPDATE, 0, 0);

    return true;
}

bool CVideoManager::OnStopVideoEx(int iCallId)
{
    VOICE_INFO("Video on stop video [%d]", iCallId);
    OnStopVideo(iCallId);
    ipvp_video_stop(mkGetHandle(), iCallId, 0);
    return true;
}

bool CVideoManager::ResetVideo(VideoInfo * pVideoInfo, CCallInfo * pCallInfo)
{
    if (NULL == pVideoInfo
            || NULL == pCallInfo)
    {
        return false;
    }

#ifdef _T49
    // 已经存在Video，则重置参数
    ipvp_video_t video;
    memset(&video, 0, sizeof(ipvp_video_t));
    GetIpvpVideoInfo(video, pCallInfo);

    if (0 != memcmp(&video, &pVideoInfo->videoInfo, sizeof(ipvp_video_t)))
    {
        VOICE_INFO("reset video param");
        // 保存新视频参数
        memcpy(&pVideoInfo->videoInfo, &video, sizeof(ipvp_video_t));

        ipvp_video_modify(mkGetHandle(), pCallInfo->GetCallID(), &video, 0);
        VOICE_INFO("video modify flag [%d][%d]", pCallInfo->GetCallID(), video.flags);
    }
#endif
    return true;
}

bool CVideoManager::OnResetVideo(int iCallId)
{
    VOICE_INFO("Video on reset video [%d]", iCallId);
    CCallInfo * pInfo = talklogic_FindCallInfoByCallId(iCallId);
    if (NULL == pInfo)
    {
        VOICE_INFO("Video not found call info");
        return false;
    }

    if (!pInfo->IsVideoTalk())
    {
        VOICE_INFO("Not video talk");
        return false;
    }

    VideoInfo * pVideoInfo = GetVideoInfo(pInfo->GetCallID());
    if (NULL != pVideoInfo)
    {
        ResetVideo(pVideoInfo, pInfo);
        return true;
    }

    // 重置视频参数时，会出现未开启通话的情况，此时直接开启视频
    OnStarVideo(iCallId);
    return true;
}

// 调整发送分辨率
bool CVideoManager::ResetVideoSize(int iCallId)
{
    VOICE_INFO("Video on reset video size[%d]", iCallId);
    CCallInfo * pCallInfo = talklogic_FindCallInfoByCallId(iCallId);
    if (NULL == pCallInfo)
    {
        VOICE_INFO("Video not found call info");
        return false;
    }

    if (!pCallInfo->IsVideoTalk()
            || pCallInfo->IsLocalHold())
    {
        VOICE_INFO("video talk[%d] local hold[%d]", pCallInfo->IsVideoTalk(), pCallInfo->IsLocalHold());
        return false;
    }

    VideoInfo * pVideoInfo = GetVideoInfo(pCallInfo->GetCallID());
    if (NULL == pVideoInfo)
    {
        VOICE_INFO("pVideoInfo is null");
        return false;
    }

    ipvp_video_t video;
    memset(&video, 0, sizeof(ipvp_video_t));
    GetIpvpVideoInfo(video, pCallInfo);

    if (0 != memcmp(&video, &pVideoInfo->videoInfo, sizeof(ipvp_video_t)))
    {
        memcpy(&pVideoInfo->videoInfo, &video, sizeof(ipvp_video_t));

        int iWidth = 0;
        int iHeight = 0;
        for (int iIndex = 0; iIndex < video.enc_max && iIndex < IPVP_MAX_CODEC; ++iIndex)
        {
            if (video.enc_list[iIndex].win_width >= iWidth
                    && video.enc_list[iIndex].win_height >= iHeight)
            {
                // 取最大发送分辨率
                iWidth = video.enc_list[iIndex].win_width;
                iHeight = video.enc_list[iIndex].win_height;
            }
        }

        ipvp_quality_t  quality;
        memset(&quality, 0, sizeof(quality));
        quality.size = sizeof(ipvp_quality_t);
        quality.flag = IPVP_QUALITY_WIDTH | IPVP_QUALITY_HEIGHT;
        quality.width = iWidth;
        quality.height = iHeight;

        VOICE_INFO("video modify width[%d] height[%d]", iWidth, iHeight);
#ifdef _T49
        ipvp_video_set_quality(mkGetHandle(), pCallInfo->GetCallID(), IPVP_VIDEO_ID_SPEECH, &quality, 0);
#endif
    }

    return true;
}

bool CVideoManager::OnSendKeyframe(int iCallId, int iVideoType, char * pInfo)
{
    VOICE_INFO("Video ReqKeyframe type[%d]", iVideoType);
    if (NULL != pInfo)
    {
        VOICE_INFO("Keyfram [%s]", pInfo);
    }

#ifdef _T49
    ipvp_video_req_keyframe(mkGetHandle(), iCallId, IPVP_VIDEO_ID_SPEECH, 0);
#endif
    return true;
}

// 设置带宽
bool CVideoManager::SetVideoBandwidth(int iIn, int iOut)
{
    VOICE_INFO("Video bandwidth change in[%d] out[%d]", iIn, iOut);
#ifdef _T49
    ipvp_config_bandwidth(mkGetHandle(), iIn, iOut);
#endif
    return true;
}

// 请求关键帧
bool CVideoManager::OnReqKeyframe(int iCallId, int iVideoType)
{
    VOICE_INFO("Ipvp request keyframe call[%d] type[%d]", iCallId, iVideoType);

    CCallInfo * pCallInfo = talklogic_FindCallInfoByCallId(iCallId);
    if (NULL == pCallInfo)
    {
        return false;
    }

    modVoip_RequestKeyFrame(pCallInfo->GetAccountID(), iCallId, (iVideoType == IPVP_VIDEO_ID_SHARED));

    return true;
}

bool CVideoManager::SetVideoLayout(DisplayMonitor * pDisplay)
{
    VOICE_INFO("Video Layout Change size[%d]", pDisplay->m_listLayout.size());

    ipvp_layout_t layoutLCD[VIDEOLAYOUTSIZE];
    ipvp_layout_t layoutHDMI[VIDEOLAYOUTSIZE];

    int iLCDCount = 0;
    int iHDMICount = 0;

    for (LayoutWindowList::iterator it = pDisplay->m_listLayout.begin();
            it != pDisplay->m_listLayout.end(); ++it)
    {
        LayoutWindow & window = *it;
        ipvp_layout_t layout;
        memset(&layout, 0, sizeof(ipvp_layout_t));

        layout.window.left   = (unsigned short)window.m_rectVideo.left;
        layout.window.top    = (unsigned short)window.m_rectVideo.top;
        layout.window.right  = (unsigned short)window.m_rectVideo.right;
        layout.window.bottom = (unsigned short)window.m_rectVideo.bottom;
        layout.callID  = window.m_iCallId;

        switch (window.m_eLayoutType)
        {
        case LT_VIDEO:
            {
                layout.videoID  = IPVP_VIDEO_ID_SPEECH;
            }
            break;
        case LT_CAMERA:
            {
                layout.videoID = IPVP_VIDEO_ID_CAMERA;
            }
            break;
        case LT_FILE:
            {
                layout.videoID = IPVP_VIDEO_ID_FILE;
            }
            break;
        case LT_EXTVIDEO:
            {
                layout.videoID = IPVP_VIDEO_ID_SHARED;
            }
            break;
        default:
            VOICE_ERR("Not support layout type[%d]", window.m_eLayoutType);
            continue;
        }

        VOICE_INFO("CallId[%d] videoDst[%d] win[l, t, r, b] [%d %d %d %d]", layout.callID, layout.videoID,
                   layout.window.left, layout.window.top, layout.window.right, layout.window.bottom);

        if (window.m_eDisplayDevice == DDT_LCD)
        {
            // 显示在LCD
            layoutLCD[iLCDCount++] = layout;
        }
        else if (window.m_eDisplayDevice == DDT_HDMI)
        {
            // 显示在HDMI
            layoutHDMI[iHDMICount++] = layout;
        }

        if (iLCDCount >= VIDEOLAYOUTSIZE || iHDMICount >= VIDEOLAYOUTSIZE)
        {
            VOICE_INFO("Error not support layout count");
            break;
        }
    }

    int bRet = false;

#ifdef _T49
    if (pDisplay->m_eDisplay & VDD_LCD)
    {
        bRet = ipvp_video_set_layout(mkGetHandle(), IPVP_VIDEO_DST_LCD, layoutLCD, iLCDCount, 0);
    }

    if (pDisplay->m_eDisplay & VDD_HDMI)
    {
        bRet = ipvp_video_set_layout(mkGetHandle(), IPVP_VIDEO_DST_HDMI2, layoutHDMI, iHDMICount, 0);
    }
#endif
    VOICE_INFO("Layout Device [%d] LCDcount[%d] Hdmi[%d]", pDisplay->m_eDisplay, iLCDCount, iHDMICount);
    return bRet;
}

bool CVideoManager::PreviewVideo(DisplayMonitor * pDisplay, bool bOpen)
{
    // 预览红色灯/关闭绿色
    UpdateLight(bOpen);

    // 开关摄像头
    AdjustCameraMode(bOpen);

    if (!bOpen
            && NULL != pDisplay
            && pDisplay->m_listLayout.size() > 0)
    {
        pDisplay->Reset();
    }
    return SetVideoLayout(pDisplay);
}

void CVideoManager::Uninit()
{

}

void CVideoManager::AdjustCameraMode(bool bPreview/* = false*/)
{
    bool bMuteVideo = talklogic_GetMuteStatus(true) || modhotplug_GetCameraStatus() != CST_READY;

    // 开启视频通话的时候，会出现还没进入talkrouting
    // 进入TalkRouting的时候，也会出现没开启通话的情况（先hold在进入）
    bool bVideoTalk = (talklogic_IsVideoSessionExist() || m_listVideo.size() > 0);

    //1.预览视频 或者 通话中不开启 standby
    //2.视频通话Mute状态就要发特殊黑屏的数据流
    modhotplug_CtrlCamera((bPreview || bVideoTalk/* && !bMuteVideo*/),
                          bVideoTalk && bMuteVideo);

    return;
}

void CVideoManager::UpdateLight(bool bPreview/* = false*/)
{
    VOICE_INFO("Update camera light");
    // 摄像头有两个灯，对应灯号5(绿)、6（红）
    bool bEnable = (modhotplug_GetCameraStatus() == CST_READY);

    // Ringing闪灯,需求要求不支持视频时，不闪灯
    bool bIncoming = talklogic_IsVideoRingSessionExist()
                     && configParser_GetConfigInt(kszVideoTalkEnable) != TVO_DISABLE;
    ledlamp_PostEvent(LAMP_EVENTTYPE_VIDEO_INCALL, bEnable && bIncoming);

    //VOICE_INFO("Update camera light incoming[%d]", bIncoming);

    // hold 红色慢闪
    bool bLight = talklogic_BeHaveHoldVideoSession();
    ledlamp_PostEvent(LAMP_EVENTTYPE_VIDEO_HOLD, bEnable && !bIncoming && bLight);

    //VOICE_INFO("Update camera light hold[%d]", bLight);

    // Mute 红色慢闪
    bLight = talklogic_GetMuteStatus(true);
    ledlamp_PostEvent(LAMP_EVENTTYPE_VIDEO_MUTE, bEnable && !bIncoming && bLight);

    //VOICE_INFO("Update camera light mute[%d]", bLight);

    // Talk 红色常亮
    bLight = (bPreview || talklogic_IsVideoSessionExist() /*|| m_listVideo.size() > 0*/);
    ledlamp_PostEvent(LAMP_EVENTTYPE_VIDEO_TALKING, bEnable && !bIncoming && bLight);

    //VOICE_INFO("Update camera light talking[%d]", bLight);

    // idle 绿色常亮
    ledlamp_PostEvent(LAMP_EVENTTYPE_VIDEO_IDLE, bEnable && !bIncoming && !bLight);
}

// 获取通话id对应的视频信息
VideoInfo * CVideoManager::GetVideoInfo(int iCallId)
{
    YLList<VideoInfo>::iterator it = m_listVideo.begin();
    for (; it != m_listVideo.end(); ++it)
    {
        VideoInfo & info = (*it);
        if (info.iCallId == iCallId)
        {
            return &info;
        }
    }

    return NULL;
}

// 去掉指定话路视频信息
bool CVideoManager::DeleteVideo(int iCallId)
{
    YLList<VideoInfo>::iterator it = m_listVideo.begin();
    for (; it != m_listVideo.end(); ++it)
    {
        VideoInfo & info = (*it);
        if (info.iCallId == iCallId)
        {
            VOICE_INFO("Delete video[%d]", iCallId);
            m_listVideo.erase(it);
            return true;
        }
    }

    return false;
}

//更新话路信息
bool CVideoManager::UpdateStats()
{
    bool bUpdate = false;
    YLList<VideoInfo>::iterator it = m_listVideo.begin();
    for (; it != m_listVideo.end(); ++it)
    {
        VideoInfo & info = (*it);

        //只计算Talking 的那路
        CCallInfo * pCallInfo = talklogic_GetCallInfoByCallId(info.iCallId);
        if (pCallInfo == NULL || !pCallInfo->IsTalkStart())
        {
            continue;
        }

        bUpdate |= UpdateStats(info.iCallId, &info);
    }

    return bUpdate;
}

bool CVideoManager::UpdateStats(int iCallID, VideoInfo * pVideoInfo)
{
    if (iCallID <= 0
            || NULL == pVideoInfo)
    {
        return false;
    }

    MediaStatsInfo mediaStats;

    GetStats(iCallID, mediaStats);

    return _MediaManager.ProcessStats((void *)pVideoInfo, mediaStats, true);
}

bool CVideoManager::OnStartShareVideo(int iCallId)
{
    CCallInfo * pInfo = talklogic_FindCallInfoByCallId(iCallId);
    if (NULL == pInfo)
    {
        VOICE_INFO("Video not found call info[%d]", iCallId);
        return false;
    }

    if (!pInfo->IsVideoTalk())
    {
        VOICE_INFO("start share not video");
        return false;
    }

    VideoInfo * pVideoInfo = GetVideoInfo(pInfo->GetCallID());
    if (NULL == pVideoInfo)
    {
        VOICE_ERR("Video not started");
        return true;
    }

    // 获取辅流参数
    VideoInfo info;
    memset(&info, 0, sizeof(VideoInfo));
    info.iCallId = pInfo->GetCallID();
    GetIpvpShareInfo(info.videoInfo, pInfo);
    info.videoInfo.flags = IPVP_ADDR_FL_RECVONLY;
    //memset(info.videoInfo.enc_list, 0, sizeof(ipvp_vcodec_t));

    VOICE_INFO("-------------video info begin--------------");
    VOICE_INFO("flags = %d", info.videoInfo.flags);
    VOICE_INFO("dec_max = %d", info.videoInfo.dec_max);
    VOICE_INFO("enc_max = %d", info.videoInfo.enc_max);
    VOICE_INFO("enc_key = %s", info.videoInfo.enc_key);
    VOICE_INFO("dec_key = %s", info.videoInfo.dec_key);
    VOICE_INFO("RTCP_params = %s", info.videoInfo.RTCP_params);
    VOICE_INFO("H460_params = %s", info.videoInfo.H460_params);
    VOICE_INFO("priv_capset = %s", info.videoInfo.priv_capset);
    VOICE_INFO("-------------video info end--------------");
    //if (pInfo->m_bEstablishShare)
    {
        // 调用VPM接口开启辅流传输
        ipvp_shared_start(mkGetHandle(), info.iCallId, &info.videoInfo, 0);
        if (pInfo->IsLocalHold())
        {
            ipvp_shared_hold(mkGetHandle(), info.iCallId, 1, 0);
        }
        return true;
    }

    return false;
}

bool CVideoManager::OnStopShareVideo(int iCallId)
{
    VOICE_INFO("Stop Share Video[%d][%d]", iCallId, talklogic_IsExtVideoTalk(iCallId));
    if (talklogic_IsExtVideoTalk(iCallId))
    {
        // 调用VPM接口停止辅流传输
        return !ipvp_shared_stop(mkGetHandle(), iCallId, 0);
    }

    return false;
}

bool CVideoManager::OnVideoResized(int iCallId, VideoDirection eDirec, ipvp_resize_t * pResize)
{
    if (NULL == pResize)
    {
        return false;
    }

    //bool isHeightMode = pResize->new_height > pResize->new_width;
    VideoInfo * pVideoInfo = GetVideoInfo(iCallId);
    if (NULL == pVideoInfo)
    {
        VOICE_ERR("Video not start");
        return false;
    }

    if (pVideoInfo->stRecvRes.m_height != pResize->new_height
            || pVideoInfo->stRecvRes.m_width != pResize->new_width)
    {
        pVideoInfo->stRecvRes.m_height = pResize->new_height;
        pVideoInfo->stRecvRes.m_width = pResize->new_width;

        // 刷新界面：UI有更新布局
        talklogic_UpdateWnd();
    }

//#if DEBUG_MEDIA
    VOICE_INFO("video real resolution h[%d] w[%d]", pResize->new_height, pResize->new_width);
//#endif //DEBUG_MEDIA

    return true;
}

#endif //IF_SUPPORT_VIDEO
