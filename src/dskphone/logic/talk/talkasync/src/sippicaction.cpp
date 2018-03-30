#include "talkasync_inc.h"

//////////////////////////////////////////////////////////////////////////
// Class CContactRemoteAction.
CSipPicAction::CSipPicAction(const yl::string& strLocalFilePath,
                             const yl::string& strRemoteFilePath)
    : CNetworkFileAction(strLocalFilePath, strRemoteFilePath)
{
    m_eTARequestType = NS_TA::TA_REQUEST_SIP_PIC;
}

CSipPicAction::~CSipPicAction()
{

}

bool CSipPicAction::Exec()
{
    // 调用基类进行下载.
    CNetworkFileAction::Exec();

    bool isSucc = false;
    // 如果下载成功, 则解析.
    if (IsRead() && IsOperationSucceed())
    {
        //To Do:
        //qt 压缩下图片
        isSucc = true;

        if (!isSucc)
        {
            remove(GetFilePathRemote().c_str());
        }
    }

    SetTaskActionErrorType(isSucc ? NS_TA::TA_NO_ERROR : NS_TA::TA_OTHER_ERROR);
    return isSucc;
}




