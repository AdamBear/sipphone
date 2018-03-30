#include "settinguilineselectdelegate.h"
#include "setuidatadefine.h"
#include "uicallbackfun.h"
#include "settingui/include/pageiddefine.h"
#include "settinguilogic/src/settinguimenuitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "account/include/modaccount.h"
#include "messagebox/messageboxbase.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettinguiLineSelectDelegate::CSettinguiLineSelectDelegate()
    : m_strNextPageID("")
{

}

CSettinguiLineSelectDelegate::~CSettinguiLineSelectDelegate()
{

}

CSettingUIDelegateBase * CSettinguiLineSelectDelegate::CreateLineSelectDelegate()
{
    CSettinguiLineSelectDelegate * pDelegate = new CSettinguiLineSelectDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

void CSettinguiLineSelectDelegate::InitExtraData(void * pData)
{
    yl::string * pString = (yl::string *)pData;
    if (NULL != pString)
    {
        m_strNextPageID = *pString;
    }
}

CSettingUIPageDataBase * CSettinguiLineSelectDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettinguiXSIDelegatebase::LoadPageData(bReLoad);

    if (!bReLoad || NULL == m_pPageData)
    {
        return m_pPageData;
    }

    if (0 == acc_GetRegisteredAccountNum)
    {
        ShowMessageBox(_LANG(TRID_WARNING), _LANG(TRID_NO_ACCOUNT), XSI_MSGBOX_ID_NO_ACCOUNT, 1500,
                       MESSAGEBOX_NOTE);
        return m_pPageData;
    }

    if (kszPageIDAnyWhereLocationList == m_strNextPageID
            || kszPageIDCallControllExecutiveDetails == m_strNextPageID
            || kszPageIDCallControllAssistantDetails == m_strNextPageID)
    {
#ifdef _WIN32
        LoadWin32ListAccount();
        return m_pPageData;
#endif
    }

    LoadNormalLstAccount();

    return m_pPageData;

}

bool CSettinguiLineSelectDelegate::OnAction(int eActionType, int iDataIndex)
{
    if (SET_ACTION_TYPE_CLICK == eActionType)
    {
        CSettingUIItemDataBasePtr pItemData = m_pPageData->GetItemByDataIndex(iDataIndex);
        if (NULL != pItemData && !m_strNextPageID.empty())
        {
            int nExtraData = atoi(pItemData->GetItemID().c_str());

            if (IsEnableToEnterNextPage(m_strNextPageID, nExtraData))
            {
                PageOpenData opPageData(m_strNextPageID);
                opPageData.pExtraData = (void *)&nExtraData;
                g_SetUICallBackFun.EnterPage(opPageData);

                SetRefreshDataWhenTopDelegate(true);
            }
            return true;
        }
    }

    return CSettingUIDelegateBase::OnAction(eActionType, iDataIndex);
}

void CSettinguiLineSelectDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
    if (SOFTKEY_NUM != vecSoftKey.size())
    {
        return;
    }

    if (acc_GetRegisteredAccountNum > 0)
    {
        vecSoftKey[3].m_iType = ST_ENTER;
        vecSoftKey[3].m_strSoftkeyTitle = _LANG(TRID_ENTER);
    }
    else
    {
        vecSoftKey[3].m_iType = ST_EMPTY;
        vecSoftKey[3].m_strSoftkeyTitle = "";
    }
}

void CSettinguiLineSelectDelegate::LoadWin32ListAccount()
{
    int nAccountCount = acc_GetRegisteredAccountNum();
    for (int i = 0; i < nAccountCount; ++i)
    {

        yl::string strID = commonAPI_FormatString("%d", i);
        yl::string strShowText = commonAPI_FormatString("line%d:", i + 1);

        AddAccountItem(strShowText, strID);
    }
}

void CSettinguiLineSelectDelegate::LoadNormalLstAccount()
{
    int nAccountCount = acc_AccountNum();
    for (int i = 0; i < nAccountCount; ++i)
    {
        if (!acc_IsAccountAvailable(i))
        {
            continue;
        }

        yl::string strID = commonAPI_FormatString("%d", i);
        yl::string strShowText = acc_GetAccountShowText(i);

        AddAccountItem(strShowText, strID);
    }
}

void CSettinguiLineSelectDelegate::AddAccountItem(const yl::string & strName,
        const yl::string & strID)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    AddMenuItem(m_pPageData, strID, strName);
}
