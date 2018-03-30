#include "settinguinetwork8021xdelegate.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/src/settinguibasepagedata.h"

CSettingUINetwork8021xDelegate::CSettingUINetwork8021xDelegate()
{

}

CSettingUINetwork8021xDelegate::~CSettingUINetwork8021xDelegate()
{

}

CSettingUIDelegateBase * CSettingUINetwork8021xDelegate::CreatNetwork8021xDelegate()
{
    CSettingUINetwork8021xDelegate * pDelegate = new CSettingUINetwork8021xDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettingUINetwork8021xDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /*= false*/)
{
    bool bResult = CSettinguiDataDelegateBase::PageDataFilter(pPagedata, bReload);

    if (NULL == pPagedata)
    {
        return bResult;
    }

    if (0 != GetComboboxValue(NETWORK_8021X_MODEL))
    {
        SetItemShow(pPagedata, NETWORK_8021X_IDENTITY, true);
        SetItemShow(pPagedata, NETWORK_8021X_PASSWORD, true);
    }
    else
    {
        SetItemShow(pPagedata, NETWORK_8021X_IDENTITY, false);
        SetItemShow(pPagedata, NETWORK_8021X_PASSWORD, false);
    }

    return bResult;
}

