#include "modusbheadset.h"
#include "../src/usbheadset.h"

#ifdef IF_SUPPORT_USB_AUDIO
void USBHeadset_Init()
{
    _USBHeadset.Init();
}

void USBHeadset_PlayRing()
{
    _USBHeadset.SetRingState(1);
}

void USBHeadset_StopRing()
{
    _USBHeadset.SetRingState(0);
}

bool USBHeadset_IsConnect()
{
    return _USBHeadset.IsConnect();
}
#else
void USBHeadset_Init()
{
}

void USBHeadset_PlayRing()
{
}

void USBHeadset_StopRing()
{
}
bool USBHeadset_IsConnect()
{
    return false;
}
#endif // IF_SUPPORT_USB_AUDIO
