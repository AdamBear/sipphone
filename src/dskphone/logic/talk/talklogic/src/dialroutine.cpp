#include "talklogic_inc.h"

CDialRoutine::CDialRoutine()
{
}

CDialRoutine::~CDialRoutine()
{
}

// 处理ui发送过来的Action事件
bool CDialRoutine::OnActionEvent(DataUI2Logic* pActionData)
{
    if (pActionData == NULL)
    {
        return false;
    }

    bool bHandled = false;
    switch (pActionData->eAction)
    {
    case TALK_ACTION_SEND:
        {
            DataDial2Logic* pDialData = (DataDial2Logic*)pActionData->pData;
            if (pDialData == NULL)
            {
                return false;
            }

            m_pCallInfo->m_tRemoteInfo.sDisplayName = pDialData->strName;
            m_pCallInfo->m_tRemoteInfo.sSIPName = pDialData->strNumber;

            // 根据号码和名称呼出
            bHandled = CallOut(pDialData->strNumber, pDialData->strName, pActionData->wActionParam, pActionData->lActionParam);
        }
        break;
    case TALK_ACTION_CONF:
        {
            //先判断是否为有效session，防止按conf值为空的linekey造成画面卡住问题。
            //http://bugfree.yealink.com/Bug.php?BugID=116242
            CSingleSession* pSession = _SessionManager.GetRelateSessionByCallID(pActionData->wActionParam);

            if (NULL == pSession
                    || !pSession->IsTalkSession()
                    || NULL == pSession->GetCallInfo())
            {
                break;
            }

            //建立前，会退出拨号界面,但要先记住sponsor
            CSingleSession* pSponsor = m_pCallSession->GetSponsorSession(SPONSOR_CONF);
            if (NULL == pSponsor)
            {
                break;
            }

            m_pCallSession->ExitSession();

            //拨号界面直接建立拉入会议
            pSession->SetSponsorSession(SPONSOR_CONF, pSponsor);

            if (pSession->GetCallInfo()->GetConfType() == CT_BROADSOFT
                    && pSponsor->GetCallInfo()->GetConfType() == CT_BROADSOFT)
            {
                _SessionManager.EstablishNetworkConf(pSession);
            }
            else
            {
                _SessionManager.EstablishLocalConf(pSession);
            }

            bHandled = true;
        }
        break;
    default:
        bHandled = CBaseDialRoutine::OnActionEvent(pActionData);
        break;
    }

    return bHandled;
}

// 呼出当前已拨号码
bool CDialRoutine::CallOut(const yl::string& strNumber, yl::string strName/* = ""*/,
                           int iCallOutTriggerMask/* = COT_DEFAULT*/,
                           bool bAudioOnly/* = true*/)
{
    if (strNumber.empty())
    {
        return false;
    }

    SignalParams sigp(m_pCallSession, strNumber);
    sigp.wParam.iValue = iCallOutTriggerMask;
    sigp.lParam.bValue = bAudioOnly;

    if (modtkflt_emit(TALK_SIG_DIAL_CALLOUT, sigp))
    {
        return false;
    }

    TALK_CALLOUT_RESULT eCallResult = _TalklogicManager.ProcessCallOut(m_pCallSession, m_pCallInfo->GetDialText().c_str(), strName);

    if (eCallResult == TALK_CALLOUT_SUCCESS)
    {
        bool bConfOnePress = (configParser_GetConfigInt(kszConfWithOnePress) == 1);
        if (bConfOnePress && m_pCallInfo->m_bConferenceDirect && (m_pCallInfo->GetConfType() == CT_LOCAL))
        {
            m_pCallInfo->m_bConfWithOnePress = true;
            _SessionManager.EstablishLocalConf(m_pCallSession);
        }
    }
    return true;
}

// 更改账号
void CDialRoutine::ChangeAccount(int iAccountID)
{
    if (!acc_IsAccountAvailable(iAccountID))
    {
        TALK_WARN("Unexpected account id [%d]!", iAccountID);
        return;
    }

    // 使用新的账号
    m_pCallInfo->SetAccountID(iAccountID);
}
