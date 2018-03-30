/************************************************************************
 * FileName  : CDssKeyLightLogic.h (header file)
 * Purpose   :
 * Date      : 2012/10/18 11:05:47
 ************************************************************************/

#ifndef _CDSSKEYLIGHTLOGIC_H_
#define _CDSSKEYLIGHTLOGIC_H_

#include "lamp/ledlamp/include/modledlamp.h"
#include <ylhashmap.h>
#include <yllist.h>
#include <ylvector.h>
#include "dsskeyui/include/dsskeyuicommondefine.h"

struct LIGHT_INFO
{
    LIGHT_INFO()
    {
        iLightID = -1;
        eLightType = LED_COLOR_OFF;
        iDelay = 0;
        bFlash = false;
    }

    int                 iLightID;
    DSS_KEY_COLOR_TYPE
    eLightType; // 灯颜色,红绿交替闪时指示当前灯状态,取值红色和绿色
    int                 iDelay;
    bool                bFlash;     // 是否灯颜色交替闪烁
};

// 定义扩展台红绿交替闪烁时间定时器个数
#define LIGHT_TIMER_NUM     4

struct TimerInfo
{
    int     iDelay;
    int     iRunNum;        // 运行数量，为0的时候关闭定时器
};

typedef YLList<int>     listInt;
typedef listInt::iterator   listInt_it;

class CDssKeyLightLogic
{
private:

    struct SLightMeta
    {
        int m_iDssKeyID;
        int m_iPriority;
        DSS_KEY_COLOR_TYPE m_eColorType;
        DSS_KEY_FLASH_TYPE m_eFlashType;

        SLightMeta & operator= (SLightMeta & tmp)
        {
            if (this != &tmp)
            {
                m_iDssKeyID = tmp.m_iDssKeyID;
                m_iPriority = tmp.m_iPriority;
                m_eColorType = tmp.m_eColorType;
                m_eFlashType = tmp.m_eFlashType;
            }
            return *this;
        }
    };
#ifdef _MAP_TEST_
    struct LightData
    {
        int m_nLightKey;
        SLightMeta m_lightMeta;
    };

    typedef YLList<LightData>   LightStatusMap;
#else
    typedef YLHashMap<int, SLightMeta>  LightStatusMap;
#endif
    typedef LightStatusMap::iterator    LightStatusMapIterator;


    struct SPageLightMeta
    {
        SPageLightMeta() : m_iPage(0), m_iDssKey(-1), m_iPriority(DSS_KEY_LED_PRIORITY_NORMAL),
            m_bGreen(true), m_bRed(false), m_iFlashTime(-1) {}
        int m_iPage;
        int m_iDssKey;
        int m_iPriority;
        bool m_bGreen;
        bool m_bRed;
        int m_iFlashTime;
    };
    typedef YLList<SPageLightMeta>  PageLightMetaList;  //page灯状态
    typedef PageLightMetaList::iterator PageLightMetaListIterator;

public:
    static CDssKeyLightLogic * GetInstance();
    static void DestoryInstance();

public:
    // 更新灯当前的状态
    void UpdateLight(int iDsskeyID, DSS_KEY_COLOR_TYPE eColorType
                     , DSS_KEY_FLASH_TYPE eFlashType, int iFlashuration = -1);
    // 关灯
    void TurnOffLight(int iDsskeyID);
    // 闪烁定时器响应
    BOOL OnTimer(UINT uTimerID);
    // 禁用所有灯
    void Forbid(BOOL bForbidden);
    // 设置每页显示的Dsskey个数
    void SetMaxNumPerPage(int iMaxNumPerPage);
    // 设置Dsskey对应的灯个数
    void SetMaxNumber(int iMaxNumber);
    // 设置定时关闭灯
    void SetStopLightTimerByDsskeyID(int iDsskeyID, int iStopTime = 5000);

private:
    // 定时关闭灯处理
    void OnStopLightTimer(int iDsskeyID);
    // 真正关灯的操作
    void TurnOffLightByIndex(int iDsskeyID);
    // 更新灯的状态
    void updateLightStatus(int iDsskeyID, DSS_KEY_COLOR_TYPE eColorType, DSS_KEY_FLASH_TYPE eFlashType);
    // 真正灯显示的操作
    void LightCtrl(int iDsskeyID, DSS_KEY_COLOR_TYPE eColorType, int iDelay, bool bFlash = false);
    // 更新闪烁定时器，主要是内部实现红绿闪烁和定时灭灯的操作
    void UpdateFlashTimer(int iDsskeyID, int iDelay, BOOL bFlash);
private:

#ifdef _MAP_TEST_
    LightStatusMapIterator LightStatusFind(int index);
#endif

    CDssKeyLightLogic();

    virtual ~CDssKeyLightLogic();

private:
    LightStatusMap
    m_lightStatusMap;               //记录DssKey灯的状态，已DssKeyID为标识
    UINT
    m_lightStatus;  //记录DssKey灯的状态，10个灯分别用10个bit标识，DssKeyLight1:1<<1,DssKeyLight2:1<<2,...
    listInt             m_listTimerID;                  // 定时器ID（取地址）
    PageLightMetaList   m_pageLightList;                //记录Page灯状态
    BOOL                m_bForbidden;
    int                 m_iMaxNumPerPage;               // 每页显示的Dsskey个数
    int                 m_iMaxNumber;                   // Dsskey对应的灯个数
private:
    static CDssKeyLightLogic * s_pInst;
    TimerInfo       m_arryFlashTimer[LIGHT_TIMER_NUM];
    YLList<LIGHT_INFO *>     m_listLightInfo;
};

#define g_pDssKeyLightLogic (CDssKeyLightLogic::GetInstance())

#endif
