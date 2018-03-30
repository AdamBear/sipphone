///////////////////////////////////////////////////////////
//  CVideoManager.h
//  Implementation of the Class CVideoManager
//  Created on:      05-二月-2015 21:05:04
//  Original author: huangwz
///////////////////////////////////////////////////////////

#ifndef _VIDEO_MESSAGE_H_
#define _VIDEO_MESSAGE_H_

#include <ETLLib.hpp>
#include "service_ipvp_types.h"
#include "commondefine.h"
#include "commondata.h"
#include "mediacommonfun.h"

#ifdef IF_SUPPORT_VIDEO

class CCallInfo;

struct VideoInfo
{
    int  iCallId;
    // 是否丢包状态
    bool bLostFlac;
    // 防抖计数
    int  iConut;

    // 接收到的视频分辨率
    VideoResolutionParam stRecvRes;

    ipvp_video_t videoInfo;

    VideoInfo()
    {
        iCallId = -1;
        bLostFlac = false;
        iConut = 0;

        //memset(&stRec, 0, sizeof(VideoResolutionParam));
        //memset(&videoInfo, 0, sizeof(ipvp_video_t));
    }
};

enum VideoPowerType
{
    VPT_NONE,
    VPT_FULL,       // 不限制视频1080P
    VPT_SAVING_720, // 限制720
};

enum VideoDirection
{
    VD_REC = 1,     // 视频方向：接收
    VD_SEND = 2,    // 发送
};

class CVideoManager : public CBaseMedia
{
public:
    CVideoManager();
    ~CVideoManager();

public:

    virtual bool OnEvent(MediaEventData * pData);

    virtual bool GetStats(int iCallId, MediaStatsInfo & statsInfo);

    // 取统计信息
    bool GetIpvpStats(int iCallId, VideoStatsInfo & statsInfo, bool bShare);

    // 获取通话数量
    int GetVideoNum();

    // 是否存在1080p的通话
    bool IsVideoExist(VideoSizeType eType);

    // 分辨率类型转成分辨率
    void GetRealVideoResolution(int iCallId, VideoResolutionParam & stInfo);

    // 设置显示分辨率
    bool SetDisplay(int iWidth, int iHeight, int iFps, int videoDST);

    // 建立会议
    void StartConf(const YLList<int> & listCallID);

    // 停止会议
    void StopConf(const YLList<int> & listCallID);

private:
    void Init();
    void Uninit();

    bool OnHold(int iCallId);

    bool OnMuteVideo(int iCallId, bool bNeedSleep);

    // 更新OSD层
    bool OnOsdChange(OsdLayoutInfo * pInfo);

    // 开启会议
    bool OnStartConf();

    // 开启视频
    bool OnStarVideo(int iCallId);

    bool OnStopConf(int iSession);

    bool OnStopVideo(int iCallId);

    bool OnStopVideoEx(int iCallId);

    bool OnResetVideo(int iCallId);

    bool ResetVideoSize(int iCallId);

    // 发送关键帧
    bool OnSendKeyframe(int iCallId, int iVideoType, char * pInfo);

    // 请求关键帧
    bool OnReqKeyframe(int iCallId, int iVideoType);

    // 设置ipvp带宽
    bool SetVideoBandwidth(int iIn, int iOut);

    // 设置布局
    bool SetVideoLayout(DisplayMonitor * pDisplay);

    // 预览本地视频
    bool PreviewVideo(DisplayMonitor * pDisplay, bool bOpen);

    // 调整分辨率
    //int AdjustFrameRate(int iFrameRate);

    // 分辨率类型转成分辨率
    void GetVideSize(VideoSizeType eType, int & iWidth, int & iHeight);

    void MapFrameSizeToResolution(int iFs, int & iWidth, int & iHeight);

    void UpdateLight(bool bPreview = false);

    // 调整Camera运行模式
    void AdjustCameraMode(bool bPreview = false);

    VideoInfo * GetVideoInfo(int iCallId);

    bool GetH264LevelIndex(const int level, int & iIndex);

    // 调整视频能力 1恢复 0降低
    bool OnAdjustPower(int iAction);

    bool DeleteVideo(int iCallId);

    // 取最大的编解码分辨率
    void GetMaxResolution(VideoResolutionParam & resolution, bool bEncode, int iAccount, int iCallId,
                          bool bShare = false);

    bool FetchResolution(ipvp_vcodec_t & codec, const VideoCodecExtParam & videoExt,
                         VideoResolutionParam resLimit, bool bfpsPrior = true);

    bool GetIpvpVideoInfo(ipvp_video_t & video, CCallInfo * pCallInfo);

    bool GetIpvpShareInfo(ipvp_video_t & video, CCallInfo * pCallInfo);

    void GetIpvpOsdParam(int iPosx, int iPosy, const yl::string & strOsdFile, ipvp_osd_t & osdParam);

    // 请求比特率
    bool OnReqBitrate(int iCallId, int iVideoType, int iBandwidth);

    // 设置比特率
    bool ResetBitrate(int iCallId, int iVideoType, int iBitrate);

    bool OnStartShareVideo(int iCallId);

    bool OnStopShareVideo(int iCallId);

    //  保存实际生效的分辨率
    bool OnVideoResized(int iCallId, VideoDirection eDirec, ipvp_resize_t * pResize);

private:
    bool ResetVideo(VideoInfo * pVideoInfo, CCallInfo * pCallInfo);

    bool UpdateStats();

    bool UpdateStats(int iCallID, VideoInfo * pVideoInfo);
private:

    YLList<VideoInfo> m_listVideo;

    VideoPowerType m_ePower;    // 编码视频能力
};

#endif // IF_SUPPORT_VIDEO
#endif // _VIDEO_MESSAGE_H_
