#ifndef __DIR_LOGIC_SYNC__
#define __DIR_LOGIC_SYNC__
#include <ylstl/ylstring.h>
#include <yllist.h>
#include "etlmsghandler/modetlmsghandler.h"
#include "taskaction/taskactioncallbackbase.h"
#include "service_autop.h"

using namespace NS_TA;

enum SYNC_STATUS
{
    SYNC_NONE,
    SYNC_READY,
    SYNC_RUNING,
    SYNC_FAIL,
    SYNC_FINISH,
    SYNC_IMPORT,
    SYNC_CANCEL,
    SYNC_TALK,
};

enum REASONCODE
{
    REASON_NONE,
    REASON_NOERROR,
    REASON_NETERROR,
    REASON_ACTIONERROR,
    REASON_OTHERERROR,
};

enum TASK_EVENT
{
    TASK_NONE = 0,
    TASK_BSFT_CONTACT_DOWNLOAD,
    TASK_BSFT_CALLLOG_DOWNLOAD,
    TASK_REMOTE_PHONEBOOK_DOWNLOAD,
    TASK_LOCALDIR_UPLOAD,
    TASK_LOCALDIR_DOWNLOAD,
    TASK_LOCALLOG_UPLOAD,
    TASK_LOCALLOG_DOWNLOAD,
};

typedef struct TaskInfo
{
    TASK_EVENT m_Event;
    SYNC_STATUS m_eTaskStatus;

    TaskInfo()
    {
        m_Event = TASK_NONE;
        m_eTaskStatus = SYNC_NONE;
    }

} TaskInfo;

class CDirLogicSync /*: public CTaskActionCallbackBase*/
{
public:
    CDirLogicSync(void);
    ~CDirLogicSync(void);
    static CDirLogicSync * GetInstance() DECLARE_ATTR;

    //消息处理
    static LRESULT OnMessage(msgObject & refObj);

    // 回调函数. 当一次异步读写完毕时, 会调用此函数.
    //virtual void OnTaskActionCallback(CTaskAction * pTaskAction);

    void Init()
    {
        return;
    }

    //获取联系人同步开关状态
    bool IsContactSyncEnable();

    //获取历史记录开关状态
    bool IsLocalLogSyncEnable();

    void PreLoad();

    REASONCODE RunSyncByType(TASK_EVENT eType);

    bool RunSyncByStatus(SYNC_STATUS eStatusType, bool bIsNeedClearTyrCount = false);

    bool HaveTaskInfo(TASK_EVENT eType);

    void ResetDelayTimer();

private:

    //已经修改为Autop上传，该接口废除
    //bool RunSyncAction(TASK_EVENT eType, yl::string strURL, yl::string strFilePath, REQUEST_ID eReqID = DEFAULT_ID);

    //根据请求类型拼装URL
    yl::string PacketUrlByType(TASK_EVENT eType);

    //根据请求类型获取文件全路径
    yl::string GetFullFilePathByType(TASK_EVENT eType,
                                     bool bIsUpLoad = false, bool bIsPostUpload = false);

    //定时器处理
    void OnTimer(msgObject & refObj);

    //获取本地联系人文件路径
    yl::string GetContactFullFilePath(bool bIsUpLoad = false);

    //获取本地历史记录文件路径
    yl::string GetLocalLogFullFilePath(bool bIsUpLoad = false);

    //删除路径中最后的文件名
    void RemoveFileNameFromPath(yl::string & strURL);

    //获取本地联系人MAC文件名
    void GetMacContactFileName(yl::string & strFileName);

    void GetMacLocalLogFileName(yl::string & strFileName);

    void AddFileNameToURL(yl::string & strUrl, TASK_EVENT eType);

    SYNC_STATUS GetCurrentSyncStatus(TASK_EVENT eType);

    //启动联系人上传定时器
    void RunUploadDelayTimer();

    //启动历史记录上传定时器
    void RunLocalLogUploadDelayTimer();

    void ProcessLocalLogUpload();

    //下载延时
    void RunImportDelay();

    //启动重试定时器
    void RunTryDelayTimer();

    //获取历史记录上传延迟时间配置
    int GetLogUploadTime();

    void AddTask(TaskInfo newTask);

    //上传定时器到，执行任务中所有的联系人上传任务
    void RunUploadTask(TASK_EVENT eEvent);

    void ChangeStatusByEvent(TASK_EVENT eType, SYNC_STATUS eStatusType);

    void ClearFinishTask();

    bool IsExistTask(TASK_EVENT eType);

    void ExportFileByType(TASK_EVENT eType, yl::string strFilePath);

    void PrintTaskInfo();

    bool IsNeedAddServerType(yl::string & strSyncPath);

    void AddServerType(yl::string & strSyncPath);

    void NotifyAutopSync(TASK_EVENT eType, yl::string strURL, yl::string strFilePath);

    void ProcessCallLogTask();
private:

    //重试定时器
    int m_iTryTimer;

    //联系人上传延时定时器
    int m_iUploadDelayTimer;

    //历史记录上传延时定时器
    int m_iLocalLogDelayTimer;

    //导入容错
    int m_iImportTimer;

    YLVector<TaskInfo> m_vecSyncTask;

    atp_ex_url_info m_ExSyncInfo;

    int m_nFileIndex;

};

#define g_pDirLogicSync (CDirLogicSync::GetInstance())

#endif
