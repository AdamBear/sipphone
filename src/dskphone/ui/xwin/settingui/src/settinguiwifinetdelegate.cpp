#include "settinguiwifinetdelegate.h"

#ifdef IF_SUPPORT_WIFI

#include "translateiddefine.h"
#include "baseui/include/t2xpicpath.h"
#include "settinguiwifinetitemdata.h"
#include "settingui/include/pageiddefine.h"
#include "viewmodeuimanager/include/uicommon.h"
#include "viewmodeuimanager/include/viewmodeuidefine.h"
#include <devicelib/phonedevice.h>

#if IS_COLOR
namespace
{
#define MAX_WIFI_STATE_COUNT 4
const char * g_arrIconUnLock_UnConnected[MAX_WIFI_STATE_COUNT] = { PIC_WIFI_LIST_SIGNAL_UNCONNECTED_UNLOCK_1, PIC_WIFI_LIST_SIGNAL_UNCONNECTED_UNLOCK_2,
                                                                   PIC_WIFI_LIST_SIGNAL_UNCONNECTED_UNLOCK_3, PIC_WIFI_LIST_SIGNAL_UNCONNECTED_UNLOCK_4
                                                                 };
const char * g_arrIconUnLock_Connected[MAX_WIFI_STATE_COUNT] = { PIC_WIFI_LIST_SIGNAL_CONNECTED_UNLOCK_1, PIC_WIFI_LIST_SIGNAL_CONNECTED_UNLOCK_1,
                                                                 PIC_WIFI_LIST_SIGNAL_CONNECTED_UNLOCK_3, PIC_WIFI_LIST_SIGNAL_CONNECTED_UNLOCK_4
                                                               };

const char * g_arrIconLock_UnConnected[MAX_WIFI_STATE_COUNT] = { PIC_WIFI_LIST_SIGNAL_UNCONNECTED_LOCK_1, PIC_WIFI_LIST_SIGNAL_UNCONNECTED_LOCK_1,
                                                                 PIC_WIFI_LIST_SIGNAL_UNCONNECTED_LOCK_3, PIC_WIFI_LIST_SIGNAL_UNCONNECTED_LOCK_4
                                                               };

const char * g_arrIconLock_Connected[MAX_WIFI_STATE_COUNT] = { PIC_WIFI_LIST_SIGNAL_CONNECTED_LOCK_1, PIC_WIFI_LIST_SIGNAL_CONNECTED_LOCK_1,
                                                               PIC_WIFI_LIST_SIGNAL_CONNECTED_LOCK_3, PIC_WIFI_LIST_SIGNAL_CONNECTED_LOCK_4
                                                             };
}
#endif



CSettingUIWifiNetDelegate::CSettingUIWifiNetDelegate()
{

}

CSettingUIWifiNetDelegate::~CSettingUIWifiNetDelegate()
{

}

CSettingUIDelegateBase * CSettingUIWifiNetDelegate::CreateWifiNetDelegate()
{
    CSettingUIWifiNetDelegate * pDelegate = new CSettingUIWifiNetDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIItemDataBase * CSettingUIWifiNetDelegate::CreateDefaultItemData(const ViewInfo & viewInfo)
{
    CSettingUIItemDataBase * pItemData = new CSettingUIWifiNetItemData;

    return pItemData;
}

CSettingUIPageDataBase* CSettingUIWifiNetDelegate::LoadPageData(bool bReLoad /* = false */)
{
    if (!bReLoad || kszPageIDWifiKnown != GetPageID() || NULL == m_pPageData || -1 != m_iNewFocusIndex)
    {
        return CSettingUIBtWifiDelegate::LoadPageData(bReLoad);
    }

    int nFocusDataId = GetFocusDataID();
    CSettingUIBtWifiDelegate::LoadPageData(bReLoad);

    if (NULL != m_pPageData)
    {
        int nFocusIndex = GetItemIndexByDataID(nFocusDataId);
        if (-1 != nFocusIndex)
        {
            SetFocusIndex(nFocusIndex);
            SetFirstBindShow(true);
        }
    }

    return m_pPageData;
}

bool CSettingUIWifiNetDelegate::InitListItemData(CSettingUIItemDataBase * pItemData,
        const ViewInfo & viewInfo)
{
    if (!CSettingUIBtWifiDelegate::InitListItemData(pItemData, viewInfo))
    {
        return false;
    }

    CSettingUIWifiNetItemData * pNetItemData = (CSettingUIWifiNetItemData *)pItemData;
    if (NULL == pNetItemData)
    {
        return false;
    }

    if (viewInfo.strVecCustomValue.size() < 2 || viewInfo.strVecExtraValue.size() < 1)
    {
        return true;
    }

    const yl::string& strConnectState = viewInfo.strVecCustomValue[0];
    const yl::string& strLock = viewInfo.strVecCustomValue[1];

    if (UIConmonAPI_Lang(TRID_CONNECTING) == strConnectState)
    {
        pNetItemData->SetConnectStatusIcon(PIC_WIRELESS_CONNECTING_ICON);
    }
    else if (UIConmonAPI_Lang(TRID_CONNECTED) == strConnectState)
    {
        pNetItemData->SetConnectStatusIcon(PIC_WIRELESS_CONNECTED_ICON);
    }

    if (!strLock.empty()
#if IS_COLOR
            && PT_T52 != devicelib_GetPhoneType()
#endif
       )
    {
        pNetItemData->SetEncryptIcon(PIC_WIRELESS_ENCRYPTED_ICON);
    }

    if (kszPageIDWifiKnown != GetPageID())
    {
        const yl::string& strSignalStrength = viewInfo.strVecExtraValue[0];
        yl::string strSignalStrengthIcon = "";

#if IS_COLOR
        if (PT_T52 == devicelib_GetPhoneType())
        {
            strSignalStrengthIcon = SetSignalStrengthIcon(strSignalStrength, strConnectState, strLock);
        }
        else
#endif
        {
            strSignalStrengthIcon = PIC_COMMON_WIFI_PATH;
            strSignalStrengthIcon += strSignalStrength;
        }

        pNetItemData->SetSignalStrengthIcon(strSignalStrengthIcon);

    }

    return true;
}

#if IS_COLOR
yl::string CSettingUIWifiNetDelegate::SetSignalStrengthIcon(const yl::string & strSignalStrength, const yl::string & strConnectState, const yl::string & strLock)
{
    yl::string strSignalStrengthIcon = "";

    int nSingal = atoi(strSignalStrength.c_str());
    if (nSingal > 0 && nSingal <= MAX_WIFI_STATE_COUNT)
    {
        nSingal = nSingal - 1;
        //是否已连接
        if (UIConmonAPI_Lang(TRID_CONNECTED) == strConnectState)
        {
            //是否有锁
            if (!strLock.empty())
            {
                strSignalStrengthIcon = g_arrIconLock_Connected[nSingal];
            }
            else
            {
                strSignalStrengthIcon = g_arrIconUnLock_Connected[nSingal];
            }
        }
        else
        {
            if (!strLock.empty())
            {
                strSignalStrengthIcon = g_arrIconLock_UnConnected[nSingal];
            }
            else
            {
                strSignalStrengthIcon = g_arrIconUnLock_UnConnected[nSingal];
            }
        }
    }
    return strSignalStrengthIcon;
}
#endif

#endif
