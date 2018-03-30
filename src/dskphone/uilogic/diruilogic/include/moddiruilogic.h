#ifndef __MOD_DIR_UI_LOGIC_H__
#define __MOD_DIR_UI_LOGIC_H__

#include "directorycommon.h"
#include "diruiadapterdefine.h"


void SetFunEnterUI(Fun_EnterUI pEnterUI);
void SetFunExitCurrentUI(Fun_ExitCurrentUI pExitCurrentUI);
void SetFunExitAllUI(Fun_ExitAllUI pExitAllUI);
void SetFunReturnRootGroup(Fun_ReturnRootGroup pReturnRootGroup);
void SetFunCheckState(Fun_CheckState pCheckState);
void SetFunShowMsgBox(Fun_ShowMsgBox pShowMsgBox);
void SetFunPopSureMsgBox(Fun_PopSureMsgBox pPopSureMsgBox);
void SetFunShowWaiting(Fun_ShowWaiting pShowWaiting);
void SetFunEndWaiting(Fun_EndWaiting pEndWaiting);

#ifdef DIR_SUPPORT_BATCH_OPERATE
void SetFunEnterSelectGroup(Fun_EnterSelectGroup pEnterSelectGroup);
#endif

void SetFunDialContact(Fun_DialContact pDialContact);
void SetFunDialNumber(Fun_DialNumber pDialNumber);

void SetFunGetLocalContactUIHelper(Fun_GetLocalContactUIHelper pGetLocalUIHelper);
void SetFunGetRemoteUIHelper(Fun_GetRemoteUIHelper pGetRemoteUIHelper);
void SetFunGetBSFTContactUIHelper(Fun_GetBSFTContactUIHelper pGetBSFTUIHelper);
#if IF_FEATURE_GBBOOK
void SetFunGetGenbandContactUIHelper(Fun_GetGenbandContactUIHelper pGetGenbandUIHelper);
#endif
#if DIR_UI_SUPPORT_LOAD_DATA_LIST
void SetFunGetCommonUIHelper(Fun_GetCommonUIHelper pGetCommonUIHelper);
void SetFunGetLDAPUIHelper(Fun_GetLDAPUIHelper pGetLDAPUIHelper);
#if FEATURE_BROADTOUCH_UC
void SetFunGetBTUCUIHelper(Fun_GetBTUCUIHelper pGetBTUCUIHelper);
#endif
#endif
#if IF_BUG_PLCMDIR
void SetFunGetPLCMUIHelper(Fun_GetPLCMUIHelper pGetPLCMUIHelper);
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
void SetFunGetMTSWUIHelper(Fun_GetMTSWUIHelper pGetMTSWUIHelper);
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
void SetFunGetMTSWCalllogUIHelper(Fun_GetMTSWCallogUIHelper pGetMTSWCalllogUIHelper);
#endif


void SetFunTime2DetailString(Fun_Time2DetailString pFunTime2DetailString);

#endif
