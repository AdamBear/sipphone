#include "modexpscan.h"
#include "expscanner.h"
#include "ehslogicmanager.h"
#include "exp/src/expdrive/expdriveheader/drivelogic.h"

#ifdef _EXP_BLACK_SUPPORT
#include "exp/src/expdrawer/expblack/expappblack.h"
#include "taskaction/modtaskaction.h"
#include "commonunits/commonunits_def.h"
#elif defined _XWIN
#include "exp/src/expdrawer/expcolor/expapp.h"
#endif // ! _EXPCOLOR


void exp_Init(EXP_DRIVE_TYPE eDriveType /*= EXP_DRIVE_TYPE_INVALID*/)
{

    if (eDriveType == EXP_DRIVE_TYPE_INVALID)
    {
        EXP_ERR("[Exp-Info] exp_Init failed");
        return;
    }

#ifdef _EXP_BLACK_SUPPORT

    if (eDriveType == EXP_DRIVE_TYPE_RJ)
    {
        _EXPLcdDrawThread;
        TaskAction_CreateSpecialThread(NS_TA::TA_REQUEST_EXP_DRAWER,
                                       TA_THREAD_EXP_DRAWER, OnEXPMessage);
    }

#elif defined _XWIN  // _XWIN

    if (eDriveType == EXP_DRIVE_TYPE_USB)
    {
        _EXPLcdDrawThreadColor.Launch();
    }

#endif // _XWIN

    //驱动线程放到外部初始化，ehs和exp完全分开处理
    DriveInitByType(eDriveType);

#ifdef _EHS_SUPPORT
    if (eDriveType == EXP_DRIVE_TYPE_RJ)
    {
        g_EHSLogicManager.Init();
    }
#endif

#if defined(_EXP_COLOR_SUPPORT) || defined(_EXP_BLACK_SUPPORT)
    g_ExpScanner.Init(eDriveType);
#endif

}

#if defined(_EXP_COLOR_SUPPORT) || defined(_EXP_BLACK_SUPPORT)
bool exp_IsTestMode()
{
    return g_ExpScanner.IsTestMode();
}


int exp_GetExpCounts(DEVICE_TYPE_DEF eType /*= DT_ALL*/)
{
    return g_ExpScanner.GetConnectedExpCounts(eType);
}


DEVICE_TYPE_DEF exp_GetExpTypeByIndex(int iExpID)
{
    return g_ExpScanner.GetExpTypeByExpIndex(iExpID);
}

int exp_GetCurrentPageByIndex(int iExpID)
{
    return g_ExpScanner.GetCurrentPageByExpIndex(iExpID);
}

bool exp_SetCurrentPageByIndex(int iExpID, int iPage)
{
    return g_ExpScanner.SetCurrentPageByIndex(iExpID, iPage);
}

void exp_SetExpTestMode(bool bEnable)
{
    g_ExpScanner.SetTestMode(bEnable);
}

ExpHandle exp_GetExpHandle()
{
    return g_ExpScanner.GetExpHandle();
}
#else
bool exp_IsTestMode()
{
    return false;
}

int exp_GetExpCounts(DEVICE_TYPE_DEF eType /*= DT_ALL*/)
{
    return 0;
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

void exp_SetExpTestMode(bool bEnable)
{
    return;
}
ExpHandle exp_GetExpHandle()
{
    return -1;
}
#endif

#ifdef _EHS_SUPPORT
int exp_GetEhsIndex()
{
    return g_EHSLogicManager.GetEhsIndex();
}

bool exp_InitEHSData()
{
    return g_EHSLogicManager.InitEHSData();
}

void exp_EHSCtrl(int nIndex, int nCtrlCode)
{
    g_EHSLogicManager.EHSCtrl(nIndex, nCtrlCode);
}
#else
int exp_GetEhsIndex()
{
    return -1;
}

bool exp_InitEHSData()
{
    return false;
}

void exp_EHSCtrl(int nIndex, int nCtrlCode)
{
    return;
}
#endif
