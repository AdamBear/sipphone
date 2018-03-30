#include "modledlamp.h"
#include "eventcenter.h"
#include "factory.h"
#include "ledlamp.h"
#include "interfacedefine.h"
#include <configiddefine.h>
#include <configparser/modconfigparser.h>
#include <etlmsghandler/modetlmsghandler.h>
#include "lamp/powerled/include/modpowerledlogic.h"
#include <exp/src/expdrive/deviceplugin/modexpscan.h>
#include "leddata.h"
static bool s_bEnablePowerLight = true;

//初始化接口
void ledlamp_Init()
{
    CEventCenter::Instance()->Init();

#if defined(_T3X) || defined(_T4X)
    powerled_FlushPowerLed();
#endif

}

/*
外界消息调用接口
@param:
nEventID: 传入的事件/消息ID
bUnRegister: 是否为注册事件
*/
bool ledlamp_PostEvent(LAMP_EVENTTYPE eEventType, bool bRegister/* = true */)
{
    return CEventCenter::Instance()->PostEvent(eEventType, bRegister);
}

/*
自定义灯状态:
@param:
nLightNum: 灯号
nLightTime: 亮时间 (详见下方介绍)
nDarkTime: 暗时间 (详见下方介绍)
@brief:
(-1, -1): 关灯
(0, 0): 开灯
(正数, 正数): 闪烁
其它: 不处理

开灯/闪烁时, 会自动注册LAMP_EVENTTYPE_CUSTOM消息
关灯时, 会自动解注册LAMP_EVENTTYPE_CUSTOM消息

是否立即见效, 得看配置文件中Custom的优先级
*/
bool ledlamp_PostCustomEvent(int nLightNum, int nLightTime, int nDarkTime, bool bGreen /*= false*/,
                             bool bRed /*= false*/)
{
    bool bRegister = true;
    LAMP_EVENTTYPE eEventType = LAMP_EVENTTYPE_NONE;
    /* 明、暗时间都小于零, 则关灯 */
    if (nLightTime < 0 && nDarkTime < 0)
    {
        eEventType = LAMP_EVENTTYPE_CUSTOM;
        bRegister = false;
    }
    /* 开灯/闪烁 */
    else if ((0 == nLightTime && 0 == nDarkTime) ||
             (nLightTime > 0 && nDarkTime > 0))
    {
        eEventType = LAMP_EVENTTYPE_CUSTOM;
    }
    /* 其它状态不处理 */
    else
    {
        return false;
    }

    return CEventCenter::Instance()->PostEvent(eEventType, bRegister, nLightNum, nLightTime, nDarkTime);
}

// 设置dsskey灯的接口,可设置为红灯或橙灯,但不能设置为绿灯,绿灯接口为ledlamp_PostEvent
bool ledlamp_PostDsskeyEvent(int iIndex, int nLightTime /*= -1*/, int nDarkTime /*= -1*/,
                             bool bGreen /*= true*/, bool bRed /*= false*/)
{
    return CEventCenter::Instance()->PostDsskeyEvent(iIndex, nLightTime, nDarkTime, bGreen, bRed);
}

bool ledlamp_SwitchControl(bool bIsOn)
{
    return CEventCenter::Instance()->SwitchControl(bIsOn);
}

int ledlamp_GetLampID(LAMP_EVENTTYPE eEventType)
{
    tEventUnit eEvent = _LedData.GetEventUnit(eEventType);
    return eEvent._nLampNum;
}
