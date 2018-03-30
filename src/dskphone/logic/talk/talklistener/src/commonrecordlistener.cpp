#include "talklistener_inc.h"
#include "talk/talkproxy/include/recordasyncproxy.h"

CCommonRecordListener::CCommonRecordListener()
    : LSN_CLS_LOG_NAME
{
    RegETLMsg(SIP_CALL_RECORD_ON_RESULT, SIP_CALL_RECORD_OFF_RESULT);

    REG_SYNC_SIGNAL(TALK_SIG_ACTION_EVENT, CCommonRecordListener::OnUIAction);
    REG_SYNC_SIGNAL(TALK_SIG_GET_DATA_TO_UI, CCommonRecordListener::OnGetDataToUI);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_HOST_CHANGE, CCommonRecordListener::UpdateRecord);
    REG_SYNC_SIGNAL(TALK_SIG_SESSION_PRE_DELETE, CCommonRecordListener::UpdateRecord);
}

bool CCommonRecordListener::OnMessage(msgObject & msg)
{
    CSessionProxy proxy = talklogic_GetSessionIdByCallID(msg.lParam, false);
    sip_reason_info_t* pReasonInfo = (sip_reason_info_t*)msg.GetExtraData();
    if (pReasonInfo == NULL || !proxy)
    {
        return false;
    }

    CListenInfo* priv = GetSessionData(proxy);
    if (priv == NULL)
    {
        return false;
    }

    return priv->m_RecordProxy.OnSipRecordCallBack(msg.message == SIP_CALL_RECORD_ON_RESULT, pReasonInfo->code == 0);
}

void CCommonRecordListener::OnTaskActionCallback(CTaskAction* pTaskAction)
{
    if (pTaskAction == NULL)
    {
        return;
    }

    ASYNC_MAP_ACTION::iterator it = m_mapAction.find(pTaskAction);
    if (it == m_mapAction.end())
    {
        return;
    }

    CListenInfo* priv = GetCallerData(it->second);
    if (priv != NULL)
    {
        priv->m_RecordProxy.OnURLRecordCallBack(it->second, pTaskAction->IsOperationSucceed());
    }

    m_mapAction.erase(it);
}

CTaskAction* CCommonRecordListener::CreateURLRecord(yl::string& strFullUrl)
{
    if (strFullUrl.empty())
    {
        return NULL;
    }

    LPCSTR lpHttpHeader = "http://";
    if (!strncmp(strFullUrl.c_str(), lpHttpHeader, strlen(lpHttpHeader)) == 0)
    {
        strFullUrl = lpHttpHeader + strFullUrl;
    }

    CNetworkFileAction* pAction = TaskAction_CreateNetworkFileAction(szXmlURLRecord, strFullUrl.c_str());
    if (pAction == NULL)
    {
        return NULL;
    }

    pAction->SetSync(false);
    pAction->SetCallback(this);

    // 开始下载.
    TaskAction_ExecAction(pAction);
    return pAction;
}

void CCommonRecordListener::UpdateRecordStatus(const CSessionStateProxy& proxy, CRecordASyncProxy& record)
{
    // 如果当前话路是通话状态
    if (proxy.IsTalkSession())
    {
        // 如果当前Session是焦点或者顶级host session是焦点，且存在通话录音，则灭灯
        if (proxy.IsFocusSession()
                || proxy.IsTopestHostFocused()
                || talklogic_GetFocusedSessionID() == INVALID_SESSION_ID)
        {
            // 在焦点session析构时，存在一个_SessionManager.GetFocusedSession()为空的间歇期
            record.UpdateRecordStatus(false);
        }
    }
}

DECL_SIGNAL_HANDLER(CCommonRecordListener::OnUIAction)
{
    DataUI2Logic* pDataUI2Logic = (DataUI2Logic*)sigp.wParam.pValue;
    if (pDataUI2Logic == NULL
            || priv == NULL
            || pDataUI2Logic->eAction != TALK_ACTION_RECORD_RESULT)
    {
        return false;
    }

    CSessionProxy proxy(sigp.iSessionID);
    if (proxy.BeMulticastPage())
    {
        return false;
    }

    CCallInfo* pCallInfo = proxy.GetCallInfo();
    if (pCallInfo == NULL)
    {
        return false;
    }

    int iDsskeyID = pDataUI2Logic->wActionParam;
    DssKey_Type eType = (DssKey_Type)pDataUI2Logic->lActionParam;

    CRecordASyncProxy& record = priv->m_RecordProxy;
    if (record.IsWaitForResult(eType))
    {
        SIG_REPLY(false);
    }

    /* 创建异步下载 */
    if (eType == DT_URLRECORD)
    {
        yl::string strFullUrl = dsskey_GetDsskeyValue(iDsskeyID);
        CTaskAction* pAction = CreateURLRecord(strFullUrl);
        if (pAction == NULL)
        {
            return false;
        }

        m_mapAction[pAction] = pCallInfo->GetCallID();
    }

    SIG_REPLY(record.OnRecord(pCallInfo, iDsskeyID, eType));
}

DECL_SIGNAL_HANDLER(CCommonRecordListener::OnGetDataToUI)
{
    CSessionProxy proxy(sigp.iSessionID);
    CCallInfo* pCallInfo = proxy.GetCallInfo();

    if (NULL == pCallInfo || priv == NULL)
    {
        return false;
    }

    CRecordASyncProxy& data = priv->m_RecordProxy;
    pCallInfo->m_eRecordType = data.m_eRecordType;
    pCallInfo->m_eURLRecordError = data.m_eURLRecordError;
    pCallInfo->m_eSIPRecordStatus = data.m_eSIPRecordStatus;
    return true;
}

DECL_SIGNAL_HANDLER(CCommonRecordListener::UpdateRecord)
{
    // 非top不处理
    CSessionStateProxy proxy(sigp.iSessionID);
    if (proxy.GetHost() || !proxy.IsSingle() || priv == NULL)
    {
        return false;
    }

    UpdateRecordStatus(proxy, priv->m_RecordProxy);
    return true;
}

