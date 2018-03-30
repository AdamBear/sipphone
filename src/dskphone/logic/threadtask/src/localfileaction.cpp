#include <stdio.h>

#include "localfileaction.h"
//#include <fstream>
#include "taskaction/taskactioncallbackbase.h"


CLocalFileAction::CLocalFileAction(const yl::string & strLocalFilePath)
    : CFileAction(strLocalFilePath)
{
    // 初始化基类的类型值为本地文件请求.
    m_eTARequestType = TA_REQUEST_LOCAL_FILE;
}

CLocalFileAction::~CLocalFileAction()
{

}

bool CLocalFileAction::Exec()
{
    // 直接调用基类的Exec().
    return CFileAction::Exec();
}

bool CLocalFileAction::Read()
{
    // 本件路径是否为空.
    if (GetFilePath().empty())
    {
        SetTaskActionErrorType(TA_FILE_INVALID_PATH);
        return false;
    }

    // 文件是否可以打开.
    FILE * ifFile = fopen(GetFilePath().c_str(), "r");
    if (!ifFile)
    {
        SetTaskActionErrorType(TA_FILE_FAILED_TO_OPEN);
        return false;
    }

    // 获取文件长度.
    fseek(ifFile, 0L, SEEK_END);
    int nLength = ftell(ifFile);
    fseek(ifFile, 0L, SEEK_SET);
    if (nLength <= 0)
    {
        SetTaskActionErrorType(TA_FILE_NO_DATA_WHEN_READ);
        fclose(ifFile);
        return false;
    }

    // 分配buffer.
    char * pszBuffer = new char [nLength];
    if (pszBuffer == NULL)
    {
        SetTaskActionErrorType(TA_MEMORY_ERROR);
        fclose(ifFile);
        return false;
    }

    // 将文件内容写入Buffer
    fread(pszBuffer, 1, nLength, ifFile);
    fclose(ifFile);

    SetFileContent(pszBuffer);
    DELETE_ARRAY(pszBuffer);
    SetTaskActionErrorType(TA_NO_ERROR);

    return true;
}

bool CLocalFileAction::Write()
{
    // 文件路径为空.
    if (GetFilePath().empty())
    {
        SetTaskActionErrorType(TA_FILE_INVALID_PATH);
        return false;
    }

    // 没有内容可保存.
    if (GetFileContent().empty())
    {
        SetTaskActionErrorType(TA_FILE_NO_DATA_SPECIFIED_WHEN_WRITE);
        return false;
    }

    // 打开文件
    FILE * ofFile = fopen(GetFilePath().c_str(), "w");
    if (!ofFile)
    {
        SetTaskActionErrorType(TA_FILE_FAILED_TO_OPEN);
        return false;
    }

    // 将新内容写入到文件中.
    fwrite(GetFileContent().c_str(), 1, GetFileContent().length(), ofFile);
    fclose(ofFile);

    SetTaskActionErrorType(TA_NO_ERROR);

    // 回调.
    if (CTaskActionCallbackBase * pCallbackBase = GetCallback())
    {
        pCallbackBase->OnTaskActionCallback(this);
    }

    return true;
}


