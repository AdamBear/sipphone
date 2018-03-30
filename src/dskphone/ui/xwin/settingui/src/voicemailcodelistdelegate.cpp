#include "voicemailcodelistdelegate.h"
#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settinguilogic/src/settinguilineedititemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "account/include/modaccount.h"
#include "devicelib/phonedevice.h"
#include "translateiddefine.h"
#include "setuidatadefine.h"
#include "settingui/include/pageiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
#define SET_IME_ALL "all"
#define SET_DEFAULT_123 "123"
#define ITEM_MAX_LENGTH 99
}


CVoiceMailCodeListDelegate::CVoiceMailCodeListDelegate()
{
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED
                          , &CVoiceMailCodeListDelegate::OnMessage);
}

CVoiceMailCodeListDelegate::~CVoiceMailCodeListDelegate()
{
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED
                            , &CVoiceMailCodeListDelegate::OnMessage);
}

CSettingUIDelegateBase * CVoiceMailCodeListDelegate::CreateVoiceMailCodeListDelegate()
{
    CVoiceMailCodeListDelegate * pDelegate = new CVoiceMailCodeListDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CVoiceMailCodeListDelegate::OnMessage(msgObject & msg)
{
    if (ACCOUNT_STATUS_CHANGED == msg.message)
    {
        CSettingUIDelegateBase * pDelegate = g_pSettingUIManager->GetPageDelegate(
                kszPageIDVoiceMailCodeList);
        if (NULL != pDelegate)
        {
            if (pDelegate == g_pSettingUIManager->GetTopDelegate())
            {
                g_SetUICallBackFun.RefreshUI(true, kszPageIDVoiceMailCodeList);
            }
            else
            {
                pDelegate->SetRefreshDataWhenTopDelegate(true);
            }

            return TRUE;
        }
    }

    return FALSE;
}

CSettingUIPageDataBase * CVoiceMailCodeListDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (NULL != m_pPageData && bReLoad)
    {
        LoadMailCodeItem(m_pPageData);
    }

    return m_pPageData;
}

yl::string CVoiceMailCodeListDelegate::GetValueByIndex(int nIndex)
{
    yl::string strValue = "";
    if (NULL != m_pPageData)
    {
        CSettingUIItemDataBase * pItemData = GetItemDataByDataIndex(nIndex);
        if (NULL != pItemData)
        {
            strValue = pItemData->GetValue();
        }
    }

    return strValue;
}

int CVoiceMailCodeListDelegate::GetRealIndex(int nIndex)
{
    CSettingUIItemDataBase * pItemData = GetItemDataByDataIndex(nIndex);
    if (NULL != pItemData)
    {
        return atoi(pItemData->GetItemID().c_str());
    }

    return -1;
}

void CVoiceMailCodeListDelegate::LoadMailCodeItem(CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }

    int nTotalAccount = acc_AccountNum();
    for (int i = 0; i < nTotalAccount; ++i)
    {
        if (LS_REGISTERED != acc_GetState(i))
        {
            continue;
        }

        yl::string strValue = configParser_GetCfgItemStringValue(kszAccountVoiceMailNumber, i);
        yl::string strName = acc_GetAccountShowText(i);

        if (strName.empty())
        {
            strName = _LANG(TRID_ACCOUNT_EMPTY);
        }

        yl::string strID = commonAPI_FormatString("%d", i);

		AddLineEditItemData(pPageData, strID, strName, strValue, ITEM_MAX_LENGTH, SET_IME_ALL, SET_DEFAULT_123,
			false, "", true);
    }
}

void CVoiceMailCodeListDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (m_pPageData->GetTotalItemNumber() > 0)
    {
        vecSoftKey[3].m_iType = ST_SAVE;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SAVE);
    }
}

bool CVoiceMailCodeListDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    for (int i = 0; i < m_pPageData->GetTotalItemNumber(); ++i)
    {
        yl::string strValue = GetValueByIndex(i);
        int nRealIndex = GetRealIndex(i);
        configParser_SetCfgItemStringValue(kszAccountVoiceMailNumber, nRealIndex, strValue,
                                           CONFIG_LEVEL_PHONE);
    }

    return CSettingUIDelegateBase::SavePageData(pPageData, bBackToPrevious, bPopHint);
}
