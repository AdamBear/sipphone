#include "genbandbookaction.h"

#if IF_FEATURE_GBBOOK
void CGenbandBookAction::SetData(BaseDirDataRecord * pData)
{
    m_pData = pData;
}


CGenbandBookAction::CGenbandBookAction(const yl::string & strLocalFilePath,
                                       const yl::string & strRemoteFilePath, GBBOOK_OPERATION_TYPE eType)
    : CNetworkFileAction(strLocalFilePath, strRemoteFilePath)
{
    // 设置TaskAction的类型.
    m_eTARequestType = NS_TA::TA_REQUEST_NETWORK_FILE;

    m_eType = eType;

    m_strGabKey = "";
    //m_pHelperEx = new NS_TA::CNetworkFileActionDownloadHelper();
}

CGenbandBookAction::~CGenbandBookAction()
{
    if (m_pData != NULL)
    {
        delete m_pData;
    }
    //DELETE_OBJECT(m_pHelperEx);
}

void CGenbandBookAction::SetGabKey(const yl::string & strKey)
{
    m_strGabKey = strKey;
}

#if 0
void CGenbandBookAction::SetDownloadMode(TASKACTION_DL_MODE eMode)
{
    m_eDownLoadMode = eMode;
}

TASKACTION_DL_MODE CGenbandBookAction::GetDownlodMode()
{
    return m_eDownLoadMode;
}

bool CGenbandBookAction::Write()
{
    // 如果下载助手未实例化, 则返回TA_MEMORY_ERROR错误.
    if (m_pHelperEx == NULL)
    {
        SetTaskActionErrorType(NS_TA::TA_MEMORY_ERROR);
        return false;
    }

    m_pHelperEx->SetRemoteFilePath(m_strFilePathRemote);
    m_pHelperEx->SetSavedFilePath(GetFilePath());
    m_pHelperEx->SetFileUsername(m_strUsername);
    m_pHelperEx->SetFilePassword(m_strPassword);
    m_pHelperEx->SetCharset(m_strCharset);
    m_pHelperEx->SetDownLoadMode(GetDownlodMode());

    yl::string strHead;
    strHead = commonAPI_FormatString("application/xml;charset=%s\r\n%s", m_strCharset.c_str(),
                                     "SOAPAction: NA");
    m_pHelperEx->SetHeader(strHead);

    NS_TA::TaskActionErrorType eResult = NS_TA::TA_FILE_OPERATION_FALIED;
    if (IsDelete())
    {
        eResult = m_pHelperEx->TA_DeleteFile();
    }
    else
    {
        eResult = m_pHelperEx->TA_PostFile(IsPut());
    }

    if (eResult != NS_TA::TA_NO_ERROR)
    {
        SetTaskActionErrorType(eResult);
        return false;
    }

    SetTaskActionErrorType(NS_TA::TA_NO_ERROR);

    return true;
}
#endif
#endif
