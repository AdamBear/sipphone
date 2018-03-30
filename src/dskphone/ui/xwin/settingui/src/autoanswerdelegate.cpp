#include "autoanswerdelegate.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguilogic/src/autoanswercontroller.h"
#include "settinguilogic/include/common.h"
#include "baseui/include/baseuicommon.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "account/include/modaccount.h"

CAutoAnswerDelegate::CAutoAnswerDelegate()
{

}

CAutoAnswerDelegate::~CAutoAnswerDelegate()
{

}

CSettingUIDelegateBase * CAutoAnswerDelegate::CreateAutoAnswerDelegate()
{
    CAutoAnswerDelegate * pDelegate = new CAutoAnswerDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CAutoAnswerDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIAccountDelegateBase::LoadPageData(bReLoad);

    if (NULL == m_pPageData)
    {
        return m_pPageData;
    }

    if (bReLoad)
    {
        LoadLineIDItem();
    }

    if (-1 != m_iLineID)
    {
        AutoAnswerData stAutoAnswer;
        if (g_pAutoAnswerController->GetAutoAnswerDataById(&stAutoAnswer, m_iLineID))
        {
            SetComboboxValue(AUTOANSWER_STATUS, stAutoAnswer.m_isOn);
            SetComboboxValue(AUTOANSWER_MUTE, stAutoAnswer.m_IsAutoAnswerMute);
        }
    }

    return m_pPageData;
}

bool CAutoAnswerDelegate::PageDataFilter(CSettingUIPageDataBase * pPagedata,
        bool bReload /* = false */)
{
    if (NULL == pPagedata)
    {
        return true;
    }

    bool bResult = CSettingUIAccountDelegateBase::PageDataFilter(pPagedata, bReload);

    if (-1 == m_iLineID)
    {
        SetItemShow(pPagedata, AUTOANSWER_LINE);
        SetItemShow(pPagedata, AUTOANSWER_STATUS);
        SetItemShow(pPagedata, AUTOANSWER_MUTE);
    }

    return bResult;
}

bool CAutoAnswerDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
                                       bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == m_pPageData || !pPageData->IsEnable() || !GetShowStatus())
    {
        return true;
    }

    AutoAnswerData stAutoAnswer;
    stAutoAnswer.m_isOn = GetComboboxValue(AUTOANSWER_STATUS);
    stAutoAnswer.m_IsAutoAnswerMute = GetComboboxValue(AUTOANSWER_MUTE);
    stAutoAnswer.m_nLineId = m_iLineID;

    g_pAutoAnswerController->SetAutoAnswerData(stAutoAnswer);

    ShowNoteMessgaeBox(_LANG(TRID_SAVING_CONFIG_FILE), MSGBOX_ID_SAVE);

    return true;
}

void CAutoAnswerDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (-1 != m_iLineID)
    {
        return;
    }

    ShowOnlyBackSoftkey(vecSoftKey);
}

void CAutoAnswerDelegate::OnComboBoxValueChange(int nIndex)
{
    if (NULL != m_pPageData && AUTOANSWER_LINE == GetItemIDByDataIndex(nIndex))
    {
        m_iLineID = GetComboboxValue(AUTOANSWER_LINE);
        g_SetUICallBackFun.RefreshUI(false);
    }
}

void CAutoAnswerDelegate::LoadLineIDItem()
{
    m_iLineID = -1;

    if (NULL == m_pPageData)
    {
        return;
    }

    CSettingUIItemDataBasePtr pItemData = m_pPageData->GetItemByItemId(AUTOANSWER_LINE);
    CSettingUIComboBoxItemData * pCmbItemData = CSettingUIComboBoxItemData::GetComboBoxDataItem(
                pItemData);
    if (NULL == pCmbItemData)
    {
        return;
    }

    pCmbItemData->ClearOptionItems();

    for (int i = 0; i < acc_AccountNum(); i++)
    {
        if (acc_IsAccountAvailable(i))
        {
            yl::string strAccount = acc_GetAccountShowText(i);
            yl::string strLine = _LANG(TRID_LINE);
            yl::string strDisp = commonAPI_FormatString("%s %d(%s)", strLine.c_str(), i + 1,
                                 strAccount.c_str());

            // 记录第一个可用账号
            if (AddLineIDOption(pCmbItemData, strDisp, i) && -1 == m_iLineID)
            {
                m_iLineID = i;
            }
        }
    }

#ifdef IF_FEATURE_PSTN
    // 获取用户信息
    ListAccounts listAccountId = acc_GetAvailableAccountList(ACCOUNT_TYPE_PSTN);
    ListAccounts::ListIterator iter = listAccountId.begin();
    for (; iter != listAccountId.end(); ++iter)
    {
        int iAccountId = *iter;
        yl::string strDisp = acc_GetLabel(iAccountId);

        if (AddLineIDOption(pCmbItemData, strDisp, iAccountId) && -1 == m_iLineID)
        {
            m_iLineID = iAccountId;
        }
    }
#endif

    if (-1 != m_iLineID)
    {
        yl::string strValue = commonAPI_FormatString("%d", m_iLineID);
        pCmbItemData->SetValue(strValue);
    }
}

bool CAutoAnswerDelegate::AddLineIDOption(CSettingUIComboBoxItemData * pCmbItemData,
        const yl::string & strDisp, int nLineID)
{
    if (NULL != pCmbItemData)
    {
        CSettingUIComboBoxOption * pOption = new CSettingUIComboBoxOption;
        if (NULL != pOption)
        {
            pOption->m_strDisplayName = strDisp;
            pOption->m_strItemVale = commonAPI_FormatString("%d", nLineID);

            if (pCmbItemData->AddOptionItem(pOption))
            {
                return true;
            }
            else
            {
                delete pOption;
            }
        }
    }

    return false;
}
