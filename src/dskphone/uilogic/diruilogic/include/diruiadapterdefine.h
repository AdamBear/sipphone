#ifndef __DIR_UI_ADAPTER_DEFINE_H__
#define __DIR_UI_ADAPTER_DEFINE_H__

#include "directorycommon.h"
#include "idiruihelper.h"
#include "ilocalcontactuihelper.h"
#include "iremoteuihelper.h"
#include "ibsftcontactuihelper.h"
#include "igenbandcontactuihelper.h"
#if DIR_UI_SUPPORT_LOAD_DATA_LIST
#include "icommonuihelper.h"
#include "ildapuihelper.h"
#if FEATURE_BROADTOUCH_UC
#include "ibtucuihelper.h"
#endif
#endif
#if IF_BUG_PLCMDIR
#include "iplcmuihelper.h"
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
#include "imtswuihelper.h"
#endif

// 进入新界面
typedef void (*Fun_EnterUI)(int nDirType, int nUIState, int nAction, int nId);

// 退出当前界面
typedef void (*Fun_ExitCurrentUI)(int nUIState, bool bReloadData);
// 退出某一类型联系人所有界面
typedef void (*Fun_ExitAllUI)(int nDirType);
// 退回到某一类型联系人的根目录
typedef void (*Fun_ReturnRootGroup)(int nDirType);
// 检测当前状态是否正确
typedef bool (*Fun_CheckState)(int nCheckState, int nCurState);

// 显示提示框
typedef void (*Fun_ShowMsgBox)(const char * pszMsg, const char * pszExtra, int nAction);
// 弹出确认提示框
typedef bool (*Fun_PopSureMsgBox)(const char * pszTitle, const char * pszMsg, int nAction,
                                  int nId, const char * pszExtra);

// 显示等待框
typedef void (*Fun_ShowWaiting)(const char * pszTitle, const char * pszMsg, int nAction);
// 关闭等待框
typedef void (*Fun_EndWaiting)();

#ifdef DIR_SUPPORT_BATCH_OPERATE
typedef bool (*Fun_EnterSelectGroup)(int nDirType, int nExcludeGroupId, bool bRootInclude,
                                     int nAction);
#endif

// 拔号
typedef bool (*Fun_DialContact)(int nAccountId, const yl::string & strName, LIST_ATTRIBUTE & lsAttr,
                                int nDialAction);
typedef bool (*Fun_DialNumber)(int nAccountId, const yl::string & strName,
                               const yl::string & strNumber, int nDialAction);

typedef ILocalContactUIHelper * (*Fun_GetLocalContactUIHelper)(IDirUIHelperPtr & pDirUIHelper);
typedef IRemoteUIHelper * (*Fun_GetRemoteUIHelper)(IDirUIHelperPtr & pDirUIHelper);
typedef IBSFTContactUIHelper * (*Fun_GetBSFTContactUIHelper)(IDirUIHelperPtr & pDirUIHelper);
#if IF_FEATURE_GBBOOK
typedef IGenbandContactUIHelper * (*Fun_GetGenbandContactUIHelper)(IDirUIHelperPtr & pDirUIHelper);
#endif
#if DIR_UI_SUPPORT_LOAD_DATA_LIST
typedef ICommonUIHelper * (*Fun_GetCommonUIHelper)(IDirUIHelperPtr & pDirUIHelper);
typedef ILDAPUIHelper * (*Fun_GetLDAPUIHelper)(IDirUIHelperPtr & pDirUIHelper);
#if FEATURE_BROADTOUCH_UC
typedef IBTUCUIHelper * (*Fun_GetBTUCUIHelper)(IDirUIHelperPtr & pDirUIHelper);
#endif
#endif
#if IF_BUG_PLCMDIR
typedef IPLCMUIHelper * (*Fun_GetPLCMUIHelper)(IDirUIHelperPtr & pDirUIHelper);
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
typedef IMTSWUIHelper * (*Fun_GetMTSWUIHelper)(IDirUIHelperPtr & pDirUIHelper);
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
typedef IMTSWCallLogUIHelper * (*Fun_GetMTSWCallogUIHelper)(IDirUIHelperPtr & pDirUIHelper);
#endif

typedef yl::string(*Fun_Time2DetailString)(time_t tmTime, const yl::string & strSep, bool bShowComma);

#endif
