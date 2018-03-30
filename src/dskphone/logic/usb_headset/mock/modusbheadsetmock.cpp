#include "modusbheadsetmock.h"

#include "modusbheadset.h"


USBHeadsetMock * g_pUSBHeadsetMock = NULL;

void USBHeadset_StopRing()
{
    if (g_pUSBHeadsetMock)
    {
        g_pUSBHeadsetMock->StopRing();
    }
}
