#include "baseui/include/fullscreenmessagebox.h"

namespace
{
#define MSG_BOX_LAYOUT_FILE_FULL_SCREEN "messagebox/fullscreenmsgbox.xml"
}

CFullScreenMessageBox::CFullScreenMessageBox()
{

}

CFullScreenMessageBox::~CFullScreenMessageBox()
{

}

CMessageBoxBase * CFullScreenMessageBox::CreateFullScreenMsgBox(const MSGBOX_INFO & stMsgInfo)
{
    CFullScreenMessageBox * pFullScreenMsgBox = new CFullScreenMessageBox;

    if (NULL != pFullScreenMsgBox)
    {
        pFullScreenMsgBox->loadContent(MSG_BOX_LAYOUT_FILE_FULL_SCREEN);

        pFullScreenMsgBox->CreateWindowObject(true);

        pFullScreenMsgBox->SetID(stMsgInfo.iID);
        pFullScreenMsgBox->SetType(stMsgInfo.eType);
        pFullScreenMsgBox->SetExitTime(stMsgInfo.nMsTime);
        pFullScreenMsgBox->SetMessage(stMsgInfo.strNote);
        pFullScreenMsgBox->SetRemoveByKey(false);
    }

    return pFullScreenMsgBox;
}

bool CFullScreenMessageBox::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    //不允许任何按键通过
    return true;
}
