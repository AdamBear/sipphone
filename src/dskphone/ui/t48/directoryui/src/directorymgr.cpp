#include "baseui/framelist.h"
#include "baseui/treelist.h"

#include "directorymgr.h"

#include "delegatefactory.h"
#include "directorycommon.h"
#include "dlgcalllog.h"
#include "dlgdirectory.h"
#include "dlgdirdetail.h"
#include "dlgdiredit.h"
#include "dlgdirnew.h"
#include "dlgdirsearch.h"
#include "dlgdirsetting.h"
#include "dirctrlfactory.h"
#include "translateiddefine.h"
#include "browserdelegate.h"
#include "detaildelegate.h"
#include "precisesearchdelegate.h"
#include "searchdelegate.h"
#include "settingdelegate.h"
#include "contactphotoselector.h"
#include "groupselector.h"
#include "numberselector.h"
#include "ringselector.h"
#include "bsftcalllogcontroler.h"
#include "bsftcontactcontroler.h"
#include "ldapcontactcontroler.h"
#include "btuccontactcontroler.h"
#include "contactgroupdelegate.h"
#include "ldapcontactcontroler.h"
#include "remoteserversearchcontroler.h"
#include "groupedittitle.h"
#include "bsftpwdmgr.h"
#include "ringselector.h"
#include "transfernumselector.h"
#include "presence/include/presence_inc.h"
#include "qtcommon/qmisc.h"
#include "etlmsghandler/modetlmsghandler.h"
#include "uikernel/languagehelper.h"
#include "uimanager/moduimanager.h"
#include "callmanager/modcallmanager.h"
#include "messagebox/selectmessagebox.h"
#include "messagebox/modmessagebox.h"
#include "moddirectoryui.h"
#include "dlgcalllog.h"
#include "globalinputhook/modinputhook.h"

#include "configiddefine.h"
#include "configparser/modconfigparser.h"
#include "remotecontactcontroler.h"
#include "edkui/modedkui.h"
#include "edkui/edkmapdefine.h"

#include "edk/include/modedk.h"
#include "uimanager/moduimanager.h"
#include "qtcommon/qmisc.h"

#include "additiondelegate.h"
#if IF_FEATURE_GBBOOK
#include "accountselector.h"
#endif

#if IF_FEATURE_MUTI_CALLLOG
#include "account/include/modaccount.h"
#endif


#if IF_FEATURE_METASWITCH_CALLLOG
#include "mtswcalllogcontroler.h"
#endif

#if IF_FEATURE_METASWITCH
#include "metaswitch/mtswcontrol/include/mtsw_messagedefine.h"
#endif

#if IF_FEATURE_METASWITCH_DIRECTORY
#include "additiondelegate.h"
#include "mtswcontactcontroler.h"
#endif
IMPLEMENT_GETINSTANCE(CDirectoryMgr)

bool CallLog_KeypadHookCallbackFun(int nKeyCode, bool bPress)
{
    bool bEnableCallHistoryByOffHook = (1 == configParser_GetConfigInt(kszCallHistoryByOffHook));
    if (!bEnableCallHistoryByOffHook)
    {
        return false;
    }

    bool bRet = false;
    CDlgCallLog * pDlgCallLog = qobject_cast<CDlgCallLog *>(UIManager_GetPrivateDlg(DLG_CDlgCallLog));
    if (NULL != pDlgCallLog && callmanager_GetCurrentPageName() == CALLMGR_PAGE_NAME_CALL_LOG)
    {
        bRet = pDlgCallLog->KeypadHook(nKeyCode, bPress);
    }

    CDlgDirDetail * pDlgDirDetail = qobject_cast<CDlgDirDetail *>(UIManager_GetPrivateDlg(
                                        DLG_CDlgDirDetail));
    if (NULL != pDlgDirDetail && UIManager_GetTopWindow() == pDlgDirDetail)
    {
        bRet |= pDlgDirDetail->KeypadHook(nKeyCode, bPress);
    }
    return bRet;
}

CDirectoryMgr::CDirectoryMgr()
    : m_nModType(DIR_MOD_TYPE_NONE)
    , m_nLastType(DIRECTORY_TYPE_NONE)
    , m_nCurType(DIRECTORY_TYPE_NONE)
    , m_nLastGroupId(knInvalidRId)
    , m_nCurGroupId(knInvalidRId)
    , m_nLastPageDataIndex(0)
    , m_nCurPageDataIndex(0)
    , m_pPhotoSelector(NULL)
    , m_pGroupSelector(NULL)
    , m_pNumSelector(NULL)
    , m_pRingSelector(NULL)
#if IF_FEATURE_GBBOOK
    , m_pAccSelector(NULL)
#endif
    , m_pTransferNumSelector(NULL)
#if DIR_SUPPORT_SELECT_CONTACT
    , m_pFunSelectContact(NULL)
#endif
    , m_bIsEditCallLog(false)
{
    etl_RegisterMsgHandle(DIRECTORY_MESSAGE_BEGIN, DIRECTORY_MESSAGE_END, &CDirectoryMgr::OnMessage);
    etl_RegisterMsgHandle(CALLLOG_MESSAGE_BEGIN, CALLLOG_MESSAGE_END, &CDirectoryMgr::OnMessage);
    etl_RegisterMsgHandle(PRESENCE_MESSAGE_FAVORITE_CHANGE, PRESENCE_MESSAGE_FAVORITE_CHANGE,
                          &CDirectoryMgr::OnMessage);
#if IF_FEATURE_METASWITCH
    etl_RegisterMsgHandle(MTSW_MESSAGE_LOGIN, MTSW_MESSAGE_AUTH_ERR, &CDirectoryMgr::OnMessage);
#endif
    // 注册按键回调函数
    inputHook_RegisterKeyPadHookProc(CallLog_KeypadHookCallbackFun);
}

CDirectoryMgr::~CDirectoryMgr()
{
    etl_UnregisterMsgHandle(DIRECTORY_MESSAGE_BEGIN, DIRECTORY_MESSAGE_END, &CDirectoryMgr::OnMessage);
    etl_UnregisterMsgHandle(CALLLOG_MESSAGE_BEGIN, CALLLOG_MESSAGE_END, &CDirectoryMgr::OnMessage);
    etl_UnregisterMsgHandle(PRESENCE_MESSAGE_FAVORITE_CHANGE, PRESENCE_MESSAGE_FAVORITE_CHANGE,
                            &CDirectoryMgr::OnMessage);
#if IF_FEATURE_METASWITCH
    etl_UnregisterMsgHandle(MTSW_MESSAGE_LOGIN, MTSW_MESSAGE_AUTH_ERR, &CDirectoryMgr::OnMessage);
#endif
    UnInit();
}

void CDirectoryMgr::Init()
{
    REGISTER_DLG(CDlgCallLog);
    REGISTER_DLG(CDlgDirectory);
    REGISTER_DLG(CDlgDirDetail);
    REGISTER_DLG(CDlgDirEdit);
    REGISTER_DLG(CDlgDirNew);
    REGISTER_DLG(CDlgDirSearch);
    REGISTER_DLG(CDlgDirSetting);

    m_pGroupDelegate.Reset();
    m_pActiveDelegate.Reset();
}

void CDirectoryMgr::UnInit()
{
    DIRUI_INFO("CDirectoryMgr::UnInit()");

    //在Control 里面UnInit会导致进入详情界面等也清空数据
    ClearDirectoryLogicData();

    if (!m_pGroupDelegate.IsEmpty())
    {
        m_pGroupDelegate.Reset();
    }

    if (!m_pActiveDelegate.IsEmpty())
    {
        m_pActiveDelegate->UnInit();
        m_pActiveDelegate.Reset();
    }

    m_nLastType = DIRECTORY_TYPE_NONE;
    m_nCurType = DIRECTORY_TYPE_NONE;
    m_nLastGroupId = knInvalidRId;
    m_nCurGroupId = knInvalidRId;
    m_nLastPageDataIndex = 0;
    m_nCurPageDataIndex = 0;

    ReleaseSelector();

#if DIR_SUPPORT_SELECT_CONTACT
    CancelSelectDirectory();
#endif

    SetIsEditCallLog(false);
}

CContactPhotoSelector * CDirectoryMgr::GetPhotoSelector()
{
    if (NULL != m_pPhotoSelector)
    {
        return m_pPhotoSelector;
    }

    m_pPhotoSelector = new CContactPhotoSelector;

    return m_pPhotoSelector;
}

CGroupSelector * CDirectoryMgr::GetGroupSelector()
{
    if (NULL != m_pGroupSelector)
    {
        return m_pGroupSelector;
    }

    m_pGroupSelector = new CGroupSelector;

    return m_pGroupSelector;
}

CNumberSelector * CDirectoryMgr::GetNumberSelector()
{
    if (NULL != m_pNumSelector)
    {
        return m_pNumSelector;
    }

    m_pNumSelector = new CNumberSelector;

    return m_pNumSelector;
}

CRingSelector * CDirectoryMgr::GetRingSelector()
{
    if (NULL != m_pRingSelector)
    {
        return m_pRingSelector;
    }

    m_pRingSelector = new CRingSelector;

    return m_pRingSelector;
}

#if IF_FEATURE_GBBOOK
CAccountSelector * CDirectoryMgr::GetAccountSelector()
{
    if (NULL != m_pAccSelector)
    {
        return m_pAccSelector;
    }

    m_pAccSelector = new CAccountSelector;

    return m_pAccSelector;
}
#endif

CTransferNumSelector * CDirectoryMgr::GetTransferNumSelector()
{
    if (NULL == m_pTransferNumSelector)
    {
        m_pTransferNumSelector = new CTransferNumSelector;
    }

    return m_pTransferNumSelector;
}

void CDirectoryMgr::ReleaseSelector()
{
    if (NULL != m_pPhotoSelector)
    {
        delete m_pPhotoSelector;
        m_pPhotoSelector = NULL;
    }

    if (NULL != m_pGroupSelector)
    {
        delete m_pGroupSelector;
        m_pGroupSelector = NULL;
    }

    if (NULL != m_pNumSelector)
    {
        delete m_pNumSelector;
        m_pNumSelector = NULL;
    }

    if (NULL != m_pRingSelector)
    {
        delete m_pRingSelector;
        m_pRingSelector = NULL;
    }

#if IF_FEATURE_GBBOOK
    if (NULL != m_pAccSelector)
    {
        delete m_pAccSelector;
        m_pAccSelector = NULL;
    }
#endif

    if (NULL != m_pTransferNumSelector)
    {
        delete m_pTransferNumSelector;
        m_pTransferNumSelector = NULL;
    }
}

void CDirectoryMgr::SetIsEditCallLog(bool bIsEditCallLog)
{
    m_bIsEditCallLog = bIsEditCallLog;
}

bool CDirectoryMgr::GetIsEditCallLog()
{
    return m_bIsEditCallLog;
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

    UIManager_PutPrivateDlg(DLG_CDlgDirDetail);
    UIManager_PutPrivateDlg(DLG_CDlgDirEdit);
    UIManager_PutPrivateDlg(DLG_CDlgDirNew);
    UIManager_PutPrivateDlg(DLG_CDlgDirSearch);
    callmanager_PutPage(CALLMGR_PAGE_NAME_CALL_LOG);
    callmanager_PutPage(CALLMGR_PAGE_NAME_DIRECTORY);
}

void CDirectoryMgr::CancelSelectDirectory()
{
    m_pFunSelectContact = NULL;
}

bool CDirectoryMgr::GetIsInSelecting()
{
    return NULL != m_pFunSelectContact;
}
#endif

LRESULT CDirectoryMgr::OnMessage(msgObject & refMessage)
{
    DIRUI_INFO("CDirectoryMgr::OnMessage(%u)", refMessage.message);

    CDirectoryListDelegatePtr pListDelegate = g_DirectoryMgr.GetActiveDelegate();

    // 搜索消息只发送给当前的视图处理
    if (LOCAL_SEARCH_RESULT == refMessage.message)
    {
        CSearchDelegate * pSearchDelegate = CSearchDelegate::GetDirDelegate(pListDelegate);

        if (NULL == pSearchDelegate)
        {
            return TRUE;
        }

        CDirectoryControlerPtr pControler = pSearchDelegate->GetControler();

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
    else if (PRESENCE_MESSAGE_FAVORITE_CHANGE == refMessage.message
             || (refMessage.message >= BROADSOFT_BUDDYLIST_MESSAGE_BEGIN
                 && refMessage.message <= BROADSOFT_DIRECTORY_MESSAGE_END))
    {
        nDirType = DIRECTORY_TYPE_BTUC;
    }
    else if (refMessage.message >= BLUETOOTH_CONTACT_BEGIN
             && refMessage.message <= BLUETOOTH_CONTACT_END)
    {
        nDirType = DIRECTORY_TYPE_BLUETOOTH;
    }
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
#if IF_FEATURE_GBBOOK
    else if (GENBAND_BOOK_REQ_RESULT == refMessage.message || GENBAND_BOOK_RELOAD == refMessage.message)
    {
        nDirType = DIRECTORY_TYPE_GENBAND;
    }
#endif
#if IF_FEATURE_METASWITCH_CALLLOG
    else if (refMessage.message >= METASWITCH_CALLLOG_MESSAGE_BEGIN
             && refMessage.message <= METASWITCH_CALLLOG_MESSAGE_END)
    {
        nDirType = CALL_LOG_TYPE_MTSW;
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (refMessage.message >= METASWITCH_DIRECTORY_MESSAGE_BEGIN
             && refMessage.message <= METASWITCH_PHONEBOOK_MESSAGE_END)
    {
        nDirType = DIRECTORY_TYPE_METASWICTH;
    }
#endif
#if IF_FEATURE_METASWITCH
    if (refMessage.message == MTSW_MESSAGE_LOGIN
            || refMessage.message == MTSW_MESSAGE_AUTH_ERR)
    {
        nDirType = g_DirectoryMgr.GetActiveDirType();
        DIRUI_INFO("g_DirectoryMgr.GetActiveDirType() %d", nDirType);
    }
#endif

    CDirectoryControlerPtr pControler = g_DirCtrlFactory.FindControler(nDirType);
    CGroupDelegatePtr pGroupDelegate = g_DirectoryMgr.GetGroupDelegate();

    if (!pControler.IsEmpty())
    {
        retValue = pControler->OnMessage(refMessage);
    }
    else if (DIRECTORY_TYPE_NONE != nDirType)
    {
        retValue = TRUE;
    }

    if (!pGroupDelegate.IsEmpty())
    {
        pGroupDelegate->OnMessage(refMessage);
    }

    return retValue;
}

void CDirectoryMgr::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    switch (pMessageBox->GetType())
    {
    case MESSAGEBOX_NOTE:
        {
            int nAction = pMessageBox->GetID();
            int nId = CDirUIHelper::PopMsgBoxActionId(nAction);

            CDirUIHelper::OnCancelWaiting();

            if (MessageBox_Cancel == pMessageBox->GetResult() && -1 != nAction && DIR_ACTION_NONE != nAction)
            {
                CDirectoryControlerPtr pControler = GetActiveControler();

                if (!pControler.IsEmpty())
                {
                    pControler->ExecAction(nAction, INVALID_ITEM_INDEX, nId);
                }
            }
        }
        break;
    case MESSAGEBOX_Cancel:
        {
            int nAction = pMessageBox->GetID();
            int nId = CDirUIHelper::PopMsgBoxActionId(nAction);

            CDirUIHelper::OnCancelWaiting();

            if (MessageBox_Cancel == pMessageBox->GetResult() && -1 != nAction && DIR_ACTION_NONE != nAction)
            {
                if (configParser_GetConfigInt(kszBsftDirUpdateEnable))
                {
                    CDirectoryControlerPtr pControler = GetActiveControler();
                    CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);
                    if (pBSFTContact != NULL)
                    {
                        pBSFTContact->CancelAction(nAction, INVALID_ITEM_INDEX, nId);
                    }
                    break;
                }
                CDirectoryControlerPtr pControler = GetActiveControler();

                if (!pControler.IsEmpty())
                {
                    pControler->CancelAction(nAction, INVALID_ITEM_INDEX, nId);
                }
            }
        }
        break;
    case MESSAGEBOX_OkCancel:
        {
            int nAction = pMessageBox->GetID();
            int nId = CDirUIHelper::PopMsgBoxActionId(nAction);

            if (MessageBox_OK == pMessageBox->GetResult() && -1 != nAction && DIR_ACTION_NONE != nAction)
            {
                // 删除群组转给CContactGroupDelegate处理
                if (DIR_ACTION_LOCAL_DEL_GROUP == nAction
#if IF_FEATURE_GBBOOK
                        || DIR_ACTION_GENBAND_DEL_GROUP == nAction
#endif
                   )
                {
                    CGroupDelegatePtr pActiveGroupDelegate = GetGroupDelegate();
                    CContactGroupDelegate * pContactGroup = CContactGroupDelegate::GetGroupDelegate(
                            pActiveGroupDelegate);

                    if (NULL != pContactGroup)
                    {
                        pContactGroup->DelGroup(nId);
                    }
                    break;
                }

                CDirectoryControlerPtr pControler = GetActiveControler();

                if (!pControler.IsEmpty())
                {
                    pControler->ExecAction(nAction, INVALID_ITEM_INDEX, nId);
                }
            }
        }
        break;
    case MESSAGEBOX_SELECT:
        {
            int nAction = pMessageBox->GetID();

            if (DIR_UI_ACTION_RING_SETTING == nAction)
            {
                CRingSelector * pRingSelector = GetRingSelector();

                if (NULL != pRingSelector)
                {
                    pRingSelector->StopRingPlay();
                }
            }
#if IF_FEATURE_GBBOOK
            else if (DIR_UI_ACTION_ACCOUNT_CHOOSE == nAction)
            {
                CAccountSelector * pAccSelector = GetAccountSelector();

                if (NULL == pAccSelector)
                {
                    break;
                }

                int nAccountId = pAccSelector->GetDialAccountId();

                //CDirUIHelper::SetDialAccountId(nAccountId);

                CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
                CCommonDirCtrl * pCommonDirCtrl = CCommonDirCtrl::GetCommonControler(pControler);
                if (pCommonDirCtrl != NULL)
                {
                    pCommonDirCtrl->DialContact(pCommonDirCtrl->GetContactId(), nAccountId);
                }
                break;
            }
            else if (DIR_UI_ACTION_TRANSFER_NUMBER_CHOOSE == nAction)
            {
                // transfer 类型
                CTransferNumSelector * pNumberSelector = GetTransferNumSelector();

                if (NULL == pNumberSelector)
                {
                    break;
                }

                yl::string strNumber;

                if (!pNumberSelector->GetNumber(strNumber))
                {
                    break;
                }

                int nAccountId = CDirUIHelper::GetDialAccountId();

                CDirectoryControlerPtr pControler = GetActiveControler();
                if (!pControler.IsEmpty())
                {
                    if (MessageBox_Send == pMessageBox->GetResult())
                    {
                        pControler->SetBlindTransfer(false);
                    }
                    else if (MessageBox_BTransfer == pMessageBox->GetResult())
                    {
                        pControler->SetBlindTransfer(true);
                    }
                    else
                    {
                        break;
                    }
                }
                CDirUIHelper::Dial(nAccountId, pNumberSelector->GetName(), strNumber, pControler.Get(), NULL);
            }

            if (MessageBox_Account == pMessageBox->GetResult())
            {
                CAccountSelector * pAccSelector = g_DirectoryMgr.GetAccountSelector();

                if (NULL == pAccSelector)
                {
                    return ;
                }

                MsgBoxSoftkey skCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
                VecMsgBoxSoftkey vecSoftKey;

                vecSoftKey.push_back(skCancel);

                MessageBox_ShowSelectBox(&g_DirectoryMgr, vecSoftKey, SELECT_POP_BOX_TYPE_TINY, pAccSelector,
                                         LANG_TRANSLATE(TRID_CHOOSE_ACCOUNT), DIR_UI_ACTION_ACCOUNT_CHOOSE);

                break;
            }
#endif

            if (MessageBox_OK != pMessageBox->GetResult())
            {
                break;
            }

            if (DIR_UI_ACTION_RING_SETTING == nAction)
            {
                OnRingSetting();
            }
            else if (DIR_UI_ACTION_NUMBER_CHOOSE == nAction)
            {
                CNumberSelector * pNumberSelector = GetNumberSelector();

                if (NULL == pNumberSelector)
                {
                    break;
                }

                yl::string strNumber;

                if (!pNumberSelector->GetNumber(strNumber))
                {
                    break;
                }

                int nAccountId = CDirUIHelper::GetDialAccountId();

                CDirectoryControlerPtr pControler = GetActiveControler();

                CDirUIHelper::Dial(nAccountId, pNumberSelector->GetName(), strNumber, pControler.Get(), NULL);
            }
            else if (DIR_UI_ACTION_COMMON_SELECT_BOX == nAction)
            {
                int nSelectAction = CDirUIHelper::PopSelectBoxAction();

                if (DIR_ACTION_NONE == nSelectAction)
                {
                    break;
                }

                CGroupSelector * pGroupSelector = GetGroupSelector();

                if (NULL == pGroupSelector)
                {
                    break;
                }

                CDirectoryControlerPtr pControler = GetActiveControler();

                if (!pControler.IsEmpty())
                {
                    pControler->ExecAction(nSelectAction, INVALID_ITEM_INDEX, pGroupSelector->GetSelectGroupId());
                }
            }
        }
        break;
    case MESSAGEBOX_BSFT_PWD:
        {
            if (MessageBox_OK != pMessageBox->GetResult())
            {
                break;
            }

            yl::string strPwd = pMessageBox->GetExtraData().toString().toUtf8().data();
            int nDirType = pMessageBox->GetID();

            RetryChgPwdRuest(nDirType, strPwd);
        }
        break;
    case MESSAGEBOX_LDAP_PWD:
        {
            if (MessageBox_OK != pMessageBox->GetResult())
            {
                break;
            }

            //int nDirType = pMessageBox->GetID();
            CDirectoryControlerPtr pControler = GetActiveControler();
            CLDAPContactControler * pLdapContact = CLDAPContactControler::GetControler(pControler);

            if (NULL != pLdapContact)
            {
                pLdapContact->OnAction(DIR_ACTION_LDAP_CHG_PWD, -1, knInvalidRId);
            }

        }
        break;
#if IF_FEATURE_METASWITCH_DIRECTORY
    case MESSAGEBOX_MTSW_COMMPORTAL:
        {
            if (MessageBox_OK != pMessageBox->GetResult())
            {
                break;
            }

            int nDirType = pMessageBox->GetID();
            CDirectoryControlerPtr pControler = GetActiveControler();

            if (DIRECTORY_TYPE_METASWICTH == nDirType)
            {
                CMTSWContactControler * pMTSWContact = CMTSWContactControler::GetControler(pControler);

                if (NULL != pMTSWContact)
                {
                    pMTSWContact->OnAction(DIR_ACTION_MTSW_CHG_PWD, -1, knInvalidRId);
                }
            }
            else if (CALL_LOG_TYPE_MTSW == nDirType)
            {
                CMTSWCallLogControler * pMTSWCallLog = CMTSWCallLogControler::GetControler(pControler);

                if (NULL != pMTSWCallLog)
                {
                    pMTSWCallLog->OnAction(DIR_ACTION_MTSW_CHG_PWD, -1, knInvalidRId);
                }
            }
        }
        break;
#endif
    default:
        break;
    }

    g_DirectoryMgr.SetIsEditCallLog(false);
}

CDirectoryListDelegatePtr CDirectoryMgr::GetActiveDelegate()
{
    return m_pActiveDelegate;
}

CDirectoryControlerPtr CDirectoryMgr::GetActiveControler()
{
    if (!m_pActiveDelegate.IsEmpty())
    {
        return m_pActiveDelegate->GetControler();
    }

    // 新建联系人场景
    CDlgDirNew * pDlgDirNew = (CDlgDirNew *)UIManager_GetPrivateDlg(DLG_CDlgDirNew);

    if (NULL != pDlgDirNew && UIManager_GetTopWindow() == pDlgDirNew)
    {
        int nDirType = pDlgDirNew->GetDirType();

        return g_DirCtrlFactory.GetControler(nDirType);
    }

    return CDirectoryControlerPtr(NULL);
}

CDirUIHelperPtr CDirectoryMgr::GetActiveUIHelper()
{
    if (!m_pActiveDelegate.IsEmpty())
    {
        return m_pActiveDelegate->GetUIHelper();
    }

    return CDirUIHelperPtr(NULL);
}

CGroupDelegatePtr CDirectoryMgr::GetGroupDelegate()
{
    return m_pGroupDelegate;
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

void CDirectoryMgr::OnUIShow(int nUIState, CTreeList * pTreeList, CFrameList * pFrameList)
{
    DIRUI_INFO("OnUIShow(State:%d TreeList:%p FrameList:%p)", nUIState, pTreeList, pFrameList);

    if ((DIR_UI_STATE_BROWSER != nUIState && DIR_UI_STATE_SETTING != nUIState)
            || NULL == pTreeList || NULL == pFrameList)
    {
        DIRUI_INFO("CDirectoryMgr::OnUIShow() fail. UIState:%d ThreeList:%p FrameList:%p",
                   nUIState, pTreeList, pFrameList);
        return;
    }

    CGroupDelegatePtr pActiveGroupDelegate = GetGroupDelegate();
    CDirectoryListDelegatePtr pActiveDelegate = GetActiveDelegate();

    if (pActiveGroupDelegate.IsEmpty())
    {
        DIRUI_INFO("CDirectoryMgr::OnUIShow() fail.GroupDelegate:%p ActiveDelegate:%p",
                   pActiveGroupDelegate.Get(), pActiveDelegate.Get());
        return;
    }

    bool bUIReturn = false;

    if (pActiveDelegate.IsEmpty() || pActiveDelegate->GetState() != nUIState)
    {
        bUIReturn = true;
    }

    CGroupDelegate * pGroupDelegate = pActiveGroupDelegate.Get();

    if (pTreeList->GetFrameDelegate() != pGroupDelegate)
    {
        pTreeList->SetDelegate(pGroupDelegate);

        CFrameListTitlePtr pFrameTitle = pTreeList->GetFrameListTitle();

        if (NULL != pFrameTitle)
        {
            pFrameTitle->SetAction(pGroupDelegate);
        }
    }

    if (bUIReturn)
    {
        if (DIR_UI_STATE_BROWSER == nUIState)
        {
            pGroupDelegate->SetSetting(false);
        }
        else
        {
            pGroupDelegate->SetSetting(true);
        }

        pGroupDelegate->ExpandToSelectGroup();

        int nDirType = pGroupDelegate->GetSelectGroupType();

        if (nDirType != m_nCurType)
        {
            m_nCurType = nDirType;
        }

        CDirectoryControlerPtr pControler = g_DirCtrlFactory.FindControler(nDirType);
        int nGroupId = pGroupDelegate->GetSelectGroupId();

        if (!pControler.IsEmpty())
        {
            nGroupId = pControler->GetGroupId();
        }

        CDirectoryListDelegatePtr pReturnDelegate = g_DirDelegateFactory.FindContactDelegate(nUIState);
        int nOriPageDataIndex = 0;

        if (!pReturnDelegate.IsEmpty())
        {
            nOriPageDataIndex = pReturnDelegate->GetPageDataIndex();
        }

        CheckContactDelegate(nUIState, nGroupId, false);

        pActiveDelegate = GetActiveDelegate();

        if (0 != nOriPageDataIndex && !pActiveDelegate.IsEmpty())
        {
            pActiveDelegate->SetPageDataIndex(nOriPageDataIndex);
        }
    }

    CDirectoryListDelegate * pDelegate = pActiveDelegate.Get();

    if (NULL == pDelegate)
    {
        DIRUI_INFO("CDirectoryMgr::OnUIShow() fail.ActiveDelegate is nill");
        return;
    }

    if (pFrameList->GetFrameDelegate() != pDelegate)
    {
        pFrameList->SetDelegate(pDelegate);

        CDirUIHelperPtr pUIHelper = pDelegate->GetUIHelper();

        if (!pUIHelper.IsEmpty())
        {
            pUIHelper->OnGroupChange();
        }
    }

    bool bReadOnly = !CDirectoryControler::IsDirCanEidt(m_nModType);

    if (DIR_UI_STATE_BROWSER == nUIState)
    {
        pGroupDelegate->SetReadOnly(bReadOnly);
    }

    pGroupDelegate->OnChangeGroup(m_nCurType);

    if (!bUIReturn || DIRECTORY_TYPE_NONE == m_nCurType)
    {
        pGroupDelegate->RefreshFrameList();
    }

    CDirectoryControlerPtr pControler = GetActiveControler();

    // 三级权限可能会引起只有LDAP的情况
    if (!pControler.IsEmpty())
    {
        pControler->SetReadOnly(bReadOnly);

        int nGroupId = pGroupDelegate->GetSelectGroupId();

        if (!pControler.IsEmpty())
        {
            nGroupId = pControler->GetGroupId();
        }
        pControler->SetGroupId(nGroupId);

        if (bUIReturn)
        {
            pControler->PreLoadContactOnUIReturn(nGroupId);
        }
    }

    pDelegate->RefreshFrameList();
}

void CDirectoryMgr::OnUIHide(CTreeList * pTreeList, CFrameList * pFrameList)
{
    DIRUI_INFO("OnUIHide(TreeList:%p FrameList:%p)", pTreeList, pFrameList);

    if (NULL != pTreeList)
    {
        pTreeList->SetDelegate(NULL);
    }

    if (NULL != pFrameList)
    {
        pFrameList->SetDelegate(NULL);
    }
}

bool CDirectoryMgr::IsGroupShow(int nType, int nGroupId)
{
    CDirectoryControlerPtr pControler = GetActiveControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    if (pControler->GetType() != nType || pControler->GetGroupId() != nGroupId)
    {
        return false;
    }

    return true;
}

bool CDirectoryMgr::OnGroupClick(int nType, int nGroupId)
{
    if (DIRECTORY_TYPE_LDAP == nType)
    {
        return false;
    }

    CDirectoryListDelegatePtr pActiveDelegate = GetActiveDelegate();
    CBrowserDelegate * pBrowserDelegate = CBrowserDelegate::GetDirDelegate(pActiveDelegate);
    CSettingDelegate * pSettingDelegate = CSettingDelegate::GetDirDelegate(pActiveDelegate);

    if (NULL == pBrowserDelegate)
    {
        if (NULL == pSettingDelegate)
        {
            return false;
        }

        pBrowserDelegate = pSettingDelegate;
    }

    CGroupDelegatePtr pGroupDelegate = GetGroupDelegate();
    if (!pGroupDelegate.IsEmpty())
    {
        pGroupDelegate->OnChangeGroup(nType);
    }

    if (pBrowserDelegate->GetDirType() == nType)
    {
        return pBrowserDelegate->OnChangeGroup(nGroupId);
    }

    CFrameList * pFrameList = (CFrameList *)pBrowserDelegate->GetFrameList();

    if (NULL == pFrameList)
    {
        return false;
    }

    m_nCurType = nType;
    CheckContactDelegate(pBrowserDelegate->GetState(), nGroupId, false);

    pActiveDelegate = GetActiveDelegate();
    pBrowserDelegate = CBrowserDelegate::GetDirDelegate(pActiveDelegate);
    pSettingDelegate = CSettingDelegate::GetDirDelegate(pActiveDelegate);

    if (NULL == pBrowserDelegate)
    {
        if (NULL == pSettingDelegate)
        {
            return false;
        }

        pBrowserDelegate = pSettingDelegate;
    }

    pFrameList->ClearContent();
    pFrameList->SetDelegate(pBrowserDelegate);

    pBrowserDelegate->OnChangeGroup(nGroupId);

    //http://10.2.1.199/Bug.php?BugID=88401
    if (EDK_IsTipModeEnable())
    {
        yl::string strHint = EDKUI_GetHintByDirectoryType(nType);
        CBaseDialog * pDlg = UIManager_GetTopWindow();

        //联系人黑名单和本地通讯录的DirType都是DIRECTORY_TYPE_LOCAL 需要特殊处理
        if (DIRECTORY_TYPE_LOCAL == nType && nGroupId == kszBlackListGroupId)
        {
            strHint = EDK_UI_ACTION_BLACK_LIST;
        }

        if (pDlg != NULL)
        {
            pDlg->SetHintTitle(toQString(strHint));

            TitleLayoutInfo objInfo;
            if (pDlg->IsShowTitle(objInfo))
            {
                UIManager_UpdateWnd(pDlg);
            }
        }
    }

    return true;
}

void CDirectoryMgr::OnSearchClick()
{
    CGroupDelegatePtr pGroupDelegate = GetGroupDelegate();
    CDirectoryListDelegatePtr pActiveDelegate = GetActiveDelegate();

    if (pGroupDelegate.IsEmpty() || pActiveDelegate.IsEmpty())
    {
        return;
    }

    int nDirType = pActiveDelegate->GetDirType();
    int nGroupId = pActiveDelegate->GetGroupId();

    // 三级权限被禁止时，如果LDAP可用，也可以进入搜索界面
    if (DIRECTORY_TYPE_NONE == nDirType)
    {
        if (!LDAP_IsEnable())
        {
            return;
        }

        nDirType = DIRECTORY_TYPE_LDAP;
        nGroupId = knInvalidRId;
    }

#if IF_FEATURE_GBBOOK
    if (DIRECTORY_TYPE_GENBAND == nDirType
            && nGroupId == kszGabBookGroupId)
    {
        EnterPreciseSearch(nDirType, nGroupId);
        return;
    }
#endif

    EnterSearch(nDirType, nGroupId, PHONE_KEY_NONE);
}

void CDirectoryMgr::OnNewContactClick()
{
    CGroupDelegatePtr pGroupDelegate = GetGroupDelegate();

    if (pGroupDelegate.IsEmpty())
    {
        return;
    }

    CContactGroupNodePtr pGroupNode = (CContactGroupNodePtr)pGroupDelegate->GetSelectTreeDataNode();
    int nContactType = DIRECTORY_TYPE_LOCAL;
    int nGroupId = kszAllContactId;

    if (NULL != pGroupNode)
    {
        int nSelContactType = pGroupNode->GetType();

        if (DIRECTORY_TYPE_LOCAL == nSelContactType)
        {
            nGroupId = pGroupNode->GetGroupId();
        }
        else if (DIRECTORY_TYPE_BROADSOFT == nSelContactType)
        {
            nContactType = DIRECTORY_TYPE_BROADSOFT;
            nGroupId = kszPersonalId;
        }
#if IF_FEATURE_GBBOOK
        else if (DIRECTORY_TYPE_GENBAND == nSelContactType)
        {
            nContactType = DIRECTORY_TYPE_GENBAND;
            nGroupId = pGroupNode->GetGroupId() == kszAllContactId ? kszGbNAGroupId : pGroupNode->GetGroupId();
        }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
        else if (DIRECTORY_TYPE_METASWICTH == nSelContactType)
        {
            nContactType = DIRECTORY_TYPE_METASWICTH;
            nGroupId = kszContactList;
        }
#endif
    }

    EnterAddition(nContactType, nGroupId, false);
}

void CDirectoryMgr::OnSettingClick()
{
    EnterSetting();
}

bool CDirectoryMgr::EnterDirectory(int nDirType, int nGroupId, CALL_TYPE eCallType /*= CT_CALLALL*/)
{
    DIRUI_INFO("EnterDirectory(DirType:%d GroupId:%d)", nDirType, nGroupId);

    if (DIRECTORY_TYPE_LDAP == nDirType)
    {
        if (!LDAP_IsEnable())
        {
            DIRUI_ERR("Enter LDAP fail, when ldap is disabled.");
            return false;
        }

        EnterSearch(nDirType, knInvalidRId, PHONE_KEY_NONE);
        return true;
    }

    if (DIRECTORY_TYPE_BTUC == nDirType)
    {
        if (!CBTUCContactControler::IsConfigure())
        {
            DIRUI_ERR("Enter BTUC fail, when BTUC is not configured.");
            return false;
        }
    }
    else if (!CCommonDirCtrl::IsDirEnable(nDirType))
    {
        DIRUI_INFO("EnterDirectory fail, when IsDirEnable(%d) is disabled.", nDirType);
        return false;
    }

    int nModType = DIR_MOD_TYPE_NONE;
    CGroupDelegatePtr pActiveGroupDelegate = GetGroupDelegate();

    if (!pActiveGroupDelegate.IsEmpty())
    {
        nModType = pActiveGroupDelegate->GetType();
    }

    if (!CheckGroupDelegate(nDirType))
    {
        return false;
    }

    pActiveGroupDelegate = GetGroupDelegate();
    CGroupDelegate * pGroupDelegate = pActiveGroupDelegate.Get();

    if (NULL == pGroupDelegate)
    {
        return false;
    }

    bool bValid = true;

    // 特殊处理 本地联系人/黑名单
    if (DIRECTORY_TYPE_LOCAL == nDirType)
    {
        if (knInvalidRId == nGroupId)
        {
            bValid = pGroupDelegate->IsDirTypeValid(nDirType);
        }
        else
        {
            bValid = (NULL != pGroupDelegate->GetGroupNode(nDirType, nGroupId));
        }
    }
    else
    {
        bValid = pGroupDelegate->IsDirTypeValid(nDirType);
    }

    if (!bValid)
    {
        pGroupDelegate->AdjustDirType(nDirType, nGroupId);
        m_nCurType = nDirType;
    }

#if IF_FEATURE_MUTI_CALLLOG
    CALL_TYPE eMultyCallType = CT_CALLALL;
    if (BsftLog_IsMutilLogEnable()
            && CALL_LOG_TYPE_BROADSOFT == nDirType)
    {
        int nAccountID = -1;
        if (nGroupId == kszPlacedLogId)
        {
            eMultyCallType = CT_OUTGOING;
            yl::string strNumber = "";
            LocalLog_GetLastOutCallNumAndAccountID(strNumber, nAccountID);
        }
        else if (nGroupId == kszMissLogId)
        {
            eMultyCallType = CT_MISSCALL;
            nAccountID = LocalLog_GetLastChildMissAccountID();
        }

        int nAvailableNum = acc_GetAvailableAccountCount();
        if (nAvailableNum > 1 && nAccountID < 0)
        {
            nGroupId = kszRootId;
        }
        else
        {
            int nID = nAvailableNum == 1 ? acc_GetUsableAccount() : 0;
            if (nAccountID >= 0)
            {
                nID = nAccountID;
            }
            nGroupId = BsftLog_GetGroupIDByAccountID(nID, eMultyCallType);
        }
    }
#endif

    // 调整不存在的组Id
    nGroupId = pGroupDelegate->AdjustGroupId(nDirType, nGroupId);

    if (!CheckContactDelegate(DIR_UI_STATE_BROWSER, nGroupId, (nModType != pGroupDelegate->GetType())))
    {
        return false;
    }

#if IF_FEATURE_MUTI_CALLLOG
    if (CALL_LOG_TYPE_BROADSOFT == nDirType)
    {
        CDirectoryControlerPtr pControler = GetActiveControler();
        CBSFTCallLogControler * pBSFTControler = CBSFTCallLogControler::GetControler(pControler);
        if (pBSFTControler != NULL)
        {
            pBSFTControler->SetCallType(eCallType);
        }
    }
#endif

    CDirectoryListDelegatePtr pActiveDelegate = GetActiveDelegate();
    CBrowserDelegate * pBrowserDelegate = CBrowserDelegate::GetDirDelegate(pActiveDelegate);

    if (NULL == pBrowserDelegate)
    {
        return false;
    }

    nModType = pGroupDelegate->GetType();

    // 创建显示界面
    CBaseDialog * pDlgPage = NULL;
    CTreeList * pTreeList = NULL;
    CFrameList * pFrameList = NULL;
    yl::string strPageName = "";

    if (DIR_MOD_TYPE_CONTACT == nModType)
    {
        strPageName = CALLMGR_PAGE_NAME_DIRECTORY;
        CreateContactPage(pDlgPage, pTreeList, pFrameList);
    }
    else if (DIR_MOD_TYPE_CALLLOG == nModType)
    {
        strPageName = CALLMGR_PAGE_NAME_CALL_LOG;
        CreateCallLogPage(pDlgPage, pTreeList, pFrameList);
    }

    if (NULL == pDlgPage || NULL == pTreeList || NULL == pFrameList)
    {
        return false;
    }

    //设置当前窗口的提示命令
    pDlgPage->SetHintTitle(toQString(EDKUI_GetHintByDirectoryType(nDirType)));

    //联系人黑名单和本地通讯录的DirType都是DIRECTORY_TYPE_LOCAL 需要特殊处理
    if (DIRECTORY_TYPE_LOCAL == nDirType && nGroupId == kszBlackListGroupId)
    {
        pDlgPage->SetHintTitle(EDK_UI_ACTION_BLACK_LIST);
    }

    // 左侧群组初始化
    pGroupDelegate->SetSetting(false);
    pTreeList->ClearContent();
    pTreeList->SetDelegate(pGroupDelegate);

    CFrameListTitlePtr pFrameTitle = pTreeList->GetFrameListTitle();

    if (NULL != pFrameTitle)
    {
        pFrameTitle->SetAction(pGroupDelegate);
    }

    // 调整不显示的组Id，设置选中对应的显示组
    int nDefaultGroupId = pGroupDelegate->GetDefaultGroupId(nDirType, nGroupId);

    pGroupDelegate->SetDefaultGroup(nDirType, nDefaultGroupId);

#if IF_FEATURE_MUTI_CALLLOG
    //右侧需要显示默认账号的数据
    if (CALL_LOG_TYPE_BROADSOFT == nDirType && nGroupId == -1 && BsftLog_IsMutilLogEnable())
    {
        nGroupId = nDefaultGroupId;
    }
#endif

    // 右侧联系人界面初始化
    pFrameList->SetDelegate(pBrowserDelegate);

    callmanager_SetPageDlgByName(strPageName, pDlgPage);
    callmanager_EnterPage(strPageName);

    pBrowserDelegate->OnChangeGroup(nGroupId, true);

    return true;
}

bool CDirectoryMgr::SwitchLastDirectory(int nDefaultType, int nDefaultGroupId)
{
    DIRUI_INFO("SwitchLastDirectory(%d %d)", nDefaultType, nDefaultGroupId);

    int nModType = GetModType(nDefaultType);
    CDirectoryListDelegatePtr pActiveDelegate = GetActiveDelegate();

    // 多次调用容错（CallMgr在EnterDirectory后会多调用一次这个接口）
    if (m_nModType == nModType || (!pActiveDelegate.IsEmpty()
                                   && DIR_UI_STATE_BROWSER != pActiveDelegate->GetState()))
    {
        return true;
    }

    int nDirType = m_nLastType;
    int nGroupId = m_nLastGroupId;
    int nPageDataIndex = m_nLastPageDataIndex;

    if (DIRECTORY_TYPE_NONE == nDirType)
    {
        nDirType = nDefaultType;
        nGroupId = nDefaultGroupId;
        nPageDataIndex = 0;
    }

    if (!CheckGroupDelegate(nDirType))
    {
        return false;
    }

    CGroupDelegatePtr pGroupDelegate = GetGroupDelegate();

    if (pGroupDelegate.IsEmpty())
    {
        return false;
    }

    bool bValid = true;

    // 特殊处理 本地联系人/黑名单
    if (DIRECTORY_TYPE_LOCAL == nDirType)
    {
        bValid = (NULL != pGroupDelegate->GetGroupNode(nDirType, nGroupId));
    }
    else
    {
        bValid = pGroupDelegate->IsDirTypeValid(nDirType);
    }

    if (!bValid)
    {
        pGroupDelegate->AdjustDirType(nDirType, nGroupId);
        m_nCurType = nDirType;
    }

    // 调整不存在的组Id
    nGroupId = pGroupDelegate->AdjustGroupId(nDirType, nGroupId);

    if (!CheckContactDelegate(DIR_UI_STATE_BROWSER, nGroupId, true))
    {
        return false;
    }

    pActiveDelegate = GetActiveDelegate();

    if (!pActiveDelegate.IsEmpty())
    {
        pActiveDelegate->SetPageDataIndex(nPageDataIndex);
    }

    nGroupId = pGroupDelegate->GetDefaultGroupId(nDirType, nGroupId);
    pGroupDelegate->SetDefaultGroup(nDirType, nGroupId);

    return true;
}

bool CDirectoryMgr::CreateContactPage(CBaseDialog *& pDlgPage, CTreeList *& pTreeList,
                                      CFrameList *& pFrameList)
{
    CDlgDirectory * pDlgDirectory = (CDlgDirectory *)UIManager_CreatePrivateDlg(DLG_CDlgDirectory, "",
                                    false);

    if (NULL == pDlgDirectory)
    {
        return false;
    }

    pTreeList = pDlgDirectory->GetTreeList();
    pFrameList = pDlgDirectory->GetFrameList();

    if (NULL == pTreeList || NULL == pFrameList)
    {
        UIManager_PutPrivateDlg(pDlgDirectory);
        return false;
    }

    pDlgPage = pDlgDirectory;
    return true;
}

bool CDirectoryMgr::CreateCallLogPage(CBaseDialog *& pDlgPage, CTreeList *& pTreeList,
                                      CFrameList *& pFrameList)
{
    CDlgCallLog * pDlgCallLog = (CDlgCallLog *)UIManager_CreatePrivateDlg(DLG_CDlgCallLog, "", false);

    if (NULL == pDlgCallLog)
    {
        return false;
    }

    pTreeList = pDlgCallLog->GetTreeList();
    pFrameList = pDlgCallLog->GetFrameList();

    if (NULL == pTreeList || NULL == pFrameList)
    {
        UIManager_PutPrivateDlg(pDlgCallLog);
        return false;
    }

    pDlgPage = pDlgCallLog;
    return true;
}

void CDirectoryMgr::EnterSearch(int nDirType, int nGroupId, int nKeyCode)
{
    DIRUI_INFO("EnterSearch(DirType:%d GroupId:%d KeyCode:%d)", nDirType, nGroupId, nKeyCode);

    if (nDirType != m_nCurType)
    {
        m_nCurType = nDirType;
    }

    if (!CheckContactDelegate(DIR_UI_STATE_SEARCH, nGroupId, false))
    {
        DIRUI_ERR("CheckContactDelegate fail in EnterSearch");
        return;
    }

    CDirectoryListDelegatePtr pActiveDelegate = GetActiveDelegate();
    CSearchDelegate * pSearchDelegate = CSearchDelegate::GetDirDelegate(pActiveDelegate);

    if (NULL == pSearchDelegate)
    {
        DIRUI_ERR("CSearchDelegate is empty in EnterSearch");
        return;
    }

    yl::string strKey = "";

    if (!pSearchDelegate->Init(nDirType, nGroupId, strKey))
    {
        DIRUI_ERR("CSearchDelegate init fail in EnterSearch");
        return;
    }

    // 防止连续多次进入联系人界面
    CDlgDirSearch * pDlgDirSearch = (CDlgDirSearch *)UIManager_GetPrivateDlg(DLG_CDlgDirSearch);
    CBaseDialog * pDlgTop = UIManager_GetTopWindow();

    if (NULL != pDlgDirSearch && pDlgDirSearch == pDlgTop)
    {
        return;
    }

    // 进入联系人界面
    if (NULL == pDlgDirSearch)
    {
        pDlgDirSearch = (CDlgDirSearch *)UIManager_CreatePrivateDlg(DLG_CDlgDirSearch);

        if (NULL == pDlgDirSearch)
        {
            DIRUI_ERR("UIManager_CreatePrivateDlg(DLG_CDlgDirSearch) fail in EnterSearch");
            return;
        }
    }

    CFrameList * pContactFrameList = pDlgDirSearch->GetContactFrameList();

    if (NULL == pContactFrameList)
    {
        DIRUI_ERR("GetContactFrameList fail in EnterSearch");
        UIManager_PutPrivateDlg(pDlgDirSearch);
        return;
    }

    pContactFrameList->SetDelegate(pSearchDelegate);

    if (DIRECTORY_TYPE_LDAP == nDirType)
    {
        pDlgDirSearch->SetContactType(DIRECTORY_TYPE_LOCAL);
        pDlgDirSearch->SetSearchType(DIRECTORY_SEARCH_TYPE_LDAP);
    }
    else
    {
        pDlgDirSearch->SetContactType(nDirType);
        pDlgDirSearch->SetSearchType(DIRECTORY_SEARCH_TYPE_NORMAL);
    }

    //http://10.2.1.199/Bug.php?BugID=88327 添加对
    pDlgDirSearch->SetHintTitle(toQString(EDKUI_GetHintByDirectoryType(nDirType)));
    // 显示界面
    UIManager_SetTopWindow(pDlgDirSearch);
    UIManager_UpdateWnd(pDlgDirSearch);

    pDlgDirSearch->SetInitSearchKey(nKeyCode);
}

// 进入高级搜索
void CDirectoryMgr::EnterPreciseSearch()
{
    if (DIRECTORY_TYPE_BROADSOFT != m_nCurType)
    {
        m_nCurType = DIRECTORY_TYPE_BROADSOFT;
    }

    int nGroupId = kszAllContactId;
    CDirectoryControlerPtr pControler = GetActiveControler();
    CBSFTContactControler * pBSFTControler = CBSFTContactControler::GetControler(pControler);

    if (NULL != pBSFTControler)
    {
        nGroupId = pBSFTControler->GetGroupId();
    }

    if (!CheckContactDelegate(DIR_UI_STATE_PRECISE_SEARCH, nGroupId, false))
    {
        return;
    }

    CDirectoryListDelegatePtr pActiveDelegate = GetActiveDelegate();
    CPreciseSearchDelegate * pSearchDelegate = CPreciseSearchDelegate::GetDirDelegate(pActiveDelegate);

    if (NULL == pSearchDelegate)
    {
        return;
    }

    // 防止连续多次进入联系人界面
    CDlgDirSearch * pDlgDirSearch = (CDlgDirSearch *)UIManager_GetPrivateDlg(DLG_CDlgDirSearch);
    CBaseDialog * pDlgTop = UIManager_GetTopWindow();

    if (NULL != pDlgDirSearch && pDlgDirSearch == pDlgTop)
    {
        return;
    }

    // 进入联系人界面
    if (NULL == pDlgDirSearch)
    {
        pDlgDirSearch = (CDlgDirSearch *)UIManager_CreatePrivateDlg(DLG_CDlgDirSearch);

        if (NULL == pDlgDirSearch)
        {
            return;
        }
    }

    CFrameList * pContactFrameList = pDlgDirSearch->GetContactFrameList();

    if (NULL == pContactFrameList)
    {
        UIManager_PutPrivateDlg(pDlgDirSearch);
        return;
    }

    pContactFrameList->SetDelegate(pSearchDelegate);
    pDlgDirSearch->SetSearchType(DIRECTORY_SEARCH_TYPE_BROADSOFT);

    // 显示界面
    UIManager_SetTopWindow(pDlgDirSearch);
    UIManager_UpdateWnd(pDlgDirSearch);
}

// 进入高级搜索
void CDirectoryMgr::EnterPreciseSearch(int nDirType, int nGroupId)
{
    if (DIRECTORY_TYPE_BROADSOFT != nDirType
            && DIRECTORY_TYPE_GENBAND != nDirType)
    {
        return;
    }

    m_nCurType = nDirType;

    if (!CheckContactDelegate(DIR_UI_STATE_PRECISE_SEARCH, nGroupId, false))
    {
        return;
    }

    CDirectoryListDelegatePtr pActiveDelegate = GetActiveDelegate();
    CPreciseSearchDelegate * pSearchDelegate = CPreciseSearchDelegate::GetDirDelegate(pActiveDelegate);

    if (NULL == pSearchDelegate)
    {
        return;
    }

    // 防止连续多次进入联系人界面
    CDlgDirSearch * pDlgDirSearch = (CDlgDirSearch *)UIManager_GetPrivateDlg(DLG_CDlgDirSearch);
    CBaseDialog * pDlgTop = UIManager_GetTopWindow();

    if (NULL != pDlgDirSearch && pDlgDirSearch == pDlgTop)
    {
        return;
    }

    // 进入联系人界面
    if (NULL == pDlgDirSearch)
    {
        pDlgDirSearch = (CDlgDirSearch *)UIManager_CreatePrivateDlg(DLG_CDlgDirSearch);

        if (NULL == pDlgDirSearch)
        {
            return;
        }
    }

    CFrameList * pContactFrameList = pDlgDirSearch->GetContactFrameList();

    if (NULL == pContactFrameList)
    {
        UIManager_PutPrivateDlg(pDlgDirSearch);
        return;
    }

    pContactFrameList->SetDelegate(pSearchDelegate);
    pDlgDirSearch->InitPreciseSearch(nDirType);
    if (nDirType == DIRECTORY_TYPE_GENBAND)
    {
        pDlgDirSearch->SetSearchType(DIRECTORY_SEARCH_TYPE_GENBAND);
    }
    else if (nDirType == DIRECTORY_TYPE_BROADSOFT)
    {
        pDlgDirSearch->SetSearchType(DIRECTORY_SEARCH_TYPE_BROADSOFT);
    }

    // 显示界面
    UIManager_SetTopWindow(pDlgDirSearch);
    UIManager_UpdateWnd(pDlgDirSearch);
}

void CDirectoryMgr::EnterSetting()
{
    CGroupDelegatePtr pActiveGroupDelegate = GetGroupDelegate();
    CDirectoryListDelegatePtr pActiveDelegate = GetActiveDelegate();
    CGroupDelegate * pGroupDelegate = pActiveGroupDelegate.Get();
    CDirectoryControlerPtr pControler = pActiveDelegate->GetControler();

    if (NULL == pGroupDelegate || pActiveDelegate.IsEmpty() || pControler.IsEmpty())
    {
        DIRUI_ERR("EnterSetting() fail GroupDelegate:%p ActiveDelegate:%p Control:%p",
                  pGroupDelegate, pActiveDelegate.Get(), pControler.Get());
        return;
    }

    if (pControler->IsReadOnly())
    {
        DIRUI_ERR("EnterSetting() fail while isn't ediable");
        return;
    }

    // 防止连续多次进入设置界面
    CDlgDirSetting * pDlgDirSetting = (CDlgDirSetting *)UIManager_GetPrivateDlg(DLG_CDlgDirSetting);
    CBaseDialog * pDlgTop = UIManager_GetTopWindow();

    if (NULL != pDlgDirSetting && pDlgDirSetting == pDlgTop)
    {
        return;
    }

    // 进入联系人界面
    if (NULL == pDlgDirSetting)
    {
        pDlgDirSetting = (CDlgDirSetting *)UIManager_CreatePrivateDlg(DLG_CDlgDirSetting);

        if (NULL == pDlgDirSetting)
        {
            DIRUI_ERR("UIManager_CreatePrivateDlg(DLG_CDlgDirSetting) fail when EnterSetting()");
            return;
        }
    }

    CTreeList * pTreeList = pDlgDirSetting->GetTreeList();
    CFrameList * pFrameList = pDlgDirSetting->GetFrameList();

    if (NULL == pTreeList || NULL == pFrameList)
    {
        UIManager_PutPrivateDlg(pDlgDirSetting);
        DIRUI_ERR("EnterSetting() fail, TreeList:%p FrameList:%p", pTreeList, pFrameList);
        return;
    }

    int nListDirType = pActiveDelegate->GetDirType();
    int nListGroupId = pActiveDelegate->GetGroupId();

    pDlgDirSetting->SetDirType(nListDirType);

    // 左侧群组初始化
    pTreeList->ClearContent();
    pTreeList->SetDelegate(pGroupDelegate);
    pGroupDelegate->SetSetting(true);
    pGroupDelegate->ExpandToSelectGroup();

    CFrameListTitlePtr pFrameTitle = pTreeList->GetFrameListTitle();

    if (NULL != pFrameTitle)
    {
        pFrameTitle->SetAction(pGroupDelegate);

        CGroupEditTitle * pGroupEditTitle = CGroupEditTitle::GetGroupEditTitle(pFrameTitle);
        CContactGroupDelegate * pContactGroupDelegate = CContactGroupDelegate::GetGroupDelegate(
                    pActiveGroupDelegate);
        bool bNewGroupBtnVisible = false;

        if (NULL != pContactGroupDelegate && pContactGroupDelegate->IsCanAddGroup())
        {
            bNewGroupBtnVisible = true;
        }

        if (NULL != pGroupEditTitle)
        {
            pGroupEditTitle->SetBtnVisible(bNewGroupBtnVisible);
        }
    }

    // 右侧联系人界面初始化
    m_nCurType = pGroupDelegate->GetSelectGroupType();

    int nSelGroupId = pGroupDelegate->GetSelectGroupId();
    int nGroupId = AdjustContactListGroupId(nListDirType, m_nCurType, nListGroupId, nSelGroupId);

    pGroupDelegate->SetDefaultGroup(m_nCurType, nGroupId);

    CSettingDelegate * pSettingDelegate = NULL;

    if (CheckContactDelegate(DIR_UI_STATE_SETTING, nGroupId, false))
    {
        pActiveDelegate = GetActiveDelegate();
        pSettingDelegate = CSettingDelegate::GetDirDelegate(pActiveDelegate);
    }

    if (NULL == pSettingDelegate)
    {
        pTreeList->SetDelegate(NULL);

        if (NULL != pFrameTitle)
        {
            pFrameTitle->SetAction(NULL);
        }

        UIManager_PutPrivateDlg(pDlgDirSetting);
        return;
    }

    pFrameList->SetDelegate(pSettingDelegate);
    pSettingDelegate->OnChangeGroup(nGroupId);

    // 显示界面
    UIManager_SetTopWindow(pDlgDirSetting);
    UIManager_UpdateWnd(pDlgDirSetting);
}

bool CDirectoryMgr::CheckGroupDelegate(int nDirType)
{
    DIRUI_INFO("CheckGroupDelegate(%d)", nDirType);

    int nModType = GetModType(nDirType);
    int nLastPageDataIndex = m_nLastPageDataIndex;
    int nLastGroupId = m_nLastGroupId;
    int nLastType = m_nLastType;
    CGroupDelegatePtr pGroupDelegate = GetGroupDelegate();
    CDirectoryListDelegatePtr pActiveDelegate = GetActiveDelegate();

    if (pGroupDelegate.IsEmpty() || pActiveDelegate.IsEmpty())
    {
        m_nLastType = DIRECTORY_TYPE_NONE;
        m_nCurGroupId = knInvalidRId;
        m_nLastGroupId = knInvalidRId;
        m_nLastPageDataIndex = 0;
        m_nCurPageDataIndex = 0;

        if (pGroupDelegate.IsEmpty() || nModType != pGroupDelegate->GetType())
        {
            m_pGroupDelegate = g_DirDelegateFactory.GetGroupDelegate(nModType);
        }

        m_nModType = nModType;
    }
    else if (nModType != pGroupDelegate->GetType())
    {
        m_nLastType = pActiveDelegate->GetDirType();
        m_nLastGroupId = pActiveDelegate->GetGroupId();
        m_nLastPageDataIndex = pActiveDelegate->GetPageDataIndex();

        if (nDirType == nLastType)
        {
            m_nCurGroupId = nLastGroupId;
            m_nCurPageDataIndex = nLastPageDataIndex;
        }

        m_pGroupDelegate = g_DirDelegateFactory.GetGroupDelegate(nModType);

        m_nModType = nModType;
    }

    m_nCurType = nDirType;

    if (m_pGroupDelegate.IsEmpty())
    {
        DIRUI_ERR("Check Group Delegate fail, DirType:%d", nDirType);
        return false;
    }

    DIRUI_INFO("CheckGroupDelegate finish CurType:%d CurGroupId:%d CurDataIndex:%d LastType:%d LastGroupId:%d LastDataIndex:%d",
               m_nCurType, m_nCurGroupId, m_nCurPageDataIndex, m_nLastType, m_nLastGroupId, m_nLastPageDataIndex);

    return true;
}

void CDirectoryMgr::SetCurDirType(int nDirType)
{
    m_nCurType = nDirType;
}

bool CDirectoryMgr::CheckContactDelegate(int nUIState, int nGroupId, bool bChgMod)
{
    int nLastType = DIRECTORY_TYPE_NONE;
    int nLastUIState = DIR_UI_STATE_NONE;
    int nPageDataIndex = 0;

    DIRUI_INFO("CheckContactDelegate CurType:%d UIState:%d GroupId:%d ChgMode:%d Delegate:%p",
               m_nCurType, nUIState, nGroupId, (bChgMod ? 1 : 0), m_pActiveDelegate.Get());

    if (m_pActiveDelegate.IsEmpty())
    {
        m_pActiveDelegate = g_DirDelegateFactory.GetContactDelegate(nUIState);
    }
    else if (m_pActiveDelegate->GetState() != nUIState)
    {
        nLastUIState = m_pActiveDelegate->GetState();
        nLastType = m_pActiveDelegate->GetDirType();

        if ((DIR_UI_STATE_BROWSER == nUIState && DIR_UI_STATE_SETTING == nLastUIState)
                || (DIR_UI_STATE_SETTING == nUIState && DIR_UI_STATE_BROWSER == nLastUIState))
        {
            nPageDataIndex = m_pActiveDelegate->GetPageDataIndex();
        }

        m_pActiveDelegate->UnInit();
        m_pActiveDelegate = g_DirDelegateFactory.GetContactDelegate(nUIState);
    }
    else
    {
        nLastUIState = nUIState;

        CDirectoryControlerPtr pControler = m_pActiveDelegate->GetControler();

        if (!pControler.IsEmpty())
        {
            nLastType = pControler->GetType();
        }
    }

    if (m_pActiveDelegate.IsEmpty())
    {
        DIRUI_ERR("Check Contact Delegate fail, State:%d DirType:%d GroupId:%d", nUIState, m_nCurType,
                  nGroupId);
        return false;
    }

    if (nLastUIState != nUIState)
    {
        m_pActiveDelegate->Init(m_nCurType, nGroupId, nPageDataIndex);
    }
    else if (nLastType != m_nCurType || (DIRECTORY_TYPE_NONE == m_nCurType && bChgMod))
    {
        // 三级权限中如果history和directory都配置空，切换时也要进行初始化
        if (bChgMod)
        {
            nPageDataIndex = m_nCurPageDataIndex;
        }

        m_pActiveDelegate->Init(m_nCurType, nGroupId, nPageDataIndex);
    }
    else
    {
        CDirectoryControlerPtr pControler = m_pActiveDelegate->GetControler();

        if (pControler.IsEmpty())
        {
            return false;
        }

        m_pActiveDelegate->SetPageDataIndex(nPageDataIndex);
        pControler->SetGroupId(nGroupId);
    }

    DIRUI_INFO("Finish CheckContactDelegate LastType:%d LastUIState:%d PageDataIndex:%d",
               nLastType, nLastUIState, nPageDataIndex);

    return true;
}

int CDirectoryMgr::AdjustContactListGroupId(int nLastDirType, int nDirType, int nLastGroupId,
        int nSelGroupId)
{
    if (nLastDirType != nDirType)
    {
        return nSelGroupId;
    }

    if (DIRECTORY_TYPE_REMOTE == nDirType)
    {
        return nLastGroupId;
    }

    if (DIRECTORY_TYPE_BROADSOFT == nDirType && kszCustomId == nSelGroupId)
    {
        return nLastGroupId;
    }

    if (DIRECTORY_TYPE_LOCAL == nDirType && kszFavoriteGroupId == nSelGroupId)
    {
        return kszAllContactId;
    }

    return nSelGroupId;
}

void CDirectoryMgr::EnterDetail(int nDirType, int nContactId)
{
    CDirectoryListDelegatePtr pActiveDelegate = GetActiveDelegate();
    CDirectoryControlerPtr pControler = pActiveDelegate->GetControler();

    if (pActiveDelegate.IsEmpty())
    {
        return;
    }

    if (pControler.IsEmpty() || pControler->IsReadOnly())
    {
        DIRUI_ERR("EnterSetting() fail while isn't ediable");
        return;
    }

    int nGroupId = pActiveDelegate->GetGroupId();
    bool bEditContact = false;

    if (DIRECTORY_TYPE_LOCAL == nDirType)
    {
        bEditContact = true;
    }
    else if (DIRECTORY_TYPE_BROADSOFT == nDirType)
    {
        if (NULL != CPreciseSearchDelegate::GetDirDelegate(pActiveDelegate)
                || kszPersonalId == nGroupId || kszAllContactId == nGroupId)
        {
            bEditContact = true;
        }
    }
#if IF_FEATURE_GBBOOK
    else if (DIRECTORY_TYPE_GENBAND == nDirType)
    {
        if (nGroupId != kszGabBookGroupId)
        {
            bEditContact = true;
        }
    }
#endif
#if IF_FEATURE_METASWITCH_DIRECTORY
    else if (DIRECTORY_TYPE_METASWICTH == nDirType)
    {
        if (kszExtention != nGroupId && kszMLHG != nGroupId)
        {
            bEditContact = true;
        }
    }
#endif
    if (!CheckContactDelegate(DIR_UI_STATE_DETAIL, nGroupId, false))
    {
        return;
    }

    pActiveDelegate = GetActiveDelegate();
    CDetailDelegate * pDetailDelegate = CDetailDelegate::GetDirDelegate(pActiveDelegate);

    if (NULL == pDetailDelegate)
    {
        return;
    }

    CBaseDialog * pDlgTop = UIManager_GetTopWindow();

    if (bEditContact)
    {
        // 防止连续多次进入设置界面
        CDlgDirEdit * pDlgDirEdit = (CDlgDirEdit *)UIManager_GetPrivateDlg(DLG_CDlgDirEdit);

        if (NULL != pDlgDirEdit && pDlgDirEdit == pDlgTop)
        {
            return;
        }

        // 进入联系人界面
        if (NULL == pDlgDirEdit)
        {
            pDlgDirEdit = (CDlgDirEdit *)UIManager_CreatePrivateDlg(DLG_CDlgDirEdit);

            if (NULL == pDlgDirEdit)
            {
                return;
            }
        }

        CFrameList * pFrameList = pDlgDirEdit->GetFrameList();

        if (NULL == pFrameList)
        {
            UIManager_PutPrivateDlg(pDlgDirEdit);
            return;
        }

        pFrameList->SetDelegate(pDetailDelegate);
        pDlgDirEdit->SetContact(nDirType);

        UIManager_SetTopWindow(pDlgDirEdit);
        UIManager_UpdateWnd(pDlgDirEdit);
        return;
    }

    CDlgDirDetail * pDlgDirDetail = (CDlgDirDetail *)UIManager_GetPrivateDlg(DLG_CDlgDirDetail);

    if (NULL != pDlgDirDetail && pDlgDirDetail == pDlgTop)
    {
        return;
    }

    // 进入联系人界面
    if (NULL == pDlgDirDetail)
    {
        pDlgDirDetail = (CDlgDirDetail *)UIManager_CreatePrivateDlg(DLG_CDlgDirDetail);

        if (NULL == pDlgDirDetail)
        {
            return;
        }
    }

    CFrameList * pFrameList = pDlgDirDetail->GetFrameList();

    if (NULL == pFrameList)
    {
        UIManager_PutPrivateDlg(pDlgDirDetail);
        return;
    }

    pFrameList->SetDelegate(pDetailDelegate);
    pDlgDirDetail->SetContact(nDirType);

    UIManager_SetTopWindow(pDlgDirDetail);
    UIManager_UpdateWnd(pDlgDirDetail);
}

void CDirectoryMgr::EnterAddition(int nDirType, int nGroupId, bool bCopy,
                                  bool bIsDsskeyAdd /*= false*/)
{
    CDirectoryListDelegatePtr pActiveDelegate = GetActiveDelegate();
    int nBeforeAddGroupId = -1;
    if (!pActiveDelegate.IsEmpty())
    {
        CDirectoryControlerPtr pControler = pActiveDelegate->GetControler();

        if (pControler.IsEmpty() || pControler->IsReadOnly())
        {
            DIRUI_ERR("EnterAddition() fail while isn't ediable");
            return;
        }
        nBeforeAddGroupId = pControler->GetGroupId();
        m_pActiveDelegate.Reset();
    }

    // 防止连续多次进入联系人界面
    CDlgDirNew * pDlgDirNew = (CDlgDirNew *)UIManager_GetPrivateDlg(DLG_CDlgDirNew);
    CBaseDialog * pDlgTop = UIManager_GetTopWindow();

    if (NULL != pDlgDirNew && pDlgDirNew == pDlgTop)
    {
        return;
    }

    // 进入联系人界面
    if (NULL == pDlgDirNew)
    {
        pDlgDirNew = (CDlgDirNew *)UIManager_CreatePrivateDlg(DLG_CDlgDirNew);

        if (NULL == pDlgDirNew)
        {
            return;
        }
    }

    if (!bCopy)
    {
        LIST_ATTRIBUTE lsAttr;

        CDirUIHelper::SetAttributeList(DIRECTORY_TYPE_NONE, lsAttr);
    }


    // 联系人添加界面没有用CFrameList
    pDlgDirNew->Init(bCopy, nDirType, nGroupId, bIsDsskeyAdd);
    pDlgDirNew->SetBeforeGroupId(nBeforeAddGroupId);

    // 显示界面
    UIManager_SetTopWindow(pDlgDirNew);
    UIManager_UpdateWnd(pDlgDirNew);
}

void CDirectoryMgr::EnterRingSetting(int nId)
{
    yl::string strRingPath = "";
    CGroupDelegatePtr pGroupDelegate = GetGroupDelegate();
    CContactGroupDelegate * pContactGroupDelegate = CContactGroupDelegate::GetGroupDelegate(
                pGroupDelegate);

    if (NULL == pContactGroupDelegate)
    {
        return;
    }

    pContactGroupDelegate->GetLocalGroupRing(nId, strRingPath);

    CRingSelector * pRingSelector = GetRingSelector();

    if (NULL == pRingSelector)
    {
        return;
    }

    pRingSelector->Init(nId, strRingPath);

    MsgBoxSoftkey skCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
    MsgBoxSoftkey skOk(LANG_TRANSLATE(TRID_OK), MessageBox_OK);
    VecMsgBoxSoftkey vecSoftKey;

    vecSoftKey.push_back(skCancel);
    vecSoftKey.push_back(skOk);

    QString strTitle = LANG_TRANSLATE(TRID_RING_TONES);
    CMessageBoxBase * pSelectBox = MessageBox_ShowSelectBox(this, vecSoftKey,
                                   SELECT_POP_BOX_TYPE_NORMAL,
                                   pRingSelector, strTitle, DIR_UI_ACTION_RING_SETTING);

    if (NULL != pSelectBox)
    {
        pSelectBox->SetEventFilter(pRingSelector);
    }
}

void CDirectoryMgr::OnRingSetting()
{
    CGroupDelegatePtr pGroupDelegate = GetGroupDelegate();
    CContactGroupDelegate * pContactGroupDelegate = CContactGroupDelegate::GetGroupDelegate(
                pGroupDelegate);

    if (NULL == pContactGroupDelegate)
    {
        return;
    }

    CRingSelector * pRingSelector = GetRingSelector();

    if (NULL == pRingSelector)
    {
        return;
    }

    yl::string strRingPath;

    if (!pRingSelector->GetRingPath(strRingPath))
    {
        return;
    }

    int nGroupId = pRingSelector->GetId();
    DIRECTORY_RET_CODE retCode = (DIRECTORY_RET_CODE)pContactGroupDelegate->UpdateLocalGroupRing(
                                     nGroupId, strRingPath);

    if (DIR_RET_SUCC == retCode)
    {
        CDirUIHelper::ShowMsgBox(TRID_SAVING_RING);
    }
    else
    {
        CDirUIHelper::ShowErrorMsg(retCode);
    }
}

void CDirectoryMgr::RetryChgPwdRuest(int nDirType, const yl::string & strPwd)
{
    if (strPwd.empty())
    {
        CDirUIHelper::ShowMsgBox(TRID_PASSWORD_CANNOT_BE_EMPTY);

        return;
    }

    _BsftPwdMgr.SetPassword(strPwd);

    CDirectoryControlerPtr pControler = GetActiveControler();

    if (DIRECTORY_TYPE_BROADSOFT == nDirType)
    {
        CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

        if (NULL != pBSFTContact)
        {
            pBSFTContact->OnAction(DIR_ACTION_BSFT_CHG_PWD, -1, knInvalidRId);
        }
    }
    else if (CALL_LOG_TYPE_BROADSOFT == nDirType)
    {
        CBSFTCallLogControler * pBSFTCallLog = CBSFTCallLogControler::GetControler(pControler);

        if (NULL != pBSFTCallLog)
        {
#if IF_FEATURE_MUTI_CALLLOG
            pBSFTCallLog->OnAction(DIR_ACTION_BSFT_CHG_PWD, -1, pBSFTCallLog->GetGroupId());
#else
            pBSFTCallLog->OnAction(DIR_ACTION_BSFT_CHG_PWD, -1, knInvalidRId);
#endif
        }
    }
}

int CDirectoryMgr::GetModType(int nDirType) const
{
    if (nDirType > DIRECTORY_TYPE_NONE && nDirType < DIRECTORY_TYPE_MAX)
    {
        return DIR_MOD_TYPE_CONTACT;
    }

    if (CALL_LOG_TYPE_LOCAL == nDirType || CALL_LOG_TYPE_BROADSOFT == nDirType
#if IF_FEATURE_METASWITCH_CALLLOG
            || CALL_LOG_TYPE_MTSW == nDirType
#endif
       )
    {
        return DIR_MOD_TYPE_CALLLOG;
    }

    return DIR_MOD_TYPE_NONE;
}

void CDirectoryMgr::ClearDirectoryLogicData()
{
    DIRUI_INFO("CDirectoryMgr::ClearDirectoryLogicData()");

#if IF_FEATURE_GBBOOK
    if (m_nCurType == DIRECTORY_TYPE_GENBAND)
    {
        Dir_ClearContactData(kszGabBookGroupId, GB_DIR);
        if (configParser_GetConfigInt(kszGbRetainSearchFilter) != 1)
        {
            GAB_ClearSearchInfo();
        }
    }
#endif
    if (DIRECTORY_TYPE_BROADSOFT == m_nCurType)
    {
        if (!BsftDir_UpdateEnable())
        {
            ContactListData pData;
            Dir_GetContactListByGroupId(&pData, kszRootId, BSFT_DIR);
            for (int i = 0; i < pData.m_listId.size(); ++i)
            {
                int nId = pData.m_listId[i];
                BsftDir_ClearContactInfo(nId);
            }
            BsftDir_ClearContactInfo(kszServerSearchGroupId);
        }
    }
}

#if DIR_SUPPORT_NET_CALLLOG_DELETE
void CDirectoryMgr::OnClearGroupClick()
{
    DIRUI_INFO("OnClearGroupClick");
    if (configParser_GetConfigInt(kszBsftLogDeleteEnable) == 0)
    {
        DIRUI_INFO("Delete not Enable!!");
        return;
    }

    CDirectoryListDelegatePtr pActiveDelegate = GetActiveDelegate();

    if (pActiveDelegate.IsEmpty())
    {
        DIRUI_INFO("CDirectoryMgr::OnClearGroupClick get ActiveDelegate fail");
        return;
    }

    CBrowserDelegate * pBrowserDelegate = CBrowserDelegate::GetDirDelegate(pActiveDelegate);

    if (NULL == pBrowserDelegate)
    {
        DIRUI_INFO("CDirectoryMgr::OnClearGroupClick get BrowserDelegate fail");
        return;
    }

    CFrameList * pFrameList = (CFrameList *)pBrowserDelegate->GetFrameList();

    if (NULL == pFrameList)
    {
        DIRUI_INFO("CDirectoryMgr::OnClearGroupClick get FrameList fail");
        return;
    }

    CGroupDelegatePtr pActiveGroupDelegate = GetGroupDelegate();

    if (pActiveGroupDelegate.IsEmpty())
    {
        DIRUI_INFO("CDirectoryMgr::OnClearGroupClick get ActiveGroupDelegate fail");
        return;
    }

    CGroupDelegate * pGroupDelegate = pActiveGroupDelegate.Get();

    int nDirType = pGroupDelegate->GetSelectGroupType();
    int nGroupId = pGroupDelegate->GetSelectGroupId();

    CDirectoryControlerPtr pControler = g_DirCtrlFactory.FindControler(nDirType);
    if (!pControler.IsEmpty())
    {
        nGroupId = pControler->GetGroupId();
    }

    if (1 == configParser_GetConfigInt(kszMutilBroadSoftLog))
    {
        int nAvailableNum = acc_GetAvailableAccountCount();
        if (nAvailableNum == 1)
        {
            nGroupId = BsftLog_GetGroupIDByAccountID(acc_GetUsableAccount(), CT_CALLALL);
        }
    }

    DIRUI_INFO("CDirectoryMgr::OnClearGroupClick get Group ID %d, %d",
               pGroupDelegate->GetSelectGroupId(), nGroupId);

    if (nGroupId < 0)
    {
        CDirUIHelper::ShowMsgBox(TRID_PLEASE_SELECT_GROUP);
    }
    else if (pFrameList->GetTotalDataCount() < 1)
    {
        CDirUIHelper::ShowMsgBox(TRID_NO_RECORD);
    }
    else
    {
        CALL_TYPE eCallType = BsftLog_GetCallTypeByGroupID(nGroupId);

        QString strHint = LANG_TRANSLATE(TRID_DO_YOU_WANT_TO_DELETE_ALL_CALL_LOGS);
        switch (eCallType)
        {
        case CT_OUTGOING:
            strHint = LANG_TRANSLATE(TRID_DO_YOU_WANT_TO_DELETE_ALL_PLACED_CALL_LOGS);
            break;
        case CT_INCOMING:
            strHint = LANG_TRANSLATE(TRID_DO_YOU_WANT_TO_DELETE_ALL_RECEIVED_CALL_LOGS);
            break;
        case CT_MISSCALL:
            strHint = LANG_TRANSLATE(TRID_DO_YOU_WANT_TO_DELETE_ALL_MISSED_CALL_LOGS);
            break;
        case CT_CALLALL:
        default:
            break;
        }

        strHint += "\n";
        strHint += LANG_TRANSLATE(TRID_THIS_ACTION_CAN_NOT_BE_REVERSED);

        CDirUIHelper::PopSureMsgBox(TRID_WARNING, strHint.toUtf8().data(),
                                    DIR_ACTION_BSFT_CLEAR_GROUP, nGroupId);
    }
}
#endif
