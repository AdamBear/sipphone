#include "middlemessagebox.h"

namespace
{
#define MSG_BOX_LAYOUT_FILE_MIDDLE "messagebox/middlemsgbox.xml"
}

CMiddleMessageBox::CMiddleMessageBox()
{
}

CMiddleMessageBox::~CMiddleMessageBox()
{
}

CMessageBoxBase* CMiddleMessageBox::CreateMiddleMsgBox(const MSGBOX_INFO& stMsgInfo)
{
    CMiddleMessageBox * pMiddleMsgBox = new CMiddleMessageBox;

    if (NULL != pMiddleMsgBox)
    {
        pMiddleMsgBox->loadContent(MSG_BOX_LAYOUT_FILE_MIDDLE);
        pMiddleMsgBox->CreateWindowObject();
        pMiddleMsgBox->SetID(stMsgInfo.iID);
        pMiddleMsgBox->SetType(stMsgInfo.eType);
        pMiddleMsgBox->SetExtraData(stMsgInfo.extraData);
        pMiddleMsgBox->SetMessage(stMsgInfo.strNote);
        pMiddleMsgBox->SetSoftkey(stMsgInfo.vecSoftkey);
    }

    return pMiddleMsgBox;
}
