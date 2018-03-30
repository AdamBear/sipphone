#include <QtGui>
#include "baseui/framelist.h"
#include "diruihelper.h"
#include "commondirctrl.h"
#include "directorycommon.h"
#include "directorylistaction.h"
#include "translateiddefine.h"
#include "directorymgr.h"
#include "dlgdirdetail.h"
#include "dlgdiredit.h"
#include "dlgdirnew.h"
#include "dlgdirsearch.h"
#include "bsftcontactcontroler.h"
#include "localcontactcontroler.h"
#include "remotecontactcontroler.h"
#include "settingdelegate.h"
#include "contactlisttitle.h"
#ifdef DIR_SUPPORT_BATCH_OPERATE
#include "groupselector.h"
#endif
#include "numberselector.h"
#include "transfernumselector.h"
#include "account/include/modaccount.h"
#include "configparser/modconfigparser.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "baseui/contactbaseitem.h"
#include "baseui/detaillistitem.h"
#include "baseui/framelistaction.h"
#include "baseui/framesearchtitle.h"
#include "messagebox/modmessagebox.h"
#include "messagebox/selectmessagebox.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "talk/emergency/include/modemergency.h"
#include "talk/digitmap/include/moddigitmap.h"
#include "baseui/t4xpicpath.h"
#include "inputmethod/inputmethod.h"

namespace
{
#define SHORT_TIME 1500
}

CDirUIHelper::MAP_MSG_BOX_ACTION_ID CDirUIHelper::s_mapActionId;
LIST_ATTRIBUTE CDirUIHelper::s_listAtrr;
bool CDirUIHelper::s_bShowWaiting = false;
int CDirUIHelper::s_nDialAccountId = AUTO_ACCOUNT_IDENTIFY;
int CDirUIHelper::s_nAttrType = DIRECTORY_TYPE_NONE;
#ifdef DIR_SUPPORT_BATCH_OPERATE
int CDirUIHelper::s_nSelectBoxAction = DIR_ACTION_NONE;
#endif


CDirUIHelper::CDirUIHelper(int nType)
    : m_pFrameList(NULL)
    , m_pFocusInvalidFrameList(NULL)
    , m_nType(nType)
{

}

CDirUIHelper::~CDirUIHelper()
{
    DIRUI_INFO("~CDirUIHelper %p", this);
    UnInit();
}

void CDirUIHelper::Init()
{
    m_pFrameList = NULL;
    m_pFocusInvalidFrameList = NULL;
}

void CDirUIHelper::UnInit()
{
    m_pFrameList = NULL;
    m_pFocusInvalidFrameList = NULL;
}

int CDirUIHelper::GetType() const
{
    return m_nType;
}

void CDirUIHelper::ExitCurrentUI(int nUIState)
{
    if (DIR_UI_STATE_CHG_PWD == nUIState)
    {
        return;
    }

    CBaseDialog * pDlgTop = UIManager_GetTopWindow();

    if (NULL != pDlgTop)
    {
        UIManager_PutPrivateDlg(pDlgTop);
    }

    if (DIR_UI_STATE_DETAIL != nUIState)
    {
        return;
    }

    RefreshUI(true, true);
}

void CDirUIHelper::ExitAllUI(int nDirType)
{
    DIRUI_INFO("ExitAllUI(%d)", nDirType);

    CDlgDirNew * pDlgDirNew = (CDlgDirNew *)UIManager_GetPrivateDlg(DLG_CDlgDirNew);
    CBaseDialog * pDlgTop = UIManager_GetTopWindow();

    if (NULL != pDlgDirNew && pDlgTop == pDlgDirNew
            && (DIRECTORY_TYPE_LOCAL == nDirType || DIRECTORY_TYPE_BROADSOFT == nDirType
#if IF_FEATURE_GBBOOK
                || DIRECTORY_TYPE_GENBAND == nDirType
#endif
               )
       )
    {
        UIManager_PutPrivateDlg(pDlgDirNew);
    }

    CDlgDirEdit * pDlgDirEdit = (CDlgDirEdit *)UIManager_GetPrivateDlg(DLG_CDlgDirEdit);
    pDlgTop = UIManager_GetTopWindow();

    if (NULL != pDlgDirEdit && pDlgDirEdit == pDlgTop && pDlgDirEdit->GetDirType() == nDirType)
    {
        UIManager_PutPrivateDlg(pDlgTop);
    }
    else
    {
        CDlgDirDetail * pDlgDirDetail = (CDlgDirDetail *)UIManager_GetPrivateDlg(DLG_CDlgDirDetail);

        if (NULL != pDlgDirDetail && pDlgDirDetail == pDlgTop && pDlgDirDetail->GetDirType() == nDirType)
        {
            UIManager_PutPrivateDlg(pDlgTop);
        }
    }

    CDlgDirSearch * pDlgSearch = (CDlgDirSearch *)UIManager_GetPrivateDlg(DLG_CDlgDirSearch);
    pDlgTop = UIManager_GetTopWindow();

    if (NULL != pDlgSearch && pDlgSearch == pDlgTop)
    {
        int nSearchType = pDlgSearch->GetSearchType();

        if ((DIRECTORY_SEARCH_TYPE_NORMAL == nSearchType && pDlgSearch->GetContactType() == nDirType)
                || (DIRECTORY_SEARCH_TYPE_LDAP == nSearchType && DIRECTORY_TYPE_LDAP == nDirType)
                || (DIRECTORY_SEARCH_TYPE_BROADSOFT == nSearchType && DIRECTORY_TYPE_BROADSOFT == nDirType)
#if IF_FEATURE_GBBOOK
                || (DIRECTORY_SEARCH_TYPE_GENBAND == nSearchType && DIRECTORY_TYPE_GENBAND == nDirType)
#endif
           )
        {
            UIManager_PutPrivateDlg(pDlgTop);
        }
    }
}

void CDirUIHelper::ReturnRootGroup(int nDirType)
{
    if (DIRECTORY_TYPE_BROADSOFT == nDirType)
    {
        CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
        CBSFTContactControler * pBSFTContact = CBSFTContactControler::GetControler(pControler);

        if (NULL == pBSFTContact || (!pBSFTContact->IsBrowserState() && !pBSFTContact->IsSettingState()))
        {
            DIRUI_ERR("ReturnRootGroup fail BSFTContact:%p", pBSFTContact);
            return;
        }

        DIRUI_INFO("ReturnRootGroup(%d) GroupId:%d", nDirType, pBSFTContact->GetGroupId());

        if (pBSFTContact->GetGroupId() > kszCustomId)
        {
            pBSFTContact->SetGroupId(kszCustomId);
        }

        RefreshUI(true, true);
    }
}

void CDirUIHelper::RefreshUIAfterDeleted(bool bBackUI/* = false*/)
{
    CDirectoryControlerPtr pActiveControler = g_DirectoryMgr.GetActiveControler();

    if (!pActiveControler.IsEmpty() && pActiveControler->IsDetailState())
    {
        CDirUIHelper::ExitCurrentUI(DIR_UI_STATE_DETAIL);
    }

    RefreshUI(true, false);
}

void CDirUIHelper::RefreshUI(bool bDataChange, bool bResetFocus)
{
    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();

    if (pDelegate.IsEmpty())
    {
        return;
    }

    if (bDataChange)
    {
        pDelegate->ResetIndex();

        CSettingDelegate * pSettingDelegate = CSettingDelegate::GetDirDelegate(pDelegate);

        if (NULL != pSettingDelegate)
        {
            pSettingDelegate->ResetSelectCount();
        }
    }

    pDelegate->RefreshFrameList();
}

void CDirUIHelper::ShowMsgBox(const char * pszMsg, const char * pszExtra/* = NULL*/,
                              int nAction/* = DIR_ACTION_NONE*/)
{
    if (NULL == pszMsg)
    {
        DIRUI_ERR("ShowMsgBox pszMsg is NULL");
        return;
    }

    QString strMsg = LANG_TRANSLATE(pszMsg);

    if (NULL != pszExtra)
    {
        strMsg += LANG_TRANSLATE(pszExtra);
    }

    MessageBox_ShowCommonMsgBox(&g_DirectoryMgr, strMsg, MESSAGEBOX_NOTE, SHORT_TIME, "", "", nAction);
}

#if IF_FEATURE_GBBOOK
void CDirUIHelper::ShowMsgBox(const char * pszMsg, int nTime, const char * pszExtra/* = NULL*/,
                              int nAction/* = DIR_ACTION_NONE*/)
{
    if (NULL == pszMsg)
    {
        DIRUI_ERR("ShowMsgBox pszMsg is NULL");
        return;
    }

    QString strMsg = LANG_TRANSLATE(pszMsg);

    if (NULL != pszExtra)
    {
        strMsg += LANG_TRANSLATE(pszExtra);
    }

    MessageBox_ShowCommonMsgBox(&g_DirectoryMgr,
                                strMsg, MESSAGEBOX_NOTE, nTime, "", "", nAction);
}
#endif

bool CDirUIHelper::PopSureMsgBox(const char * pszTitle, const char * pszMsg, int nAction, int nId)
{
    if (NULL == pszTitle || NULL == pszMsg)
    {
        DIRUI_ERR("PopSureMsgBox pszTitle:%s pszMsg:%s", (NULL == pszTitle ? "NULL" : pszTitle),
                  (NULL == pszMsg ? "NULL" : pszMsg));
        return false;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

    if (pControler.IsEmpty())
    {
        return false;
    }

    SetMsgBoxActionId(nAction, nId);

    MessageBox_ShowCommonMsgBox(&g_DirectoryMgr, LANG_TRANSLATE(pszMsg), MESSAGEBOX_OkCancel, -1, "",
                                "", nAction);

    return true;
}

bool CDirUIHelper::PopSureMsgBox(const char * pszTitle, const char * pszMsg, int nAction, int nId,
                                 const char * pExtTitle,
                                 const char * pszExtra/* = NULL*/, bool bExistEdit/* = false*/)
{
    if (NULL == pszTitle || NULL == pszMsg)
    {
        DIRUI_ERR("PopSureMsgBox pszTitle:%s pszMsg:%s", (NULL == pszTitle ? "NULL" : pszTitle),
                  (NULL == pszMsg ? "NULL" : pszMsg));
        return false;
    }

    QString strTitle = LANG_TRANSLATE(pszTitle);
    if (pExtTitle != NULL)
    {
        strTitle += " ";
        strTitle += pExtTitle;
    }

    QString strMsg = LANG_TRANSLATE(pszMsg);
    if (pszExtra != NULL)
    {
        strMsg += " ";
        strMsg += pszExtra;
    }


    SetMsgBoxActionId(nAction, nId);

    MessageBox_ShowCommonMsgBox(&g_DirectoryMgr,
                                strMsg, MESSAGEBOX_OkCancel, -1, "", "", nAction);

    return true;
}


#ifdef DIR_SUPPORT_BATCH_OPERATE
int CDirUIHelper::PopSelectBoxAction()
{
    int nAction = s_nSelectBoxAction;

    s_nSelectBoxAction = DIR_ACTION_NONE;

    return nAction;
}

bool CDirUIHelper::EnterSelectGroup(int nDirType, int nExcludeGroupId, bool bRootInclude,
                                    int nAction)
{
    CGroupSelector * pGroupSelector = g_DirectoryMgr.GetGroupSelector();

    if (NULL == pGroupSelector)
    {
        return false;
    }

    pGroupSelector->Init(nDirType, nExcludeGroupId, bRootInclude);

    if (pGroupSelector->GetGroupAmount() <= 0)
    {
        return false;
    }

    s_nSelectBoxAction = nAction;

    MsgBoxSoftkey skCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
    MsgBoxSoftkey skOk(LANG_TRANSLATE(TRID_OK), MessageBox_OK);
    VecMsgBoxSoftkey vecSoftKey;

    vecSoftKey.push_back(skCancel);
    vecSoftKey.push_back(skOk);

    QString strTitle = LANG_TRANSLATE(TRID_CHOOSE_A_GROUP);

    MessageBox_ShowSelectBox(&g_DirectoryMgr, vecSoftKey, SELECT_POP_BOX_TYPE_NORMAL,
                             pGroupSelector, strTitle, DIR_UI_ACTION_COMMON_SELECT_BOX);

    return true;
}

bool CDirUIHelper::GetSelectContactId(VEC_CONTACT_ID & vecContactId)
{
    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    CSettingDelegate * pSettingDelegate = CSettingDelegate::GetDirDelegate(pDelegate);

    if (NULL == pSettingDelegate)
    {
        return false;
    }

    return pSettingDelegate->GetSelectContactId(vecContactId);
}

bool CDirUIHelper::IsSelectAllContact()
{
    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    CSettingDelegate * pSettingDelegate = CSettingDelegate::GetDirDelegate(pDelegate);

    if (NULL == pSettingDelegate)
    {
        return false;
    }

    return pSettingDelegate->IsSelectAll();
}
#endif

void CDirUIHelper::ShowWaiting(const char * pszTitle, const char * pszMsg, int nAction)
{
    if (NULL == pszTitle || NULL == pszMsg)
    {
        DIRUI_ERR("ShowWaiting pszTitle:%s pszMsg:%s", (NULL == pszTitle ? "NULL" : pszTitle),
                  (NULL == pszMsg ? "NULL" : pszMsg));
        return;
    }

    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();

    if (pControler.IsEmpty())
    {
        return;
    }

    if (s_bShowWaiting)
    {
        return;
    }

    QString strWaiting = LANG_TRANSLATE(pszMsg);

    s_bShowWaiting = true;

#if IF_FEATURE_METASWITCH_DIRECTORY
    if (nAction == DIR_ACTION_MTSW_ADD_CONTACTLIST_FINISH)
    {
        MessageBox_ShowCommonMsgBox(&g_DirectoryMgr,
                                    LANG_TRANSLATE(pszMsg), MESSAGEBOX_NOTE, -1, "", "", nAction);
    }
    else
#endif
    {
        MessageBox_ShowCommonMsgBox(&g_DirectoryMgr,
                                    LANG_TRANSLATE(pszMsg), MESSAGEBOX_Cancel, -1, "", "", nAction);

    }
}

void CDirUIHelper::EndWaiting()
{
    MessageBox_RemoveMessageBox(&g_DirectoryMgr, MessageBox_Destory);
}

void CDirUIHelper::OnCancelWaiting()
{
    s_bShowWaiting = false;
}

void CDirUIHelper::ShowErrorMsg(int nError)
{
    yl::string strErrMsg;

    CCommonDirCtrl::GetErrMsg(nError, strErrMsg);

    if (!strErrMsg.empty())
    {
        ShowMsgBox(strErrMsg.c_str());
    }
}

void CDirUIHelper::SetMsgBoxActionId(int nAction, int nId)
{
    s_mapActionId[nAction] = nId;
}

int CDirUIHelper::PopMsgBoxActionId(int nAction)
{
    MAP_MSG_BOX_ACTION_ID::iterator iter = s_mapActionId.find(nAction);

    if (iter != s_mapActionId.end())
    {
        int nId = iter->second;

        s_mapActionId.erase(iter);

        return nId;
    }

    return knInvalidRId;
}

void CDirUIHelper::EnterUI(int nDirType, int nUIState, int nAction, int nId)
{
    DIRUI_INFO("EnterUI(DirType:%d State:%d Action:%d Id:%d)", nDirType, nUIState, nAction, nId);

    if (DIR_UI_STATE_BROWSER == nUIState)
    {
        g_DirectoryMgr.OnGroupClick(nDirType, nId);
        return;
    }
    else if (DIR_UI_STATE_SEARCH == nUIState)
    {
        g_DirectoryMgr.EnterSearch(nDirType, nId, PHONE_KEY_NONE);
        return;
    }
    else if (DIR_UI_STATE_DETAIL == nUIState)
    {
        g_DirectoryMgr.EnterDetail(nDirType, nId);
        return;
    }
    else if (DIR_UI_STATE_ADDITION == nUIState)
    {
        nId = kszAllContactId;

        if (DIR_ACTION_LOCAL_ENTER_COPY_TO_BLACKLIST == nAction)
        {
            nId = kszBlackListGroupId;
        }

        g_DirectoryMgr.EnterAddition(nDirType, nId, true);
        return;
    }
    else if (DIR_UI_STATE_CHG_PWD == nUIState)
    {
#if IF_FEATURE_METASWITCH_DIRECTORY
        if (nDirType == DIRECTORY_TYPE_METASWICTH
                || nDirType == CALL_LOG_TYPE_MTSW)
        {
            MessageBox_ShowMTSWCommportalBox(&g_DirectoryMgr, nDirType);
            return;
        }
#endif
        QString strUserIDTitle;
        QString strPwdTitle;
        yl::string strUserID;


        int nAccountID = 0;
#if IF_FEATURE_MUTI_CALLLOG
        if (BsftLog_IsMutilLogEnable() && nDirType == CALL_LOG_TYPE_BROADSOFT)
        {
            nAccountID = BsftLog_GetAccountIDByGroupID(nId);
        }
#endif
        if (IsBsftAllowSipAuth())
        {
            strUserIDTitle = LANG_TRANSLATE(TRID_SIP_REGISTER_NAME);
            strPwdTitle = LANG_TRANSLATE(TRID_SIP_PASSWORD);
            strUserID = configParser_GetCfgItemStringValue(kszAccountRegisterName, nAccountID);
        }
        else
        {
            strUserIDTitle = LANG_TRANSLATE(TRID_USER_ID);
            strPwdTitle = LANG_TRANSLATE(TRID_PASSWORD);
            strUserID = configParser_GetCfgItemStringValue(kszAccountXSIUserID, nAccountID);
        }

        QString strUtf8UserID = toQString(strUserID);

        MessageBox_ShowXSIPasswordBox(&g_DirectoryMgr, strUserIDTitle, strUtf8UserID, strPwdTitle,
                                      nDirType);
        return;
    }
#if IF_FEATURE_34162
    else if (DIR_UI_STATE_LDAP_CHG_PWD == nUIState)
    {
        QString strUserIDTitle;
        QString strPwdTitle;
        yl::string strUserID;

        if (IsBsftAllowSipAuth())
        {
            strUserIDTitle = LANG_TRANSLATE(TRID_SIP_REGISTER_NAME);
            strPwdTitle = LANG_TRANSLATE(TRID_SIP_PASSWORD);
            strUserID = configParser_GetCfgItemStringValue(kszAccountRegisterName, 0);
        }
        else
        {
            strUserIDTitle = LANG_TRANSLATE(TRID_USER_ID);
            strPwdTitle = LANG_TRANSLATE(TRID_PASSWORD);
            strUserID = configParser_GetCfgItemStringValue(kszAccountXSIUserID, 0);
        }

        QString strUtf8UserID = toQString(strUserID);
        MessageBox_ShowLdapPasswordBox(&g_DirectoryMgr, strUserIDTitle, strUtf8UserID, nDirType);
        return;
    }
#endif

    DIRUI_ERR("EnterUI(DirType:%d State:%d Action:%d Id:%d) fail", nDirType, nUIState, nAction, nId);
}

bool CDirUIHelper::Dial(int nAccountId, const yl::string & strName, LIST_ATTRIBUTE & lsAttr,
                        CDirectoryControler * pContrl, BaseDirDataRecord * pRecord)
{
    int nSize = lsAttr.size();

    SESSION_STATE state = talklogic_GetSessionStateBySessionID(talklogic_GetFocusedSessionID());

    if (0 == nSize)
    {
        ShowMsgBox(TRID_PHONE_NUMBER_CANNOT_BE_EMPTY);
        return false;
    }
    else if (1 == nSize && SESSION_PRETRAN != state)
    {
        ST_CONTACT_ATTRIBUTE & stAttr = lsAttr.front();

        Dial(nAccountId, strName, stAttr.m_strValue, pContrl, pRecord);
        return true;
    }

    s_nDialAccountId = nAccountId;

    SetAttributeList(g_DirectoryMgr.GetActiveDirType(), lsAttr);

	qSetHideKeyboard();

    if (SESSION_PRETRAN == state)
    {
        CTransferNumSelector * pNumberSelector = g_DirectoryMgr.GetTransferNumSelector();

        if (NULL == pNumberSelector)
        {
            return false;
        }

        pNumberSelector->Init(TRAN_NUM_DIR);
        pNumberSelector->SetName(strName);

        MsgBoxSoftkey skCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
        MsgBoxSoftkey skSend(LANG_TRANSLATE(TRID_SEND), MessageBox_Send);
        MsgBoxSoftkey skBTransfer(LANG_TRANSLATE(TRID_BLIND_TRANSFER), MessageBox_BTransfer);

        VecMsgBoxSoftkey vecSoftKey;

        vecSoftKey.push_back(skBTransfer);
        vecSoftKey.push_back(skSend);
        vecSoftKey.push_back(skCancel);

        QString strTitle = toQString(strName, true);


        CMessageBoxBase * pSelectBox = MessageBox_ShowSelectBox(&g_DirectoryMgr,
                                       vecSoftKey,
                                       SELECT_POP_BOX_TYPE_NORMAL,
                                       pNumberSelector,
                                       strTitle,
                                       DIR_UI_ACTION_TRANSFER_NUMBER_CHOOSE);

        if (NULL != pSelectBox)
        {
            pSelectBox->SetEventFilter(pNumberSelector);
        }

    }
    else
    {
        CNumberSelector * pNumberSelector = g_DirectoryMgr.GetNumberSelector();

        if (NULL == pNumberSelector)
        {
            return false;
        }

        pNumberSelector->SetName(strName);

        MsgBoxSoftkey skCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
        VecMsgBoxSoftkey vecSoftKey;

        vecSoftKey.push_back(skCancel);
#if IF_FEATURE_GBBOOK
        yl::string strTemp = strName;
        if (g_DirectoryMgr.GetActiveDirType() == DIRECTORY_TYPE_GENBAND)
        {
            MsgBoxSoftkey skAccount(LANG_TRANSLATE(TRID_ACCOUNT), MessageBox_Account);
            vecSoftKey.push_back(skAccount);

            int iAccountId = nAccountId;
            if (nAccountId == -1)
            {
                iAccountId = acc_GetDefaultAccount();
            }
            strTemp = "Account:" + acc_GetUsername(iAccountId);
        }

        QString strTitle = toQString(strTemp, true);
#else
        QString strTitle = toQString(strName, true);
#endif

        MessageBox_ShowSelectBox(&g_DirectoryMgr, vecSoftKey, SELECT_POP_BOX_TYPE_TINY, pNumberSelector,
                                 strTitle, DIR_UI_ACTION_NUMBER_CHOOSE);

    }
    return true;
}

bool CDirUIHelper::Dial(int nAccountId, const yl::string & strName, const yl::string & strNumber,
                        CDirectoryControler * pContrl, BaseDirDataRecord * pRecord)
{
    if (strNumber.empty())
    {
        ShowMsgBox(TRID_PHONE_NUMBER_CANNOT_BE_EMPTY);
        return false;
    }

#ifdef IF_BT_SUPPORT_PHONE
    if (talklogic_IsFoucsInBluetoothDialSession())
    {
        DIRUI_INFO("CDirUIHelper::Dial, Mobile CallOut Number=[%s] Name=[%s]", strNumber.c_str(),
                   strName.c_str());

        talklogic_CallMobileOut(strNumber, strName);

        ResetDialAccountId();

        return true;
    }
#endif

#if DIR_SUPPORT_SELECT_CONTACT
    if (g_DirectoryMgr.GetIsInSelecting())
    {
        g_DirectoryMgr.OnSelectContact(strName, strNumber);
        return true;
    }
#endif

    // 如果是-1,则使用联系人配置的账号
    if (-1 != nAccountId && !acc_IsAccountAvailable(nAccountId)
            && !Emergency_IsEmergencyMode(strNumber.c_str()))
    {
        // 如果没有账号，则取当前账号
        nAccountId = acc_GetDefaultAccount();
        // 如果默认账号不存在，则尝试IP直拨账号
        if (configParser_GetConfigInt(kszEnableDirectIPCall) != 0 && -1 == nAccountId)
        {
            nAccountId = acc_GetDirectIPAccountID();
        }

        if (!acc_IsAccountAvailable(nAccountId))
        {
            ShowMsgBox(TRID_INVALID_ACCOUNT);
            return false;
        }
    }
    else if (AUTO_SELECT_LINE_ID == nAccountId)
    {
        nAccountId = acc_GetDefaultAccount();
    }

    ResetDialAccountId();

    bool bExeMatch = false;
    if (pContrl != NULL)
    {
        switch (pContrl->GetType())
        {
        case CALL_LOG_TYPE_LOCAL:
        case CALL_LOG_TYPE_BROADSOFT:
            bExeMatch = (pRecord != NULL
                         && pRecord->IsMutable()) ? DigitMap_IsConfigEnable(kszDigitMapInHistoryDial, nAccountId) : false;
            break;
        case DIRECTORY_TYPE_LOCAL:
        case DIRECTORY_TYPE_REMOTE:
        case DIRECTORY_TYPE_LDAP:
        case DIRECTORY_TYPE_BROADSOFT:
        case DIRECTORY_TYPE_BTUC:
            bExeMatch = DigitMap_IsConfigEnable(kszDigitMapInDirDial, nAccountId);
            break;
        default:
            bExeMatch = true;
            break;
        }

        DIRUI_INFO("CDirUIHelper::Dial type=[%d] acc=[%d] match=[%d]", pContrl->GetType(), nAccountId,
                   bExeMatch);
    }

    CallOut_Param tCalloutParam;
    if (!bExeMatch)
    {
        tCalloutParam.tDialPlanMask.RemoveDigitMapFlag();
    }

    // 判断转移类型
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    if (!pControler.IsEmpty())
    {
        tCalloutParam.eTransferType = pControler->IsBlindTransfer() ? DTT_BLIND_TRANSFER : DTT_CALL_OUT;
        pControler->SetBlindTransfer(false);
    }

    if (g_DirectoryMgr.GetIsEditCallLog())
    {
        talklogic_EnterDialUI(strNumber, nAccountId);
    }
    else
    {
        talklogic_CallOut(strNumber, strName, nAccountId, &tCalloutParam, true);
    }

    return true;
}

bool CDirUIHelper::DialCalllog(int nAccountId, LIST_ATTRIBUTE & lsAttr,
                               CDirectoryControler * pContrl, BaseDirDataRecord * pRecord)
{
    int nSize = lsAttr.size();

    SESSION_STATE state = talklogic_GetSessionStateBySessionID(talklogic_GetFocusedSessionID());

    if (0 == nSize)
    {
        ShowMsgBox(TRID_PHONE_NUMBER_CANNOT_BE_EMPTY);
        return false;
    }
    else if (1 == nSize)
    {
        ST_CONTACT_ATTRIBUTE & stAttr = lsAttr.front();

        DialCalllog(nAccountId, stAttr.m_strValue, pContrl, pRecord);
        return true;
    }

    s_nDialAccountId = nAccountId;

    SetAttributeList(g_DirectoryMgr.GetActiveDirType(), lsAttr);

    if (SESSION_PRETRAN == state)
    {
        CTransferNumSelector * pNumberSelector = g_DirectoryMgr.GetTransferNumSelector();

        if (NULL == pNumberSelector)
        {
            return false;
        }

        pNumberSelector->Init(TRAN_NUM_DIR);

        pNumberSelector->SetName(pRecord->GetDisplayName());

        MsgBoxSoftkey skCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
        MsgBoxSoftkey skSend(LANG_TRANSLATE(TRID_SEND), MessageBox_Send);
        MsgBoxSoftkey skBTransfer(LANG_TRANSLATE(TRID_BLIND_TRANSFER), MessageBox_BTransfer);

        VecMsgBoxSoftkey vecSoftKey;

        vecSoftKey.push_back(skBTransfer);
        vecSoftKey.push_back(skSend);
        vecSoftKey.push_back(skCancel);

        QString strTitle = toQString(pRecord->GetDisplayName(), true);
        CMessageBoxBase * pSelectBox = MessageBox_ShowSelectBox(&g_DirectoryMgr,
                                       vecSoftKey,
                                       SELECT_POP_BOX_TYPE_NORMAL,
                                       pNumberSelector,
                                       strTitle,
                                       DIR_UI_ACTION_TRANSFER_NUMBER_CHOOSE);

        if (NULL != pSelectBox)
        {
            pSelectBox->SetEventFilter(pNumberSelector);
        }

    }
    else
    {
        CNumberSelector * pNumberSelector = g_DirectoryMgr.GetNumberSelector();

        if (NULL == pNumberSelector)
        {
            return false;
        }

        pNumberSelector->SetName(pRecord->GetDisplayName());

        MsgBoxSoftkey skCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
        VecMsgBoxSoftkey vecSoftKey;

        vecSoftKey.push_back(skCancel);

        QString strTitle = toQString(pRecord->GetDisplayName(), true);
#if IF_FEATURE_GBBOOK
        yl::string strTemp = strTitle.toUtf8().data();
        if (g_DirectoryMgr.GetActiveDirType() == DIRECTORY_TYPE_GENBAND)
        {
            MsgBoxSoftkey skAccount(LANG_TRANSLATE(TRID_ACCOUNT), MessageBox_Account);
            vecSoftKey.push_back(skAccount);

            int iAccountId = nAccountId;
            if (nAccountId == -1)
            {
                iAccountId = acc_GetDefaultAccount();
            }
            strTemp = "Account:" + acc_GetUsername(iAccountId);
        }

        strTitle = toQString(strTemp, true);
#endif

        MessageBox_ShowSelectBox(&g_DirectoryMgr, vecSoftKey, SELECT_POP_BOX_TYPE_TINY, pNumberSelector,
                                 strTitle, DIR_UI_ACTION_NUMBER_CHOOSE);

    }
    return true;
}

bool CDirUIHelper::DialCalllog(int nAccountId, const yl::string & strNumber,
                               CDirectoryControler * pContrl, BaseDirDataRecord * pRecord)
{
    SESSION_STATE state = talklogic_GetSessionStateBySessionID(talklogic_GetFocusedSessionID());

    if (SESSION_PRETRAN == state)
    {
        CTransferNumSelector * pNumberSelector = g_DirectoryMgr.GetTransferNumSelector();

        if (NULL == pNumberSelector)
        {
            return false;
        }

        pNumberSelector->Init(TRAN_NUM_OTHER);
        pNumberSelector->SetName(pRecord->GetDisplayName());
        pNumberSelector->SetNumber(strNumber);

        MsgBoxSoftkey skCancel(LANG_TRANSLATE(TRID_CANCEL), MessageBox_Cancel);
        MsgBoxSoftkey skSend(LANG_TRANSLATE(TRID_SEND), MessageBox_Send);
        MsgBoxSoftkey skBTransfer(LANG_TRANSLATE(TRID_BLIND_TRANSFER), MessageBox_BTransfer);

        VecMsgBoxSoftkey vecSoftKey;

        vecSoftKey.push_back(skBTransfer);
        vecSoftKey.push_back(skSend);
        vecSoftKey.push_back(skCancel);

        QString strTitle = toQString(pRecord->GetDisplayName(), true);


        CMessageBoxBase * pSelectBox = MessageBox_ShowSelectBox(&g_DirectoryMgr,
                                       vecSoftKey,
                                       SELECT_POP_BOX_TYPE_NORMAL,
                                       pNumberSelector,
                                       strTitle,
                                       DIR_UI_ACTION_TRANSFER_NUMBER_CHOOSE);

        if (NULL != pSelectBox)
        {
            pSelectBox->SetEventFilter(pNumberSelector);
        }
    }
    else
    {
        Dial(nAccountId, pRecord->GetDisplayName(), strNumber, pContrl, pRecord);
    }
    return true;
}

int CDirUIHelper::GetDialAccountId()
{
    return s_nDialAccountId;
}

void CDirUIHelper::ResetDialAccountId()
{
    s_nDialAccountId = AUTO_ACCOUNT_IDENTIFY;
}

#if IF_FEATURE_GBBOOK
void CDirUIHelper::SetDialAccountId(int nAccountId)
{
    s_nDialAccountId = nAccountId;
}
#endif

void CDirUIHelper::SetFrameList(CFrameList * pFrameList)
{
    DIRUI_INFO("CDirUIHelper::SetFrameList(%p)", pFrameList);
    m_pFrameList = pFrameList;

    if (NULL != m_pFrameList)
    {
        OnAttachFrameList();
    }
}

void CDirUIHelper::SetFocusInvalidFrameList(CFrameList * pFrameList)
{
    m_pFocusInvalidFrameList = pFrameList;
}

void CDirUIHelper::OnAttachFrameList()
{
    DIRUI_INFO("CDirUIHelper::OnAttachFrameList FrameList:%p", m_pFrameList);

    if (NULL == m_pFrameList)
    {
        return;
    }

#ifdef FL_SUPPORT_TITLE
    m_pFrameList->SetFrameListTitle(NULL);
    m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_NONE_TITLE_ITEM_COUNT);
#else
    m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_NORMAL_ITEM_COUNT);
#endif
}

int CDirUIHelper::GetActionByItem(int nItemAction)
{
    if (LIST_ITEM_ACTION_SELECT == nItemAction)
    {
        return DIR_ACTION_COMMON_SELECT;
    }
    else if (CONTACT_ITEM_ACTION_DETAIL == nItemAction)
    {
        return DIR_ACTION_COMMON_DETAIL;
    }

    return DIR_ACTION_NONE;
}

int CDirUIHelper::GetLisItemId(CListItem * pItem)
{
    int nContactItemType = CContactBaseItem::GetContactBaseItemType();

    if (NULL == pItem || nContactItemType != (nContactItemType & pItem->GetType()))
    {
        return knInvalidRId;
    }

    CContactBaseItem * pContactItem = (CContactBaseItem *)pItem;

    return pContactItem->GetId();
}

int CDirUIHelper::GetActionByTitle(int nUIState, int nTitleAction)
{
    if (LIST_TITLE_ACTION_DIAL_CONTACT == nTitleAction)
    {
        return DIR_ACTION_COMMON_SELECT;
    }

    return DIR_ACTION_NONE;
}

void CDirUIHelper::OnGroupChange()
{
}

void CDirUIHelper::SetAttributeList(int nDirType, LIST_ATTRIBUTE & lsAtrr)
{
    s_nAttrType = nDirType;
    s_listAtrr.clear();

    LIST_ATTRIBUTE::iterator iter = lsAtrr.begin();

    for (; iter != lsAtrr.end(); iter++)
    {
        ST_CONTACT_ATTRIBUTE & stAttr = *iter;

        s_listAtrr.push_back(stAttr);
    }
}

int CDirUIHelper::GetAttributeType()
{
    return s_nAttrType;
}

int CDirUIHelper::GetAttributeSize()
{
    return s_listAtrr.size();
}

bool CDirUIHelper::GetAttributeByIndex(int nIndex, yl::string & strName, yl::string & strValue)
{
    LIST_ATTRIBUTE::iterator iter = s_listAtrr.begin();

    for (int i = 0; iter != s_listAtrr.end(); ++iter, ++i)
    {
        if (i != nIndex)
        {
            continue;
        }

        ST_CONTACT_ATTRIBUTE & stAttr = *iter;

        strName = stAttr.m_strName;
        strValue = stAttr.m_strValue;
        return true;
    }

    return false;
}

bool CDirUIHelper::GetAttributeByName(int nIndex, const char * pszName, yl::string & strValue)
{
    if (NULL == pszName)
    {
        return false;
    }

    LIST_ATTRIBUTE::iterator iter = s_listAtrr.begin();

    for (int i = 0; iter != s_listAtrr.end(); ++iter)
    {
        ST_CONTACT_ATTRIBUTE & stAttr = *iter;

        if (0 != strcmp(pszName, stAttr.m_strName.c_str()))
        {
            continue;
        }

        if (i == nIndex)
        {
            strValue = stAttr.m_strValue;
            return true;
        }

        ++i;
    }

    return false;
}

bool CDirUIHelper::RefreshListItemAmount(int nUIState, int nFromIndex, int nTotalCount)
{
    if (NULL == m_pFrameList)
    {
        return false;
    }

    if (DIR_UI_STATE_DETAIL == nUIState)
    {
        return true;
    }

    if (DIR_UI_STATE_SEARCH == nUIState)
    {
        RefreshSearchTitle(nTotalCount);
        return true;
    }

    if (DIR_UI_STATE_SETTING == nUIState)
    {
        CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
        CSettingDelegate * pSettingDelegate = CSettingDelegate::GetDirDelegate(pDelegate);

        if (NULL != pSettingDelegate)
        {
            pSettingDelegate->SetTotalCount(nTotalCount);
            RefreshSettingTitle(nTotalCount, pSettingDelegate->GetSelectCount());
        }

        return true;
    }

    return false;
}

bool CDirUIHelper::IsShowSettingTitle()
{
    if (NULL == m_pFrameList)
    {
        return false;
    }

    CFrameListTitlePtr pTitle = m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CContactListTitle::IsContactListTitle(pTitle))
    {
        return true;
    }

    return false;
}

void CDirUIHelper::RefreshSettingTitle(int nTotalCount, int nSelectCount)
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    CContactListTitlePtr pTitle = (CContactListTitlePtr)m_pFrameList->GetFrameListTitle();

    if (NULL != pTitle && CContactListTitle::IsContactListTitle(pTitle))
    {
        pTitle->SetTotalAmount(nTotalCount);
        pTitle->SetSelectAmount(nSelectCount);
    }
}

bool CDirUIHelper::RefreshContactItem(int nItemIndex, int nDataIndex, int nDataCount,
                                      int nContactId,
                                      const yl::string & strName, const yl::string & strInfo,
                                      const yl::string & strHead
#if IF_FEATURE_FAVORITE_DIR
                                      , int nFavoriteIndex// = -1
#endif
                                     )
{
    DIRUI_INFO("RefreshContactItem(ItemIndex:%d DataIndex:%d ContactId:%d Name:%s Info:%s Head:%s)",
               nItemIndex, nDataIndex, nContactId, strName.c_str(), strInfo.c_str(), strHead.c_str());

    if (NULL == m_pFrameList)
    {
        return false;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    CSettingDelegate * pSettingDelegate = CSettingDelegate::GetDirDelegate(pDelegate);
    CDirectoryListDelegate * pListDelegate = pDelegate.Get();

    if (NULL == pListDelegate || pListDelegate != m_pFrameList->GetFrameDelegate())
    {
        return false;
    }

    if (0 == nItemIndex)
    {
        if (!m_pFrameList->IsCanReuseItem(CContactBaseItem::GetContactBaseItemType()))
        {
            m_pFrameList->ClearContent();
        }

        m_pFrameList->SetTotalDataCount(nDataCount);
        pListDelegate->SetPageDataIndex(nDataIndex);
    }

    bool bAdd = false;
    CContactBaseItemPtr pItem = (CContactBaseItemPtr)m_pFrameList->GetItemByIndex(nItemIndex);

    if (NULL == pItem)
    {
        pItem = new CContactBaseItem;
        DIRUI_INFO("new CContactBaseItem %p", pItem);

        if (NULL == pItem)
        {
            return false;
        }
        pItem->setObjectName(QString("Item%1").arg(nItemIndex));
        bAdd = true;
    }

    pItem->ClearContent();

    pItem->SetAction(pListDelegate);
    pItem->SetDataIndex(nDataIndex);

    if (nContactId == kszViewMoreId)
    {
        pItem->SetTitle(LANG_TRANSLATE(TRID_DIRECTORY_MORE_ITEM));
        pItem->setShowSubTitle(false);
        pItem->SetIcon(strHead.c_str());
        pItem->SetId(nContactId);
        if (NULL != pSettingDelegate)
        {
            pItem->SetEdit(true);
            pItem->SetChecked(pSettingDelegate->IsSelect(nContactId));
        }
        pItem->SetDetail(false);
        pItem->setShowInfo(false);
        pItem->SetContactPicVisible(false);
        if (bAdd && !m_pFrameList->AddItem(pItem))
        {
            pItem->Release();
            return false;
        }
        return true;

    }
    pItem->SetTitle(toQString(strName, true));
    pItem->setShowSubTitle(true);
    CDirectoryControlerPtr pControler = g_DirectoryMgr.GetActiveControler();
    yl::string strSearch = pControler->GetSearchKey();
    if (configParser_GetConfigInt(kszBsftDirUpdateEnable) == 1 || pControler->GetType() != DIRECTORY_TYPE_BROADSOFT)
    {
        pItem->SetHighLightText(toQString(strSearch));
    }

    if (strInfo.empty())
    {
        pItem->SetSubTitle(LANG_TRANSLATE(TRID_NONE));
    }
    else
    {
        pItem->SetSubTitle(toQString(strInfo, true));
    }

    pItem->SetIcon(strHead.c_str());
    pItem->SetId(nContactId);

    if (NULL != pSettingDelegate)
    {
        pItem->SetEdit(true);
        pItem->SetChecked(pSettingDelegate->IsSelect(nContactId));
    }
    else
    {
        pItem->SetEdit(false);

        CDirectoryControlerPtr pControler = pListDelegate->GetControler();
        bool bReadOnly = true;

        if (!pControler.IsEmpty())
        {
            bReadOnly = pControler->IsReadOnly();
#if IF_FEATURE_FAVORITE_DIR
            if (configParser_GetConfigInt(kszFavoriteDirAutoSetSwitch) == 1)
            {
                if (pControler->GetType() == DIRECTORY_TYPE_LOCAL
                        && LocalDir_IsContactInFavorite(nContactId))
                {
                    pItem->SetIcon(PIC_CONTACTS_ICON_FAVORITE);
                }

                if (nFavoriteIndex != -1)
                {
                    pItem->setShowInfo(true);
                    pItem->SetInfo(QString::number(nFavoriteIndex));
                }
                else
                {
                    pItem->setShowInfo(false);
                }
            }
#endif
        }

        pItem->SetDetail(!bReadOnly);
    }

    if (bAdd && !m_pFrameList->AddItem(pItem))
    {
        pItem->Release();
        return false;
    }

    return true;
}

bool CDirUIHelper::RefreshDetailItem(int nItemIndex, int nDataIndex, int nDataCount,
                                     QString & strTitle, QString & strValue)
{
    if (NULL == m_pFrameList)
    {
        return false;
    }

    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    CDirectoryListDelegate * pListDelegate = pDelegate.Get();

    if (NULL == pListDelegate || pListDelegate != m_pFrameList->GetFrameDelegate())
    {
        return false;
    }

    if (0 == nItemIndex)
    {
        if (!m_pFrameList->IsCanReuseItem(CDetailListItem::GetDetailListItemType()))
        {
            m_pFrameList->ClearContent();
        }

        m_pFrameList->SetTotalDataCount(nDataCount);
        pListDelegate->SetPageDataIndex(nDataIndex);
    }

    CDetailListItemPtr pItem = (CDetailListItemPtr)m_pFrameList->GetItemByIndex(nItemIndex);

    bool bAdd = false;

    if (NULL == pItem)
    {
        bAdd = true;
        pItem = new CDetailListItem;

        if (NULL == pItem)
        {
            return false;
        }
    }

    pItem->ClearContent();
    pItem->SetAction(pListDelegate);
    pItem->SetDataIndex(nDataIndex);

    pItem->SetTitle(strTitle);
    pItem->SetValue(strValue);

    if (bAdd && !m_pFrameList->AddItem(pItem))
    {
        pItem->Release();
    }

    return true;
}

void CDirUIHelper::RefreshSearchTitle(int nTotalCount)
{
    if (NULL == m_pFrameList)
    {
        return;
    }

    if (0 == nTotalCount)
    {
        m_pFrameList->SetFrameListTitle(NULL);
        m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_NONE_TITLE_ITEM_COUNT);
    }
    else
    {
        CFrameSearchTitlePtr pTitle = (CFrameSearchTitlePtr)m_pFrameList->GetFrameListTitle();

        if (NULL != pTitle && (!CFrameSearchTitle::IsSearchTitle(pTitle)))
        {
            pTitle = NULL;
            m_pFrameList->SetFrameListTitle(NULL);
            m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_NONE_TITLE_ITEM_COUNT);
        }

        bool bAdd = false;

        if (NULL == pTitle)
        {
            bAdd = true;
            pTitle = new CFrameSearchTitle;
        }

        if (NULL != pTitle)
        {
            pTitle->SetSearchAmount(nTotalCount);

            if (bAdd)
            {
                m_pFrameList->SetFrameListTitle(pTitle);
                m_pFrameList->SetMaxPageItemCount(DIR_UI_LIST_TITLE_ITEM_COUNT);
            }
            else
            {
                pTitle->RefreshTitle();
            }
        }
    }
}

bool CDirUIHelper::AdjustListData(int nTotalCount, int & nDataIndex, int & nDataCount)
{
    // 数据为空，不需要调整
    if (nTotalCount <= 0 || NULL == m_pFrameList)
    {
        return false;
    }

    int nMaxPageCount = m_pFrameList->GetMaxPageItemCount();

    // 只有一页数据时
    if (nTotalCount <= nMaxPageCount)
    {
        if (nDataIndex > 0)
        {
            nDataIndex = 0;
            nDataCount = nTotalCount;
            return true;
        }

        return false;
    }

    // 超过一页数据
    if (nMaxPageCount > 0 && (nDataIndex + nMaxPageCount) > nTotalCount)
    {
        int nRemainder = nTotalCount % nMaxPageCount;

        if (0 == nRemainder)
        {
            nDataIndex = nTotalCount - nMaxPageCount;
            nDataCount = nMaxPageCount;
        }
        else
        {
            nDataIndex = nTotalCount - nRemainder;
            nDataCount = nRemainder;
        }

        return true;
    }

    return false;
}

#ifdef DIR_SUPPORT_BATCH_OPERATE
void CDirUIHelper::OnBatchCopyContactSuccess()
{
    CDirectoryListDelegatePtr pDelegate = g_DirectoryMgr.GetActiveDelegate();
    CSettingDelegate * pSettingDelegate = CSettingDelegate::GetDirDelegate(pDelegate);

    if (NULL == pSettingDelegate)
    {
        return;
    }

    pSettingDelegate->ResetSelectCount();
    pSettingDelegate->RefreshFrameList();
}
#endif
