#ifndef __REMOTE_CTR_MANAGER_H__
#define __REMOTE_CTR_MANAGER_H__

// 用于存储一个命令信息
struct ActionURIMsg
{
    UINT        message;
    WPARAM      wParam;
    LPARAM      lParam;
    yl::string  strExtraData;

    yl::string  strDisplayName;     // MakeCall呼出时的显示名
};

class CAccessManager
{

public:
    static CAccessManager & GetInstance() DECLARE_ATTR;
private:
    CAccessManager();
    ~CAccessManager();
public:
    // 是否正在等待用户确认能否控制话机
    BOOL IsDelayNotify()
    {
        return m_bDelayNotify;
    }
    VOID ClearDelayNotifyFlag()
    {
        m_bDelayNotify = FALSE;
    }
    VOID SetWaitingAuthorization(BOOL bAuthorization)
    {
        m_bWaitingAuthorization = bAuthorization;
    }
    // 设置获取到的结果,为TRUE表示允许控制话机,否则表示不允许
    VOID SetPermissions(BOOL bAllowCtr);
    //处理CGI发送的MakeCall消息及ActionURI消息,返回TRUE表示可以继续执行该消息
    BOOL ProcessMsg(msgObject & objMessage);

private:
    // 清除命令的缓存
    VOID Clear();
    // 配置其它辅助数据
    VOID CacheAssistData(ActionURIMsg & stActionMsgInfo);
    // 还原辅助数据
    VOID RestoreAssistData(const ActionURIMsg & stActionMsgInfo);

private:
    BOOL
    m_bWaitingAuthorization;    // 是否正在等待用户授权控制话机或者呼出
    BOOL
    m_bDelayNotify;             // 通话中或者显示xmlbrowser时,延迟到idle显示提示
    ActionURIMsg
    m_ActionURIMsg;             // 在询问用户是否允许ActionURI时保存下的命令,在得到允许后再执行
};

#define _AccessManager CAccessManager::GetInstance()

#endif

