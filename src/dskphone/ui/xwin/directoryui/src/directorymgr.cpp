#include "directorymgr.h"

#include "xwindow/xListView.h"

#include "adapterfactory.h"
#include "directorycommon.h"
#include "dlgdirectory.h"
#include "dlgdirsearch.h"
#include "dlgcustomphoto.h"
#include "browseadapter.h"
#include "bsftpwdadapter.h"
#include "detailadapter.h"
#include "numberadapter.h"
#include "precisesearchadapter.h"
#include "searchadapter.h"
#if IF_FEATURE_34162
#include "ldappwdadapter.h"
#endif
#include "accountadapter.h"
#include "bsftcalllogcontroler.h"
#include "bsftcontactcontroler.h"
#include "btuccontactcontroler.h"
#include "ldapcontactcontroler.h"
#include "remotecontactcontroler.h"
#include "remoteserversearchcontroler.h"
#include "dirctrlfactory.h"
#include "diruihelper.h"
#include "localcontactuihelper.h"
#include "remoteuihelper.h"
#include "bsftcontactuihelper.h"
#if IF_BUG_PLCMDIR
#include "plcmuihelper.h"
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
#include "mtswcontactuihelper.h"
#endif
#if IF_FEATURE_GBBOOK
#include "genbandcontactuihelper.h"
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
#include "mtswcallloguihelper.h"
#endif
#include "moddiruilogic.h"
#include "moddirectoryui.h"
#include "presence/include/presence_inc.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "settinguilogic/include/modsetuilogic.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "uimanager/moduimanager.h"

#ifdef PHOTO_SHOW
#include "dlgcustomphoto.h"
#endif


IMPLEMENT_GETINSTANCE(CDirectoryMgr)

CDirectoryMgr::CDirectoryMgr()
#if DIR_SUPPORT_SELECT_CONTACT
    : m_pFunSelectContact(NULL)
#endif
{
    etl_RegisterMsgHandle(DIRECTORY_MESSAGE_BEGIN, DIRECTORY_MESSAGE_END, &CDirectoryMgr::OnMessage);
    etl_RegisterMsgHandle(CALLLOG_MESSAGE_BEGIN, CALLLOG_MESSAGE_END, &CDirectoryMgr::OnMessage);
#if FEATURE_BROADTOUCH_UC
    etl_RegisterMsgHandle(PRESENCE_MESSAGE_FAVORITE_CHANGE, PRESENCE_MESSAGE_FAVORITE_CHANGE,
                          &CDirectoryMgr::OnMessage);
#endif
}

CDirectoryMgr::~CDirectoryMgr()
{
    etl_UnregisterMsgHandle(DIRECTORY_MESSAGE_BEGIN, DIRECTORY_MESSAGE_END, &CDirectoryMgr::OnMessage);
    etl_UnregisterMsgHandle(CALLLOG_MESSAGE_BEGIN, CALLLOG_MESSAGE_END, &CDirectoryMgr::OnMessage);
#if FEATURE_BROADTOUCH_UC
    etl_UnregisterMsgHandle(PRESENCE_MESSAGE_FAVORITE_CHANGE, PRESENCE_MESSAGE_FAVORITE_CHANGE,
                            &CDirectoryMgr::OnMessage);
#endif

    UnInit();
}

void CDirectoryMgr::Init()
{
#ifdef PHOTO_SHOW
    REGISTER_DLG(CDlgCustomPhoto);
#endif
    REGISTER_DLG(CDlgDirectory);
    REGISTER_DLG(CDlgDirSearch);

    SetFunEnterUI(&CDirUIHelper::EnterUI);
    SetFunExitCurrentUI(&CDirUIHelper::ExitCurrentUI);
    SetFunExitAllUI(&CDirUIHelper::ExitAllUI);
    SetFunReturnRootGroup(&CDirUIHelper::ReturnRootGroup);
    SetFunCheckState(&CDirUIHelper::CheckState);
    SetFunDialContact(&CDirUIHelper::DialContact);
    SetFunDialNumber(&CDirUIHelper::DialNumber);
    SetFunGetLocalContactUIHelper(&CLocalContactUIHelper::GetLocalContactUIHelper);
    SetFunGetRemoteUIHelper(&CRemoteUIHelper::GetRemoteUIHelper);
    SetFunGetBSFTContactUIHelper(&CBSFTContactUIHelper::GetBSFTUIHelper);
#if IF_BUG_PLCMDIR
    SetFunGetPLCMUIHelper(&CPlcmUIHelper::GetPLCMUIHelper);
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    SetFunGetMTSWUIHelper(&CMTSWContactUIHelper::GetMTSWUIHelper);
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    SetFunGetMTSWCalllogUIHelper(&CMTSWCallLogUIHelper::GetMTSWCalllogUIHelper);
#endif
#if IF_FEATURE_GBBOOK
    SetFunGetGenbandContactUIHelper(&CGenbandContactUIHelper::GetGenbandUIHelper);
#endif
    SetFunTime2DetailString(&Timet2DetailString);

    m_pActiveAdapter.Reset();
}

void CDirectoryMgr::UnInit()
{
    DIRUI_INFO("CDirectoryMgr::UnInit()");

    Reset();

    SetFunEnterUI(NULL);
    SetFunExitCurrentUI(NULL);
    SetFunExitAllUI(NULL);
    SetFunReturnRootGroup(NULL);
    SetFunCheckState(NULL);
    SetFunDialContact(NULL);
    SetFunDialNumber(NULL);
    SetFunGetLocalContactUIHelper(NULL);
    SetFunGetRemoteUIHelper(NULL);
    SetFunGetBSFTContactUIHelper(NULL);
#if IF_BUG_PLCMDIR
    SetFunGetPLCMUIHelper(NULL);
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    SetFunGetMTSWUIHelper(NULL);
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    SetFunGetMTSWCalllogUIHelper(NULL);
#endif
#if IF_FEATURE_GBBOOK
    SetFunGetGenbandContactUIHelper(NULL);
#endif

#if DIR_SUPPORT_SELECT_CONTACT
    CancelSelectDirectory();
#endif
}

void CDirectoryMgr::Reset()
{
    if (!m_pActiveAdapter.IsEmpty())
    {
        m_pActiveAdapter->UnInit();
        m_pActiveAdapter.Reset();
    }

    LIST_DIR_ADAPTER::iterator iter = m_listAdapter.begin();

    for (; iter != m_listAdapter.end(); ++iter)
    {
        CDirectoryAdapterPtr & pAdapter = *iter;

        if (!pAdapter.IsEmpty())
        {
            pAdapter->UnInit();
            pAdapter.Reset();
        }
    }

    m_listAdapter.clear();
}

LRESULT CDirectoryMgr::OnMessage(msgObject & refMessage)
{
    DIRUI_INFO("CDirectoryMgr::OnMessage(%u)", refMessage.message);

    CDirectoryAdapterPtr pListAdapter = g_DirectoryMgr.GetActiveAdapter();

    // 搜索消息只发送给当前的视图处理
    if (LOCAL_SEARCH_RESULT == refMessage.message)
    {
        CSearchAdapter * pSearchAdapter = CSearchAdapter::GetSearchAdapter(pListAdapter);

        if (NULL == pSearchAdapter)
        {
            return TRUE;
        }

        CDirectoryControlerPtr pControler = pSearchAdapter->GetControler();

        if (pControler.IsEmpty())
        {
            return TRUE;
        }

        pControler->OnMessage(refMessage);
        return TRUE;
    }

    LRESULT retValue = FALSE;
    int nDirType = DIRECTORY_TYPE_NONE;

    if (refMessage.message >= LOCAL_DIRECTORY_MESSAGE_BEGIN
            && refMessage.message <= LOCAL_DIRECTORY_MESSAGE_END)
    {
        nDirType = DIRECTORY_TYPE_LOCAL;
    }
    else if (refMessage.message >= REMOTE_DIRECTORY_MESSAGE_BEGIN
             && refMessage.message <= REMOTE_DIRECTORY_MESSAGE_END)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        CRemoteServerSearchControler * pServerSearchControler =
            CRemoteServerSearchControler::GetServerSearchControler(pControler);

        if (NULL != pServerSearchControler)
        {
            nDirType = DIRECTORY_TYPE_REMOTE_SEARCH;
        }
        else
        {
            nDirType = DIRECTORY_TYPE_REMOTE;
        }
    }
    else if (refMessage.message >= BROADSOFT_DIRECTORY_MESSAGE_BEGIN
             && refMessage.message <= BROADSOFT_PHONEBOOK_MESSAGE_END)
    {
        nDirType = DIRECTORY_TYPE_BROADSOFT;
    }
    else if (refMessage.message >= LDAP_MESSAGE_BEGIN && refMessage.message <= LDAP_MESSAGE_END)
    {
        nDirType = DIRECTORY_TYPE_LDAP;
    }
#if FEATURE_BROADTOUCH_UC
    else if (PRESENCE_MESSAGE_FAVORITE_CHANGE == refMessage.message
             || (refMessage.message >= BROADSOFT_BUDDYLIST_MESSAGE_BEGIN
                 && refMessage.message <= BROADSOFT_DIRECTORY_MESSAGE_END))
    {
        nDirType = DIRECTORY_TYPE_BTUC;
    }
#endif
    else if (refMessage.message >= LOCAL_CALLLOG_MESSAGE_BEGIN
             && refMessage.message <= LOCAL_CALLLOG_MESSAGE_END)
    {
        nDirType = CALL_LOG_TYPE_LOCAL;
    }
    else if (refMessage.message >= BROADSOFT_CALLLOG_MESSAGE_BEGIN
             && refMessage.message <= BROADSOFT_CALLLOG_MESSAGE_END)
    {
        nDirType = CALL_LOG_TYPE_BROADSOFT;
    }
    else if (SUPER_SEARCH_RESULT == refMessage.message)
    {
        nDirType = DIRECTORY_TYPE_BROADSOFT;
    }
#if IF_BUG_PLCMDIR
    else if (PLCM_PHONEBOOK_ALL_DOWNLOADED_FINISHED == refMessage.message)
    {
        nDirType = DIRECTORY_TYPE_PLCM;
    }
#endif
#ifdef IF_SUPPORT_BLUETOOTH_CONTACT
    else if (refMessage.message >= BLUETOOTH_CONTACT_BEGIN
             && refMessage.message <= BLUETOOTH_CONTACT_END)
    {
        nDirType = DIRECTORY_TYPE_BLUETOOTH;
    }
#endif // IF_SUPPORT_BLUETOOTH_CONTACT
#if IF_FEATURE_GBBOOK
    else if (GENBAND_BOOK_REQ_RESULT == refMessage.message || GENBAND_BOOK_RELOAD == refMessage.message)
    {
        nDirType = DIRECTORY_TYPE_GENBAND;
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (refMessage.message >= METASWITCH_DIRECTORY_MESSAGE_BEGIN
             && refMessage.message <= METASWITCH_PHONEBOOK_MESSAGE_END)
    {
        nDirType = DIRECTORY_TYPE_METASWICTH;
    }
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    else if (refMessage.message >= METASWITCH_CALLLOG_MESSAGE_BEGIN
             && refMessage.message <= METASWITCH_CALLLOG_MESSAGE_END)
    {
        nDirType = CALL_LOG_TYPE_MTSW;
    }
#endif

    CDirectoryControlerPtr pControler = g_DirCtrlFactory.FindControler(nDirType);

    if (!pControler.IsEmpty())
    {
        retValue = pControler->OnMessage(refMessage);
    }
    else if (DIRECTORY_TYPE_NONE != nDirType)
    {
        retValue = TRUE;
    }

    return retValue;
}

CDirectoryAdapterPtr CDirectoryMgr::GetActiveAdapter()
{
    return m_pActiveAdapter;
}

CDirectoryControlerPtr CDirectoryMgr::GetActiveControler()
{
    if (!m_pActiveAdapter.IsEmpty())
    {
        return m_pActiveAdapter->GetControler();
    }

    return CDirectoryControlerPtr(NULL);
}

IDirUIHelperPtr CDirectoryMgr::GetActiveUIHelper()
{
    if (!m_pActiveAdapter.IsEmpty())
    {
        return m_pActiveAdapter->GetUIHelper();
    }

    return IDirUIHelperPtr(NULL);
}

int CDirectoryMgr::GetActiveDirType()
{
    CDirectoryControlerPtr pControler = GetActiveControler();

    if (!pControler.IsEmpty())
    {
        return pControler->GetType();
    }

    return DIRECTORY_TYPE_NONE;
}

bool CDirectoryMgr::EnterState(int nState, int nDirType, int nGroupId)
{
    CDirectoryAdapterPtr pActiveAdapter = GetActiveAdapter();

    // 已经是该类型联系人的UI状态，则不重复进入相应状态
    if (!pActiveAdapter.IsEmpty() && pActiveAdapter->GetState() == nState
            && pActiveAdapter->GetDirType() == nDirType)
    {
        return false;
    }

    CDirectoryAdapterPtr pAdapter = g_AdapterFactory.GetContactAdapter(nState);

    if (pAdapter.IsEmpty())
    {
        return false;
    }

    m_listAdapter.push_back(pAdapter);

    if (!m_pActiveAdapter.IsEmpty())
    {
        m_pActiveAdapter->SetCache(true);
        m_pActiveAdapter->SetListView(NULL);
    }

    m_pActiveAdapter = pAdapter;

    pAdapter->Init(nDirType, nGroupId);

    return true;
}

bool CDirectoryMgr::LeaveCurrentState(int nRefreshType/* = DIR_UI_REFRESH_TYPE_ALL*/)
{
    if (m_pActiveAdapter.IsEmpty())
    {
        return false;
    }

    int nUIState = m_pActiveAdapter->GetState();
    CDlgDirectoryBase * pDlg = GetDirDialog(nUIState);

    if (NULL == pDlg)
    {
        return false;
    }

    m_listAdapter.pop_back();

    LIST_DIR_ADAPTER::iterator iter = m_listAdapter.begin();
    bool bRemainDlg = false;

    for (; iter != m_listAdapter.end(); ++iter)
    {
        CDirectoryAdapterPtr pAdapter = *iter;

        if (!pAdapter.IsEmpty() && pDlg == GetDirDialog(pAdapter->GetState()))
        {
            bRemainDlg = true;
            break;
        }
    }

    m_pActiveAdapter->UnInit();
    m_pActiveAdapter.Reset();

    if (!bRemainDlg)
    {
        UIManager_PutPrivateDlg(pDlg);
    }

    if (m_listAdapter.size() <= 0)
    {
        return true;
    }

    m_pActiveAdapter = m_listAdapter.back();

    if (m_pActiveAdapter.IsEmpty())
    {
        return false;
    }

    pDlg = GetDirDialog(m_pActiveAdapter->GetState());

    if (NULL == pDlg)
    {
        return false;
    }

    xListView * pListView = pDlg->GetListView();

    if (NULL == pListView)
    {
        return false;
    }

    m_pActiveAdapter->OnReactive();

    pListView->SetAdapter(m_pActiveAdapter.Get());

    if (pDlg != UIManager_GetTopWindow())
    {
        UIManager_SetTopWindow(pDlg);
    }

    if (DIR_UI_REFRESH_TYPE_NONE != nRefreshType)
    {
        m_pActiveAdapter->RefreshUI(nRefreshType);
    }

    return true;
}

void CDirectoryMgr::OnReturnIdle(CDlgDirectoryBase * pDlg)
{
    if (0 == m_listAdapter.size())
    {
        return;
    }

    DM_ExitDirectory();
}

CDlgDirectoryBase * CDirectoryMgr::CreateDirDialog(int nState)
{
    CDlgDirectoryBase * pDlg = NULL;

    if (DIR_UI_STATE_SEARCH == nState)
    {
        pDlg = static_cast<CDlgDirectoryBase *>(UIManager_GetPrivateDlg(DLG_CDlgDirSearch));

        if (NULL == pDlg)
        {
            pDlg = static_cast<CDlgDirectoryBase *>(UIManager_CreatePrivateDlg(DLG_CDlgDirSearch));
        }
    }
    else
    {
        pDlg = static_cast<CDlgDirectoryBase *>(UIManager_GetPrivateDlg(DLG_CDlgDirectory));

        if (NULL == pDlg)
        {
            pDlg = static_cast<CDlgDirectoryBase *>(UIManager_CreatePrivateDlg(DLG_CDlgDirectory));
        }
    }

    return pDlg;
}

CDlgDirectoryBase * CDirectoryMgr::GetDirDialog(int nState)
{
    if (DIR_UI_STATE_SEARCH == nState)
    {
        return static_cast<CDlgDirectoryBase *>(UIManager_GetPrivateDlg(DLG_CDlgDirSearch));
    }

    return static_cast<CDlgDirectoryBase *>(UIManager_GetPrivateDlg(DLG_CDlgDirectory));
}

bool CDirectoryMgr::EnterDirectory(int nDirType, int nGroupId)
{
    if (!CDirectoryControler::IsUserLevelShow(nDirType, nGroupId))
    {
        return false;
    }

    DIRUI_INFO("EnterDirectory(DirType:%d GroupId:%d)", nDirType, nGroupId);

    if (DIRECTORY_TYPE_LDAP == nDirType)
    {
        if (!LDAP_IsEnable())
        {
            return false;
        }

        EnterSearch(nDirType, knInvalidRId, PHONE_KEY_NONE);
        return true;
    }

    if (DIRECTORY_TYPE_REMOTE == nDirType && CRemoteContactControler::IsServerSearchGroup(nGroupId))
    {
        EnterSearch(DIRECTORY_TYPE_REMOTE_SEARCH, nGroupId, PHONE_KEY_NONE);

        return true;
    }

#if IF_FEATURE_MUTI_CALLLOG
    if (CALL_LOG_TYPE_BROADSOFT == nDirType
            && CBSFTCallLogControler::IsMutiAccountChooseGroup(nGroupId)
            && NULL == UIManager_GetPrivateDlg(DLG_CDlgDirectory))
    {
        EnterAccountChoose(nDirType, nGroupId, false);
        return true;
    }
#endif

    if (DIRECTORY_TYPE_BTUC == nDirType)
    {
#if FEATURE_BROADTOUCH_UC
        if (!CBTUCContactControler::IsConfigure())
        {
            return false;
        }

        // 帐号未注册成功前，进入Menu界面
        if (knInvalidRId != nGroupId && !CBTUCContactControler::IsGroupIdValid(nGroupId))
        {
            nGroupId = knInvalidRId;
        }
#else
        return false;
#endif
    }
    else if (!CCommonDirCtrl::IsDirEnable(nDirType))
    {
        DIRUI_INFO("IsDirEnable(%d) fail when EnterDirectory", nDirType);
        return false;
    }

    return EnterBrowser(nDirType, nGroupId);
}

bool CDirectoryMgr::EnterBrowser(int nDirType, int nGroupId)
{
    if (!EnterState(DIR_UI_STATE_BROWSER, nDirType, nGroupId))
    {
        return false;
    }

    CBrowseAdapter * pBrowseAdapter = CBrowseAdapter::GetBrowserAdapter(m_pActiveAdapter);

    if (NULL == pBrowseAdapter)
    {
        return false;
    }

    CDlgDirectoryBase * pDlgDirectory = CreateDirDialog(DIR_UI_STATE_BROWSER);

// 创建显示界面
    if (NULL == pDlgDirectory)
    {
        return false;
    }

    xListView * pListView = pDlgDirectory->GetListView();

    if (NULL == pListView)
    {
        UIManager_PutPrivateDlg(pDlgDirectory);
        return false;
    }

    pListView->SetAdapter(pBrowseAdapter);

    pBrowseAdapter->ClearHistoryPage();

    UIManager_SetTopWindow(pDlgDirectory);

    int nRealGroupId = nGroupId;
    CDirectoryControlerPtr pControl = GetActiveControler();
    if (!pControl.IsEmpty())
    {
        nRealGroupId = pControl->GetGroupId();
    }

    pBrowseAdapter->OnChangeGroup(nRealGroupId, true);

    return true;
}

bool CDirectoryMgr::EnterGroup(int nDirType, int nGroupId)
{
    CDirectoryAdapterPtr pActiveAdapter = GetActiveAdapter();
    CBrowseAdapter * pBrowserAdapter = CBrowseAdapter::GetBrowserAdapter(pActiveAdapter);

    if (NULL == pBrowserAdapter || nDirType != pBrowserAdapter->GetDirType())
    {
        return false;
    }

    pBrowserAdapter->EnterGroup(nDirType, nGroupId);
    return true;
}

bool CDirectoryMgr::EnterSearch(int nDirType, int nGroupId, int nKeyCode)
{
    if (!EnterState(DIR_UI_STATE_SEARCH, nDirType, nGroupId))
    {
        return false;
    }

    CSearchAdapter * pSearchAdapter = CSearchAdapter::GetSearchAdapter(m_pActiveAdapter);

    if (NULL == pSearchAdapter)
    {
        return false;
    }

    CDlgDirSearch * pDlgSearch = static_cast<CDlgDirSearch *>(CreateDirDialog(DIR_UI_STATE_SEARCH));

    if (NULL == pDlgSearch)
    {
        return false;
    }

    xListView * pListView = pDlgSearch->GetListView();

    if (NULL == pListView)
    {
        UIManager_PutPrivateDlg(pDlgSearch);

        return false;
    }

    pListView->SetAdapter(pSearchAdapter);

    UIManager_SetTopWindow(pDlgSearch);

    pSearchAdapter->RefreshUI();

    pDlgSearch->SetInitSearchKey(nKeyCode);

    return true;
}

// 进入高级搜索
void CDirectoryMgr::EnterPreciseSearch()
{
    CDirectoryControlerPtr pControler = GetActiveControler();
    if (pControler.IsEmpty())
    {
        return;
    }

    int nDirType = pControler->GetType();

    CDirectoryAdapterPtr pAdapter = GetActiveAdapter();

    if (pAdapter.IsEmpty() || nDirType != pAdapter->GetDirType())
    {
        return;
    }

    if (!EnterState(DIR_UI_STATE_PRECISE_SEARCH, nDirType, pAdapter->GetGroupId()))
    {
        return;
    }

    CPreciseSearchAdapter * pSearchAdapter = CPreciseSearchAdapter::GetPreciseSearchAdapter(
                m_pActiveAdapter);

    if (NULL == pSearchAdapter)
    {
        return;
    }

    CDlgDirectoryBase * pDlgDirectory = GetDirDialog(DIR_UI_STATE_PRECISE_SEARCH);

    if (NULL == pDlgDirectory)
    {
        return;
    }

    xListView * pListView = pDlgDirectory->GetListView();

    if (NULL == pListView)
    {
        UIManager_PutPrivateDlg(pDlgDirectory);
        return;
    }

    pListView->SetAdapter(pSearchAdapter);

    UIManager_SetTopWindow(pDlgDirectory);

    pSearchAdapter->EnterPreciseSearch();
}

void CDirectoryMgr::EnterDetail(int nAction, int nDirType, int nGroupId, int nContactId)
{
    // 通话中历史记录按OK键可以进入详情
    if (!CDirectoryControler::IsDirCanEdit(nDirType, nGroupId) && CALL_LOG_TYPE_LOCAL != nDirType
            && CALL_LOG_TYPE_BROADSOFT != nDirType
#if IF_FEATURE_METASWITCH_CALLLOG
            && CALL_LOG_TYPE_MTSW != nDirType
#endif
       )
    {
        return ;
    }

    if (!EnterState(DIR_UI_STATE_DETAIL, nDirType, nGroupId))
    {
        return;
    }

    CDirectoryAdapterPtr pAdapter = GetActiveAdapter();

    CDetailAdapter * pDetailAdapter = CDetailAdapter::GetDetailAdapter(pAdapter);

    if (NULL == pDetailAdapter)
    {
        return;
    }

    CDlgDirectoryBase * pDlgDirectory = CreateDirDialog(DIR_UI_STATE_DETAIL);

    // 创建显示界面
    if (NULL == pDlgDirectory)
    {
        return;
    }

    xListView * pListView = pDlgDirectory->GetListView();

    if (NULL == pListView)
    {
        UIManager_PutPrivateDlg(pDlgDirectory);
        return;
    }

    pListView->SetAdapter(pDetailAdapter);

    UIManager_SetTopWindow(pDlgDirectory);

    pDetailAdapter->EnterDetail(nAction);
}

void CDirectoryMgr::EnterNumberChoose(int nDirType, int nGroupId)
{
    if (!EnterState(DIR_UI_STATE_CHOOSE_NUMBER, nDirType, nGroupId))
    {
        return;
    }

    CDirectoryAdapterPtr pAdapter = GetActiveAdapter();
    CNumberAdapter * pNumberAdapter = CNumberAdapter::GetNumberAdapter(pAdapter);

    if (NULL == pNumberAdapter)
    {
        return;
    }

    CDlgDirectoryBase * pDlgDirectory = CreateDirDialog(DIR_UI_STATE_CHOOSE_NUMBER);

    // 创建显示界面
    if (NULL == pDlgDirectory)
    {
        return;
    }

    xListView * pListView = pDlgDirectory->GetListView();

    if (NULL == pListView)
    {
        UIManager_PutPrivateDlg(pDlgDirectory);
        return;
    }

    pListView->SetAdapter(pNumberAdapter);

    UIManager_SetTopWindow(pDlgDirectory);

    pNumberAdapter->EnterNumberChoose();
}

void CDirectoryMgr::EnterPhotoChoose(int nDirType, int nContactId)
{
#ifdef PHOTO_SHOW
    CDlgCustomPhoto* pDlgCustomPhoto = static_cast<CDlgCustomPhoto*>(UIManager_GetPrivateDlg(DLG_CDlgCustomPhoto));

    if (NULL != pDlgCustomPhoto && pDlgCustomPhoto == UIManager_GetTopWindow())
    {
        return;
    }

    if (NULL == pDlgCustomPhoto)
    {
        pDlgCustomPhoto = static_cast<CDlgCustomPhoto*>(UIManager_CreatePrivateDlg(DLG_CDlgCustomPhoto));

        if (NULL == pDlgCustomPhoto)
        {
            return;
        }
    }

    UIManager_SetTopWindow(pDlgCustomPhoto);
#endif
}

void CDirectoryMgr::EnterBsftPwd()
{
    CDirectoryAdapterPtr pAdapter = GetActiveAdapter();

    if (pAdapter.IsEmpty())
    {
        return;
    }

    int nDirType = pAdapter->GetDirType();

    if (DIRECTORY_TYPE_BROADSOFT != nDirType && CALL_LOG_TYPE_BROADSOFT != nDirType)
    {
        return;
    }

    if (!EnterState(DIR_UI_STATE_CHG_PWD, nDirType, pAdapter->GetGroupId()))
    {
        return;
    }

    CBsftPwdAdapter * pBsftPwdAdapter = CBsftPwdAdapter::GetBsftPwdAdapter(m_pActiveAdapter);

    if (NULL == pBsftPwdAdapter)
    {
        return;
    }

    CDlgDirectoryBase * pDlgDirectory = CreateDirDialog(DIR_UI_STATE_CHG_PWD);

    if (NULL == pDlgDirectory)
    {
        return;
    }

    xListView * pListView = pDlgDirectory->GetListView();

    if (NULL == pListView)
    {
        UIManager_PutPrivateDlg(pDlgDirectory);
        return;
    }

    pListView->SetAdapter(pBsftPwdAdapter);

    UIManager_SetTopWindow(pDlgDirectory);

    pBsftPwdAdapter->EnterPwdSetting();
}

#if IF_FEATURE_34162
void CDirectoryMgr::EnterLDAPPwd()
{
    CDirectoryAdapterPtr pAdapter = GetActiveAdapter();

    if (pAdapter.IsEmpty())
    {
        return;
    }

    int nDirType = pAdapter->GetDirType();

    if (DIRECTORY_TYPE_LDAP != nDirType)
    {
        return;
    }

    if (!EnterState(DIR_UI_STATE_LDAP_CHG_PWD, nDirType, pAdapter->GetGroupId()))
    {
        return;
    }

    CLDAPPwdAdapter * pLDAPPwdAdapter = CLDAPPwdAdapter::GetLDAPPwdAdapter(m_pActiveAdapter);

    if (NULL == pLDAPPwdAdapter)
    {
        return;
    }

    CDlgDirectoryBase * pDlgDirectory = CreateDirDialog(DIR_UI_STATE_LDAP_CHG_PWD);

    if (NULL == pDlgDirectory)
    {
        return;
    }

    xListView * pListView = pDlgDirectory->GetListView();

    if (NULL == pListView)
    {
        UIManager_PutPrivateDlg(pDlgDirectory);
        return;
    }

    pListView->SetAdapter(pLDAPPwdAdapter);

    UIManager_SetTopWindow(pDlgDirectory);

    pLDAPPwdAdapter->EnterPwdSetting();
}
#endif

void CDirectoryMgr::EnterAccountChoose(int nDirType, int nGroupId, bool bRegisterAccount /*= true*/)
{
    if (!EnterState(DIR_UI_STATE_CHOOSE_ACCOUNT, nDirType, nGroupId))
    {
        return;
    }

    CDirectoryAdapterPtr pAdapter = GetActiveAdapter();
    CAccountAdapter* pAccountAdapter = CAccountAdapter::GetAccountAdapter(m_pActiveAdapter);

    if (NULL == pAccountAdapter)
    {
        return;
    }

    CDlgDirectoryBase * pDlgDirectory = CreateDirDialog(DIR_UI_STATE_CHOOSE_ACCOUNT);

    // 创建显示界面
    if (NULL == pDlgDirectory)
    {
        return;
    }

    xListView * pListView = pDlgDirectory->GetListView();

    if (NULL == pListView)
    {
        UIManager_PutPrivateDlg(pDlgDirectory);
        return;
    }

    pListView->SetAdapter(pAccountAdapter);

    UIManager_SetTopWindow(pDlgDirectory);

    pAccountAdapter->EnterAccountChoose(bRegisterAccount);
}

#if DIR_SUPPORT_SELECT_CONTACT
void CDirectoryMgr::SetContactSelector(DIR_SELECT_CONTACT_FUN pFunSelectContact)
{
    m_pFunSelectContact = pFunSelectContact;
}

void CDirectoryMgr::OnSelectContact(yl::string strContactName, yl::string strContactNumber)
{
    DIR_INFO("CDirectoryMgr::OnSelectContact %s, %s", strContactName.c_str(), strContactNumber.c_str());

    if (NULL != m_pFunSelectContact)
    {
        m_pFunSelectContact(strContactName, strContactNumber);
        m_pFunSelectContact = NULL;
    }

    DM_ExitDirectory();

    CBaseDialog * pDlgTop = UIManager_GetTopWindow();
    while (NULL != pDlgTop && eWT_Directory == pDlgTop->GetWindowType())
    {
        UIManager_PutPrivateDlg(pDlgTop);
        pDlgTop = UIManager_GetTopWindow();
    }
}

void CDirectoryMgr::CancelSelectDirectory()
{
    m_pFunSelectContact = NULL;
}

bool CDirectoryMgr::IsInSelecting()
{
    return NULL != m_pFunSelectContact;
}

#endif
