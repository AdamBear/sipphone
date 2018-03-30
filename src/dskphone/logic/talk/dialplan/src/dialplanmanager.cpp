#include "dialplan_inc.h"

IMPLEMENT_GETINSTANCE(CDialPlanManager)

CDialPlanManager::CDialPlanManager()
{
}

CDialPlanManager::~CDialPlanManager()
{
}

bool CDialPlanManager::OnTimer(UINT uTimerID)
{
    if (uTimerID == TIMER_ID(m_iDialNowTimer))
    {
        /* DialNow 定时器触发的HotLine延时呼出 */
        if (m_iCurrentMode == COT_HOTLINE)
        {
            HotLineProcess();
        }
        else
        {
            CallOut(m_strDialText, m_iCurrentMode);
        }
    }
    else if (uTimerID == TIMER_ID(m_iAutoDialTimer))
    {
        CallOut(m_strDialText, COT_AUTODIAL);
    }
    else if (uTimerID == TIMER_ID(m_iAutoQuitTimer))
    {
        int iCallID = talklogic_GetCallIdBySessionID(m_iSessionID);
        talklogic_ExitSession(iCallID);
    }
    else
    {
        return false;
    }

    TALK_INFO("dialplan info[%d][%d][%d] mode[%d] text[%s]", m_iSessionID, m_iDialNowTimer,
              m_eDialState, m_iCurrentMode, m_strDialText.c_str());
    timerKillThreadTimer(uTimerID);
    return true;
}

bool CDialPlanManager::OnTextChanged(int iSessionID, const yl::string& strText)
{
    if (iSessionID != m_iSessionID
            || m_strDialText == strText)
    {
        return false;
    }

    TALK_INFO("OnTextChanged info[%d] text[%s]->[%s]", m_iSessionID, m_strDialText.c_str(), strText.c_str());

    // 更改拨号号码
    m_strDialText = strText;

    do
    {
        /* 预拨号界面AutopCode TestMode 匹配 */
        if (!IsPredialRoutine() || talklogic_GetSessionNum() > 1)
        {
            break;
        }

        int iAutoPMatchResult = Autop_MatchAutoPCodePrefix(m_strDialText);
        if (1 == iAutoPMatchResult)
        {
            talklogic_UIEvent(iSessionID, TALK_ACTION_CANCEL);
        }

        if (iAutoPMatchResult >= 0)
        {
            return true;
        }

        int iTestModeType = commonUnits_GetTestModeType(m_strDialText);
        if (iTestModeType != TST_NONE)
        {
            // 退出拨号界面
            talklogic_UIEvent(iSessionID, TALK_ACTION_CANCEL);
            TALK_INFO("Enter TestMode [%d]", iTestModeType);
            // 进入测试模式
            AdapterBox_EnterTestMode(iTestModeType);
            return true;
        }
    }
    while (0);

    CheckDialNow();
    DialPlanProcess();

    if (m_bFirstDial)
    {
        m_bFirstDial = false;
    }

    return true;
}

bool CDialPlanManager::OnEnterDial(int iSessionID, int iAccountID, const yl::string& strText)
{
    m_bFirstDial = true;
    m_iSessionID = iSessionID;
    m_iAccount = iAccountID;
    m_strDialText = strText;
    m_iCurrentMode = COT_DEFAULT;
    m_iAutoDialTimer = GetAutoDialTimer();
    m_iAutoQuitTimer = GetAutoQuitTimer();
    m_eDialState = talklogic_GetSessionStateBySessionID(iSessionID);
    m_bEnableDialNow = true;

    CheckDialNow();

    TALK_INFO("OnEnterDial info[%d][%d] text[%s] match[%d] timer[%d][%d]", m_eDialState, m_iSessionID,
              m_strDialText.c_str(), m_bEnableDialNow, m_iAutoDialTimer, m_iAutoQuitTimer);
    return DialPlanProcess();
}

bool CDialPlanManager::OnLeaveDial(int iSessionID)
{
    if (iSessionID != m_iSessionID)
    {
        return false;
    }

    TALK_INFO("OnLeaveDial info[%d]", m_iSessionID);
    m_eDialState = SESSION_UNKNOWN;
    m_iSessionID = -1;
    m_strDialText.clear();
    CancelAll();
    return true;
}

bool CDialPlanManager::OnAccountChanged(int iSessionID, int iAccountNew)
{
    if (m_iSessionID != iSessionID)
    {
        return false;
    }

    m_iAccount = iAccountNew;
    return DialPlanProcess();
}

bool CDialPlanManager::DialPlanProcess()
{
    /* 先取消上一次DialNow */
    CancelDialNow();
    m_iDialNowTimer = 0;

    /* Hotline > Digitmap > DialNow */
    if (HotLineProcess()
            || DigitMapProcess()
            || DialNowProcess())
    {
        return true;
    }

    /* 未触发DialNow情况下，激活AutoDial */
    SetAutoDialAndQuitTimer();
    return false;
}

bool CDialPlanManager::CallOut(const yl::string& strText, int iMask/* = 0*/, int iTimer /*= 0*/)
{
    if (strText.empty())
    {
        return false;
    }

    TALK_INFO("DialNow session[%d] acc[%d] timer[%d] mask[%#x] text[%s]",
              m_iSessionID, m_iAccount, iTimer, iMask, strText.c_str());
    /* 时间太小以同步替代异步执行 */
    if (iTimer <= 1)
    {
        CancelAll();

        yl::string strDialNum = strText;
        // 只包含非法字符则不呼出
        RemoveInvalidChar(strDialNum);
        if (strDialNum.empty())
        {
            return false;
        }

        DataDial2Logic DialData;
        DialData.strNumber = strDialNum;

        return talklogic_UIEvent(m_iSessionID, TALK_ACTION_CURRENT_CALLOUT, iMask, 0,
                                 (void*)&DialData);
    }

    m_iDialNowTimer = iTimer;
    m_iCurrentMode = iMask;
    SetDialNow();
    return true;
}

bool CDialPlanManager::IsEnableDigitMap()
{
    if (!m_bEnableDialNow)
    {
        return false;
    }

    /* 总开关优先级最高 */
    if (!IsDialRoutine() || !DigitMap_IsEnable(m_iAccount))
    {
        return false;
    }

    /* Predial 受单独开关控制 */
    return !IsPredialRoutine() || DigitMap_IsConfigEnable(kszDigitMapInPredial, m_iAccount);
}

bool CDialPlanManager::HotLineProcess()
{
    if (!m_bFirstDial || IsPredialRoutine() || IsBTDialRoutine())
    {
        return false;
    }

    /* AuthCall 优先级最高 */
    if (configParser_GetCfgItemIntValue(kszAccountAuthCallSwitch, m_iAccount) != 0)
    {
        yl::string strNum = configParser_GetCfgItemStringValue(kszAccountAuthCallNumber, m_iAccount);
        if (CallOut(strNum, COT_AUTHCALL))
        {
            TALK_INFO("DialNowPreProcess authCall [%s]", strNum.c_str());
            return true;
        }
    }

    /* Hotline流程 */
    if (!m_strDialText.empty() || talklogic_GetSessionNum() > 1)
    {
        /* Hotline号码不为空且当前未输入号码且当前只有一个话路时才呼出HotLine */
        return false;
    }

    int iHotLineTimer = configParser_GetConfigInt(kszHotlineDelay) * 1000 + 1;
    yl::string strHotLineNum = configParser_GetConfigString(kszHotlineNumber);

    if (IsEnableDigitMap())
    {
        int iAccount = m_iAccount;
        int iDMTimer = iHotLineTimer;
        yl::string strDMNum;

        if (DigitMap_IsDelayCallout(m_strDialText, iAccount))
        {
            iDMTimer = DigitMap_GetTimer(m_strDialText, iAccount);
            if (DigitMap_GetFmtUri(strDMNum, iAccount) && !strDMNum.empty())
            {
                /* 处理优先级 二选一 时间越小优先级越高，相同时间下 DigitMap > HotLine */
                if (strHotLineNum.empty() || iHotLineTimer >= iDMTimer)
                {
                    iHotLineTimer = iDMTimer;
                    strHotLineNum = strDMNum;
                }
            }
        }
    }

    TALK_INFO("DialNowPreProcess hotline [%s][%d]", strHotLineNum.c_str(), iHotLineTimer);
    return CallOut(strHotLineNum, COT_HOTLINE, m_iCurrentMode == COT_HOTLINE ? 0 : iHotLineTimer);
}

bool CDialPlanManager::DigitMapProcess()
{
    if (!IsEnableDigitMap())
    {
        return false;
    }

    if (DigitMap_PlayDialTone(m_strDialText, m_iAccount))
    {
        talklogic_UIEvent(m_iSessionID, TALK_ACTION_PLAY_TONE, TONE_DIAL2);
    }

    if (DigitMap_IsDelayCallout(m_strDialText, m_iAccount))
    {
        int iTimer = DigitMap_GetTimer(m_strDialText, m_iAccount);
        return CallOut(m_strDialText, COT_DIGITMAP, iTimer);
    }

    return true;
}

bool CDialPlanManager::DialNowProcess()
{
    if (!m_bEnableDialNow)
    {
        return false;
    }

    if (_DialRuleController.IsDialNowNumber(m_strDialText, m_iAccount))
    {
        int iTimer = configParser_GetConfigInt(kszDialNowDelay) * 1000;
        return CallOut(m_strDialText, COT_DIALNOW, iTimer);
    }

    return false;
}

bool CDialPlanManager::OnRestart(int iSessionID)
{
    if (iSessionID != m_iSessionID)
    {
        return false;
    }

    TALK_DBG("OnRestart session[%d]", iSessionID);

    SetDialNow();
    return true;
}

bool CDialPlanManager::OnFocusChanged(int iSessionID, bool bFocus)
{
    if (iSessionID != m_iSessionID)
    {
        return false;
    }

    TALK_DBG("OnFocusChanged session[%d] focus[%d]", iSessionID, bFocus);

    if (bFocus)
    {
        SetDialNow();
    }
    else
    {
        CancelAll();
    }

    return true;
}

void CDialPlanManager::SetAutoDialAndQuitTimer()
{
    if (IsPredialRoutine())
    {
        bool bAllowAutoDial = !m_strDialText.empty();
        if (bAllowAutoDial)
        {
            /* predial奇葩配置，特殊处理流程 */
            bAllowAutoDial = (configParser_GetConfigInt(kszPredialAutodialSwitch) != 0)
                             && (Autop_MatchAutoPCodePrefix(m_strDialText) < 0)
                             && !commonUnits_IsTestModePrefix(m_strDialText);
        }

        TALK_DBG("SetAutoDialAndQuitTimer %d %s", bAllowAutoDial, m_strDialText.c_str());
        /* Predial AutoDial 和 AutoQuit是互斥的 */
        if (bAllowAutoDial)
        {
            SetAutoDial();
            CancelAutoQuit();
        }
        else
        {
            SetAutoQuit();
            CancelAutoDial();
        }
    }
    else
    {
        SetAutoDial();
        SetAutoQuit();
    }
}

bool CDialPlanManager::IsBTDialRoutine()
{
#ifdef IF_BT_SUPPORT_PHONE
    return talklogic_IsBluetoothSessionBySessionID(m_iSessionID);
#else
    return false;
#endif
}

void CDialPlanManager::CheckDialNow()
{
    CCallInfo* pCallInfo = talklogic_GetCallInfoBySessionID(m_iSessionID);
    if (pCallInfo != NULL && pCallInfo->m_eTalkCallOutType == TCT_PREFIX)
    {
        int iValue = configParser_GetConfigInt(kszDigitMapInPressDesskeyPrefix);
        m_bEnableDialNow = iValue != 0;

        TALK_DBG("CheckDialNow EnableDialNow[%d]", m_bEnableDialNow);
    }
}
