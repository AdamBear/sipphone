#include <ETLLib.hpp>
#include "eventcenter.h"
#include "factory.h"
#include "eventparser.h"
#include "ledlamp.h"
#include "dsklog/log.h"
#include "leddata.h"
#include "devicelib/phonedevice.h"

/* 测试事件是否为Custom, 若是, 则设置eventUnit的值 */
void _TestCustomAndSet(LAMP_EVENTTYPE eEventType,
                       int nLightNumForCustom,
                       int nLightTimeForCustom,
                       int nDarkTimeForCustom,
                       tEventUnit & eventUint
                      )
{
    if (LAMP_EVENTTYPE_CUSTOM == eEventType)
    {
        eventUint.MapLed[nLightNumForCustom] = LED_COLOR_YELLOW;
        eventUint._nLightTime = nLightTimeForCustom;
        eventUint._nDarkTime = nDarkTimeForCustom;
    }
}

CEventCenter CEventCenter::ms_Center;
/*
处理具体事件
@param
    nOutEventID: 事件ID
    bUnRegister: 是否为解注册事件
        以下三个参数为Custom保留
    nLightNumForCustom: 灯号
    nLightTimeForCustom: 亮时间
    nDarkTimeForCustom: 暗时间
*/
bool CEventCenter::PostEvent(LAMP_EVENTTYPE eEventType,
                             bool bRegister,
                             int nLightNumForCustom/* = 0*/,
                             int nLightTimeForCustom/* = 0*/,
                             int nDarkTimeForCustom/* = 0*/)
{
#if _LEDLAMP_DEBUG
    LEDLAMP_INFO("Ledlamp: post event(%d, %d, %d, %d, %d)",
                 eEventType, bRegister, nLightNumForCustom, nLightTimeForCustom, nDarkTimeForCustom);
#endif //

    if (eEventType < LAMP_EVENTTYPE_NONE)
    {
        return false;
    }

    /* 缓存中查找事件类型 */
    tEventUnit eventUint = _LedData.GetEventUnit(eEventType);

    /* 若为自定义处理, 得特殊处理下 */
    _TestCustomAndSet(eEventType,
                      nLightNumForCustom,
                      nLightTimeForCustom,
                      nDarkTimeForCustom,
                      eventUint);

    bool bSuccess = false;

    MPA_LED_COLOR::iterator iter = eventUint.MapLed.begin();

    for (; iter != eventUint.MapLed.end(); iter++)
    {
        int iLampNum = iter->first;
        if (iLampNum < 0)
        {
            // 如果不存在该类型则不处理
            continue;
        }
        /* 交由m_eventSlot处理 */
        bSuccess = bSuccess | m_eventSlot.PostEvent(eEventType, eventUint, bRegister, iLampNum);
    }

    return bSuccess;
}

CEventCenter * CEventCenter::Instance()
{
    return &ms_Center;
}

CEventCenter::~CEventCenter()
{
}

void CEventCenter::Init()
{
    _AllLightOff();

    // 由于网络环境问题，会出现话机启动后Power LED灯不灭问题，因此强制关闭所有灯
    devicelib_SetLedStatus(0);
    _LedData.Init();
}

bool CEventCenter::SwitchControl(bool bIsOn)
{
    CLedLamp::SetTouch(bIsOn);
    if (bIsOn) //打开时重新执行一次所有灯的事件
    {
        for (int i = 0; i < _GetAllLamp().size(); ++i)
        {
            m_eventSlot.DoneListHead(_GetAllLamp()[i]->GetLightNum());
        }
    }

    return true;
}

bool CEventCenter::PostDsskeyEvent(int iIndex, int nLightTime /*= 0*/, int nDarkTime /*= 0*/,
                                   bool bGreen/* = true*/, bool bRed/* = false*/)
{
    //!!!!!, 确认DSSKEY_LIGHT_NUM
    if (iIndex < 0
            /*|| iIndex >= DSSKEY_LIGHT_NUM*/)
    {
        return false;
    }

    if ((nLightTime < 0 && nDarkTime > 0)
            || (nLightTime > 0 && nDarkTime < 0))
    {
        return false;
    }

    if (nLightTime < 0 && nDarkTime < 0)
    {
        //关灯，将所有关联的灯事件注销
        ledlamp_PostEvent(LAMP_EVENTTYPE(LAMP_EVENTTYPE_DSSKEYRED1 + iIndex), false);
        ledlamp_PostEvent(LAMP_EVENTTYPE(LAMP_EVENTTYPE_DSSKEYGREEN1 + iIndex), false);
        ledlamp_PostEvent(LAMP_EVENTTYPE(LAMP_EVENTTYPE_DSSKEYORANGE1 + iIndex), false);
        return true;
    }

    LAMP_EVENTTYPE eEventType = LAMP_EVENTTYPE_NONE;

    if (bGreen && bRed)
    {
        eEventType = LAMP_EVENTTYPE(LAMP_EVENTTYPE_DSSKEYORANGE1 + iIndex);
    }
    else if (bGreen)
    {
        eEventType = LAMP_EVENTTYPE(LAMP_EVENTTYPE_DSSKEYGREEN1 + iIndex);
    }
    else if (bRed)
    {
        eEventType = LAMP_EVENTTYPE(LAMP_EVENTTYPE_DSSKEYRED1 + iIndex);
    }

    tEventUnit & eventUint = _LedData.GetEventUnit(eEventType);
    eventUint._nDarkTime = nDarkTime;
    eventUint._nLightTime = nLightTime;

    ledlamp_PostEvent(eEventType, true);
    return true;
}
