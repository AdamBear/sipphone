#ifndef _LEDLAMPH_
#define _LEDLAMPH_
#include "ETLLib.hpp"
#include "ledlampstruct.h"

#ifdef IF_DEBUG_LOG
#define _LEDLAMP_DEBUG 1
#else
#define _LEDLAMP_DEBUG 0
#endif // IF_DEBUG_LOG


/* 底层ledlamp的封装类 */
class CLedLamp: public CMKProcessor
{
    friend struct tLedLampMemoryControl;
    friend class CFlashThread;
    friend class FlashAction;

public:
    /* 开灯 */
    void LightOn();
    /* 关灯 */
    void LightOff();
    /* 闪灯 */
    void LightFlash(int nLightTime, int nDarkTime);
    //是否启用灯控制
    static void SetTouch(bool bIsTouch);

public:
    // 设置当前灯的状态
    void SetColor(LED_COLOR eLedColor);
protected:
    // 设置所有灯的状态
    static void SetPhoneLedStatus(LED_COLOR eLedColor, bool bAdd, int iLightNum);

public:
    /* 获取灯号 */
    int GetLightNum() DECLARE_ATTR;

private:
    // 更新到灯的物理状态
    static void UpdateLight();
    //闪动定时器函数
    BOOL FlashTimer(msgObject & objMessage);
    /* 控制灯 */
    void ControlLight(bool bOpen);
    //是否灯闪动
    bool IsFlahing();
    /* 设置灯号 */
    void SetLightNum(int nLightNum);
    CLedLamp();
    ~CLedLamp();

private:
    int m_nLightNum;
    int m_nLightTime;
    int m_nDarkTime;
    bool m_bLightOn;
    mkThreadTimer m_timerFlashOn;

    // 由于一个灯存在红,绿,橙三种颜色,所以需保存当前的灯的状态
    LED_COLOR m_eColor;

    // t3x的灯是由位来控制的,每次设置都是对所有的灯都设置一次
    static int s_iLedStatus;

    static bool s_bIsTouch; //是否启用控制
};
#endif
