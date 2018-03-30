#ifndef _USB_IDLE_HANDLE_H_
#define _USB_IDLE_HANDLE_H_

#include "baseidlehandle.h"

#ifdef IF_USB_SUPPORT
class CUSBHandle : public CBaseIdleHandle
{
public:
    CUSBHandle(int nType = PS_STATE_USB_DISCONNECT);
    virtual ~CUSBHandle();
    static LRESULT OnUSBMessage(msgObject & msg);

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
    // 获取显示的图标信息
    virtual bool GetIconInfo(IdleIconInfo & IconData);
};
#endif

#endif //end of _USB_IDLE_HANDLE_H_

