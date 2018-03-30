#include "modthreadtask.h"
#include "ldapaction.h"
#include "localfileaction.h"
#include "networkfileaction.h"
#include "ringfileaction.h"
#include "uploadfileaction.h"
#include "networkfileactionhelper.h"
#include <devicelib/networkserver.h>
#include "dl/dl.h"
/************************************************************************/
//    函数名    ：    TaskAction_CreateLocalFileAction
//    功能    ：    创建一个LocalFile实例
//    参数    ：    pszLocalFilePath【in】     本地文件路径
//    返回值    ：    CLocalFileAction的实例指针
/************************************************************************/
CLocalFileAction * TaskAction_CreateLocalFileAction(const char * pszLocalFilePath)
{
    if (pszLocalFilePath == NULL)
    {
        return NULL;
    }

    // 创建CLocalFileAction实例.
    CLocalFileAction * pAction = new CLocalFileAction(pszLocalFilePath);
    return pAction;
}

/************************************************************************/
//    函数名    ：    TaskAction_CreateNetworkFileAction
//    功能    ：    创建Network实例
//    参数    ：    pszLocalFilePath【in】    本地文件路径
//                pszRemoteFilePath【in】    远程服务器文件路径
//    返回值    ：    CNetworkFileAction的实例指针
/************************************************************************/
CNetworkFileAction * TaskAction_CreateNetworkFileAction(
    const char * pszLocalFilePath, const char * pszRemoteFilePath)
{
    if (pszLocalFilePath == NULL || pszRemoteFilePath == NULL)
    {
        return NULL;
    }

    // 创建CNetworkFileAction实例.
    CNetworkFileAction * pAction =
        new CNetworkFileAction(pszLocalFilePath, pszRemoteFilePath);

    return pAction;
}


/************************************************************************/
//    函数名    ：    TaskAction_CreateRingFileAction
//    功能    ：    创建一个RingFile实例
//    参数    :    pszRingFilePath【in】  铃声文件路径
//    返回值    ：    CRingFileAction的实例指针
/************************************************************************/
CRingFileAction * TaskAction_CreateRingFileAction(const char * pszRingFilePath)
{
    if (pszRingFilePath == NULL)
    {
        return NULL;
    }

    // 创建CRingFileAction实例.
    CRingFileAction * pAction = new CRingFileAction(pszRingFilePath);
    return pAction;
}

// 证书是否安装成功.
TA_EXPORT_C bool TaskAction_Be_Install_Ca(char * pStrFile, char * pStrPath)
{
    dl_ca_info_ex info_ex;
    dl_response_t ret_response;
    memset(&info_ex, 0, sizeof(dl_ca_info_ex));

    // 获取证书信息
    /* cert_flags证书类型，普通CA为0；客户端证书DL_CLIENT_CERT */
    if (0 == dl_get_ca_info_ex(pStrFile, &info_ex, DL_CLIENT_CERT, &ret_response))
    {
        yl::string strMac = netGetMACText('\0');
        yl::string strIssued_to = info_ex.issued_to;
        if (strIssued_to.MakeLower() == strMac.MakeLower())
        {
            // 确认证书是否安装
            if (0 == dl_cert_verify(pStrFile, pStrPath, NULL, DL_CLIENT_CERT, &ret_response))
            {
                return true;
            }
        }
    }
    return false;
}

// 创建CUploadFileAction实例.
// | pszLocalFilePath | [in] 为需要上传的本地文件路径.
// | pszServerURL | [in] 为远程服务器地址.
// 创建成功返回CUploadFileAction实例指针, 否则返回NULL.
CUploadFileAction * TaskAction_CreateUploadFileAction(
    const char * pszLocalFilePath, const char * pszServerURL)
{
    if (pszLocalFilePath == NULL || pszServerURL == NULL)
    {
        return NULL;
    }

    // 创建CNetworkFileAction实例.
    CUploadFileAction * pAction =
        new CUploadFileAction(pszLocalFilePath, pszServerURL);

    return pAction;
}

TA_EXPORT_C void TaskAction_DestroyAction(CTaskAction * pAction)
{
    if (pAction == NULL)
    {
        return;
    }
}


// ThreadTaskAsst 初始化

void TaskAction_InitDLConf()
{
    InitDLConf();
}

