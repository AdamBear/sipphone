#ifndef __REMOTE_CONTACT_MANAGER__
#define __REMOTE_CONTACT_MANAGER__

#include "basecontactmanager.h"

class CContactRemoteAction;

class CRemoteContactManger : public CBaseContactManger
{
public:

    static CRemoteContactManger * GetInstance() DECLARE_ATTR;

    bool Init();

    bool ReqLoadContact(int nGroupId, const yl::string & strSearchKey = "");

    // 回调函数. 当一次异步读写完毕时, 会调用此函数.
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    //通过hotKey获取信息，是否存在hotKey，如存在通过nReHotkeyId返回hotKeyId，获取其下联系人和其他流程一致，FileDownloadStatus返回当前状态
    FileDownloadStatus GetHotKeyStatus(const yl::string & strKey, int nGroupId, int & nReHotkeyId);

    //定时更新，UI点击更新
    //nGroupId未指定更新某一地址簿，默认值更新所有
    DIRECTORY_RET_CODE Update(int nGroupId = -1);

    //配置有改变，重新加载联系人
    void ReloadContact();

    // 处理定时器消息.
    // | refMessage | 是要处理的消息.
    // 如果处理成功则返回TRUE, 否则返回FALSE.
    bool OnTimeout(unsigned int uTimerID, bool bInTalk) DECLARE_ATTR;

    //设置自动更新定时器
    void SetAutoUpdateTimer();

    //获取远程地址簿显示名
    const yl::string & GetRemoteDisplayName();

    //递归删除某个组下所有联系人数据
    virtual bool ClearContactByGroupID(int nGroupId);

    int GetRootGroupId(int nId);

protected:

    virtual bool MyInit();

    // 从Xml文件中读取远程地址薄的配置信息.
    // | strFileName | 是Xml文件名.
    // 读取成功则返回true, 否则返回false.
    bool MyLoadDataFromConfigXml(const yl::string & strFileName);

    virtual bool IsNeedShowAllGroup();

    // 创建远程下载解析Action.
    // | pszLocalFilePath | [in] 为远程文件的本地保存路径.
    // | pszRemoteFilePath | [in] 为远程文件的路径.
    CContactRemoteAction * CreateContactRemoteAction(
        const char * pszLocalFilePath, const char * pszRemoteFilePath);

    // 开始下载.
    // | nGroupId | 为远程联系人组Id.
    // 如果成功准备则返回FILE_DOWNLOAD_OK, 否则返回具体的错误码.
    int MyExecDownload(int nGroupId);

    //通过ID获取节点级数
    int GetLevelById(int nId);

private:
    CRemoteContactManger();
    virtual ~CRemoteContactManger();

private:
    yl::string m_strDisplayName;

    int m_iInitTimer;

    friend class CContactRemoteAction;
};

#define g_pRemoteDirManager (CRemoteContactManger::GetInstance())

#endif
