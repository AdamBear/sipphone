#if IF_FEATURE_METASWITCH_CALLLOG
#include "mtswcalllogcontroler.h"
#include "diruiadapter.h"
#include "metaswitch/mtswcontrol/include/modmtsw.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "account/include/modaccount.h"
#include "diruiadapter.h"
#include "taskaction/common.h"
#include "translateiddefine.h"
#include "uilogiccommon/include/uilogiclanghelper.h"

CMTSWCallLogControler::CMTSWCallLogControler()
    : CCommonDirCtrl(CALL_LOG_TYPE_MTSW, MTSW_LOG)
    , m_bDownloading(false)
    , m_bIsNeedDownLoad(true)
{

}

CMTSWCallLogControler::~CMTSWCallLogControler()
{
}

CMTSWCallLogControler* CMTSWCallLogControler::GetControler(CDirectoryControlerPtr& pControler)
{
    if (pControler.IsEmpty() || CALL_LOG_TYPE_MTSW != pControler->GetType())
    {
        return NULL;
    }

    return (CMTSWCallLogControler *)pControler.Get();
}

MTSWCallLogRecord* CMTSWCallLogControler::GetMTSWCallLog(SmartPtr& pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_MTSW_CALLLOG != pDetail->GetType())
    {
        return NULL;
    }

    return (MTSWCallLogRecord *)pDetail.Get();
}

void CMTSWCallLogControler::SetGroupId(int nGroupId)
{
#ifndef _WIN32
#warning Dir_ClearNotifyList
#endif
#if !DIR_UI_SUPPORT_LOAD_DATA_LIST
    //  Dir_ClearNotifyList(nGroupId);
#endif

    CCommonDirCtrl::SetGroupId(nGroupId);
}

int CMTSWCallLogControler::GetBrowseItemAmount()
{
    int nItemAmount = GetGroupChildrenAmount(m_nGroupId, DIR_TYPE_MTSW_LOG_GROUP);

    if (DIR_INVALID_AMOUNT != nItemAmount)
    {
        return nItemAmount;
    }

    return CCommonDirCtrl::GetBrowseItemAmount();
}

bool CMTSWCallLogControler::PreLoadContact(int nGroupId)
{
    if (!CCommonDirCtrl::PreLoadContact(nGroupId))
    {
        return false;
    }

    if (m_bDownloading || !m_bIsNeedDownLoad || talklogic_SessionExist())
    {
        DIR_INFO("do not download metaswitch call log bDownloading:[%d], IsNeedDownLoad[%d]", m_bDownloading, m_bIsNeedDownLoad);
        return true;
    }

    //若没有登录Commportal则跳转登录界面
    if (!MTSW_IsLogined())
    {
        DIR_INFO("MTSW_IsLogined fail, enter login page");
        IMTSWCallLogUIHelper* pMTSWUIHelper = g_DirUIAdapter.GetMTSWCallogUIHelper(m_pUIHelper);
        if (NULL != pMTSWUIHelper)
        {
            pMTSWUIHelper->EnterMTSWLoginPage();
        }
        return true;
    }

    //重新下载历史记录
    int nRetCode = MTSWLog_ReqLoadCallLog();

    if (DIR_RET_SUCC != nRetCode)
    {
        CCommonDirCtrl::ShowErrorMsg(nRetCode);
        return true;
    }

    m_bDownloading = true;
    g_DirUIAdapter.ShowWaiting(TRID_NOTE, TRID_LOADING_PLEASE_WAIT, DIR_ACTION_MTSW_DOWNLOAD);
    return true;
}

bool CMTSWCallLogControler::DialCallLog(int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    return DialContact(m_nContactId, nAccountId);
}

bool CMTSWCallLogControler::EditCallLog()
{
    SmartPtr pDetail = Dir_GetDetailById(m_nContactId, m_eLogicType);
    MTSWCallLogRecord* pRecord = GetMTSWCallLog(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetMTSWCallLog(ContactId:%d) fail EditCallLog", m_nContactId);
        return false;
    }

    talklogic_EnterDialUI(pRecord->GetNumber(), acc_GetDefaultAccount());

    return true;
}

void CMTSWCallLogControler::OnCopyContact(int nAction, int nId)
{
    SmartPtr pDetail = Dir_GetDetailById(nId, m_eLogicType);
    MTSWCallLogRecord* pMTSWCallLog = GetMTSWCallLog(pDetail);
    if (NULL == pMTSWCallLog)
    {
        return;
    }

    LIST_ATTRIBUTE lsAttr;
    ST_CONTACT_ATTRIBUTE stAttr;

    if (DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS == nAction)
    {
        stAttr.m_strName = DIR_ATTR_NAME;
        stAttr.m_strValue = pMTSWCallLog->GetDisplayName();
        lsAttr.push_back(stAttr);

        stAttr.m_strName = DIR_ATTR_NUMBER;
        stAttr.m_strValue = pMTSWCallLog->GetNumber();
        lsAttr.push_back(stAttr);

        SetAttributeList(m_nType, lsAttr);
        g_DirUIAdapter.EnterUI(DIRECTORY_TYPE_LOCAL, DIR_UI_STATE_ADDITION,
                               DIR_ACTION_LOCAL_COPY_TO_CONTACTS, knInvalidRId);
    }
    else if (DIR_ACTION_MTSW_ENTER_ADD_CONTACTLISTL == nAction)
    {
        stAttr.m_strName = DIR_ATTR_MTSW_FIRST_NAME;
        stAttr.m_strValue = pMTSWCallLog->GetDisplayName();
        lsAttr.push_back(stAttr);
        stAttr.m_strName = DIR_ATTR_MTSW_HOMENUMBER;
        stAttr.m_strValue = pMTSWCallLog->GetNumber();
        lsAttr.push_back(stAttr);
        stAttr.m_strName = DIR_ATTR_MTSW_WORKNUMBER;
        lsAttr.push_back(stAttr);
        stAttr.m_strName = DIR_ATTR_MTSW_MOBILENUMBER;
        lsAttr.push_back(stAttr);
        SetAttributeList(m_nType, lsAttr);
        g_DirUIAdapter.EnterUI(DIRECTORY_TYPE_METASWICTH, DIR_UI_STATE_ADDITION,
                               DIR_ACTION_MTSW_ADD_TO_CONTACTLIST, knInvalidRId);
    }
}

bool CMTSWCallLogControler::OnAction(int nAction, int nDataIndex, int nId)
{
    DIR_INFO("CMTSWCallLogControler::OnAction(Action:%d Index:%d Id:%d)", nAction, nDataIndex, nId);

    if (CCommonDirCtrl::OnAction(nAction, nDataIndex, nId))
    {
        return true;
    }

    if (DIR_ACTION_LCLOG_EDIT == nAction)
    {
        EditCallLog();
    }
    else if (DIR_ACTION_LOCAL_ENTER_COPY_TO_CONTACTS == nAction
             || DIR_ACTION_MTSW_ENTER_ADD_CONTACTLISTL == nAction)
    {
        OnCopyContact(nAction, nId);
    }
    else
    {
        return false;
    }

    return true;
}

bool CMTSWCallLogControler::ExecAction(int nAction, int nDataIndex, int nId)
{
    return false;
}

bool CMTSWCallLogControler::CancelAction(int nAction, int nDataIndex, int nId)
{
    if (DIR_ACTION_MTSW_DOWNLOAD == nAction)
    {
        m_bDownloading = false;
        m_bIsNeedDownLoad = true;
        RefreshUI(DIR_UI_REFRESH_TYPE_NONE, DIR_UI_REFRESH_PARAM_NONE);
        g_DirUIAdapter.ExitCurrentUI(UI_CALLLOG);
        return true;
    }

    return false;
}

bool CMTSWCallLogControler::DialContact(int nContactId, int nAccountId/* = AUTO_ACCOUNT_IDENTIFY*/)
{
    SmartPtr pDetail = Dir_GetDetailById(nContactId, m_eLogicType);
    MTSWCallLogRecord* pRecord = GetMTSWCallLog(pDetail);

    if (NULL == pRecord)
    {
        DIRUI_ERR("GetMTSWCallLog(ContactId:%d) fail DialContact", nContactId);
        return false;
    }

    return g_DirUIAdapter.Dial(nAccountId, pRecord->GetDisplayName(), pRecord->GetNumber(), DIR_DIAL_ACTION_DIGIT_MAP_HISTORY);
}

LRESULT CMTSWCallLogControler::OnMessage(msgObject& refMessage)
{
    if (refMessage.message < METASWITCH_CALLLOG_MESSAGE_BEGIN
            || refMessage.message > METASWITCH_CALLLOG_MESSAGE_END)
    {
        return FALSE;
    }

    DIRUI_INFO("CMTSWCallLogControler::OnMessage(Msg:%d wp:%d lp:%d)",
               refMessage.message, refMessage.wParam, refMessage.lParam);

    if (METASWITCH_CALLLOG_DOWNLOADED_RESULT == refMessage.message)
    {
        if (m_bDownloading && !MTSWLog_IsDownLoading())
        {
            g_DirUIAdapter.EndWaiting();
            m_bDownloading = false;
        }

        if (1 == refMessage.lParam)
        {
            RefreshUI(DIR_UI_REFRESH_TYPE_ALL, DIR_UI_REFRESH_PARAM_ALL);
            m_bIsNeedDownLoad = false;
        }
        else if (refMessage.lParam == NS_TA::TA_FILE_EXCEED_MAX_SIZE)
        {
            g_DirUIAdapter.ShowMsgBox(TRID_DOWNLOADFILE_OUT_OF_MEMORY);
        }
        else
        {
            g_DirUIAdapter.ShowMsgBox(TRID_LOAD_FAILED);
        }

        return TRUE;
    }
    return FALSE;
}

void CMTSWCallLogControler::PrepareDetailData()
{
    m_vecDetail.clear();

    SmartPtr pDetail = GetCurrentDetail();
    MTSWCallLogRecord* pCallLog = CMTSWCallLogControler::GetMTSWCallLog(pDetail);
    if (NULL == pCallLog)
    {
        return;
    }

    yl::string strDisplayName = pCallLog->GetDisplayName();
    yl::string strNumber = pCallLog->GetNumber();

    if (strDisplayName.empty() && strNumber.empty())
    {
        strDisplayName = _UILOGIC_LANG_TRAN(TRID_ANONYMOUS);
        strNumber = _UILOGIC_LANG_TRAN(TRID_ANONYMOUS);
    }

    ST_DETAIL_DATA stDetail;
    stDetail.nType = DETAIL_DATA_TYPE_NONE;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_NAME);
    stDetail.stAttr.m_strValue = strDisplayName;
    m_vecDetail.push_back(stDetail);

    stDetail.nType = DETAIL_DATA_TYPE_NUMBER;
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_NUMBER);
    stDetail.stAttr.m_strValue = strNumber;
    m_vecDetail.push_back(stDetail);

    stDetail.nType = DETAIL_DATA_TYPE_NONE;

    time_t tmTick = TransMTSWCallLogTime(pCallLog->m_strBornTick.c_str(), true);
    stDetail.stAttr.m_strValue = g_DirUIAdapter.Time2DetailString(tmTick, false);
    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_TIME);
    m_vecDetail.push_back(stDetail);

    stDetail.stAttr.m_strName = _UILOGIC_LANG_TRAN(TRID_DURATION);
    stDetail.stAttr.m_strValue = pCallLog->m_strDuration;
    m_vecDetail.push_back(stDetail);
}

bool CMTSWCallLogControler::IsGroupData(SmartPtr& pDetail)
{
    if (pDetail.IsEmpty() || DIR_TYPE_MTSW_LOG_GROUP != pDetail->GetType())
    {
        return false;
    }

    return true;
}

#endif //IF_FEATURE_METASWITCH_CALLLOG
