#include "settinguiadvaccountlistdelegate.h"
#include "uicallbackfun.h"
#include "settinguimanager.h"
#include "settinguimenudetailitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"
#include "account/include/modaccount.h"
#include "devicelib/phonedevice.h"
#include "translateiddefine.h"
#include "setuidatadefine.h"
#include "settingui/include/pageiddefine.h"
#include "languagehelper/modlangtranslate.h"
#include "settinguilogic/src/advsettingcontroller.h"
#include "idleuilogic/include/modidleul.h"
#include "idlescreen//include/modidlescreen.h"


namespace
{
const char * const kszOpenPage = "openpage(";
#define ACTION_OPENPAGE_LENGTH 10
}

CSettinguiAdvAccountListDelegate::CSettinguiAdvAccountListDelegate()
{
#ifdef IF_FEATURE_PSTN
    etl_RegisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED
                          , &CSettinguiAdvAccountListDelegate::OnMessage);
    etl_RegisterMsgHandle(ACCOUNT_PSTN_STATUS_CHANGED, ACCOUNT_PSTN_STATUS_CHANGED
                          , &CSettinguiAdvAccountListDelegate::OnMessage);
#endif
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED
                          , &CSettinguiAdvAccountListDelegate::OnMessage);
}

CSettinguiAdvAccountListDelegate::~CSettinguiAdvAccountListDelegate()
{
#ifdef IF_FEATURE_PSTN
    etl_UnregisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED
                            , &CSettinguiAdvAccountListDelegate::OnMessage);
    etl_UnregisterMsgHandle(ACCOUNT_PSTN_STATUS_CHANGED, ACCOUNT_PSTN_STATUS_CHANGED
                            , &CSettinguiAdvAccountListDelegate::OnMessage);
#endif
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED
                            , &CSettinguiAdvAccountListDelegate::OnMessage);
}

CSettingUIDelegateBase * CSettinguiAdvAccountListDelegate::CreateAdvAccountListDelegate()
{
    CSettinguiAdvAccountListDelegate * pDelegate = new CSettinguiAdvAccountListDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettinguiAdvAccountListDelegate:: OnMessage(msgObject & msg)
{
    if (ACCOUNT_STATUS_CHANGED == msg.message
#ifdef IF_FEATURE_PSTN
            || ACCOUNT_DEFAULT_CHANGED == msg.message
            || ACCOUNT_PSTN_STATUS_CHANGED == msg.message
#endif
       )
    {
        CSettingUIDelegateBase * pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDAdvAccountList);
#ifdef IF_FEATURE_PSTN
        if (NULL == pDelegate)
        {
            pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDPSTNAccountList);
        }
#endif

        if (NULL != pDelegate)
        {
            if (pDelegate == g_pSettingUIManager->GetTopDelegate())
            {
                g_SetUICallBackFun.RefreshUI(true, kszPageIDAdvAccountList);
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

CSettingUIPageDataBase * CSettinguiAdvAccountListDelegate::LoadPageData(bool bReLoad /* = false */)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        LoadAccountItem(m_pPageData);
    }

    return m_pPageData;
}

bool CSettinguiAdvAccountListDelegate::OnSoftkeyAction(int iType, bool bLongPress, int nDataIndex)
{
#ifdef IF_FEATURE_PSTN
    if (ST_DEFAULT == iType && NULL != m_pPageData)
    {
        int nAccountID = GetAccountIDByIndex(m_pPageData->GetFocusIndex());
        if (-1 != nAccountID)
        {
            acc_SetDefaultAccount(nAccountID, true);
        }

        return true;
    }
#endif

    return CSettingUIDelegateBase::OnSoftkeyAction(iType, bLongPress, nDataIndex);
}

void CSettinguiAdvAccountListDelegate::OnEnterPage(const yl::string & strPage, int iDataIndex)
{
    yl::string strRealPage = strPage;
#if IF_FEATURE_CLOUD_ACCOUNT
    if (kszPageIDAdvCommonAccountSetting == strPage || kszPageIDAdvCloudAccountSetting == strPage)
    {
        if (1 == configParser_GetConfigInt(kszCloudActive))
        {
            strRealPage = kszPageIDAdvCloudAccountSetting;
        }
        else
        {
            strRealPage = kszPageIDAdvCommonAccountSetting;
        }
    }
#endif
    PageOpenData opPageData(strRealPage);
    int nRealIndex = GetAccountIDByIndex(iDataIndex);
    opPageData.pExtraData = (void *)&nRealIndex;
    g_SetUICallBackFun.EnterPage(opPageData);
}

void CSettinguiAdvAccountListDelegate::GetPagePrivateSoftkey(CArraySoftKeyBarData & vecSoftKey)
{
#ifdef IF_FEATURE_PSTN
    if (SOFTKEY_NUM != vecSoftKey.size() || NULL == m_pPageData)
    {
        return;
    }

    int nFocusIndex = m_pPageData->GetFocusIndex();
    int nAccountID = GetAccountIDByIndex(nFocusIndex);

    if (acc_GetDefaultAccount() != nAccountID && acc_IsAccountAvailable(nAccountID))
    {
        vecSoftKey[2].m_iType = ST_DEFAULT;
        vecSoftKey[2].m_strSoftkeyTitle = _LANG(TRID_DEFAULT);
        return ;
    }

    vecSoftKey[2].m_iType = ST_EMPTY;
    vecSoftKey[2].m_strSoftkeyTitle = "";

    if (m_pPageData->GetTotalItemNumber() <= 0)
    {
        vecSoftKey[3].m_iType = ST_EMPTY;
        vecSoftKey[3].m_strSoftkeyTitle = "";
    }
#endif
}

int CSettinguiAdvAccountListDelegate::GetAccountIDByIndex(int nIndex)
{
    CSettingUIItemDataBase * pItemData = GetItemDataByDataIndex(nIndex);
    if (NULL != pItemData)
    {
        return atoi(pItemData->GetItemID().c_str());
    }

    return -1;
}

void CSettinguiAdvAccountListDelegate::LoadAccountItem(CSettingUIPageDataBase * pPageData)
{
    if (NULL == pPageData)
    {
        return;
    }
#ifdef IF_FEATURE_PSTN
    if (kszPageIDPSTNAccountList != GetPageID())
#endif
    {
        int iAccountNum = acc_AccountNum();
        for (int i = 0; i < iAccountNum; i++)
        {
            yl::string strUserName = acc_GetAccountShowText(i);
            yl::string strStatus = GetStatusStringByAccountID(i);

            yl::string strNextPageID = kszPageIDAdvCommonAccountSetting;
#if IF_FEATURE_CLOUD_ACCOUNT
            if (1 == configParser_GetConfigInt(kszCloudActive))
            {
                strNextPageID = kszPageIDAdvCloudAccountSetting;
            }
#endif

            if (strUserName.empty())
            {
                strUserName = commonAPI_FormatString("(%s)", _LANG(TRID_EMPTY).c_str());
            }
#if !IF_BUG_30082
#if IF_FEATURE_CLOUD_ACCOUNT
            else if (1 == configParser_GetConfigInt(kszCloudActive) && ACCOUNT_YEALINK == acc_GetAccountType(i))
            {
                strUserName += "(Cloud)";
            }
#endif
#endif //IF_BUG_30082

            AddDetailMenuItem(strUserName, strStatus, i, strNextPageID);
        }
    }

#ifdef IF_FEATURE_PSTN
    // 获取用户信息
    if (kszPageIDPSTNAccountList == GetPageID())
    {
        VEC_INT_DATA vecId;

        IdleUL_GetPstnIdList(vecId);

        for (int i = 0; i < vecId.size(); ++i)
        {
            int iAccountId = vecId[i];
            AddPstnItem(iAccountId);
        }
    }
    else
    {
        ListAccounts listAccountId = acc_GetAccountList(ACCOUNT_TYPE_PSTN);
        ListAccounts::ListIterator iter = listAccountId.begin();
        for (; iter != listAccountId.end(); ++iter)
        {
            int iAccountId = *iter;
            AddPstnItem(iAccountId);
        }
    }
#endif
}

#ifdef IF_FEATURE_PSTN
void CSettinguiAdvAccountListDelegate::AddPstnItem(int nAccountID)
{
    yl::string strLabel = acc_GetLabel(nAccountID);
    yl::string strStatus = GetStatusStringByAccountID(nAccountID);
    yl::string strNextPageID = kszPageIDAdvPSTNAccountSetting;
    AddDetailMenuItem(strLabel, strStatus, nAccountID, strNextPageID);
}
void CSettinguiAdvAccountListDelegate::ClearPstnInfo()
{
    if (kszPageIDPSTNAccountList != GetPageID())
    {
        return;
    }
    IdleUL_ClearPstnIdList();
    idleScreen_DropStatus(PS_STATE_PSTN_ACCOUNT_NOTE);
}

#endif

void CSettinguiAdvAccountListDelegate::AddDetailMenuItem(const yl::string & strDisplayName,
        const yl::string & strValue, int nAccountID, const yl::string & strNextPageID)
{
    if (NULL == m_pPageData)
    {
        return;
    }

    CSettingUIDetailMenuItemDataPtr pItemData = new CSettingUIDetailMenuItemData;
    if (NULL != pItemData)
    {
        pItemData->SetDisplayName(strDisplayName);
        pItemData->SetValue(strValue);
        pItemData->SetShowIndex(true);
        pItemData->SetShowColon(false);
        yl::string strID = commonAPI_FormatString("%d", nAccountID);
        pItemData->SetItemID(strID);
        yl::string strAction = commonAPI_FormatString("%s%s)", kszOpenPage, strNextPageID.c_str());
        pItemData->SetClickAction(strAction);

        if (!m_pPageData->AddItem(pItemData))
        {
            delete pItemData;
        }
    }
}

yl::string CSettinguiAdvAccountListDelegate::GetStatusStringByAccountID(int nAccountID)
{
    yl::string strAccountStatus = "";

    bool bDefault = nAccountID == acc_GetDefaultAccount();

    switch (acc_GetState(nAccountID))
    {
    case LS_UNKNOW:
        strAccountStatus = _LANG(TRID_UNKNOWN);
        break;
    case LS_DISABLE:
        strAccountStatus = _LANG(TRID_DISABLED);
        break;
    case LS_REGISTERING:
        strAccountStatus = _LANG(TRID_REGISTERING);
        break;
    case LS_REGISTERED:
    case LS_DISABLE_REG:
        {
            if (bDefault && (acc_AccountNum(ACCOUNT_TYPE_ALL) > 1 || PT_CP920 != devicelib_GetPhoneType()))
            {
                strAccountStatus = _LANG(TRID_DEFAULT_ACCOUNT);
            }
            else
            {
                strAccountStatus = _LANG(TRID_REGISTERED);
            }
        }
        break;
    case LS_REGISTER_FAIL:
        strAccountStatus = _LANG(TRID_REGISTER_FAIL);
        break;
    case LS_BE_UNREGISTERED:
        strAccountStatus = _LANG(TRID_UNREGISTERED);
        break;
    case LS_SYSTEM_DEFAULT_ACCOUNT:
        strAccountStatus = _LANG(TRID_DEFAULT_ACCOUNT);
        break;
    case LS_INEXISTENCE:
        strAccountStatus = _LANG(TRID_INEXISTENCE);
        break;
    case LS_UNREGISTERING:
        strAccountStatus = _LANG(TRID_UNREGISTERING);
        break;
    case LS_EXIT:
        strAccountStatus = _LANG(TRID_EXIT);
        break;
    default:
        break;
    }

    return strAccountStatus;
}

bool CSettinguiAdvAccountListDelegate::OnBackAction()
{
#ifdef IF_FEATURE_PSTN
    ClearPstnInfo();
#endif
    return CSettingUIDelegateBase::OnBackAction();
}

void CSettinguiAdvAccountListDelegate::OnReturnToIdle()
{
#ifdef IF_FEATURE_PSTN
    ClearPstnInfo();
#endif
    CSettingUIDelegateBase::OnReturnToIdle();
}
