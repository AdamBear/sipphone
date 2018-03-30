#include "autoanswerasyncproxy.h"
#include "talk/talklogic/include/modtalklogic.h"
#include "talk/talklogic/src/talklogic_inc.h"
#include "refuseasyncproxy.h"
#include "talk/talkadapter/include/sessionstateproxy.h"
#include "talk/talkproxy/include/refuseasyncproxy.h"

CAutoAnswerASyncProxy::CAutoAnswerASyncProxy(int iSessionID)
    : CBaseTalkASyncProxy(iSessionID), m_pRefuseAsyncProxy(new CRefuseASyncProxy(iSessionID))
{
}

CAutoAnswerASyncProxy::~CAutoAnswerASyncProxy()
{

}

bool CAutoAnswerASyncProxy::ProcessAutoAnswer(CCallInfo* pCallInfo)
{
    if (pCallInfo == NULL)
    {
        return false;
    }

#ifdef IF_FEATURE_PSTN
    if (!acc_IsPstnAccount(pCallInfo->GetAccountID()))
    {
        return false;
    }
#endif // IF_FEATURE_PSTN

    // Intercom Barge开启时,如果没有被拒绝,则一定要自动应答
    if (pCallInfo->m_bIsIntercom && ProcessIntercom(pCallInfo))
    {
        return true;
    }

    if (ProcessAccountAutoAnswer(pCallInfo))
    {
        return true;
    }

    // 处理Voismart定制的answer-after=X
    if (AutoAnswerBySIPInfo(pCallInfo, &pCallInfo->GetRingUserInfo()))
    {
        return true;
    }

    //未进行自动应答时，执行拒接部分操作
    m_pRefuseAsyncProxy->RefuseBySIPInfo(pCallInfo, &pCallInfo->GetRingUserInfo());
    return false;
}

bool CAutoAnswerASyncProxy::ProcessIntercom(CCallInfo* pCallInfo)
{
    if (configParser_GetConfigInt(kszIntercomAllowance) == 0
            || pCallInfo == NULL || !pCallInfo->m_bIsIntercom)
    {
        return false;
    }
    // 如果已经自动应答则不需要继续处理
    if (IsOn())
    {
        return false;
    }

    // http://192.168.1.99/Bug.php?BugID=9742
    if (configParser_GetConfigInt(kszIntercomBargeSwitch) != 0)
    {
        SetAutoAnswerTimer(pCallInfo);
    }
    else
    {
        if (configParser_GetConfigInt(kszDialAllowIntercomBarge) != 1)
        {
            CSessionStateProxy focusSession;
            if (focusSession.IsConecting() || focusSession.IsDialing())
            {
                TALK_INFO("RingingRoutine In Dial or Connect not allow auto answer");
                return false;
            }
        }
        //Tadiran版本规划将通话被hold时自动应答加入中性
        if (IsAllTalkSessionHoldOrHeld())
        {
            TALK_INFO("RingingRoutine All TalkSession Hold Or Held");
            SetAutoAnswerTimer(pCallInfo);
            return true;
        }
        // Intercom Barge关闭时,如果只有一路通话则AA,否则根据配置判断
        int iSessionCount = talklogic_GetSessionNum();
        if (iSessionCount == 1)
        {
            SetAutoAnswerTimer(pCallInfo);
        }
        else if (iSessionCount > 1)
        {
            pCallInfo->m_bAutoAnswerNow = false;
            SetAutoAnswerTimer(pCallInfo);
        }
        else
        {
            // 第二路不自动接起,去掉原配置kszAASecondIntercom的使用
            return false;
        }
    }
    return true;
}

bool CAutoAnswerASyncProxy::ProcessAccountAutoAnswer(CCallInfo* pCallInfo)
{
    if (pCallInfo == NULL)
    {
        return false;
    }
    CListenInfo* priv = modtklsn_get_privdata(pCallInfo->GetSessionID());
    if (priv == NULL)
    {
        return false;
    }
    // 如果对应的账号开启了AA，且只有一路通话
    //当前不存在其他通话，自动应答，否则自动应答无效
    // 采用新方案 auto answer开启后，第一路通话中，有新来电，第一路通话结束后，第二路通话要能自动接起
    //http://192.168.1.99/Bug.php?BugID=34693
    if ((configParser_GetConfigInt(kszAutoAnswerMultiCall) != 0
            || configParser_GetConfigInt(kszAutoAnswerFirstCallOnly) == 0
            || talklogic_GetSessionNum() == 1))
    {
#ifdef IF_FEATURE_PSTN
        if (acc_IsPstnAccount(pCallInfo->GetAccountID()))
        {
            if (acc_IsAccountAutoAnswerEnable(pCallInfo->GetAccountID()))
            {
                if (talklogic_GetSessionNum() > 1)
                {
                    pCallInfo->m_bAutoAnswerNow = false;
                }

#ifdef IF_FEATURE_SPECIAL_AATONE
                if (configParser_GetConfigInt(kszSpecialAutoAnswerTone) != 0)
                {
                    SetSpecialAutoAnswerTimer(pCallInfo);
                }
                else
#endif
                {
                    SetAutoAnswerTimer(pCallInfo);
                }
                return true;
            }
        }
        else
#endif // IF_FEATURE_PSTN
        {
            if (acc_IsAccountAutoAnswerEnable(pCallInfo->GetAccountID())
                    || TiptelA_IsAutoAnswer()
#if IF_FEATURE_LOOPBACK
                    || (pCallInfo->m_bIsLoopBackCall && configParser_GetConfigInt(kszLoopBackAAMode) == 1)
#endif
               )
            {
                if (talklogic_GetSessionNum() > 1)
                {
                    pCallInfo->m_bAutoAnswerNow = false;
                }

                if (TiptelA_IsAutoAnswer())
                {
                    priv->m_eAAChannel = TiptelA_GetSpeekMode();
                }

#ifdef IF_FEATURE_SPECIAL_AATONE
                if (configParser_GetConfigInt(kszSpecialAutoAnswerTone) != 0)
                {
                    SetSpecialAutoAnswerTimer(pCallInfo);
                }
                else
#endif
                {
                    SetAutoAnswerTimer(pCallInfo);
                }

                return talklogic_GetSessionNum() == 1;
            }
        }
    }
    return false;
}

bool CAutoAnswerASyncProxy::AutoAnswerBySIPInfo(CCallInfo* pCallInfo, CallInUserInfo* pUserInfo)
{
    if (pCallInfo == NULL)
    {
        return false;
    }

    CListenInfo* priv = modtklsn_get_privdata(pCallInfo->GetSessionID());
    if (priv == NULL)
    {
        return false;
    }
    CSessionProxy proxy(pCallInfo->GetSessionID());
    if (!proxy.IsSingle())
    {
        return false;
    }

    int iSessionNum = talklogic_GetSessionNum();
    TALK_INFO("AutoAnswerBySIPInfo: Info[%d], Session Size[%d] AutoAnswerType[%d]",
              pUserInfo->iAutoAnswer, iSessionNum, pUserInfo->uAutoAnswerType);
    int iInterval = pUserInfo->iAutoAnswer > 0 ? pUserInfo->iAutoAnswer : 0;

    if (SIP_CALL_P_ANTO_ANSWER_URGENT == pUserInfo->uAutoAnswerType)
    {
        if (iSessionNum == 1 || iSessionNum == 2)
        {
            SetAutoAnswerTimer(pCallInfo);
            return true;
        }
    }
    else if (SIP_CALL_P_ANTO_ANSWER_IMPERIOUS == pUserInfo->uAutoAnswerType)
    {
        if (iSessionNum == 1 || iSessionNum == 2)
        {
            SetAutoAnswerTimer(pCallInfo);
        }
        else
        {
            // 挂断其他通话
            YLVector<int>lSessionID;
            talklogic_GetAllSessionID(lSessionID);
            for (int i = 0; i < lSessionID.size(); ++i)
            {
                if ((int)proxy != lSessionID[i])
                {
                    CSessionProxy(lSessionID[i]).ExitSession();
                }
            }

            SetAutoAnswerTimer(pCallInfo);
        }

        return true;
    }
    else if (SIP_CALL_P_ANTO_ANSWER_SILENT == pUserInfo->uAutoAnswerType)
    {
        // 只有自己当前这路通话
        if (iSessionNum == 1)
        {
            SetAutoAnswerTimer(pCallInfo, 100, false);
            // 标志是"Silent"的自动应答, 对方挂机后不播忙音
            pCallInfo->m_bSilentAnswer = true;
            return true;
        }
        else if (iSessionNum == 2)
        {
            // 不处理
        }
    }
    else if (SIP_CALL_P_ANTO_ANSWER_NORMAL == pUserInfo->uAutoAnswerType)
    {
        if (iSessionNum == 1)
        {
            // 只有自己当前这路通话
            SetAutoAnswerTimer(pCallInfo);
            return true;
        }
        else if (iSessionNum == 2)
        {
            // 当前还有另外一路通话
            // 不处理
        }
    }
    else
    {
        if (SIP_CALL_AUTO_ANSWER_DEVICE_UNKNOWN != pUserInfo->uAutoAnswerDevice)
        {
            // 通话中新来电，如果有force参数，则强制接起新来电
            if (pUserInfo->bForceAutoAnswer || iSessionNum == 1)
            {
                if (SIP_CALL_AUTO_ANSWER_DEVICE_HEADSET == pUserInfo->uAutoAnswerDevice)
                {
                    // 如果耳机未开启则用免提
                    if (voice_GetHeadsetStatus())
                    {
                        priv->m_eAAChannel = CHANNEL_HEADSET;
                    }
                    else
                    {
                        priv->m_eAAChannel = CHANNEL_HANDFREE;
                    }
                }
                else if (SIP_CALL_AUTO_ANSWER_DEVICE_SPEAKER == pUserInfo->uAutoAnswerDevice)
                {
                    priv->m_eAAChannel = CHANNEL_HANDFREE;
                }
                else if (SIP_CALL_AUTO_ANSWER_DEVICE_ACTIVE == pUserInfo->uAutoAnswerDevice
                         || SIP_CALL_AUTO_ANSWER_DEVICE_HANDSET == pUserInfo->uAutoAnswerDevice)
                {
                    if (voice_GetHandsetStatus())
                    {
                        priv->m_eAAChannel = CHANNEL_HANDSET;
                    }
                    else if (voice_GetHeadsetStatus())
                    {
                        priv->m_eAAChannel = CHANNEL_HEADSET;
                    }
                    else
                    {
                        priv->m_eAAChannel = CHANNEL_HANDFREE;
                    }
                }
                if (iInterval == 0)
                {
                    SetAutoAnswerTimer(pCallInfo);
                }
                else
                {
                    // iInterval秒 后自动应答
                    SetAutoAnswerTimer(pCallInfo, iInterval * 1000);
                }
                return true;
            }
        }
        else
        {
            //http://10.2.1.199/Bug.php?BugID=15438
            //增加对force参数的支持
            if (pUserInfo->iAutoAnswer > 0)
            {
                if (iSessionNum == 1 || pUserInfo->bForceAutoAnswer)
                {
                    // 去掉通话个数的判断,解决http://10.2.1.199/Bug.php?BugID=8324
                    // 只有自己当前这路通话
                    if (pUserInfo->iAutoAnswer == 0)
                    {
                        SetAutoAnswerTimer(pCallInfo);
                        return true;
                    }
                    else
                    {
                        // iInterval秒 后自动应答
                        SetAutoAnswerTimer(pCallInfo, pUserInfo->iAutoAnswer * 1000);
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool CAutoAnswerASyncProxy::OnCallBack()
{
    CCallInfo* pCallInfo = talklogic_GetCallInfoBySessionID(m_iSessionID);
    if (pCallInfo == NULL)
    {
        return false;
    }

    if (configParser_GetConfigInt(kszAutoAnswerMultiCall) != 1)
    {
        if (!pCallInfo->m_bAutoAnswerNow && talklogic_GetSessionNum() != 1)
        {
            return false;
        }
    }

    // 自动应答定时器响应后杀掉定时器，避免出现多次应答的情况
    Kill();

    // 停止AutoAnswer信号音
    voice_StopTone(TONE_AUTOANSWER);

    pCallInfo->m_bIsAutoAnswerCall = true;

    talklogic_AnswerCall(pCallInfo->GetCallID());
    return true;
}

void CAutoAnswerASyncProxy::SetAutoAnswerTimer(CCallInfo* pCallInfo,
        int iTimeMS /*= DEFAULT_TONE_INTERVAL*/,
        bool bPlayTone /*= true*/)
{
    if (pCallInfo == NULL)
    {
        return ;
    }
    CListenInfo* priv = modtklsn_get_privdata(pCallInfo->GetSessionID());
    if (priv == NULL)
    {
        return;
    }
    SetTimer(iTimeMS);
    bool bEnablePlayTone = pCallInfo->m_bIsIntercom
                           && (configParser_GetConfigInt(kszIntercomToneSwitch) != 0);
    bEnablePlayTone |= !pCallInfo->m_bIsIntercom
                       && (configParser_GetConfigInt(kszEnableAutoAnswerTone) != 0);

    bool bInterComSpeakerMode = pCallInfo->m_bIsIntercom
                                && (configParser_GetConfigInt(kszIntercomHeadsetPrior) == 0);

    TALK_INFO("AutoAnswerTimer bEnablePlayTone[%d], IntercomEnable[%d] InterComSpeakerMode[%d]",
              bEnablePlayTone, pCallInfo->m_bIsIntercom, bInterComSpeakerMode);
    if (bPlayTone && bEnablePlayTone && pCallInfo->m_bAutoAnswerNow)
    {
        voice_PlayTone(TONE_AUTOANSWER);
    }
    // 强制使用免提接听
    if (bInterComSpeakerMode)
    {
        priv->m_eAAChannel = CHANNEL_HANDFREE;
    }
}

#ifdef IF_FEATURE_SPECIAL_AATONE
void CAutoAnswerASyncProxy::SetSpecialAutoAnswerTimer(CCallInfo* pCallInfo)
{
    if (pCallInfo == NULL)
    {
        return;
    }
    int iTimerAutoAnswer = 1000;
    bool bNeedPlayTone = false;

    bool bEnablePlayTone = pCallInfo->m_bIsIntercom
                           && (configParser_GetConfigInt(kszIntercomToneSwitch) != 0);
    bEnablePlayTone |= !pCallInfo->m_bIsIntercom
                       && (configParser_GetConfigInt(kszEnableAutoAnswerTone) != 0);

    bool bInterComSpeakerMode = pCallInfo->m_bIsIntercom
                                && (configParser_GetConfigInt(kszIntercomHeadsetPrior) == 0);
    TALK_INFO("AutoAnswerTimer bEnablePlayTone[%d], IntercomEnable[%d] InterComSpeakerMode[%d]",
              bEnablePlayTone, pCallInfo->m_bIsIntercom, bInterComSpeakerMode);

    if (bEnablePlayTone && pCallInfo->m_bAutoAnswerNow)
    {
        if (configParser_GetConfigInt(kszSpecialAutoAnswerTone) != 0)
        {
            int iDelay = configParser_GetConfigInt(kszAutoAnswerDelay);
            if (iDelay < 1 || iDelay > 4)
            {
                iDelay = 3;
            }
            // iDelay中的1s需要响两声，算下来每次466ms
            iTimerAutoAnswer = iDelay * 466 * 2;

            if (talklogic_GetSessionNum() == 1)
            {
                voice_PlayTone(TONE_AUTOANSWER);
            }
            else
            {
                voice_PlayTone(TONE_AUTOANSWER, iTimerAutoAnswer);
            }
        }
    }
    SetTimer(iTimerAutoAnswer);
}
#endif

bool CAutoAnswerASyncProxy::IsAllTalkSessionHoldOrHeld()
{
    YLVector<int> lSessionID;
    talklogic_GetAllSessionID(lSessionID, false);
    for (int i = 0; i < lSessionID.size(); ++i)
    {
        CSessionProxy proxy(lSessionID[i]);
        if (proxy.IsTalkSession() || proxy.IsInHoldOrHeld())
        {
            continue;
        }
        return false;
    }
    return true;
}
