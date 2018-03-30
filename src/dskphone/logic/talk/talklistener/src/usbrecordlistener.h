#ifndef __USB_RECORD_LISTENER_H__
#define __USB_RECORD_LISTENER_H__
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)

#include "basetalklistener.h"

/* usb record 业务 */

class CUSBRecordListener : public CBaseTalkListener
{
public:
    CUSBRecordListener();
protected:
    DECL_SIGNAL_HANDLER(OnRoutineChanged);
    DECL_SIGNAL_HANDLER(OnHoldChange);
    DECL_SIGNAL_HANDLER(OnSessionExit);
    DECL_SIGNAL_HANDLER(OnTalkToIdle);
};

#endif // (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#endif // __USB_RECORD_LISTENER_H__
