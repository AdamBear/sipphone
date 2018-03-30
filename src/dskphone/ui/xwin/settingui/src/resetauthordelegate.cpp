#include "resetauthordelegate.h"
#include "settinguilogic/include/settingdefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settingui/include/settinguidialogmanager.h"
#include "settinguilogic/src/datafactory.h"
#include "settinguilogic/src/dataloader.h"
#include "settinguilogic/include/pagedatafactory.h"
#include "settinguilogic/src/settinguibaseitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"

#include "setuidatadefine.h"
#include "settinguilogic/include/common.h"
#include "settinguilogic/include/zero_common.h"
#include "common/common_data_define.h"
#include "translateiddefine.h"
#include "feature/include/modfeature.h"

#include "languagehelper/modlangtranslate.h"
#include "settinguilogic/include/modsetuilogic.h"
#include "settingui/src/uicallbackfun.h"
#include <configiddefine.h>
#include "commonunits/modcommonunits.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "account/include/modaccount.h"
#include "adapterbox/include/modadapterbox.h"

namespace
{
#define   SOFT_KEY_BACK_INDEX   0
#define   SOFT_KEY_OK_INDEX     3
}

CResetAuthorDelegate::CResetAuthorDelegate()
    : CSettinguiDataDelegateBase()
{
}

CResetAuthorDelegate::~CResetAuthorDelegate()
{
}

CSettingUIDelegateBase * CResetAuthorDelegate::CreateResetAuthorDelegate()
{
    CResetAuthorDelegate * pStatusDelegate = new CResetAuthorDelegate;

    return (CSettingUIDelegateBasePtr)pStatusDelegate;
}

bool CResetAuthorDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
                                        bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    yl::string strPassword = GetValue(RESET_FACTORY_PASSWORD);
    yl::string strAdminName = feature_UserModeGetUserName(UT_ADMIN);
    if (strAdminName.empty())
    {
        return true;
    }

    // 匹配密码
    if (commonAPI_CheckPassword(strAdminName.c_str(), strPassword.c_str()))
    {
#if IF_BUG_20555
        if (acc_LogoutBeforeReset())
#endif  //IF_BUG_20555
        {
            IDLEUI_INFO("Before AdapterBox_ResetFactory.");
            // 恢复出厂配置
            AdapterBox_ResetFactory(RESET_OPTION_ALL);
        }
    }
    else
    {
        // 输错，密码框置空
        SetValue(RESET_FACTORY_PASSWORD, "");
        g_SetUICallBackFun.RefreshUI(false, GetPageID());
        CSettingUIDataValidInfo dataValid;
        int nDataIndex = GetItemDataIndexByItemId(RESET_FACTORY_PASSWORD);
        dataValid.SetValidIndex(nDataIndex);
        dataValid.SetValidNote(_LANG(TRID_PASSWORD_IS_WRONG));
        g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);
    }

    return true;
}

CSettingUIPageDataBase * CResetAuthorDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettinguiDataDelegateBase::LoadPageData(bReLoad);

    if (NULL != m_pPageData && bReLoad)
    {
        yl::string strAdminName = feature_UserModeGetUserName(UT_ADMIN);
        if (!strAdminName.empty())
        {
            SetValue(RESET_FACTORY_USERNAME, strAdminName);
        }
    }

#if IF_FEATURE_AUTOP_IME
    CSettingUILineEditItemDataPtr pEditItem = CSettingUILineEditItemData::GetLineEditDataItem(
                m_pPageData->GetItemByItemId(RESET_FACTORY_PASSWORD));
    if (NULL != pEditItem)
    {
        yl::string strImeType = commonUnits_GetIMEByType(kszAdvancedSettingPasswordIME);
        pEditItem->SetDefaultIME(strImeType);
    }
#endif

    return m_pPageData;
}

bool CResetAuthorDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        eActionType = SET_ACTION_TYPE_SAVE;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

