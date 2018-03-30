#include "settinguiadvanceddelegate.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "pstnadapter/include/modpstnadapter.h"
#include "messagebox/modmessagebox.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/include/settinguicommondata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "modsettingui.h"
#include "talk/fwd_dnd/include/modfwddnd.h"


CSettingUIAdvancedDelegate::CSettingUIAdvancedDelegate()
{

}

CSettingUIAdvancedDelegate::~CSettingUIAdvancedDelegate()
{

}

CSettingUIDelegateBase * CSettingUIAdvancedDelegate::CreateAdvancedDelegate()
{
    CSettingUIAdvancedDelegate * pAdvancedDelegate = new CSettingUIAdvancedDelegate;

    return pAdvancedDelegate;
}

bool CSettingUIAdvancedDelegate::ProcessMsgCallBack(int nResult, int nID,
        void * pExtraData/* = NULL*/)
{
    if (SETTING_MessageBox_OK == nResult && MSG_ID_NETWORK_CHANGED == nID)
    {
        SettingUI_Reboot(0);

        return true;
    }

    return CSettingUIDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

bool CSettingUIAdvancedDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /*= false*/)
{
    bool bResult = CSettingUIDelegateBase::PageDataFilter(pPagedata, bReload);

#if IF_BUG_29376
    SetItemShow(pPagedata, REBOOT, false);
#else
    SetItemShow(pPagedata, REBOOT, true);
#endif

    bool bShowFlashTime = false;

#ifdef IF_FEATURE_PSTN
    bShowFlashTime = pstnadapter_GetDeviceCount() > 0 ? true : false;
#endif

    SetItemShow(pPagedata, ADVANCDE_FLASH_TIME, bShowFlashTime);
    if (!fwd_IsActive())
    {
        SetItemShow(pPagedata, ADVANCDE_INTERNATIONAL_FORWARD, false);
    }

    return bResult;
}
