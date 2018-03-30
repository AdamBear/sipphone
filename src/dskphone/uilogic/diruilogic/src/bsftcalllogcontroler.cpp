#include "bsftcalllogcontroler.h"
#include "translateiddefine.h"
#include "bsftpwdmgr.h"
#include "bsftcontactcontroler.h"
#include "directorycommon.h"
#include "account/include/modaccount.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"
#include "translateiddefine.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#include "diruiadapter.h"

#if DIR_SUPPORT_NET_CALLLOG_DELETE
#include "contacts/directory/src/basemethod.h"
#endif

CBSFTCallLogControler::CBSFTCallLogControler()
    : CCommonDirCtrl(CALL_LOG_TYPE_BROADSOFT, BSFT_LOG)
    , m_bDownloading(false)
#if IF_FEATURE_MUTI_CALLLOG
    , m_eDirType(CT_CALLALL)
#endif
#if IF_FEATURE_MUTI_CALLLOG
    , m_nAccountID(-1)
#endif
{
}

CBSFTCallLogControler::~CBSFTCallLogControler()
{
}

CBSFTCallLogControler * CBSFTCallLogControler::GetControler(CDirectoryControlerPtr & pControler)
{
    if (pControler.IsEmpty() || CALL_LOG_TYPE_BROADSOFT != pControler->GetType())
    {
        return NULL;
    }

    return (CBSFTCallLogControler *)pControler.Get();
}

BSFTCallLogRecord * CBSFTCallLogControler::GetBSFTCallLog(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_BSFT_CALLLOG != pDetail->GetType())
    {
        return NULL;
    }

    return (BSFTCallLogRecord *)pDetail.Get();
}

#if IF_FEATURE_MUTI_CALLLOG
bool CBSFTCallLogControler::IsMutiAccountChooseGroup(int nGroupId)
{
    if (!BsftLog_IsMutilLogEnable())
    {
        return false;
    }

    int nAccountID = -1;

    if (kszPlacedLogId == nGroupId)
    {
        yl::string strNumber = "";
        LocalLog_GetLastOutCallNumAndAccountID(strNumber, nAccountID);
    }
    else if (kszMissLogId == nGroupId)
    {
        nAccountID = LocalLog_GetLastChildMissAccountID();
    }

    if (acc_GetAvailableAccountCount() > 1 && nAccountID < 0)
    {
        return true;
    }

    return false;
}
#endif

void CBSFTCallLogControler::SetGroupId(int nGroupId)
{
#ifndef _WIN32
#warning Dir_ClearNotifyList
#endif
#if !DIR_UI_SUPPORT_LOAD_DATA_LIST
//  Dir_ClearNotifyList(nGroupId);
#endif

    CCommonDirCtrl::SetGroupId(nGroupId);
}

bool CBSFTCallLogControler::PreLoadContact(int nGroupId)
{
#if IF_FEATURE_MUTI_CALLLOG
    DIRUI_INFO("CBSFTCallLogControler::PreLoadContact nGroupID [%d]", nGroupId);
#endif

    if (!CCommonDirCtrl::PreLoadContact(nGroupId))
    {
        return false;
    }

    int nAccountID = 0;

#if IF_FEATURE_MUTI_CALLLOG
    if (BsftLog_IsMutilLogEnable())
    {
        if (BsftLog_IsAccountGroup(nGroupId))
        {
            return true;
        }
        nAccountID = BsftLog_GetAccountIDByGroupID(nGroupId);
    }
#endif

    if (!BWCallControl_IsAccountXSIEnable(nAccountID))
    {
        g_DirUIAdapter.ShowMsgBox(TRID_XSI_INFORMATION_IS_INCOMPLETE);
        return true;
    }

    IDirUIHelper * pUIHelper = m_pUIHelper.Get();

    if (NULL == pUIHelper)
    {
        return false;
    }

    _BsftPwdMgr.SetDirType(m_nType);

    int nStatus = _BsftPwdMgr.CheckBSFTPassword(pUIHelper, nGroupId);

    if (XSI_SIPACCOUNT_NOT_AVAILABLE == nStatus)
    {
        return true;
    }
    else if (XSI_SUCCESS != nStatus)
    {
        return false;
    }

    int nRetCode = BsftLog_ReqLoadCallLog(nGroupId);

    if (DIR_RET_SUCC != nRetCode)
    {
        CCommonDirCtrl::ShowErrorMsg(nRetCode);
        return true;
    }

    m_bDownloading = true;
    g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_BSFT_DOWNLOAD);
    return true;
}

#if DIR_UI_FEATURE_LOAD_DATA_LIST
bool CBSFTCallLogControler::LoadMenuData(int nDataIndex, int nDataCount)
{
    if (!BWCallControl_IsAccountXSIEnable(0))
    {
        g_DirUIAdapter.ShowMsgBox(TRID_XSI_INFORMATION_IS_INCOMPLETE);
        return true;
    }

    return CCommonDirCtrl::LoadMenuData(nDataIndex, nDataCount);
}
#endif

bool CBSFTCallLogControler::DialCallLog(int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    return DialContact(m_nContactId, nAccountId);
}

bool CBSFTCallLogControler::EditCallLog()
{
    SmartPtr pDetail = Dir_GetDetailById(m_nContactId, m_eLogicType);
    BSFTCallLogRecord * pRecord = GetBSFTCallLog(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetBSFTCallLog(ContactId:%d) fail EditCallLog", m_nContactId);
        return false;
    }

    talklogic_EnterDialUI(pRecord->GetNumber(), acc_GetDefaultAccount());

    return true;
}

bool CBSFTCallLogControler::IsPersonalCalllog()
{
    SmartPtr pDetail = Dir_GetDetailById(m_nContactId, m_eLogicType);

    if (pDetail.IsEmpty())
    {
        return false;
    }

    int nContactId = Dir_IsContactNameExist(pDetail->GetDisplayName(), BSFT_DIR);

    if (nContactId <= 0)
    {
        return false;
    }

    return (CBSFTContactControler::IsPersonal(nContactId));
}

bool CBSFTCallLogControler::IsGroupData(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty())
    {
        return false;
    }

    if (DIR_TYPE_BSFT_LOG_GROUP == pDetail->GetType())
    {
        return true;
    }

    return CCommonDirCtrl::IsGroupData(pDetail);
}

void CBSFTCallLogControler::OnEnterMenu()
{
    if (!BWCallControl_IsAccountXSIEnable(0))
    {
        g_DirUIAdapter.ShowMsgBox(TRID_XSI_INFORMATION_IS_INCOMPLETE);
    }
}

bool CBSFTCallLogControler::IsCanEnterSearchByKey(int nKey) const
{
    return false;
}

int CBSFTCallLogControler::GetBrowseItemAmount()
{
#if IF_FEATURE_MUTI_CALLLOG && IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
    if (knInvalidRId == GetGroupId() && -1 != m_nAccountID)
    {
#ifndef WIN32
#warning "需要等逻辑层添加0注册账号时获取呼叫类型组的接口,1个账号待确认"
#endif // !WIN32
        VEC_CONTACT_ID vecID;
        Dir_GetChildGroupByGroupID(vecID, m_nAccountID, BSFT_LOG);
        return vecID.size();
    }
#endif

    int nItemAmount = GetGroupChildrenAmount(m_nGroupId, DIR_TYPE_BSFT_LOG_GROUP);

    if (DIR_INVALID_AMOUNT != nItemAmount)
    {
        return nItemAmount;
    }

    return CCommonDirCtrl::GetBrowseItemAmount();
}

void CBSFTCallLogControler::OnCopyContact(int nContactId)
{
    SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
    BSFTCallLogRecord * pBSFTCallLog = GetBSFTCallLog(pDetail);

    if (NULL == pBSFTCallLog)
    {
        return;
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = DIR_ATTR_NAME;
    stAttr.m_strValue = pBSFTCallLog->GetDisplayName();
    lsAttr.push_back(stAttr);

    stAttr.m_strName = DIR_ATTR_NUMBER;
    stAttr.m_strValue = pBSFTCallLog->GetNumber();
    lsAttr.push_back(stAttr);

    SetAttributeList(m_nType, lsAttr);
    g_DirUIAdapter.EnterUI(DIRECTORY_TYPE_BROADSOFT, DIR_UI_STATE_ADDITION,
                           DIR_ACTION_BSFT_ADD_TO_PERSONAL, kszPersonalId);
}

void CBSFTCallLogControler::PrepareDetailData()
{
    m_vecDetail.clear();

    SmartPtr pDetail = GetCurrentDetail();
    BSFTCallLogRecord * pCallLog = CBSFTCallLogControler::GetBSFTCallLog(pDetail);

    if (NULL == pCallLog)
    {
        return;
    }

    ST_DETAIL_DATA stDetail;

    stDetail.nType = DETAIL_DATA_TYPE_NONE;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_NAME);
    stDetail.stAttr.m_strValue = pCallLog->GetDisplayName();

    m_vecDetail.push_back(stDetail);

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_NUMBER);
    stDetail.stAttr.m_strValue = pCallLog->GetNumber();

    m_vecDetail.push_back(stDetail);

    time_t tmTick = TransBsftCallLogTime(pCallLog->m_strBornTick.c_str(), true);

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_TIME);
    stDetail.stAttr.m_strValue = g_DirUIAdapter.Time2DetailString(tmTick);

    m_vecDetail.push_back(stDetail);
}

bool CBSFTCallLogControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CBSFTCallLogControler::OnAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

#if IF_FEATURE_MUTI_CALLLOG
    //在账号选择界面获取账号+类型的组IDDIR_UI_STATE_CHOOSE_ACCOUNT
    if (BsftLog_IsMutilLogEnable() && DIR_UI_STATE_CHOOSE_ACCOUNT == GetState())
    {
        //如果需要显示类型选择界面(无左右键)，则记录账号ID,并进入类型选择界面
        //如果不需要进入类型选择界面，则计算出列表的组ID
        m_nAccountID = nId;
#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST
        nId = knInvalidRId;
#else
        nId = BsftLog_ChangeToActualGroup(nId, m_eDirType);
#endif
    }
#endif

    if (CCommonDirCtrl::OnAction(nAction, nDataIndex, nId))
    {
        return true;
    }

    switch (nAction)
    {
    case DIR_ACTION_BSFT_ENTER_ADD_TO_PERSONAL:
        {
            OnCopyContact(nId);
        }
        break;
    case DIR_ACTION_BSFT_CHG_PWD:
        {
            if (_BsftPwdMgr.RetryBSFTPwd(m_pUIHelper.Get()))
            {
                m_bDownloading = true;

                g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT,
                                           DIR_ACTION_BSFT_DOWNLOAD);
            }
        }
        break;
    case DIR_ACTION_BSFTLOG_EDIT:
        {
            EditCallLog();
        }
        break;
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_BSFT_BATCH_MOVE_TO_PERSONAL:
        {
            return g_DirUIAdapter.PopSureMsgBox(TRID_NOTE, TRID_ASK_ADD_SELECTED_TO_PERSONAL, nAction, nId);
        }
        break;
#endif
#if DIR_SUPPORT_NET_CALLLOG_DELETE
    case DIR_ACTION_BSFTLOG_DELETE:
        {
            int nRealID = (DIR_UI_STATE_DETAIL == GetState()) ? m_nContactId : nId;
            return g_DirUIAdapter.PopSureMsgBox(TRID_BSFT_LOG_DELETE_TITLE, TRID_BSFT_LOG_DELETE, nAction, nRealID);
        }
        break;
    case DIR_ACTION_BSFTLOG_CLEAR:
        {
            return OnDeleteGroup(nAction, GetGroupId());
        }
        break;
#endif
    default:
        {
            return false;
        }
        break;
    }

    return true;
}

bool CBSFTCallLogControler::ExecAction(int nAction, int nDataIndex, int nId)
{
    switch (nAction)
    {
    case DIR_ACTION_BSFT_ENTER_CHG_PWD:
        {
            _BsftPwdMgr.EnterBSFTPwdUI();
            return true;
        }
        break;
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_BSFT_BATCH_MOVE_TO_PERSONAL:
        {
            VEC_CONTACT_ID vecContactId;
            IDirUIHelper * pUIHelper = m_pUIHelper.Get();

            if (NULL == pUIHelper || !pUIHelper->GetSelectContactId(vecContactId))
            {
                return false;
            }

            int nSize = vecContactId.size();

            if (nSize <= 0)
            {
                return false;
            }

            for (int i = 0; i < nSize ; ++i)
            {
                int nContactId = vecContactId[i];
                SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
                BSFTCallLogRecord * pCallLog = GetBSFTCallLog(pDetail);

                if (NULL == pCallLog)
                {
                    continue;
                }

                ContactBroadSoftPersonal itemData;

                itemData.m_strDisplayName = pCallLog->GetDisplayName();

                // 去掉左右空格
                itemData.m_strDisplayName = commonAPI_TrimString(itemData.m_strDisplayName, " ");

                itemData.SetNumber(pCallLog->GetNumber());

                if (!CBSFTContactControler::CheckPersonalDataValid(itemData))
                {
                    continue;
                }

                int nOriContactId = Dir_IsContactNameExist(itemData.m_strDisplayName, m_eLogicType);

                if (nOriContactId == nContactId)
                {
                    DIRUI_INFO("Dir_IsContactNameExist(%d, %s, %s)", nContactId, itemData.m_strDisplayName.c_str(),
                               pCallLog->GetNumber().c_str());

                    BsftDir_EditPersonalContact(nContactId, itemData, SEARCH_NONE);
                }
                else
                {
                    DIRUI_INFO("BSFTDir_AddContact2Personal(%s %s)", itemData.m_strDisplayName.c_str(),
                               pCallLog->GetNumber().c_str());

                    BsftDir_AddContact2Personal(itemData);
                }
            }

            g_DirUIAdapter.ShowMsgBox(TRID_ADD_CONTACTS_SUCCESS);

            CBSFTCallLogUIHelper * pUIHelper = GetBSFTCallLogUIHelper();

            if (NULL != pUIHelper)
            {
                pUIHelper->OnBatchCopyContactSuccess();
            }

            return true;
        }
        break;
#endif
#if DIR_SUPPORT_NET_CALLLOG_DELETE
    case DIR_ACTION_BSFTLOG_DELETE:
        {
            int nRetCode = BsftLog_RemoveSingleLog(nId);

            bool bBack = (DIR_UI_STATE_DETAIL == GetState()) ? true : false;
            int nAction = bBack ? DIR_ACTION_MSGBOX_DETAIL_DELETE_BACK : DIR_ACTION_NONE;
            if (CheckDeleteCallLog(nRetCode, nAction) && !bBack)
            {
                IDirUIHelper * pUIHelper = m_pUIHelper.Get();
                if (NULL != pUIHelper)
                {
                    pUIHelper->RefreshUIAfterDeleted(bBack);
                }
            }

            return true;
        }
        break;
    case DIR_ACTION_BSFTLOG_CLEAR:
        {
            DIRUI_INFO("CBSFTCallLogControler::ClearAllCallLog() [%d]", GetGroupId());
            int nRetCode = BsftLog_RemoveGroupLog(GetGroupId());
            if (CheckDeleteCallLog(nRetCode))
            {
                RefreshUI(DIR_UI_REFRESH_TYPE_ALL, DIR_UI_REFRESH_PARAM_ALL);
            }

            return true;
        }
        break;
    case DIR_ACTION_MSGBOX_DETAIL_DELETE_BACK:
        {
            IDirUIHelper * pUIHelper = m_pUIHelper.Get();
            if (NULL != pUIHelper)
            {
                g_DirUIAdapter.ExitCurrentUI(GetState());
                pUIHelper->RefreshUIAfterDeleted(true);
            }
            return true;
        }
        break;
#endif
    default:
        break;
    }

    return false;
}

bool CBSFTCallLogControler::CancelAction(int nAction, int nDataIndex, int nId)
{
    if (DIR_ACTION_BSFT_DOWNLOAD == nAction)
    {
        m_bDownloading = false;

        IDirUIHelperPtr pDirUIHelper = m_pUIHelper;
        IDirUIHelper * pUIHelper = pDirUIHelper.Get();

        if (NULL != pUIHelper)
        {
            pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                                 DIR_UI_REFRESH_PARAM_NONE);
        }

        return true;
    }

    return false;
}

bool CBSFTCallLogControler::DialContact(int nContactId, int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
    BSFTCallLogRecord * pRecord = GetBSFTCallLog(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetBSFTCallLog(ContactId:%d) fail DialContact", nContactId);
        return false;
    }

#if IF_FEATURE_MUTI_CALLLOG
    if (AUTO_ACCOUNT_IDENTIFY == nAccountId)
    {
        nAccountId = pRecord->m_nLineID;
    }
#endif

    int nDialAction = DIR_DIAL_ACTION_DIGIT_MAP_HISTORY;

#if IF_BUG_27045
    CALL_TYPE nType = pRecord->GetLogType();

    //未接和已接来电才需要加区号
    if (CT_MISSCALL == nType || CT_INCOMING == nType)
    {
        nDialAction |= DIR_DIAL_ACTION_ADD_AREA_CODE;
    }
#endif

    return g_DirUIAdapter.Dial(nAccountId, pRecord->GetDisplayName(), pRecord->GetNumber(),
                               nDialAction);
}

LRESULT CBSFTCallLogControler::OnMessage(msgObject & refMessage)
{
    if (refMessage.message < BROADSOFT_CALLLOG_MESSAGE_BEGIN
            || refMessage.message > BROADSOFT_CALLLOG_MESSAGE_END)
    {
        return FALSE;
    }

    DIRUI_INFO("CBSFTCallLogControler::OnMessage(Msg:%d wp:%d lp:%d)",
               refMessage.message, refMessage.wParam, refMessage.lParam);

    if (BROADSOFT_CALLLOG_DOWNLOADED_RESULT == refMessage.message)
    {
        IDirUIHelperPtr pDirUIHelper = m_pUIHelper;
        IDirUIHelper * pUIHelper = pDirUIHelper.Get();

        if (NULL == pUIHelper)
        {
            return FALSE;
        }

        if (m_bDownloading)
        {
            g_DirUIAdapter.EndWaiting();
            m_bDownloading = false;
        }

        if (1 == refMessage.lParam)
        {
            pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                                 DIR_UI_REFRESH_PARAM_ALL);
        }
        else if (401 == refMessage.lParam)
        {
            DIRUI_INFO("RetryBSFTPwdEnter while BROADSOFT_CALLLOG_DOWNLOADED_RESULT");
            _BsftPwdMgr.SetDirType(m_nType);
            _BsftPwdMgr.CheckBSFTPassword(pUIHelper, GetGroupId());
        }
        else if (73 == refMessage.lParam)
        {
            g_DirUIAdapter.ShowMsgBox(TRID_SERVER_URL_IS_INVALID);
        }
        else
        {
            g_DirUIAdapter.ShowMsgBox(TRID_FAIL_TO_CONNECT_TO_SERVER);
        }

        return TRUE;
    }
    else if (BROADSOFT_CALLLOG_RELOAD == refMessage.message)
    {
        if (m_bDownloading)
        {
            g_DirUIAdapter.EndWaiting();
            m_bDownloading = false;
        }

        _BsftPwdMgr.ExitBSFTPwdUI();
        g_DirUIAdapter.ExitAllUI(m_nType);
        return TRUE;
    }

    return FALSE;
}

#if IF_FEATURE_MUTI_CALLLOG
bool CBSFTCallLogControler::GetBrowseGroupList(VEC_CONTACT_ID & vecId)
{
    if (!BsftLog_IsMutilLogEnable() || BsftLog_IsAccountGroup(m_nGroupId))
    {
        return CCommonDirCtrl::GetBrowseGroupList(vecId);
    }

    return Dir_GetChildGroupByGroupID(vecId, Dir_GetParentIdById(m_nGroupId, BSFT_LOG), BSFT_LOG);
}

void CBSFTCallLogControler::UnInit()
{
    m_nAccountID = -1;

    CCommonDirCtrl::UnInit();

    BsftLog_ClearCallLog();
}

void CBSFTCallLogControler::SetCallType(CALL_TYPE eDirType)
{
    m_eDirType = eDirType;
}

int CBSFTCallLogControler::GetAccountID()
{
    return m_nAccountID;
}
#endif

#if DIR_SUPPORT_NET_CALLLOG_DELETE
bool CBSFTCallLogControler::OnDeleteGroup(int nAction, int nGroupId)
{
    yl::string strNote = "";
    CALL_TYPE eCurrentType = BsftLog_GetCallTypeByGroupID(nGroupId);
    if (CT_CALLALL == eCurrentType)
    {
        strNote = TRID_DO_YOU_WANT_TO_DELETE_ALL_CALL_LOGS;
    }
    else if (CT_MISSCALL == eCurrentType)
    {
        strNote = TRID_DO_YOU_WANT_TO_DELETE_ALL_MISSED_CALL_LOGS;
    }
    else if (CT_OUTGOING == eCurrentType)
    {
        strNote = TRID_DO_YOU_WANT_TO_DELETE_ALL_PLACED_CALL_LOGS;
    }
    else if (CT_INCOMING == eCurrentType)
    {
        strNote = TRID_DO_YOU_WANT_TO_DELETE_ALL_RECEIVED_CALL_LOGS;
    }

    if (strNote.empty())
    {
        return false;
    }

    yl::string strExtraData = TRID_THIS_ACTION_CAN_NOT_BE_REVERSED;

    return g_DirUIAdapter.PopSureMsgBox(TRID_BSFT_LOG_DELETE_ALL_TITLE, strNote.c_str(),
                                        nAction, nGroupId, strExtraData.c_str());
}

bool CBSFTCallLogControler::CheckDeleteCallLog(int nRetCode, int nAction /* = DIR_ACTION_NONE */)
{
    if (DIR_RET_SUCC == nRetCode)
    {
        g_DirUIAdapter.ShowMsgBox(TRID_DELETING, "", nAction);
        return true;
    }
    else if (DIR_RET_FAIL == nRetCode)
    {
        g_DirUIAdapter.ShowMsgBox(TRID_DELETE_FIALED);
    }
    else
    {
        CCommonDirCtrl::ShowErrorMsg(nRetCode);
    }

    return false;
}

#if IF_DIR_UI_SHOW_CALLLOG_GROUP_LIST && IF_FEATURE_MUTI_CALLLOG
int CBSFTCallLogControler::GetMenuIdByIndex(int nIndex)
{
    if (!IsBrowseState() || -1 == m_nAccountID || knInvalidRId != GetGroupId())
    {
        return CCommonDirCtrl::GetMenuIdByIndex(nIndex);
    }

    VEC_CONTACT_ID vecID;
    Dir_GetChildGroupByGroupID(vecID, m_nAccountID, BSFT_LOG);

    if (nIndex < 0 || nIndex >= vecID.size())
    {
        return knInvalidRId;
    }

    return vecID[nIndex];
}
#endif

#endif
