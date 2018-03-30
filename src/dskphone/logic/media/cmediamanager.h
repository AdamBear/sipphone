///////////////////////////////////////////////////////////
//  CMediaManager.h
//  Implementation of the Class CMediaManager
//  Created on:      06-二月-2015 10:10:26
//  Original author: huangwz
///////////////////////////////////////////////////////////
#ifndef _MEDIA_MANAGER_H_
#define _MEDIA_MANAGER_H_

#ifdef _T49
#include <ETLLib.hpp>
#include "etlmsghandler/modetlmsghandler.h"
#include "singletonclassdefine.h"
#include "cvideomanager.h"
#include "commondata.h"

typedef YLList< CBaseMedia *>   MediaList;

class CMediaManager : public CBaseMedia
{
    DECLARE_SINGLETON_CLASS(CMediaManager)
public:

    void Init();

    void Uninit();

    // 定时器
    bool OnTimer(UINT uTimeID);

    static LRESULT OnMessage(msgObject & objMsg);

    bool GetTalkStatistics(TalkStatisticsInfo & info);

    bool GetTalkStatistics(YLList<SessionInfo> & listInfo);

    virtual bool OnEvent(MediaEventData * pData);

    virtual bool GetStats(int iCallId, MediaStatsInfo & statsInfo);

    bool GetEventData(msgObject & objMsg, MediaEventData & data);

    bool SetVideoLayout(DisplayMonitor & display);

    bool SetVideoLayout();

    bool PreviewVideo(DisplayMonitor & display, bool bOpen);

    // 降低视频能力
    bool ReduceVideoPower();

    // 恢复视频能力
    bool ResumeVideoPower();

    void OnIpvpStarted();

    bool ProcessStats(void * pInfo, const MediaStatsInfo & mediaStat, bool bVideo = true);

    // 取通话对端的视频分辨率
    bool GetVideoResolution(int iCallId, VideoSizeInfo & stInfo);

    // 设置显示分辨率
    bool SetDisplay(int iWidth, int iHeight, int iFps, int videoDST);

    // 建立会议
    void StartConf(const YLList<int> & listCallID);

    // 停止会议
    void StopConf(const YLList<int> & listCallID);

private:
    bool IsSupportVideo();

    // 调整音视频芯片运行状态
    void AdjustDspPower(bool bStart = false, int iCallId = -1);

    void SetUpdateStatTimer();

    void CancleUpdateStatTimer();

    void UpdateStat();

    // 取视频处理
    CVideoManager * GetVideoProcessor();
private:
    // 处理事件
    MediaList       m_listProcessor;
    // 音视频芯片运行状态
    int             m_iDspMode;

    int             m_iTimerDelayAdjustDsp;

    // 丢包率检测
    int             m_iTimerUpdateStat;

    DisplayMonitor  m_displayLayout;
};

#define _MediaManager GET_SINGLETON_INSTANCE(CMediaManager)
#endif //#ifdef _T49
#endif // _MEDIA_MANAGER_H_
