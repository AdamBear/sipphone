#include "modmedia.h"
#include "cmediamanager.h"

#ifdef IS_MEDIA_TEST
#include "mediatest.h"
#endif // test

#ifdef _T49
// 后续黑白屏也会使用media，先用宏隔开


void modmedia_Init()
{
    _MediaManager.Init();
#ifdef IS_MEDIA_TEST
    CMediaTest::GetInstance().init();
#endif IS_MEDIA_TEST
}

void modmedia_SetVideoLayout(DisplayMonitor & display)
{
    _MediaManager.SetVideoLayout(display);
}

void modmedia_SetVideoLayout()
{
    _MediaManager.SetVideoLayout();
}

void modmedia_CtrlLocalVideo(DisplayMonitor & display, bool bOpen)
{
    _MediaManager.PreviewVideo(display, bOpen);
}

void modmedia_ReducePower()
{
    _MediaManager.ReduceVideoPower();
}

void modmedia_ResumePower()
{
    _MediaManager.ResumeVideoPower();
}

void modmedia_GetStats(YLList<SessionInfo> & listInfo)
{
    _MediaManager.GetTalkStatistics(listInfo);
}

bool modmedia_GetStats(int iCallId, MediaStatsInfo & statsInfo)
{
    _MediaManager.GetStats(iCallId, statsInfo);

    // UI 要求找不到call info就返回false
    return (iCallId == statsInfo.iCallId);
}

bool modmedia_GetResolution(int iCallId, VideoSizeInfo & stInfo)
{
    return _MediaManager.GetVideoResolution(iCallId, stInfo);
}

bool modmedia_SetDisplay(int iWidth, int iHeight, int iFps, int videoDST /*= IPVP_VIDEO_DST_HDMI2*/)
{
    return _MediaManager.SetDisplay(iWidth, iHeight, iFps, videoDST);
}

void modmedia_StartConf(const YLList<int> & listCallID)
{
    _MediaManager.StartConf(listCallID);
}

void modmedia_StopConf(const YLList<int> & listCallID)
{
    _MediaManager.StopConf(listCallID);
}

#endif // _T49