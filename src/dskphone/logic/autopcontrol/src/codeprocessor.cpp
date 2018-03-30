#include "codeprocessor.h"
#include "autopcontrolmanager.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "adapterbox/include/modadapterbox.h"
#include "setting_page_define.h"
#include "interfacedefine.h"
#include "autopcontrol/include/modautopcontrol.h"

CCodeProcessor::CCodeProcessor()
    : CBaseAutopProcessor(AUT_CODE)
{
    m_pCodeParam = NULL;
}


CCodeProcessor::~CCodeProcessor()
{
    m_pCodeParam = NULL;
}

bool CCodeProcessor::ProcessConnect()
{
    //从data 中取数据
    m_pCodeParam = _AutopControlData.GetMatchCodeInfo();

    if (NULL == m_pCodeParam
            || NULL == m_pCodeParam->lpAutoPData)
    {
        return false;
    }

    if (!GetUserName().empty())
    {
        strcpy(m_pCodeParam->lpAutoPData->username, GetUserName().c_str());
    }

    if (!GetPasswd().empty())
    {
        strcpy(m_pCodeParam->lpAutoPData->password, GetPasswd().c_str());
    }

    //加入用户名密码
    yl::string strUrl = m_pCodeParam->lpAutoPData->url;

    if (IsIncludeAuthInfo(strUrl))
    {
        strUrl = DeleteAuthInfoFromUrl(strUrl);
    }

    strUrl = AddAuthorizedInfo(strUrl, m_pCodeParam->lpAutoPData->username,
                               m_pCodeParam->lpAutoPData->password);

    SetUrl(strUrl);

    //保存配置
    SaveConfig();

    return ReadyToConnect();
}

bool CCodeProcessor::ReadyToConnect()
{
    if (NULL == m_pCodeParam)
    {
        return false;
    }

    ZERO_DEBUG("autop code ready to connect[%s] authstatus[%d]", GetUrl().c_str(), IsAuthSatus());

    if (Autop_IsNeedShowUpdating())
    {
        AdapterBox_ZeroOpenDlg(ZERO_DLG_UPDATEING);
    }

    SetProcessEndStatus(true);

    if (IsAuthSatus())
    {
        return ReplyAuthInfo();
    }

    return Autop_NotifyAutoServerEx(false, ATP_MSG_REQ_START_BY_INFO, 0, 0, sizeof(atp_info_t),
                                    (LPCVOID)m_pCodeParam->lpAutoPData);
}

void CCodeProcessor::SaveConfig()
{
    if (m_pCodeParam == NULL
            || m_pCodeParam->lpAutoPData == NULL)
    {
        return;
    }

    configParser_SetCfgItemStringValue(
        kszAutopCodeUser,
        m_pCodeParam->iIdx,
        m_pCodeParam->lpAutoPData->username
        , CONFIG_LEVEL_PHONE);

    configParser_SetCfgItemStringValue(
        kszAutopCodePassword,
        m_pCodeParam->iIdx,
        m_pCodeParam->lpAutoPData->password
        , CONFIG_LEVEL_PHONE);
}

bool CCodeProcessor::ProcessRedirectMsg(msgObject & objMsg)
{
    bool bHandle = true;
    switch (objMsg.message)
    {
    case ATP_MSG_NOTIFY_AUTH_ERR:
        {
            SetProcessEndStatus(false);
            ProcessAuth();
        }
        break;
    case ATP_MSG_NOTIFY_AUTOP_END:
        {
            if (IsProcessEndMsg())
            {
                bHandle = false;

                if (_AutopControlMamager.IsNeedShowUpdatingWnd())
                {
                    //显示更新结束弹窗
                    ShowUpdatingWnd(1 == objMsg.wParam ? ZERO_DLG_UPDATEFINISH : ZERO_DLG_UPDATEFAIL);
                }

                // 需要立即置为不处理end 消息
                // http://bugfree.yealink.com/Bug.php?BugID=108618
                SetProcessEndStatus(false);
            }
        }
        break;
    default:
        bHandle = false;
        break;
    }

    return bHandle;
}

void CCodeProcessor::ShowAuthWnd()
{
    AdapterBox_ZeroOpenDlg(ZERO_DLG_AUTOP_AUTHENTICATION);
}
