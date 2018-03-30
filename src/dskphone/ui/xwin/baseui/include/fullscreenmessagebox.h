#ifndef __FULL_SCREEN_MESSAGEBOX_H__
#define __FULL_SCREEN_MESSAGEBOX_H__

#include "messagebox/messageboxbase.h"

enum
{
    MESSAGEBOX_FULLSCREEN = MESSAGEBOX_END + 2,
};

class CFullScreenMessageBox : public CMessageBoxBase
{
public:
    CFullScreenMessageBox();
    ~CFullScreenMessageBox();

    static CMessageBoxBase * CreateFullScreenMsgBox(const MSGBOX_INFO & stMsgInfo);

public:
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);

};

#endif // __FULL_SCREEN_MESSAGEBOX_H__
