#include "baseautopprocessor.h"
#include "adapterbox/include/modadapterbox.h"
#include "autopcontrolmanager.h"
#include "lamp/powerled/include/modpowerledlogic.h"
#include "interfacedefine.h"
#include "service_dsk_define.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "commonunits/modcommonunits.h"
#include "lamp/powersaving/include/powersavingmsgdefine.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "../include/modautopcontrol.h"
#include "idlescreen/include/modidlescreen.h"
#include "../include/autopcontrollmsgdefine.h"
#include "lamp/backlight/include/modbacklight.h"

CBaseAutopProcessor::CBaseAutopProcessor(AUTOP_UPDATE_TYPE eAutopType)
{
    m_eAutopType = eAutopType;
    m_strUrl = "";
    m_bProcessEnd = false;
    m_bAuthStatus = false;
    // 更新超时定时，默认1分钟
    m_iUpdateTimeout = configParser_GetConfigInt(kszAutopUpdateTimeout);
}

CBaseAutopProcessor::~CBaseAutopProcessor()
{
    ZERO_INFO("autop [%d] process end", GetType());
}

void CBaseAutopProcessor::SetUpdateTimeout()
{
    timerSetThreadTimer((UINT) &m_iUpdateTimeout, m_iUpdateTimeout);
}

void CBaseAutopProcessor::CancelUpdateTimeout()
{
    timerKillThreadTimer((UINT) &m_iUpdateTimeout);
}

bool CBaseAutopProcessor::ReplyAuthInfo()
{
    yl::string strAuthInfo = commonAPI_FormatString("%s:%s", GetUserName().c_str(),
                             GetPasswd().c_str());

    ZERO_DEBUG("reply auth info [%s] size[%d]", strAuthInfo.c_str(), strAuthInfo.size());

    return msgPostMsgToThreadEx(msgGetThreadByName(VP_AP_THREAD_NAME), ATP_MSG_REQ_RETURN_AUTH_INFO,
                                0, 0, strAuthInfo.size() + 1, strAuthInfo.c_str());
}

bool CBaseAutopProcessor::SetUpAutop(const yl::string & strOriginalUrl)
{
    if (strOriginalUrl.empty())
    {
        return false;
    }

    yl::string strUrl = strOriginalUrl;

    if (Autop_IsNeedShowUpdating())
    {
        ShowUpdatingWnd(ZERO_DLG_UPDATEING);
    }

    ZERO_DEBUG("ready to connect[%s] authstatus[%d]", strUrl.c_str(), IsAuthSatus());

    SetProcessEndStatus(true);

    if (IsAuthSatus()) //输入鉴权信息时，发消息
    {
        return ReplyAuthInfo();
    }
    else
    {
        //特殊字符编码处理 BugID=76246
        commonUnits_EncodeAutopURI(strUrl);

        yl::string strCmd(BOOT_PATH"bin/");
        strCmd.append("autoServer.exx --url ");
        strCmd.append(strUrl.c_str());
        strCmd.append(" &");

        commonUnits_System(strCmd.c_str());
        return true;
    }
}

void CBaseAutopProcessor::ShowUpdatingWnd(ZeroSpDlgType eWndType)
{
    AdapterBox_ZeroOpenDlg(eWndType);
}

void CBaseAutopProcessor::ShowAuthWnd()
{
    AdapterBox_ZeroOpenDlg(ZERO_DLG_REDIRECTOR_AUTHENTICATION);
}

bool CBaseAutopProcessor::IsNeedShowAuth()
{
    if (talklogic_SessionExist())
    {
        return false;
    }

    return true;
}

bool CBaseAutopProcessor::ExitProcessor()
{
    if (IsAuthSatus())
    {
        Autop_NotifyAutoServer(false, ATP_MSG_REQ_RETURN_AUTH_INFO, 1, 0);
    }
    SetAuthStatus(false);
    return true;
}

bool CBaseAutopProcessor::ProcessAuth()
{
    SetAuthStatus(true);

    // 根据策略来控制是否要弹窗
    if (IsNeedShowAuth())
    {
        ShowAuthWnd();
    }
    else
    {
        _AutopControlMamager.Exit();
    }

    return true;
}

bool CBaseAutopProcessor::IsIncludeAuthInfo(const yl::string & strUrl)
{
    if (strUrl.empty())
    {
        return false;
    }

    char bufType[200] = {0};
    char bufUserName[200] = {0};
    char bufPasswd[200] = {0};
    char bufURL[200] = {0};

    // 解析用户名和密码
    return 4 == sscanf(strUrl.c_str(), "%[^:]://%[^:]:%[^@]@%s", bufType, bufUserName, bufPasswd,
                       bufURL);
}

yl::string CBaseAutopProcessor::DeleteAuthInfoFromUrl(const yl::string & strOriginalUrl)
{
    yl::string strUrl = strOriginalUrl;
    if (!IsIncludeAuthInfo(strOriginalUrl))
    {
        return strUrl;
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
    }
    return strUrl;
}

yl::string CBaseAutopProcessor::AddAuthorizedInfo(const yl::string & strOriginalUrl,
        const yl::string & strUserName, const yl::string & strPassword)
{
    yl::string strUrl = strOriginalUrl;

    if (strOriginalUrl.empty()
            || strUserName.empty())
    {
        return strUrl;
    }

    char * pBuf = (char *) strUrl.c_str();
    char * pTemp = NULL;
    if ((pTemp = strstr(pBuf, "://")) != NULL)
    {
        *pTemp = '\0';
        pTemp += 3;

        // 加入用户名和密码
        char bufURL[512] = {0};
        sprintf(bufURL, "%s://%s:%s@%s", pBuf, strUserName.c_str(), strPassword.c_str(), pTemp);
        strUrl = bufURL;
    }

    return strUrl;
}

CNorMalAutopProcessor::CNorMalAutopProcessor()
    : CBaseAutopProcessor(AUT_NORMAL)
{
    m_bIsNeedShowUpdating = configParser_GetConfigInt(kszAutoprovisionPromptEnable) == 1;
}

CNorMalAutopProcessor::~CNorMalAutopProcessor()
{

}

bool CNorMalAutopProcessor::ProcessRedirectMsg(msgObject & objMsg)
{
    bool bHandle = true;
    switch (objMsg.message)
    {
    case ATP_MSG_NOTIFY_AUTOP_START:
        {
            OnAutoProvision();
            powerled_FlushPowerLed();
            if (Autop_IsNeedShowUpdating())
            {
                ShowUpdatingWnd(ZERO_DLG_UPDATEING);
            }
        }
        break;
    case ATP_MSG_NOTIFY_AUTOP_END:
        {
            // 下面的字符串标志与WEB保持一致
            yl::string strResponseReboot = "reboot";
            // AP_MSG_AUTOP_DONE,
            // wParam: 0/1/-1 0:没有更新，1：更新成功，-1：更新失败 lParam: 0/1 0:不需重启，1：需要重启
            if (objMsg.lParam == 0)
            {
                // 不需要重启
                strResponseReboot = "";
            }

            configParser_SetConfigString(kszAutopUpdateStatus, strResponseReboot.c_str(), CONFIG_LEVEL_PHONE);

            if (Autop_IsNeedShowUpdating())
            {
                //显示更新结束弹窗
                ShowUpdatingWnd(1 == objMsg.wParam ? ZERO_DLG_UPDATEFINISH : ZERO_DLG_UPDATEFAIL);
            }

            bHandle = false;
        }
        break;
    case  ATP_MSG_NOTIFY_AUTH_ERR:
        {
            ProcessAuth();
        }
        break;
    default:
        bHandle = false;
        break;
    }
    return bHandle;
}

void CNorMalAutopProcessor::OnAutoProvision()
{
    // 记录话机正在自动更新,方便网页用于获取当前更新状态
    configParser_SetConfigString(kszAutopUpdateStatus, "Autoprovision", CONFIG_LEVEL_PHONE);
}

bool CNorMalAutopProcessor::ProcessConnect()
{
    return ReadyToConnect();
}

bool CNorMalAutopProcessor::ReadyToConnect()
{
    SetProcessEndStatus(true);
    return ReplyAuthInfo();
}

void CNorMalAutopProcessor::ShowUpdatingWnd(ZeroSpDlgType eWndType)
{
    AUTOP_UPDATE_STATUS eStatus = AUT_STATUS_NONE;
    if (eWndType == ZERO_DLG_UPDATEING)
    {
        eStatus = AUT_STATUS_UPDATING;
    }
    else if (eWndType == ZERO_DLG_UPDATEFAIL)
    {
        eStatus = AUT_STATUS_FAILED;
    }
    else if (eWndType == ZERO_DLG_UPDATEFINISH)
    {
        eStatus = AUT_STATUS_SUCCESS;
    }

    if (eStatus != AUT_STATUS_NONE)
    {
        etl_NotifyApp(FALSE, AUTOP_AUTOP_STATUS, eStatus, GetType());
    }
}

void CNorMalAutopProcessor::ShowAuthWnd()
{
    //重置节电与背光
    backlight_Reset(false);
    etl_NotifyApp(FALSE, AUTOP_AUTOP_STATUS, AUT_STATUS_AUTH, GetType());
}
