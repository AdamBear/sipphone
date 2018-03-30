#include "talkhelp_inc.h"

IMPLEMENT_GETINSTANCE(CCallWaiting)

CCallWaiting::CCallWaiting() : m_bEnableCallwait(false)
{
}

CCallWaiting::~CCallWaiting()
{
}

void CCallWaiting::UpdateAndSendCode()
{
    //更新状态并发送CallWait Code
    bool bEnableCallwait = (configParser_GetConfigInt(kszCallWaitingSwitch) != 0);
    yl::string strOnCode = configParser_GetConfigString(kszCallWaitingOnCode);
    yl::string strOffCode = configParser_GetConfigString(kszCallWaitingOffCode);

    bool bChange = false;
    //状态改变才发送Code
    if (m_bEnableCallwait != bEnableCallwait)
    {
        m_bEnableCallwait = bEnableCallwait;
        m_objCallWaitCodeInfo.SetSendFlag(m_bEnableCallwait);
        bChange = true;
    }

    bChange = m_objCallWaitCodeInfo.SetOnCode(strOnCode.c_str()) || bChange;
    bChange = m_objCallWaitCodeInfo.SetOffCode(strOffCode.c_str()) || bChange;

    if (bChange)
    {
        //状态改变才发送URL
        if (m_bEnableCallwait)
        {
#if IF_FEATURE_EVENT_MODULE
            event_SendEvent(EVENT_OPEN_CALLWAIT);
#else
            actURL_SendActionURL(ACTION_URL_OPEN_CALLWAIT);
#endif
        }
        else
        {
#if IF_FEATURE_EVENT_MODULE
            event_SendEvent(EVENT_CLOSE_CALLWAIT);
#else
            actURL_SendActionURL(ACTION_URL_CLOSE_CALLWAIT);
#endif
        }

        //状态改变才发送Code
        SendCallWaitingCode();
    }
}

//根据状态发送OnCode或者OffCode
void CCallWaiting::SendCallWaitingCode(int iLineId)
{
    if (-1 == iLineId)
    {
        iLineId = acc_GetDefaultAccount();
    }

    if (m_bEnableCallwait)
    {
        feature_SendOnCode(iLineId, m_objCallWaitCodeInfo);
    }
    else
    {
        feature_SendOffCode(iLineId, m_objCallWaitCodeInfo);
    }
}

bool CCallWaiting::IsEnableCallWait()
{
#ifdef IF_FEATURE_BROADSOFT_CALL_WAITING
    // 开启服务器Callwaiting，不管本地callwaiting
    if (configParser_GetConfigInt(kszBroadsoftActive) == 1
            && configParser_GetConfigInt(kszCallWaitingMode) == 1)
    {
        return true;
    }
#endif

    return m_bEnableCallwait;
}
