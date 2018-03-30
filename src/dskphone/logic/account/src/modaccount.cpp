#include "account_inc.h"

#include "baseaccount.h"
#include "sipaccount.h"
#include "pstnaccount.h"
#include "modaccount.h"
#include "accountmanager.h"
#ifdef IF_FEATURE_GETBALANCE
#include "getbalance.h"
#endif
#include "cloudaccountinfomanager.h"

#include "service_h323_message.h"

// 初始化
void acc_Init()
{
    _AccountManager.InitializeObject();
#ifdef IF_FEATURE_CLOUD
    _CloudAccountInfoManager.Init();
#endif //#ifdef IF_FEATURE_CLOUD
}

// 账号数
int acc_AccountNum(int eAccountType /* = ACCOUNT_TYPE_SIP */)
{
#if DEBUG_ACCOUNT
    ACCOUNT_INFO("acc_AccountNum(%d)", eAccountType);
#endif //DEBUG_ACCOUNT

    static int s_nSipAccountNum = -1;
    if (s_nSipAccountNum == -1)
    {
        // 计算SIP账号个数
        s_nSipAccountNum = configParser_GetConfigInt(kszAccountNum);
        if (s_nSipAccountNum > 16 || s_nSipAccountNum < 0)
        {
            ACCOUNT_ERR("Error sip account number in config");
            s_nSipAccountNum = 1;
        }
    }

#if DEBUG_ACCOUNT
    ACCOUNT_INFO("SipAccountNum=%d", s_nSipAccountNum);
#endif //DEBUG_ACCOUNT
#ifdef IF_FEATURE_PSTN
    // PSTN账号个数
    int iPstnAccountNum = pstnadapter_GetLineCount();
#endif // IF_FEATURE_PSTN
    // 计算指定账号个数
    int iResult = 0;
    if (eAccountType & ACCOUNT_TYPE_SIP)
    {
        iResult += s_nSipAccountNum;
    }

    if (eAccountType & ACCOUNT_TYPE_IP_DIRECT)
    {
        iResult += 1;
    }

#ifdef IF_FEATURE_PSTN
    if (eAccountType & ACCOUNT_TYPE_PSTN)
    {
        iResult += iPstnAccountNum;
    }
#endif // IF_FEATURE_PSTN

#if DEBUG_ACCOUNT
    ACCOUNT_INFO("acc_AccountNum(%d)=%d", eAccountType, iResult);
#endif //DEBUG_ACCOUNT
    return iResult;
}

// 获取账号状态
LINE_STATE acc_GetState(int iAccountID)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccountID, false);
    return pAccount ? pAccount->GetAccountState() : LS_UNKNOW;
}

// 获取已注册的账号数目
int acc_GetRegisteredAccountNum(int eAccountType /* = ACCOUNT_TYPE_SIP */)
{
    return _AccountManager.GetRegisteredAccountNum(eAccountType);
}

ListAccounts acc_GetRegisteFailAccountList(int eAccountType /* = ACCOUNT_TYPE_SIP */)
{
    return _AccountManager.GetRegisteFailAccountList(eAccountType);
}

/*******************************************************************
** 函数描述:   获取当前账号
** 参数：      无
** 返回:       当前账号ID
********************************************************************/
int acc_GetDefaultAccount()
{
    return _AccountManager.GetDefaultAccountID();
}

/*******************************************************************
** 函数描述:   设置当前账号
** 参数：      iAccount  账号ID
               bSave 是否保存至配置文件
** 返回:       是否默认账号设置成功
********************************************************************/
bool acc_SetDefaultAccount(int iAccount, bool bSave/* = false*/)
{
    if (acc_IsAccountAvailable(iAccount))
    {
        if (bSave)
        {
            return _AccountManager.SaveDefaultAccount(iAccount);
        }
        else
        {
            return _AccountManager.SetDefaultAccount(iAccount);
        }
    }
    return false;
}

/*******************************************************************
** 函数描述:   切换当前账号
** 参数：      无
** 返回:       当前账号ID
********************************************************************/
int acc_SwitchDefaultAccount(bool bForward /* = true*/)
{
    return _AccountManager.SwitchDefaultAccount(bForward);
}

bool acc_ExistAvailableAccount(int eAccountType /* = ACCOUNT_TYPE_SIP */)
{
    return _AccountManager.GetUsableAccountCount(false, eAccountType) > 0;
}

int acc_GetUsableAccount(bool bFirst, int eAccountType /* = ACCOUNT_TYPE_SIP */)
{
    CBaseAccount * pAccount = _AccountManager.GetUsableAccount(bFirst, eAccountType);
    return pAccount ? pAccount->GetAccountId() : INVALID_ACCOUNTID;
}

int acc_GetUsableAccount(const yl::string& strServer, const yl::string& strName/* = ""*/)
{
    CBaseAccount * pAccount = _AccountManager.GetUsableAccount(strServer, strName);
    return pAccount ? pAccount->GetAccountId() : INVALID_ACCOUNTID;
}

#ifdef IF_FEATURE_PSTN
int acc_GetUsableAccountLike(int iFirstChoiceAccountId)
{
    CBaseAccount * pAccount = _AccountManager.GetUsableAccountLike(iFirstChoiceAccountId);
    return pAccount ? pAccount->GetAccountId() : INVALID_ACCOUNTID;
}
#endif // IF_FEATURE_PSTN

// 获取获取指定账号的PickupCode，如果账号的配置为空，会取公共的配置
yl::string acc_GetPickupCodeByType(int iAccountId, PICKUP_TYPE eType)
{
    yl::string strCode;
    if (eType == PT_DIRECTED)
    {
        strCode = configParser_GetCfgItemStringValue(kszAccountDPickupCode, iAccountId);
    }
    else if (eType == PT_GROUP)
    {
        strCode = configParser_GetCfgItemStringValue(kszAccountGPickupCode, iAccountId);
    }

    //账号没有设置PickupCode使用全局设置
    if (strCode.empty())
    {
        if (eType == PT_DIRECTED)
        {
            return configParser_GetConfigString(kszDPickupCode);
        }
        else if (eType == PT_GROUP)
        {
            return configParser_GetConfigString(kszGPickupCode);
        }
    }
    return strCode;
}

int acc_GetFirstUserNameAccount()
{
    CBaseAccount * pAccount = _AccountManager.GetFirstUserNameAccount();
    return pAccount ? pAccount->GetAccountId() : INVALID_ACCOUNTID;
}

// 通过账号名获取账号ID
int acc_GetAccountIDByName(const char * pName, const char * pServer/* = NULL*/)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountBySIPName(pName, pServer);
    return pAccount ? pAccount->GetAccountId() : INVALID_ACCOUNTID;
}

/**
 * @brief 判断账号是否启用
 * @param iAccount 账号ID
 * @return 账号是否启用
 */
bool acc_IsAccountEnable(int iAccount)
{
#ifdef IF_FEATURE_PSTN
    if (iAccount >= PSTN_LINE_BEGIN_ID && iAccount <= PSTN_LINE_END_ID)
    {
        return configParser_GetCfgItemIntValue(kszPSTNAccountEnable, iAccount - PSTN_LINE_BEGIN_ID) != 0;
    }
    else
#endif // IF_FEATURE_PSTN
#ifdef IF_FEATURE_H323
        if (H323ACCOUNTID == iAccount || H323ACCOUNTID_IP == iAccount)
        {
            CH323Account * pAccount =  dynamic_cast<CH323Account *>(_AccountManager.GetAccountByAccountId(
                                           iAccount, false));
            return pAccount ? pAccount->IsAccountEnable() : false;
        }
        else
#endif //IF_FEATURE_H323
        {
            return configParser_GetCfgItemIntValue(kszAccountEnable, iAccount) != 0;
        }
    return false;
}
/**
 * @brief 设置账号是否启用
 * @param iAccount 账号ID
 * @param bEnable 账号是否启用
 * @param bUserChange 是否用户修改
 * @return 操作是否成功
 */
bool acc_SetAccountEnable(int iAccount, bool bEnable,
                          CONFIG_LEVEL_TYPE eType /* = CONFIG_LEVEL_IGNORE */)
{
    int iEnable = bEnable ? 1 : 0;
#ifdef IF_FEATURE_PSTN
    if (iAccount >= PSTN_LINE_BEGIN_ID && iAccount <= PSTN_LINE_END_ID)
    {
        return configParser_SetCfgItemIntValue(kszPSTNAccountEnable, iAccount - PSTN_LINE_BEGIN_ID, iEnable,
                                               eType);
    }
    else
#endif // IF_FEATURE_PSTN
    {
        return configParser_SetCfgItemIntValue(kszAccountEnable, iAccount, iEnable, eType);
    }
    return false;
}

/**
 * @brief 通过账号ID获取账号Label配置
 * @param iAccountID 账号ID
 * @return 账号Label配置
 */
yl::string acc_GetAccountLabel(int iAccountID)
{
#ifdef IF_FEATURE_PSTN
    if (iAccountID >= PSTN_LINE_BEGIN_ID && iAccountID <= PSTN_LINE_END_ID)
    {
        return configParser_GetCfgItemStringValue(kszPSTNAccountLabel, iAccountID - PSTN_LINE_BEGIN_ID);
    }
    else
#endif // IF_FEATURE_PSTN
#ifdef IF_FEATURE_H323
        if (iAccountID >= H323ACCOUNTID && iAccountID <= H323ACCOUNTID_IP)
        {
            if (iAccountID == H323ACCOUNTID_IP)
            {
                // H323 ip直播是虚拟的账号；
                return "";
            }

            return configParser_GetConfigString(kszAccountH323DisplayName);
        }
        else
#endif //IF_FEATURE_H323
        {
            return configParser_GetCfgItemStringValue(kszAccountLabel, iAccountID);
        }

    return yl::string();
}
/**
 * @brief 设置账号Label
 * @param iAccount 账号ID
 * @param strLabel 账号Label
 * @param bUserChange 是否用户修改
 * @return 操作是否成功
 */
bool acc_SetAccountLabel(int iAccountID, const yl::string& strLabel,
                         CONFIG_LEVEL_TYPE eType /* = CONFIG_LEVEL_IGNORE */)
{
#ifdef IF_FEATURE_PSTN
    if (iAccountID >= PSTN_LINE_BEGIN_ID && iAccountID <= PSTN_LINE_END_ID)
    {
        return configParser_SetCfgItemStringValue(kszPSTNAccountLabel, iAccountID - PSTN_LINE_BEGIN_ID,
                strLabel, eType);
    }
    else
#endif // IF_FEATURE_PSTN
#ifdef IF_FEATURE_H323
        if (iAccountID >= H323ACCOUNTID && iAccountID <= H323ACCOUNTID_IP)
        {
            if (iAccountID == H323ACCOUNTID_IP)
            {
                // H323 ip直播是虚拟的账号；
                return true;
            }

            return configParser_SetConfigString(kszAccountH323DisplayName, strLabel.c_str(), eType);
        }
        else
#endif //IF_FEATURE_H323
        {
            return configParser_SetCfgItemStringValue(kszAccountLabel, iAccountID, strLabel, eType);
        }
    return false;
}

bool acc_IsAccountSaveMissCallLog(int iAccountID)
{
#ifdef IF_FEATURE_PSTN
    if (iAccountID >= PSTN_LINE_BEGIN_ID && iAccountID <= PSTN_LINE_END_ID)
    {
        return configParser_GetCfgItemIntValue(kszPSTNAccountMissCallLog, iAccountID - PSTN_LINE_BEGIN_ID) != 0;
    }
    else
#endif // IF_FEATURE_PSTN
    {
        return configParser_GetCfgItemIntValue(kszAccountMissCallLog, iAccountID) != 0;
    }
    return false;
}

/**
 * @brief 判断账号是否开启自动应答
 * @param iAccountID 账号ID
 * @return 账号是否开启自动应答
 */
bool acc_IsAccountAutoAnswerEnable(int iAccountID /* = INVALID_ACCOUNTID */)
{
    if (INVALID_ACCOUNTID == iAccountID)
    {
        // 入参为INVALID_ACCOUNTID时，查询所有账号，有任意账号启用则返回true。
        YLList<int> listAccounts;
        listAccounts = acc_GetAvailableAccountList();
        bool bEnable = false;
        for (YLList<int>::iterator it = listAccounts.begin(); it != listAccounts.end(); ++it)
        {
            CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(*it, false);
            if (NULL == pAccount || INVALID_ACCOUNTID == pAccount->GetAccountId())
            {
                continue;
            }
            //只要有一个账号开启就为true
            bEnable |= pAccount->GetAutoAnswerSwitch();
        }
        return bEnable;
    }
    else
    {
        CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccountID, false);

        if (NULL != pAccount)
        {
            return pAccount->GetAutoAnswerSwitch();
        }
    }

    return false;
}
/**
 * @brief 设置账号是否开启自动应答
 * @param iAccount 账号ID
 * @param bAutoAnswer 账号是否开启自动应答
 * @param bUserChange 是否用户修改
 * @return 操作是否成功
 */
bool acc_SetAccountAutoAnswer(int iAccountID, bool bAutoAnswer,
                              CONFIG_LEVEL_TYPE eType /* = CONFIG_LEVEL_IGNORE */)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccountID);

    if (NULL == pAccount)
    {
        return false;
    }

    return pAccount->SetAutoAnswerSwitch(bAutoAnswer, eType);
}

/**
 * @brief 判断账号是否开启自动应答静音
 * @param iAccountID 账号ID
 * @return 账号是否开启自动应答静音
 */
bool acc_IsAccountAutoAnswerMute(int iAccountID)
{
#ifdef IF_FEATURE_PSTN
    if (iAccountID >= PSTN_LINE_BEGIN_ID && iAccountID <= PSTN_LINE_END_ID)
    {
        return configParser_GetCfgItemIntValue(kszPSTNAccountAutoAnswerMute,
                                               iAccountID - PSTN_LINE_BEGIN_ID) != 0;
    }
    else
#endif // IF_FEATURE_PSTN
#ifdef IF_FEATURE_H323
        if (iAccountID == H323ACCOUNTID_IP)
        {
            // H323 ip直播是虚拟的账号；
            return false;
        }
        else if (iAccountID == H323ACCOUNTID)
        {
            return false;
        }
        else
#endif //IF_FEATURE_H323
        {
            return configParser_GetCfgItemIntValue(kszAccountAutoAnswerMuteSwitch, iAccountID) != 0;
        }
    return false;
}
/**
 * @brief 设置账号是否开启自动应答静音
 * @param iAccount 账号ID
 * @param bAutoAnswerMute 账号是否开启自动应答静音
 * @param bUserChange 是否用户修改
 * @return 操作是否成功
 */
bool acc_SetAccountAutoAnswerMute(int iAccountID, bool bAutoAnswerMute,
                                  CONFIG_LEVEL_TYPE eType /* = CONFIG_LEVEL_IGNORE */)
{
    int iAutoAnswerMute = bAutoAnswerMute ? 1 : 0;
#ifdef IF_FEATURE_PSTN
    if (iAccountID >= PSTN_LINE_BEGIN_ID && iAccountID <= PSTN_LINE_END_ID)
    {
        return configParser_SetCfgItemIntValue(kszPSTNAccountAutoAnswerMute,
                                               iAccountID - PSTN_LINE_BEGIN_ID, iAutoAnswerMute, eType);
    }
    else
#endif // IF_FEATURE_PSTN
#ifdef IF_FEATURE_H323
        if (iAccountID == H323ACCOUNTID_IP)
        {
            // H323 ip直播是虚拟的账号；
            return false;
        }
        else if (iAccountID == H323ACCOUNTID)
        {
            return false;
        }
        else
#endif //IF_FEATURE_H323
        {
            return configParser_SetCfgItemIntValue(kszAccountAutoAnswerMuteSwitch, iAccountID, iAutoAnswerMute,
                                                   eType);
        }
    return false;
}

/**
 * @brief 通过账号ID获取账号铃声配置
 * @param iAccountID 账号ID
 * @return 账号铃声配置
 */
yl::string acc_GetAccountRing(int iAccountID)
{
#ifdef IF_FEATURE_PSTN
    if (iAccountID >= PSTN_LINE_BEGIN_ID && iAccountID <= PSTN_LINE_END_ID)
    {
        return configParser_GetCfgItemStringValue(kszPSTNAccountRing, iAccountID - PSTN_LINE_BEGIN_ID);
    }
    else
#endif // IF_FEATURE_PSTN
#ifdef IF_FEATURE_H323
        if (iAccountID == H323ACCOUNTID_IP)
        {
            // H323 ip直播是虚拟的账号；
            return yl::string();
        }
        else if (iAccountID == H323ACCOUNTID)
        {
            return yl::string();
        }
        else
#endif //IF_FEATURE_H323
        {
            return configParser_GetCfgItemStringValue(kszAccountRing, iAccountID);
        }
    return yl::string();
}
/**
 * @brief 设置账号铃声配置
 * @param iAccount 账号ID
 * @param strRing 账号铃声配置
 * @param bUserChange 是否用户修改
 * @return 操作是否成功
 */
bool acc_SetAccountRing(int iAccountID, const yl::string& strRing,
                        CONFIG_LEVEL_TYPE eType /* = CONFIG_LEVEL_IGNORE */)
{
#ifdef IF_FEATURE_PSTN
    if (iAccountID >= PSTN_LINE_BEGIN_ID && iAccountID <= PSTN_LINE_END_ID)
    {
        return configParser_SetCfgItemStringValue(kszPSTNAccountRing, iAccountID - PSTN_LINE_BEGIN_ID,
                strRing, eType);
    }
    else
#endif // IF_FEATURE_PSTN
#ifdef IF_FEATURE_H323
        if (iAccountID == H323ACCOUNTID_IP)
        {
            // H323 ip直播是虚拟的账号；
            return false;
        }
        else if (iAccountID == H323ACCOUNTID)
        {
            return false;
        }
        else
#endif //IF_FEATURE_H323
        {
            return configParser_SetCfgItemStringValue(kszAccountRing, iAccountID, strRing, eType);
        }
    return false;
}


/*******************************************************************
** 函数描述:        获取账号显示名
** 参数：            szConfigName-配置名 iAccountID-账号ID iConfig-配置值
** 返回:          是否修改成功
********************************************************************/
yl::string acc_GetAccountShowText(int iAccountID, bool bUseDisplayNameFirst/* = false*/)
{
    yl::string strDisplayname;

    do
    {
        CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccountID);
        if (pAccount)
        {
            strDisplayname = pAccount->GetCustomShowText();
            if (!strDisplayname.empty())
            {
                break;
            }
        }
        if (bUseDisplayNameFirst)
        {
            strDisplayname = configParser_GetCfgItemStringValue(kszAccountDisplayName, iAccountID);
            if (!strDisplayname.empty())
            {
                break;
            }

            strDisplayname = acc_GetAccountLabel(iAccountID);
            if (!strDisplayname.empty())
            {
                break;
            }
        }
        else
        {
            strDisplayname = acc_GetAccountLabel(iAccountID);
            if (!strDisplayname.empty())
            {
                break;
            }

            strDisplayname = acc_GetDisplayName(iAccountID);
            if (!strDisplayname.empty())
            {
                break;
            }
        }

        strDisplayname = acc_GetRegisterName(iAccountID);
        if (!strDisplayname.empty())
        {
            break;
        }

        strDisplayname = acc_GetUsername(iAccountID);
        if (!strDisplayname.empty())
        {
            break;
        }
    }
    while (false);

    return strDisplayname;
}

#ifndef _T49
// 获取未读语音留言数量
int acc_GetUnReadVoiceMailNum(int iAccountID /*= -1*/)
{
    return _AccountManager.GetVoiceMailNum(false, iAccountID);
}

// 获取已读语音留言数量
int acc_GetReadedVoiceMailNum(int iAccountID /*= -1*/)
{
    return _AccountManager.GetVoiceMailNum(true, iAccountID);
}

int acc_GetLastVMailAccountId()
{
    return _AccountManager.GetLastVMailAccountId();
}
#endif

int acc_GetRegisterFailId(int iAccount)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccount, false);
    return pAccount ? pAccount->GetRegisterFailId() : 0;
}

yl::string acc_GetRegisterFailInfo(int iAccount)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccount, false);
    return pAccount ? pAccount->GetRegisterFailInfo() : yl::string();
}

bool acc_IsDirectIPAccount(int iAccount, bool bCheck /* = true*/)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccount, bCheck);
    return pAccount ? pAccount->IsDirectIPAccount() : false;
}

#ifdef IF_FEATURE_PSTN
bool acc_IsPstnAccount(int iAccount)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccount);
    return pAccount ? (pAccount->GetAccountType() == ACCOUNT_TYPE_PSTN) : false;
}
#endif // IF_FEATURE_PSTN

yl::string acc_GetServerName(int iAccount)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccount, false);
    return (pAccount != NULL) ? pAccount->GetServerName() : yl::string();
}

int acc_GetServerPort(int iAccount)
{
    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(iAccount,
                             false));
    return pAccount ? pAccount->GetServerPort() : 5060;
}

SIP_SERVER_TYPE acc_GetServerType(int iAccount)
{
    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(iAccount,
                             false));
    return pAccount ? pAccount->GetServerType() : SST_DEFAULT;
}

bool acc_IsGenesysAccount(int iAccountID)
{
    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(
                                 iAccountID));
    if (pAccount == NULL)
    {
        return SST_DEFAULT;
    }
    return (SST_GENESYS_ADV == pAccount->GetServerType() || SST_GENESYS == pAccount->GetServerType());
}

int acc_GetServerTransport(int iAccount)
{
    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(iAccount,
                             false));
    if (pAccount == NULL)
    {
        return -1;
    }
    return configParser_GetCfgItemIntValue(kszSipserverTransport, pAccount->GetAccountId(),
                                           pAccount->GetServerID());
}

int acc_GetAccountIDByServer(const yl::string & strServer)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByServerName(strServer);
    return pAccount ? pAccount->GetAccountId() : INVALID_ACCOUNTID;
}

int acc_GetAccountIDByFullName(const char * pFullName)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByFullName(pFullName);
    return pAccount ? pAccount->GetAccountId() : INVALID_ACCOUNTID;
}

#ifdef IF_FEATURE_PSTN
int acc_GetAccountIDByPstnName(const char * szPstnName)
{
    CPstnAccount * pAccount = _AccountManager.GetAccountByPstnName(szPstnName);
    return pAccount ? pAccount->GetAccountId() : INVALID_ACCOUNTID;
}

int acc_GetAccountIDByDeviceID(int iDeviceId)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByDeviceId(iDeviceId);
    return pAccount ? pAccount->GetAccountId() : INVALID_ACCOUNTID;
}

int acc_GetDeviceIDByAccountID(int iAccountId)
{
    CPstnAccount * pAccount = dynamic_cast<CPstnAccount *>(_AccountManager.GetAccountByAccountId(
                                  iAccountId));
    return pAccount ? pAccount->GetDeviceId() : -1;
}
#endif // IF_FEATURE_PSTN

int acc_GetAccountIDByUserInfo(BasicUserInfo * pUser)
{
    return acc_GetAccountIDByName(pUser->strNumber.c_str(), pUser->strServer.c_str());
}

int acc_GetNoEmptyServerAccountID()
{
    CBaseAccount * pAccount = _AccountManager.GetNoEmptyServerAccount();
    return pAccount ? pAccount->GetAccountId() : INVALID_ACCOUNTID;
}

bool acc_IsAccountAvailable(int iAccount)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccount);
    return pAccount ? pAccount->IsAccountUsable() : false;
}

int acc_GetAcdAccountID()
{
#ifdef IF_FEATURE_ACD
    return _AccountManager.GetACDAccountId();
#else
    return INVALID_ACCOUNTID;
#endif
}

int acc_GetHetlingAccountID()
{
#ifdef IF_FEATURE_ACD
    return _AccountManager.GetHotelingAccountId();
#else
    return INVALID_ACCOUNTID;
#endif
}

int acc_GetDirectIPAccountID(bool bCheck/* = true*/)
{
    CBaseAccount * pAccount = _AccountManager.GetDirectIPAccount(bCheck);
    return pAccount ? pAccount->GetAccountId() : INVALID_ACCOUNTID;
}

ListAccounts acc_GetValidAccountList(int eAccountType /* = ACCOUNT_TYPE_ALL */)
{
    CAccountList listAccounts;
    _AccountManager.GetValidAccountList(listAccounts);
    ListAccounts listValidAccouts;
    for (CAccountList::iterator it = listAccounts.begin(); it != listAccounts.end(); ++it)
    {
        CBaseAccount * pAccount = *it;
        if (pAccount && (pAccount->GetAccountType() & eAccountType))
        {
            listValidAccouts.push_back((*it)->GetAccountId());
        }
    }
    return listValidAccouts;
}

ListAccounts acc_GetAvailableAccountList(int eAccountType /* = ACCOUNT_TYPE_ALL */)
{
    CAccountList listAccounts;
    _AccountManager.GetAvailableAccountList(listAccounts);
    ListAccounts listAvailableAccouts;
    for (CAccountList::iterator it = listAccounts.begin(); it != listAccounts.end(); ++it)
    {
        CBaseAccount * pAccount = *it;
        if (pAccount && (pAccount->GetAccountType() & eAccountType))
        {
            listAvailableAccouts.push_back(pAccount->GetAccountId());
        }
    }
    return listAvailableAccouts;
}

ListAccounts acc_GetAccountList(int eAccountType /* = ACCOUNT_TYPE_ALL */,
                                bool bOnlyUser/* = false*/)
{
    CAccountList listAccounts;
    _AccountManager.GetAccountList(listAccounts, bOnlyUser);
    ListAccounts listAccountId;
    for (CAccountList::iterator it = listAccounts.begin(); it != listAccounts.end(); ++it)
    {
        CBaseAccount * pAccount = *it;
        if (pAccount && (pAccount->GetAccountType() & eAccountType))
        {
            listAccountId.push_back(pAccount->GetAccountId());
        }
    }
    return listAccountId;
}

CALL_AV_TYPE acc_GetDefaultCallMode(int iAccount)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccount);
    return pAccount ? pAccount->GetDefaultCallMode() : CAVT_UNKNOWN;
}

void acc_EraseAllAccount(int eAccountType /* = ACCOUNT_TYPE_SIP */, int iType/* = -1*/)
{
    _AccountManager.EraseAllAccount(eAccountType, iType);
}

void acc_EmptyAllAccount(int eAccountType /* = ACCOUNT_TYPE_SIP */, int iType/* = -1*/)
{
    _AccountManager.EmptyAllAccount(eAccountType, iType);
}

void acc_EraseAccountById(int iAccount)
{
    _AccountManager.EraseAccountById(iAccount);
}

void acc_EmptyAccountById(int iAccount)
{
    _AccountManager.EmptyAccountById(iAccount);
}

void acc_LoadAccount(int iAccount)
{
    _AccountManager.LoadLineAccountById(iAccount);
}

bool acc_LogoutBeforeReset()
{
    return true;
    // http://bugfree.yealink.com/Bug.php?BugID=97260 去除
    //return _AccountManager.LogoutBeforeReset() != 0;
}

bool acc_BeWaitLogoutBeforeReset()
{
    return _AccountManager.BeWaitLogoutBeforeReset() != 0;
}

bool acc_IsAccountIDValid(int iAccount)
{
    // 不允许sip ip直播，会返回空指针
    return _AccountManager.GetAccountByAccountId(iAccount) != NULL;
}

yl::string acc_GetRegisterName(int iAccount)
{
    yl::string strRegisterNmane = "";
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccount);
    if (pAccount)
    {
        strRegisterNmane = pAccount->GetRegisterName();
    }

    return strRegisterNmane;
}

yl::string acc_GetLabel(int iAccount)
{
    if (iAccount >= SIP_LINE_BEGIN_ID && iAccount <= SIP_LINE_END_ID)
    {
        return acc_GetAccountLabel(iAccount);
    }
    else if (iAccount == H323ACCOUNTID_IP)
    {
        // H323 ip直播是虚拟的账号；
        return "";
    }
    else if (iAccount == H323ACCOUNTID)
    {
        return configParser_GetConfigString(kszAccountH323DisplayName);
    }

#ifdef IF_FEATURE_PSTN
    else if (iAccount >= PSTN_LINE_BEGIN_ID && iAccount <= PSTN_LINE_END_ID)
    {
        yl::string strLabel = acc_GetAccountLabel(iAccount);
        yl::string strResult(strLabel);
        if (!strLabel.empty())
        {
            strResult.append("(");
        }
        char szAccountKey[32] = { 0 };
        snprintf(szAccountKey, sizeof(szAccountKey), "PSTN%d", iAccount - PSTN_LINE_BEGIN_ID + 1);
        strResult.append(szAccountKey);
        if (!strLabel.empty())
        {
            strResult.append(")");
        }
        return strResult;
    }
#endif // IF_FEATURE_PSTN
    return yl::string();
}

yl::string acc_GetDisplayName(int iAccount)
{
    yl::string strDisplayName = "";
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccount);
    if (pAccount)
    {
        strDisplayName = pAccount->GetDisplayName();
    }

    return strDisplayName;
}

yl::string acc_GetUsername(int iAccount)
{
    yl::string strUserName = "";
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccount);
    if (pAccount)
    {
        strUserName = pAccount->GetUserName();
    }

    return strUserName;
}

yl::string acc_GetPassward(int iAccount)
{
    return configParser_GetCfgItemStringValue(kszAccountPassword, iAccount);
}

BLA_TYPE acc_GetBLAType(int iAccount)
{
#if IF_BUG_39041
    BLA_TYPE eBLAType = BLA_NONE;
    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(iAccount));
    if (pAccount)
    {
        eBLAType = pAccount->GetBLAType();
    }
#else
    BLA_TYPE eBLAType = (BLA_TYPE)configParser_GetCfgItemIntValue(kszBLAType, iAccount);
    if (eBLAType == BLA_TELTRONNIC_BLA)
    {
        eBLAType = BLA_DRAFT_BLA;
    }
    else if (eBLAType == BLA_BROADSOFT_SCA
             && acc_GetServerType(iAccount) == SST_GENBAND)
    {
        return BLA_GENBAND;
    }
#endif
    return eBLAType;
}

void acc_SetSyncSubscribe(int iAccount, bool bState)
{
    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(
                                 iAccount));
    if (pAccount)
    {
        pAccount->SetSyncSubscribeResult(bState);
    }
}

bool acc_GetSyncSubscribe(int iAccount)
{
    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(
                                 iAccount));
    return pAccount ? pAccount->GetSyncSubscribeResult() : false;
}

yl::string acc_GetFullName(int iAccount)
{
    if (!acc_IsAccountIDValid(iAccount))
    {
        return yl::string();
    }
    yl::string strFullName = acc_GetUsername(iAccount);
    strFullName += "@";
    strFullName += acc_GetServerName(iAccount);
    return strFullName;
}

void acc_GetAccountInfo(yl::string & strInfoOut, int eAccountType /* = ACCOUNT_TYPE_SIP */)
{
    _AccountManager.GetAccountInfo(strInfoOut, eAccountType);
}

bool acc_SetAccountInfo(yl::string & strInfoIn)
{
    return _AccountManager.SetAccountInfo(strInfoIn);
}

bool acc_IsAllAccountDisable()
{
    return _AccountManager.IsAllAccountDisable();
}

void acc_RefreshLineState(int iAccount)
{
    _AccountManager.RefreshLineStateById(iAccount);
}

void acc_BackupRestoreCfg(int iAccountId, bool bIsRestore)
{
    _AccountManager.BackupRestoreCfg(iAccountId, bIsRestore);
}


LRESULT OnAccountManagerMsg(msgObject & objMessage);

LRESULT acc_AccountOnMessage(msgObject & msg)
{
    return OnAccountManagerMsg(msg);
}

// 映射sip账号状态到本地的账号
LINE_STATE acc_MapState(int iState)
{
    LINE_STATE eState = LS_UNKNOW;
    switch (iState)
    {
    case SIP_REG_UNKNOW:
        eState = LS_UNKNOW;
        break;
    case SIP_REG_DISABLE:
        eState = LS_DISABLE;
        break;
    case SIP_REG_REGING:
        eState = LS_REGISTERING;
        break;
    case SIP_REG_REGED:
        eState = LS_REGISTERED;
        break;
    case SIP_REG_REGFAIL:
        eState = LS_REGISTER_FAIL;
        break;
    case SIP_REG_UNREGING:
        eState = LS_UNREGISTERING;
        break;
    case SIP_REG_UNREGEND:
        eState = LS_BE_UNREGISTERED;
        break;
    case SIP_REG_REGISTERED_ON_BOOT:
        eState = LS_INEXISTENCE;
        break;
    case SIP_REG_DISABLE_REGISTER:
        eState = LS_DISABLE_REG;
        break;
    default:
        break;
    }

    return eState;
}

bool acc_SetCustomShowText(int iAccount, const yl::string & strText)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccount);
    return pAccount ? pAccount->SetCustomShowText(strText) : false;
}

yl::string acc_GetCustomShowText(int iAccount)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccount);
    return  pAccount ? pAccount->GetCustomShowText() : yl::string();
}

#ifdef IF_FEATURE_GETBALANCE
// 下载当前通话的账号余额信息
void acc_LoadBalance()
{
    int iAccount = _AccBalanceManager.GetBalanceID();
    if (iAccount >= 0)
    {
        // 下载账号余额 提示下载情况,为防止多显示此处只提示可以成功下载的情况
        if (_AccBalanceManager.LoadBalance(iAccount))
        {
            _AccBalanceManager.ShowLoadingBalance();
        }

        _AccBalanceManager.SetBalanceID(-1);
    }
    else
    {
        // 正在加载，且提示框已经被退出，且退出的方式是外部的(非用户取消)
        if (_AccBalanceManager.IsLoadingBalance()
                && !_AccBalanceManager.IsCancelGetBalance())
        {
            // 窗口是因外部原因退出的，或
            ACCOUNT_WARN("Popup Load Balance Box Again !");
            _AccBalanceManager.ShowLoadingBalance();
        }
    }
}

//设置在结束通话后需要获取账号余额信息的账号
void acc_SetNeedGetBalanceAccount(int iAccount, bool bRealLoad /*= false*/)
{
    _AccBalanceManager.SetBalanceID(iAccount);

    if (bRealLoad)
    {
        acc_LoadBalance();
    }
}

void acc_GetBalanceInfo(Balance_Info_t & tInfo)
{
    _AccBalanceManager.GetBalanceInfo(tInfo);
}

//取消此次账号余额下载
void acc_CancelLoadBalance()
{
    _AccBalanceManager.CancelLoadBalance();
}

bool acc_IsLoadingBalance()
{
    return _AccBalanceManager.IsLoadingBalance();
}
#endif

yl::string acc_GetBLANum(int iAccount)
{
    return configParser_GetCfgItemStringValue(kszBLANumber, iAccount);
}

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
bool acc_GetHotelingSwitch(int iAccount)
{
    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(
                                 iAccount));
    return pAccount ? pAccount->GetHotelingSwitch() : false;
}

bool acc_GetFlexibleSeatingSwitch(int iAccount)
{
    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(
                                 iAccount));
    return pAccount ? pAccount->GetFlexibleSeatingSwitch() : false;
}
#endif

bool acc_IsNoNeedRegisterAccount(const int iAccount)
{
    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(
                                 iAccount));
    return pAccount ? pAccount->IsNoNeedRegisterStatus() : false;
}

bool acc_IsInFinalRegisterState(const int iAccount)
{
    LINE_STATE eStats = acc_GetState(iAccount);
    return eStats == LS_REGISTERED || eStats == LS_REGISTER_FAIL || eStats == LS_DISABLE_REG;
}

bool acc_IsUseableStatus(const LINE_STATE eStats)
{
    return eStats == LS_REGISTERED || eStats == LS_DISABLE_REG || eStats == LS_SYSTEM_DEFAULT_ACCOUNT;
}

bool acc_IsDisableStatus(int iAccount)
{
    LINE_STATE eStats = acc_GetState(iAccount);
    return eStats <= LS_DISABLE;
}

#ifdef _DECT
bool acc_IsTLSEncrypt(int iAccount)
{
    return acc_GetServerTransport(iAccount) == 2;
}

bool acc_GetDisplayNameByFullName(const BasicUserInfo & basicInfo, yl::string & strDisplayName)
{
    int iAccountID = acc_GetAccountIDByName(basicInfo.strNumber.c_str(), basicInfo.strServer.c_str());
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccountID);

    if (pAccount)
    {
        strDisplayName = acc_GetLabel(iAccountID);
        if (strDisplayName.empty())
        {
            strDisplayName = pAccount->GetDisplayName();
        }

        return true;
    }

    return false;
}
#endif

#ifdef _T49
yl::string acc_GetVideoParam(int iAccountID, int iConfigID, const yl::string & strConfigID,
                             const yl::string & strParamName)
{
    if (iAccountID < 0 || iAccountID >= acc_AccountNum())
    {
        return "";
    }

    /*
    profile-level-id=428028;packetization-mode=1;...
    */

    YLList<yl::string> listParam;
    commonAPI_splitStringBySeparator(configParser_GetCfgItemStringValue(strConfigID.c_str(), iAccountID,
                                     iConfigID), ';', listParam);
    if (listParam.size() <= 0)
    {
        return "";
    }

    yl::string strParam;
    bool bFound = false;
    for (YLList<yl::string>::iterator itBeg = listParam.begin();
            itBeg != listParam.end();
            ++ itBeg)
    {
        yl::string & strItem = (*itBeg);
        strItem.trim_both(" ");

        if (strItem.length() >= strParamName.length()
                && strItem.substr(0, strParamName.length()) == strParamName)
        {
            bFound = true;
            strParam = strItem;
            break;
        }
    }

    if (!bFound)
    {
        return strParam;
    }

    listParam.clear();
    commonAPI_splitStringBySeparator(strParam, '=', listParam);
    if (listParam.size() == 2)
    {
        return listParam.back();
    }

    return "";
}
#endif

#ifdef _DECT
bool acc_GetAccountInfo(YLList<AccountInfo> & listAccount)
{
    AccountInfo accountInfo;
    int iAccountNum = acc_AccountNum();
    for (int i = 0; i < iAccountNum; i++)
    {
        accountInfo.iAccountID = i;
        accountInfo.strUserName = acc_GetUsername(i);
        accountInfo.strServerName = acc_GetServerName(i);
        listAccount.push_back(accountInfo);
    }

    return true;
}
#endif

int acc_GetEmergencyAccount()
{
    CBaseAccount * pAccount = _AccountManager.GetEmergencyAccount();
    return pAccount ? pAccount->GetAccountId() : INVALID_ACCOUNTID;
}

bool acc_IsBLAEnable(int iAccount)
{
    BLA_TYPE eBLAType = acc_GetBLAType(iAccount);
    return eBLAType == BLA_BROADSOFT_SCA || BLA_DRAFT_BLA == eBLAType || BLA_GENBAND == eBLAType;
}

bool acc_IsBLANeedSeize(int iAccount)
{
    BLA_TYPE eBLAType = acc_GetBLAType(iAccount);
    return eBLAType == BLA_BROADSOFT_SCA || BLA_DRAFT_BLA == eBLAType;
}

int acc_GetAvailableAccountCount()
{
    return _AccountManager.GetUsableAccountCount();
}

#if IF_FEATURE_32591
yl::string acc_GetStatusBarLabel(int iAccount)
{
    return configParser_GetCfgItemStringValue(kszAccountStatusBarLabel, iAccount);
}

yl::string acc_GetAccountStatusShowText(int iAccount)
{
    yl::string strStatusBar = acc_GetStatusBarLabel(iAccount);
    return strStatusBar.empty() ? acc_GetAccountShowText(iAccount) : strStatusBar;
}
#endif

ACCOUNT_TYPE acc_GetAccountType(int iAccount)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccount);
    if (pAccount)
    {
        return pAccount->GetAccountType();
    }

    return ACCOUNT_TYPE_NONE;
}

SipAccountType acc_GetSipAccountType(int iAccount)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccount);
    if (pAccount && ACCOUNT_TYPE_SIP == pAccount->GetAccountType())
    {
        return (SipAccountType)configParser_GetCfgItemIntValue(kszAccountType, iAccount);
    }

    return ACCOUNT_COMMON;
}

AccountProtocolType acc_GetAccountProtocol(int iAccount)
{
    CBaseAccount * pAccount = _AccountManager.GetAccountByAccountId(iAccount);
    if (pAccount != NULL)
    {
        return pAccount->GetProtocol();
    }

    return PROTOCOL_NONE;
}

#ifdef IF_FEATURE_CLOUD

SipAccountType acc_GetRegisterSipAccountTypeById(int iAccount)
{
    if (acc_IsAccountAvailable(iAccount))
    {
        return acc_GetSipAccountType(iAccount);
    }

    return ACCOUNT_COMMON;
}

//只有一个云账号
int acc_GetRegisterYlCloudAccount()
{
    for (int i = 0; i < acc_AccountNum(); i++)
    {
        int nAccountId = i;
        if (acc_GetSipAccountType(nAccountId) == ACCOUNT_YEALINK)
        {
            return nAccountId;
        }
    }

    return -1;
}

bool acc_CloudAccountLogin(CloudAccountInfo & stInfo)
{
    return _CloudAccountInfoManager.ReqLogin(stInfo);
}

bool acc_CloudPinLogin(int nAccountId, const yl::string& strPinCode)
{
    return _CloudAccountInfoManager.CloudYLPinLogin(nAccountId, strPinCode);
}

bool acc_RemoveCloudAccountInfo(const yl::string & strUserName,
                                SipAccountType eType/* = ACCOUNT_YEALINK*/)
{
    return _CloudAccountInfoManager.RemoveHistoryCloudAccountInfo(strUserName, eType);
}

bool acc_GetCloudAccountHistory(VectorInfo & refData, SipAccountType eType/* = ACCOUNT_YEALINK*/,
                                OrderType eOrderType /*= REVERSE_ORDER*/)
{
    return _CloudAccountInfoManager.GetCloudAccountInfo(refData, eType, eOrderType);
}

bool acc_GetCloudAccount(CloudAccountInfo & refData)
{
    return _CloudAccountInfoManager.GetLoginCloudAccountInfo(refData);
}

bool acc_NeedCloudAccountGuide()
{
    if (!acc_IsCloudEnable())
    {
        return false;
    }

    return _AccountManager.IsAllAccountEmpty();
}

bool acc_IsCloudAccount(int nAccountId)
{
    SipAccountType eType = acc_GetSipAccountType(nAccountId);
    if (eType == ACCOUNT_ZOOM
            || eType == ACCOUNT_YEALINK
            || eType == ACCOUNT_PEXIP
            || eType == ACCOUNT_MIND
            || eType == ACCOUNT_BLUEJEANS
            || eType == ACCOUNT_CUSTOM)
    {
        return true;
    }

    return false;
}

bool acc_Is3rdCloudAccount(SipAccountType eType)
{
    if (eType == ACCOUNT_ZOOM
            || eType == ACCOUNT_PEXIP
            || eType == ACCOUNT_MIND
            || eType == ACCOUNT_BLUEJEANS
            || eType == ACCOUNT_CUSTOM)
    {
        return true;
    }

    return false;
}

bool acc_IsCloudEnable()
{
    if (configParser_GetConfigInt(kszVCAccountActive) == 0
            || configParser_GetConfigInt(kszVCAccountEnable) == 0)
    {
        return false;
    }

    return true;
}

bool acc_CloudAccountLogout(int nAccountId)
{
    return _CloudAccountInfoManager.CloudAccountLogout(nAccountId);
}

bool acc_CancelCloudLogin(int nAccountId)
{
    return _CloudAccountInfoManager.CancelLogin(nAccountId);
}

#endif //#ifdef IF_FEATURE_CLOUD

bool acc_IsSupportH264(int iAccountId)
{
    return _AccountManager.IsSupportH264(iAccountId);
}

/*******************************************************************
** 函数描述:        通过配置名和账号ID获取配置
** 参数：            szConfigName-配置名 iAccountID-账号ID
** 返回:          配置信息
********************************************************************/
int acc_GetCfgItemIntValue(const char * szConfigName, int iAccountID)
{
    return configParser_GetCfgItemIntValue(szConfigName, iAccountID);
}

#ifdef IF_FEATURE_H323
int acc_GetH323DirectIPAccountID()
{
    CBaseAccount * pAccount = _AccountManager.GetH323DirectIPAccount();
    if (pAccount == NULL)
    {
        return -1;
    }

    return pAccount->GetAccountId();
}

int acc_MapH323AccountId2Logic(int iAccountId)
{
    // 当前只有一个H323账号 0 ->H323ACCOUNTID
    return _AccountManager.MapH323AccountId2Logic(iAccountId);
}

LINE_STATE acc_MapH323State2Logic(int iState)
{
    // 注册失败情况下的处理，Vcs根据gk名字特殊处理
    LINE_STATE eState = LS_BE_UNREGISTERED;
    switch (iState)
    {
    case H323_REG_UNKNOWN:
        eState = LS_BE_UNREGISTERED;
        break;
    //case SIP_REG_DISABLE: eState = LS_DISABLE; break;
    case H323_REG_REGING:
        eState = LS_REGISTERING;
        break;
    case H323_REG_REGED:
        eState = LS_REGISTERED;
        break;
    case H323_REG_FAILED:
        eState = LS_REGISTER_FAIL;
        break;
    case H323_REG_UNREGING:
        eState = LS_UNREGISTERING;
        break;
    case H323_REG_UNREGED:
        eState = LS_BE_UNREGISTERED;
        break;
    //case SIP_REG_REGISTERED_ON_BOOT: eState = LS_INEXISTENCE; break;
    default:
        break;
    }

    return eState;
}
#endif //#ifdef IF_FEATURE_H323

int acc_GetAccountTransPort(int nAccountId)
{
    if (nAccountId < 0)
    {
        return -1;
    }

    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(
                                 nAccountId));
    if (pAccount != NULL)
    {
        return pAccount->GetTransPort();
    }

    return -1;
}

bool acc_GetCustomLabel(int iAccount, int index, yl::string & strLabel)
{
    if (iAccount < 0 || index < 0 || !strLabel.empty())
    {
        return false;
    }

    if (!acc_IsAutoLabelEnable(iAccount))
    {
        return false;
    }

    yl::string str = configParser_GetCfgItemStringValue(kszAccountAutoLableRule, iAccount);

    yl::string strAccLabel = acc_GetLabel(iAccount);
    // 手动 > 规则 > 账号
    bool bRet = commonUnits_GetCustomLabel(strLabel, str, strAccLabel, index);

    ACCOUNT_INFO("GetCustomLabel line=[%d] index=[%d] str=[%s] ret=[%d]", iAccount, index, str.c_str(),
                 bRet);
    return bRet;
}

bool acc_IsAutoLabelEnable(int iAccount)
{
    return configParser_GetCfgItemIntValue(kszAccountAutoLable, iAccount) != 0;
}

bool acc_SetServerByServerID(int nAccountId, int iServerID)
{
    if (nAccountId < 0)
    {
        return false;
    }

    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(
                                 nAccountId));
    if (pAccount == NULL)
    {
        return false;
    }

    return pAccount->SetServerByServerID(iServerID);
}

#if IF_BUG_39041
bool acc_SetSupportSCA(int nAccountId, bool bSupport)
{
    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(nAccountId));
    if (pAccount == NULL)
    {
        return false;
    }
    ACCOUNT_INFO("SetSupportSCA  nAccountId[%d] bSupport[%d]", nAccountId, bSupport);
    pAccount->SetSupportSCA(bSupport);
    return true;
}

bool acc_IsSupportSCA(int nAccountId)
{
    CSipAccount * pAccount = dynamic_cast<CSipAccount *>(_AccountManager.GetAccountByAccountId(nAccountId));
    if (pAccount == NULL)
    {
        return false;
    }

    return pAccount->IsSupportSCA();
}
#endif
