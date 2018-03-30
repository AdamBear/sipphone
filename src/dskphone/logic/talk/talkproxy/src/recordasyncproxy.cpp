#include "recordasyncproxy.h"
#include "voip/modvoip.h"
#include "dsskey/include/moddsskey.h"

// 对应录音类型是否开启
#define PROXY_RERORD_IS_ON(type)        (m_eRecordType & type)
// 关闭录音
#define PROXY_RERORD_TURN_OFF(type)     (m_eRecordType &= ~type)
// 开启录音
#define PROXY_RERORD_TURN_ON(type)      (m_eRecordType |= type)
// 录音状态反转，勿使用组合状态，三种录音方式目前无法同时兼顾
#define PROXY_RERORD_SWITCH(type)       (PROXY_RERORD_IS_ON(type) ? PROXY_RERORD_TURN_OFF(type) : PROXY_RERORD_TURN_ON(type))


CRecordASyncProxy::CRecordASyncProxy(int iSessionID)
    : CBaseTalkASyncProxy(iSessionID)
{
    m_iSIPRecordKey = -1;
    m_iURLRecordKey = -1;
    m_eRecordType = INF_RECORD_NONE;
    m_bWaitURLRecord = false;
    m_eURLRecordError = URE_NONE;
#if IF_BUG_22457_Record
    m_bWaitSIPRecord = false;
    m_eSIPRecordStatus = SIP_RECORD_STATUS_UNKNOWN;
#endif
}

void CRecordASyncProxy::SetRecord(INF_RECORD_TYPE eRecord)
{
    m_eRecordType |= eRecord;
    SetTimerOnce(1000);
}

bool CRecordASyncProxy::OnRecord(CCallInfo* pCallInfo, int iDsskeyID, DssKey_Type eType)
{
    if (pCallInfo == NULL)
    {
        return false;
    }

    int iAccountID = pCallInfo->GetAccountID();
    int iCallID = pCallInfo->GetCallID();

    TALK_INFO("SetRecord key[%d] type[%d] acc[%d] call[%d]", iDsskeyID, eType, iAccountID, iCallID);
    switch (eType)
    {
    case DT_RECORD:
        {
#if IF_BUG_22457_Record
            if (m_bWaitSIPRecord)
            {
                return false;
            }

            if (acc_IsAccountIDValid(iAccountID))
            {
                if (configParser_GetConfigInt(kszFortinetRecord) != 0)
                {
                    modVoip_StartFortinetRecord(iAccountID, iCallID, !PROXY_RERORD_IS_ON(INF_RECORD_SIP));
                }
                else
                {
                    //根据m_bRecordBySIP的状态给sip发送消息
                    modVoip_StartURLRecord(iAccountID, iCallID, !PROXY_RERORD_IS_ON(INF_RECORD_SIP));
                }
            }

            // 发送消息后置变量
            m_bWaitSIPRecord = true;
            // 如果未录音，则记下按键键值
            if (!PROXY_RERORD_IS_ON(INF_RECORD_SIP))
            {
                m_iSIPRecordKey = iDsskeyID;
            }
#else
            PROXY_RERORD_SWITCH(INF_RECORD_SIP);

            if (acc_IsAccountIDValid(iAccountID))
            {
                //根据m_bRecordBySIP的状态给sip发送消息
                modVoip_StartURLRecord(iAccountID, iCallID, PROXY_RERORD_IS_ON(INF_RECORD_SIP));
            }

            if (iDsskey != INVALID_DSSKEY_ID)
            {
                bool isOn = PROXY_RERORD_IS_ON(INF_RECORD_SIP);
                m_iSIPRecordKey = isOn ? iDsskey : INVALID_DSSKEY_ID;
                SetRecordLight(m_iSIPRecordKey, isOn);
            }
#endif // IF_BUG_22457_Record
            return true;
        }
        break;
    case DT_URLRECORD:
        {
            // 发送消息后置变量
            m_bWaitURLRecord = true;
            // 如果未录音，则记下按键键值
            if (!PROXY_RERORD_IS_ON(INF_RECORD_URL))
            {
                m_iURLRecordKey = iDsskeyID;
            }

            return true;
        }
        break;
    default:
        return false;
    }

    return true;
}

bool CRecordASyncProxy::OnSipRecordCallBack(bool bStart, bool bSuccess)
{
    AUTO_SCOPED_TIMER;
    TALK_DBG("OnSipRecord start[%d] succ[%d]", bStart, bSuccess);

    m_bWaitSIPRecord = false;
    Dsskey_Status eStatus = DS_RECORD_IDLE;

    if (bStart)
    {
        // 开启录音成功
        if (bSuccess)
        {
            PROXY_RERORD_TURN_ON(INF_RECORD_SIP);
            m_eSIPRecordStatus = SIP_RECORD_STATUS_ON_SUCCESS;
            eStatus = DS_RECORD_USING;
        }
        // 开启录音失败
        else
        {
            PROXY_RERORD_TURN_OFF(INF_RECORD_SIP);
            m_eSIPRecordStatus = SIP_RECORD_STATUS_ON_FAILED;
        }
    }
    else
    {
        // 关闭录音成功
        if (bSuccess)
        {
            PROXY_RERORD_TURN_OFF(INF_RECORD_SIP);
            m_eSIPRecordStatus = SIP_RECORD_STATUS_OFF_SUCCESS;
            eStatus = DS_NON;
        }
        // 关闭录音失败
        else
        {
            PROXY_RERORD_TURN_ON(INF_RECORD_SIP);
            m_eSIPRecordStatus = SIP_RECORD_STATUS_OFF_FAILED;
            eStatus = DS_RECORD_USING;
        }
    }

    if (talklogic_GetHostSessionID(m_iSessionID) == INVALID_SESSION_ID)
    {
        dsskey_SetDsskeyStatus(m_iSIPRecordKey, eStatus);
        dsskey_refreshDSSKeyUI(m_iSIPRecordKey);
    }

    if (eStatus != DS_RECORD_USING)
    {
        m_iSIPRecordKey = -1;
    }

    if (!bSuccess)
    {
        SetRecord(INF_RECORD_SIP);
    }

    talklogic_UpdateWnd();
    return true;
}

bool CRecordASyncProxy::OnURLRecordCallBack(int iSessionID, bool bSuccess)
{
    yl::string strContent;
    if (bSuccess)
    {
        xml_document docXmlFileURLRecord;
        if (docXmlFileURLRecord.load_file(szXmlURLRecord))
        {
            //若文件存在，且读取正确
            xml_node nodeContent;
            xml_node nodeURLRecordData = docXmlFileURLRecord.first_child();
            while (nodeURLRecordData)
            {
                if (strstr(nodeURLRecordData.name(), "IPPhoneText") != NULL)
                {
                    nodeContent = nodeURLRecordData.child("Text");
                    if (nodeContent)
                    {
                        strContent = nodeContent.text().get();
                    }
                    break;
                }
                nodeURLRecordData = nodeURLRecordData.next_sibling();
            }
        }

        unlink(szXmlURLRecord);
    }

    TALK_INFO("suc[%d] callid[%d] str[%s]", bSuccess, strContent.c_str());
    m_bWaitURLRecord = false;

    if (strContent.empty() || !bSuccess)
    {
        return false;
    }

    bool bFocus = talklogic_GetHostSessionID(iSessionID) == INVALID_SESSION_ID;
    LPCSTR lpszContent = strContent.c_str();

    bool bStoped = false;
    Dsskey_Status eStatus = DS_NON;
    bool bClear = false;

    // 对录音结果的解析
    if (strstr(lpszContent, "successfully started") != NULL)
    {
        eStatus = DS_RECORD_USING;

        if (!PROXY_RERORD_IS_ON(INF_RECORD_URL))
        {
            PROXY_RERORD_TURN_ON(INF_RECORD_URL);
        }
    }
    else if (strstr(lpszContent, "cannot be stopped") != NULL)
    {
        eStatus = DS_RECORD_USING;

        if (!PROXY_RERORD_IS_ON(INF_RECORD_URL))
        {
            PROXY_RERORD_TURN_ON(INF_RECORD_URL);
        }

        if (bFocus)
        {
            m_eURLRecordError = URE_CANNOT_BE_STOPPED;
            bStoped = true;
        }
    }
    else if (strstr(lpszContent, "successfully stopped") != NULL
             || strstr(lpszContent, "the recording box is full") != NULL
             || strstr(lpszContent, "cannot be started") != NULL
             || strstr(lpszContent, "cannot be recorded") != NULL)
    {
        if (PROXY_RERORD_IS_ON(INF_RECORD_URL))
        {
            PROXY_RERORD_TURN_OFF(INF_RECORD_URL);

            if (bFocus)
            {
                //关掉URL录音对应的灯
                if (strstr(lpszContent, "cannot be started") != NULL)
                {
                    m_eURLRecordError = URE_CANNOT_BE_STARTED;
                }
                else if (strstr(lpszContent, "the recording box is full") != NULL)
                {
                    m_eURLRecordError = URE_FULL;
                }
                else if (strstr(lpszContent, "cannot be recorded") != NULL)
                {
                    m_eURLRecordError = URE_CANNOT_BE_RECORDED;
                }
                else
                {
                    m_eURLRecordError = URE_NONE;
                }

                if (strstr(lpszContent, "successfully stopped") == NULL)
                {
                    bStoped = true;
                }
            }

            bClear = true;
        }
        else if (bFocus)
        {
            if (strstr(lpszContent, "cannot be started") != NULL)
            {
                m_eURLRecordError = URE_CANNOT_BE_STARTED;
            }
            else if (strstr(lpszContent, "the recording box is full") != NULL)
            {
                m_eURLRecordError = URE_FULL;
            }
            else if (strstr(lpszContent, "cannot be recorded") != NULL)
            {
                m_eURLRecordError = URE_CANNOT_BE_RECORDED;
            }
            else
            {
                m_eURLRecordError = URE_NONE;
            }

            if (strstr(lpszContent, "successfully stopped") == NULL)
            {
                bStoped = true;
            }
        }
    }

    if (bFocus)
    {
        dsskey_SetDsskeyStatus(m_iURLRecordKey, eStatus);
        dsskey_refreshDSSKeyUI(m_iURLRecordKey);
    }

    if (bClear)
    {
        m_iURLRecordKey = -1;
    }

    if (bStoped)
    {
        SetRecord(INF_RECORD_URL);
    }

    talklogic_UpdateWnd();
    return true;
}

bool CRecordASyncProxy::IsWaitForResult(DssKey_Type eType)
{
    return (m_bWaitSIPRecord && eType == DT_RECORD) || (m_bWaitURLRecord && eType == DT_URLRECORD);
}

void CRecordASyncProxy::UpdateRecordStatus(bool bUsing)
{
    if (PROXY_RERORD_IS_ON(INF_RECORD_SIP))
    {
        SetRecordLight(m_iSIPRecordKey, bUsing);
    }

    if (PROXY_RERORD_IS_ON(INF_RECORD_URL))
    {
        SetRecordLight(m_iURLRecordKey, bUsing);
    }
}

void CRecordASyncProxy::SetRecordLight(int iDsskeyID, bool bUsing)
{
    if (iDsskeyID == INVALID_DSSKEY_ID)
    {
        return;
    }

    dsskey_SetDsskeyStatus(iDsskeyID, bUsing ? DS_RECORD_USING : DS_NON);
    dsskey_refreshDSSKeyUI(iDsskeyID);
}

bool CRecordASyncProxy::OnCallBack()
{
    if (PROXY_RERORD_IS_ON(INF_RECORD_URL))
    {
        m_eURLRecordError = URE_NONE;
    }

    if (PROXY_RERORD_IS_ON(INF_RECORD_SIP))
    {
        m_eSIPRecordStatus = SIP_RECORD_STATUS_UNKNOWN;
    }

    if (INF_RECORD_NONE != m_eRecordType)
    {
        talklogic_UpdateWnd();
        return true;
    }

    return false;
}
