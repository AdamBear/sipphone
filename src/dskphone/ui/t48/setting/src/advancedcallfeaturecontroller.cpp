#include "advancedcallfeaturecontroller.h"

#include "setting/include/common.h"

#include "advancedcallfeaturemodel.h"
#include "basemethod.h"

#include "account/include/modaccount.h"
namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CAdvancedCallFeatureController
CAdvancedCallFeatureController * CAdvancedCallFeatureController::m_pController = 0;

CAdvancedCallFeatureController * CAdvancedCallFeatureController::GetInstance()
{
    // Instantiate the singleton instance if not existed.
    if (m_pController == NULL)
    {
        m_pController = new CAdvancedCallFeatureController();

        // Do initialization.
        if (m_pController != NULL)
        {
            m_pController->MyInit();
        }
    }
    return m_pController;
}

void CAdvancedCallFeatureController::ReleaseInstance()
{
    // Release the singleton instance if existed.
    if (m_pController != NULL)
    {
        // Do finalization.
        m_pController->MyFinal();

        DELETE_OBJ(m_pController);
    }
}

bool CAdvancedCallFeatureController::GetPageMenuListByClickAction(
    MenuItemListData * pData, const yl::string & strClickAction) const
{
    // Check the click action.
    if (strClickAction.empty() || pData == NULL)
    {
        return false;
    }

    // Call the base method.
    return NS_SETTING::GetPageMenuListByClickAction(pData, strClickAction);
}

const yl::string CAdvancedCallFeatureController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CAdvancedCallFeatureController::GetGeneralData(
    AdvancedCallFeatureGeneralData * pData) const
{
    if (pData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // Get key as send.
    pData->m_eType = static_cast<KeyAsSendType>(m_pModel->GetKeyAsSend());
    // Get history-save switch value.
    pData->m_isHistorySaved = m_pModel->IsHistorySaved();
    return true;
}

void CAdvancedCallFeatureController::SetGeneralData(
    const AdvancedCallFeatureGeneralData & refData)
{
    if (m_pModel != NULL)
    {
        // Set key as send.
        m_pModel->SetKeyAsSend(refData.m_eType);
        // Set history-save switch value.
        m_pModel->SetHistorySaved(refData.m_isHistorySaved);
    }
}

bool CAdvancedCallFeatureController::GetHotlineData(
    AdvancedCallFeatureHotlineData * pData) const
{
    if (pData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // Get hot line number.
    pData->m_strHotlineNumber = m_pModel->GetHotlineNumber();
    // Get hot line delay.
    pData->m_nHotlineDelay = m_pModel->GetHotlineDelay();
    return true;
}

void CAdvancedCallFeatureController::SetHotlineData(
    const AdvancedCallFeatureHotlineData & refData)
{
    if (m_pModel != NULL)
    {
        // Set hot line number.
        m_pModel->SetHotlineNumber(refData.m_strHotlineNumber);
        // Set hot line delay.
        m_pModel->SetHotlineDelay(refData.m_nHotlineDelay);
    }
}

bool CAdvancedCallFeatureController::GetValidAccountList(AccountItemListData * pData) const
{
    if (pData == NULL)
    {
        return false;
    }

    AccountData itemData;

    //获取有效账号
    ListAccounts listAccounts = acc_GetAvailableAccountList();
    for (ListAccounts::iterator iter = listAccounts.begin(); iter != listAccounts.end(); ++iter)
    {
        int iAccount = *iter;
        itemData.m_strName = acc_GetUsername(iAccount) + "@" + acc_GetServerName(iAccount);
        itemData.m_nID = iAccount;
        pData->m_listAccount.push_back(itemData);
    }
    return !pData->m_listAccount.empty();
}

int CAdvancedCallFeatureController::GetDefaultAccountID()
{
    return acc_GetDefaultAccount();
}
void CAdvancedCallFeatureController::SetDefaultAccount(const int & nAccountID)
{
    // http://10.2.1.199/Bug.php?BugID=18936
//  _AccountManager.SetDefaultAccount(nAccountID);
    acc_SetDefaultAccount(nAccountID, true);
}

bool CAdvancedCallFeatureController::GetAutoRedialData(
    AdvancedCallFeatureAutoRedialData * pData) const
{
    if (pData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // Get auto redial switch value.
    pData->m_isOn = m_pModel->IsAutoRedialOn();
    // Get auto redial interval.
    pData->m_nRedialInterval = m_pModel->GetAutoRedialInterval();
    // Get auto redial times.
    pData->m_nRedialTimes = m_pModel->GetAutoRedialTimes();
    return true;
}

void CAdvancedCallFeatureController::SetAutoRedialData(
    const AdvancedCallFeatureAutoRedialData & refData)
{
    if (m_pModel != NULL)
    {
        // Set auto redial switch value.
        m_pModel->SetAutoRedial(refData.m_isOn);
        // Set auto redial interval.
        m_pModel->SetAutoRedialInterval(refData.m_nRedialInterval);
        // Set auto redial times.
        m_pModel->SetAutoRedialTimes(refData.m_nRedialTimes);
    }
}

bool CAdvancedCallFeatureController::GetCallCompletionData(
    AdvancedCallFeatureCallCompletionData * pData) const
{
    if (pData == NULL || m_pModel == NULL)
    {
        return false;
    }

    // Get call-completion switch value.
    pData->m_isOn = m_pModel->IsCallCompletionOn();
    return true;
}

void CAdvancedCallFeatureController::SetCallCompletionData(
    const AdvancedCallFeatureCallCompletionData & refData)
{
    if (m_pModel != NULL)
    {
        // Set call-completion switch value.
        m_pModel->SetCallCompletion(refData.m_isOn);
    }
}

CAdvancedCallFeatureController::CAdvancedCallFeatureController()
    : m_pModel(NULL)
{

}

CAdvancedCallFeatureController::~CAdvancedCallFeatureController()
{

}

bool CAdvancedCallFeatureController::MyInit()
{
    // Instantiate the model instance.
    if (m_pModel == NULL)
    {
        m_pModel = new CAdvancedCallFeatureModel();
    }
    return m_pModel != NULL;
}

bool CAdvancedCallFeatureController::MyFinal()
{
    // Release the model instance.
    if (m_pModel != NULL)
    {
        delete m_pModel;
        m_pModel = NULL;
    }
    return m_pModel == NULL;
}

void CAdvancedCallFeatureController::SetHotDeskBaseData(const HotDeskBaseData & refData)
{
    bool bChange = false;
    yl::string strTemp = refData.m_strUserName;
    if (strTemp.empty())
    {
        strTemp = refData.m_strRegisterName;
    }

    int iAccountID = 0;

#if IF_BUG_25349
    yl::string strClearAccount = configParser_GetConfigString(kszHotDeskingClearAccount);
    YLList<yl::string> listAccountID;

    if (commonAPI_splitStringBySeparator(strClearAccount, ',', listAccountID)
            && listAccountID.size() > 0)
    {
        YLList<yl::string>::iterator it = listAccountID.begin();
        for (; it != listAccountID.end(); it++)
        {
            yl::string strAccountID = *it;
            int nID = atoi(strAccountID.c_str());

            if (listAccountID.size() == 1
                    && (nID > acc_AccountNum() || nID <= 0))
            {
                iAccountID = 0;
                break;
            }
            else if (nID > 0 && nID <= acc_AccountNum())
            {
                iAccountID = nID - 1;
                break;
            }
        }
    }
#endif

    // Set account status.
    bChange = configParser_SetCfgItemIntValue(kszAccountEnable, iAccountID, 1, CONFIG_LEVEL_PHONE)
              || bChange;
    // Set hotdesk type
    bChange = configParser_SetCfgItemIntValue(kszAccountIsHotDeskAccount, iAccountID, 1,
              CONFIG_LEVEL_PHONE) || bChange;
    // Set account label.
    bChange = configParser_SetCfgItemStringValue(kszAccountLabel, iAccountID, strTemp.c_str(),
              CONFIG_LEVEL_PHONE) || bChange;
    // Set account display name.
    bChange = configParser_SetCfgItemStringValue(kszAccountDisplayName, iAccountID, strTemp.c_str(),
              CONFIG_LEVEL_PHONE) || bChange;

    // Set account register name.
    if (refData.m_strRegisterName.empty())
    {
        bChange = configParser_SetCfgItemStringValue(kszAccountRegisterName, iAccountID,
                  refData.m_strUserName.c_str(), CONFIG_LEVEL_PHONE) || bChange;
    }
    else
    {
        bChange = configParser_SetCfgItemStringValue(kszAccountRegisterName, iAccountID,
                  refData.m_strRegisterName.c_str(), CONFIG_LEVEL_PHONE) || bChange;
    }

    bChange = configParser_SetCfgItemStringValue(kszAccountUserName, iAccountID, strTemp.c_str(),
              CONFIG_LEVEL_PHONE) || bChange;

    if (configParser_GetConfigInt(kszHotDeskingPassword) != 0)
    {
        bChange = configParser_SetCfgItemStringValue(kszAccountPassword, iAccountID,
                  refData.m_strPassword.c_str(), CONFIG_LEVEL_PHONE) || bChange;
    }

    bool bLocked = configParser_GetConfigInt(kszLockLevel) != 0;
    if ((!refData.m_strSipServer1.empty()
            || !refData.m_strSipServer2.empty()) && !bLocked)
    {
        // Set account sip server1
        bChange = configParser_SetCfgItemStringValue(kszAccountSIPServer1Host, iAccountID,
                  refData.m_strSipServer1.c_str(), CONFIG_LEVEL_PHONE) || bChange;
        // Set account sip server2
        bChange = configParser_SetCfgItemStringValue(kszAccountSIPServer2Host, iAccountID,
                  refData.m_strSipServer2.c_str(), CONFIG_LEVEL_PHONE) || bChange;
    }

    if (!refData.m_strOutboundProxy1.empty()
            || !refData.m_strOutboundProxy2.empty())
    {
        // Set account outbound status.
        bChange = configParser_SetCfgItemIntValue(kszAccountOutboundSwitch, 0,
                  refData.m_bOutboundEnable ? 1 : 0, CONFIG_LEVEL_PHONE) || bChange;

        if (!bLocked)
        {
            // Set account outbound proxy.
            //bChange = configParser_SetCfgItemStringValue(kszAccountBakSIPServerHost, 0, refData.m_strOutboundProxy1.c_str(), CONFIG_LEVEL_PHONE) || bChange;
            bChange = configParser_SetCfgItemStringValue(kszAccountOutboundProxy, 0, 1,
                      refData.m_strOutboundProxy1.c_str(), CONFIG_LEVEL_PHONE) || bChange;
            bChange = configParser_SetCfgItemStringValue(kszAccountOutboundProxy, 0, 2,
                      refData.m_strOutboundProxy2.c_str(), CONFIG_LEVEL_PHONE) || bChange;
        }
    }

    if (bChange || configParser_GetConfigInt(kszExitWizardAfterLogin) == 0)
    {
        //Notify Config Change
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, iAccountID);

        //异步通知账号配置改变
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, iAccountID);
    }
}

}  // namespace NS_SETTING
