#include "settinguibasicsettinguidelegate.h"
#include "pstnadapter/include/modpstnadapter.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/modsettingui.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "autopcontrol/include/modautopcontrol.h"
#include <settingui/include/settinguidefine.h>
#if IF_UNLOCK_CHECK_POPBOX
#include "settingui/src/settingmsgboxmgr.h"
#endif

namespace
{
#if IF_UNLOCK_CHECK_POPBOX
const char* kszUnlockCheck = "unlockcheck";
#endif
const char* kszActionUpdateConfig = "updateconfig";
#define MSG_ID_UPDATE_CONFIG            1400
}

CSettingUIBasicSettingDelegate::CSettingUIBasicSettingDelegate()
{

}

CSettingUIBasicSettingDelegate::~CSettingUIBasicSettingDelegate()
{

}

CSettingUIDelegateBase* CSettingUIBasicSettingDelegate::CreateBasicSettingDelegate()
{
    CSettingUIBasicSettingDelegate* pAdvancedDelegate = new CSettingUIBasicSettingDelegate;

    return pAdvancedDelegate;
}

bool CSettingUIBasicSettingDelegate::PageDataFilter(CSettingUIPageDataBase* pPagedata,
        bool bReload /*= false*/)
{
    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

    bool bShowCountry = false;

#ifdef IF_FEATURE_PSTN
    bShowCountry = pstnadapter_GetDeviceCount() > 0 ? true : false;
#endif

    SetItemShow(pPagedata, BASIC_SETTING_COUNTRY, bShowCountry);

#if !IF_FEATURE_GENBAND_E911
    SetItemShow(pPagedata, E911_LOCATION, false);
#endif

    return bResult;
}

bool CSettingUIBasicSettingDelegate::ProcessAction(const yl::string strAction, int iDataIndex)
{
    // 重启
    if (0 == strAction.compare(NS_SET::kszActionReboot) && !CheckTalking())
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_WARNING),
                                               _LANG(TRID_IF_REBOOT_THE_PHONE),
                                               SETTING_MESSAGEBOX_OkCancel, "", "", -1, MSG_ID_REBOOT);
        return true;
    }
    else if (0 == strAction.compare(kszActionUpdateConfig) && !CheckTalking())
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_WARNING),
                                               _LANG(TRID_IF_PROVISION_NOW),
                                               SETTING_MESSAGEBOX_OkCancel, "", "", -1, MSG_ID_UPDATE_CONFIG);
        return true;
    }
#if IF_UNLOCK_CHECK_POPBOX
    else if (0 == strAction.compare(kszUnlockCheck) && !CheckTalking())
    {
        g_SettingMsgBoxMgr.ShowUnlockCheckMsgBox(true);
        return true;
    }
#endif

    return CSettingUIDelegateBase::ProcessAction(strAction, iDataIndex);
}

bool CSettingUIBasicSettingDelegate::ProcessMsgCallBack(int nResult, int nID, void* pExtraData /*= NULL*/)
{
    if (SETTING_MessageBox_OK == nResult && MSG_ID_REBOOT == nID)
    {
        SettingUI_Reboot(0);
        return true;
    }
    else if (SETTING_MessageBox_OK == nResult && MSG_ID_UPDATE_CONFIG == nID)
    {
        Autop_NotifyAutoServer(false, ATP_MSG_REQ_START_BY_DEFAULT, 0, 1);
        return true;
    }

    return CSettingUIDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}
