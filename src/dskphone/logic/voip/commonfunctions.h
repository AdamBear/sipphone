#ifndef __VOIP_COMMON_FUNCTIONS_H__
#define __VOIP_COMMON_FUNCTIONS_H__
#include <ylstring.h>
#include "ETLLib.hpp"
#include "dsklog/log.h"

#define VLH264_LEVEL_MAX 16
#define VLH264_COLS_NUM 3
#define VLH264_INDEX_FS 2
#define VLH264_INDEX_MBPS 1
#define VLH264_INDEX_LEVEL 0

extern const unsigned long VCS_H264_LEVEL_LIMITS[VLH264_LEVEL_MAX][VLH264_COLS_NUM];

template<typename H264_PARAM>
bool set_h264_param(H264_PARAM & param, int w, int h, int fps, bool bCeil)
{
    param.max_fs = ((w + 15) / (float)16) * ((h + 15) / (float)16);
    param.max_mbps = param.max_fs * fps;

    int nCol = VLH264_LEVEL_MAX - 1;
    for (; nCol >= 0; nCol--)
    {
        //适配Level
        if (VCS_H264_LEVEL_LIMITS[nCol][VLH264_INDEX_FS] <= param.max_fs)
        {
            break;
        }
    }

    param.level = static_cast<unsigned char>(VCS_H264_LEVEL_LIMITS[chMIN(VLH264_LEVEL_MAX - 1,
                  nCol)][VLH264_INDEX_LEVEL]);
    param.max_fs = VCS_H264_LEVEL_LIMITS[chMIN(VLH264_LEVEL_MAX - 1, nCol)][VLH264_INDEX_FS];
    param.max_mbps = param.max_fs * fps;

    COMMONUNITS_INFO("set h264 param w[%d] h[%d] fps[%d] to max_fs[%d] max_mbps[%d] level[%d].",
                     w, h, fps,
                     param.max_fs, param.max_mbps, param.level);

    // H323因底层算法导致要取整,初步测试，不需要取整
//  if (bCeil)
//  {
//      param.max_fs = chCEIL(param.max_fs, 256);
//      param.max_mbps = chCEIL(param.max_mbps, 500);
//  }

//  COMMONUNITS_INFO("set h264 param w[%d] h[%d] fps[%d] to max_fs[%d] max_mbps[%d] level[%d].",
//      w, h, fps,
//      param.max_fs, param.max_mbps, param.level);

    return true;
}

template<typename H264_PARAM>
bool set_h264_param_by_config(H264_PARAM & param, const yl::string & strConfig)
{
    if (strConfig.length() < 6)
    {
        return false;
    }

    char szLevel[2] = "";
    szLevel[0] = strConfig.at(4);
    szLevel[1] = strConfig.at(5);

    int iLevel = 0;
    sscanf(szLevel, "%x", &iLevel);

    COMMONUNITS_INFO("set_h264_param_by_config[%s][%d]", szLevel, iLevel);

    int nCol = VLH264_LEVEL_MAX - 1;
    for (; nCol >= 0; nCol--)
    {
        //适配Level
        if (VCS_H264_LEVEL_LIMITS[nCol][VLH264_INDEX_LEVEL] <= iLevel)
        {
            break;
        }
    }

    param.level = static_cast<unsigned char>(VCS_H264_LEVEL_LIMITS[chMIN(VLH264_LEVEL_MAX - 1,
                  nCol)][VLH264_INDEX_LEVEL]);
    param.max_fs = VCS_H264_LEVEL_LIMITS[chMIN(VLH264_LEVEL_MAX - 1, nCol)][VLH264_INDEX_FS];
    param.max_mbps = param.max_fs * 30;

    COMMONUNITS_INFO("set h264 param [%s] iLevel[%d] to max_fs[%d] max_mbps[%d] level[%d].",
                     strConfig.c_str(), iLevel,
                     param.max_fs, param.max_mbps, param.level);

    return true;
}

#endif //__VOIP_COMMON_FUNCTIONS_H__
