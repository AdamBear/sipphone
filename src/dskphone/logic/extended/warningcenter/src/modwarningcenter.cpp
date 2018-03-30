#include "extended/warningcenter/include/modwarningcenter.h"
#include "warningcenter.h"

bool WarningCenter_Init()
{
    _WarningManager.Init();
    return true;
}

bool WarningCenter_IsWarningModeEnable()
{
    return _WarningManager.IsWarningModeEnable();
}

int WarningCenter_GetWarning(YLVector<int>& vecWarning)
{
    return _WarningManager.GetWarningList(vecWarning);
}

void WarningCenter_RemoveWarning(int iWarning)
{
    _WarningManager.RemoveWarning(iWarning);
}

void WarningCenter_CancelWarning()
{
    _WarningManager.SetWarningStatus(false);
}
