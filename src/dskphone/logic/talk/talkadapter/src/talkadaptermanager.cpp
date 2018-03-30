#include "talkadapter_inc.h"
#include "talklogic/src/autoredialmanager.h"
#include "voice/include/modvoice.h"
#include "talk/talkdsskey/include/dsskeybindmanager.h"
#include "talk/talklistener/include/modtalklistener.h"

static LPCSTR GetActionName(TALK_ACTION_TYPE e)
{
    switch (e)
    {
        CASE_VALE_RET(TALK_ACTION_NONE);
        CASE_VALE_RET(TALK_ACTION_KEYPRESS);
        CASE_VALE_RET(TALK_ACTION_TEXT_CHANGE);
        CASE_VALE_RET(TALK_ACTION_SEND);
        CASE_VALE_RET(TALK_ACTION_DPICKUP);
        CASE_VALE_RET(TALK_ACTION_GPICKUP);
        CASE_VALE_RET(TALK_ACTION_CANCEL);
        CASE_VALE_RET(TALK_ACTION_NEWCALL);
        CASE_VALE_RET(TALK_ACTION_ANSWER);
        CASE_VALE_RET(TALK_ACTION_REJECT);
        CASE_VALE_RET(TALK_ACTION_SILENCE);
        CASE_VALE_RET(TALK_ACTION_FWD_TO);
        CASE_VALE_RET(TALK_ACTION_HOLD);
        CASE_VALE_RET(TALK_ACTION_RESUME);
        CASE_VALE_RET(TALK_ACTION_SWAP);
        CASE_VALE_RET(TALK_ACTION_MUTE);
        CASE_VALE_RET(TALK_ACTION_TRAN);
        CASE_VALE_RET(TALK_ACTION_CONF);
        CASE_VALE_RET(TALK_ACTION_SPLIT);
        CASE_VALE_RET(TALK_ACTION_FOCUSE_CHANGE);
        CASE_VALE_RET(TALK_ACTION_SCREEN_CLICK);
        CASE_VALE_RET(TALK_ACTION_VOLUME_CHANGE);
        CASE_VALE_RET(TALK_ACTION_VOLUMEBAR_SHOW);
        CASE_VALE_RET(TALK_ACTION_REDIAL);
        CASE_VALE_RET(TALK_ACTION_GROUP_LISTEN);
        CASE_VALE_RET(TALK_ACTION_HOLD_PUBLIC);
        CASE_VALE_RET(TALK_ACTION_HOLD_PRIVATE);
        CASE_VALE_RET(TALK_ACTION_HOLD_SWITCH);
        CASE_VALE_RET(TALK_ACTION_WND_FOCUSE_CHANGE);
        CASE_VALE_RET(TALK_ACTION_ACCOUNT_CHANGE);
        CASE_VALE_RET(TALK_ACTION_OK);
        CASE_VALE_RET(TALK_ACTION_REMOVE_CONF_PARTY);
        CASE_VALE_RET(TALK_ACTION_CONF_FAR_MUTE);
        CASE_VALE_RET(TALK_ACTION_ENTER_CONF_MANAGER);
        CASE_VALE_RET(TALK_ACTION_EXIT_CONF_MANAGER);
        CASE_VALE_RET(TALK_ACTION_DTMF);
        CASE_VALE_RET(TALK_ACTION_PARK);
        CASE_VALE_RET(TALK_ACTION_GROUP_PARK);
        CASE_VALE_RET(TALK_ACTION_RETRIEVE_PARK);
        CASE_VALE_RET(TALK_ACTION_OEM_FLASH);
        CASE_VALE_RET(TALK_ACTION_SMS);
        CASE_VALE_RET(TALK_ACTION_CALLINFO);
        CASE_VALE_RET(TALK_ACTION_MENU_BACK);
        CASE_VALE_RET(TALK_ACTION_MENU_ENTER);
        CASE_VALE_RET(TALK_ACTION_MENU_AUTO_QUIT);
        CASE_VALE_RET(TALK_ACTION_EXIT_FAILD_INFO);
        CASE_VALE_RET(TALK_ACTION_TRAN_TO);
        CASE_VALE_RET(TALK_ACTION_MUTE_VIDEO);
        CASE_VALE_RET(TALK_ACTION_TO_MOBILE);
        CASE_VALE_RET(TALK_ACTION_TO_SIP);
        CASE_VALE_RET(TALK_ACTION_PREFIX_PARK);
        CASE_VALE_RET(TALK_ACTION_CALL_BACK);
        CASE_VALE_RET(TALK_ACTION_RECORD_START);
        CASE_VALE_RET(TALK_ACTION_RECORD_STOP);
        CASE_VALE_RET(TALK_ACTION_RECORD_PAUSE);
        CASE_VALE_RET(TALK_ACTION_RECORD_RESUME);
        CASE_VALE_RET(TALK_ACTION_RECORD_BACK);
        CASE_VALE_RET(TALK_ACTION_CALL_INITIATION);
        CASE_VALE_RET(TALK_ACTION_CALL_PUSH);
        CASE_VALE_RET(TALK_ACTION_DIAL_ON_HOLD);
        CASE_VALE_RET(TALK_ACTION_OPENDOOR);
        CASE_VALE_RET(TALK_ACTION_CALL_DECLINE);
        CASE_VALE_RET(TALK_ACTION_TRAN_BIND_SPONSOR);
        CASE_VALE_RET(TALK_ACTION_FLASH);
        CASE_VALE_RET(TALK_ACTION_KEY_PAD);
        CASE_VALE_RET(TALK_ACTION_REQUEST_VIDEO);
        CASE_VALE_RET(TALK_ACTION_RESPOND_VIDEO);
        CASE_VALE_RET(TALK_ACTION_VIDEO_SHARING);
        CASE_VALE_RET(TALK_ACTION_CONF_FAR_HOLD);
        CASE_VALE_RET(TALK_ACTION_REMOVE_NET_CONF_PARTY);
        CASE_VALE_RET(TALK_ACTION_RECORD_RESULT);
        CASE_VALE_RET(TALK_ACTION_PLAY_TONE);
        CASE_VALE_RET(TALK_ACTION_CURRENT_CALLOUT);
        CASE_VALE_RET(TALK_ACTION_JOIN);
        CASE_VALE_RET(TALK_ACTION_SPLIT_ALL);
        CASE_VALE_RET(TALK_ACTION_FAR_RESUME);
    default:
        return NULL;
    }
}

#define FOR_LIST(it) \
for (LIST_SESSION::iterator it = m_listSession.begin(); it != m_listSession.end(); ++it)

CTalkAdaptManager::CTalkAdaptManager() : m_iFocusedSessionID(INVALID_SESSION_ID), m_iFocusedSessionIndex(-1)
{
}

bool CTalkAdaptManager::SetFocusSession(int iSessionID, bool bManualTrigger /*= false*/)
{
    if (m_iFocusedSessionID == iSessionID)
    {
        return false;
    }

    AUTO_WND_UPDATE;

    if (bManualTrigger)
    {
        ProcessBeforeSwitchSession();
    }

    int iLastSessionID = m_iFocusedSessionID;
    // 先取消当前焦点
    if (m_iFocusedSessionID != INVALID_SESSION_ID)
    {
        m_iFocusedSessionID = INVALID_SESSION_ID;
        modtklsn_emit(TALK_SIG_SESSION_FOCUS_CHANGE, iLastSessionID);
    }

    if (iSessionID == INVALID_SESSION_ID)
    {
        return false;
    }

    // 将之前的焦点SessionID置给的焦点话路
    Push(iLastSessionID);
    TALK_INFO("focus changed [%d]->[%d] index[%d]", iLastSessionID, iSessionID);

    m_iFocusedSessionID = iSessionID;

    /* 记录上一路焦点 */
    CListenInfo* priv = modtklsn_get_privdata(m_iFocusedSessionID);
    if (priv != NULL)
    {
        priv->iLastFocusSession = iLastSessionID;
    }

    OnSessionSizeChanged();

    /* 通知外部 */
    SignalParams sigp(iSessionID);
    sigp.wParam.bValue = true;
    modtklsn_emit(TALK_SIG_SESSION_FOCUS_CHANGE, sigp);
    // 焦点更改处理
    etl_NotifyApp(false, TALK_MSG_SESSION_FOUCS_CHANGE, m_iFocusedSessionID, 0);

    return true;
}

CSessionProxy CTalkAdaptManager::GetFirstUnhideSession()
{
    CSessionProxy proxy(Pop());

    while (proxy)
    {
        /* 如果进入来电前的焦点话路还存在，则重置为焦点，否则按规则重置焦点 */
        if (IsAllowToSetAsFocused(proxy))
        {
            return proxy;
        }

        proxy = Pop();
    }

    return proxy;
}

CSessionProxy CTalkAdaptManager::GetNextSession(const CSessionProxy& except/* = CSessionProxy()*/)
{
    if (IsEmpty())
    {
        return CSessionProxy();
    }

    // 记录第一个通话话路(hold状态)
    CSessionProxy fistTalk;
    // 在删除焦点Session之后重置新的焦点Session
    FOR_LIST(it)
    {
        CSessionProxy proxy(*it);
        if (proxy == except
                || !IsAllowToSetAsFocused(proxy))
        {
            continue;
        }

        // 1.通话中的界面
        // 2.来电界面
        // 3.拨号界面

        if (proxy.IsInTalking()
                || proxy.IsInherits(ROUTINE_RINGING)
                || proxy.IsInherits(ROUTINE_DIALING))
        {
            return proxy;
        }

        // 4.记录第一个通话话路(hold状态)
        if (proxy.IsTalkSession() && !fistTalk)
        {
            fistTalk = proxy;
        }
    }

    if (fistTalk)
    {
        // 设置属于通话状态的Session
        return fistTalk;
    }

    // 设置第一个session
    return GetFirstUnhideSession();
}

void CTalkAdaptManager::ResetFocusedSession(const CSessionProxy& except/* = CSessionProxy()*/)
{
    SetFocusSession(GetNextSession(except));
}

void CTalkAdaptManager::OnSessionCreate(int iSessionID)
{
    if (iSessionID == INVALID_SESSION_ID)
    {
        return;
    }

    m_listSession.push_back(iSessionID);

    // 单纯的增加不改变index
}

void CTalkAdaptManager::OnSessionDestory(int iSessionID)
{
    if (iSessionID == INVALID_SESSION_ID)
    {
        return;
    }

    LIST_SESSION::iterator it = app::find(m_listSession.begin(), m_listSession.end(), iSessionID);
    if (it != m_listSession.end())
    {
        m_listSession.erase(it);
    }

    Remove(iSessionID);

    // 减少的情况下会改变index
    OnSessionSizeChanged();
}

void CTalkAdaptManager::ResetFocusedByLastSession(int iSessionID)
{
    if (iSessionID == m_iFocusedSessionID)
    {
        CSessionProxy proxy(GetFirstUnhideSession());
        if (proxy)
        {
            SetFocusSession(proxy);
            return;
        }

        /* 没有上一路，当前焦点直接置空 */
        ResetFocusedSession();
    }
}

int CTalkAdaptManager::Pop()
{
    if (m_listStackFocus.size() == 0)
    {
        return INVALID_SESSION_ID;
    }

    LIST_SESSION::iterator it = --m_listStackFocus.end();
    int iSessionID = *it;
    m_listStackFocus.erase(it);
    return iSessionID;
}

void CTalkAdaptManager::Push(int iSessionID)
{
    if (iSessionID == INVALID_SESSION_ID)
    {
        return;
    }

    Remove(iSessionID);
    m_listStackFocus.push_back(iSessionID);
}

void CTalkAdaptManager::Remove(int iSessionID)
{
    if (iSessionID == INVALID_SESSION_ID)
    {
        return;
    }

    LIST_SESSION::iterator it = app::find(m_listStackFocus.begin(), m_listStackFocus.end(), iSessionID);
    if (it != m_listStackFocus.end())
    {
        m_listStackFocus.erase(it);
    }
}

bool CTalkAdaptManager::ProcessBeforeSwitchSession()
{
    CSessionProxy proxy(m_iFocusedSessionID);
    if (!proxy || CanBeSetAsNoFocused(proxy))
    {
        return false;
    }

    proxy.ExitSession();
    return true;
}

int CTalkAdaptManager::GetSessionListSize()
{
    return m_listSession.size();
}

int CTalkAdaptManager::OnSessionSizeChanged()
{
    int iIndex = 0;
    FOR_LIST(it)
    {
        CSessionProxy proxy(*it);

        if (!(proxy == m_iFocusedSessionID))
        {
            /* 非隐藏话路才递增index */
            if (!proxy.IsHide())
            {
                ++iIndex;
            }

            continue;
        }

        TALK_INFO("focus index changed index[%d]->[%d]", m_iFocusedSessionIndex, iIndex);
        m_iFocusedSessionIndex = iIndex;
        break;
    }

    return iIndex;
}

CSessionProxy CTalkAdaptManager::GetSessionByBLAIndex(int iAccountID, int iIndex)
{
    FOR_LIST(it)
    {
        CSessionProxy proxy(*it);
        CCallInfo* pCallInfo = proxy.GetCallInfo();

        if (pCallInfo == NULL || !proxy.IsSingle())
        {
            continue;
        }

        if (pCallInfo->GetAccountID() == iAccountID
                && pCallInfo->m_iBLAIndex == iIndex)
        {
            return proxy;
        }
    }

    return CSessionProxy();
}

bool CTalkAdaptManager::IsBlaIndexUsing(int iIndex)
{
    FOR_LIST(it)
    {
        CSessionProxy proxy(*it);
        CCallInfo* pCallInfo = proxy.GetCallInfo();

        if (pCallInfo == NULL || !proxy.IsSingle())
        {
            continue;
        }

        if (pCallInfo->m_iBLAIndex == iIndex)
        {
            TALK_INFO("IsDraftBlaIndexUsing [%d] using", iIndex);
            return true;
        }
    }

    return false;
}

bool CTalkAdaptManager::CanBeSetAsNoFocused(const CSessionStateProxy& proxy)
{
    // autoredial 继承自 finish，但是却可以设置为焦点，单独处理
    if (proxy.IsInherits(ROUTINE_AUTOREDIAL))
    {
        return true;
    }

    if (proxy.IsInherits(ROUTINE_DIALING)
            || proxy.IsInherits(ROUTINE_FINISHED))
    {
        return false;
    }

    return true;
}

bool CTalkAdaptManager::IsAllowToSetAsFocused(const CSessionStateProxy& proxy)
{
    return proxy && !proxy.IsHide();
}

bool CTalkAdaptManager::ProcessAction(int iSessionID, DataUI2Logic& ActionData)
{
    AUTO_WND_UPDATE;

    // 将Action交给对应的Session处理
    CSessionStateProxy proxy(iSessionID);
    if (!proxy)
    {
        return false;
    }

    SignalParams sigp(proxy);
    sigp.wParam.pValue = &ActionData;

    TALK_INFO("ProcessAction session[%d] action[%d][%s][%d][%d]", iSessionID, ActionData.eAction, GetActionName(ActionData.eAction), ActionData.wActionParam, ActionData.lActionParam);

    if (modtkflt_emit(TALK_SIG_ACTION_EVENT, sigp))
    {
        return true;
    }

    // 交给目标Session处理
    if (ProcessAction(proxy, ActionData))
    {
        return true;
    }

    return false;
}

bool CTalkAdaptManager::ProcessAction(CSessionStateProxy& proxy, DataUI2Logic& ActionData)
{
    CCallInfo* pCallInfo = proxy.GetCallInfo();
    CListenInfo* priv = modtklsn_get_privdata(proxy);
    if (pCallInfo == NULL || priv == NULL)
    {
        return false;
    }

    /* 从Action中剥离出扩展业务 */
    switch (ActionData.eAction)
    {
    case TALK_ACTION_TEXT_CHANGE:
    case TALK_ACTION_SEND:
    case TALK_ACTION_CURRENT_CALLOUT:
    case TALK_ACTION_DPICKUP:
    case TALK_ACTION_RETRIEVE_PARK:
    case TALK_ACTION_TRAN:
    case TALK_ACTION_PARK:
    case TALK_ACTION_GROUP_PARK:
        {
            // UI触发的号码输入不需要刷新
            AUTO_SCOPED_SIG_BLOCK;

            DataDial2Logic* pDialData = (DataDial2Logic*)ActionData.pData;
            if (pDialData != NULL && proxy.IsSingle())
            {
                yl::string strNumber = pDialData->strNumber;
                pCallInfo->SetDialedNumber(strNumber);

                if (TALK_ACTION_DPICKUP == ActionData.eAction
                        && strNumber.empty()
                        && !priv->m_bDPickupDial)
                {
                    // 如果是首次进行Dpickup的选择，则设置标志
                    TALK_INFO("Change to Directed Pickup Mode for Dial Routine!");
                    priv->m_bDPickupDial = true;
                    break;
                }
                else if (ActionData.eAction == TALK_ACTION_CURRENT_CALLOUT)
                {
                    ActionData.eAction = TALK_ACTION_SEND;
                }
            }

            // 继续处理
        }
        break;
    case TALK_ACTION_FOCUSE_CHANGE:
        {
            WPARAM iNewFocus = ActionData.wActionParam;
            if (GetFocusedSessionID() == iNewFocus)
            {
                return false;
            }

            bool bSwitch = ProcessBeforeSwitchSession();
            // 如果焦点Session有切换则更新界面
            return SetFocusSession(iNewFocus) || bSwitch;
        }
        break;
    case TALK_ACTION_KEYPRESS:
    case TALK_ACTION_SCREEN_CLICK:
    case TALK_ACTION_EXIT_FAILD_INFO:
        {
#if IF_FEATURE_KTS
            if (ActionData.eAction == TALK_ACTION_EXIT_FAILD_INFO
                    && !proxy.IsInherits(ROUTINE_RINGING))
            {
                break;
            }

            if (!pCallInfo->m_bAnswered && proxy.IsKTSIgnore())
            {
                int iKey = ActionData.wActionParam;

                if (iKey >= PHONE_KEY_VOLUME_DECREASE
                        && iKey <= PHONE_KEY_VOLUME_INCREASE)
                {
                    pCallInfo->m_bPassiveRingMute = false;
                    pCallInfo->SetRingMute(false);
                }
                // 数字键直接进预拨号界面
                else if (IsDigitKey(iKey))
                {
                    if (!priv->IsCCShow() && !pCallInfo->IsPreForword())
                    {
                        pCallInfo->SetRingMute(true);
                        pCallInfo->m_bPassiveRingMute = true;
                        ActionData.eAction = TALK_ACTION_NEWCALL;
                        return ProcessAction(proxy, ActionData);
                    }
                }
                else if (!pCallInfo->IsRingMute())
                {
                    pCallInfo->SetRingMute(true);
                    pCallInfo->m_bPassiveRingMute = true;
                }

                return true;
            }
#endif
        }
        break;
    case TALK_ACTION_ANSWER:
        {
            proxy = proxy.GetIncomeSession();
            CCallInfo* pCallInfo = proxy.GetCallInfo();
            if (pCallInfo == NULL)
            {
                return false;
            }
            TALK_ANSWER_TYPE eAnwserType = (TALK_ANSWER_TYPE)(ActionData.wActionParam);
            if (eAnwserType == TAT_CHANNEL)
            {
                int iKey = ActionData.lActionParam;
#if IF_FEATURE_KTS
                if (proxy.IsKTSIgnore())
                {
                    // 耳麦不需要进拨号界面
                    if (iKey != PHONE_KEY_HEADSET_CTRL
                            || configParser_GetConfigInt(kszHeadsetCtrlCall) != 0)
                    {
                        ActionData.eAction = TALK_ACTION_NEWCALL;
                        ActionData.wActionParam = ActionData.lActionParam;
                        return ProcessAction(proxy, ActionData);
                    }

                    return true;
                }
                else
#endif
                {
                    if (configParser_GetConfigInt(kszAnswerByHand) == 0)
                    {
                        break;
                    }
                }
            }
        }
        break;
    case TALK_ACTION_REJECT:
        {
            proxy = proxy.GetIncomeSession();
            CCallInfo* pCallInfo = proxy.GetCallInfo();
            if (pCallInfo == NULL)
            {
                return false;
            }

#if IF_FEATURE_KTS
            if (ActionData.wActionParam == TAT_CHANNEL
                    && proxy.IsKTSIgnore())
            {
                return true;
            }
#endif
            bool bRecall = proxy.IsInherits(ROUTINE_RECALL);
#ifdef IF_FEATURE_EMERGENCY
            //如果是紧急号码回呼动作，则过滤x键
            if (bRecall && proxy.IsEmergencyCalled())
            {
                return true;
            }
#endif
            // 主动拒绝才记录
            pCallInfo->SetRelationInfo((bRecall ? CR_NONE : CR_REJECTED));

#if IF_BUG_38956
            if (0 == configParser_GetConfigInt(kszRejectIncomingCallSwitch))
            {
                return false;
            }
#endif

            if (!acc_IsAccountIDValid(pCallInfo->GetAccountID()))
            {
                return false;
            }

            ActionData.wActionParam = proxy.IsCallDeclineEnable() ? RT_NORMAL : RT_DND;
        }
        break;
    case TALK_ACTION_CANCEL:
        {
            if (proxy.IsInherits(ROUTINE_TALKING))
            {

#ifdef IF_FEATURE_EMERGENCY // 判断当前话路是否是紧急呼叫
                //若是 紧急通话，并且配置有开启
                if (proxy.IsEmergencyCalled())
                {
                    //ui 挂断紧急通话时，需要发送标记告知sip
                    modVoip_EmergencyCall_HangupReInvite(pCallInfo->GetAccountID(), pCallInfo->GetCallID());
                    //隐藏话路
                    proxy.SetHide(true);
                    //只操作本地的hold操作，不向服务器发送hold消息。hangdup接口会告知sip端处理hold消息
                    proxy.HoldSession(true, HT_EMENERGY_CALL);
                    return true;
                }
                else
#endif //IF_FEATURE_EMERGENCY
                    // 通道键引发的退出,则判断是否需要转移
                    if (proxy.IsInPreTrans()
                            && configParser_GetConfigInt(kszOnhookAttendTranSwitch) != 0
                            && ActionData.wActionParam == 1
#if IF_BUG_30961
                            && !pCallInfo->m_bTranByBLF
#endif
                       )// 通道按键引起
                    {
                        proxy.TranCurrentSession();
                        return true;
                    }
            }
            else if (proxy.IsInherits(ROUTINE_CONNECTING)
                     && ActionData.wActionParam == 1)
            {
                if (proxy.GetRoutineState() == ROUTINE_RINGBACK)
                {
                    // 通道键引发的退出,则判断是否需要盲转
                    // 如果是Tran呼出,则判断挂机是否盲转
                    if (proxy.GetTranSponsor()
                            && configParser_GetConfigInt(kszOnhookAttendTranSwitch) != 0)
                    {
#if IF_BUG_21754
                        etlSleep(500);
#endif
                        proxy.TranCurrentSession();
                        return true;
                    }
                }

#if IF_BUG_40237
                if (proxy.GetTranSponsor()
                        && configParser_GetConfigInt(kszOnhookAttendTranSwitch) != 0)
                {
                    TALK_INFO("On hook Wait For Transfer");
                    proxy.SetHide(true);
                    pCallInfo->m_bTransferAfterRingBack = true;
                    return true;
                }
#endif
            }
            else if (proxy.IsInherits(ROUTINE_RINGING))
            {
                if (proxy.IsInherits(ROUTINE_RECALL)
                        && proxy.IsEmergencyCalled())
                {
                    return true;
                }

                ActionData.wActionParam = proxy.IsCallDeclineEnable() ? RT_NORMAL : RT_DND;
            }
            else if (proxy.IsInherits(ROUTINE_PRETRAN))
            {
                if (ActionData.wActionParam == 1)
                {
                    // 通道键引发的退出，则判断是否需要盲转
                    if (configParser_GetConfigInt(kszOnhookBlindTranSwitch) != 0
                            && !pCallInfo->GetDialText().empty())
                    {
                        ActionData.eAction = TALK_ACTION_TRAN;
                        ActionData.wActionParam = 0;
                    }
                }
            }
        }
        break;
    case TALK_ACTION_WND_FOCUSE_CHANGE:
    case TALK_ACTION_KEY_PAD:
        {
            // 窗口焦点失去后,需要将定时器去掉,避免定时器到,拨号界面突然消失
            if (proxy == m_iFocusedSessionID)
            {
                bool bFocus = (bool)ActionData.wActionParam;
                if (ActionData.eAction == TALK_ACTION_KEY_PAD)
                {
                    bFocus = !bFocus;
                }
            }
        }
        break;
    case TALK_ACTION_GPICKUP:
        {
            int iAccontID = pCallInfo->GetAccountID();
            if (!acc_IsAccountIDValid(iAccontID))
            {
                break;
            }

            yl::string strDialedNum = acc_GetPickupCodeByType(iAccontID, PT_GROUP);

            // 设置Pickup呼出标志
            pCallInfo->m_eTalkCallOutType = TCT_PICKUP;

            ActionData.eAction = TALK_ACTION_SEND;
            ActionData.wActionParam = COT_DSSKEY;
            DataDial2Logic data;
            data.strNumber = strDialedNum;
            ActionData.pData = &data;
        }
        break;
    case TALK_ACTION_MENU_BACK:
    case TALK_ACTION_MENU_ENTER:
    case TALK_ACTION_MENU_AUTO_QUIT:
        {
            // 旧的不处理
            return true;
        }
        break;
    case TALK_ACTION_SWAP:
        {
            return proxy.SwapToAnother();
        }
        break;
    case TALK_ACTION_SMS:
        {
#ifdef IF_BT_SUPPORT_PHONE
            if (pCallInfo->IsBtTalk() || !proxy.IsInherits(ROUTINE_DIALING))
            {
                return false;
            }
#endif

            return AdapterBox_EnterConnectVoiceMailPage(pCallInfo->GetAccountID());
        }
        break;
    case TALK_ACTION_ACCOUNT_CHANGE:
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
    case TALK_ACTION_CALL_INITIATION:
        {
            if (TALK_ACTION_CALL_INITIATION == ActionData.eAction)
            {
                // 为1 表示设置Call Initiation标志，为0表示取消标志
                pCallInfo->m_eTalkCallOutType = ActionData.wActionParam == 1 ? TCT_CALL_INITIATION : TCT_NORMAL;

                if (ActionData.wActionParam == 0)
                {
                    return true;
                }
            }
            else if (TALK_ACTION_ACCOUNT_CHANGE == ActionData.eAction)
#else
        {
#endif
            {
                pCallInfo->SetAccountID(ActionData.wActionParam);

                if (ActionData.lActionParam == 0)
                {
                    return true;
                }
            }

            yl::string strDialText, strDisplayname;
            // 如果当前已存在号码，则直接呼出
            if (!AdapterBox_TalkUIGetInputContent(strDialText, strDisplayname)
                    || strDialText.empty())
            {
                return false;
            }

            pCallInfo->SetDialedNumber(strDialText);

            // 触发呼出
            ActionData.eAction = TALK_ACTION_SEND;
            ActionData.wActionParam = COT_DSSKEY;
            DataDial2Logic data;
            data.strNumber = strDialText;
            data.strName = strDisplayname;

            ActionData.pData = &data;
            pCallInfo->m_tRemoteInfo.sDisplayName = strDisplayname;
            pCallInfo->m_tRemoteInfo.sSIPName = strDialText;
        }
        break;
    case TALK_ACTION_OK:
        {
            if (!proxy.IsTalkSession())
            {
                break;
            }

            switch (configParser_GetConfigInt(kszOKMapStrategyInTalking))
            {
            case CALL_OK_INFO:
                {
                    modVoip_SendOEMFlash(pCallInfo->GetAccountID(), pCallInfo->GetCallID());
                }
                break;
            case CALL_OK_STATUS:
                {
                    AdapterBox_SettingUIEnterPage(MENU_PAGE_STATUS);
                }
                break;
            case CALL_OK_UNHOLD:
                {
                    //只处理解除hold
                    if (proxy.IsHold())
                    {
                        proxy.HoldSession(false);
                    }
                }
                break;
            default:
                break;
            }
        }
        break;
    case TALK_ACTION_OEM_FLASH:
        {
            if (proxy.IsTalkSession())
            {
                return modVoip_SendOEMFlash(pCallInfo->GetAccountID(), pCallInfo->GetCallID());
            }
        }
        break;
    case TALK_ACTION_NEWCALL:
        {
            if (proxy.IsInherits(ROUTINE_FINISHED))
            {
                // 因账号错误引发的通化错误不允许创建拨号
                if (pCallInfo->m_errorInfo.iReason == TALK_ERROR_CALLOUT_IA)
                {
                    return false;
                }
            }
            else if (pCallInfo->BeMulticastPage())
            {
                return false;
            }

            if (!_DsskeyBindManager.IsAllowNewCallByAccount(pCallInfo->GetAccountID()))
            {
                return false;
            }

            ROUTINE_STATE eRoutine = IsDigitKey(ActionData.wActionParam) ? ROUTINE_PREDIAL : ROUTINE_DIALING;
            CSessionProxy newCall = talklogic_CreateNewRoutine(eRoutine, proxy);
            pCallInfo = newCall.GetCallInfo();
            if (pCallInfo != NULL && eRoutine == ROUTINE_PREDIAL)
            {
                yl::string strNumber = GetDigitKeyString(ActionData.wActionParam);
                pCallInfo->SetDialedNumber(strNumber);
                pCallInfo->m_iDigitKey = ActionData.wActionParam;
            }
            return true;
        }
        break;
    case TALK_ACTION_CALL_DECLINE:
        {
            // call decline 未开, 则不响应事件
            if (!proxy.IsCallDeclineEnable())
            {
                return false;
            }

            ActionData.eAction = TALK_ACTION_REJECT;
            ActionData.wActionParam = ActionData.wActionParam ? RT_NORMAL : RT_DND;
        }
        break;
#if IF_FEATURE_DIAL_ON_HOLD
    case TALK_ACTION_DIAL_ON_HOLD:
        {
            if (proxy.IsAllowNewDail())
            {
                return false;
            }

            ROUTINE_STATE eRoutine = ROUTINE_PREDIAL;
#ifdef IF_FEATURE_PSTN
            if (pCallInfo->GetCallLineType() == CALL_LINE_PSTN)
            {
                eRoutine = ROUTINE_DIALING;
            }
#endif // IF_FEATURE_PSTN

            CSessionProxy newCall = talklogic_CreateNewRoutine(eRoutine, proxy);
            pCallInfo = newCall.GetCallInfo();
            if (pCallInfo != NULL)
            {
                yl::string strNumber = GetDigitKeyString(ActionData.wActionParam);
                pCallInfo->SetDialedNumber(strNumber);
                pCallInfo->m_iDigitKey = ActionData.wActionParam;
            }

            return true;
        }
        break;
#endif
    case TALK_ACTION_SILENCE:
    case TALK_ACTION_VOLUME_CHANGE:
        {
            pCallInfo->m_bPassiveRingMute = false;
            pCallInfo->SetRingMute(ActionData.eAction == TALK_ACTION_SILENCE);
        }
        break;
    case TALK_ACTION_ENTER_CONF_MANAGER:
    case TALK_ACTION_EXIT_CONF_MANAGER:
        {
            proxy.SetConfManager(ActionData.eAction == TALK_ACTION_ENTER_CONF_MANAGER);
        }
        break;
#if IF_BUG_27796
    case TALK_ACTION_PREFIX_PARK:
        {
            yl::string strNumber = configParser_GetConfigString(kszZultysParkValue);

            if (proxy.IsTalkSession())
            {
                RemoveInvalidChar(strNumber);
                strNumber = commonAPI_FormatString("sip:%s@%s", strNumber.c_str(), pCallInfo->m_tLocalInfo.sServerName.c_str());
                proxy.TransferForCallpark(strNumber);
            }
            else
            {
                ActionData.eAction = TALK_ACTION_NEWCALL;
                if (ProcessAction(proxy, ActionData))
                {
                    talklogic_FillCallNum(strNumber);
                }
            }

            return true;
        }
        break;
#endif  //IF_BUG_27796
#if IF_BUG_23548
    case TALK_ACTION_CALL_BACK:
        {
            yl::string strName = "Call Back";
            int nAccountId = pCallInfo->GetAccountID();
            yl::string strNumber = configParser_GetConfigString(kszInitiateCallback);
            if (strNumber.empty())
            {
                break;
            }

            // 退出当前通话
            ActionData.eAction = TALK_ACTION_CANCEL;
            ProcessAction(proxy, ActionData);
            // 拨出特征码
            return talklogic_CallOut(strNumber, strName, nAccountId);
        }
        break;
#endif
#ifdef IF_FEATURE_PSTN
    case TALK_ACTION_FLASH:
        {
            return pstnadapter_Flash(acc_GetDeviceIDByAccountID(pCallInfo->GetAccountID()));
        }
        break;
#endif // IF_FEATURE_PSTN

    default:
        break;
    }

    return talklogic_UIEvent(proxy, ActionData);
}

