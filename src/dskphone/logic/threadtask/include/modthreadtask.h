#ifndef _THREAD_TASK_ASST_H__
#define _THREAD_TASK_ASST_H__
#include "taskaction/modtaskaction.h"
#include "threadtask/include/common.h"
#ifdef __cplusplus
#define TA_EXPORT_C extern "C"
#else
#define TA_EXPORT_C
#endif

class CLdapAction;
class CLocalFileAction;
class CNetworkFileAction;
class CRingFileAction;
class CUploadFileAction;

// ThreadTaskAsst 初始化

TA_EXPORT_C void TaskAction_InitDLConf();


// 创建CLocalFileAction实例.
// | pszLocalFilePath | [in] 是本地文件路径.
// 创建成功返回CLocalFileAction实例指针, 否则返回NULL.
TA_EXPORT_C CLocalFileAction * TaskAction_CreateLocalFileAction(
    const char * pszLocalFilePath);


// 创建CNetworkFileAction实例.
// | pszLocalFilePath | [in] 为远程文件的本地保存路径.
// | pszRemoteFilePath | [in] 为远程文件的路径.
// 创建成功返回CNetworkFileAction实例指针, 否则返回NULL.
TA_EXPORT_C CNetworkFileAction * TaskAction_CreateNetworkFileAction(
    const char * pszLocalFilePath, const char * pszRemoteFilePath);


// 创建CRingFileAction实例.
// | pszRingFilePath | [in] 为铃声的路径.
// 创建成功返回CRingFileAction实例指针, 否则返回NULL.
TA_EXPORT_C CRingFileAction * TaskAction_CreateRingFileAction(
    const char * pszRingFilePath);

// 证书是否安装成功.
TA_EXPORT_C bool TaskAction_Be_Install_Ca(char * pStrFile, char * pStrPath);


// 创建CUploadFileAction实例.
// | pszLocalFilePath | [in] 为需要上传的本地文件路径.
// | pszServerURL | [in] 为远程服务器地址.
// 创建成功返回CUploadFileAction实例指针, 否则返回NULL.
TA_EXPORT_C CUploadFileAction * TaskAction_CreateUploadFileAction(
    const char * pszLocalFilePath, const char * pszServerURL);

TA_EXPORT_C void TaskAction_DestroyAction(CTaskAction * pAction);
#endif //_THREAD_TASK_ASST_H__
