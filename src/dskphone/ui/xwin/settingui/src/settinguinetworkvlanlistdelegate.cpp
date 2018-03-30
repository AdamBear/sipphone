#include "settinguinetworkvlanlistdelegate.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"

#ifdef _CP
CSettingUINetworkVlanListDelegate::CSettingUINetworkVlanListDelegate()
{

}

CSettingUINetworkVlanListDelegate::~CSettingUINetworkVlanListDelegate()
{

}

CSettingUIDelegateBase * CSettingUINetworkVlanListDelegate::CreateNetworkVlanListDelegate()
{
    CSettingUINetworkVlanListDelegate * pDelegate = new CSettingUINetworkVlanListDelegate;

    return pDelegate;
}

bool CSettingUINetworkVlanListDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /*= false*/)
{
    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

    if (NULL != pPagedata)
    {
        bool bShowPcPort = true;
#ifdef _CP
        bShowPcPort = false;
#endif
        SetItemShow(pPagedata, VLAN_PCPORT, bShowPcPort);
    }

    return bResult;
}
#endif
