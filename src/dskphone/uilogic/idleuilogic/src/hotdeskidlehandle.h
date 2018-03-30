#ifndef _HOTDESK_IDLE_HANDLE_H_
#define _HOTDESK_IDLE_HANDLE_H_

#include "baseidlehandle.h"

class CHotdesktHandle : public CBaseIdleHandle
{
public:
    CHotdesktHandle(int nType = PS_STATE_NOTE_HOTDESK);
    virtual ~CHotdesktHandle();

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
};

#endif //end of _HOTDESK_IDLE_HANDLE_H_

