#include "messageboxcallback.h"
#include "messageboxmanager.h"


///////////////////////////////////CMsgBoxCallBackAction///////////////////////////////////////
CMsgBoxCallBackAction::CMsgBoxCallBackAction()
{

}

CMsgBoxCallBackAction::~CMsgBoxCallBackAction()
{
#ifndef _WIN32
#warning POPUPBOX_INFO
#endif
    //POPUPBOX_INFO("RemoveMessageBox By ~CMsgBoxCallBackAction");

    _MessageBoxManager.RemoveMessageBox(this, MessageBox_Destory, false);
}


///////////////////////////////////CMsgBoxCallBackFunWrapper///////////////////////////////////////
CMsgBoxCBFunWrapper::CMsgBoxCBFunWrapper(MSGBOX_CALLBACK_FUN pCallBackFun)
    : m_pCallBackFun(pCallBackFun)
{

}



CMsgBoxCBFunWrapper::~CMsgBoxCBFunWrapper()
{

}

void CMsgBoxCBFunWrapper::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL != pMessageBox
            && NULL != m_pCallBackFun)
    {
        m_pCallBackFun(pMessageBox);
    }

#ifndef _WIN32
#warning deleteLater
#endif
    //deleteLater();
    delete (this);
}
