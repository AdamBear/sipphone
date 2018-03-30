#include "settinguistatusaccountdelegate.h"
#include "uicallbackfun.h"
#include <settinguilogic/include/settingdefine.h>
#include "settinguimanager.h"
#include "settingui/include/pageiddefine.h"
#include "settingui/include/itemnamedefine.h"
#include "settinguimenudetailitemdata.h"
#include "settinguilogic/src/settinguibasepagedata.h"

#include "settinguilogic/include/common.h"
#include "settinguilogic/src/statuscontroller.h"
#include "common/common_data_define.h"
#include "account/include/modaccount.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

CSettingUIStatusAccountDelegate::CSettingUIStatusAccountDelegate()
{
#ifdef IF_FEATURE_PSTN
    etl_RegisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED
                          , &CSettingUIStatusAccountDelegate::OnMessage);
    etl_RegisterMsgHandle(ACCOUNT_PSTN_STATUS_CHANGED, ACCOUNT_PSTN_STATUS_CHANGED
                          , &CSettingUIStatusAccountDelegate::OnMessage);
#endif
    etl_RegisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED
                          , &CSettingUIStatusAccountDelegate::OnMessage);
}

CSettingUIStatusAccountDelegate::~CSettingUIStatusAccountDelegate()
{
#ifdef IF_FEATURE_PSTN
    etl_UnregisterMsgHandle(ACCOUNT_DEFAULT_CHANGED, ACCOUNT_DEFAULT_CHANGED
                            , &CSettingUIStatusAccountDelegate::OnMessage);
    etl_UnregisterMsgHandle(ACCOUNT_PSTN_STATUS_CHANGED, ACCOUNT_PSTN_STATUS_CHANGED
                            , &CSettingUIStatusAccountDelegate::OnMessage);
#endif
    etl_UnregisterMsgHandle(ACCOUNT_STATUS_CHANGED, ACCOUNT_STATUS_CHANGED
                            , &CSettingUIStatusAccountDelegate::OnMessage);
}

CSettingUIDelegateBase * CSettingUIStatusAccountDelegate::CreateStatusAccountDelegate()
{
    CSettingUIStatusAccountDelegate * pDelegate = new CSettingUIStatusAccountDelegate;

    return (CSettingUIDelegateBasePtr)pDelegate;
}

LRESULT CSettingUIStatusAccountDelegate::OnMessage(msgObject & msg)
{
    if (ACCOUNT_STATUS_CHANGED == msg.message
#ifdef IF_FEATURE_PSTN
            || ACCOUNT_DEFAULT_CHANGED == msg.message
            || ACCOUNT_PSTN_STATUS_CHANGED == msg.message
#endif
       )
    {
        CSettingUIDelegateBase * pDelegate = g_pSettingUIManager->GetPageDelegate(kszPageIDStatusAccount);
        if (NULL != pDelegate && pDelegate == g_pSettingUIManager->GetTopDelegate())
        {
            g_SetUICallBackFun.RefreshUI(true, kszPageIDStatusAccount);
            return TRUE;
        }
    }

    return FALSE;
}

CSettingUIPageDataBase * CSettingUIStatusAccountDelegate::LoadPageData(bool bReLoad /*= false*/)
{
    CSettingUIDelegateBase::LoadPageData(bReLoad);

    if (bReLoad && NULL != m_pPageData)
    {
        LoadAccountListData();
    }

    return m_pPageData;
}

bool CSettingUIStatusAccountDelegate::SavePageData(CSettingUIPageDataBase * pPageData,
        bool bBackToPrevious /*= true*/, bool bPopHint /*= true*/)
{
    return true;
}

void CSettingUIStatusAccountDelegate::LoadAccountListData()
{
    if (NULL == m_pPageData)
    {
        return ;
    }

    // 获取用户信息
    int iAccountNum = acc_AccountNum();
    for (int i = 0; i < iAccountNum; i++)
    {
        yl::string strUserName = acc_GetAccountShowText(i);
        yl::string strStatus = GetStatusStringByAccountID(i);
#if IF_BUG_30082
        if (strUserName.empty())
        {
            strUserName = _LANG(TRID_ACCOUNT_EMPTY);
        }
        AddItem(strUserName, strStatus);
#else
        if (acc_IsInFinalRegisterState(i) || LS_REGISTERING == acc_GetState(i))
        {
#if IF_FEATURE_CLOUD_ACCOUNT
            if (1 == configParser_GetConfigInt(kszCloudActive) && ACCOUNT_YEALINK == acc_GetAccountType(i))
            {
                strUserName += "(Cloud)";
            }
#endif
            AddItem(strUserName, strStatus);
        }
#endif //IF_BUG_30082
    }

#ifdef IF_FEATURE_PSTN
    // 获取用户信息
    ListAccounts listAccountId = acc_GetAccountList(ACCOUNT_TYPE_PSTN);
    ListAccounts::ListIterator iter = listAccountId.begin();
    for (; iter != listAccountId.end(); ++iter)
    {
        int iAccountId = *iter;
        yl::string strLabel = acc_GetLabel(iAccountId);
        yl::string strStatus = GetStatusStringByAccountID(iAccountId);
        AddItem(strLabel, strStatus);
    }
#endif

    // 若没有可用账号, 则status->account插入一个空的item
    if (0 == m_pPageData->GetTotalItemNumber())
    {
        AddItem(_LANG(TRID_NONE));
    }
}

void CSettingUIStatusAccountDelegate::AddItem(const yl::string & strItemName,
        const yl::string & strValue /*= ""*/)
{
    if (NULL == m_pPageData)
    {
        return;
    }
    CSettingUIDetailMenuItemDataPtr pItemData = new CSettingUIDetailMenuItemData;
    if (NULL != pItemData)
    {
        pItemData->SetDisplayName(strItemName);
        pItemData->SetValue(strValue);
        pItemData->SetShowIndex(true);
        pItemData->SetShowColon(false);

        if (!m_pPageData->AddItem(pItemData))
        {
            delete pItemData;
        }
    }
}

yl::string CSettingUIStatusAccountDelegate::GetStatusStringByAccountID(int nAccountID)
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
