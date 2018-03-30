#include "account_inc.h"
#include "sipaccount.h"
#include "modaccount.h"
#include "voip/modvoip.h"

//构造函数
CSipAccount::CSipAccount()
    : CBaseAccount(ACCOUNT_TYPE_SIP, PROTOCOL_SIP)
{
    m_iServerID = -1;
    m_iFailId = 0;
    m_strFailInfo = "";
#ifndef _T49
    m_uReadedVoiceMailCount = 0;
    m_uUnReadVoiceMailCount = 0;
#endif
    m_bSyncSubscribe = false;
    m_bDNDSyncAndOn = false;
    m_bCFASyncAndOn = false;

    m_strServerName = "";

    m_eServerType = SST_DEFAULT;
#ifdef IF_FEATURE_GETBALANCE
    m_bBalanceEnable = false;
    m_strBalanceLink = "";
    m_strPasswd = "";
#endif
    m_iServerPort = 5060;

    m_nTransPort = -1;
#if IF_BUG_39041
    m_bIsSupportSCA = true;
#endif
}

//析构函数
CSipAccount::~CSipAccount()
{
}

//打印帐号信息
void CSipAccount::PrintAccoutInfo() const
{
    ACCOUNT_INFO("$$$$$$$$PrintAccoutInfo$$$$$$$$");
    ACCOUNT_INFO("Account ID[%d]", m_iAccountId);
    ACCOUNT_INFO("Account State[%d]", m_eAccountState);
    ACCOUNT_INFO("Direct IP[%d]", IsDirectIPAccount());
    ACCOUNT_INFO("Account Name[%s]", GetUserName().c_str());
    ACCOUNT_INFO("Server Name[%s]", GetServerName().c_str());
    ACCOUNT_INFO("Display Name[%s]", GetDisplayName().c_str());
    ACCOUNT_INFO("Fail Id[%d], Fail Info[%s]", m_iFailId, m_strFailInfo.c_str());
    ACCOUNT_INFO("Auto Answer[%d], Auto Answer Time[%d]", acc_IsAccountAutoAnswerEnable(m_iAccountId),
                 GetAutoAnswerTime());
    ACCOUNT_INFO("DTMF Type[%d]", configParser_GetCfgItemIntValue(kszAccountDTMFType, m_iAccountId));
    ACCOUNT_INFO("Account Ring File[%s]", acc_GetAccountRing(m_iAccountId).c_str());
#ifndef _T49
    ACCOUNT_INFO("Voice Mail Number[%s], OldCount[%d], NewCount[%d]",
                 configParser_GetCfgItemStringValue(kszAccountVoiceMailNumber, m_iAccountId).c_str(),
                 m_uReadedVoiceMailCount, m_uUnReadVoiceMailCount);
    ACCOUNT_INFO("Open Local Video[%d]",
                 0/*configParser_GetCfgItemIntValue("kszAccountActiveCamera*", m_iAccountId)*/);
#endif // T49
    ACCOUNT_INFO("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
}

bool CSipAccount::IsEmptyAccount()
{
    //PrintAccoutInfo();

    if (configParser_GetCfgItemIntValue(kszAccountEnable, m_iAccountId) != 0
            || configParser_GetCfgItemStringValue(kszAccountLabel, m_iAccountId) != ""
            || configParser_GetCfgItemStringValue(kszAccountDisplayName, m_iAccountId) != ""
            || configParser_GetCfgItemStringValue(kszAccountUserName, m_iAccountId) != ""
            || configParser_GetCfgItemStringValue(kszAccountRegisterName, m_iAccountId) != ""
            || configParser_GetCfgItemStringValue(kszAccountPassword, m_iAccountId) != ""
            || configParser_GetCfgItemStringValue(kszAccountServerAddr, m_iAccountId, 1) != "")
    {
        return false;
    }

    return true;
}

//读取ServerName配置
void CSipAccount::InitServerName()
{
    //!!!!!，改为使用configParser处理
    yl::string strServer = "";
    for (int i = 1; i <= SIPSERVER_NUM; ++i)
    {
        /*
                char bufServer[32] = {0};
                sprintf(bufServer, "sipserver%d", i);

                HREGISTRY hFile = registryOpen((LPCSTR)commonAPI_FormatString("%s%d.cfg", SIP_ACCOUNT_FILE_PATH, m_iAccountId), bufServer);
                if (NULL == hFile)
                {
                    continue;
                }

                strServer = (LPCSTR)registryGetString(hFile, "server", "");
                if (!strServer.empty()
                    && strServer != "0.0.0.0")
                {
                    m_strServerName = strServer;
                    return;
                }
        */
        strServer = configParser_GetCfgItemStringValue(kszAccountServerAddr, m_iAccountId, i);
        if (!strServer.empty()
                && strServer != "0.0.0.0")
        {
            m_strServerName = strServer;
            m_iServerID = i;
            m_iServerPort = configParser_GetCfgItemIntValue(kszAccountServerPort, m_iAccountId, i);
            return;
        }
    }

    // 如果备份服务器还是为空,则使用旧的服务器,用以兼容旧的服务器
    if (strServer.empty()
            || strServer == "0.0.0.0")
    {
        m_strServerName = configParser_GetCfgItemStringValue(kszAccountSIPServerHost, m_iAccountId);
        m_iServerPort = configParser_GetCfgItemIntValue(kszAccountSIPServerPort, m_iAccountId);
        return;
    }
    m_strServerName = "";
}

//从配置文件中加载帐号信息
bool CSipAccount::LoadAccountFromConfig(const yl::string & lpszAccountPath,
                                        int iAccountId /* = -1 */)
{
    if (GET_CONFIG_PROCESS_TYPE() == CFG_SERVER_PROCESS)
    {

        if (iAccountId > acc_AccountNum(ACCOUNT_TYPE_SIP)
                && iAccountId != ACCOUNTID_OF_IPDIRECT_CALL)// ip直拨账号不能return
        {
            return false;
        }
    }
    else if (GET_CONFIG_PROCESS_TYPE() == INI_FILE_PROCESS)
    {
        if (lpszAccountPath.empty())
        {
            //账号地址为空或不存在，不予处理
            ACCOUNT_WARN("Account config file path is empty!");
            return false;
        }

        if (iAccountId == INVALID_ACCOUNTID)
        {
            //账号Id小于0，则账号Id从文件名中解析
            yl::string strFileName = pathGetFileNamePos(lpszAccountPath.c_str());
            int iTempId = 0;
            if (sscanf(strFileName.c_str(), SIP_ACCOUNT_FILE_NAME"%d.cfg", &iTempId) == 1)
            {
                //解析文件名成功
                iAccountId = iTempId;
            }
            else
            {
                //解析账号ID不成功，返回失败
                ACCOUNT_WARN("Cannot get account ID from config file name [%s]!", lpszAccountPath.c_str());
                return false;
            }
        }
    }

    // 校验账号ID有效性
    if (iAccountId < SIP_LINE_BEGIN_ID || iAccountId > SIP_LINE_END_ID)
    {
        ACCOUNT_WARN("Invalid PSTN account ID [%d]!", iAccountId);
        return false;
    }

    //赋值账号关联配置文件
    m_strConfigPath = lpszAccountPath;
    //赋值账号Id
    m_iAccountId = iAccountId;
    if (IsSpecialAccount())
    {
        //IP直拨，将账号状态设置为注册上
        m_eAccountState = netIsNetworkReady() ? LS_REGISTERED : LS_REGISTER_FAIL;
        m_eAccountType = ACCOUNT_TYPE_IP_DIRECT;
    }
    else
    {
        //非IP直拨，则将账号状态设置未注册上
        m_eAccountState = LS_BE_UNREGISTERED;
    }
    InitServerName();

    m_eServerType = (SIP_SERVER_TYPE)configParser_GetCfgItemIntValue(kszAccountSIPServerType,
                    iAccountId);
    m_eSipAccountType = (SipAccountType)configParser_GetCfgItemIntValue(kszAccountType, iAccountId);

    if (m_eServerType == SST_BROADSOFT && configParser_GetConfigInt(kszBroadsoftActive) != 1)
    {
        m_eServerType = SST_DEFAULT;
    }

#ifdef IF_FEATURE_GETBALANCE
    // 获取查询账号余额的信息
    m_bBalanceEnable = configParser_GetCfgItemIntValue(kszAccountBalanceEnable, iAccountId) != 0;

    if (configParser_GetConfigInt(kszGetBalanceEnable) == 0)
    {
        m_bBalanceEnable = false;
    }

    m_strPasswd = configParser_GetCfgItemStringValue(kszAccountPassword, iAccountId);

    m_strBalanceLink = configParser_GetCfgItemStringValue(kszAccountBalanceLink, iAccountId);
#endif

    if (acc_IsAccountEnable(m_iAccountId))
    {
        // 移植技术支持 http://10.2.1.199/Bug.php?BugID=6655
        // 如果账号名为空,则将账号关闭
        if (configParser_GetConfigInt(kszCorrectAccount) != 0)
        {
            yl::string strUserName = configParser_GetCfgItemStringValue(kszAccountUserName, m_iAccountId);
            if (strUserName.empty())
            {
                //向所有线程广播配置文件更改的消息
                //改变帐号启用属性，并保存，广播消息
                if (acc_SetAccountEnable(m_iAccountId, false))
                {
                    msgBroadpostThreadMsg(CONFIG_MSG_BCAST_CHANGED, ST_SIPLINE, m_iAccountId);
                }

                m_eAccountState = LS_DISABLE;
            }
        }
    }
    else
    {
        m_eAccountState = LS_DISABLE;
    }

#ifdef IF_SUPPORT_VIDEO
    LoadCodecInfo(iAccountId);
#endif //#ifdef IF_SUPPORT_VIDEO

    return true;
}

//帐号状态改变
bool CSipAccount::LineStateChange(LINE_STATE eState, account_register_reason_t & sReason,
                                  bool bGuiQuery/*=false*/)
{
    if (!acc_IsAccountEnable(m_iAccountId))
    {
        //LogoutBeforeReset不生效
        if (eState == LS_BE_UNREGISTERED)
        {
            SetAccountState(LS_DISABLE, bGuiQuery);
        }

        SetRegisterReason(sReason);
        //帐号未启用，不处理注册状态消息
        return false;
    }

    // 账号重新注册后需要重置与服务器同步的状态
    SetSyncSubscribeResult(false);

    //将Line状态重新设置
    SetAccountState(eState, bGuiQuery);

    SetRegisterReason(sReason);

    return true;
}

int CSipAccount::GetAutoAnswerTime() const
{
    int nAutoAnswerTime = 0;
    if (acc_IsAccountAutoAnswerEnable(m_iAccountId))
    {
        nAutoAnswerTime = configParser_GetCfgItemIntValue(kszAutoAnswerTimeout, m_iAccountId);
        //由秒转换为毫秒
        nAutoAnswerTime = chMAX(200, nAutoAnswerTime * 1000);
    }
    return nAutoAnswerTime;
}

CALL_AV_TYPE CSipAccount::GetDefaultCallMode() const
{
    CALL_AV_TYPE eDefCallMode = (CALL_AV_TYPE)configParser_GetCfgItemIntValue(
                                    kszAccountCallMode, m_iAccountId);
    if (eDefCallMode == CAVT_UNKNOWN)
    {
        eDefCallMode = CAVT_VIDEO;
    }
    return eDefCallMode;
}

//获取注册失败Id
int CSipAccount::GetRegisterFailId() const
{
    return m_iFailId;
}

//获取注册失败信息
yl::string CSipAccount::GetRegisterFailInfo() const
{
    return m_strFailInfo;
}

//获取配置路径
yl::string CSipAccount::GetConfigPath() const
{
    return m_strConfigPath;
}

//获取用户名
yl::string CSipAccount::GetUserName() const
{
    return configParser_GetCfgItemStringValue(kszAccountUserName, m_iAccountId);
}

//获取服务器名
yl::string CSipAccount::GetServerName() const
{
    return m_strServerName;
}

//设置服务器名
void CSipAccount::SetServerName(const char * lpszServerName)
{
    if (NULL == lpszServerName)
    {
        return;
    }
    m_strServerName = lpszServerName;
}

//获取显示名
yl::string CSipAccount::GetDisplayName() const
{
    yl::string strUserName = GetUserName();
    if (strUserName != "")
    {
        yl::string strDisplayName =
            configParser_GetCfgItemStringValue(kszAccountDisplayName, m_iAccountId);

        if (strDisplayName == "")
        {
            strDisplayName = strUserName;
        }
        return strDisplayName;
    }
    return "";
}

//获取服务器类型
SIP_SERVER_TYPE CSipAccount::GetServerType() const
{
    return m_eServerType;
}

//获取注册名
yl::string CSipAccount::GetRegisterName() const
{
    return configParser_GetCfgItemStringValue(kszAccountRegisterName, m_iAccountId);
}

#ifndef _T49
//获取未读语音信息数量
UINT CSipAccount::GetUnReadVoiceMailCount() const
{
    return m_uUnReadVoiceMailCount;
}

//设置未读语音信息数量
void CSipAccount::SetUnReadVoiceMailCount(UINT nCount)
{
    m_uUnReadVoiceMailCount = nCount;
}

//获取已读语音信息数量
UINT CSipAccount::GetReadedVoiceMailCount() const
{
    return m_uReadedVoiceMailCount;
}

//设置已读语音信息数量
void CSipAccount::SetReadedVoiceMailCount(UINT nCount)
{
    m_uReadedVoiceMailCount = nCount;
}
#endif

// 获取同步订阅是否成功
bool CSipAccount::GetSyncSubscribeResult() const
{
    return m_bSyncSubscribe;
}

// 设置同步订阅是否成功
void CSipAccount::SetSyncSubscribeResult(bool bSuccess)
{
    m_bSyncSubscribe = bSuccess;
}

#if defined(IF_FEATURE_BROADSOFT_CALL_CENTER) || defined(IF_FEATURE_GENESYS_ACD)
//获取账号ACD开关值
bool CSipAccount::GetACDSwitch() const
{
    return configParser_GetCfgItemIntValue(kszAccountEnableACD, m_iAccountId) == 1;
}

// 获取账号Hoteling的开关值
bool CSipAccount::GetHotelingSwitch() const
{
    return configParser_GetCfgItemIntValue(kszAccountEnableHoteling, m_iAccountId) == 1;
}

// 获取账号FlexibleSeating的开关值
bool CSipAccount::GetFlexibleSeatingSwitch() const
{
    return configParser_GetCfgItemIntValue(kszAccountEnableFlexibleSeating, m_iAccountId) == 1;
}
#endif

void CSipAccount::SetServerId(int iServerID)
{
    m_iServerID = iServerID;
}

int CSipAccount::GetServerID() const
{
    return m_iServerID;
}

//获取BLA类型
BLA_TYPE CSipAccount::GetBLAType() const
{
#if IF_BUG_39041
    if (!m_bIsSupportSCA)
    {
        return BLA_NONE;
    }

    BLA_TYPE eBLAType = (BLA_TYPE)configParser_GetCfgItemIntValue(kszBLAType, m_iAccountId);
    if (eBLAType == BLA_TELTRONNIC_BLA)
    {
        eBLAType = BLA_DRAFT_BLA;
    }
    else if (eBLAType == BLA_BROADSOFT_SCA
             && acc_GetServerType(m_iAccountId) == SST_GENBAND)
    {
        eBLAType = BLA_GENBAND;
    }

    return eBLAType;
#else
    return (BLA_TYPE)acc_GetBLAType(m_iAccountId);
#endif
}

#ifndef _T49
// 获取是否处理VoiceMail开关
bool CSipAccount::GetVoiceMailSwitch() const
{
    return configParser_GetCfgItemIntValue(kszAccountVMIndication, m_iAccountId) == 1;
}
#endif

// 获取服务器端口
int CSipAccount::GetServerPort() const
{
    return m_iServerPort;
}

// 重置服务器端口
void CSipAccount::ResetServerPort()
{
    m_iServerPort = configParser_GetCfgItemIntValue(kszAccountServerPort, m_iAccountId, m_iServerID);
}

void CSipAccount::SetTransPort(int nTransPort)
{
    m_nTransPort = nTransPort;
}

int CSipAccount::GetTransPort()
{
    return m_nTransPort;
}

#ifdef IF_SUPPORT_VIDEO
void CSipAccount::LoadCodecInfo(int iAccountId)
{
    for (int i = 0; i < 4; i++)
    {
        yl::string strCodecType = configParser_GetCfgItemStringValue(kszAccountVideoCodecType, iAccountId,
                                  i);
        if (strCodecType.compare("H264") == 0
                && configParser_GetCfgItemIntValue(kszAccountVideoCodecEnable, iAccountId, i) == 1)
        {
            m_bSupportH264 = true;
            return;
        }
    }

    m_bSupportH264 = false;
    return;
}
#endif //#ifdef IF_SUPPORT_VIDEO

// 是否是特殊的云账号
bool CSipAccount::IsSpecialCloudAccount()
{
    SipAccountType eType = (SipAccountType)configParser_GetCfgItemIntValue(kszAccountType,
                           m_iAccountId);
    if (eType == ACCOUNT_ZOOM
            || eType == ACCOUNT_MIND
            || eType == ACCOUNT_BLUEJEANS)
    {
        return true;
    }

    return false;
}

// 判断是否需要注册
bool CSipAccount::NeedRegister()
{
    return !IsSpecialCloudAccount()
           && !configParser_GetCfgItemIntValue(kszAccountDirectIP, m_iAccountId);
}

bool CSipAccount::GetAutoAnswerSwitch() const
{
    if (ACCOUNT_TYPE_IP_DIRECT == GetAccountType())
    {
        return configParser_GetConfigInt(kszIPCallAutoAnswerMuteSwitch) != 0;
    }

    return configParser_GetCfgItemIntValue(kszAutoAnswerSwitch, m_iAccountId) != 0;
}

bool CSipAccount::SetAutoAnswerSwitch(bool bEnable, CONFIG_LEVEL_TYPE eType) const
{
    if (ACCOUNT_TYPE_IP_DIRECT == GetAccountType())
    {
        return configParser_SetConfigInt(kszIPCallAutoAnswerMuteSwitch, bEnable, eType);
    }

    return configParser_SetCfgItemIntValue(kszAutoAnswerSwitch, m_iAccountId, bEnable, eType);
}

bool CSipAccount::SetServerByServerID(int iServerID)
{
    if (iServerID > SIPSERVER_NUM)
    {
        ACCOUNT_INFO("ServerID not Exist");
        return false;
    }

    bool bServerNameChange = false;
    // 重新初始化Server
    if (iServerID <= 0)
    {
        yl::string strOldServer = GetServerName();
        InitServerName();
        bServerNameChange = strOldServer != GetServerName();
    }
    else if (iServerID != m_iServerID)
    {
        yl::string strReplaceServer = configParser_GetCfgItemStringValue(kszAccountServerAddr, m_iAccountId,
                                      iServerID);
        bServerNameChange = strReplaceServer != GetServerName();
        // 更改服务器地址
        ACCOUNT_INFO("Change Account[%d] SIP Server to %s", m_iAccountId, strReplaceServer.c_str());
        SetServerName(strReplaceServer.c_str());
        SetServerId(iServerID);
        ResetServerPort();
    }

    if (bServerNameChange)
    {
        etl_NotifyApp(FALSE, ACCOUNT_CONFIG_CHANGE, m_iAccountId, 0);
    }

    return true;
}
