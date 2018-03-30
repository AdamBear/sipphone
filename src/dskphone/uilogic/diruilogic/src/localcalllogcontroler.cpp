#include "localcalllogcontroler.h"
#include "translateiddefine.h"
#include "uilogiccommon/include/uilogiclanghelper.h"
#ifdef DIR_SUPPORT_BATCH_OPERATE
#include "localcontactcontroler.h"
#include "commonunits/commonunits_def.h"
#include <resource/modresource.h>
#endif
#include "account/include/modaccount.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "diruiadapter.h"
#include "directorycommon.h"
#include "wireless/bluetoothNew/include/modbluetooth.h"


namespace
{
void GetCallLogDuration(int nDuration, yl::string & strDuration)
{
    char szBuffer[128];

    int nHour = nDuration / 3600;
    int nMinSec = nDuration % 3600;
    int nMin = nMinSec / 60;
    int nSec = nMinSec % 60;

    sprintf(szBuffer, "%02d:%02d:%02d", nHour, nMin, nSec);
    szBuffer[sizeof(szBuffer) - 1] = '\0';

    strDuration = szBuffer;
}
}

CLocalCallLogControler::CLocalCallLogControler()
    : CCommonDirCtrl(CALL_LOG_TYPE_LOCAL, LOCAL_LOG)
    , m_bChooseNumberToSend(true)
{
}

CLocalCallLogControler::~CLocalCallLogControler()
{
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

    IDirUIHelperPtr pDirUIHelper = m_pUIHelper;
    IDirUIHelper * pUIHelper = pDirUIHelper.Get();

    if (NULL != pUIHelper)
    {
        pUIHelper->RefreshUIAfterDeleted();
    }

    return true;
}

bool CLocalCallLogControler::ClearAllCallLog()
{
    LocalLog_ClearLogs(GetGroupId());

    IDirUIHelperPtr pDirUIHelper = m_pUIHelper;
    IDirUIHelper * pUIHelper = pDirUIHelper.Get();

    if (NULL != pUIHelper)
    {
        pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                             DIR_UI_REFRESH_PARAM_ALL);
    }

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

#if IF_BUG_27560
    if (pRecord->m_bHideSipName)
    {
        actURI_SetHideTalkInfo(true);
    }
#endif

    if (IsEnterNumberChoose(pRecord->m_strRelationNum))
    {
        OnEnterNumberChoose(nAccountId, pRecord->GetNumber(), pRecord->GetDisplayName(), pRecord->m_strRelationNum);
        m_bChooseNumberToSend = false;
    }
    else
    {
        talklogic_EnterDialUI(pRecord->GetNumber(), nAccountId);
    }

    return true;
}

int CLocalCallLogControler::GetBrowseItemAmount()
{
    int nItemAmount = GetGroupChildrenAmount(m_nGroupId, DIR_TYPE_LOCALLOG_GROUP);

    if (DIR_INVALID_AMOUNT != nItemAmount)
    {
        return nItemAmount;
    }

    return CCommonDirCtrl::GetBrowseItemAmount();
}

void CLocalCallLogControler::SetGroupId(int nGroupId)
{
#ifndef _WIN32
#warning Dir_ClearNotifyList
#endif
#if !DIR_UI_SUPPORT_LOAD_DATA_LIST
//  Dir_ClearNotifyList(nGroupId);
#endif

    CCommonDirCtrl::SetGroupId(nGroupId);
}

bool CLocalCallLogControler::IsCanEnterSearchByKey(int nKey) const
{
    return false;
}

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
        {
            OnCopyContact(nId, DIR_ACTION_LOCAL_COPY_TO_CONTACTS, kszAllContactId);
        }
        break;
    case DIR_ACTION_LOCAL_ENTER_COPY_TO_BLACKLIST:
        {
            OnCopyContact(nId, DIR_ACTION_LOCAL_COPY_TO_BLACKLIST, kszBlackListGroupId);
        }
        break;
    case DIR_ACTION_LCLOG_DEL_CALL_LOG:
        {
            DeleteCallLog(nId);
        }
        break;
    case DIR_ACTION_LCLOG_DEL_ALL_CALL_LOG:
        {
            return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_CLEAR_ALL_CALL_RECORDS,
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
            return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_MOVE_SELECTED_TO_BLACK_LIST, nAction,
                                                nId);
        }
        break;
    case DIR_ACTION_LCLOG_BATCH_DEL_CALL_LOG:
        {
            IDirUIHelper * pUIHelper = m_pUIHelper.Get();

            if (NULL != pUIHelper && pUIHelper->IsSelectAllContact())
            {
                return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_CLEAR_ALL_CALL_RECORDS, nAction, nId);
            }
            else
            {
                return g_DirUIAdapter.PopSureMsgBox(TRID_WARNING, TRID_IF_DELETE_SELECTED_CONTACT_ITEM, nAction,
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

#if IF_BUG_27560
bool CLocalCallLogControler::GetContactNameToUI(yl::string & strName)
{
    SmartPtr pDetail = Dir_GetDetailById(m_nContactId, m_eLogicType);
    LocalLogItemRecord * pRecord = GetLocalCallLog(pDetail);

    if (NULL == pRecord)
    {
        return false;
    }

    strName = pRecord->GetDisplayNameToUI();

    if (strName.empty())
    {
        strName = pRecord->GetNumberToUI();
    }

    return true;
}
#endif

bool CLocalCallLogControler::IsChooseNumberToSend()
{
    return m_bChooseNumberToSend;
}

bool CLocalCallLogControler::ExecAction(int nAction, int nDataIndex, int nId)
{
    switch (nAction)
    {
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

            int nBlackListSize = Dir_GetContactCount(kszBlackListGroupId, LOCAL_DIR);

            if ((nBlackListSize + nSize) > DIRECTORY_MAX_BLACKLIST_SIZE)
            {
                g_DirUIAdapter.ShowMsgBox(TRID_ONLY_30_CONTACTS_ALLOWED);
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
                g_DirUIAdapter.ShowMsgBox(TRID_ADD_BLACKLIST_SUCCESS);

                IDirUIHelper * pUIHelper = m_pUIHelper.Get();

                if (NULL != pUIHelper)
                {
                    pUIHelper->OnBatchCopyContactSuccess();
                }
            }
            else if (DIR_RET_GROUP_SIZE_UNEXPECTED == nRetCode)
            {
                g_DirUIAdapter.ShowMsgBox(TRID_ONLY_30_CONTACTS_ALLOWED);
            }
            else
            {
                CCommonDirCtrl::ShowErrorMsg(nRetCode);
            }

            return true;
        }
        break;
    case DIR_ACTION_LCLOG_BATCH_DEL_CALL_LOG:
        {
            IDirUIHelperPtr pDirUIHelper = m_pUIHelper;
            IDirUIHelper * pUIHelper = pDirUIHelper.Get();

            if (NULL == pUIHelper)
            {
                return false;
            }

            if (pUIHelper->IsSelectAllContact())
            {
                if (!LocalLog_ClearLogs(GetGroupId()))
                {
                    return false;
                }
            }
            else
            {
                VEC_CONTACT_ID vecContactId;

                if (!pUIHelper->GetSelectContactId(vecContactId))
                {
                    return false;
                }

                if (!LocalLog_RemoveLog(vecContactId, GetGroupId()))
                {
                    return false;
                }
            }

            pUIHelper->RefreshUI(DIR_UI_REFRESH_TYPE_LIST_VIEW | DIR_UI_REFRESH_TYPE_TITLE,
                                 DIR_UI_REFRESH_PARAM_ALL);
            return true;
        }
        break;
#endif
    default:
        break;
    }

    return false;
}

void CLocalCallLogControler::OnCopyContact(int nContactId, int nAction, int nGroupId)
{
    SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
    LocalLogItemRecord * pRecord = GetLocalCallLog(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetLocalCallLog fail OnCopyContact(Action:%d Id:%d)", nAction, nContactId);
        return;
    }

#if IF_BUG_27560
    if (pRecord->m_bHideSipName)
    {
        if (nAction == DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS)
        {
            g_DirUIAdapter.ShowMsgBox(TRID_UNABLE_TO_ADD_TO_CONTACTS);
        }

        if (nAction == DIR_ACTION_LOCAL_ENTER_COPY_TO_BLACKLIST)
        {
            g_DirUIAdapter.ShowMsgBox(TRID_UNABLE_TO_ADD_TO_BLACKLIST);
        }

        return;
    }
#endif

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    stAttr.m_strName = DIR_ATTR_NAME;
    stAttr.m_strValue = pRecord->GetDisplayName();
    if (stAttr.m_strValue.empty())
    {
        stAttr.m_strValue = pRecord->GetNumber();
    }

    lsAttr.push_back(stAttr);

    stAttr.m_strName = DIR_ATTR_NUMBER;
    stAttr.m_strValue = pRecord->GetNumber();
    lsAttr.push_back(stAttr);

    SetAttributeList(m_nType, lsAttr);
    g_DirUIAdapter.EnterUI(DIRECTORY_TYPE_LOCAL, DIR_UI_STATE_ADDITION, nAction, nGroupId);
}

void CLocalCallLogControler::PrepareDetailData()
{
    m_vecDetail.clear();

    SmartPtr pDetail = GetCurrentDetail();
    LocalLogItemRecord * pCallLog = GetLocalCallLog(pDetail);

    if (NULL == pCallLog)
    {
        return;
    }

    ST_DETAIL_DATA stDetail;

    stDetail.nType = DETAIL_DATA_TYPE_NONE;

#if IF_BUG_27560
    yl::string strName = pCallLog->GetDisplayNameToUI();
    yl::string strNumber = pCallLog->GetNumberToUI();
#else
    yl::string strName = pCallLog->GetDisplayName();
    yl::string strNumber = pCallLog->GetNumber();
#endif

    if (strName.empty())
    {
        strName = strNumber;
    }

    yl::string strTransName;

    TransBsftFACText(strName, strTransName);

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_NAME);
    stDetail.stAttr.m_strValue = strTransName;
    m_vecDetail.push_back(stDetail);

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_NUMBER);
    stDetail.stAttr.m_strValue = strNumber;
    m_vecDetail.push_back(stDetail);

    time_t tmTick = (time_t)atoi(pCallLog->m_strBornTick.c_str());

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_TIME);
    stDetail.stAttr.m_strValue = g_DirUIAdapter.Time2DetailString(tmTick);
    m_vecDetail.push_back(stDetail);

#if !IF_FEATURE_HIDE_CALLLOG_DETAIL_LINE
    yl::string strLine;

    if (pCallLog->m_strLocalSipName.empty())
    {
#ifdef IF_SUPPORT_BLUETOOTH
        if (Bluetooth_GetBtAccountID() == pCallLog->m_nLine)
        {
            strLine = _UILOGIC_LANG_TRAN(TRID_BLUETOOTH_MOBILE_LINE);
        }
        else
#endif
        {
            strLine = _UILOGIC_LANG_TRAN(TRID_UNKNOWN_LINE);
        }
    }
    else
    {
        if (acc_GetRegisterName(acc_GetDirectIPAccountID()) == pCallLog->m_strLocalSipName)
        {
            strLine = _UILOGIC_LANG_TRAN(TRID_DIRECT_IP_LINE);
        }
#ifdef IF_FEATURE_PSTN
        else if (pCallLog->m_strLocalSipName.find("PSTN") != yl::string::npos)
        {
            strLine = pCallLog->m_strLocalSipName;
        }
#endif
        else
        {
            int nAccountId = acc_GetAccountIDByName(pCallLog->m_strLocalSipName.c_str(),
                                                    pCallLog->m_strLocalSipServer.c_str());

            // http://10.2.1.199/Bug.php?BugID=38583
            if (-1 == nAccountId)
            {
                strLine = _UILOGIC_LANG_TRAN(TRID_UNKNOWN);
            }
            else
            {
                strLine = _UILOGIC_LANG_TRAN(TRID_LINE);

                char szBuffer[32];

                _snprintf(szBuffer, sizeof(szBuffer), " %d", (nAccountId + 1));
                szBuffer[sizeof(szBuffer) - 1] = '\0';

                strLine += szBuffer;
            }
        }
    }

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_LINE);
    stDetail.stAttr.m_strValue = strLine;
    m_vecDetail.push_back(stDetail);
#endif

    yl::string strDuration;

    GetCallLogDuration(atoi(pCallLog->m_strDuration.c_str()), strDuration);

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_DURATION);
    stDetail.stAttr.m_strValue = strDuration;
    m_vecDetail.push_back(stDetail);

    if (LocalLog_DetailedEnable()
            && (CR_NONE != pCallLog->m_eRelation || !pCallLog->m_strRelationNum.empty()))
    {
        stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_RELATION);
        stDetail.stAttr.m_strValue = "";
        GetDisplayNameByRelation(pCallLog->m_eRelation, pCallLog->m_strRelationNum, stDetail.stAttr.m_strValue);
        m_vecDetail.push_back(stDetail);
    }
}

void CLocalCallLogControler::GetDisplayNameByRelation(CALL_RELATION& eRelation,
        yl::string& strRelationNum, yl::string& strOutput)
{
    switch (eRelation)
    {
    case CR_BUSY:
        {
            strOutput = _UILOGIC_LANG_TRAN(TRID_BUSY);
        }
        break;
    case CR_REJECTED:
        {
            strOutput = _UILOGIC_LANG_TRAN(TRID_REJECTED);
        }
        break;
    case CR_TRANS_TO:
        {
            strOutput = _UILOGIC_LANG_TRAN(TRID_TRANSFER_TO);
        }
        break;
    case CR_FWD_TO:
        {
            strOutput = _UILOGIC_LANG_TRAN(TRID_FORWARD_SELECTIVE_FORWARD_TO);
        }
        break;
    case CR_NONE:
        {
            strOutput = _UILOGIC_LANG_TRAN(TRID_NA);
            return;
        }
        break;
    default:
        break;
    }

    if (!strRelationNum.empty())
    {
        if (!strOutput.empty())
        {
            strOutput.append(" ");
        }
        strOutput.append(strRelationNum);
    }
}

int CLocalCallLogControler::GetDialAction()
{
    int nDialAction = DIR_DIAL_ACTION_DIGIT_MAP_HISTORY;

#if IF_BUG_27045
    CALL_TYPE nType = pRecord->GetLogType();

    //未接和已接来电才需要加区号
    if (CT_MISSCALL == nType || CT_INCOMING == nType)
    {
        nDialAction |= DIR_DIAL_ACTION_ADD_AREA_CODE;
    }
#endif

    return nDialAction;
}

bool CLocalCallLogControler::DialContact(int nContactId, int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
    LocalLogItemRecord * pRecord = GetLocalCallLog(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetLocalCallLog fail DialContact(ContactId:%d)", nContactId);
        return false;
    }

    if (AUTO_ACCOUNT_IDENTIFY == nAccountId)
    {
#ifdef IF_FEATURE_PSTN
        if (pRecord->m_strLocalSipName.find("PSTN") != yl::string::npos)
        {
            nAccountId = acc_GetAccountIDByPstnName(pRecord->m_strLocalSipName.c_str());
        }
        else
        {
            nAccountId = acc_GetAccountIDByName(pRecord->m_strLocalSipName.c_str(),
                                                pRecord->m_strLocalSipServer.c_str());
        }
#else
        nAccountId = acc_GetAccountIDByName(pRecord->m_strLocalSipName.c_str(),
                                            pRecord->m_strLocalSipServer.c_str());
#endif
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

#if IF_BUG_27560
    if (pRecord->m_bHideSipName)
    {
        actURI_SetHideTalkInfo(true);
    }
#endif

    if (IsEnterNumberChoose(pRecord->m_strRelationNum))
    {
        OnEnterNumberChoose(nAccountId, strNumber, pRecord->GetDisplayName(), pRecord->m_strRelationNum);
        m_bChooseNumberToSend = true;
    }
    else
    {
        g_DirUIAdapter.Dial(nAccountId, pRecord->GetDisplayName(), strNumber, GetDialAction());
    }

    return true;
}

bool CLocalCallLogControler::IsEnterNumberChoose(const yl::string& strRelationNum)
{
    if (DIR_UI_STATE_DETAIL == GetState()
            && !strRelationNum.empty()
            && LocalLog_DetailedEnable())
    {
        return true;
    }

    return false;
}

void CLocalCallLogControler::OnEnterNumberChoose(int nAccountId,
        const yl::string& strNumber, const yl::string& strName,
        const yl::string& strRelationNum)
{
    LIST_ATTRIBUTE lsNumber;
    ST_CONTACT_ATTRIBUTE stAttr;
    stAttr.m_strName = TRID_NUMBER;
    stAttr.m_strValue = strNumber;
    lsNumber.push_back(stAttr);

    stAttr.m_strName = TRID_RELATION;
    stAttr.m_strValue = strRelationNum;
    lsNumber.push_back(stAttr);

    g_DirUIAdapter.Dial(nAccountId, strName, lsNumber, GetDialAction());
}

bool CLocalCallLogControler::OnNumberChooseResult(int nAccountId, const yl::string& strNumber)
{
    if (DIR_UI_STATE_CHOOSE_NUMBER != GetState())
    {
        return false;
    }

    if (m_bChooseNumberToSend)
    {
        SmartPtr pDetail = Dir_GetDetailById(m_nContactId, m_eLogicType);
        LocalLogItemRecord * pRecord = GetLocalCallLog(pDetail);
        if (NULL == pRecord)
        {
            DIRUI_ERR("GetLocalCallLog fail DialContact(ContactId:%d)", m_nContactId);
            return false;
        }

#if IF_BUG_27560
        if (pRecord->m_bHideSipName)
        {
            actURI_SetHideTalkInfo(true);
        }
#endif

        yl::string strDisp = "";
        if (pRecord->m_strRelationNum != strNumber)
        {
            strDisp = pRecord->GetDisplayName();
        }

        g_DirUIAdapter.Dial(nAccountId, strDisp, strNumber, GetDialAction());
    }
    else
    {
        talklogic_EnterDialUI(strNumber, nAccountId);
    }

    return true;
}

#ifdef DIR_SUPPORT_SWITCH_DETAIL
bool CLocalCallLogControler::GetContactImage(int nId, yl::string & strImage) const
{
    SmartPtr pDetail = Dir_GetDetailById(nId, m_eLogicType);
    LocalLogItemRecord * pRecord = GetLocalCallLog(pDetail);

    if (NULL == pRecord)
    {
        return false;
    }

    strImage = pRecord->GetPhoto();

    if (!strImage.empty())
    {
        res_toRealPath(strImage, PHFILE_TYPE_CONTACT_IMAGE);
    }

    return true;
}
#endif
