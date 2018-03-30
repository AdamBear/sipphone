#include "modautopcontrol.h"
#include "autopcontrolmanager.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include <interfacedefine.h>

bool Autop_Init()
{
    _AutopControlMamager.Init();
    return true;
}

//开始RPS
bool Autop_Start(AUTOP_UPDATE_TYPE eAutopType)
{
    return _AutopControlMamager.Start(eAutopType);
}

void Autop_StartByZero(bool bStartByZero)
{
    _AutopControlMamager.SetStartByZero(bStartByZero);
    _AutopControlMamager.Start(AUT_ZERO);
}

bool Autop_IsRunByZero()
{
    return _AutopControlMamager.IsStartByZero();
}

void Autop_SetAutopNow(bool bAutopNow, int iValue /*=-1*/)
{
    _AutopControlMamager.SetAutopNow(bAutopNow);

    if (iValue != -1)
    {
        configParser_SetConfigInt(kszAutopNow, iValue, CONFIG_LEVEL_PHONE);
    }
}

bool Autop_IsAutopNow()
{
    return _AutopControlMamager.IsAutopNow();
}

AUTOP_UPDATE_TYPE Autop_GetUpdateType()
{
    return _AutopControlMamager.GetUpdateType();
}

//开始autop 连接流程
bool Autop_StartRPSConnect()
{
    yl::string strUserName = configParser_GetConfigString(kszRedirectUser);
    yl::string strPasswd;
    if (!strUserName.empty())
    {
        yl::string strPasswd = configParser_GetConfigString(kszRedirectPassword);
    }
    else
    {
        strUserName = configParser_GetConfigString(kszAutoProvisionUsername);
        strPasswd = configParser_GetConfigString(kszAutoProvisionPassword);
    }

    return _AutopControlMamager.ProcessStartConnect(AUT_RPS, strUserName, strPasswd);
}

bool Autop_StartZeroConnect()
{
    yl::string strName = configParser_GetConfigString(kszAutoProvisionUsername);
    yl::string strPwd = configParser_GetConfigString(kszAutoProvisionPassword);

    return _AutopControlMamager.ProcessStartConnect(AUT_ZERO, strName, strPwd);
}

bool AutopStartCodeConnect(const yl::string & strName/* = ""*/, const yl::string & strPwd/* = ""*/)
{
    Autop_Start(AUT_CODE);
    return _AutopControlMamager.ProcessStartConnect(AUT_CODE, strName, strPwd);
}

bool Autop_StartConnect(AUTOP_UPDATE_TYPE eAutopType, const yl::string & strName /* = "" */,
                        const yl::string & strPwd /* = "" */)
{
    if (AUT_CODE == eAutopType)
    {
        Autop_Start(AUT_CODE);
    }

    return _AutopControlMamager.ProcessStartConnect(eAutopType, strName, strPwd);
}

bool Autop_Exit()
{
    _AutopControlMamager.Exit();

    return true;
}

bool Autop_IsUpdating(bool bSync/* = false*/)
{
    //同步的方式，直接向autop查询，autop保证不会阻塞
    if (bSync)
    {
        return  msgSendMsgToThreadTimeoutEx(msgGetThreadByName(VP_AP_THREAD_NAME), ATP_MSG_REQ_GET_POWER_ON_STATUS, 0, 0, 0, NULL, 200) != AUTOP_POWER_ON_END;
    }

    return AUT_NONE != _AutopControlMamager.GetUpdateType();
}

bool Autop_IsNeedShowUpdating()
{
    return _AutopControlMamager.IsNeedShowUpdatingWnd();
}

bool Autop_IsRpsEnable()
{
    return 1 == configParser_GetConfigInt(kszRedirectEnable);
}

POWER_ON_STATUS Autop_GetAutopStatus()
{
    return (POWER_ON_STATUS) _AutopControlMamager.GetAutopStatus();
}

bool Autop_WaitingForNetwork()
{
    return _AutopControlMamager.WaitingForNetwork();
}

bool Autop_CodeProcessing()
{
    return _AutopControlMamager.GetUpdateType() == AUT_CODE;
}

int Autop_MatchAutoPCodePrefix(const yl::string & strNum, char cKey/* = 0*/)
{
    if (!_AutopControlMamager.IsAutopCodeUsable())
    {
        return -1;
    }

    return _AutopControlData.MatchAutoPCodePrefix(strNum, cKey);
}

bool Autop_SetAuthInfo(const yl::string & strUserName, const yl::string & strPasswd,
                       bool bSaveConfig/* = true*/)
{
    _AutopControlMamager.SetPassword(strPasswd);
    _AutopControlMamager.SetUserName(strUserName);

    if (bSaveConfig)
    {
        configParser_SetConfigString(kszAutoProvisionUsername, strUserName.c_str(), CONFIG_LEVEL_PHONE);
        configParser_SetConfigString(kszAutoProvisionPassword, strPasswd.c_str(), CONFIG_LEVEL_PHONE);

//#warning V82
        ZERO_INFO("Autop_SetAuthInfo [%s] [%s]", strUserName.c_str(), strPasswd.c_str());
    }

    return true;
}

yl::string Autop_GetCurrentUserName()
{
    return _AutopControlMamager.GetUserName();
}

yl::string Autop_GetCurrentPassword()
{
    return _AutopControlMamager.GetPasswd();
}

yl::string Autop_AutopCodeGetCurrentName()
{
    const LPAUTOP_PARAM lpParam = _AutopControlData.GetMatchCodeInfo();

    if (NULL == lpParam
            || NULL == lpParam->lpAutoPData)
    {
        return "";
    }

    return lpParam->name;
}

#if IF_BUG_30885
bool Autop_StartOption66Connect()
{
    if (NULL == Autop_GetUpdateType())
    {
        Autop_Start(AUT_TELSTRA_OPTION66);
    }

    yl::string strName = configParser_GetConfigString(kszAutoProvisionUsername);
    yl::string strPwd = configParser_GetConfigString(kszAutoProvisionPassword);
    return _AutopControlMamager.ProcessStartConnect(AUT_TELSTRA_OPTION66, strName, strPwd);
}

bool Autop_StartDMSConnect()
{
    if (NULL == Autop_GetUpdateType())
    {
        Autop_Start(AUT_TELSTRA_DMS);
    }

    yl::string strName = configParser_GetConfigString(kszAutoProvisionUsername);
    yl::string strPwd = configParser_GetConfigString(kszAutoProvisionPassword);
    return _AutopControlMamager.ProcessStartConnect(AUT_TELSTRA_DMS, strName, strPwd);
}
#endif //IF_BUG_30885

#if IF_FEATURE_METASWITCH
bool Autop_StartHttpDigestTConnect()
{
    yl::string strName = configParser_GetConfigString(kszAutoProvisionUsername);
    yl::string strPwd = configParser_GetConfigString(kszAutoProvisionPassword);
    return _AutopControlMamager.ProcessStartConnect(AUT_MTSW_HTTP_DIGEST, strName, strPwd);
}
#endif

bool Autop_IsLaunchRPS()
{
    if (1 != configParser_GetConfigInt(kszRedirectEnable))
    {
        return false;
    }

    yl::string strURL = configParser_GetConfigString(kszAutoProvisionUrl);
    if (1 == configParser_GetConfigInt(kszRPSPriority)
            || strURL.empty())
    {
        return true;
    }

    return false;
}

bool Autop_NotifyAutoServer(bool bSync, UINT uMesg, WPARAM wParam, LPARAM lParam)
{
    if (bSync)
    {
        return msgSendMsgToThread(msgGetThreadByName(VP_AP_THREAD_NAME), uMesg, wParam, lParam);
    }
    else
    {
        return msgPostMsgToThread(msgGetThreadByName(VP_AP_THREAD_NAME), uMesg, wParam, lParam);
    }
}

bool Autop_NotifyAutoServerEx(bool bSync, UINT uMesg, WPARAM wParam, LPARAM lParam, int iSize,
                              LPCVOID pExtraData)
{
    if (bSync)
    {
        return msgSendMsgToThreadEx(msgGetThreadByName(VP_AP_THREAD_NAME), uMesg, wParam, lParam, iSize,
                                    pExtraData);
    }
    else
    {
        return msgPostMsgToThreadEx(msgGetThreadByName(VP_AP_THREAD_NAME), uMesg, wParam, lParam, iSize,
                                    pExtraData);
    }
}
