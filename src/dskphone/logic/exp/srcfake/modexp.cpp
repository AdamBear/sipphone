#include "modexp.h"


bool exp_IsTestMode()
{
    return false;
}


int exp_GetExpCounts(DEVICE_TYPE_DEF eType /*= DT_ALL*/)
{
    return -1;
}


DEVICE_TYPE_DEF exp_GetExpTypeByIndex(int iExpID)
{
    return DT_INVALID;
}

int exp_GetCurrentPageByIndex(int iExpID)
{
    return 0;
}

bool exp_SetCurrentPageByIndex(int iExpID, int iPage)
{
    return false;
}

int exp_GetEhsIndex()
{
    return 0;
}

void exp_EHSCtrl(int nIndex, int nCtrlCode)
{
    return;
}

bool exp_SetExpTestMode(bool bEnable)
{
    return false;
}

void exp_Init(EXP_DRIVE_TYPE eDriveType /*= EXP_DRIVE_TYPE_INVALID*/)
{
    return;
}

