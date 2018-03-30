#include "ringfileaction.h"
#include "taskaction/taskactioncallbackbase.h"
#include "threadtask/include/networkfileactionhelper.h"



//////////////////////////////////////////////////////////////////////////
// Class CRingFileAction.
CRingFileAction::CRingFileAction(const yl::string & strFilePath)
    : CTaskAction(),
      m_strFilePath(strFilePath),
      m_nRingIndex(knInvalidRingFileIndex),
      m_pHelper(NULL)
{
    m_eTARequestType = TA_REQUEST_RING_FILE;

    // 实例化下载助手.
    if (m_pHelper == NULL)
    {
        m_pHelper = new CNetworkFileActionDownloadHelper;
    }
}

CRingFileAction::~CRingFileAction()
{
    SAFE_DELETE(m_pHelper);
}

void CRingFileAction::SetExtraInfo(const yl::string & strExtraInfo)
{
    m_strExtraInfo = strExtraInfo;
}

bool CRingFileAction::Exec()
{
    bool isSucceed = false;
    if (IsRead())  // 读取铃声文件.
    {
        isSucceed = Download();
    }
    else  // 写入铃声文件.
    {
        isSucceed = Upload();
    }
    return isSucceed;
}

bool CRingFileAction::Download()
{
    if (m_pHelper == NULL)
    {
        SetTaskActionErrorType(TA_MEMORY_ERROR);
        return false;
    }

    m_nRingIndex = knInvalidRingFileIndex;
    m_strLocalSavedPath.clear();

    // 下载铃声文件.
    m_pHelper->SetRemoteFilePath(m_strFilePath);  // 设置远程下载路径.
    m_pHelper->SetExtraInfo(m_strExtraInfo);
    TaskActionErrorType eResult = m_pHelper->DownloadRingFile(
                                      &m_nRingIndex, &m_strLocalSavedPath);
    SetTaskActionErrorType(eResult);

    return eResult == TA_NO_ERROR;
}

bool CRingFileAction::Upload()
{
    // 铃声文件上传, 当前不支持.
    SetTaskActionErrorType(TA_FILE_OPERATION_NOT_SUPPORTED);
    return false;
}
