#include "settinguidndlineselectdelegate.h"
#include "uicallbackfun.h"
#include "setuidatadefine.h"
#include "settinguilogic/src/settinguicomboboxitemdata.h"
#include "settinguimenudetailitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "account/include/modaccount.h"
#include "settingui/include/pageiddefine.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "feature/include/modfeature.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

namespace
{
const yl::string kszItemID = "Empty";
}

CSettinguiDNDLineSelectDelegate::CSettinguiDNDLineSelectDelegate()
{

}

CSettinguiDNDLineSelectDelegate::~CSettinguiDNDLineSelectDelegate()
{

}

CSettingUIDelegateBase * CSettinguiDNDLineSelectDelegate::CreateDNDLineSelectDelegate()
{
    CSettinguiDNDLineSelectDelegate * pDelegate = new CSettinguiDNDLineSelectDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

CSettingUIPageDataBase * CSettinguiDNDLineSelectDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (!bReLoad || NULL == m_pPageData)
    {
        return m_pPageData;
    }

    int nAccountCount = acc_AccountNum();
    for (int i = 0; i < nAccountCount; ++i)
    {
        if (!acc_IsAccountAvailable(i))
        {
            continue;
        }

        yl::string strShowText = acc_GetAccountShowText(i);
        yl::string strValue = configParser_GetCfgItemStringValue(kszAccountDNDStatus, i);
        AddDNDAccountItem(strShowText, i, strValue);
    }

#ifdef IF_FEATURE_PSTN_DND
    // 获取用户信息
    ListAccounts listAccountId = acc_GetAccountList(ACCOUNT_TYPE_PSTN);
    ListAccounts::ListIterator iter = listAccountId.begin();
    for (; iter != listAccountId.end(); ++iter)
    {
        int iAccountId = *iter;
        yl::string strLabel = acc_GetLabel(iAccountId);
        yl::string strValue = configParser_GetCfgItemStringValue(kszAccountDNDStatus, iAccountId);
        AddDNDAccountItem(strLabel, iAccountId, strValue);
    }
#endif

    if (m_pPageData->GetTotalItemNumber() <= 0)
    {
        AddBaseItemData(m_pPageData, _LANG(TRID_NONE), "", kszItemID, true, false);
    }

    return m_pPageData;
}

bool CSettinguiDNDLineSelectDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /* = true */, bool bPopHint /* = true */)
{
    if (NULL == pPageData || !pPageData->IsEnable())
    {
        return false;
    }

    bool bChange = false;
    for (int i = 0; i < pPageData->GetTotalItemNumber(); ++i)
    {
        CSettingUIItemDataBasePtr pItemData = pPageData->GetItemByDataIndex(i);
        if (NULL != pItemData)
        {
            int nAccountID = atoi(pItemData->GetItemID().c_str());
            int nValue = atoi(pItemData->GetValue().c_str());
            bChange |= configParser_SetCfgItemIntValue(kszAccountDNDStatus, nAccountID, nValue,
                       CONFIG_LEVEL_PHONE);
        }
    }

    if (bChange)
    {
        etl_NotifyApp(TRUE, CONFIG_MSG_BCAST_CHANGED, ST_DND, 0);
    }

    m_bBackToPreviousInterface = bBackToPrevious;
    g_SetUICallBackFun.PopCommonMessageBox(m_strPageID, _LANG(TRID_NOTE), _LANG(TRID_SAVING_CONFIG_FILE)
                                           , SETTING_MESSAGEBOX_NOTE, "", "", NS_SET::g_SetPopNormalTime
                                           , -1, (void *)(&m_bBackToPreviousInterface));
    return true;

}

bool CSettinguiDNDLineSelectDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        if (!m_pPageData->IsEnable())
        {
            EnterDndPage(iDataIndex);
        }
        else
        {
            SavePageData(m_pPageData);
        }

        return true;
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

void CSettinguiDNDLineSelectDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (SOFTKEY_NUM != vecSoftKey.size() || NULL == m_pPageData)
    {
        return;
    }

    if (m_pPageData->GetTotalItemNumber() >= 1 && !IsItemShow(kszItemID))
    {
        if (!m_pPageData->IsEnable())
        {
            vecSoftKey[1].m_iType = ST_EMPTY;
            vecSoftKey[1].m_strSoftkeyTitle = "";
            vecSoftKey[2].m_iType = ST_EMPTY;
            vecSoftKey[2].m_strSoftkeyTitle = "";
            vecSoftKey[3].m_iType = ST_ENTER;
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ENTER);
        }
        else
        {
            if (IsAccountAllOn())
            {
                vecSoftKey[1].m_iType = ST_ALLOFF;
                vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_ALL_OFF);
            }
            else
            {
                vecSoftKey[1].m_iType = ST_ALLON;
                vecSoftKey[1].m_strSoftkeyTitle = _LANG(TRID_ALL_ON);
            }

            vecSoftKey[2].m_iType = ST_ENTER;
            vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_ENTER);
            vecSoftKey[3].m_iType = ST_SAVE;
            vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_SAVE);
        }
    }
    else
    {
        vecSoftKey[1].m_iType = ST_EMPTY;
        vecSoftKey[1].m_strSoftkeyTitle = "";
        vecSoftKey[2].m_iType = ST_EMPTY;
        vecSoftKey[2].m_strSoftkeyTitle = "";
        vecSoftKey[3].m_iType = ST_EMPTY;
        vecSoftKey[3].m_strSoftkeyTitle = "";
    }
}

bool CSettinguiDNDLineSelectDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
    if (ST_ALLOFF == iType || ST_ALLON == iType)
    {
        yl::string strValue = (ST_ALLOFF == iType) ? "0" : "1";
        SetAllItemValue(strValue);
        g_SetUICallBackFun.RefreshUI(false, GetPageID());
        return true;
    }
    else if (ST_ENTER == iType)
    {
        EnterDndPage(nDataIndex);
        return true;
    }
    else if (ST_SAVE == iType)
    {
        SavePageData(m_pPageData);
    }

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

void CSettinguiDNDLineSelectDelegate::EnterDndPage(int iDataIndex)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    CSettingUIItemDataBasePtr pItemData = m_pPageData->GetItemByDataIndex(iDataIndex);
    if (NULL != pItemData)
    {
        int nExtraData = atoi(pItemData->GetItemID().c_str());
        PageOpenData opPageData(kszPageIDAccountDND);
        opPageData.pExtraData = (void *)&nExtraData;
        g_SetUICallBackFun.EnterPage(opPageData);
        SetRefreshDataWhenTopDelegate(true);
    }
}

void CSettinguiDNDLineSelectDelegate::AddDNDAccountItem(const yl::string strDiap, int nAccountID,
        const yl::string & strValue)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    CSettingUIComboBoxItemDataPtr pItemData = new CSettingUIComboBoxItemData;
    if (NULL != pItemData)
    {
        pItemData->SetShowIndex(true);
        pItemData->SetDisplayName(strDiap);
        yl::string strID = commonAPI_FormatString("%d", nAccountID);
        pItemData->SetItemID(strID);
        pItemData->SetValue(strValue);
#if !IF_SUPPORT_DND_LINE_SELECT_SWITCH
        pItemData->SetEnable(false);
#endif
        pItemData->SetResetable(false);

        for (int i = 0; i < 2; ++i)
        {
            CSettingUIComboBoxOption * pOption = new CSettingUIComboBoxOption;
            if (NULL != pOption)
            {
                pOption->m_strDisplayName = (0 == i) ? _LANG(TRID_OFF) : _LANG(TRID_ON);
                pOption->m_strItemVale = commonAPI_FormatString("%d", i);

                if (!pItemData->AddOptionItem(pOption))
                {
                    delete pOption;
                }
            }
        }

        if (!m_pPageData->AddItem(pItemData))
        {
            delete pItemData;
        }
    }
}

bool CSettinguiDNDLineSelectDelegate::IsAccountAllOn()
{
    if (NULL == m_pPageData)
    {
        return false;
    }

    for (int i = 0; i < m_pPageData->GetTotalItemNumber(); ++i)
    {
        CSettingUIItemDataBasePtr pItemData = m_pPageData->GetItemByDataIndex(i);
        if (NULL != pItemData && "0" == pItemData->GetValue())
        {
            return false;
        }
    }

    return true;
}

void CSettinguiDNDLineSelectDelegate::SetAllItemValue(const yl::string & strValue)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    for (int i = 0; i < m_pPageData->GetTotalItemNumber(); ++i)
    {
        CSettingUIItemDataBasePtr pItemData = m_pPageData->GetItemByDataIndex(i);
        if (NULL != pItemData)
        {
            pItemData->SetValue(strValue);
        }
    }
}
