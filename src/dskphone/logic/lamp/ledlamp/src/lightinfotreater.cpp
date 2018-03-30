#include "lightinfotreater.h"
#include "factory.h"
#include "ledlamp.h"
#include <ETLLib.hpp>
#include "dsklog/log.h"

// 测试代码
void PrintLightInfo(ATTRLIST & attrList)
{
#if 0
    LEDLAMP_INFO("Light info Begin");
    ATTRLIST::iterator it;
    for (it = attrList.begin(); it != attrList.end(); ++it)
    {
        LEDLAMP_INFO("Type[%d], Priority[%d], LightTime[%d], DarkTime:[%d], LedColor[%d]",
                     (*it).eEventType, (*it).nPriority, (*it).nLightTime, (*it).nDarkTime, (*it).eLedColor);
    }
    LEDLAMP_INFO("Light info End");
#endif
}

//处理灯事件
bool CLightInfoTreater::TreatLightEvent(LAMP_EVENTTYPE eEventType,
                                        const tEventUnit & eventUnit,
                                        VECTORLIGHTINFO & vLightInfo,
                                        bool bPopUp,
                                        int iLampNum)
{
    tLightInfo * pLightInfo = GetLightInfo(iLampNum, vLightInfo);
    if (pLightInfo == NULL)
    {
        return false;
    }

    if (bPopUp
            ? PopUp(pLightInfo, eventUnit, eEventType)
            : Push(pLightInfo, eventUnit, eEventType))
    {
        SortList(pLightInfo->attrList);
    }

    return true;
}

//压入事件
bool CLightInfoTreater::Push(tLightInfo * pLightInfo,
                             const tEventUnit & eventUnit,
                             LAMP_EVENTTYPE eEventType)
{
    if (pLightInfo == NULL)
    {
        return false;
    }

    MPA_LED_COLOR MapLedColor = eventUnit.MapLed;

    /* 测试当前Type是否与list中相同, 若是, 则不加入 */
    if (!pLightInfo->attrList.size() == 0)
    {
        for (ATTRLIST::iterator it = pLightInfo->attrList.begin(); it != pLightInfo->attrList.end(); ++it)
        {
            if (eEventType == (*it).eEventType)
            {
                (*it).nDarkTime = eventUnit._nDarkTime;
                (*it).nLightTime = eventUnit._nLightTime;
                (*it).nPriority = eventUnit._npriority;
                (*it).eLedColor = MapLedColor[pLightInfo->nLightNum];
                return true;
            }
        }
    }

    /* 当前attr压入list中 */
    tAttrStruct attr;
    attr.eEventType = eEventType;
    attr.nDarkTime = eventUnit._nDarkTime;
    attr.nLightTime = eventUnit._nLightTime;
    attr.nPriority = eventUnit._npriority;
    attr.eLedColor = MapLedColor[pLightInfo->nLightNum];

    pLightInfo->attrList.push_front(attr);
    return true;
}


typedef  bool (*COMPAREPRI)(const tAttrStruct & first, const tAttrStruct & second);

/* list的比较函数, 升序排列 */
bool comparePri(const tAttrStruct & first, const tAttrStruct & second)
{
    return first.nPriority < second.nPriority;
}

/* 排序list */
void CLightInfoTreater::SortList(ATTRLIST & attrList)
{
    Sort<tAttrStruct, COMPAREPRI>(attrList, comparePri);
}

/*
获取灯nLightNum中优先级最高的事件时间
@param
nLightNum: 灯号
vLightInfo:
*/
const tAttrStruct * CLightInfoTreater::GetHighestTime(int nLightNum, VECTORLIGHTINFO & vLightInfo)
{
    std::pair<int, int> pare;
    pare.first = -1;
    pare.second = -1;

    tLightInfo * pLightInfo = GetLightInfo(nLightNum, vLightInfo);
    if (!pLightInfo)
    {
        return NULL;
    }

    if (pLightInfo->attrList.size() == 0)
    {
        return NULL;
    }
    /* attrList为有序链表, 其头元素必为优先级最高的 */
    return &pLightInfo->attrList.front();
}

/*
测试是否为解注册事件, 若是则解注册eEventType对应的事件.
@param:
pLightInfo:
eventUnit:
eEventType: 输入的事件类型
*/
bool CLightInfoTreater::PopUp(tLightInfo * pLightInfo,
                              const tEventUnit & eventUnit,
                              LAMP_EVENTTYPE eEventType)
{
    if (pLightInfo == NULL)
    {
        return false;
    }

    bool bRet = false;

    YLList< tAttrStruct >::iterator it;
    for (it = pLightInfo->attrList.begin(); it != pLightInfo->attrList.end(); ++it)
    {
        if (eEventType == (*it).eEventType)
        {
            pLightInfo->attrList.erase(it);
            bRet = true;
            break;
        }
    }

    //关闭对应的灯
    CLedLamp * pLedLamp = _GetLedLamp(pLightInfo->nLightNum);
    if (bRet
            && NULL != pLedLamp)
    {
        pLedLamp->LightOff();
    }
    /* 其它情况, 为非解注册事件 */
    return bRet;
}


/* 由灯获取号一个列表 */
tLightInfo * CLightInfoTreater::GetLightInfo(int nLightNum,
        VECTORLIGHTINFO & vLightInfo)
{
    tLightInfo * pLightInfo = 0;
    int iIndex = 0;
    for (; iIndex < vLightInfo.size(); ++iIndex)
    {
        if (vLightInfo[iIndex]->nLightNum == nLightNum)
        {
            pLightInfo = vLightInfo[iIndex];
            return pLightInfo;
        }
    }

    if (iIndex == vLightInfo.size())
    {
        pLightInfo = new tLightInfo();
        pLightInfo->nLightNum = nLightNum;
        vLightInfo.push_back(pLightInfo);
    }

    return pLightInfo;
}

