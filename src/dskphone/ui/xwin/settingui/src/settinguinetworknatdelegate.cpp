#include "settinguinetworknatdelegate.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define INDEX_NAT_STUN_SERVER 1
}

CSettingUINetworkNATDelegate::CSettingUINetworkNATDelegate()
{

}

CSettingUINetworkNATDelegate::~CSettingUINetworkNATDelegate()
{

}

CSettingUIDelegateBase * CSettingUINetworkNATDelegate::CreatNetworkNATDelegate()
{
    CSettingUINetworkNATDelegate * pDelegate = new CSettingUINetworkNATDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettingUINetworkNATDelegate::PageDataValid(CSettingUIPageDataBase * pPagedata,
        CSettingUIDataValidInfo & validInfo)
{
    if (NULL == pPagedata)
    {
        return false;
    }

    if (1 == GetComboboxValue(NETWORK_NAT_STATUS) && GetValue(NETWORK_NAT_STUNSERVER).empty())
    {
        validInfo.SetValidIndex(INDEX_NAT_STUN_SERVER);
        validInfo.SetValidNote(_LANG(TRID_STUN_SERVER_IS_EMPTY));
    }

    return CSettinguiDataDelegateBase::PageDataValid(pPagedata, validInfo);
}
