#if (defined(IF_SUPPORT_BLUETOOTH ) || defined(IF_SUPPORT_WIFI))
#include "../include/viewmodectrlerhelper.h"
#include "../include/viewmodeuidefine.h"
#include "btuictrler.h"
#include "wifiuictrler.h"

IMPLEMENT_GETINSTANCE(CViewModeCtrlerHelper)
CViewModeCtrlerHelper::CViewModeCtrlerHelper()
{
    m_vecCtrler.clear();
}

CViewModeCtrlerHelper::~CViewModeCtrlerHelper()
{
}

CViewModeControler * CViewModeCtrlerHelper::GetCtrler(int iUIType)
{
    CtrlerInfo ctrlerInfo;
    CtrlerType eType;
    CViewModeControler * pCtrler = NULL;

    switch (iUIType)
    {
    case WIRELESS_UI_TYPE_BT_MENU:
    case WIRELESS_UI_TYPE_BT_SCAN:
    case WIRELESS_UI_TYPE_BT_PAIRED:
    case WIRELESS_UI_TYPE_BT_DEV_DETAIL:
    case WIRELESS_UI_TYPE_BT_DONGLE_INFO:
    case WIRELESS_UI_TYPE_BT_PIN_INPUT:
        eType = CtrlerBt;
#ifdef IF_SUPPORT_BLUETOOTH
        pCtrler = FindCtrlerInfoByType(eType, ctrlerInfo) ? ctrlerInfo.pCtrler : new CBtUICtrler;
#endif
        break;
    case WIRELESS_UI_TYPE_WIFI_MENU:
    case WIRELESS_UI_TYPE_WIFI_SCAN:
    case WIRELESS_UI_TYPE_WIFI_PROFILE:
    case WIRELESS_UI_TYPE_WIFI_NET_DETAIL:
    case WIRELESS_UI_TYPE_WIFI_NET_STATUS:
    case WIRELESS_UI_TYPE_WIFI_NET_EDIT:
    case WIRELESS_UI_TYPE_WIFI_NET_ADD:
    case WIRELESS_UI_TYPE_WIFI_NET_DETAIL_2_ADD:
    case WIRELESS_UI_TYPE_WIFI_PIN_INPUT:
    case WIRELESS_UI_TYPE_WIFI_AUTH_NETWORK:
        eType = CtrlerWifi;
#ifdef IF_SUPPORT_WIFI
        pCtrler = FindCtrlerInfoByType(eType, ctrlerInfo) ? ctrlerInfo.pCtrler : new CWifiUICtrler;
#endif
        break;
    default:
        break;
    }

    SetCtrlerInfo(eType, pCtrler);
    return pCtrler;
}

bool CViewModeCtrlerHelper::ReleaseCtrler(CViewModeControler ** pCtrler)
{
    for (int i = 0; i < m_vecCtrler.size(); ++i)
    {
        CtrlerInfo & ctrlerTemp = m_vecCtrler[i];
        if (*pCtrler == ctrlerTemp.pCtrler
                && -- ctrlerTemp.iCount <= 0)
        {
            //CBtUICtrler* pTempCtrler = NULL;
            //switch (ctrlerTemp.eType)
            //{
            //case CtrlerBt: pTempCtrler = dynamic_cast<CBtUICtrler*>(*pCtrler); break;
            //case CtrlerWifi: pTempCtrler = dynamic_cast<CWifiUICtrler*>(*pCtrler); break;
            //default:
            //  break;
            //}

            //// 需要释放实例, 否则没办法调用实例的析构函数
            //if (pTempCtrler)
            //{
            //  delete pTempCtrler;
            //  pTempCtrler = NULL;
            //  m_vecCtrler.removeAt(i);
            //
            //  return true;
            //}
        }
    }

    return false;
}

bool CViewModeCtrlerHelper::FindCtrlerInfoByType(CtrlerType eType, CtrlerInfo & ctrlerInfo)
{
    for (int i = 0; i < m_vecCtrler.size(); ++ i)
    {
        CtrlerInfo & ctrlerTemp = m_vecCtrler[i];
        if (ctrlerTemp.eType == eType)
        {
            ctrlerInfo = ctrlerTemp;
            return true;
        }
    }

    return false;
}

void CViewModeCtrlerHelper::SetCtrlerInfo(CtrlerType eType, CViewModeControler * pCtrler)
{
    CtrlerInfo ctrlerInfo;
    if (!FindCtrlerInfoByType(eType, ctrlerInfo))
    {
        ctrlerInfo.eType = eType;
        ctrlerInfo.pCtrler = pCtrler;
        m_vecCtrler.push_back(ctrlerInfo);
    }

    SetRefrenceCount(eType, true);
}

void CViewModeCtrlerHelper::SetRefrenceCount(CtrlerType eType, bool bIncrease)
{
    for (int i = 0; i < m_vecCtrler.size(); ++i)
    {
        CtrlerInfo & ctrlerTemp = m_vecCtrler[i];
        if (ctrlerTemp.eType == eType)
        {
            ctrlerTemp.iCount = bIncrease ? ++ ctrlerTemp.iCount : -- ctrlerTemp.iCount;
            SETTINGUI_INFO("Ctrler count[%d], reference count[%d][%p]", m_vecCtrler.size(), ctrlerTemp.iCount,
                           ctrlerTemp.pCtrler);
        }
    }
}

#endif // IF_SUPPORT_BLUETOOTH || IF_SUPPORT_WIFI
