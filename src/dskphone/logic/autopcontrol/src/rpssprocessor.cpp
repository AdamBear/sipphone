#include "rpssprocessor.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "adapterbox/include/modadapterbox.h"
#include "commonapi/stringhelperapi.h"
#include "devicelib/networkserver.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "../include/autopcontrollmsgdefine.h"
#include "autopcontrolmanager.h"
#include "lamp/powerled/include/modpowerledlogic.h"

CRpsProcessor::CRpsProcessor()
    : CBaseAutopProcessor(AUT_RPS)
{
    m_uMaxConnectTimes = 3;
    m_uConnectTimes = 0;

    //读取配置
    InitConfig();
}
CRpsProcessor::~CRpsProcessor()
{

}

void CRpsProcessor::InitConfig()
{
}

bool CRpsProcessor::ExitProcessor()
{
    CancelUpdateTimeout();
    return CBaseAutopProcessor::ExitProcessor();
}

//void CRpsProcessor::ShowAuthWnd()
//{
//  AdapterBox_ZeroOpenDlg(ZERO_DLG_REDIRECTOR_AUTHENTICATION);
//}

bool CRpsProcessor::OnTimer(UINT uTimerID)
{
    if (uTimerID == (UINT) &m_iUpdateTimeout)
    {
        ZERO_INFO("AutopControl On Time out [%d]", m_iUpdateTimeout);

        CancelUpdateTimeout();

        AdapterBox_ZeroOpenDlg(ZERO_DLG_UPDATE_TIME_OUT);
        return true;
    }
    return false;
}

bool CRpsProcessor::OnTaskActionCallback(CNetworkFileAction * pTaskAction)
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
    case TA_HTTP_DOWNLOAD_UNAUTHORIZED:
        // 流程走不到这里，不处理
        break;
    case TA_NO_ERROR:
    case TA_FILE_NO_DATA_WHEN_READ:
        // 触发更新
        ReadyToConnect();
        break;
    case TA_URL_REDIRECT:
        //autop 表示不会走到这个流程
        {
            if (!strUrl.empty())
            {
                yl::string strUrlWithoutAuth, strUser, strPasswd;
                if (ParseUserName(strUrl, strUrlWithoutAuth, strUser, strPasswd))
                {
                    SetUrl(strUrl);
                    SetAuthInfo(strUser, strPasswd);
                }
            }

            //if(IsSaveInfoToAutopOn())
            //{
            //  configParser_SetConfigString(kszAutoProvisionUrl, GetUrl().c_str(), CONFIG_LEVEL_PHONE);
            //  if(!GetUserName().empty())
            //  {
            //      configParser_SetConfigString(kszAutoProvisionUsername, GetUserName().c_str(), CONFIG_LEVEL_PHONE);
            //  }
            //  if(!GetPasswd().empty())
            //  {
            //      configParser_SetConfigString(kszAutoProvisionPassword, GetPasswd().c_str(), CONFIG_LEVEL_PHONE);
            //  }
            //}

            ProcessConnect();
        }
        break;
    default:
        //处理重连机制
        UINT uConnectTimes = GetConnectTimes();
        ++uConnectTimes;
        SetConnectTimes(uConnectTimes);
        ZERO_INFO("retry connect times = %d", uConnectTimes);
        // 连接失败,则重复连接CONNECT_TIMES次
        if (uConnectTimes < m_uMaxConnectTimes)
        {
            ProcessConnect();
        }
        else
        {
            // 弹出连接失败提示
            if (_AutopControlMamager.IsNeedShowUpdatingWnd())
            {
                ShowUpdatingWnd(ZERO_DLG_UPDATEFAIL);
            }
        }
        break;
    }
    return true;
}

bool CRpsProcessor::ReadyToConnect()
{
    yl::string strUrl = GetUrl();

    //是否需要加入鉴权信息
    if (IsAuthSatus())
    {
        if (IsIncludeAuthInfo(strUrl))
        {
            strUrl = DeleteAuthInfoFromUrl(strUrl);
        }
        strUrl = AddAuthorizedInfo(strUrl, GetUserName(), GetPasswd());
    }

    return SetUpAutop(strUrl);
}

bool CRpsProcessor::ProcessConnect()
{
    //开机自动进入RPS界面
    //if (!GetUrl().empty())
    {
        AdapterBox_ZeroOpenDlg(ZERO_DLG_READY_REDIRECT);
    }

    bool bResult = false;
    // 第一次连接，需要从配置中读取
    if (GetUrl().empty())
    {
        // 取服务器地址
        yl::string strServer = configParser_GetConfigString(kszRedirectServer);
        yl::string strMac = netGetMACText('-');

        // 服务器地址要带上mac
        if (strServer.empty())
        {
            strServer = commonAPI_FormatString("https://%s/service.do?method=redirect&mac=%s",
                                               "rps.yealink.com", strMac.c_str());
        }
        else
        {
            // 由于给客户的地址是https://rps.yealink.com, 而实际要连接的https://rps.yealink.com/service.do,所以这里加上service.do.
            strServer = strServer + "/service.do" + "?method=redirect&mac=" + strMac;
        }
        m_strOldUrl = strServer;

        bResult = _AutopControlMamager.ConnectServer(strServer, GetType());
    }
    else
    {
        if (m_strOldUrl == GetUrl())
        {
            bResult = _AutopControlMamager.ConnectServer(m_strOldUrl, GetType());
        }
        else
        {
            bResult = ReadyToConnect();
        }
    }

    //设置更新超时
    SetUpdateTimeout();

    return bResult;
}

bool CRpsProcessor::ProcessRedirectMsg(msgObject & objMsg)
{
    bool bHandle = true;
    switch (objMsg.message)
    {
    case ATP_MSG_NOTIFY_AUTOP_END:
        {
            char * pIgnore = (char *) objMsg.GetExtraData();
            int iIgnore = 0;
            if (NULL != pIgnore
                    && strlen(pIgnore) > 0)
            {
                iIgnore = atoi(pIgnore);
            }
            ZERO_INFO("ExtraData [%d]\n", iIgnore);
            //如果是鉴权失败之后的end消息，不进行处理
            if (1 == iIgnore)
            {
                ZERO_INFO("ATP_MSG_NOTIFY_AUTOP_END Be Ignored !");
                return true;
            }

            if (IsProcessEndMsg())
            {
                if (_AutopControlMamager.IsNeedShowUpdatingWnd())
                {
                    ShowUpdatingWnd(1 == objMsg.wParam ? ZERO_DLG_UPDATEFINISH : ZERO_DLG_UPDATEFAIL);
                }

                // 需要立即置为不处理end 消息
                // http://bugfree.yealink.com/Bug.php?BugID=108618
                SetProcessEndStatus(false);
                powerled_FlushPowerLed();
                //bHandle = false;
            }

            if (1 == objMsg.wParam) //更新成功
            {
                configParser_SetConfigInt(kszRedirectEnable, 0);

                if (IsAuthSatus())
                {
                    // 保存用户名密码到临时
                    configParser_SetConfigString(kszRedirectUser, GetUserName().c_str(), CONFIG_LEVEL_PHONE);
                    configParser_SetConfigString(kszRedirectPassword, GetPasswd().c_str(), CONFIG_LEVEL_PHONE);
                }
            }
            CancelUpdateTimeout();
            etl_NotifyApp(FALSE, AUTOP_AUTOP_FINISH, objMsg.wParam, objMsg.lParam);
        }
        break;
    case ATP_MSG_NOTIFY_AUTH_ERR:
        {
            SetProcessEndStatus(false);
            ProcessAuth();
            CancelUpdateTimeout();
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

bool CRpsProcessor::ParseUserName(const yl::string & strOriginalUrl, yl::string & strUrl,
                                  yl::string & strUserName, yl::string & strPasswd)
{
    if (strOriginalUrl.empty())
    {
        return false;
    }

    char bufType[200] = {0};
    char bufUserName[200] = {0};
    char bufPasswd[200] = {0};
    char bufURL[200] = {0};

    // 解析用户名和密码
    if (4 == sscanf(strOriginalUrl.c_str(), "%[^:]://%[^:]:%[^@]@%s", bufType, bufUserName, bufPasswd,
                    bufURL))
    {
        strUrl = bufType;
        strUrl += "://";
        strUrl += bufURL;

        strUserName = bufUserName;
        strPasswd = bufPasswd;
    }

    return true;
}
