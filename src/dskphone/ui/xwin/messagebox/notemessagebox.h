#ifndef __NOTE_MESSAGEBOX_H__
#define __NOTE_MESSAGEBOX_H__

#include "messageboxbase.h"


class CNoteMessageBox : public CMessageBoxBase
{
public:
    CNoteMessageBox();
    ~CNoteMessageBox();

    static CMessageBoxBase * CreateNoteMsgBox(const MSGBOX_INFO & stMsgInfo);

#if IF_MSG_BOX_SHOW_ICON
private:
    //标志是否需要显示图片
    bool                m_bShowIcon;
#endif
};

#endif // __NOTE_MESSAGEBOX_H__
