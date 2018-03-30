#include "modmessagebox.h"

#include "messageboxcache.h"

#include "notemessagebox.h"
#include "commonmessagebox.h"
#include "passwordmessagebox.h"

#include "translateiddefine.h"
#include "languagehelper/modlangtranslate.h"

void MessageBox_RegisterMsgBox(MESSAGEBOX_TYPE eMsgBoxType, FUN_CREATE_MSGBOX pFunCreateMsgBox)
{
    _MessageBoxManager.RegisterMsgBox(eMsgBoxType, pFunCreateMsgBox);
}

void MessageBox_ShowCommonMsgBox(const MSGBOX_INFO & stMsgInfo)
{
    CMessageBoxBase * pMessageBox = _MessageBoxManager.ApplyMsgBox(stMsgInfo);

    MessageBox_ShowMessageBox(stMsgInfo.pAction, pMessageBox);
}

void MessageBox_ShowMessageBox(CMsgBoxCallBackAction * pAction, CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    _MessageBoxManager.AddMessageBox(pMessageBox, pAction);
}

CMessageBoxBase * MessageBox_GetTopMessageBox()
{
    return _MessageBoxManager.GetTopMessageBox();
}

bool MessageBox_RemoveMessageBoxType(MESSAGEBOX_TYPE nType,
                                     MESSAGEBOX_RESULT eResult/* = MessageBox_Cancel*/)
{
    return _MessageBoxManager.RemoveMessageBoxType(nType, eResult);
}

bool MessageBox_RemoveMessageBox(CMsgBoxCallBackAction * pAction,
                                 MESSAGEBOX_RESULT eResult/* = MessageBox_Cancel*/,
                                 int iID/* = -1*/)
{
    if (NULL == pAction)
    {
        return false;
    }

    return _MessageBoxManager.RemoveMessageBox(pAction, eResult, true, iID);
}

bool MessageBox_RemoveTopMessageBox(MESSAGEBOX_RESULT eResult/* = MessageBox_Cancel*/)
{
    return _MessageBoxManager.RemoveTopMessageBox(eResult);
}

bool MessageBox_RemoveAllMessageBox(MESSAGEBOX_RESULT eResult/* = MessageBox_Cancel*/)
{
    return _MessageBoxManager.RemoveAllMessageBox(eResult);
}

int MessageBox_GetMessageBoxCount()
{
    return _MessageBoxManager.GetMessageBoxCount();
}

void MessageBox_ConnettShowMessageSlot(xSlotHandler* pHandler, slotMethod pMethod)
{
    _MessageBoxManager.signalMessageBoxShow.connect(pHandler, pMethod);
}

void MessageBox_DisconnectShowMessageSlot(xSlotHandler* pHandler, slotMethod pMethod)
{
    _MessageBoxManager.signalMessageBoxShow.disconnect(pHandler, pMethod);
}
