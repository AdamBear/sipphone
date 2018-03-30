#include "zeroprocessor.h"
#include "autopcontrolmanager.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "adapterbox/include/modadapterbox.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "../include/autopcontrollmsgdefine.h"
#include "../include/modautopcontrol.h"
#include "devicelib/phonedevice.h"
#include "path_define.h"
#include "commonapi/stringhelperapi.h"
#include "commonapi/common_api_enumtypes.h"
#include "devicelib/networkserver.h"

#if IF_FEATURE_METASWITCH
#include "service_config.h"
#endif

#if IF_BUG_30885
bool CZeroProcessor::m_bFirstStart;
#endif //IF_BUG_30885

CZeroProcessor::CZeroProcessor()
    : CBaseAutopProcessor(AUT_ZERO)
{
#if IF_BUG_30885
    m_bAddAuthorized = false;
    m_bAutoPWithOption66 = false;
    m_strOption66URL = "";
    m_bConnectWithCfg = true;
    m_uConnectInterval = 3 * 1000;
    m_uMaxConnectTimes = 3;
    m_bFirstStart = true;
    m_bAddCfgFlag = false;
#endif //IF_BUG_30885
}

#if IF_FEATURE_METASWITCH
CZeroProcessor::CZeroProcessor(AUTOP_UPDATE_TYPE eType)
    : CBaseAutopProcessor(eType)
{
#if IF_BUG_30885
    m_bAddAuthorized = false;
    m_bAutoPWithOption66 = false;
    m_strOption66URL = "";
    m_bConnectWithCfg = true;
    m_uConnectInterval = 3 * 1000;
    m_uMaxConnectTimes = 3;
    m_bFirstStart = true;
    m_bAddCfgFlag = false;
#endif //IF_BUG_30885
}
#endif

CZeroProcessor::~CZeroProcessor()
{

}

bool CZeroProcessor::ProcessConnect()
{
    return ReadyToConnect();
}

bool CZeroProcessor::ReadyToConnect()
{
    yl::string strURL = configParser_GetConfigString(kszAutoProvisionUrl);

    //URL中存在，则使用URL的
    if (!IsIncludeAuthInfo(strURL))
    {
        // 用户输入的用户名密码优先
        // strURL = DeleteAuthInfoFromUrl(strURL);
        strURL = AddAuthorizedInfo(strURL, GetUserName(), GetPasswd());
    }
    else
    {
        //TODO.
        //url 里面有数据，需要解析出来
        //ParseUserName();
    }

    return SetUpAutop(strURL);
}

bool CZeroProcessor::ProcessRedirectMsg(msgObject & objMsg)
{
    bool bHandle = true;
    switch (objMsg.message)
    {
    case ATP_MSG_NOTIFY_AUTOP_END:
        {
            ZERO_INFO("redirect done result = %d, reboot = %d   [%d]", objMsg.wParam, objMsg.lParam,
                      IsProcessEndMsg());


#if IF_FEATURE_METASWITCH
            if (1 == objMsg.wParam
                    && GetType() == AUT_MTSW_HTTP_DIGEST)
            {
                //configParser_SetConfigInt(kszMTSWIsLogined, 1);
                etl_NotifyApp(FALSE, CONFIG_MSG_BCAST_CHANGED, ST_PROGRAMABLE_KEY, 0);
            }
#endif

#if IF_BUG_30885
            if (1 == objMsg.lParam)
            {
                // 如果需要重启，则直接进入rebooting界面
                AdapterBox_ZeroOpenDlg(1 == objMsg.wParam ? ZERO_DLG_REBOOTING : ZERO_DLG_UPDATEFAIL);

                // DHCP Option 66更新完后的处理
                if (GetAutoPWithOption66())
                {
                    Autop_SetAutopNow(true, 1);
                }
            }
            else
#endif
                if (IsProcessEndMsg())
                {
                    if (_AutopControlMamager.IsNeedShowUpdatingWnd())
                    {
                        ShowUpdatingWnd(1 == objMsg.wParam ? ZERO_DLG_UPDATEFINISH : ZERO_DLG_UPDATEFAIL);
                    }

                    // 需要立即置为不处理end 消息
                    SetProcessEndStatus(false);
                    // http://bugfree.yealink.com/Bug.php?BugID=108618

                    // 需要给外部再处理一次
                    bHandle = false;
                    etl_NotifyApp(FALSE, AUTOP_AUTOP_FINISH, objMsg.wParam, objMsg.lParam);
                }
        }
        break;
    case ATP_MSG_NOTIFY_AUTH_ERR:
        {
#if IF_FEATURE_METASWITCH
            if (configParser_GetConfigInt(kszMTSWLoginMode) == 1
                    && GetType() == AUT_MTSW_HTTP_DIGEST)
            {
                configParser_SetConfigString(kszAutoProvisionPassword, "");
                AdapterBox_ZeroOpenDlg(ZERO_DLG_AUTHFAIL_PAGE);
                SetProcessEndStatus(false);
                SetAuthStatus(true);
            }
            else
#endif //IF_FEATURE_METASWITCH
            {
                SetProcessEndStatus(false);
                ProcessAuth();
            }

        }
        break;
    case ATP_MSG_NOTIFY_EXIT_AUTH:
        {
            bHandle = false;
        }
        break;
    default:
        bHandle = true;
        break;
    }

    return bHandle;
}

void CZeroProcessor::ShowAuthWnd()
{
    AdapterBox_ZeroOpenDlg(ZERO_DLG_ZERO_AUTHENTICATION);
}

#if IF_BUG_30885
bool CZeroProcessor::OnTaskActionCallback(CNetworkFileAction * pTaskAction)
{
    if (NULL == pTaskAction)
    {
        return false;
    }

    yl::string strUrl;
    pTaskAction->GetRedirectURL(strUrl);
    if (!strUrl.empty())
    {
        SetUrl(strUrl);
    }

    switch (pTaskAction->GetTaskActionErrorType())
    {
    case TA_NO_ERROR:
        {
            m_bAddAuthorized = false;

            if (m_bAutoPWithOption66)
            {
                _AutopControlMamager.ConnectServer(m_strOption66URL, GetType());
            }
            else
            {
                // 触发更新
                SetAuthInfo(configParser_GetConfigString(kszAutoProvisionUsername),
                            configParser_GetConfigString(kszAutoProvisionPassword));

                ReadyToConnect();
            }
        }
        break;
    case TA_HTTP_DOWNLOAD_UNAUTHORIZED:
        {
            ZERO_INFO("CTelstraAutopControl::OnTaskActionCallback add=[%d] first=[%d] 66=[%d]",
                      m_bAddAuthorized, m_bFirstStart, m_bAutoPWithOption66);

            if (!m_bAddAuthorized)
            {
                // 第二次加上现有用户名密码进行鉴权
                if (m_bAutoPWithOption66)
                {
                    ProcessOption66Connect(true);
                }
                else
                {
                    ProcessDMSConnect(true);
                }
            }
            else
            {
                // 弹出认证界面
                if (!configParser_GetConfigString(kszAutoProvisionUsername).empty()
                        || !m_bFirstStart)
                {
                    AdapterBox_ZeroOpenDlg(ZERO_DLG_UNAUTHORIZED);
                }
                else
                {
                    AdapterBox_ZeroOpenDlg(ZERO_DLG_AUTOPCONFIG);
                }

                m_bFirstStart = false;
            }
        }
        break;
    default:
        {
            UINT nConnectTimes = GetConnectTimes();
            ++nConnectTimes;
            SetConnectTimes(nConnectTimes);
            ZERO_INFO("retry connect times = %d", nConnectTimes);
            // 连接失败,则重复连接CONNECT_TIMES次
            if (nConnectTimes < m_uMaxConnectTimes)
            {
                // 延迟几秒再连接,可以等待网络准备好
                etlSleep(m_uConnectInterval);

                if (m_bAutoPWithOption66)
                {
                    ProcessOption66Connect();
                }
                else
                {
                    BOOL bCurConnectCfg = GetConnectWithCfg();
                    SetConnectWithCfg(!bCurConnectCfg);
                    ProcessDMSConnect(m_bAddAuthorized);
                }
            }
            else if (m_bAutoPWithOption66)
            {
                ProcessDMSConnect(false);
            }
            else
            {
                m_bAddAuthorized = false;
                AdapterBox_ZeroOpenDlg(ZERO_DLG_UPDATEFAIL);
            }
        }
        break;
    }

    return true;
}

yl::string CZeroProcessor::GetDHCPOption66Url()
{
    yl::string strUrl = etlGetRegistryString(szDHCPAdd, "DHCP", "66", "");
    if (strUrl.empty())
    {
        return "";
    }

    const char * szValue = (LPCSTR) strUrl.c_str();
    int iLen = strlen(szValue);
    int iPos = 0;
    yl::string strRet;

    unsigned int nTmp = 0;

    for (; iPos < iLen / 2; iPos++)
    {
        sscanf(szValue + 2 * iPos, "%2x", &nTmp);
        strRet.push_back(nTmp);
    }

    if (nTmp == 0
            && strRet.size() > 0)
    {
        strRet = strRet.substr(0, strRet.size() - 1);
    }

    if (!strRet.empty()
            && strRet.find("://") == yl::string::npos)
    {
        strRet.insert(0, "tftp://");
    }

    return strRet;
}

bool CZeroProcessor::ProcessOption66Connect(bool bAddAuth/* = false*/)
{
    yl::string strServer = GetDHCPOption66Url();
    if (strServer.empty()
            || configParser_GetConfigInt(kszEnableDHCPOption66) == 0)
    {
        return false;
    }

    PacketCfg(strServer, GetConfigFileByPhone());

    m_bAutoPWithOption66 = true;
    m_strOption66URL = strServer;

    // 如果之前有加过，则还需添加
    if (bAddAuth
            || m_bAddAuthorized)
    {
        m_bAddAuthorized = true;
        yl::string strUserName = configParser_GetConfigString(kszAutoProvisionUsername);
        yl::string strPassword = configParser_GetConfigString(kszAutoProvisionPassword);
        if (IsIncludeAuthInfo(strServer))
        {
            strServer = DeleteAuthInfoFromUrl(strServer);
        }

        strServer = AddAuthorizedInfo(strServer, strUserName, strPassword);
    }

    ZERO_INFO("Start Autop With Option 66. Option66URL = %s", m_strOption66URL.c_str());

    return _AutopControlMamager.ConnectServer(strServer.c_str(), GetType());
}

void CZeroProcessor::PacketCfg(yl::string & strServer, const yl::string & strCfgFile)
{
    yl::string strBakServer = strServer;

    // 重定向，需要加上y000000000.cfg进行连接，解决某些服务器拒绝访问目录而返回403，导致连接失败，所以此处作特殊处理
    // 手动加上文件名进行连接,如果y00000000000.cfg连接失败,则尝试连接mac.cfg,只要一个连接成功就可以,
    // 当连接成功后,传给autop进行更新的地址,需将该文件名去掉,否则autop只会更新这个文件
    // 判断是否是文件,最后四位是否是.cfg
    // 参数bNeedAddCfg说明:由于存在URL1(我们提供的固定的地址)和URL2(用户配置的重定向地址),而URL1不能加上cfg文件名
    if ((m_bAddCfgFlag ||
            (strServer.length() >= 4 && strServer.substr(strServer.length() - 4) != ".cfg")))
    {
        // 如果之前有加过,则需要将加过去掉
        // 如之前访问了y000.cfg,mac.cfg都没有成功,当再次尝试连接的时候,该函数的地址将会带mac.cfg,
        // 所以需要用m_bAddCfgFlag来判断,先去掉mac.cfg
        if (m_bAddCfgFlag && strServer.length() >= 4 && strServer.substr(strServer.length() - 4) == ".cfg")
        {
            char * pTemp = strrchr((char *) strServer.c_str(), '/');
            if (NULL != pTemp)
            {
                *pTemp = '\0';
            }
        }
        m_bAddCfgFlag = true;
        if (!strServer.empty()
                && strServer[strServer.length() - 1] != '/')
        {
            strServer += "/";
        }

        strServer += strCfgFile;
    }
}

yl::string CZeroProcessor::GetConfigFileByPhone()
{
    // M7要求从配置文件中读取
    yl::string strConfigName = configParser_GetConfigString(kszAutoProvisionCommonName);

    // 如果是空的,则从出厂中读取
    if (strConfigName.empty())
    {
        strConfigName = configParser_GetConfigString(kszAutopFacCommonFileName);
    }

    if (!strConfigName.empty())
    {
        return strConfigName;
    }

    devicelib_GetPhoneType();
    switch (g_iPhoneType)
    {
    case 28:
    default:
        strConfigName = "y000000000000.cfg";
        break;
    }
    return strConfigName;
}

bool CZeroProcessor::ProcessDMSConnect(bool bAddAuth /* = false */)
{
    m_bAutoPWithOption66 = false;
    m_strOption66URL = "";

    yl::string strServer = configParser_GetConfigString(kszAutoProvisionUrl);

    if (strServer.empty())
    {
        AdapterBox_ZeroOpenDlg(ZERO_DLG_UPDATEFAIL);
        return false;
    }

    // 如果之前有加过，则还需添加
    if (bAddAuth
            || m_bAddAuthorized)
    {
        m_bAddAuthorized = true;
        yl::string strUserName = configParser_GetConfigString(kszAutoProvisionUsername);
        yl::string strPassword = configParser_GetConfigString(kszAutoProvisionPassword);
        if (IsIncludeAuthInfo(strServer))
        {
            strServer = DeleteAuthInfoFromUrl(strServer);
        }
        AddAuthorizedInfo(strServer, strUserName, strPassword);
    }

    ZERO_INFO("Start Autop With DMS. URL = %s", strServer.c_str());

    return GetConnectWithCfg() ? ConnectWithCfg(strServer.c_str()) : ConnectWithMac(strServer.c_str());
}

bool CZeroProcessor::ConnectWithCfg(const char * pURL)
{
    if (NULL == pURL
            || 0 == strlen(pURL))
    {
        return false;
    }

    m_bConnectWithCfg = true;
    yl::string strServer = pURL;

    PacketCfg(strServer, GetConfigFileByPhone());

    ZERO_INFO("now try connect cfg config");

    return _AutopControlMamager.ConnectServer(strServer, GetType());
}

bool CZeroProcessor::ConnectWithMac(const char * pURL)
{
    if (NULL == pURL
            || 0 == strlen(pURL))
    {
        return false;
    }

    m_bConnectWithCfg = false;
    yl::string strServer = pURL;
    PacketCfg(strServer, GetConfigMacFile());

    ZERO_INFO("now try connect mac config");
    return _AutopControlMamager.ConnectServer(strServer, GetType());
}

yl::string CZeroProcessor::GetConfigMacFile()
{
    // 获取mac地址,当作文件名
    yl::string strMac = netGetMACText('\0');
    // http://10.3.5.199/Bug.php?BugID=61974
    strMac = commonAPI_StringConvert(strMac, STRING_CONVERT_TO_LOWER);
    strMac += ".cfg";
    return strMac;
}

#endif //IF_BUG_30885
