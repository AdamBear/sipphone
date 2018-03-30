#include "fileaction.h"


//////////////////////////////////////////////////////////////////////////
// Class CLocalFileAction.
CFileAction::CFileAction(const yl::string & strFilePath)
    : CTaskAction(),
      m_strFilePath(strFilePath)
{

}

CFileAction::~CFileAction()
{

}

bool CFileAction::Exec()
{
    bool isSucceed = false;
    if (IsRead())  // 读文件.
    {
        isSucceed = Read();
    }
    else  // 写文件.
    {
        isSucceed = Write();
    }

    return isSucceed;
}

