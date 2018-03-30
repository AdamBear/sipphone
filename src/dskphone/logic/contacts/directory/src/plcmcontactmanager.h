#ifndef __PLCM_CONTACT_MANAGER__
#define __PLCM_CONTACT_MANAGER__

#include "basecontactmanager.h"

class CContactRemoteAction;


#if IF_BUG_PLCMDIR

class CPLCMContactManger : public CBaseContactManger
{
public:

    static CPLCMContactManger * GetInstance() DECLARE_ATTR;

    bool Init();

    bool ReqLoadContact(int nGroupId);

    // 回调函数. 当一次异步读写完毕时, 会调用此函数.
    virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    //定时更新，UI点击更新
    //nGroupId未指定更新某一地址簿，默认值更新所有
    DIRECTORY_RET_CODE Update();

    //配置有改变，重新加载联系人
    void ReloadContact();

    // 处理定时器消息.
    // | refMessage | 是要处理的消息.
    // 如果处理成功则返回TRUE, 否则返回FALSE.
    bool OnTimeout(unsigned int uTimerID, bool bInTalk) DECLARE_ATTR;

    //设置自动更新定时器
    void SetAutoUpdateTimer();

protected:

    virtual bool MyInit();

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

    bool GetRemoteUrl(yl::string & strUrl);

private:
    CPLCMContactManger();
    virtual ~CPLCMContactManger();

private:
    int m_iInitTimer;
};

#define g_pPlcmDirManager (CPLCMContactManger::GetInstance())

#endif

#endif
