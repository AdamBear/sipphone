#include "dirlogicsync.h"
#include "threadtask/include/uploadfileaction.h"
#include "taskaction/taskaction.h"
#include "taskaction/modtaskaction.h"
#include "dirdefine.h"
#include "localcontactmanager.h"
#include "locallogmanager.h"
#include "directoryenumtypes.h"
#include "path_define.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "moddirinterface.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "service_system.h"
#include "devicelib/networkserver.h"
#include "xportmanager.h"
#include "directorymessagedefine.h"
#include "talk/talklogic/include/talkmsgdefine.h"
#include "talk/talklogic/include/modtalklogic.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include<arpa/inet.h>
#include "autopcontrol/include/modautopcontrol.h"

#define UNDEFINE_TIMERID -1
#define knUploadTimeout 3*1000
#define knImportTimer 2*1000

#define MAX_FILE_ID 10

enum UPLOAD_MSG_LPARAM
{
    UPLOAD_MSG_LPARAM_LOCALDIR = 0,
    UPLOAD_MSG_LPARAM_LOCALLOG,
};

static int s_iTryCount = 0;

void ResetTryCount()
{
    if (0 != s_iTryCount)
    {
        s_iTryCount = 0;
    }
}

CDirLogicSync::CDirLogicSync(void)
{
    //重试定时器
    m_iTryTimer = UNDEFINE_TIMERID;
    //上传延时定时器
    m_iUploadDelayTimer = UNDEFINE_TIMERID;
    m_iLocalLogDelayTimer = UNDEFINE_TIMERID;

    m_nFileIndex = 0;

    //导入容错定时器
    m_iImportTimer = UNDEFINE_TIMERID;
    m_vecSyncTask.clear();

    etl_RegisterMsgHandle(TM_TIMER, TM_TIMER, OnMessage);
    etl_RegisterMsgHandle(LOCAL_DIRECTORY_UPLOAD, LOCAL_DIRECTORY_UPLOAD, OnMessage);
    etl_RegisterMsgHandle(LOCAL_CALLLOG_UPLOAD, LOCAL_CALLLOG_UPLOAD, OnMessage);
    etl_RegisterMsgHandle(ATP_MSG_NOTIFY_DOWNLOAD_START, ATP_MSG_NOTIFY_DOWNLOAD_START, OnMessage);
    etl_RegisterMsgHandle(ATP_MSG_NOTIFY_DOWNLOAD_END, ATP_MSG_NOTIFY_DOWNLOAD_END, OnMessage);
    etl_RegisterMsgHandle(ATP_MSG_REQ_UPLOAD_FILE_BY_INFO, ATP_MSG_REQ_UPLOAD_FILE_BY_INFO,
                          OnMessage);
    etl_RegisterMsgHandle(TALK_MSG_SESSION_DESTORY, TALK_MSG_SESSION_DESTORY, OnMessage);
}

CDirLogicSync::~CDirLogicSync(void)
{
    etl_UnregisterMsgHandle(TM_TIMER, TM_TIMER, OnMessage);
    etl_UnregisterMsgHandle(LOCAL_DIRECTORY_UPLOAD, LOCAL_DIRECTORY_UPLOAD, OnMessage);
    etl_UnregisterMsgHandle(LOCAL_CALLLOG_UPLOAD, LOCAL_CALLLOG_UPLOAD, OnMessage);
    etl_UnregisterMsgHandle(ATP_MSG_NOTIFY_DOWNLOAD_START, ATP_MSG_NOTIFY_DOWNLOAD_START,
                            OnMessage);
    etl_UnregisterMsgHandle(ATP_MSG_NOTIFY_DOWNLOAD_END, ATP_MSG_NOTIFY_DOWNLOAD_END, OnMessage);
    etl_UnregisterMsgHandle(ATP_MSG_REQ_UPLOAD_FILE_BY_INFO, ATP_MSG_REQ_UPLOAD_FILE_BY_INFO,
                            OnMessage);
    etl_UnregisterMsgHandle(TALK_MSG_SESSION_DESTORY, TALK_MSG_SESSION_DESTORY, OnMessage);
}

CDirLogicSync * CDirLogicSync::GetInstance()
{
    static CDirLogicSync sinstance;
    return &sinstance;
}

bool CDirLogicSync::IsContactSyncEnable()
{
    return 1 == configParser_GetConfigInt(kszLocalDirSyncEnable);
}

bool CDirLogicSync::IsLocalLogSyncEnable()
{
    return 1 == configParser_GetConfigInt(kszLocalCallLogSyncEnable);
}

REASONCODE CDirLogicSync::RunSyncByType(TASK_EVENT eType)
{
    if (!netIsNetworkReady())
    {
        return REASON_NETERROR;
    }

    if (TASK_NONE == eType || !IsExistTask(eType))
    {
        return REASON_OTHERERROR;
    }

    yl::string strUrl = PacketUrlByType(eType);
    if (TASK_LOCALDIR_DOWNLOAD == eType || TASK_LOCALLOG_DOWNLOAD == eType)
    {
        //下载时的URL没有文件名需要加上
        AddFileNameToURL(strUrl, eType);
    }

    yl::string strFilePath = GetFullFilePathByType(eType);
    if (strUrl.empty() || strFilePath.empty())
    {
        return REASON_OTHERERROR;
    }

    NotifyAutopSync(eType, strUrl, strFilePath);
    return REASON_NOERROR;
}


void CDirLogicSync::AddFileNameToURL(yl::string & strUrl, TASK_EVENT eType)
{
    if (strUrl == "")
    {
        return;
    }
    yl::string strExtention = pathGetFileExtensionPos(strUrl.c_str());
    if (strExtention != ".xml")
    {
        //下载URL没有带文件名要加上
        switch (eType)
        {
        case TASK_LOCALDIR_DOWNLOAD:
            {
                GetMacContactFileName(strExtention);
            }
            break;
        case TASK_LOCALLOG_DOWNLOAD:
            {
                GetMacLocalLogFileName(strExtention);
            }
            break;
        default:
            break;
        }

        strUrl.append(strExtention.c_str());
    }
}

yl::string CDirLogicSync::PacketUrlByType(TASK_EVENT eType)
{
    if (TASK_NONE == eType)
    {
        return "";
    }

    yl::string strSyncURL;
    yl::string strConfigPath;
    bool bIsHistory = false;
    if (TASK_LOCALDIR_DOWNLOAD == eType || TASK_LOCALDIR_UPLOAD == eType)
    {
        strConfigPath = configParser_GetConfigString(kszLocalDirSyncPath);
    }
    else if (TASK_LOCALLOG_UPLOAD == eType || TASK_LOCALLOG_DOWNLOAD == eType)
    {
        bIsHistory = true;
        strConfigPath = configParser_GetConfigString(kszLocalLogSyncPath);
    }

    DIR_INFO("strConfigPath [%s]", strConfigPath.c_str());

    yl::string strAutopPath = configParser_GetConfigString(kszAutoProvisionUrl);
    if (strAutopPath.empty())
    {
        strAutopPath = configParser_GetConfigString(kszAutopSucessUrl);
    }
    if (strAutopPath.empty() && strConfigPath.empty())
    {
        return "";
    }

    bool bNeedFileName = true;
    RemoveFileNameFromPath(strAutopPath);

    if (strConfigPath.empty())
    {
        strSyncURL = strAutopPath;
    }
    else
    {
        if (strConfigPath.substr(0, 7) == "http://"  || strConfigPath.substr(0, 8) == "https://"
                || strConfigPath.substr(0, 6) == "ftp://" || strConfigPath.substr(0, 7) == "tftp://")
        {
            strSyncURL = strConfigPath;
        }
        else
        {
            if (IsNeedAddServerType(strConfigPath))
            {
                //如果相对路径为IP，则添加协议头 ，形如“X.X.X.X/”添加协议头，“/X.X.X.X”当做相对路径
                AddServerType(strConfigPath);
                strSyncURL = strConfigPath;
            }
            else
            {
                if (strAutopPath.empty())
                {
                    return "";
                }
                strSyncURL = strAutopPath;
                if (strSyncURL.substr(strSyncURL.length() - 1, 1) != "/"
                        && strConfigPath.substr(0, 1) != "/")
                {
                    strSyncURL.append("/");
                }
                if (strSyncURL.substr(strSyncURL.length() - 1, 1) == "/"
                        && strConfigPath.substr(0, 1) == "/")
                {
                    strConfigPath = strConfigPath.substr(1, strConfigPath.length() - 1);
                }
                strSyncURL += strConfigPath;
            }

        }
    }

    if ((strSyncURL.substr(0, 7) == "http://"
            || strSyncURL.substr(0, 8) == "https://")
            && configParser_GetConfigInt(kszHttpUploadMethod) == 1) //http post
    {
        bNeedFileName = false;
    }

#if IF_FEATURE_XMLBROWSER_HOTELING
    yl::string strExtention = pathGetFileExtensionPos(strSyncURL.c_str());
    if (bNeedFileName
            && strExtention == ".xml")
    {
        return strSyncURL;
    }
#endif

    RemoveFileNameFromPath(strSyncURL);

    if (strSyncURL.substr(strSyncURL.length() - 1, 1) != "/")
    {
        strSyncURL.append("/");
    }

    if (bNeedFileName)
    {
        yl::string strTemp;
        if (bIsHistory)
        {
            GetMacLocalLogFileName(strTemp);
        }
        else
        {
            GetMacContactFileName(strTemp);
        }

        strSyncURL.append(strTemp);
    }

    return strSyncURL;
}


yl::string CDirLogicSync::GetFullFilePathByType(TASK_EVENT eType,
        bool bIsUpLoad /*= false*/, bool bIsPostUpload/* = false*/)
{
    switch (eType)
    {
    case TASK_LOCALDIR_UPLOAD:
    case TASK_LOCALDIR_DOWNLOAD:
        {
            if (IsContactSyncEnable())
            {
                return GetContactFullFilePath(TASK_LOCALDIR_UPLOAD == eType);
            }
        }
        break;
    case TASK_LOCALLOG_UPLOAD:
    case TASK_LOCALLOG_DOWNLOAD:
        {
            if (IsLocalLogSyncEnable())
            {
                return GetLocalLogFullFilePath(TASK_LOCALLOG_UPLOAD == eType);
            }
        }
        break;
    default:
        break;
    }

    return "";
}

#if 0 //已经修改为Autop上传，该接口废除
bool CDirLogicSync::RunSyncAction(TASK_EVENT eType, yl::string strURL, yl::string strFilePath)
{
    DIR_INFO("RunSyncAction: Event[%d] URL[%s] FilePath[%s] ", eType, strURL.c_str(),
             strFilePath.c_str());

    NS_TA::CNetworkFileAction * pAction = NULL;
    if (TASK_LOCALDIR_DOWNLOAD == eType || TASK_LOCALLOG_DOWNLOAD == eType)
    {
        pAction = TaskAction_CreateNetworkFileAction(strFilePath.c_str(), strURL.c_str());
        pAction->SetRead(true);
    }
    if (TASK_LOCALDIR_UPLOAD == eType || TASK_LOCALLOG_UPLOAD == eType)
    {
        ExportFileByType(eType, strFilePath);
        pAction = TaskAction_CreateUploadFileAction(strFilePath.c_str(), strURL.c_str());
        bool isPut = configParser_GetConfigInt(kszHttpUploadMethod) == 0;
        pAction->SetPut(isPut);
        pAction->SetRead(false);
    }
    if (NULL == pAction)
    {
        return false;
    }

    yl::string strUser = configParser_GetConfigString(kszAutoProvisionUsername);
    yl::string strPassword = configParser_GetConfigString(kszAutoProvisionPassword);
    pAction->SetTaskID(TASK_CreateTaskID(0, eType));
    pAction->SetSync(false);  // 异步.
    pAction->SetCallback(this);  // 回调函数.
    pAction->SetUsername(strUser);
    pAction->SetPassword(strPassword);

    ChangeStatusByEvent(eType, SYNC_RUNING);

    return TaskAction_ExecAction(pAction);
}
#endif

void CDirLogicSync::RemoveFileNameFromPath(yl::string & strURL)
{
    yl::string strExtention = pathGetFileExtensionPos(strURL.c_str());
    if (strExtention == ".ini" || strExtention == ".cfg" || strExtention == ".xml")
    {
        //带文件名则获取上层目录
        strURL = pathRemoveFileSpec((LPSTR)strURL.c_str());
    }
}

void CDirLogicSync::GetMacContactFileName(yl::string & strFileName)
{
    strFileName.clear();
    char szTemp[64] = {0};

    yl::string strName = netGetMACText('\0');
#if IF_FEATURE_XMLBROWSER_HOTELING
    if (configParser_GetConfigInt(kszHotelingLoginStatus) == 1)
    {
        yl::string strUser = configParser_GetConfigString(kszHotelingUserName);
        if (!strUser.empty())
        {
            strName = strUser;
        }
    }
#endif

    sprintf(szTemp, "%s-contact.xml", strName.c_str());

    strFileName = szTemp;
}

void CDirLogicSync::GetMacLocalLogFileName(yl::string & strFileName)
{
    strFileName.clear();
    char szTemp[64] = {0};

    yl::string strName = netGetMACText('\0');
#if IF_FEATURE_XMLBROWSER_HOTELING
    if (configParser_GetConfigInt(kszHotelingLoginStatus) == 1)
    {
        yl::string strUser = configParser_GetConfigString(kszHotelingUserName);
        if (!strUser.empty())
        {
            strName = strUser;
        }
    }
#endif

    sprintf(szTemp, "%s-calllog.xml", strName.c_str());
    strFileName = szTemp;
}

yl::string CDirLogicSync::GetContactFullFilePath(bool bIsUpLoad /*= false*/)
{
    yl::string strFilePath = g_strConfigPath;
    yl::string strTemp;
    GetMacContactFileName(strTemp);
    if (bIsUpLoad)
    {
        // 若无此目录, 则创建
        strFilePath = TEMP_PATH "uploadlog";
        if (!pathFileExist(strFilePath.c_str()))
        {
            pathCreateDirectory(strFilePath.c_str());
        }
        strFilePath.append("/");
    }

    strFilePath.append(strTemp.c_str());

    return strFilePath;
}

yl::string CDirLogicSync::GetLocalLogFullFilePath(bool bIsUpLoad /*= false*/)
{
    yl::string strLocalLogFilePath = TEMP_PATH;
    if (bIsUpLoad)
    {
        // 若无此目录, 则创建
        strLocalLogFilePath = TEMP_PATH "uploadlog";
        if (!pathFileExist(strLocalLogFilePath.c_str()))
        {
            pathCreateDirectory(strLocalLogFilePath.c_str());
        }
        strLocalLogFilePath.append("/");
    }

    yl::string strFileName;
    //每次上传使用相同名字
    GetMacLocalLogFileName(strFileName);

    strLocalLogFilePath.append(strFileName);
    return strLocalLogFilePath;
}

//void CDirLogicSync::OnTaskActionCallback(CTaskAction * pTaskAction)
//{
//    if (NULL == pTaskAction)
//    {
//        return;
//    }
//
//    TaskActionErrorType eErrorType = pTaskAction->GetTaskActionErrorType();
//    TASK_EVENT eEventType = TASK_GetEventByTaskID(pTaskAction->GetTaskID());
//    CNetworkFileAction * pNetworkAction = dynamic_cast<CNetworkFileAction *>(pTaskAction);
//    DIR_INFO("CDirLogicSync Callback : ErrorType[%d], EventType[%d]", eErrorType, eEventType);
//
//
//    switch (eErrorType)
//    {
//    case TA_NO_ERROR:
//        {
//            if (TASK_LOCALDIR_DOWNLOAD == eEventType)
//            {
//                //通知导入
//                //NS_TA::CNetworkFileAction *pNetworkAction = dynamic_cast<NS_TA::CNetworkFileAction*>(pTaskAction);
//                ChangeStatusByEvent(eEventType, SYNC_IMPORT);
//                int iImportResult = IMPORT_INVALID_FILE;
//                iImportResult = XPortFactory::ImportByPath(pNetworkAction->GetFilePath().c_str(), false);
//                if (IMPORT_NO_ERROR != iImportResult)
//                {
//                    ChangeStatusByEvent(eEventType, SYNC_FINISH);
//                }
//                break;
//            }
//            else if (TASK_LOCALDIR_UPLOAD == eEventType)
//            {
//                //删除上传临时文件
//                //NS_TA::CUploadFileAction* pNetworkAction = dynamic_cast<NS_TA::CUploadFileAction*>(pNetworkAction);
//                //remove(pNetworkAction->GetFilePath().c_str());
//                ChangeStatusByEvent(eEventType, SYNC_FINISH);
//            }
//            else if (TASK_LOCALLOG_UPLOAD == eEventType)
//            {
//                ChangeStatusByEvent(eEventType, SYNC_FINISH);
//                //NS_TA::CUploadFileAction* pNetworkAction = dynamic_cast<NS_TA::CUploadFileAction*>(pNetworkAction);
//                //文件名未定，上传的是原始文件，先不能删，后续改
//                //remove(pNetworkAction->GetFilePath().c_str());
//                //SetCurrentSyncStatus(SYNC_NONE, eReqID);
//            }
//            else if (TASK_LOCALLOG_DOWNLOAD == eEventType)
//            {
//                ChangeStatusByEvent(eEventType, SYNC_IMPORT);
//            }
//
//            ResetTryCount();
//        }
//        break;
//    case TA_HTTP_DOWNLOAD_UNAUTHORIZED:
//        {
//            //鉴权错误认为失败，不重试
//            ChangeStatusByEvent(eEventType, SYNC_FINISH);
//        }
//        break;
//    default:
//        {
//            ++s_iTryCount;
//
//            DIR_INFO("try count[%d] try time [%d] this is [%d]",
//                     configParser_GetConfigInt(kszAutopAttemptTimes),
//                     configParser_GetConfigInt(kszAutopAttemptDelay), s_iTryCount);
//
//            if (s_iTryCount < configParser_GetConfigInt(kszAutopAttemptTimes))
//            {
//                ChangeStatusByEvent(eEventType, SYNC_FAIL);
//                RunTryDelayTimer();
//            }
//            else
//            {
//                ChangeStatusByEvent(eEventType, SYNC_FINISH);
//                s_iTryCount = 0;  //重置重试次数
//            }
//        }
//        break;
//    }
//
//    remove(pNetworkAction->GetFilePath().c_str());
//    ClearFinishTask();
//    TASK_DeleteTask(pNetworkAction->GetTaskID());
//    pNetworkAction->Release();
//}


LRESULT CDirLogicSync::OnMessage(msgObject & refObj)
{

    bool bHandled = true;
    switch (refObj.message)
    {
    case LOCAL_DIRECTORY_UPLOAD:
        {
            DIR_INFO("Received LOCAL_DIRECTORY_UPLOAD wParam[%d]", refObj.wParam);
            if (g_pDirLogicSync->IsContactSyncEnable())
            {
                if (SYNC_IMPORT == g_pDirLogicSync->GetCurrentSyncStatus(TASK_LOCALDIR_DOWNLOAD))
                {
                    //导入容错
                    g_pDirLogicSync->RunImportDelay();
                }
                else
                {
                    TaskInfo newTask;
                    newTask.m_Event = TASK_LOCALDIR_UPLOAD;
                    newTask.m_eTaskStatus = SYNC_READY;
                    g_pDirLogicSync->AddTask(newTask);
                    g_pDirLogicSync->RunUploadDelayTimer();
                }
            }
        }
        break;
    case LOCAL_CALLLOG_UPLOAD:
        {
            g_pDirLogicSync->ProcessLocalLogUpload();
        }
        break;
    case SYS_MSG_NOTIFY_NETWORK_STATUS:
        {
            DIR_INFO("Received network status changed");
            if (!netIsNetworkReady())
            {
                DIR_INFO("Network not ready");
                return false;
            }
            g_pDirLogicSync->RunSyncByStatus(SYNC_FAIL, true);
        }
        break;
    case ATP_MSG_NOTIFY_DOWNLOAD_START:
        {
            DIR_INFO("Received DIR Download Start: lparam[%d]", refObj.lParam);
            if (g_pDirLogicSync->IsContactSyncEnable())
            {
                if (UPLOAD_MSG_LPARAM_LOCALDIR == refObj.lParam)
                {
                    DIR_INFO("UPLOAD_MSG_LPARAM_LOCALDIR == refObj.lParam");
                    TaskInfo newTask;
                    newTask.m_Event = TASK_LOCALDIR_DOWNLOAD;
                    newTask.m_eTaskStatus = SYNC_READY;
                    g_pDirLogicSync->AddTask(newTask);
                }
            }
            if (g_pDirLogicSync->IsLocalLogSyncEnable() && UPLOAD_MSG_LPARAM_LOCALLOG == refObj.lParam)
            {
//              if (g_pDirLogicSync->HaveTaskInfo(TASK_LOCALLOG_UPLOAD))
//              {
//                  //上传期间有下载，先上传
//                  RunUploadTask(TASK_LOCALLOG_UPLOAD);
//              }
                TaskInfo newTask;
                newTask.m_Event = TASK_LOCALLOG_DOWNLOAD;
                newTask.m_eTaskStatus = SYNC_READY;
                g_pDirLogicSync->AddTask(newTask);
            }
        }
        break;
    case ATP_MSG_NOTIFY_DOWNLOAD_END:
        {
            DIR_INFO("Received DIR Download END: wparam[%d] [%d]", refObj.wParam, refObj.lParam);

            if ((UPLOAD_MSG_LPARAM_LOCALLOG == refObj.lParam && !g_pDirLogicSync->IsLocalLogSyncEnable())
                    || (UPLOAD_MSG_LPARAM_LOCALDIR == refObj.lParam && !g_pDirLogicSync->IsContactSyncEnable()))
            {
                return false;
            }

            bool bDownSuccess = true;
            if (0 != refObj.wParam)
            {
                bDownSuccess = false;
            }
            yl::string strPath;
            TASK_EVENT eTaskType;
            if (UPLOAD_MSG_LPARAM_LOCALDIR == refObj.lParam)
            {
                eTaskType = TASK_LOCALDIR_DOWNLOAD;
            }
            else if (UPLOAD_MSG_LPARAM_LOCALLOG == refObj.lParam)
            {
                eTaskType = TASK_LOCALLOG_DOWNLOAD;
            }
            else
            {
                return true;
            }

            if (!g_pDirLogicSync->HaveTaskInfo(eTaskType))
            {
                DIR_INFO("Task Error");
                return false;
            }
            int iImportResult = IMPORT_INVALID_FILE;
			bool bIsNeedChangeEvent = false;
            if (bDownSuccess)
            {
                strPath = g_pDirLogicSync->GetFullFilePathByType(eTaskType);
                g_pDirLogicSync->ChangeStatusByEvent(eTaskType, SYNC_IMPORT);
                //此处是联系人的导入，calllog导入需另做处理 ！！！！！！！！！！！！！！！！！！
                DIR_INFO("IMPORT FILE PATH [%s]", strPath.c_str());
                if (UPLOAD_MSG_LPARAM_LOCALDIR == refObj.lParam)
                {
                    iImportResult = XPortFactory::ImportByPath(strPath.c_str(), false);
					bIsNeedChangeEvent = IMPORT_NO_ERROR != iImportResult;
                }
                else if (UPLOAD_MSG_LPARAM_LOCALLOG == refObj.lParam)
                {
                    yl::string strTempPath = (char *)refObj.GetExtraData();
                    if (strTempPath.empty())
                    {
                        strTempPath = strPath;
                    }

                    g_pDirLogicSync->ResetDelayTimer();
                    iImportResult = g_pLocalLogManager->ImportCalllog(strTempPath);

					bIsNeedChangeEvent = true;
                }
            }

			if (!bDownSuccess
				|| bIsNeedChangeEvent)
            {
                g_pDirLogicSync->ChangeStatusByEvent(eTaskType, SYNC_FINISH);
            }
            g_pDirLogicSync->ClearFinishTask();
        }
        break;
    case CONFIG_MSG_BCAST_CHANGED:
        {
            DIR_INFO("Received Web Contact changed");
            if (ST_CONTACTS == refObj.wParam
                    && g_pDirLogicSync->IsContactSyncEnable())
            {
#if IF_FEATURE_XMLBROWSER_HOTELING
                if (refObj.lParam == 3)
                {
                    break;
                }
#endif
                TaskInfo newTask;
                newTask.m_Event = TASK_LOCALDIR_UPLOAD;
                newTask.m_eTaskStatus = SYNC_READY;
                g_pDirLogicSync->AddTask(newTask);
                g_pDirLogicSync->RunUploadDelayTimer();
            }
        }
        break;
    case TM_TIMER:
        {
            g_pDirLogicSync->OnTimer(refObj);
        }
        break;
    case ATP_MSG_REQ_UPLOAD_FILE_BY_INFO:
        {
            DIR_INFO("Received DIR [ATP_MSG_REQ_UPLOAD_FILE_BY_INFO] : wparam[%d] lParam[%d]", refObj.wParam,
                     refObj.lParam);
            if (g_pDirLogicSync->IsContactSyncEnable()
                    && UPLOAD_MSG_LPARAM_LOCALDIR == refObj.lParam)
            {
                g_pDirLogicSync->ChangeStatusByEvent(TASK_LOCALDIR_UPLOAD, SYNC_FINISH);
                g_pDirLogicSync->ClearFinishTask();
                return true;
            }
            else if (g_pDirLogicSync->IsLocalLogSyncEnable() &&
                     UPLOAD_MSG_LPARAM_LOCALLOG == refObj.lParam)
            {
                g_pDirLogicSync->ChangeStatusByEvent(TASK_LOCALLOG_UPLOAD, SYNC_FINISH);
                g_pDirLogicSync->ClearFinishTask();
                return true;
            }
        }
        break;
    case TALK_MSG_SESSION_DESTORY:
        {
            DIR_INFO("TALK_MSG_SESSION_DESTORY sync");

            g_pDirLogicSync->ProcessCallLogTask();
        }
        break;
    default:
        {
            bHandled = false;
        }
        break;
    }

    return bHandled ? 1 : 0;
}

void CDirLogicSync::OnTimer(msgObject & refObj)
{
    if ((UINT)&m_iUploadDelayTimer == refObj.wParam)
    {
        //延时上传定时器
        timerKillThreadTimer((UINT)&m_iUploadDelayTimer);
        m_iUploadDelayTimer = UNDEFINE_TIMERID;
        RunUploadTask(TASK_LOCALDIR_UPLOAD);
    }
    //历史记录上传定时器
    else if ((UINT)&m_iLocalLogDelayTimer == refObj.wParam)
    {
        //延时上传定时器
        timerKillThreadTimer((UINT)&m_iLocalLogDelayTimer);
        m_iLocalLogDelayTimer = UNDEFINE_TIMERID;
        bool bSessionExist = talklogic_SessionExist();
        DIR_INFO("Upload Call Log Session Exist [%d]", bSessionExist);
        if (!bSessionExist)
        {
            RunUploadTask(TASK_LOCALLOG_UPLOAD);
        }
        else
        {
            //通话中不去上传
            ChangeStatusByEvent(TASK_LOCALLOG_UPLOAD, SYNC_TALK);
        }
    }
    //重试定时器
    else if ((UINT)&m_iTryTimer == refObj.wParam)
    {
        timerKillThreadTimer((UINT)&m_iTryTimer);
        m_iTryTimer = UNDEFINE_TIMERID;
        RunSyncByStatus(SYNC_FAIL, true);
    }
    //导入定时器
    else if ((UINT)&m_iImportTimer == refObj.wParam)
    {
        timerKillThreadTimer((UINT)&m_iImportTimer);
        m_iImportTimer = UNDEFINE_TIMERID;
        for (int i = 0; i < m_vecSyncTask.size(); i++)
        {
            if (SYNC_IMPORT == m_vecSyncTask[i].m_eTaskStatus)
            {
                ChangeStatusByEvent(m_vecSyncTask[i].m_Event, SYNC_FINISH);
            }
        }
        ClearFinishTask();

        //通知UI重新加载
        etl_NotifyApp(false, LOCAL_DIRECTORY_RELOAD, 0, 0);
    }
}

SYNC_STATUS CDirLogicSync::GetCurrentSyncStatus(TASK_EVENT eType)
{
    for (int i = 0; i < m_vecSyncTask.size(); i++)
    {
        if (eType == m_vecSyncTask[i].m_Event)
        {
            DIR_INFO("GetCurrentSyncStatus [%d]", m_vecSyncTask[i].m_eTaskStatus);
            return m_vecSyncTask[i].m_eTaskStatus;
        }
    }

    return SYNC_NONE;
}

void CDirLogicSync::PreLoad()
{
    if (g_pDirLogicSync->IsContactSyncEnable() && netIsNetworkReady())
    {
        for (int i = 0; i < DECT_HANDSET_NUM + 1; i++)
        {
            TaskInfo newTask;
            newTask.m_Event = TASK_LOCALDIR_DOWNLOAD;
            AddTask(newTask);

            DIR_INFO("CDirLogicSync::PreLoad()");
            RunSyncByType(TASK_LOCALDIR_DOWNLOAD);
        }
    }
}

void CDirLogicSync::RunUploadDelayTimer()
{
    if (UNDEFINE_TIMERID != m_iUploadDelayTimer)
    {
        timerKillThreadTimer((UINT)&m_iUploadDelayTimer);
    }
    m_iUploadDelayTimer = knUploadTimeout;
    timerSetThreadTimer((UINT)&m_iUploadDelayTimer, m_iUploadDelayTimer);
}

int CDirLogicSync::GetLogUploadTime()
{
    int nUploadTime = configParser_GetConfigInt(kszLogUploadTime);
    if (nUploadTime < 10 || nUploadTime > 600)
    {
        nUploadTime = 60;
    }

    return nUploadTime;
}

void CDirLogicSync::RunLocalLogUploadDelayTimer()
{
    DIR_INFO("CDirLogicSync::RunLocalLogUploadDelayTimer [%d]", m_iLocalLogDelayTimer);
    if (UNDEFINE_TIMERID != m_iLocalLogDelayTimer)
    {
        // 在上传的定时器时间内部需要重新上传，等定时器时间完成在重新上传。
        return;
        //timerKillThreadTimer((unsigned int)&m_iLocalLogDelayTimer);
    }
    m_iLocalLogDelayTimer = GetLogUploadTime() * 1000;
    DIR_INFO("CDirLogicSync::RunLocalLogUploadDelayTimer start [%d]", m_iLocalLogDelayTimer);
    timerSetThreadTimer((unsigned int)&m_iLocalLogDelayTimer, m_iLocalLogDelayTimer);
}

void CDirLogicSync::RunTryDelayTimer()
{
    if (UNDEFINE_TIMERID != m_iTryTimer)
    {
        timerKillThreadTimer((UINT)&m_iTryTimer);
        DIR_INFO("kill m_iTryTimer [%d]", m_iTryTimer);
    }

    m_iTryTimer = configParser_GetConfigInt(kszAutopAttemptDelay);
    if (m_iTryTimer <= 0)
    {
        m_iTryTimer = 3;
    }

    m_iTryTimer *= 1000;
    DIR_INFO("RunTryDelayTimer [%d] ", m_iTryTimer);
    timerSetThreadTimer((UINT)&m_iTryTimer, m_iTryTimer);
}

void CDirLogicSync::AddTask(TaskInfo newTask)
{
    DIR_INFO("ADD Task Before:");
    PrintTaskInfo();

    YLVector<TaskInfo>::iterator iter = m_vecSyncTask.begin();
    for (; iter != m_vecSyncTask.end(); iter ++)
    {
        bool bHandle = (((iter->m_Event == TASK_LOCALDIR_UPLOAD
                          && newTask.m_Event == TASK_LOCALDIR_DOWNLOAD)
                         || (iter->m_Event == TASK_LOCALLOG_UPLOAD && newTask.m_Event == TASK_LOCALLOG_DOWNLOAD)));

        if (!bHandle)
        {
            bHandle = (((iter->m_Event == TASK_LOCALDIR_DOWNLOAD && newTask.m_Event == TASK_LOCALDIR_UPLOAD)
                        || (iter->m_Event == TASK_LOCALLOG_DOWNLOAD && newTask.m_Event == TASK_LOCALLOG_UPLOAD)));
        }

        if (bHandle)
        {
            if (SYNC_FAIL == iter->m_eTaskStatus)
            {
                iter->m_Event = newTask.m_Event;
                iter->m_eTaskStatus = SYNC_READY;
                ResetTryCount();
                DIR_INFO("task change");
                PrintTaskInfo();
            }
            return;
        }
        if (iter->m_Event == newTask.m_Event)
        {
            if (SYNC_FAIL == iter->m_eTaskStatus)
            {
                iter->m_eTaskStatus = SYNC_READY;
                ResetTryCount();
                DIR_INFO("status change");
                PrintTaskInfo();
            }
            return;
        }
    }

    m_vecSyncTask.push_back(newTask);

    DIR_INFO("ADD Task End:");
    PrintTaskInfo();
}

void CDirLogicSync::RunUploadTask(TASK_EVENT eEvent)
{
    for (int index = 0; index < m_vecSyncTask.size();)
    {
        if (eEvent == m_vecSyncTask[index].m_Event
                && SYNC_READY == m_vecSyncTask[index].m_eTaskStatus)
        {
            REASONCODE eResult = RunSyncByType(m_vecSyncTask[index].m_Event);
            if (REASON_NETERROR == eResult)
            {
                ChangeStatusByEvent(m_vecSyncTask[index].m_Event, SYNC_FAIL);
                ++index;
            }
            else if (REASON_OTHERERROR == eResult)
            {
                m_vecSyncTask.removeAt(index);
            }
            else //REASON_NOERROR
            {
                index++;
            }
        }
        else
        {
            ++index;
        }
    }
}

void CDirLogicSync::ClearFinishTask()
{
    DIR_INFO("Clear Finish Task Before:");
    PrintTaskInfo();

    for (int index = 0; index < m_vecSyncTask.size();)
    {
        if (SYNC_FINISH == m_vecSyncTask[index].m_eTaskStatus)
        {
            m_vecSyncTask.removeAt(index);
        }
        else
        {
            index++;
        }
    }

    DIR_INFO("Clear Finish Task End:");
    PrintTaskInfo();
}

void CDirLogicSync::ChangeStatusByEvent(TASK_EVENT eType, SYNC_STATUS eStatusType)
{
    DIR_INFO("Change Stauts Before:");
    PrintTaskInfo();

    YLVector<TaskInfo>::iterator iter = m_vecSyncTask.begin();
    for (; iter != m_vecSyncTask.end(); iter ++)
    {
        if (eType == iter->m_Event)
        {
            iter->m_eTaskStatus = eStatusType;
        }
    }

    DIR_INFO("Change Stauts End:");
    PrintTaskInfo();
}

bool CDirLogicSync::RunSyncByStatus(SYNC_STATUS eStatusType, bool bIsNeedClearTyrCount /*= false*/)
{
    for (int i = 0; i < m_vecSyncTask.size();)
    {
        if (eStatusType == m_vecSyncTask[i].m_eTaskStatus)
        {
            m_vecSyncTask[i].m_eTaskStatus = SYNC_READY;
            REASONCODE eResult = RunSyncByType(m_vecSyncTask[i].m_Event);
            if (REASON_NETERROR == eResult)
            {
                ChangeStatusByEvent(m_vecSyncTask[i].m_Event, SYNC_FAIL);
                if (bIsNeedClearTyrCount)
                {
                    ResetTryCount();
                }
            }
            else if (REASON_OTHERERROR == eResult)
            {
                m_vecSyncTask.removeAt(i);
                if (bIsNeedClearTyrCount)
                {
                    ResetTryCount();
                }
                continue;
            }
        }

        i++;
    }
    return true;
}

bool CDirLogicSync::IsExistTask(TASK_EVENT eType)
{
    for (int i = 0; i < m_vecSyncTask.size(); i++)
    {
        if (eType == m_vecSyncTask[i].m_Event)
        {
            return true;
        }
    }

    return false;
}

void CDirLogicSync::ExportFileByType(TASK_EVENT eType, yl::string strFilePath)
{
    if (TASK_LOCALDIR_UPLOAD == eType)
    {
        g_pLocalDirManager->ExportContactAndGroup(strFilePath);
    }
    else if (TASK_LOCALLOG_UPLOAD == eType)
    {
        g_pLocalLogManager->ExportContactToFile(strFilePath);
    }
}

void CDirLogicSync::RunImportDelay()
{
    if (UNDEFINE_TIMERID != m_iImportTimer)
    {
        timerKillThreadTimer((unsigned int)&m_iImportTimer);
    }
    m_iImportTimer = knImportTimer;
    timerSetThreadTimer((unsigned int)&m_iImportTimer, m_iImportTimer);
}

void CDirLogicSync::PrintTaskInfo()
{
    for (int i = 0; i < m_vecSyncTask.size(); i++)
    {
        DIR_INFO(" m_vecSyncTask[%d] TaskStatus[%d] Event[%d]",
                 i, m_vecSyncTask[i].m_eTaskStatus, m_vecSyncTask[i].m_Event);
    }
}

bool CDirLogicSync::IsNeedAddServerType(yl::string & strSyncPath)
{
    union
    {
        struct sockaddr addr;
        struct sockaddr_in6 addr6;
        struct sockaddr_in addr4;
    } addr;
    memset(&addr, 0, sizeof(addr));
    yl::string::size_type size = strSyncPath.find_first_of('/');
    yl::string strTemp = strSyncPath.substr(0, size);

    if (1 == inet_pton(AF_INET, strTemp.c_str(), &addr.addr4.sin_addr)
            || 1 == inet_pton(AF_INET6, strTemp.c_str(), &addr.addr6.sin6_addr))
    {
        return true;
    }

    return false;
}

void CDirLogicSync::AddServerType(yl::string & strSyncPath)
{
    yl::string strTemp;
    yl::string strDefultServerType = configParser_GetConfigString(kszDefultServerType);
    if (strDefultServerType == "0")
    {
        strTemp = "tftp://";
    }
    else if (strDefultServerType == "1")
    {
        strTemp = "http://";
    }
    else if (strDefultServerType == "2")
    {
        strTemp = "https://";
    }
    else if (strDefultServerType == "3")
    {
        strTemp = "ftp://";
    }
    else
    {
        //默认为tftp
        strTemp = "tftp://";
    }
    strSyncPath = strTemp + strSyncPath;
}

void CDirLogicSync::NotifyAutopSync(TASK_EVENT eType, yl::string strURL, yl::string strFilePath)
{
    DIR_INFO("RunSyncAction: Event[%d] URL[%s] FilePath[%s]", eType, strURL.c_str(),
             strFilePath.c_str());
    UINT uMessage;
    LPARAM lParam;
    yl::string strPath, strUser, strPassword;
    memset(&m_ExSyncInfo, 0, sizeof(atp_ex_url_info));
    switch (eType)
    {
    case TASK_LOCALDIR_UPLOAD:
        {
            uMessage = ATP_MSG_REQ_UPLOAD_FILE_BY_INFO;
            lParam = UPLOAD_MSG_LPARAM_LOCALDIR;
            m_ExSyncInfo.encrpty = configParser_GetConfigInt(kszDirEncryptionEnable);
        }
        break;
    case TASK_LOCALLOG_UPLOAD:
        {
            uMessage = ATP_MSG_REQ_UPLOAD_FILE_BY_INFO;
            lParam = UPLOAD_MSG_LPARAM_LOCALLOG;
            m_ExSyncInfo.encrpty = configParser_GetConfigInt(kszCallLogEncryptionEnable);
        }
        break;
    default:
        {
            DIR_INFO("NotifyAutopSync TASK ERROR [%d]", eType);
            return;
        }
        break;
    }

    ExportFileByType(eType, strFilePath);

    memcpy(m_ExSyncInfo.url, strURL.c_str(), sizeof(char) * strURL.length());
    memcpy(m_ExSyncInfo.src_path, strFilePath.c_str(), sizeof(char) * strFilePath.length());
    yl::string strRemoteFile = pathGetFileNamePos(strFilePath.c_str());
    memcpy(m_ExSyncInfo.remote_name, strRemoteFile.c_str(), sizeof(char) * strRemoteFile.length());

    strUser = configParser_GetConfigString(kszAutoProvisionUsername);
    strPassword = configParser_GetConfigString(kszAutoProvisionPassword);
    memcpy(m_ExSyncInfo.username, strUser.c_str(), sizeof(char) * strUser.length());
    memcpy(m_ExSyncInfo.password, strPassword.c_str(), sizeof(char) * strPassword.length());

    Autop_NotifyAutoServerEx(false, uMessage, 0, lParam,
                             sizeof(atp_ex_url_info), (LPCVOID)&m_ExSyncInfo);
}

bool CDirLogicSync::HaveTaskInfo(TASK_EVENT eType)
{
    bool bHaved = false;
    YLVector<TaskInfo>::iterator iter = m_vecSyncTask.begin();
    for (; iter != m_vecSyncTask.end(); iter++)
    {
        if (eType == iter->m_Event)
        {
            bHaved = true;
            break;
        }
    }
    return bHaved;
}

void CDirLogicSync::ProcessLocalLogUpload()
{
    // 开关打开才上传
    if (IsLocalLogSyncEnable())
    {
        TaskInfo newTask;
        newTask.m_Event = TASK_LOCALLOG_UPLOAD;
        newTask.m_eTaskStatus = SYNC_READY;
        AddTask(newTask);
        RunLocalLogUploadDelayTimer();
    }
    else
    {
        //本次开关关闭，如果存在正在等待task，立即上传一次
        RunSyncByType(TASK_LOCALLOG_UPLOAD);
    }
}

void CDirLogicSync::ProcessCallLogTask()
{
    //timerKillThreadTimer((UINT)&m_iLocalLogDelayTimer);
    //m_iLocalLogDelayTimer = UNDEFINE_TIMERID;
    DIR_INFO("CDirLogicSync::ProcessCallLogTask");
    RunSyncByStatus(SYNC_TALK, true);
}

void CDirLogicSync::ResetDelayTimer()
{
    if (m_iLocalLogDelayTimer != UNDEFINE_TIMERID)
    {
        timerKillThreadTimer((UINT)&m_iLocalLogDelayTimer);
        m_iLocalLogDelayTimer = UNDEFINE_TIMERID;
    }
}
