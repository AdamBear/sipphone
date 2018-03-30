#include "settinguiautoprovisiondelegate.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settinguilogic/src/datasave.h"
#include "settinguilogic/src/advsettingcontroller.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include <configiddefine.h>
#include "commonunits/modcommonunits.h"

namespace
{
#define MSGBOX_ID_PROVISON_NOW  1003
}

CSettinguiAutoProvisionDelegate::CSettinguiAutoProvisionDelegate()
{

}

CSettinguiAutoProvisionDelegate::~CSettinguiAutoProvisionDelegate()
{

}

CSettingUIDelegateBase * CSettinguiAutoProvisionDelegate::CreateAutoProvisionDelegate()
{
    CSettinguiAutoProvisionDelegate * pDelegate = new CSettinguiAutoProvisionDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

bool CSettinguiAutoProvisionDelegate::ProcessMsgCallBack(int nResult, int nID,
        void * pExtraData /* = NULL */)
{
    if (MSGBOX_ID_PROVISON_NOW == nID)
    {
        if (SETTING_MessageBox_OK == nResult)
        {
            BeginAutoProvision();
        }

        if (SETTING_MessageBox_OK == nResult || SETTING_MessageBox_Cancel == nResult)
        {
            g_SetUICallBackFun.FallBackToPrevousPage();
        }

        return true;
    }

    return CSettinguiDataDelegateBase::ProcessMsgCallBack(nResult, nID, pExtraData);
}

CSettingUIPageDataBase * CSettinguiAutoProvisionDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettinguiDataDelegateBase::LoadPageData(bReLoad);

#if IF_FEATURE_AUTOP_IME
    if (NULL != m_pPageData && bReLoad)
    {
        CSettingUILineEditItemDataPtr pUserEditItem = CSettingUILineEditItemData::GetLineEditDataItem(
                    m_pPageData->GetItemByItemId(AUTOP_USERNAME));
        if (NULL != pUserEditItem)
        {
            yl::string strImeType = commonUnits_GetIMEByType(kszZeroUserIME);
            pUserEditItem->SetDefaultIME(strImeType);
        }

        CSettingUILineEditItemDataPtr pPwdEditItem = CSettingUILineEditItemData::GetLineEditDataItem(
                    m_pPageData->GetItemByItemId(AUTOP_PASSWORD));
        if (NULL != pPwdEditItem)
        {
            yl::string strImeType = commonUnits_GetIMEByType(kszZeroPasswordIME);
            pPwdEditItem->SetDefaultIME(strImeType);
        }
    }
#endif

    return m_pPageData;
}

bool CSettinguiAutoProvisionDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (CheckTalking() || NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    CSettingUIDataValidInfo dataValid;
    if (!PageDataValid(pPageData, dataValid))
    {
        g_SetUICallBackFun.DealWithValidData(dataValid, m_strPageID);
        return false;
    }

    g_DataSaver->SaveData(pPageData);

    if (!GetValue(AUTOP_URL).empty())
    {
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_IF_PROVISION_NOW)
                                               , SETTING_MESSAGEBOX_OkCancel, "", "", -1
                                               , MSGBOX_ID_PROVISON_NOW, (void *)(&m_bBackToPreviousInterface));
    }
    else
    {
        m_bBackToPreviousInterface = bBackToPrevious;
        g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_SAVING_CONFIG_FILE)
                                               , SETTING_MESSAGEBOX_NOTE, "", "", NS_SET::g_SetPopNormalTime
                                               , -1, (void *)(&m_bBackToPreviousInterface));
    }

    return true;
}
