#include "record_inc.h"

#ifdef _T49
bool RecordInterface_Screenshot(const char * pFile)
{
    if (NULL == pFile
            || strcmp(pFile, "") == 0)
    {
        return false;
    }

    int iRet = ipvp_video_snapshot(mkGetHandle(), -1, pFile, 0);
    APP_INFO("RecordInterface Screenshot, name[%s], result[%d]", pFile, iRet);

    return (0 == iRet);
}
#endif

#ifdef IF_USB_SUPPORT
#include <dsklog/log.h>

bool RecordInterface_RecordStart(const char * pFile, int sync/* = 0*/)
{
    if (NULL == pFile
            || strcmp(pFile, "") == 0)
    {
        return false;
    }

#ifdef _T49
    // 先降分辨率
    modmedia_ReducePower();
#endif

    // 再开始录制
    int iRet = ipvp_record_start2(mkGetHandle(), pFile, sync);

    APP_INFO("RecordInterface RecordStart, name[%s], result[%d]", pFile, iRet);
    return (0 == iRet);
}

bool RecordInterface_RecordStop(int sync/* = 0*/)
{
    int iRet = ipvp_record_stop(mkGetHandle(), sync);

#ifdef _T49
    // 恢复视频能力
    modmedia_ResumePower();
#endif

    APP_INFO("RecordInterface RecordStop, result[%d]", iRet);
    return (0 == iRet);
}

bool RecordInterface_RecordPause(int sync/* = 0*/)
{
    APP_INFO("RecordInterface RecordPause");

    return (0 == ipvp_record_pause(mkGetHandle(), sync));
}

bool RecordInterface_RecordResume(int sync/* = 0*/)
{
    APP_INFO("RecordInterface RecordResume");

    return (0 == ipvp_record_resume(mkGetHandle(), sync));
}

bool RecordInterface_PlayStart(const char * pFile, int sync/* = 0*/)
{
    if (NULL == pFile
            || strcmp(pFile, "") == 0)
    {
        return false;
    }

    int iRet = ipvp_media_play2(mkGetHandle(), pFile, sync);

#ifdef IF_SUPPORT_VIDEO
    // 播放视频, 调整布局
    RecordInterface_SetVideoLCDLayout();
    RecordInterface_SetVideoHDMILayout();
#endif

    APP_INFO("RecordInterface PlayStart, name[%s], result[%d]", pFile, iRet);
    return (0 == iRet);
}

bool RecordInterface_PlayPause(int sync/* = 0*/)
{
    int iRet = ipvp_media_pause(mkGetHandle(), sync);

    APP_INFO("RecordInterface PlayPause, result[%d]", iRet);
    return (0 == iRet);
}

bool RecordInterface_PlayResume(int sync/* = 0*/)
{
    int iRet = ipvp_media_resume(mkGetHandle(), sync);

    APP_INFO("RecordInterface PlayResume, result[%d]", iRet);
    return (0 == iRet);
}

bool RecordInterface_PlayStop(int sync/* = 0*/)
{
    int iRet = ipvp_media_stop(mkGetHandle(), sync);

#ifdef IF_SUPPORT_VIDEO
    // 播放结束, 清空布局
    RecordInterface_ClearVideoLCDLayout();
    RecordInterface_ClearVideoHDMILayout();
#endif

    APP_INFO("RecordInterface PlayStop, result[%d]", iRet);
    return (0 == iRet);
}

bool RecordInterface_PlaySeek(unsigned int locate, int sync/* = 0*/)
{
    int iRet = ipvp_media_seek(mkGetHandle(), locate, sync);

    APP_INFO("RecordInterface PlaySeek, locate[%u], result[%d]", locate, iRet);
    return (0 == iRet);
}

// 获取媒体信息
bool RecordInterface_GetInfo(MediaInfo & mediaInfo, const char * pFile)
{
    if (NULL == pFile
            || strcmp(pFile, "") == 0)
    {
        return false;
    }

    ipvp_pinfo_t ipvpData;
    int nRet = ipvp_media_getinfo(mkGetHandle(), &ipvpData, pFile);
    if (0 == nRet && ipvpData.sec_total > 0)
    {
        mediaInfo.strFile = pFile;
        mediaInfo.nSecTotal = ipvpData.sec_total;
        mediaInfo.nSecCurrent = ipvpData.sec_current;
        return true;
    }

    APP_WARN("ipvp get media error file[%s] ret[%d] total[%d]", pFile, nRet, ipvpData.sec_total);
    return false;
}

#ifdef IF_SUPPORT_VIDEO
void RecordInterface_SetVideoLCDLayout()
{
    // 调整LCD布局
    DisplayMonitor sMonitor;
    sMonitor.AddFileLayout(chRect(0, 0, 1280, 800));

    modmedia_SetVideoLayout(sMonitor);
}

void RecordInterface_SetVideoHDMILayout()
{
    // 调整HDMI布局
    if (modhotplug_IsHdmiConnected()
            && modhotplug_GetHdmiSwitch())
    {
        DisplayMonitor sMonitor;
        sMonitor.AddHdmiFileLayout(chRect(0, 0, 1920, 1080));
        modmedia_SetVideoLayout(sMonitor);
    }
}

void RecordInterface_ClearVideoLCDLayout()
{
    DisplayMonitor sMonitor;
    sMonitor.m_eDisplay = VDD_LCD;
    modmedia_SetVideoLayout(sMonitor);
}

void RecordInterface_ClearVideoHDMILayout()
{
    DisplayMonitor sMonitor;
    sMonitor.m_eDisplay = VDD_HDMI;
    modmedia_SetVideoLayout(sMonitor);
}
#endif //

#endif // IF_USB_RECORD
