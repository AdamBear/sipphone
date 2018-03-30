#ifndef __MIDDLE_MESSAGEBOX_H__
#define __MIDDLE_MESSAGEBOX_H__

#include "commonmessagebox.h"

class CMiddleMessageBox : public CCommonMessageBox
{
public:
    CMiddleMessageBox();
    virtual ~CMiddleMessageBox();

    static CMessageBoxBase* CreateMiddleMsgBox(const MSGBOX_INFO & stMsgInfo);

};

#endif // __MIDDLE_MESSAGEBOX_H__
