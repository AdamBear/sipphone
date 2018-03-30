#include "moddsskeymock.h"

#include "moddsskey.h"


DSSKeyMock * g_pDSSKeyMock = NULL;

void dsskey_EHSCtrl(int iIndex, int iCtrlCode)
{
    if (g_pDSSKeyMock)
    {
        g_pDSSKeyMock->EHSCtrl(iIndex, iCtrlCode);
    }
}

bool dsskey_IsDsskey(int iKeyCode, bool bExp /*= false*/)
{
    return g_pDSSKeyMock && g_pDSSKeyMock->IsDsskey(iKeyCode, bExp);
}
