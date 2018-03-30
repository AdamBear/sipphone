#include "ledlamp.h"
#include <stdio.h>
#include "factory.h"
#include <unistd.h>
#include <pthread.h>
#include "devicelib/phonedevice.h"
#include "ledlampstruct.h"
#include "leddata.h"
#include "dsklog/log.h"



// 初始化灯的状态
int CLedLamp::s_iLedStatus = 0;
bool CLedLamp::s_bIsTouch = true;
static mkThreadTimer s_timerFlushLed;
static CLedLamp * s_pLedLamp = NULL;


bool CLedLamp::IsFlahing()
{
    //定时器运行，即闪动
    return m_timerFlashOn.IsTimerRuning() == TRUE;
}
CLedLamp::CLedLamp()
{
    m_nLightNum = -1;
    m_bLightOn = false;

    m_eColor = LED_COLOR_YELLOW;
    if (s_pLedLamp == NULL)
    {
        s_pLedLamp = this;
    }
}

CLedLamp::~CLedLamp()
{
    s_pLedLamp = NULL;
}

/* 控制灯 */
void CLedLamp::ControlLight(bool bOpen)
{
    SetPhoneLedStatus(m_eColor, bOpen, m_nLightNum);
}

/* 开灯 */
void CLedLamp::LightOn()
{
    m_timerFlashOn.KillTimer();
    ControlLight(true);
}

/* 关灯 */
void CLedLamp::LightOff()
{
    m_timerFlashOn.KillTimer();
    ControlLight(false);
}

/* 闪灯 */
void CLedLamp::LightFlash(int nLightTime, int nDarkTime)
{
    //开始时先亮灯
    m_bLightOn = true;
    ControlLight(true);
    m_nLightTime = nLightTime;
    m_nDarkTime = nDarkTime;
    m_timerFlashOn.SetTimer(m_nLightTime, NULL, MK_MSG_ENTRY(this, CLedLamp::FlashTimer));
}

//闪动定时器相应函数
BOOL CLedLamp::FlashTimer(msgObject & objMessage)
{
    if (objMessage.wParam == (UINT)&m_timerFlashOn)
    {
        m_bLightOn = !m_bLightOn;
        ControlLight(m_bLightOn);
        UpdateLight();
        m_timerFlashOn.KillTimer();
        if (m_bLightOn)
        {
            m_timerFlashOn.SetTimer(m_nLightTime, NULL, MK_MSG_ENTRY(this, CLedLamp::FlashTimer));
        }
        else
        {
            m_timerFlashOn.SetTimer(m_nDarkTime, NULL, MK_MSG_ENTRY(this, CLedLamp::FlashTimer));
        }

        s_timerFlushLed.KillTimer();
    }
    else if (objMessage.wParam == (UINT)&s_timerFlushLed)
    {
        UpdateLight();
        s_timerFlushLed.KillTimer();
    }
    //这里返回0，使线程在出现空闲时，不会Sleep或者锁住
    objMessage.lResult = 0;
    return true;
}

/* 获取灯号 */
int CLedLamp::GetLightNum()
{
    return m_nLightNum;
}

/* 设置灯号 */
void CLedLamp::SetLightNum(int nLightNum)
{
    m_nLightNum = nLightNum;
}

// 设置状态或去掉状态
void CLedLamp::SetColor(LED_COLOR eLedColor)
{
    m_eColor = eLedColor;
}

// 设置所有灯的状态
void CLedLamp::SetPhoneLedStatus(LED_COLOR eLedColor, bool bAdd, int iLightNum)
{
    // 记住设置之前的状态
    int iOldStatus = s_iLedStatus;

    //先关闭这个灯的所有颜色
    _LedData.RemoveAllColor(s_iLedStatus, iLightNum);

    if (bAdd)
    {
        //在设置需要的颜色
        s_iLedStatus |= _LedData.GetColorValue(iLightNum, eLedColor);
    }

    // 新状态与就状态一致，不重复设置
    if (iOldStatus == s_iLedStatus)
    {
        //LEDLAMP_INFO("CLedLamp::SetPhoneLedStatus old status = new status [%d]", s_iLedStatus);
        return ;
    }

#if _LEDLAMP_DEBUG
    LEDLAMP_INFO("CLedLamp::SetPhoneLedStatus color[%d] add[%d] id[%d] old[%#x] new[%#x]",
                 eLedColor, bAdd, iLightNum, iOldStatus, s_iLedStatus);
#endif

    s_timerFlushLed.SetTimer(100, NULL, MK_MSG_ENTRY(s_pLedLamp, CLedLamp::FlashTimer));
}

void CLedLamp::SetTouch(bool bIsTouch)
{
    s_bIsTouch = bIsTouch;
}

void CLedLamp::UpdateLight()
{
#if _LEDLAMP_DEBUG
    LEDLAMP_INFO("Update Lihgt [%d] s_bIsTouch[%d] ", s_iLedStatus, s_bIsTouch);
#endif
    if (s_bIsTouch)
    {
        devicelib_SetLedStatus(s_iLedStatus);

#ifdef _CP
        // 扩展口的LED灯
        devicelib_SetExtLed(s_iLedStatus);
#endif

    }
}

