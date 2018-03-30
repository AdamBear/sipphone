#include "factory.h"
#include "ledlamp.h"
#include "dsklog/log.h"
#include <ylvector.h>

/* CLedLamp对象数组控制类 */
struct tLedLampMemoryControl
{
    typedef YLVector< CLedLamp * > VPLED;
    VPLED m_vpLedLamp; /* 存储 */
    ~tLedLampMemoryControl()
    {
        for (int iIndex = 0; iIndex < m_vpLedLamp.size(); ++iIndex)
        {
            CLedLamp * pLedLamp = m_vpLedLamp[iIndex];
            delete pLedLamp;
        }
        m_vpLedLamp.clear();
    }

    /* 获取相应灯号的灯 */
    CLedLamp * _GetLedLamp(int nLightNum)
    {
        CLedLamp * pLedLamp = 0;
        /* 从数组中找 */
        for (int iIndex = 0; iIndex < m_vpLedLamp.size(); ++iIndex)
        {
            pLedLamp = m_vpLedLamp[iIndex];
            /* 找到了, 返回灯 */
            if (pLedLamp->GetLightNum() == nLightNum)
            {
                return pLedLamp;
            }
        }
        /* 没有找到, 则新建一个 */
        pLedLamp = new CLedLamp();
        pLedLamp->SetLightNum(nLightNum);
        /* 放入m_vpLedLamp_Num中 */
        m_vpLedLamp.push_back(pLedLamp);
        return pLedLamp;
    }

    void _RemoveLedLamp(int nLightNum)
    {
        CLedLamp * pLedLamp = 0;
        /* 从数组中找 */
        for (int iIndex = 0; iIndex < m_vpLedLamp.size(); ++iIndex)
        {
            pLedLamp = m_vpLedLamp[iIndex];
            /* 找到了, 删除 */
            if (pLedLamp != NULL
                    && pLedLamp->GetLightNum() == nLightNum)
            {
                pLedLamp->LightOff();
                delete pLedLamp;
                m_vpLedLamp.removeAt(iIndex);
                break;
            }
        }
    }

    void AllLightOff()
    {
        for (int i = 0; i < m_vpLedLamp.size(); ++i)
        {
            m_vpLedLamp[i]->LightOff();
        }
    }

};

/* 全局灯组管理器, 用于程序关闭后内存的自动释放 */
tLedLampMemoryControl g_ledLampMemoryControl;

/* 取相应灯号的灯 */
CLedLamp * _GetLedLamp(int nLightNum)
{
    return g_ledLampMemoryControl._GetLedLamp(nLightNum);
}

void _RemoveLedLamp(int nLightNum)
{
    return g_ledLampMemoryControl._RemoveLedLamp(nLightNum);
}

void _AllLightOff()
{
    LEDLAMP_INFO("Led Lamp:All light off!");
    //TODO:关闭所有灯
    /* 获取灯号0的灯 */
    g_ledLampMemoryControl.AllLightOff();
}

YLVector< CLedLamp * > & _GetAllLamp()
{
    return g_ledLampMemoryControl.m_vpLedLamp;
}
