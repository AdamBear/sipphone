#include "moddiruilogic.h"
#include "diruiadapter.h"
#include "bsftpwdmgr.h"


void SetFunEnterUI(Fun_EnterUI pEnterUI)
{
    g_DirUIAdapter.SetFunEnterUI(pEnterUI);
}

void SetFunExitCurrentUI(Fun_ExitCurrentUI pExitCurrentUI)
{
    g_DirUIAdapter.SetFunExitCurrentUI(pExitCurrentUI);
}

void SetFunExitAllUI(Fun_ExitAllUI pExitAllUI)
{
    g_DirUIAdapter.SetFunExitAllUI(pExitAllUI);
}

void SetFunReturnRootGroup(Fun_ReturnRootGroup pReturnRootGroup)
{
    g_DirUIAdapter.SetFunReturnRootGroup(pReturnRootGroup);
}

void SetFunCheckState(Fun_CheckState pCheckState)
{
    g_DirUIAdapter.SetFunCheckState(pCheckState);
}

void SetFunShowMsgBox(Fun_ShowMsgBox pShowMsgBox)
{
    g_DirUIAdapter.SetFunShowMsgBox(pShowMsgBox);
}

void SetFunPopSureMsgBox(Fun_PopSureMsgBox pPopSureMsgBox)
{
    g_DirUIAdapter.SetFunPopSureMsgBox(pPopSureMsgBox);
}

void SetFunShowWaiting(Fun_ShowWaiting pShowWaiting)
{
    g_DirUIAdapter.SetFunShowWaiting(pShowWaiting);
}

void SetFunEndWaiting(Fun_EndWaiting pEndWaiting)
{
    g_DirUIAdapter.SetFunEndWaiting(pEndWaiting);
}

#ifdef DIR_SUPPORT_BATCH_OPERATE
void SetFunEnterSelectGroup(Fun_EnterSelectGroup pEnterSelectGroup)
{
    g_DirUIAdapter.SetFunEnterSelectGroup(pEnterSelectGroup);
}
#endif

void SetFunDialContact(Fun_DialContact pDialContact)
{
    g_DirUIAdapter.SetFunDialContact(pDialContact);
}

void SetFunDialNumber(Fun_DialNumber pDialNumber)
{
    g_DirUIAdapter.SetFunDialNumber(pDialNumber);
}

void SetFunGetLocalContactUIHelper(Fun_GetLocalContactUIHelper pGetLocalUIHelper)
{
    g_DirUIAdapter.SetFunGetLocalContactUIHelper(pGetLocalUIHelper);
}

void SetFunGetRemoteUIHelper(Fun_GetRemoteUIHelper pGetRemoteUIHelper)
{
    g_DirUIAdapter.SetFunGetRemoteUIHelper(pGetRemoteUIHelper);
}

void SetFunGetBSFTContactUIHelper(Fun_GetBSFTContactUIHelper pGetBSFTUIHelper)
{
    g_DirUIAdapter.SetFunGetBSFTContactUIHelper(pGetBSFTUIHelper);
}

#if IF_FEATURE_GBBOOK
void SetFunGetGenbandContactUIHelper(Fun_GetGenbandContactUIHelper pGetGenbandUIHelper)
{
    g_DirUIAdapter.SetFunGetGenbandContactUIHelper(pGetGenbandUIHelper);
}
#endif

#if DIR_UI_FEATURE_LOAD_DATA_LIST
void SetFunGetCommonUIHelper(Fun_GetCommonUIHelper pGetCommonUIHelper)
{
    g_DirUIAdapter.SetFunGetCommonUIHelper(pGetCommonUIHelper);
}

void SetFunGetLDAPUIHelper(Fun_GetLDAPUIHelper pGetLDAPUIHelper)
{
    g_DirUIAdapter.SetFunGetLDAPUIHelper(pGetLDAPUIHelper);
}

#if FEATURE_BROADTOUCH_UC
void SetFunGetBTUCUIHelper(Fun_GetBTUCUIHelper pGetBTUCUIHelper)
{
    g_DirUIAdapter.SetFunGetBTUCUIHelper(pGetBTUCUIHelper);
}
#endif
#endif

#if IF_BUG_PLCMDIR
void SetFunGetPLCMUIHelper(Fun_GetPLCMUIHelper pGetPLCMUIHelper)
{
    g_DirUIAdapter.SetFunGetPLCMUIHelper(pGetPLCMUIHelper);
}
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
void SetFunGetMTSWUIHelper(Fun_GetMTSWUIHelper pGetMTSWUIHelper)
{
    g_DirUIAdapter.SetFunGetMTSWUIHelper(pGetMTSWUIHelper);
}
#endif

#if IF_FEATURE_METASWITCH_CALLLOG
void SetFunGetMTSWCalllogUIHelper(Fun_GetMTSWCallogUIHelper pGetMTSWCalllogUIHelper)
{
    g_DirUIAdapter.SetFunGetMTSWCallogUIHelper(pGetMTSWCalllogUIHelper);
}
#endif

void SetFunTime2DetailString(Fun_Time2DetailString pFunTime2DetailString)
{
    g_DirUIAdapter.SetFunTime2DetailString(pFunTime2DetailString);
}
