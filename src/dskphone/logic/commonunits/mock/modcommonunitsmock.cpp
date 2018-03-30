#include "modcommonunitsmock.h"

#include "../modcommonunits.h"


CommonUnitsMock * g_pCommonUnitsMock = NULL;

int commonUnits_SendSimulateKey(UINT uMessage, int iKey, bool bExp /*= false*/)
{
    return g_pCommonUnitsMock->SendSimulateKey(uMessage, iKey, bExp);
}

void commonUnits_GetDefaultRing(yl::string & strRingPath)
{
    if (g_pCommonUnitsMock)
    {
        g_pCommonUnitsMock->GetDefaultRing(strRingPath);
    }
}
