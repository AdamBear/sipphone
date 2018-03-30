#include "mediacommonfun.h"
#include "dsklog/log.h"

#ifdef IF_SUPPORT_VIDEO
enum
{
    H264_COL_LEVEL,     // LEVEL Form UA
    H264_COL_MAX_MBPS,  // Max macroblock processing rate,MB/s
    H264_COL_MAX_FS,        // Max frame size,MBs

    H264_COLS_NUM
};

enum H264_LEVEL_NUMBER
{
    H264_LEVEL_1,       // 64Kbps,  correspond to 15
    H264_LEVEL_1b,  // 128Kbps, correspond to 19
    H264_LEVEL_1_1, // 192Kbps, correspond to 22
    H264_LEVEL_1_2, // 384Kbps, correspond to 29
    H264_LEVEL_1_3, // 768Kbps, correspond to 36
    H264_LEVEL_2,       // 2Mbps,   correspond to 43
    H264_LEVEL_2_1, // 4Mbps,   correspond to 50
    H264_LEVEL_2_2, // 4Mbps,   correspond to 57
    H264_LEVEL_3,       // 10Mbps,  correspond to 64
    H264_LEVEL_3_1, // 14Mbps,  correspond to 71
    H264_LEVEL_3_2, // 20Mbps,  correspond to 78
    H264_LEVEL_4,       // 20Mbps,  correspond to 85
    H264_LEVEL_4_1, // 50Mbps,  correspond to 92
    H264_LEVEL_4_2, // 50Mbps,  correspond to 99
    H264_LEVEL_5,       // 135Mbps, correspond to 106
    H264_LEVEL_5_1, // 240Mbps, correspond to 113
    H264_LEVEL_5_2, // 240Mbps, correspond to 120

    H264_LEVEL_MAX
};

const unsigned long H264_LEVEL_LIMITS[H264_LEVEL_MAX][H264_COLS_NUM] =
{
    {10,    1485,   99},    // H264Level_1
    //{19,  1485,   99},    // H264Level_1b
    {11,    3000,   396},   // H264Level_1_1
    {12,    6000,   396},   // H264Level_1_2
    {13,    11880,  396},   // H264Level_1_3
    {20,    11880,  396},   // H264Level_2
    {21,    19800,  792},   // H264Level_2_1
    {22,    20250,  1620},  // H264Level_2_2
    {30,    40500,  1620},  // H264Level_3
    {31,    108000, 3600},  // H264Level_3_1
    {32,    216000, 5120},  // H264Level_3_2
    {40,    245760, 8192},  // H264Level_4
    {41,    245760, 8192},  // H264Level_4_1
    {42,    522240, 8704},  // H264Level_4_2
    {50,    589824, 22080}, // H264Level_5
    {51,    983040, 36864}, // H264Level_5_1
    {52,    2073600, 36864} // H264Level_5_2
};

// frame size in MBs, for each video size
const int H264_FS[VST_SIZE_MAX] =
{
    8100,  // VST_1080P
    3600,  // VST_720P
    1584,  // VST_4CIF
    396,   // VST_CIF

    5120,  // VST_SXGA
    3840,  // VST_WXGA_5_3
    3072,  // VST_XGA
    1875,  // VST_SVGA
    1200,  // VST_VGA

    2304,  // VST_W576P
    1344,  // VST_W448P
    375    // VST_WQVGA
};

void MapFrameSizeToResolution(int iFs, int & iWidth, int & iHeight)
{
    bool bHandled = true;
    switch (iFs)
    {
    case 8100:
    {
        iWidth = 1920;
        iHeight = 1080;
    }
    break;
    case 3600:
    {
        iWidth = 1280;
        iHeight = 720;
    }
    break;
    case 1584:
    {
        iWidth = 704;
        iHeight = 576;
    }
    break;
    case 396:
    {
        iWidth = 352;
        iHeight = 288;
    }
    break;
    case 5120:
    {
        iWidth = 1280;
        iHeight = 1024;
    }
    break;
    case 3840:
    {
        iWidth = 1280;
        iHeight = 768;
    }
    break;
    case 3072:
    {
        iWidth = 1024;
        iHeight = 768;
    }
    break;
    case 1875:
    {
        iWidth = 800;
        iHeight = 600;
    }
    break;
    case 1200:
    {
        iWidth = 640;
        iHeight = 480;
    }
    break;
    case 2304:
    {
        iWidth = 1024;
        iHeight = 576;

    }
    break;
    case 1344:
    {
        iWidth = 768;
        iHeight = 448;
    }
    break;
    case 375:
    {
        iWidth = 400;
        iHeight = 240;
    }
    break;
    default:
        break;
    }
}

void SwapBig2LittleSort(int * arr, int len)
{
    int i = 0;
    int j = 0;
    int temp = 0;
    for (i = 0; i < len - 1; ++i)
    {
        for (j = i + 1; j < len; ++j)
        {
            if (arr[i] < arr[j])
            {
                temp = arr[j];
                arr[j] = arr[i];
                arr[i] = temp;
            }
        }
    }
}

// 调整帧率
int AdjustFrameRate(int iFrameRate)
{
    // 视频引擎只支持不大于30帧
    if (iFrameRate >= 30)
    {
        return 30;
    }
    else if (iFrameRate >= 15)
    {
        return 15;
    }
    else if (iFrameRate >= 10)
    {
        return 10;
    }
    else if (iFrameRate >= 5)
    {
        return 5;
    }

    return 5;
}

bool GetH264LevelIndex(const int level, int & iIndex)
{
    //COMMONUNITS_INFO("Level [%d]", level);
    iIndex = H264_LEVEL_MAX;
    for (int i = 0; i < H264_LEVEL_MAX; i++)
    {
        if (H264_LEVEL_LIMITS[i][H264_COL_LEVEL] == level)
        {
            iIndex = i;
            break;
        }
    }

    if (H264_LEVEL_MAX == iIndex)
    {
        return false;
    }
    return true;
}

int MediaComm_ParseVideoParam(const char * pCodec, const VideoCodecExtParam & videoExt,
                              VideoResolutionParam ResolutionArr[], int iArrSize)
{
    if (NULL == pCodec)
    {
        return 0;
    }

#define VIDEO_MAX_FPS 30

    int iCount = 0;
    if (strcmp("H263", pCodec) == 0)
    {
        // 确定优先级
        if (videoExt.m_iCIF4 > 0 && iCount < iArrSize)
        {
            ResolutionArr[iCount].m_fps = VIDEO_MAX_FPS / videoExt.m_iCIF4;
            ResolutionArr[iCount].m_width = 704;
            ResolutionArr[iCount].m_height = 576;
            iCount++;
        }

        if (videoExt.m_iCIF > 0 && iCount < iArrSize)
        {
            ResolutionArr[iCount].m_fps = VIDEO_MAX_FPS / videoExt.m_iCIF;
            ResolutionArr[iCount].m_width     = 352;
            ResolutionArr[iCount].m_height    = 288;
            iCount++;
        }

        if (videoExt.m_iQCIF > 0 && iCount < iArrSize)
        {
            ResolutionArr[iCount].m_fps = VIDEO_MAX_FPS / videoExt.m_iQCIF;
            ResolutionArr[iCount].m_width    = 176;
            ResolutionArr[iCount].m_height    = 144;
            iCount++;
        }

        if (videoExt.m_iSQCIF > 0 && iCount < iArrSize)
        {
            ResolutionArr[iCount].m_fps = VIDEO_MAX_FPS / videoExt.m_iSQCIF;
            ResolutionArr[iCount].m_width    = 128;
            ResolutionArr[iCount].m_height    = 96;
            iCount++;
        }
        //COMMONUNITS_INFO("H263 frmrate[%d] width[%d] height[%d]", codec.codec_frmrate, codec.win_width, codec.win_height);
    }
    else if (strcmp("H264", pCodec) == 0)
    {
        int iLevelIndex = H264_LEVEL_MAX;

        if (!GetH264LevelIndex(videoExt.m_iLevel, iLevelIndex))
        {
            VOICE_INFO("Trans H264 to video codec fail, contact ipvp");
            return 0;
        }

        int iMaxMBPS = 0;
        int iMaxFS = 0;

        // 协商值不为0，使用协商值（协商值一定大于等于定义值）
        iMaxMBPS = (videoExt.m_iMaxMBPS == 0) ?
                   (H264_LEVEL_LIMITS[iLevelIndex][H264_COL_MAX_MBPS]) : videoExt.m_iMaxMBPS;
        iMaxFS   = (videoExt.m_iMaxFS == 0) ?
                   (H264_LEVEL_LIMITS[iLevelIndex][H264_COL_MAX_FS]) : videoExt.m_iMaxFS;

        // 用于排序及优先级
        int arrFrameSize[VST_SIZE_MAX];
        for (int i = 0; i < (int)VST_SIZE_MAX; i++)
        {
            arrFrameSize[i] = H264_FS[i];
        }

        SwapBig2LittleSort(arrFrameSize, VST_SIZE_MAX);

//      VOICE_INFO("MaxMBPS[%d] MaxFS[%d]", iMaxMBPS, iMaxFS);

        for (int i = 0; i < VST_SIZE_MAX; i++)
        {
//          VOICE_INFO("count[%d] arrsize[%d] Fs[%d]", iCount, iArrSize, arrFrameSize[i]);
            if (iCount >= iArrSize)
            {
                break;
            }

            if (arrFrameSize[i] <= iMaxFS)
            {
                int iMaxFrameRate = iMaxMBPS / arrFrameSize[i];

                int iFrameRate = AdjustFrameRate(iMaxFrameRate);
                if (iFrameRate > 0)
                {
                    ResolutionArr[iCount].m_fps = iFrameRate;
                    MapFrameSizeToResolution(arrFrameSize[i], ResolutionArr[iCount].m_width,
                                             ResolutionArr[iCount].m_height);
                    iCount++;
                }
            }
        }
    }
    else
    {
        VOICE_INFO("Not support code[%s]", pCodec);
    }

    return iCount;
}

void GetMaxVideoResolution(ipvp_vcodec_t & codec, VideoCodecExtParam & videoExt,
                           VideoResolutionParam & resolution)
{
    VideoResolutionParam resolutionArr[1];
    int iArrSize = 1;

    int iRet = MediaComm_ParseVideoParam(codec.codec_name, videoExt, resolutionArr, iArrSize);
    if (iRet > 0)
    {
        resolution.m_fps = resolutionArr[0].m_fps;
        resolution.m_width = resolutionArr[0].m_width;
        resolution.m_height = resolutionArr[0].m_height;
    }

    return;
}

void GetMaxVideoResolution(VideoResolutionParam & resolution,
                           ipvp_vcodec_t codecList[IPVP_MAX_CODEC],
                           VideoCodecExtParam & videoExt, VideoCodecList & codecListAva)
{
    // 取第一个解码codec中最大分辨率
    GetMaxVideoResolution(codecList[0], videoExt, resolution);

    VOICE_INFO("decodec 0 [%d] [%d] [%d]", resolution.m_width, resolution.m_height, resolution.m_fps);

    VideoCodecList::iterator iter = codecListAva.begin();
    VideoResolutionParam resolutionTmp;
    // 因为第0个codec是通话建立的消息携带，目前可用codec从1开始，以后合并统一处理
    int iCodec = 1;
    for (; iter != codecListAva.end() && iCodec < IPVP_MAX_CODEC; ++iter)
    {
        ipvp_vcodec_t & codec = (*iter).codec;
        VideoCodecExtParam & ext = (*iter).extParam;
        GetMaxVideoResolution(codecList[iCodec], ext, resolutionTmp);
        VOICE_INFO("decodec %d [%d] [%d] [%d]", iCodec, resolution.m_width, resolution.m_height,
                   resolution.m_fps);

        iCodec++;
        if (resolutionTmp.m_width >= resolution.m_width
                && resolutionTmp.m_height >= resolution.m_height)
        {
            resolution = resolutionTmp;
        }
    }

    // 分辨率映射到类型
    return;
}

// 从video中取出最大的分辨率
// 注意：videoExt特殊处理，因为第一个视频codec的扩展参数目前只能特殊携带
void MediaComm_GetMaxVideoResolution(VideoResolutionParam & resolution, ipvp_video_t & video,
                                     VideoExtInfo & videoExt,
                                     VideoStreamInfo & videoStream, bool bEncode/* = false*/)
{
    if (bEncode)
    {
        GetMaxVideoResolution(resolution, video.enc_list, videoExt.m_stRemote, videoStream.listRemoteCodec);
    }
    else
    {
        GetMaxVideoResolution(resolution, video.dec_list, videoExt.m_stLocal, videoStream.listLocalCodec);
    }

    return;
}

// 是否支持H264
bool MediaComm_IsSupportH264(ipvp_video_t & video)
{
    int iCodec = 0;
    for (; iCodec < IPVP_MAX_CODEC; ++iCodec)
    {
        if (strcmp(video.dec_list[iCodec].codec_name, "H264") == 0)
        {
            return true;
        }
    }

    return false;
}

bool MediaComm_CompareResolution(VideoResolutionParam resolution, ResolutionSize ResSize)
{
    int iWidth = 0;
    int iHeight = 0;
    switch (ResSize)
    {
    case SIZE_1080P:
    {
        iWidth = 1920;
        iHeight = 1080;
    }
    break;
    case SIZE_720P:
    {
        iWidth = 1280;
        iHeight = 720;
    }
    break;
    case SIZE_448P:
    {
        iWidth = 704;
        iHeight = 576;
    }
    break;
    default:
        break;
    }

    return resolution.m_width * resolution.m_height * resolution.m_fps > iWidth * iHeight * 30;
//  return (resolution.m_width >= iWidth && resolution.m_height > iHeight)
//      || (resolution.m_width > iWidth && resolution.m_height >= iHeight);
}
#endif //
