#include "diruiadapter.h"


IMPLEMENT_GETINSTANCE(CDirUIAdapter)

CDirUIAdapter::CDirUIAdapter()
    : m_pEnterUI(NULL)
    , m_pExitCurrentUI(NULL)
    , m_pExitAllUI(NULL)
    , m_pReturnRootGroup(NULL)
    , m_pCheckState(NULL)
    , m_pShowMsgBox(NULL)
    , m_pPopSureMsgBox(NULL)
    , m_pShowWaiting(NULL)
    , m_pEndWaiting(NULL)
#ifdef DIR_SUPPORT_BATCH_OPERATE
    , m_pEnterSelectGroup(NULL)
#endif
    , m_pDialContact(NULL)
    , m_pDialNumber(NULL)
    , m_pGetLocalUIHelper(NULL)
    , m_pGetRemoteUIHelper(NULL)
    , m_pGetBSFTUIHelper(NULL)
#if DIR_UI_FEATURE_LOAD_DATA_LIST
    , m_pGetCommonUIHelper(NULL)
    , m_pGetLDAPUIHelper(NULL)
#if FEATURE_BROADTOUCH_UC
    , m_pGetBTUCUIHelper(NULL)
#endif
#endif
#if IF_BUG_PLCMDIR
    , m_pGetPLCMUIHelper(NULL)
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    , m_pGetMTSWUIHelper(NULL)
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    , m_pGetMTSWUICalllogHelper(NULL)
#endif
    , m_pFunTime2DetailString(NULL)
{
}

CDirUIAdapter::~CDirUIAdapter()
{
}

void CDirUIAdapter::SetFunEnterUI(Fun_EnterUI pEnterUI)
{
    m_pEnterUI = pEnterUI;
}

void CDirUIAdapter::SetFunExitCurrentUI(Fun_ExitCurrentUI pExitCurrentUI)
{
    m_pExitCurrentUI = pExitCurrentUI;
}

void CDirUIAdapter::SetFunExitAllUI(Fun_ExitAllUI pExitAllUI)
{
    m_pExitAllUI = pExitAllUI;
}

void CDirUIAdapter::SetFunReturnRootGroup(Fun_ReturnRootGroup pReturnRootGroup)
{
    m_pReturnRootGroup = pReturnRootGroup;
}

void CDirUIAdapter::SetFunCheckState(Fun_CheckState pCheckState)
{
    m_pCheckState = pCheckState;
}

void CDirUIAdapter::SetFunShowMsgBox(Fun_ShowMsgBox pShowMsgBox)
{
    m_pShowMsgBox = pShowMsgBox;
}

void CDirUIAdapter::SetFunPopSureMsgBox(Fun_PopSureMsgBox pPopSureMsgBox)
{
    m_pPopSureMsgBox = pPopSureMsgBox;
}

void CDirUIAdapter::SetFunShowWaiting(Fun_ShowWaiting pShowWaiting)
{
    m_pShowWaiting = pShowWaiting;
}

void CDirUIAdapter::SetFunEndWaiting(Fun_EndWaiting pEndWaiting)
{
    m_pEndWaiting = pEndWaiting;
}

#ifdef DIR_SUPPORT_BATCH_OPERATE
void CDirUIAdapter::SetFunEnterSelectGroup(Fun_EnterSelectGroup pEnterSelectGroup)
{
    m_pEnterSelectGroup = pEnterSelectGroup;
}
#endif

void CDirUIAdapter::SetFunDialContact(Fun_DialContact pDialContact)
{
    m_pDialContact = pDialContact;
}

void CDirUIAdapter::SetFunDialNumber(Fun_DialNumber pDialNumber)
{
    m_pDialNumber = pDialNumber;
}

void CDirUIAdapter::SetFunGetLocalContactUIHelper(Fun_GetLocalContactUIHelper pGetLocalUIHelper)
{
    m_pGetLocalUIHelper = pGetLocalUIHelper;
}

void CDirUIAdapter::SetFunGetRemoteUIHelper(Fun_GetRemoteUIHelper pGetRemoteUIHelper)
{
    m_pGetRemoteUIHelper = pGetRemoteUIHelper;
}

void CDirUIAdapter::SetFunGetBSFTContactUIHelper(Fun_GetBSFTContactUIHelper pGetBSFTUIHelper)
{
    m_pGetBSFTUIHelper = pGetBSFTUIHelper;
}

#if IF_FEATURE_GBBOOK
void CDirUIAdapter::SetFunGetGenbandContactUIHelper(Fun_GetGenbandContactUIHelper pGetGenbandUIHelper)
{
    m_pGetGenbandUIHelper = pGetGenbandUIHelper;
}
#endif

#if DIR_UI_FEATURE_LOAD_DATA_LIST
void CDirUIAdapter::SetFunGetCommonUIHelper(Fun_GetCommonUIHelper pGetCommonUIHelper)
{
    m_pGetCommonUIHelper = pGetCommonUIHelper;
}

void CDirUIAdapter::SetFunGetLDAPUIHelper(Fun_GetLDAPUIHelper pGetLDAPUIHelper)
{
    m_pGetLDAPUIHelper = pGetLDAPUIHelper;
}

#if FEATURE_BROADTOUCH_UC
void CDirUIAdapter::SetFunGetBTUCUIHelper(Fun_GetBTUCUIHelper pGetBTUCUIHelper)
{
    m_pGetBTUCUIHelper = pGetBTUCUIHelper;
}
#endif
#endif

#if IF_BUG_PLCMDIR
void CDirUIAdapter::SetFunGetPLCMUIHelper(Fun_GetPLCMUIHelper pGetPLCMUIHelper)
{
    m_pGetPLCMUIHelper = pGetPLCMUIHelper;
}
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
void CDirUIAdapter::SetFunGetMTSWUIHelper(Fun_GetMTSWUIHelper pGetMTSWUIHelper)
{
    m_pGetMTSWUIHelper = pGetMTSWUIHelper;
}
#endif

#if IF_FEATURE_METASWITCH_CALLLOG
void CDirUIAdapter::SetFunGetMTSWCallogUIHelper(Fun_GetMTSWCallogUIHelper pGetMTSWCalllogUIHelper)
{
    m_pGetMTSWUICalllogHelper = pGetMTSWCalllogUIHelper;
}
#endif

void CDirUIAdapter::SetFunTime2DetailString(Fun_Time2DetailString pFunTime2DetailString)
{
    m_pFunTime2DetailString = pFunTime2DetailString;
}

void CDirUIAdapter::EnterUI(int nDirType, int nUIState, int nAction, int nId)
{
    if (NULL != m_pEnterUI)
    {
        (*m_pEnterUI)(nDirType, nUIState, nAction, nId);
    }
}

void CDirUIAdapter::ExitCurrentUI(int nUIState, bool bReloadData/* = true*/)
{
    if (NULL != m_pExitCurrentUI)
    {
        (*m_pExitCurrentUI)(nUIState, bReloadData);
    }
}

void CDirUIAdapter::ExitAllUI(int nDirType)
{
    if (NULL != m_pExitAllUI)
    {
        (*m_pExitAllUI)(nDirType);
    }
}

void CDirUIAdapter::ReturnRootGroup(int nDirType, bool bCheckCondition /*= false*/)
{
    if (NULL != m_pReturnRootGroup)
    {
        (*m_pReturnRootGroup)(nDirType);
    }
}

bool CDirUIAdapter::CheckState(int nCheckState, int nCurState)
{
    if (NULL != m_pCheckState)
    {
        return (*m_pCheckState)(nCurState, nCurState);
    }

    return (nCheckState == nCurState);
}

void CDirUIAdapter::ShowMsgBox(const char * pszMsg, const char * pszExtra, int nAction)
{
    if (NULL != m_pShowMsgBox)
    {
        (*m_pShowMsgBox)(pszMsg, pszExtra, nAction);
    }
}

bool CDirUIAdapter::PopSureMsgBox(const char * pszTitle, const char * pszMsg, int nAction, int nId, const char * pszExtra)
{
    if (NULL != m_pPopSureMsgBox)
    {
        return (*m_pPopSureMsgBox)(pszTitle, pszMsg, nAction, nId, pszExtra);
    }

    return false;
}

void CDirUIAdapter::ShowWaiting(const char * pszTitle, const char * pszMsg, int nAction)
{
    if (NULL != m_pShowWaiting)
    {
        (*m_pShowWaiting)(pszTitle, pszMsg, nAction);
    }
}

void CDirUIAdapter::EndWaiting()
{
    if (NULL != m_pEndWaiting)
    {
        (*m_pEndWaiting)();
    }
}

#ifdef DIR_SUPPORT_BATCH_OPERATE
bool CDirUIAdapter::EnterSelectGroup(int nDirType, int nExcludeGroupId, bool bRootInclude,
                                     int nAction)
{
    if (NULL != m_pEnterSelectGroup)
    {
        return (*m_pEnterSelectGroup)(nDirType, nExcludeGroupId, bRootInclude, nAction);
    }

    return false;
}
#endif

bool CDirUIAdapter::Dial(int nAccountId, const yl::string & strName, LIST_ATTRIBUTE & lsAttr,
                         int nDialAction)
{
    if (NULL != m_pDialContact)
    {
        return (*m_pDialContact)(nAccountId, strName, lsAttr, nDialAction);
    }

    return false;
}

bool CDirUIAdapter::Dial(int nAccountId, const yl::string & strName, const yl::string & strNumber,
                         int nDialAction)
{
    if (NULL != m_pDialNumber)
    {
        return (*m_pDialNumber)(nAccountId, strName, strNumber, nDialAction);
    }

    return false;
}

ILocalContactUIHelper * CDirUIAdapter::GetLocalContactUIHelper(IDirUIHelperPtr pDirUIHelper)
{
    if (NULL != m_pGetLocalUIHelper)
    {
        return (*m_pGetLocalUIHelper)(pDirUIHelper);
    }

    return NULL;
}

IRemoteUIHelper * CDirUIAdapter::GetRemoteUIHelper(IDirUIHelperPtr pDirUIHelper)
{
    if (NULL != m_pGetRemoteUIHelper)
    {
        return (*m_pGetRemoteUIHelper)(pDirUIHelper);
    }

    return NULL;
}

IBSFTContactUIHelper * CDirUIAdapter::GetBSFTContactUIHelper(IDirUIHelperPtr pDirUIHelper)
{
    if (NULL != m_pGetBSFTUIHelper)
    {
        return (*m_pGetBSFTUIHelper)(pDirUIHelper);
    }

    return NULL;
}

#if IF_FEATURE_GBBOOK
IGenbandContactUIHelper* CDirUIAdapter::GetGenbandContactUIhelper(IDirUIHelperPtr pDirUIHelper)
{
    if (NULL != m_pGetGenbandUIHelper)
    {
        return (*m_pGetGenbandUIHelper)(pDirUIHelper);
    }

    return NULL;
}
#endif

#if DIR_UI_FEATURE_LOAD_DATA_LIST
ICommonUIHelper * CDirUIAdapter::GetCommonUIHelper(IDirUIHelperPtr pDirUIHelper)
{
    if (NULL != m_pGetCommonUIHelper)
    {
        return (*m_pGetCommonUIHelper)(pDirUIHelper);
    }

    return NULL;
}

ILDAPUIHelper * CDirUIAdapter::GetLDAPUIHelper(IDirUIHelperPtr pDirUIHelper)
{
    if (NULL != m_pGetLDAPUIHelper)
    {
        return (*m_pGetLDAPUIHelper)(pDirUIHelper);
    }

    return NULL;
}

#if FEATURE_BROADTOUCH_UC
IBTUCUIHelper * CDirUIAdapter::GetBTUCUIHelper(IDirUIHelperPtr pDirUIHelper)
{
    if (NULL != m_pGetBTUCUIHelper)
    {
        return (*m_pGetBTUCUIHelper)(pDirUIHelper);
    }

    return NULL;
}
#endif
#endif

#if IF_BUG_PLCMDIR
IPLCMUIHelper * CDirUIAdapter::GetPLCMUIHelper(IDirUIHelperPtr pDirUIHelper)
{
    if (NULL != m_pGetPLCMUIHelper)
    {
        return (*m_pGetPLCMUIHelper)(pDirUIHelper);
    }

    return NULL;
}
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
IMTSWUIHelper * CDirUIAdapter::GetMTSWUIHelper(IDirUIHelperPtr pDirUIHelper)
{
    if (NULL != m_pGetMTSWUIHelper)
    {
        return (*m_pGetMTSWUIHelper)(pDirUIHelper);
    }

    return NULL;
}
#endif

#if IF_FEATURE_METASWITCH_CALLLOG
IMTSWCallLogUIHelper * CDirUIAdapter::GetMTSWCallogUIHelper(IDirUIHelperPtr pDirUIHelper)
{
    if (NULL != m_pGetMTSWUICalllogHelper)
    {
        return (*m_pGetMTSWUICalllogHelper)(pDirUIHelper);
    }

    return NULL;
}
#endif

yl::string CDirUIAdapter::Time2DetailString(time_t tmTime, bool bShowCommoa/* = true*/)
{
    if (NULL != m_pFunTime2DetailString)
    {
        return (*m_pFunTime2DetailString)(tmTime, yl::string(" "), bShowCommoa);
    }

    return yl::string("");
}
