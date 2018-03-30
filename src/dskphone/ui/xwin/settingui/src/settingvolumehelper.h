#ifndef __SETTING_VOLUME_HELPER_H__
#define __SETTING_VOLUME_HELPER_H__

#include "ETLLib/ETLLib.hpp"
#include "xwindow-kernel/include/xthreadtimer.h"

class CSettingVolumeHelper : public xTimerHandler
{
public:
    static CSettingVolumeHelper * GetInstance();

private:
    explicit CSettingVolumeHelper();

public:
    void StopVolumeBarRing();

    void VolumeBarVisibleChanged(bool bVisiable, const yl::string & strRingFile);
    void VolumeBarValueChanged(bool bVisiable, const yl::string & strRingFile);

private:
    //停止播放音量条铃声
    void VolumeBarRingTimeOut(xThreadTimer* timer, LPVOID pExtraData);

private:
    static CSettingVolumeHelper    *    m_pInstance;
    xThreadTimer                        m_tmStopVolume;
};

#define g_pSettingVolumeHelper (CSettingVolumeHelper::GetInstance())


#endif //__SETTING_VOLUME_HELPER_H__

