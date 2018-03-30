#include "localcalllogcontroler.h"
#include "translateiddefine.h"
#ifndef DIR_SUPPORT_SINGLE_UIHELPER
#include "localcallloguihelper.h"
#endif
#ifdef DIR_SUPPORT_BATCH_OPERATE
#include "localcontactcontroler.h"
#endif
#include "transfernumselector.h"
#include "directorymgr.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"
#include "messagebox/modmessagebox.h"
#include "messagebox/selectmessagebox.h"
#include "account/include/modaccount.h"
#include "talk/talklogic/include/modtalklogic.h"
#include <resource/modresource.h>


CLocalCallLogControler::CLocalCallLogControler()
    : CCommonDirCtrl(CALL_LOG_TYPE_LOCAL, LOCAL_LOG)
{
}

CLocalCallLogControler::~CLocalCallLogControler()
{
}

CLocalCallLogUIHelper * CLocalCallLogControler::GetLocalCallLogUIHelper()
{
#ifdef DIR_SUPPORT_SINGLE_UIHELPER
    return m_pUIHelper;
#else
    return CLocalCallLogUIHelper::GetUIHelper(m_pUIHelper);
#endif
}

CLocalCallLogControler * CLocalCallLogControler::GetControler(CDirectoryControlerPtr & pControler)
{
    if (pControler.IsEmpty() || CALL_LOG_TYPE_LOCAL != pControler->GetType())
    {
        return NULL;
    }

    return (CLocalCallLogControler *)pControler.Get();
}

LocalLogItemRecord * CLocalCallLogControler::GetLocalCallLog(SmartPtr & pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_LOCALLOG_CALLLOG != pDetail->GetType())
    {
        return NULL;
    }

    return (LocalLogItemRecord *)pDetail.Get();
}

bool CLocalCallLogControler::DeleteCallLog(int nCallLogId)
{
    DIRUI_INFO("DeleteCallLog(%d) GroupId:%d", nCallLogId, GetGroupId());

    VectorId vecId;

    vecId.push_back(nCallLogId);
    LocalLog_RemoveLog(vecId, GetGroupId());

    CDirUIHelper::RefreshUIAfterDeleted();

    return true;
}

bool CLocalCallLogControler::ClearAllCallLog()
{
    LocalLog_ClearLogs(GetGroupId());

    CDirUIHelper::RefreshUI(true, true);
    return true;
}

bool CLocalCallLogControler::DialCallLog(int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    return DialContact(m_nContactId, nAccountId);
}

bool CLocalCallLogControler::EditCallLog()
{
    SmartPtr pDetail = Dir_GetDetailById(m_nContactId, m_eLogicType);
    LocalLogItemRecord * pRecord = GetLocalCallLog(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetLocalCallLog(ContactId:%d) fail EditCallLog", m_nContactId);
        return false;
    }

    int nAccountId = acc_GetAccountIDByName(pRecord->m_strLocalSipName.c_str(),
                                            pRecord->m_strLocalSipServer.c_str());

    ST_CONTACT_ATTRIBUTE stAttr;
    LIST_ATTRIBUTE lsNumber;

    stAttr.m_strValue = pRecord->GetNumber();
    stAttr.m_strName = TRID_NUMBER;
    lsNumber.push_back(stAttr);

    if (LocalLog_DetailedEnable()
            && !pRecord->m_strRelationNum.empty())
    {
        stAttr.m_strValue = pRecord->m_strRelationNum;
        stAttr.m_strName = TRID_RELATION;
        lsNumber.push_back(stAttr);
    }

    g_DirectoryMgr.SetIsEditCallLog(true);
    CDirUIHelper::DialCalllog(nAccountId, lsNumber, this, pRecord);

    return true;
}

#ifdef DIR_SUPPORT_SWITCH_DETAIL
bool CLocalCallLogControler::GetContactImage(int nId, yl::string & strImage) const
{
    SmartPtr pDetail = Dir_GetDetailById(nId, m_eLogicType);
    LocalLogItemRecord * pRecord = GetLocalCallLog(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("Can't find Contact when GetContactImage(%d)", nId);
        return false;
    }

    strImage = pRecord->GetPhoto();

    res_toRealPath(strImage, PHFILE_TYPE_CONTACT_IMAGE);

    return true;
}
#endif

bool CLocalCallLogControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIRUI_INFO("CLocalCallLogControler::OnAction(Action:%d Index:%d Id:%d)",
               nAction, nDataIndex, nId);

    if (CCommonDirCtrl::OnAction(nAction, nDataIndex, nId))
    {
        return true;
    }

    switch (nAction)
    {
    case DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS:
    case DIR_ACTION_LOCAL_ENTER_COPY_TO_BLACKLIST:
        {
            OnCopyContact(nAction, nId);
        }
        break;
    case DIR_ACTION_LCLOG_DEL_CALL_LOG:
        {
            CLocalCallLogUIHelper * pUIHelper = GetLocalCallLogUIHelper();

            if (NULL != pUIHelper && !pUIHelper->IsSureDelete(nId))
            {
                return true;
            }

            DeleteCallLog(nId);
        }
        break;
    case DIR_ACTION_LCLOG_DEL_ALL_CALL_LOG:
        {
            return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_CLEAR_ALL_CALL_RECORDS,
                                               DIR_ACTION_LCLOG_DEL_ALL_CALL_LOG, nId);
        }
        break;
    case DIR_ACTION_LCLOG_EDIT:
        {
            EditCallLog();
        }
        break;
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_LOCAL_BATCH_MOVE_TO_BLACKLIST:
        {
            return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_MOVE_SELECTED_TO_BLACK_LIST, nAction, nId);
        }
        break;
    case DIR_ACTION_LCLOG_BATCH_DEL_CALL_LOG:
        {
            if (CDirUIHelper::IsSelectAllContact())
            {
                return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_CLEAR_ALL_CALL_RECORDS, nAction, nId);
            }
            else
            {
                return CDirUIHelper::PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_SELECTED_CONTACT_ITEM, nAction,
                                                   nId);
            }
        }
        break;
#endif
    default:
        {
            DIRUI_ERR("Action:%d is not be Processed in OnAction", nAction);
            return false;
        }
        break;
    }

    return true;
}

bool CLocalCallLogControler::GetContactName(yl::string & strName)
{
    SmartPtr pDetail = Dir_GetDetailById(m_nContactId, m_eLogicType);
    LocalLogItemRecord * pRecord = GetLocalCallLog(pDetail);

    if (NULL == pRecord)
    {
        return false;
    }

    strName = pRecord->GetDisplayName();

    if (strName.empty())
    {
        strName = pRecord->GetNumber();
    }

    return true;
}

bool CLocalCallLogControler::ExecAction(int nAction, int nDataIndex, int nId)
{
    switch (nAction)
    {
    case DIR_ACTION_LCLOG_DEL_CALL_LOG:
        {
            DeleteCallLog(nId);
        }
        break;
    case DIR_ACTION_LCLOG_DEL_ALL_CALL_LOG:
        {
            ClearAllCallLog();
            return true;
        }
        break;
#ifdef DIR_SUPPORT_BATCH_OPERATE
    case DIR_ACTION_LOCAL_BATCH_MOVE_TO_BLACKLIST:
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

            int nBlackListSize = Dir_GetContactCount(kszBlackListGroupId, LOCAL_DIR);

            if ((nBlackListSize + nSize) > DIRECTORY_MAX_BLACKLIST_SIZE)
            {
                CDirUIHelper::ShowMsgBox(TRID_ONLY_30_CONTACTS_ALLOWED);
                return false;
            }

            YLList<ContactGeneralItemData> listData;

            for (int i = 0; i < nSize; ++i)
            {
                int nCallLogId = vecContactId[i];
                SmartPtr pDetail = Dir_GetDetailById(nCallLogId, m_eLogicType);
                LocalLogItemRecord * pCallLogItem = GetLocalCallLog(pDetail);

                if (NULL == pCallLogItem)
                {
                    continue;
                }

                ContactGeneralItemData itemData;

                CLocalContactControler::GetCopyDefaultContact(itemData);

                itemData.m_strDisplayName = pCallLogItem->GetDisplayName();

                if (itemData.m_strDisplayName.empty())
                {
                    itemData.m_strDisplayName = pCallLogItem->GetNumber();
                }

                itemData.m_strOfficePhoneNumber = pCallLogItem->GetNumber();

                // 去掉左右空格
                itemData.m_strDisplayName = commonAPI_TrimString(itemData.m_strDisplayName, " ");

                if (!CLocalContactControler::CheckContactDataValid(itemData))
                {
                    continue;
                }

                listData.push_back(itemData);
            }

            int nRetCode = LocalDir_AddLocalContactList(listData, kszBlackListGroupId);

            if (nRetCode >= 0)
            {
                CDirUIHelper::ShowMsgBox(TRID_MOVE_BLACKLIST_SUCCESS);

                CLocalCallLogUIHelper * pUIHelper = GetLocalCallLogUIHelper();

                if (NULL != pUIHelper)
                {
                    pUIHelper->OnBatchCopyContactSuccess();
                }
            }
            else if (DIR_RET_GROUP_SIZE_UNEXPECTED == nRetCode)
            {
                CDirUIHelper::ShowMsgBox(TRID_ONLY_30_CONTACTS_ALLOWED);
            }
            else
            {
                CDirUIHelper::ShowErrorMsg(nRetCode);
            }

            return true;
        }
        break;
    case DIR_ACTION_LCLOG_BATCH_DEL_CALL_LOG:
        {
            if (CDirUIHelper::IsSelectAllContact())
            {
                if (!LocalLog_ClearLogs(GetGroupId()))
                {
                    return false;
                }
            }
            else
            {
                VEC_CONTACT_ID vecContactId;

                if (!CDirUIHelper::GetSelectContactId(vecContactId))
                {
                    return false;
                }

                if (!LocalLog_RemoveLog(vecContactId, GetGroupId()))
                {
                    return false;
                }
            }

            CDirUIHelper::RefreshUI(true, true);
            return true;
        }
        break;
#endif
    default:
        break;
    }

    return false;
}

void CLocalCallLogControler::OnCopyContact(int nAction, int nId)
{
    SmartPtr pDetail = Dir_GetDetailById(nId, m_eLogicType);
    LocalLogItemRecord * pRecord = GetLocalCallLog(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetLocalCallLog fail OnCopyContact(Action:%d Id:%d)", nAction, nId);
        return;
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = ATTR_NAME;
    stAttr.m_strValue = pRecord->GetDisplayName();
    if (stAttr.m_strValue.empty())
    {
        stAttr.m_strValue = pRecord->GetNumber();
    }

    lsAttr.push_back(stAttr);

    stAttr.m_strName = ATTR_NUMBER;
    stAttr.m_strValue = pRecord->GetNumber();
    lsAttr.push_back(stAttr);

    CDirUIHelper::SetAttributeList(m_nType, lsAttr);
    CDirUIHelper::EnterUI(DIRECTORY_TYPE_LOCAL, DIR_UI_STATE_ADDITION, nAction, knInvalidRId);
}

bool CLocalCallLogControler::DialContact(int nContactId, int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    DIRUI_INFO("CLocalCallLogControler::DialContact(nContactId:%d nAccountId:%d)", nContactId,
               nAccountId);
    SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
    LocalLogItemRecord * pRecord = GetLocalCallLog(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetLocalCallLog fail DialContact(ContactId:%d)", nContactId);
        return false;
    }

    if (AUTO_ACCOUNT_IDENTIFY == nAccountId)
    {
        nAccountId = acc_GetAccountIDByName(pRecord->m_strLocalSipName.c_str(),
                                            pRecord->m_strLocalSipServer.c_str());
    }

    // 如果号码中不带@，并且服务器地址与本地的不一致，则加上服务器地址
    yl::string strNumber = pRecord->GetNumber();

    if (yl::string::npos == strNumber.find("@") && !pRecord->m_strRemoteSipServer.empty()
            && pRecord->m_strRemoteSipServer != pRecord->m_strLocalSipServer)
    {
        //http://192.168.1.99/Bug.php?BugID=29218
        if (configParser_GetConfigInt(kszReCallUseLocalSipServer) == 0)
        {
            strNumber = strNumber + "@" + pRecord->m_strRemoteSipServer;
        }
    }

    ST_CONTACT_ATTRIBUTE stAttr;
    LIST_ATTRIBUTE lsNumber;

    stAttr.m_strValue = strNumber;
    stAttr.m_strName = TRID_NUMBER;
    lsNumber.push_back(stAttr);

    if (LocalLog_DetailedEnable()
            && !pRecord->m_strRelationNum.empty())
    {
        stAttr.m_strValue = pRecord->m_strRelationNum;
        stAttr.m_strName = TRID_RELATION;
        lsNumber.push_back(stAttr);
    }

    CDirUIHelper::DialCalllog(nAccountId, lsNumber, this, pRecord);

    return true;
}
