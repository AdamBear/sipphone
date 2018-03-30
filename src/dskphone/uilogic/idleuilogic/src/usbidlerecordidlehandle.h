#ifndef _USB_IDLE_RECORD_IDLE_HANDLE_H_
#define _USB_IDLE_RECORD_IDLE_HANDLE_H_

#include "baseidlehandle.h"

#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
class CUSBIdleRecordHandle : public CBaseIdleHandle
{
public:
    CUSBIdleRecordHandle(int nType = PS_STATE_IDLE_RECORD);
    virtual ~CUSBIdleRecordHandle();
    static LRESULT OnUSBReocrdMessage(msgObject & msg);

public:
    // 获取弹框信息
    virtual bool GetPopupBoxInfo(PopupBoxData & popData);
    // 获取显示的图标信息
    virtual bool GetIconInfo(IdleIconInfo & IconData);
    // 按键处理
    virtual bool HandleKeyProcess(SoftKey_TYPE eSoftkey, PHONE_KEY_CODE eKeyCode);
};
#endif

#endif //end of _USB_IDLE_RECORD_IDLE_HANDLE_H_

