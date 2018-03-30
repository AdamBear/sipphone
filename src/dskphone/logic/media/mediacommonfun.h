#ifndef _MEDIA_COMMON_FUN_H_
#define _MEDIA_COMMON_FUN_H_
#include "commondefine.h"

#ifdef IF_SUPPORT_VIDEO

// 视频分辨率
struct VideoResolutionParam
{
    int m_width;
    int m_height;
    int m_fps;

    VideoResolutionParam()
    {
        Reset();
    }

    void Reset()
    {
        m_width = 0;
        m_height = 0;
        m_fps = 0;
    }
};

enum ResolutionSize
{
    SIZE_448P,
    SIZE_720P,
    SIZE_1080P,
};

// 解析分辨率
int MediaComm_ParseVideoParam(const char * pCodec, const VideoCodecExtParam & videoExt,
                              VideoResolutionParam ResolutionArr[], int iArrSize);

void MediaComm_GetMaxVideoResolution(VideoResolutionParam & resolution, ipvp_video_t & video,
                                     VideoExtInfo & videoExt,
                                     VideoStreamInfo & videoStream, bool bEncode/* = false*/);

// 判断ipvp_video_t是否支持H264
bool MediaComm_IsSupportH264(ipvp_video_t & video);

bool MediaComm_CompareResolution(VideoResolutionParam resolution, ResolutionSize ResSize);
#endif // IF_SUPPORT_VIDEO

#endif //_MEDIA_COMMON_FUN_H_
