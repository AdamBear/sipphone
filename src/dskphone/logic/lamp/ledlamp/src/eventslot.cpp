#include "eventslot.h"
#include "ledlamp.h"
#include "lightinfotreater.h"
#include "dsklog/log.h"
#include "factory.h"

VECTORLIGHTINFO CEventSlot::m_vLightInfo;

bool CEventSlot::PostEvent(LAMP_EVENTTYPE _eventType, const tEventUnit & eventUnit, bool bRegister,
                           int iLampNum)
{
    /* 放入链表(若为解注册事件, 则pop出链表) */
    CLightInfoTreater::TreatLightEvent(_eventType, eventUnit, m_vLightInfo, !bRegister, iLampNum);

    return DoneListHead(iLampNum);
}

/* 取list中头元素, 进行灯的操作 */
bool CEventSlot::DoneListHead(int nLightNum)
{
    /*
    -1,-1  关
    0, 0,  开
    正数, 正数  闪动
    */

    // 获取指定id的灯
    CLedLamp * pLedLamp = _GetLedLamp(nLightNum);
    if (pLedLamp == NULL)
    {
        return false;
    }

    const tAttrStruct * pInfo = CLightInfoTreater::GetHighestTime(nLightNum, m_vLightInfo);
    if (NULL == pInfo || nLightNum < 0)
    {
        return false;
    }

    pLedLamp->SetColor(pInfo->eLedColor);

#if _LEDLAMP_DEBUG
    LEDLAMP_INFO("DoneListHead: pInfo->nLightTime[%d] pInfo->nDarkTime[%d] eLedColor[%d] eEventType[%d]",
                 pInfo->nLightTime,
                 pInfo->nDarkTime,
                 pInfo->eLedColor,
                 pInfo->eEventType);
#endif

    if (pInfo->nLightTime < 0)
    {
        /* pInfo->first<0的情况 */
        if (pInfo->nDarkTime >= 0)
        {
            //Light < 0, Dark >= 0
            return false;
        }
        //Light < 0, Dark < 0
        pLedLamp->LightOff();
        return true;
    }
    else if (0 == pInfo->nLightTime)
    {
        /* pInfo->first==0的情况 */
        if (pInfo->nDarkTime != 0)
        {
            //Light == 0, Dark != 0
            return false;
        }
        //Light == 0, Dark == 0
        pLedLamp->LightOn();
        return true;
    }
    else
    {
        /* 留下pInfo->first>0的情况 */
        if (pInfo->nDarkTime <= 0)
        {
#if IF_FEATURE_37754
            //Light > 0, Dark == 0
            if (pInfo->nDarkTime == 0)
            {
                pLedLamp->LightOn();
                return true;
            }
#endif
            //Light > 0, Dark <= 0
            return false;
        }
        //Light > 0, Dark > 0
        pLedLamp->LightFlash(pInfo->nLightTime, pInfo->nDarkTime);
    }

    return true;
}

CEventSlot::CEventSlot()
{

}
CEventSlot::~CEventSlot()
{
    ClearVLightInfo();
}

/* 内存释放 */
void CEventSlot::ClearVLightInfo()
{
    for (int iIndex = 0; iIndex < m_vLightInfo.size(); ++iIndex)
    {
        tLightInfo * pInfo = m_vLightInfo[iIndex];

        if (pInfo)
        {
            delete pInfo;
        }
    }
    m_vLightInfo.clear();
}
