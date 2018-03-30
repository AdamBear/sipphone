#include "settingvolumehelper.h"
#include "idle/idle_data_define.h"
#include "idlescreen/include/modidlescreen.h"
#include "voice/include/modvoice.h"

namespace
{
const int       g_iRingTime = 190;
}

CSettingVolumeHelper * CSettingVolumeHelper::m_pInstance = NULL;

CSettingVolumeHelper * CSettingVolumeHelper::GetInstance()
{
    if (NULL == m_pInstance)
    {
        m_pInstance = new CSettingVolumeHelper();
    }
    return m_pInstance;
}

CSettingVolumeHelper::CSettingVolumeHelper()
{
}

void CSettingVolumeHelper::StopVolumeBarRing()
{
    voice_StopRingInMenu();
    m_tmStopVolume.KillTimer();
}

void CSettingVolumeHelper::VolumeBarVisibleChanged(bool bVisiable, const yl::string & strRingFile)
{
    if (bVisiable)
    {
        if (!m_tmStopVolume.IsTimerRuning())
        {
            voice_PlayRing(strRingFile.c_str());
            m_tmStopVolume.SingleShot(g_iRingTime, TIMER_METHOD(this,
                                      CSettingVolumeHelper::VolumeBarRingTimeOut));
        }
    }
    else
    {
        StopVolumeBarRing();
    }
}

void CSettingVolumeHelper::VolumeBarValueChanged(bool bVisiable, const yl::string & strRingFile)
{
    if (voice_GetCurrentChannel() == CHANNEL_RING)
    {
        if (bVisiable)
        {
            idleScreen_DropStatus(PS_STATE_MUTE);
        }
        else
        {
            idleScreen_SetStatus(PS_STATE_MUTE, "");
        }
    }

    if (bVisiable)
    {
        if (!m_tmStopVolume.IsTimerRuning())
        {
            voice_PlayRing(strRingFile.c_str());
            m_tmStopVolume.SingleShot(g_iRingTime, TIMER_METHOD(this,
                                      CSettingVolumeHelper::VolumeBarRingTimeOut));
        }
    }
}

void CSettingVolumeHelper::VolumeBarRingTimeOut(xThreadTimer* timer, LPVOID pExtraData)
{
    StopVolumeBarRing();
}

