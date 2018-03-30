#include "notemessagebox.h"
#include "xwindow/xWindowManager.h"


namespace
{
#define MSG_BOX_LAYOUT_FILE_NOTE "messagebox/notemsgbox.xml"
}

CNoteMessageBox::CNoteMessageBox()
#if IF_MSG_BOX_SHOW_ICON
    : m_bShowIcon(false)
#endif
{
    SetType(MESSAGEBOX_NOTE);
}

CNoteMessageBox::~CNoteMessageBox()
{

}

CMessageBoxBase * CNoteMessageBox::CreateNoteMsgBox(const MSGBOX_INFO & stMsgInfo)
{
    CNoteMessageBox * pNoteMsgBox = new CNoteMessageBox;

    if (NULL != pNoteMsgBox)
    {
        pNoteMsgBox->loadContent(MSG_BOX_LAYOUT_FILE_NOTE);

        pNoteMsgBox->CreateWindowObject(true);

        pNoteMsgBox->SetID(stMsgInfo.iID);
        pNoteMsgBox->SetExtraData(stMsgInfo.extraData);
        pNoteMsgBox->SetExitTime(stMsgInfo.nMsTime);
        pNoteMsgBox->SetTitle(stMsgInfo.strTitle);
        pNoteMsgBox->SetMessage(stMsgInfo.strNote);
    }

    return pNoteMsgBox;
}
