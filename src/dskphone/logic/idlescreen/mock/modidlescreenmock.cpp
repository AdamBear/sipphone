#include "modidlescreenmock.h"


IdleScreenMock * g_pIdleScreenMock = NULL;

bool idleScreen_SetStatus(int nStatus, const yl::string strHint /*= ""*/)
{
    return g_pIdleScreenMock->SetStatus(nStatus, strHint);
}

bool idleScreen_DropStatus(int nStatus, bool bNeedNotify /*= true*/)
{
    return g_pIdleScreenMock->DropStatus(nStatus, bNeedNotify);
}
