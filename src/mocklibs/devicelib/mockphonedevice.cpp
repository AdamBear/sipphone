#include "mockphonedevice.h"

MockPhoneDevice * g_pMockPhoneDevice = NULL;

PHONE_TYPE_DEF devicelib_GetPhoneType()
{
    return g_pMockPhoneDevice ? g_pMockPhoneDevice->GetPhoneType() : PT_UNKNOWN;
}
