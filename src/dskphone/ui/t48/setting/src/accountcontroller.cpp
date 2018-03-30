#include "accountcontroller.h"

#include <ylstring.h>  // To use NULL.

#include "account/include/modaccount.h"
#include "commonapi/stringhelperapi.h"
#include "configparser/modconfigparser.h"
#include "setting/include/common.h"
#include <configiddefine.h>
#include <path_define.h>
#include "etlmsghandler/modetlmsghandler.h"

#include "basemethod.h"

namespace NS_SETTING
{
//////////////////////////////////////////////////////////////////////////
// Class CAccountController.
CAccountController * CAccountController::m_pController = NULL;

CAccountController * CAccountController::GetInstance()
{
    // Instantiate the singleton instance.
    if (m_pController == NULL)
    {
        m_pController = new CAccountController();
    }
    return m_pController;
}

void CAccountController::ReleaseInstance()
{
    // Release the singleton instance.
    DELETE_OBJ(m_pController);
}

bool CAccountController::GetPageMenuListByClickAction(
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

const yl::string CAccountController::GetTitleByClickAction(
    const yl::string & strClickAction) const
{
    if (strClickAction.empty())
    {
        return "";
    }

    // Call the base method.
    return NS_SETTING::GetTitleByClickAction(strClickAction);
}

bool CAccountController::GetAccountByClickAction(AccountDetailData * pData,
        const yl::string & strClickAction) const
{
    if (pData == NULL || strClickAction.empty())
    {
        return false;
    }

    // Get account Id by click action.
    int nId = commonAPI_GetIdFromStrAction(strClickAction);
//  int nId = CommonAPI_ParseIdByClickAction(strClickAction, kszCAAccount);
    if (nId == -1)
    {
        return false;
    }

    // Get account label.
    pData->m_strLabel = configParser_GetCfgItemStringValue(kszAccountLabel, nId);
    // Get account display name.
    pData->m_strDisplayName = configParser_GetCfgItemStringValue(kszAccountDisplayName, nId);
    // Get account register name.
    pData->m_strRegisterName = configParser_GetCfgItemStringValue(kszAccountRegisterName, nId);
    // Get account username.
    pData->m_strUserName = configParser_GetCfgItemStringValue(kszAccountUserName, nId);
    // Get account password.
    pData->m_strPassword = configParser_GetCfgItemStringValue(kszAccountPassword, nId);

    // Get account sip server.
    yl::string strPath = commonAPI_FormatString("%s%d.cfg", SIP_ACCOUNT_FILE_PATH, nId);
    pData->m_strSipServer1 = configParser_GetCfgItemStringValue(kszAccountServerAddr, nId, 1);
    pData->m_strSipServer2 = configParser_GetCfgItemStringValue(kszAccountServerAddr, nId, 2);

    if (pData->m_strSipServer1.empty()
            && pData->m_strSipServer2.empty())
    {
        yl::string strOldServer = configParser_GetCfgItemStringValue(kszAccountSIPServerHost, nId);
        pData->m_strSipServer1 = strOldServer;
        /*
                // Get account outbound proxy.
                pData->m_strOutboundProxy = configParser_GetCfgItemStringValue(kszAccountOutboundProxy, nId);
        */
    }

    // Get account outbound proxy.
    pData->m_strOutboundProxy1 = configParser_GetCfgItemStringValue(kszAccountOutboundProxy, nId, 1);
    pData->m_strOutboundProxy2 = configParser_GetCfgItemStringValue(kszAccountOutboundProxy, nId, 2);
    // Get account status.
    pData->m_isActivated = configParser_GetCfgItemIntValue(kszAccountEnable, nId) == 1;
    // Get account outbound status.
    pData->m_isOutboundEnable = configParser_GetCfgItemIntValue(kszAccountOutboundSwitch, nId) == 1;
    // Get account auto answer
    //pData->m_bIsAutoAnswer = configParser_GetCfgItemIntValue(kszAutoAnswerSwitch, nId) == 1;
    pData->m_isNATEnable = configParser_GetCfgItemIntValue(kszAccountStunSwitch, nId) == 1;

    pData->m_nProxyFallbackInterval = configParser_GetCfgItemIntValue(kszAccountProxyFallbackInterval,
                                      nId);

    return true;
}

void CAccountController::SetAccountByClickAction(
    const yl::string & strClickAction, const AccountDetailData & refData)
{
    if (strClickAction.empty())
    {
        return;
    }

    // Get account Id by click action.
    int nId = commonAPI_GetIdFromStrAction(strClickAction);
//  int nId = CommonAPI_ParseIdByClickAction(strClickAction, kszCAAccount);
    if (nId == -1)
    {
        return;
    }

    bool bXSIConfigChanged = false;
    bool bChanged = false;
    // Set account label.
#if IF_BUG_26237
    bool bLabelChanged = false;

    bLabelChanged = configParser_SetCfgItemStringValue(kszAccountLabel, nId, refData.m_strLabel,
                    CONFIG_LEVEL_PHONE);
#else
    bChanged = configParser_SetCfgItemStringValue(kszAccountLabel, nId, refData.m_strLabel,
               CONFIG_LEVEL_PHONE) || bChanged;
#endif
    // Set account display name.
    bChanged = configParser_SetCfgItemStringValue(kszAccountDisplayName, nId, refData.m_strDisplayName,
               CONFIG_LEVEL_PHONE) || bChanged;
    // Set account register name.
    bool bRegisterChanged = configParser_SetCfgItemStringValue(kszAccountRegisterName, nId,
                            refData.m_strRegisterName, CONFIG_LEVEL_PHONE);
    bChanged = bRegisterChanged || bChanged;
    bXSIConfigChanged = bRegisterChanged || bXSIConfigChanged;

    // Set account username.
    bChanged = configParser_SetCfgItemStringValue(kszAccountUserName, nId, refData.m_strUserName,
               CONFIG_LEVEL_PHONE) || bChanged;
    // Set account password.
    bool bPwdChanged = configParser_SetCfgItemStringValue(kszAccountPassword, nId,
                       refData.m_strPassword, CONFIG_LEVEL_PHONE);
    bChanged = bPwdChanged || bChanged;
    bXSIConfigChanged = bPwdChanged || bXSIConfigChanged;

    bool bLocked = configParser_GetConfigInt(kszLockLevel) != 0;
    if (!bLocked)
    {
        yl::string strPath = commonAPI_FormatString("%s%d.cfg", SIP_ACCOUNT_FILE_PATH, nId);
        // Set account outbound proxy.
        bChanged = configParser_SetCfgItemStringValue(kszAccountOutboundProxy, nId, 1,
                   refData.m_strOutboundProxy1, CONFIG_LEVEL_PHONE) || bChanged;
        bChanged = configParser_SetCfgItemStringValue(kszAccountOutboundProxy, nId, 2,
                   refData.m_strOutboundProxy2, CONFIG_LEVEL_PHONE) || bChanged;

        // Set account sip server.
        bChanged = configParser_SetCfgItemStringValue(kszAccountServerAddr, nId, 1, refData.m_strSipServer1,
                   CONFIG_LEVEL_PHONE) || bChanged;
        bChanged = configParser_SetCfgItemStringValue(kszAccountServerAddr, nId, 2, refData.m_strSipServer2,
                   CONFIG_LEVEL_PHONE) || bChanged;
    }

    bChanged = configParser_SetCfgItemIntValue(kszAccountProxyFallbackInterval, nId,
               refData.m_nProxyFallbackInterval, CONFIG_LEVEL_PHONE) || bChanged;

    // Get account status.
    bChanged = configParser_SetCfgItemIntValue(kszAccountEnable, nId, refData.m_isActivated ? 1 : 0,
               CONFIG_LEVEL_PHONE) || bChanged;
    // Set account outbound status.
    bChanged = configParser_SetCfgItemIntValue(kszAccountOutboundSwitch, nId,
               refData.m_isOutboundEnable ? 1 : 0, CONFIG_LEVEL_PHONE) || bChanged;
    // Set account auto answer.
    //bChanged = configParser_SetCfgItemIntValue(kszAutoAnswerSwitch, nId, refData.m_bIsAutoAnswer ? 1 : 0, CONFIG_LEVEL_PHONE) || bChanged;
    bChanged = configParser_SetCfgItemIntValue(kszAccountStunSwitch, nId, refData.m_isNATEnable ? 1 : 0,
               CONFIG_LEVEL_PHONE) || bChanged;

    if (bChanged)
    {
        //Notify Config Change
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, nId);
        //异步通知账号配置改变
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, nId);
    }
#if IF_BUG_26237
    else if (bLabelChanged)
    {
        //异步通知账号配置改变
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_DEFAULT_ACCOUNT, nId);
    }
#endif

    if (bXSIConfigChanged)
    {
        //通知xsi信息改变
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_XSI_CONFIGE, nId);
    }
}

CAccountController::CAccountController()
{

}

CAccountController::~CAccountController()
{

}

}  // namespace NS_SETTING
