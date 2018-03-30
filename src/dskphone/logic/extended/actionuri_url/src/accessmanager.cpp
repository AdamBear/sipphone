#include "actionuri_url_inc.h"

CAccessManager & CAccessManager::GetInstance()
{
    static CAccessManager s_Instance;
    return s_Instance;
}

CAccessManager::CAccessManager()
{
    m_ActionURIMsg.strDisplayName = "";
    m_ActionURIMsg.strExtraData = "";
    m_ActionURIMsg.lParam = 0;
    m_ActionURIMsg.wParam = 0;
    m_ActionURIMsg.message = 0;

    m_bWaitingAuthorization = FALSE;
    m_bDelayNotify = FALSE;
}

CAccessManager::~CAccessManager()
{
}

//处理CGI发送的MakeCall消息及ActionURI消息
BOOL CAccessManager::ProcessMsg(msgObject & objMessage)
{
    // 只过滤DSK_MSG_MAKE_CALL和DSK_MSG_ACTION_URI两个消息
    if (objMessage.message != DSK_MSG_MAKE_CALL
            && objMessage.message != DSK_MSG_ACTION_URI
            && objMessage.message != DSK_MSG_REDIRECTCALL
            && objMessage.message != DSK_MSG_SETPHONE_CFG
            && objMessage.message != DSK_MSG_ACTION_URI_CONFIRM
            && objMessage.message != SIP_CMD_PASS_ACTION_URI)
    {
        // 返回TRUE表示允许继续执行该消息
        return TRUE;
    }
    COMMONUNITS_INFO("CAccessManager::ProcessMsg:0x%x, lParam:%d", objMessage.message,
                     objMessage.lParam);

    // 配置控制ActionURI是否可用
    if (0 != configParser_GetConfigInt(kszForbidActionURI))
    {
        COMMONUNITS_INFO("CAccessManager: Always Forbid to Remote Control !");
        // 返回FALSE表示不允许执行该消息
        return FALSE;
    }

    // 不需要弹框确认是否接受远程控制
    if (0 == configParser_GetConfigInt(kszShowActionURIOption))
    {
        COMMONUNITS_INFO("CAccessManager: Always Allow to Remote Control !");
        // 返回TRUE表示允许继续执行该消息
        return TRUE;
    }

    // 已经获取用户许可,第17位为1表示该消息的处理需要获得用户的许可
    if (0 == configParser_GetConfigInt(kszxEnableAURIShowMsgBox))
    {
        // 返回TRUE表示允许继续执行该消息
        COMMONUNITS_INFO("Allow to Remote Control !");
        return TRUE;
    }

    // 在等待用户确认是否能处理该消息,暂停响应
    if (m_bWaitingAuthorization
            || talklogic_SessionExist())
    {
        // 返回FALSE以阻断消息
        COMMONUNITS_WARN("Ignore Request On Waiting Authorization !");
        return FALSE;
    }

#if 0 // xmlbrowser先不处理
    //通话或者xmlbrowser不弹出确认框
    if (xmlbrowser_IsActive()
            || talklogic_SessionExist())
    {
        // 返回FALSE以阻断消息
        COMMONUNITS_WARN("Ignore Request On XmlBrowser Or Talking !");
        return FALSE;
    }
#endif
    m_bDelayNotify = TRUE;
    idleScreen_SetStatus(PS_STATE_CTR_REQUEST);

    // 将被暂停执行的命令存起来
    m_ActionURIMsg.lParam = objMessage.lParam/* - (1 << 16)*/; // 消除需获取许可的标志位
    m_ActionURIMsg.wParam = objMessage.wParam;
    m_ActionURIMsg.message = objMessage.message;
    m_ActionURIMsg.strExtraData = (LPCSTR)objMessage.GetExtraData();
    CacheAssistData(m_ActionURIMsg);

    //不在Idle, 返回Idle弹出
    if (!AdapterBox_IsInIdle())
    {
        AdapterBox_ReturnToIdle();
    }
    else
    {
        m_bWaitingAuthorization = TRUE;
    }

    return FALSE;
}

// 设置获取到的结果,为TRUE表示允许控制话机,否则表示不允许
VOID CAccessManager::SetPermissions(BOOL bAllowCtr)
{
    if (m_bWaitingAuthorization
            && bAllowCtr)
    {
        // 第一次获取权限,需要将缓存下的消息发出
        //如果当前ActionURILimitIP为空,则需置为any
        yl::string strLastIP = configParser_GetConfigString(kszAURILimitIP);
        if (strLastIP.empty())
        {
            configParser_SetConfigString(kszAURILimitIP, "any");
        }

        //在没改ActionURILimitIP之前都不会再弹出
        // 当ActionURIShowMsgBox为1时,网页会将消息的lparam第17位置为1,
        // 作为控制请求需要用户确认的标志
        configParser_SetConfigInt(kszxEnableAURIShowMsgBox, 0,
                                  CONFIG_LEVEL_PHONE);//http://10.2.1.199/Bug.php?BugID=60205

        // 将缓存的辅助数据还原
        RestoreAssistData(m_ActionURIMsg);
        // 将缓存的消息重新发出
        msgPostMsgToThreadEx(msgGetThreadByName(VP_PHONE_THREAD_NAME),
                             m_ActionURIMsg.message, m_ActionURIMsg.wParam,
                             m_ActionURIMsg.lParam, m_ActionURIMsg.strExtraData.size() + 1,
                             (void *)m_ActionURIMsg.strExtraData.c_str());
    }

    // 清除缓存
    Clear();
    // 确认了能否控制话机后,都退出等待状态
    m_bWaitingAuthorization = FALSE;
}

// 清除命令的缓存
VOID CAccessManager::Clear()
{
    m_ActionURIMsg.strDisplayName = "";
    m_ActionURIMsg.strExtraData = "";
    m_ActionURIMsg.lParam = 0;
    m_ActionURIMsg.wParam = 0;
    m_ActionURIMsg.message = 0;
}

// 配置其它辅助数据
VOID CAccessManager::CacheAssistData(ActionURIMsg & stActionMsgInfo)
{
    if (stActionMsgInfo.message == DSK_MSG_MAKE_CALL)
    {
        stActionMsgInfo.strDisplayName = configParser_GetConfigString(kszDisplayNameValue).c_str();
    }
}

// 还原辅助数据
VOID CAccessManager::RestoreAssistData(const ActionURIMsg & stActionMsgInfo)
{
    if (stActionMsgInfo.message == DSK_MSG_MAKE_CALL)
    {
        configParser_SetConfigString(kszDisplayNameValue, stActionMsgInfo.strDisplayName.c_str());
    }
}
