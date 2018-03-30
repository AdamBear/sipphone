#include "setting_inc.h"

bool GetAccountDetailData(AccountDetailData * pData, int iAccountID)
{
    if (pData == NULL)
    {
        return false;
    }

    if (-1 == iAccountID)
    {
        return false;
    }

    // Get account label.
    pData->m_strLabel = acc_GetAccountLabel(iAccountID);
    // Get account display name.
    pData->m_strDisplayName = configParser_GetCfgItemStringValue(kszAccountDisplayName, iAccountID);
    // Get account register name.
    pData->m_strRegisterName = configParser_GetCfgItemStringValue(kszAccountRegisterName, iAccountID);
    // Get account username.
    pData->m_strUserName = configParser_GetCfgItemStringValue(kszAccountUserName, iAccountID);
    // Get account password.
    pData->m_strPassword = configParser_GetCfgItemStringValue(kszAccountPassword, iAccountID);
    // Get account sip server.
    pData->m_strSipServer1 = configParser_GetCfgItemStringValue(kszAccountSIPServer1Host, iAccountID);
    pData->m_strSipServer2 = configParser_GetCfgItemStringValue(kszAccountSIPServer2Host, iAccountID);

    // 是否使用备份服务器的配置
    if (pData->m_strSipServer1.empty()
            && pData->m_strSipServer2.empty())
    {
        pData->m_strSipServer1 = configParser_GetCfgItemStringValue(kszAccountSIPServerHost, iAccountID);
    }

    pData->m_strOutboundProxy1 = configParser_GetCfgItemStringValue(kszAccountOutboundProxy, iAccountID,
                                 1);
    pData->m_strOutboundProxy2 = configParser_GetCfgItemStringValue(kszAccountOutboundProxy, iAccountID,
                                 2);

    pData->m_nProxyFallbackInterval = configParser_GetCfgItemIntValue(kszAccountProxyFallbackInterval,
                                      iAccountID);

    // Get account status.
    pData->m_isActivated = acc_IsAccountEnable(iAccountID);
    // Get account outbound status.
    pData->m_isOutboundEnable = configParser_GetCfgItemIntValue(kszAccountOutboundSwitch,
                                iAccountID) == 1;
    // Get account Nat status
    pData->m_isStunEnable = configParser_GetCfgItemIntValue(kszAccountStunSwitch, iAccountID) == 1;

    return true;
}

void SetAccountDetailData(int iAccountID, const AccountDetailData & refData)
{
    if (-1 == iAccountID)
    {
        return;
    }

#ifdef IF_FEATURE_PSTN
    configParser_SetCfgItemIntValue(kszAccountType, iAccountID, ACCOUNT_COMMON);
#endif

    bool bXSIConfigChanged = false;
    bool bChange = false;

    bool bLocked = configParser_GetConfigInt(kszLockLevel) != 0;
    // Set account label.
#if IF_BUG_21893
#if IF_BUG_26237
    bool bLabelChanged = false;

    bLabelChanged = acc_SetAccountLabel(iAccountID, refData.m_strLabel, CONFIG_LEVEL_PHONE);
#else
    bChange |= acc_SetAccountLabel(iAccountID, refData.m_strLabel, CONFIG_LEVEL_PHONE);
#endif
    // Set account display name.
    bChange |= configParser_SetCfgItemStringValue(kszAccountDisplayName, iAccountID,
               refData.m_strDisplayName.c_str(), CONFIG_LEVEL_PHONE);
    // Set account register name.
    bool bRegisterChanged = configParser_SetCfgItemStringValue(kszAccountRegisterName, iAccountID,
                            refData.m_strRegisterName.c_str(), CONFIG_LEVEL_PHONE);
    bChange = bRegisterChanged || bChange;
    bXSIConfigChanged = bRegisterChanged || bXSIConfigChanged;

    // Set account username.
    bChange |= configParser_SetCfgItemStringValue(kszAccountUserName, iAccountID,
               refData.m_strUserName.c_str(), CONFIG_LEVEL_PHONE);
    // Set account password.
    bool bPwdChanged = configParser_SetCfgItemStringValue(kszAccountPassword, iAccountID,
                       refData.m_strPassword.c_str(), CONFIG_LEVEL_PHONE);
    bChange = bPwdChanged || bChange;
    bXSIConfigChanged = bPwdChanged || bXSIConfigChanged;

    if (!bLocked)
    {
        // Set account sip server.
        bChange |= configParser_SetCfgItemStringValue(kszAccountSIPServer1Host, iAccountID,
                   refData.m_strSipServer1.c_str(), CONFIG_LEVEL_PHONE);
        bChange |= configParser_SetCfgItemStringValue(kszAccountSIPServer2Host, iAccountID,
                   refData.m_strSipServer2.c_str(), CONFIG_LEVEL_PHONE);

        // 需同时写到旧的配置中，以兼容升级到旧版本的情况
        bChange |= configParser_SetCfgItemStringValue(kszAccountSIPServerHost, iAccountID,
                   refData.m_strSipServer1.c_str(), CONFIG_LEVEL_PHONE);

        // Set account outbound proxy.
        bChange |= configParser_SetCfgItemStringValue(kszAccountOutboundProxy, iAccountID, 1,
                   refData.m_strOutboundProxy1.c_str(), CONFIG_LEVEL_PHONE);
        bChange |= configParser_SetCfgItemStringValue(kszAccountOutboundProxy, iAccountID, 2,
                   refData.m_strOutboundProxy2.c_str(), CONFIG_LEVEL_PHONE);
    }

    bChange |= configParser_SetCfgItemIntValue(kszAccountProxyFallbackInterval, iAccountID,
               refData.m_nProxyFallbackInterval, CONFIG_LEVEL_PHONE);

    // Get account status.
    bChange |= acc_SetAccountEnable(iAccountID, refData.m_isActivated, CONFIG_LEVEL_PHONE);
    // Set account outbound status.
    bChange |= configParser_SetCfgItemIntValue(kszAccountOutboundSwitch, iAccountID,
               refData.m_isOutboundEnable ? 1 : 0, CONFIG_LEVEL_PHONE);
    // Set account Nat status
    bChange |= configParser_SetCfgItemIntValue(kszAccountStunSwitch, iAccountID,
               refData.m_isStunEnable ? 1 : 0, CONFIG_LEVEL_PHONE);
#else
    bChange |= acc_SetAccountLabel(iAccountID, refData.m_strLabel);
    // Set account display name.
    bChange |= configParser_SetCfgItemStringValue(kszAccountDisplayName, iAccountID,
               refData.m_strDisplayName.c_str());
    // Set account register name.
    bool bRegisterChanged = configParser_SetCfgItemStringValue(kszAccountRegisterName, iAccountID,
                            refData.m_strRegisterName.c_str());
    bChange = bRegisterChanged || bChange;
    bXSIConfigChanged = bRegisterChanged || bXSIConfigChanged;

    // Set account username.
    bChange |= configParser_SetCfgItemStringValue(kszAccountUserName, iAccountID,
               refData.m_strUserName.c_str());
    // Set account password.
    bool bPwdChanged = configParser_SetCfgItemStringValue(kszAccountPassword, iAccountID,
                       refData.m_strPassword.c_str());
    bChange = bPwdChanged || bChange;
    bXSIConfigChanged = bPwdChanged || bXSIConfigChanged;

    if (!bLocked)
    {
        // Set account sip server.
        bChange |= configParser_SetCfgItemStringValue(kszAccountSIPServer1Host, iAccountID,
                   refData.m_strSipServer1.c_str());
        bChange |= configParser_SetCfgItemStringValue(kszAccountSIPServer2Host, iAccountID,
                   refData.m_strSipServer2.c_str());

        // 需同时写到旧的配置中，以兼容升级到旧版本的情况
        bChange |= configParser_SetCfgItemStringValue(kszAccountSIPServerHost, iAccountID,
                   refData.m_strSipServer1.c_str());

        // Set account outbound proxy.
        bChange |= configParser_SetCfgItemStringValue(kszAccountOutboundProxy, iAccountID, 1,
                   refData.m_strOutboundProxy1.c_str());
        bChange |= configParser_SetCfgItemStringValue(kszAccountOutboundProxy, iAccountID, 2,
                   refData.m_strOutboundProxy2.c_str());
    }

    bChange |= configParser_SetCfgItemIntValue(kszAccountProxyFallbackInterval, iAccountID,
               refData.m_nProxyFallbackInterval);

    // Get account status.
    bChange |= acc_SetAccountEnable(iAccount, refData.m_isActivated);
    // Set account outbound status.
    bChange |= configParser_SetCfgItemIntValue(kszAccountOutboundSwitch, iAccountID,
               refData.m_isOutboundEnable ? 1 : 0);
    // Set account Nat status
    bChange |= configParser_SetCfgItemIntValue(kszAccountStunSwitch, iAccountID,
               refData.m_isStunEnable ? 1 : 0);
#endif
    if (bChange)
    {
        //Notify Config Change
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, iAccountID);

        //异步通知账号配置改变
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, iAccountID);
    }
#if IF_BUG_26237
    else if (bLabelChanged)
    {
        //异步通知账号配置改变
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, iAccountID);
    }
#endif

    if (bXSIConfigChanged)
    {
        //通知xsi信息改变
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_XSI_CONFIGE, iAccountID);
    }
}

bool GetPhoneLockData(PhoneLockData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // Read phone lock type.
    pData->m_ePhoneLockType = (PhoneLockType)configParser_GetConfigInt(kszKeyGuardType);
    pData->m_bPhoneLock = (bool)configParser_GetConfigInt(kszKeyGuardEnable);
    pData->m_nPhoneLockTimeOut = configParser_GetConfigInt(kszLockTimeout);

    return true;
}

void SetPhoneLockData(const PhoneLockData & refData)
{

#if IF_BUG_21893
    configParser_SetConfigInt(kszKeyGuardEnable, refData.m_bPhoneLock, CONFIG_LEVEL_PHONE);
    configParser_SetConfigInt(kszKeyGuardType, refData.m_ePhoneLockType, CONFIG_LEVEL_PHONE);
    configParser_SetConfigInt(kszLockTimeout, refData.m_nPhoneLockTimeOut, CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigInt(kszKeyGuardEnable, refData.m_bPhoneLock);
    configParser_SetConfigInt(kszKeyGuardType, refData.m_ePhoneLockType);
    configParser_SetConfigInt(kszLockTimeout, refData.m_nPhoneLockTimeOut);
#endif
    // Post message.
    // need modify
//  etl_NotifyApp(false, PHONE_LOCK_TYPE_CHANGED, 0, refData.m_ePhoneLockType);
    etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_LOCK, refData.m_ePhoneLockType);
}

bool GetPhoneBacklightData(PhoneBacklightData * pData)
{
    if (NULL == pData)
    {
        return false;
    }

    pData->m_iLevel = configParser_GetConfigInt(kszActiveLevel);
    pData->m_iTime = configParser_GetConfigInt(kszBackLightTime);
    return true;
}

void SetPhoneBacklightData(const PhoneBacklightData & refData)
{
#if IF_BUG_21893
    configParser_SetConfigInt(kszActiveLevel, refData.m_iLevel, CONFIG_LEVEL_PHONE);
    configParser_SetConfigInt(kszBackLightTime, refData.m_iTime, CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigInt(kszActiveLevel, refData.m_iLevel);
    configParser_SetConfigInt(kszBackLightTime, refData.m_iTime);
#endif

    etl_NotifyApp(false, DSK_MSG_LIGHT_STATUS_CHANGE, 0, 0);
    etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, 0, 0);
}

bool GetPhoneContrastData(PhoneContrastData * pData)
{
    pData->m_iLevel = configParser_GetConfigInt(kszContrastLevel);
    return true;
}

void SetPhoneContrastData(const PhoneContrastData & refData)
{
    // 写入到配置文件里
#if IF_BUG_21893
    configParser_SetConfigInt(kszContrastLevel, refData.m_iLevel, CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigInt(kszContrastLevel, refData.m_iLevel);
#endif

    // 发送修改话机对比度
    SettingUILogic_Contrast(refData.m_iLevel);
}

bool IsFwdInternationEnable()
{
    return (bool)configParser_GetConfigInt(kszForwardInternationalEnable);
}

void SetFwdInternationActive(bool bEnable)
{
#if IF_BUG_21893
    configParser_SetConfigInt(kszForwardInternationalEnable, (int)bEnable, CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigInt(kszForwardInternationalEnable, (int)bEnable);
#endif
    //向所有线程广播配置文件更改的消息
    etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_FORWARD, 0);
    //msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_FORWARD, 0);
}

bool GetAutoProvisionData(AutoProvisionData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // Read auto provision URL.
    pData->m_strUrl = configParser_GetConfigString(kszAutoProvisionUrl);
    // Read auto provision username.
    pData->m_strUsername = configParser_GetConfigString(kszAutoProvisionUsername);
    // Read auto provision password.
    pData->m_strPassword = configParser_GetConfigString(kszAutoProvisionPassword);
    return true;
}

void SetAutoProvisionData(
    const AutoProvisionData & refData, bool bSavingUrl/* = true*/)
{
    if (bSavingUrl)
    {
        configParser_SetConfigString(kszAutoProvisionUrl, refData.m_strUrl.c_str(), CONFIG_LEVEL_PHONE);
    }
    configParser_SetConfigString(kszAutoProvisionUsername, refData.m_strUsername.c_str(),
                                 CONFIG_LEVEL_PHONE);
    configParser_SetConfigString(kszAutoProvisionPassword, refData.m_strPassword.c_str(),
                                 CONFIG_LEVEL_PHONE);
}

bool BeginAutoProvision()
{
    // TODO: Begin auto provision.
    static char kszAutopThreadNamep[] = "autoServer";
    msgPostMsgToThread(msgGetThreadByName(kszAutopThreadNamep), ATP_MSG_REQ_START_BY_DEFAULT, 0, 0);
    return true;
}

bool GetResetData(ResetToFactoryData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // Read reset-option from config file.
    pData->m_eType = static_cast<ResetOptionType>(configParser_GetConfigInt(kszResetOption));
    return true;
}

void SetResetData(const ResetToFactoryData & refData)
{
#if IF_BUG_21893
    configParser_SetConfigInt(kszResetOption, refData.m_eType, CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigInt(kszResetOption, refData.m_eType);
#endif
}

bool GetAesKeyData(AesKeyData * pData)
{
    if (pData == NULL)
    {
        return false;
    }

    // Get AES key.
    pData->m_strAesKey = configParser_GetConfigString(kszAesKey);
    // Get AES MAC key.
    pData->m_strAesMacKey = configParser_GetConfigString(kszAesMacKey);
    return true;
}

void SetAesKeyData(const AesKeyData & refData)
{
#if IF_BUG_21893
    configParser_SetConfigString(kszAesKey, refData.m_strAesKey.c_str(), CONFIG_LEVEL_PHONE);
    configParser_SetConfigString(kszAesMacKey, refData.m_strAesMacKey.c_str(), CONFIG_LEVEL_PHONE);
#else
    configParser_SetConfigString(kszAesKey, refData.m_strAesKey.c_str());
    configParser_SetConfigString(kszAesMacKey, refData.m_strAesMacKey.c_str());
#endif
}

// 生成密码
yl::string EncryptPassword(LPCSTR lpNewPassword)
{
    //char pBuffer[256] = {0};
    // 加密后的密文
    char lpCryptograph[100] = {0};
    char Salt[3] = {0};
    char itoa64[] =
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"; /* 0 ... 63 => ascii - 64 */

    for (int i = 0; i < 2; i++)
    {
        int iVal = rand();
        if (iVal < 0)
        {
            iVal = -iVal;
        }
        Salt[i] = itoa64[iVal % 64];
    }
    Salt[2] = '\0';

    strcpy(lpCryptograph, lpNewPassword);

    //获取加密后的新密码
    yl::string strResult = commonAPI_EncryptPassword(lpCryptograph, Salt);
    return strResult;
}

bool SetPassword(const PasswordData & refData, const char * pUserName)
{
    FILE * pFile = fopen(szPasswordFile, "r");
    if (pFile == NULL)
    {
        return false;
    }
    yl::string strNewPassword = EncryptPassword(refData.m_strPasswordNew.c_str());

    yl::string strNewFile = "";
    strcat((char *)pUserName, ":");
    while (!feof(pFile))
    {
        char szCode[256] = {0};
        if (fgets(szCode, 255, pFile) == NULL)
        {
            continue;
        }

        char szUserName[64] = {0};
        char szEncryptPwd[192] = {0};
        sscanf(szCode, "%[^:]:%[^\n]", szUserName, szEncryptPwd);
        if (strcmp(szCode, pUserName) == 0
                || sscanf(szCode, "%[^:]:%[^\n]", szUserName, szEncryptPwd) == 2
                && strcmp(pUserName, strcat(szUserName, ":")) == 0)
        {
            yl::string strUserName = pUserName;
            strNewFile = strNewFile + strUserName + strNewPassword + "\n";
            continue;
        }

        strNewFile = strNewFile + szCode;
    }

    //读完之后关闭文件
    fclose(pFile);
    pFile = NULL;

    //再重新打开文件
    pFile = fopen(szPasswordFile, "w+");
    if (NULL == pFile)
    {
        return false;
    }

    fwrite(strNewFile.c_str(), sizeof(char), strNewFile.length(), pFile);
    fclose(pFile);
    return true;
}

#ifdef IF_FEATURE_HOTDESKING
void SetHotDeskBaseData(const HotDeskBaseData & refData)
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
    LIST_STRING listAccountID;

    if (commonAPI_splitStringBySeparator(strClearAccount, ',', listAccountID)
            && listAccountID.size() > 0)
    {
        LIST_STRING::iterator it = listAccountID.begin();
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

    bool bLocked = configParser_GetConfigInt(kszLockLevel) != 0;
#if IF_BUG_21893
    // Set account status.
    bChange |= acc_SetAccountEnable(iAccountID, true, CONFIG_LEVEL_PHONE);
    // Set hotdesk type
    bChange |= configParser_SetCfgItemIntValue(kszAccountIsHotDeskAccount, iAccountID, 1,
               CONFIG_LEVEL_PHONE);
    // Set account label.
    bChange |= acc_SetAccountLabel(iAccountID, strTemp, CONFIG_LEVEL_PHONE);
    // Set account display name.
    bChange |= configParser_SetCfgItemStringValue(kszAccountDisplayName, iAccountID, strTemp.c_str(),
               CONFIG_LEVEL_PHONE);
#else
    // Set account status.
    bChange |= acc_SetAccountEnable(iAccountID, true);
    // Set hotdesk type
    bChange |= configParser_SetCfgItemIntValue(kszAccountIsHotDeskAccount, iAccountID, 1);
    // Set account label.
    bChange |= acc_SetAccountLabel(iAccountID, strTemp);
    // Set account display name.
    bChange |= configParser_SetCfgItemStringValue(kszAccountDisplayName, iAccountID, strTemp.c_str());
#endif
    // Set account register name.
    if (refData.m_strRegisterName.empty())
    {
#if IF_BUG_21893
        bChange |= configParser_SetCfgItemStringValue(kszAccountRegisterName, iAccountID,
                   refData.m_strUserName.c_str(), CONFIG_LEVEL_PHONE);
#else
        bChange |= configParser_SetCfgItemStringValue(kszAccountRegisterName, iAccountID,
                   refData.m_strUserName.c_str(), CONFIG_LEVEL_PHONE);
#endif
    }
    else
    {
#if IF_BUG_21893
        bChange |= configParser_SetCfgItemStringValue(kszAccountRegisterName, iAccountID,
                   refData.m_strRegisterName.c_str(), CONFIG_LEVEL_PHONE);
#else
        bChange |= configParser_SetCfgItemStringValue(kszAccountRegisterName, iAccountID,
                   refData.m_strRegisterName.c_str());
#endif
    }

#if IF_BUG_21893
    // Set account username.
    bChange |= configParser_SetCfgItemStringValue(kszAccountUserName, iAccountID, strTemp.c_str(),
               CONFIG_LEVEL_PHONE);
#else
    bChange |= configParser_SetCfgItemStringValue(kszAccountUserName, iAccountID, strTemp.c_str());
#endif

    if (configParser_GetConfigInt(kszHotDeskingPassword) != 0)
    {
#if IF_BUG_21893
        // Set account password.
        bChange |= configParser_SetCfgItemStringValue(kszAccountPassword, iAccountID,
                   refData.m_strPassword.c_str(), CONFIG_LEVEL_PHONE);
#else
        bChange |= configParser_SetCfgItemStringValue(kszAccountPassword, iAccountID,
                   refData.m_strPassword.c_str());
#endif
    }

    if ((!refData.m_strSipServer1.empty()
            || !refData.m_strSipServer2.empty()) && !bLocked)
    {
#if IF_BUG_21893
        // Set account sip server1
        bChange |= configParser_SetCfgItemStringValue(kszAccountSIPServer1Host, iAccountID,
                   refData.m_strSipServer1.c_str(), CONFIG_LEVEL_PHONE);
        // Set account sip server2
        bChange |= configParser_SetCfgItemStringValue(kszAccountSIPServer2Host, iAccountID,
                   refData.m_strSipServer2.c_str(), CONFIG_LEVEL_PHONE);
#else
        // Set account sip server1
        bChange |= configParser_SetCfgItemStringValue(kszAccountSIPServer1Host, iAccountID,
                   refData.m_strSipServer1.c_str());
        // Set account sip server2
        bChange |= configParser_SetCfgItemStringValue(kszAccountSIPServer2Host, iAccountID,
                   refData.m_strSipServer2.c_str());
#endif
    }

    if (!refData.m_strOutboundProxy1.empty()
            || !refData.m_strOutboundProxy2.empty())
    {
#if IF_BUG_21893
        // Set account outbound status.
        bChange |= configParser_SetCfgItemIntValue(kszAccountOutboundSwitch, iAccountID,
                   refData.m_bOutboundEnable ? 1 : 0, CONFIG_LEVEL_PHONE);
        if (!bLocked)
        {
            // Set account outbound proxy.
            //bChange |= configParser_SetCfgItemStringValue(kszAccountBakSIPServerHost, iAccountID, refData.m_strOutboundProxy.c_str(), CONFIG_LEVEL_PHONE);
            bChange |= configParser_SetCfgItemStringValue(kszAccountOutboundProxy, iAccountID, 1,
                       refData.m_strOutboundProxy1.c_str(), CONFIG_LEVEL_PHONE);
            bChange |= configParser_SetCfgItemStringValue(kszAccountOutboundProxy, iAccountID, 2,
                       refData.m_strOutboundProxy2.c_str(), CONFIG_LEVEL_PHONE);
        }
#else
        // Set account outbound status.
        bChange |= configParser_SetCfgItemIntValue(kszAccountOutboundSwitch, iAccountID,
                   refData.m_bOutboundEnable ? 1 : 0);
        if (!bLocked)
        {
            // Set account outbound proxy.
            //bChange |= configParser_SetCfgItemStringValue(kszAccountBakSIPServerHost, iAccountID, refData.m_strOutboundProxy.c_str());
            bChange |= configParser_SetCfgItemStringValue(kszAccountOutboundProxy, iAccountID, 1,
                       refData.m_strOutboundProxy1.c_str());
            bChange |= configParser_SetCfgItemStringValue(kszAccountOutboundProxy, iAccountID, 2,
                       refData.m_strOutboundProxy2.c_str());
        }
#endif
    }

#if IF_BUG_22024_LogonWizard_AutopURL
    yl::string strURL = "";
#if IF_BUG_29534
#define kszHotDeskingURLParam "/getPhoneCfgRom.do?filename=%s&user=%s&type=%s"

    strURL = configParser_GetConfigString(kszStart_HotDeskingURL);
    if (!strURL.empty())
    {
        yl::string strCfgName = commonUnits_GetConfigFileByPhone();
        char szBuf[256] = {0};

        char bufProductName[512] = {0};
        sys_replace_wildcards((char *)configParser_GetConfigString(kszProductName).c_str(), 0,
                              bufProductName, sizeof(bufProductName) / sizeof(char));
        yl::string strType = "yealink-";
        strType.append(bufProductName);      // tpye格式为yealink-SIP-T23P

        sprintf(szBuf, "%s" kszHotDeskingURLParam, strURL.c_str(), strCfgName.c_str(), strTemp.c_str(),
                strType.c_str());

        strURL = szBuf;
    }
#else
    if (refData.m_eAutopURL != LW_AUTOP_URL_NONE)
    {
        if (refData.m_eAutopURL == LW_AUTOP_URL_OFFICE)
        {
            strURL = configParser_GetConfigString(kszStart_HotDeskingOfficeURL);
        }
        else if (refData.m_eAutopURL == LW_AUTOP_URL_CALLCENTER)
        {
            strURL = configParser_GetConfigString(kszStart_HotDeskingCallCenterURL);
        }
    }
#endif

    if (!strURL.empty())
    {
#if IF_BUG_21893
        configParser_SetConfigString(kszAutoProvisionUrl, strURL.c_str(), CONFIG_LEVEL_PHONE);
#else
        configParser_SetConfigString(kszAutoProvisionUrl, strURL.c_str());
#endif
        // 改变autop服务器的参数
        msgPostMsgToThread(msgGetThreadByName(VP_AP_THREAD_NAME), ATP_MSG_REQ_START_BY_DEFAULT, 0, 0);
    }
#endif//IF_BUG_22024_LogonWizard_AutopURL

    if (bChange || configParser_GetConfigInt(kszExitWizardAfterLogin) == 0)
    {
        //Notify Config Change
        msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, iAccountID);

        //异步通知账号配置改变
        etl_NotifyApp(false, CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, iAccountID);
    }
}

#endif  //IF_FEATURE_HOTDESKING
