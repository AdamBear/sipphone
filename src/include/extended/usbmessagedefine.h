#ifndef _USBMESSAGEDEFINE__INCLUDE_H_
#define _USBMESSAGEDEFINE__INCLUDE_H_

#include "innermsgdefine.h"

enum UsbMessageDefine
{
    USB_MESSAGE_BEGIN = INNER_MSG_USBHEADSET_MESSAGE_BEGIN,
    USB_MSG_HEADSET_CONNECT_STATE,      // usb的连接状态               wParam:1:connect 0:disconnect 2: connecting
    USB_MSG_HEADSET_PLUG,               // usb插入或者拔出                wparam:1:in 0:out
    USB_MSG_HEADSET_KEY,                // vpm上报的usb耳机按键消息
    USB_MSG_HEADSET_ACTION,             // usb命令执行
    USB_MSG_HEADSET_ACTION_RESULT,      // usb命令执行结果
    USB_MSG_HEADSET_CMD,                // usb发送命令
    USB_MESSAGE_END = INNER_MSG_USBHEADSET_MESSAGE_END,
};

#endif //(_USBMESSAGEDEFINE__INCLUDE_H_)
