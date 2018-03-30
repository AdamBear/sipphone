#include "bsftcalllogcontroler.h"
#include "translateiddefine.h"
#include "bsftpwdmgr.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
#include "bsftcallloguihelper.h"
#endif
#include "bsftcontactcontroler.h"
#include "transfernumselector.h"
#include "directorymgr.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "messagebox/modmessagebox.h"
#include "messagebox/selectmessagebox.h"
#include "account/include/modaccount.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "broadsoft/xsiactionhelper/include/cbasehelper.h"
#include "broadsoft/bwcallcontrol/include/modbwcallcontrol.h"


CBSFTCallLogControler::CBSFTCallLogControler()
    : CCommonDirCtrl(CALL_LOG_TYPE_BROADSOFT, BSFT_LOG)
    , m_bDownloading(false)
#if IF_FEATURE_MUTI_CALLLOG
    , m_eDirType(CT_CALLALL)
#endif
{
}

CBSFTCallLogControler::~CBSFTCallLogControler()
{
}

CBSFTCallLogUIHelper * CBSFTCallLogControler::GetBSFTCallLogUIHelper()
{
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
    return m_pUIHelper;
#else
    return CBSFTCallLogUIHelper::GetUIHelper(m_pUIHelper);
#endif
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
BSFTLogGroupRecord * CBSFTCallLogControler::GetBSFTCallLogGroup(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_BSFT_LOG_GROUP != pDetail->GetType())
    {
        return NULL;
    }

    return (BSFTLogGroupRecord *)pDetail.Get();
}
#endif

bool CBSFTCallLogControler::PreLoadContact(int nGroupId)
{
#if IF_FEATURE_MUTI_CALLLOG
    DIRUI_INFO("CBSFTCallLogControler::PreLoadContact nGroupID [%d]", nGroupId);

    if (kszRootId == nGroupId)
    {
        if (!CCommonDirCtrl::PreLoadContact(nGroupId))
        {
            return false;
        }

        return true;
    }
#endif

    if (!CCommonDirCtrl::PreLoadContact(nGroupId))
    {
        return false;
    }

    int nAccountID = 0;

#if IF_FEATURE_MUTI_CALLLOG
    if (BsftLog_IsMutilLogEnable())
    {
        nAccountID = BsftLog_GetAccountIDByGroupID(nGroupId);
    }
#endif

    if (!BWCallControl_IsAccountXSIEnable(nAccountID))
    {
        CDirUIHelper::ShowMsgBox(TRID_XSI_INFORMATION_IS_INCOMPLETE);
        return true;
    }

    CBSFTCallLogUIHelper * pUIHelper = GetBSFTCallLogUIHelper();

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
        //http://bugfree.yealink.com/Bug.php?BugID=133528
        //不成功也需要重刷列表
        return true;
    }

    int nRetCode = BsftLog_ReqLoadCallLog(nGroupId);

    if (DIR_RET_SUCC != nRetCode)
    {
        CDirUIHelper::ShowErrorMsg(nRetCode);
        return true;
    }

    m_bDownloading = true;
    CDirUIHelper::ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_BSFT_DOWNLOAD);
    return true;
}

bool CBSFTCallLogControler::LoadMenuData(int nDataIndex, int nDataCount)
{
    if (!BWCallControl_IsAccountXSIEnable(0))
    {
        CDirUIHelper::ShowMsgBox(TRID_XSI_INFORMATION_IS_INCOMPLETE);
        return true;
    }

    return CCommonDirCtrl::LoadMenuData(nDataIndex, nDataCount);
}

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

void CBSFTCallLogControler::OnCopyContact(int nAction, int nId)
{
    SmartPtr pDetail = Dir_GetDetailById(nId, m_eLogicType);
    BSFTCallLogRecord * pBSFTCallLog = GetBSFTCallLog(pDetail);

    if (NULL == pBSFTCallLog)
    {
        return;
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = ATTR_NAME;
    stAttr.m_strValue = pBSFTCallLog->GetDisplayName();
    lsAttr.push_back(stAttr);

    stAttr.m_strName = ATTR_NUMBER;
    stAttr.m_strValue = pBSFTCallLog->GetNumber();
    lsAttr.push_back(stAttr);

    CDirUIHelper::SetAttributeList(m_nType, lsAttr);
    CDirUIHelper::EnterUI(DIRECTORY_TYPE_BROADSOFT, DIR_UI_STATE_ADDITION, nAction, kszPersonalId);
}

bool CBSFTCallLogControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CBSFTCallLogControler::OnAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

#if IF_FEATURE_MUTI_CALLLOG
    if (BsftLog_IsMutilLogEnable())
    {
//      if (acc_AccountNum() > 1)
//      {
        nId = BsftLog_ChangeToActualGroup(nId, m_eDirType);

        CBSFTCallLogUIHelper * pBSFTUIHelper = GetBSFTCallLogUIHelper();
        if (pBSFTUIHelper != NULL)
        {
            pBSFTUIHelper->RefreshCallLogGroup(nId);
        }
//      }
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
            OnCopyContact(nAction, nId);
        }
        break;
    case DIR_ACTION_BSFT_CHG_PWD:
        {
            CBSFTCallLogUIHelper * pBSFTUIHelper = GetBSFTCallLogUIHelper();

            if (_BsftPwdMgr.RetryBSFTPwd(pBSFTUIHelper))
            {
                m_bDownloading = true;

                if (NULL != pBSFTUIHelper)
                {
                    pBSFTUIHelper->ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT,
                                               DIR_ACTION_BSFT_DOWNLOAD);
                }
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
            return CDirUIHelper::PopSureMsgBox(TRID_NOTE, TRID_ASK_ADD_SELECTED_TO_PERSONAL, nAction, nId);
        }
        break;
#endif
#if DIR_SUPPORT_NET_CALLLOG_DELETE
    case DIR_ACTION_BSFT_DELETE_CALLLOG:
        {
            if (configParser_GetConfigInt(kszBsftLogDeleteEnable) == 1)
            {
                return CDirUIHelper::PopSureMsgBox(TRID_NOTE, TRID_BSFT_LOG_DELETE, nAction, nId);
            }
            return true;
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

            if (!CDirUIHelper::GetSelectContactId(vecContactId))
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

            CDirUIHelper::ShowMsgBox(TRID_MOVE_CONTACTS_SUCCESS);

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
    case DIR_ACTION_BSFT_DELETE_CALLLOG:
        {
            if (configParser_GetConfigInt(kszBsftLogDeleteEnable) == 1)

            {
                BsftLog_RemoveSingleLog(nId);
                CDirUIHelper::RefreshUIAfterDeleted();
            }
            return true;
        }
        break;

    case DIR_ACTION_BSFT_CLEAR_GROUP:
        {
            if (configParser_GetConfigInt(kszBsftLogDeleteEnable) == 1)
            {
                BsftLog_RemoveGroupLog(nId);
                CDirUIHelper::RefreshUIAfterDeleted();
            }
            return true;
        }
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
        CDirUIHelper::RefreshUI(false, false);
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
    if (nAccountId == AUTO_ACCOUNT_IDENTIFY)
    {
        nAccountId = pRecord->m_nLineID;
    }
#endif

    return CDirUIHelper::DialCalllog(nAccountId, pRecord->GetNumber(), this, pRecord);
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
        CBSFTCallLogUIHelper * pUIHelper = GetBSFTCallLogUIHelper();

        if (NULL == pUIHelper)
        {
            return FALSE;
        }

        if (m_bDownloading)
        {
            CDirUIHelper::EndWaiting();
            m_bDownloading = false;
        }

        if (1 == refMessage.lParam)
        {
            CDirUIHelper::RefreshUI(true, true);
        }
        else if (401 == refMessage.lParam)
        {
            DIRUI_INFO("RetryBSFTPwdEnter while BROADSOFT_CALLLOG_DOWNLOADED_RESULT");
            _BsftPwdMgr.SetDirType(m_nType);
            _BsftPwdMgr.CheckBSFTPassword(pUIHelper, GetGroupId());
        }
        else if (73 == refMessage.lParam)
        {
            CDirUIHelper::ShowMsgBox(TRID_SERVER_URL_IS_INVALID);
        }
        else
        {
            CDirUIHelper::ShowMsgBox(TRID_FAIL_TO_CONNECT_TO_SERVER);
        }

        return TRUE;
    }
    else if (BROADSOFT_CALLLOG_RELOAD == refMessage.message)
    {
        if (m_bDownloading)
        {
            CDirUIHelper::EndWaiting();
            m_bDownloading = false;
        }

        CBSFTCallLogUIHelper * pUIHelper = GetBSFTCallLogUIHelper();

        if (NULL != pUIHelper)
        {
            pUIHelper->ExitBSFTPwdUI();
        }

        CDirUIHelper::ExitAllUI(m_nType);
        return TRUE;
    }

    return FALSE;
}

#if IF_FEATURE_MUTI_CALLLOG
int CBSFTCallLogControler::GetAdjoinGroupId(bool bPrevious, int & nIndex)
{
    DIRUI_INFO("CBSFTCallLogControler::GetAdjoinGroupId [%d]", m_nGroupId);

    if (!BsftLog_IsMutilLogEnable() || BsftLog_IsAccountGroup(m_nGroupId))
    {
        return CCommonDirCtrl::GetAdjoinGroupId(bPrevious, nIndex);
    }

    VEC_CONTACT_ID vecId;
    if (!Dir_GetChildGroupByGroupID(vecId, Dir_GetParentIdById(m_nGroupId, BSFT_LOG), BSFT_LOG))
    {
        nIndex = 0;
        return knInvalidRId;
    }

    int nSize = vecId.size();

    if (nSize <= 0)
    {
        return knInvalidRId;
    }

    int i = 0;

    for (; i < nSize; ++i)
    {
        if (vecId[i] == m_nGroupId)
        {
            break;
        }
    }

    if (i >= nSize)
    {
        return vecId[0];
    }

    if (bPrevious)
    {
        --i;
    }
    else
    {
        ++i;
    }

    if (i < 0)
    {
        i = nSize - 1;
    }
    else if (i >= nSize)
    {
        i = 0;
    }

    nIndex = i;

    return vecId[i];
}

bool CBSFTCallLogControler::OnBack()
{
    if (acc_GetAvailableAccountCount() <= 1 || !BsftLog_IsMutilLogEnable())
    {
        return false;
    }

    int nCurGroupId = GetGroupId();

    if (knInvalidRId == nCurGroupId || kszRootId == nCurGroupId)
    {
        return false;
    }

    SetGroupId(kszRootId);

    CBSFTCallLogUIHelper * pBSFTUIHelper = GetBSFTCallLogUIHelper();
    if (pBSFTUIHelper != NULL)
    {
        pBSFTUIHelper->RefreshCallLogGroup(kszRootId);
    }

    return true;
}

void CBSFTCallLogControler::SetCallType(CALL_TYPE eDirType)
{
    m_eDirType = eDirType;
}

CALL_TYPE CBSFTCallLogControler::GetCallType()
{
    return m_eDirType;
}
#endif
