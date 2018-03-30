#include "settinguinetworkvpndelegate.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/networkcontroller.h"
#include "settingui/include/itemnamedefine.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettingUINetworkVPNDelegate::CSettingUINetworkVPNDelegate()
{

}

CSettingUINetworkVPNDelegate::~CSettingUINetworkVPNDelegate()
{

}

CSettingUIDelegateBase * CSettingUINetworkVPNDelegate::CreatNetworkVPNDelegate()
{
    CSettingUINetworkVPNDelegate * pDelegate = new CSettingUINetworkVPNDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettingUINetworkVPNDelegate::PageDataValid(CSettingUIPageDataBase * pPagedata,
        CSettingUIDataValidInfo & validInfo)
{
    if (NULL == pPagedata || NULL == g_pNetworkController)
    {
        return false;
    }

    yl::string strValue = "";
    CSettingUIItemDataBasePtr pItemInfo = m_pPageData->GetItemByItemId(NETWORK_VPN_ACTIVE);
    if (NULL != pItemInfo)
    {
        strValue = pItemInfo->GetValue();
    }

    if ("1" == strValue && !g_pNetworkController->IsOpenVPNConfigExist())
    {
        validInfo.SetValidIndex(0);
        validInfo.SetValidNote(_LANG(TRID_VPN_CANNOT_BE_ON_WHEN_FILE_DO_NOT_EXIST));
        return false;
    }

    return true;
}
