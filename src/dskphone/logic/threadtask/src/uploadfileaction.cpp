#include "uploadfileaction.h"

#include "networkfileactionhelper.h"

CUploadFileAction::CUploadFileAction(const yl::string & strLocalFilePath,
                                     const yl::string & strServerURL)
    : CNetworkFileAction(strLocalFilePath, strServerURL)
{
}

CUploadFileAction::~CUploadFileAction()
{

}

// 重写. 读文件.
bool CUploadFileAction::Read()
{
    return false;
}

// 重写. 写文件.
bool CUploadFileAction::Write()
{
    // 如果下载助手未实例化, 则返回TA_MEMORY_ERROR错误.
    CNetworkFileActionDownloadHelper * pHelper = GetDownloadHelper();
    if (pHelper == NULL)
    {
        SetTaskActionErrorType(TA_MEMORY_ERROR);
        return false;
    }

    pHelper->SetRemoteFilePath(GetFilePathRemote());
    pHelper->SetSavedFilePath(GetFilePath());
    pHelper->SetFileUsername(m_strUsername);
    pHelper->SetFilePassword(m_strPassword);
    pHelper->SetCharset(m_strCharset);

    TaskActionErrorType eResult = pHelper->UploadFile(IsPut());
    if (eResult != TA_NO_ERROR)
    {
        SetTaskActionErrorType(eResult);
        return false;
    }

    SetTaskActionErrorType(TA_NO_ERROR);

    return true;
}

