#ifndef __DIR_UI_ADAPTER_H__
#define __DIR_UI_ADAPTER_H__

#include "singletonclassdefine.h"
#include "diruiadapterdefine.h"


class CDirUIAdapter
{
    DECLARE_SINGLETON_CLASS(CDirUIAdapter)

public:
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
    void SetFunGetMTSWCallogUIHelper(Fun_GetMTSWCallogUIHelper pGetMTSWCalllogUIHelper);
#endif

    void SetFunTime2DetailString(Fun_Time2DetailString pFunTime2DetailString);

public:
    // 进入新界面
    void EnterUI(int nDirType, int nUIState, int nAction, int nId);

    // 退出当前界面
    void ExitCurrentUI(int nUIState, bool bReloadData = true);
    // 退出某一类型联系人所有界面
    void ExitAllUI(int nDirType);
    // 退回到某一类型联系人的根目录
    void ReturnRootGroup(int nDirType, bool bCheckCondition = false);
    // 检测当前状态是否正确
    bool CheckState(int nCheckState, int nCurState);

    // 显示提示框
    void ShowMsgBox(const char * pszMsg, const char * pszExtra = NULL, int nAction = DIR_ACTION_NONE);
    // 弹出确认提示框
    bool PopSureMsgBox(const char * pszTitle, const char * pszMsg, int nAction, int nId, const char * pszExtra = NULL);

    // 显示等待框
    void ShowWaiting(const char * pszTitle, const char * pszMsg, int nAction);
    // 关闭等待框
    void EndWaiting();

#ifdef DIR_SUPPORT_BATCH_OPERATE
    bool EnterSelectGroup(int nDirType, int nExcludeGroupId, bool bRootInclude, int nAction);
#endif

    // 拔号
    bool Dial(int nAccountId, const yl::string & strName, LIST_ATTRIBUTE & lsAttr, int nDialAction);
    bool Dial(int nAccountId, const yl::string & strName, const yl::string & strNumber,
              int nDialAction);

    ILocalContactUIHelper * GetLocalContactUIHelper(IDirUIHelperPtr pDirUIHelper);
    IRemoteUIHelper * GetRemoteUIHelper(IDirUIHelperPtr pDirUIHelper);
    IBSFTContactUIHelper * GetBSFTContactUIHelper(IDirUIHelperPtr pDirUIHelper);
#if IF_FEATURE_GBBOOK
    IGenbandContactUIHelper* GetGenbandContactUIhelper(IDirUIHelperPtr pDirUIHelper);
#endif
#if DIR_UI_SUPPORT_LOAD_DATA_LIST
    ICommonUIHelper * GetCommonUIHelper(IDirUIHelperPtr pDirUIHelper);
    ILDAPUIHelper * GetLDAPUIHelper(IDirUIHelperPtr pDirUIHelper);
#if FEATURE_BROADTOUCH_UC
    IBTUCUIHelper * GetBTUCUIHelper(IDirUIHelperPtr pDirUIHelper);
#endif
#endif
#if IF_BUG_PLCMDIR
    IPLCMUIHelper * GetPLCMUIHelper(IDirUIHelperPtr pDirUIHelper);
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    IMTSWUIHelper * GetMTSWUIHelper(IDirUIHelperPtr pDirUIHelper);
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    IMTSWCallLogUIHelper * GetMTSWCallogUIHelper(IDirUIHelperPtr pDirUIHelper);
#endif
    yl::string Time2DetailString(time_t tmTime, bool bShowCommoa = true);

protected:
    Fun_EnterUI m_pEnterUI;
    Fun_ExitCurrentUI m_pExitCurrentUI;
    Fun_ExitAllUI m_pExitAllUI;
    Fun_ReturnRootGroup m_pReturnRootGroup;
    Fun_CheckState m_pCheckState;

    Fun_ShowMsgBox m_pShowMsgBox;
    Fun_PopSureMsgBox m_pPopSureMsgBox;

    Fun_ShowWaiting m_pShowWaiting;
    Fun_EndWaiting m_pEndWaiting;

#ifdef DIR_SUPPORT_BATCH_OPERATE
    Fun_EnterSelectGroup m_pEnterSelectGroup;
#endif

    Fun_DialContact m_pDialContact;
    Fun_DialNumber m_pDialNumber;

    Fun_GetLocalContactUIHelper m_pGetLocalUIHelper;
    Fun_GetRemoteUIHelper m_pGetRemoteUIHelper;
    Fun_GetBSFTContactUIHelper m_pGetBSFTUIHelper;
#if IF_FEATURE_GBBOOK
    Fun_GetGenbandContactUIHelper m_pGetGenbandUIHelper;
#endif
#if DIR_UI_SUPPORT_LOAD_DATA_LIST
    Fun_GetCommonUIHelper m_pGetCommonUIHelper;
    Fun_GetLDAPUIHelper m_pGetLDAPUIHelper;
#if FEATURE_BROADTOUCH_UC
    Fun_GetBTUCUIHelper m_pGetBTUCUIHelper;
#endif
#endif
#if IF_BUG_PLCMDIR
    Fun_GetPLCMUIHelper m_pGetPLCMUIHelper;
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    Fun_GetMTSWUIHelper m_pGetMTSWUIHelper;
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    Fun_GetMTSWCallogUIHelper m_pGetMTSWUICalllogHelper;
#endif

    Fun_Time2DetailString m_pFunTime2DetailString;
};

#define g_DirUIAdapter GET_SINGLETON_INSTANCE(CDirUIAdapter)

#endif
