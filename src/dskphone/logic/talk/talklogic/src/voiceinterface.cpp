#include "voiceinterface.h"
#include "talklogic_inc.h"

IMPLEMENT_GETINSTANCE(CVoiceInterface)

CVoiceInterface::CVoiceInterface()
{
    m_iHoldMax = 4;
}

CVoiceInterface::~CVoiceInterface()
{
}

// hold话路
bool CVoiceInterface::HoldSession(int iCallID, bool bStatusOnly)
{
    HoldInfo* pStateInfo = GetStateInfo(iCallID);
    TALK_INFO("HoldSession [%d], state[%d] statusonly[%d]",
              iCallID, NULL == pStateInfo ? -1 : pStateInfo->eState, bStatusOnly);
    if (NULL == pStateInfo
            || HOLD_TALK_HOLD == pStateInfo->eState
            || HOLD_TALK_STOP == pStateInfo->eState)
    {
        return true;
    }

    TALK_INFO("VoiceInterface:hold talk[%d] ", iCallID);
    if (!bStatusOnly) //仅设置状态，不改变ipvp流，用于新conf hold
    {
        voice_Hold(iCallID);
    }

    pStateInfo->eState = HOLD_TALK_HOLD;
    return true;
}

// unhold话路
bool CVoiceInterface::ResumeSession(int iCallID, bool bStatusOnly)
{
    TALK_INFO("ResumeSession [%d] statusonly[%d]", iCallID, bStatusOnly);
    CBaseRoutine* pRoutine = _SessionManager.FindRoutineByCallID(iCallID);
    if (NULL == pRoutine)
    {
        return false;
    }

    CCallInfo* pCallInfo = pRoutine->GetCallInfo();
    if (NULL == pCallInfo)
    {
        return false;
    }

#if IF_FEATURE_LOOPBACK
    if (pCallInfo->m_bIsLoopBackCall)
    {
        return false;
    }
#endif

    HoldInfo* pStateInfo = GetStateInfo(iCallID);
    if (NULL == pStateInfo)
    {
        // 如果没有,则直接加进来,如被转时,被转方hold,对方接起来,就会出现这种情况
        AddSession(iCallID);
        return false;
    }

    if (HOLD_TALK_HOLD == pStateInfo->eState)
    {
        TALK_INFO("VoiceInterface:resume talk[%d]", iCallID);
        if (!bStatusOnly) //仅设置状态，不改变ipvp流，用于新conf hold
        {
            voice_Hold(iCallID, false);
        }
    }
    else if (HOLD_TALK_STOP == pStateInfo->eState)
    {
        StopMoreHoldSession(iCallID);
        TALK_INFO("VoiceInterface:start talk[%d]", iCallID);
        // 需要使用Callid的接口，避免递归调用
        pRoutine->StartTalk(iCallID);
    }
    else
    {
        // 容错处理，一般不应该执行到此处
        // http://10.3.5.199/Bug.php?BugID=62516
        TALK_WARN("VoiceInterface:Reset talk[%d]", iCallID);
        voice_ResetTalk(iCallID, pCallInfo->m_tVPMParam);
    }
    pStateInfo->eState = HOLD_TALK_TALKING;
    return true;
}

// stop话路
bool CVoiceInterface::StopSession(int iCallID)
{
#ifdef IF_BT_SUPPORT_PHONE
    if (talklogic_IsBluetoothSessionByCallID(iCallID))
    {
        return StopBtSession(iCallID);
    }
#endif

    TALK_INFO("StopSession [%d]", iCallID);
    HoldInfo* pStateInfo = GetStateInfo(iCallID);
    if (NULL == pStateInfo
            || HOLD_TALK_STOP == pStateInfo->eState)
    {
        return true;
    }

    TALK_INFO("VoiceInterface:stop talk[%d]", iCallID);
    voice_StopTalk(iCallID);

    pStateInfo->eState = HOLD_TALK_STOP;
    return true;
}

bool CVoiceInterface::StopBtSession(int iCallID)
{
    TALK_INFO("StopBtSession [%d]", iCallID);

    HoldInfo* pStateInfo = GetStateInfo(iCallID);
    if (NULL == pStateInfo
            || HOLD_TALK_STOP == pStateInfo->eState)
    {
        return true;
    }

    //http://bugfree.yealink.com/Bug.php?BugID=78338
    //存在多路手机通话，stop时需要先确认下是否有其他手机通话存在
#ifdef IF_BT_SUPPORT_PHONE
    if (!_BTMobileManager.IsMobileTalkSessionExist(iCallID))
#endif
    {
        TALK_INFO("VoiceInterface:stop talk[%d]", iCallID);
        voice_StopTalk(iCallID);
    }

    pStateInfo->eState = HOLD_TALK_STOP;
    return true;
}

// 增加一路hold信息,返回增加的对象
bool CVoiceInterface::AddSession(int iCallID, HOLD_TALK_STATE eState/* = HOLD_TALK_TALKING*/)
{
    HoldInfo* pInfo = GetStateInfo(iCallID);
    if (NULL != pInfo)
    {
        ResumeSession(iCallID);
        return true;
    }

    TALK_INFO("VoiceInterface:add session[%d][%d]", iCallID, eState);
    if (HOLD_TALK_TALKING == eState)
    {
        StopMoreHoldSession();

        CBaseRoutine* pRoutine = _SessionManager.FindRoutineByCallID(iCallID);
        if (NULL == pRoutine)
        {
            return false;
        }

        CCallInfo* pCallInfo = pRoutine->GetCallInfo();
        if (NULL == pCallInfo)
        {
            return false;
        }

        if (!pCallInfo->IsAudioTalk())
        {
            TALK_INFO("not audio talk[%d]", iCallID);
            return false;
        }
        TALK_INFO("VoiceInterface:start talk[%d]", iCallID);
        voice_StartTalk(iCallID, pCallInfo->m_tVPMParam);
    }

    HoldInfo holdInfo;
    holdInfo.iCallID = iCallID;
    holdInfo.eState = eState;
    m_listHoldInfo.push_back(holdInfo);
    return true;
}

// 获取当前设置的状态
HoldInfo* CVoiceInterface::GetStateInfo(int iCallID)
{
    YLList<HoldInfo>::iterator it = m_listHoldInfo.begin();
    for (; it != m_listHoldInfo.end(); ++it)
    {
        HoldInfo& holdInfo = (*it);
        if (holdInfo.iCallID == iCallID)
        {
            return &holdInfo;
        }
    }

    return NULL;
}

// 当前hold个数
int CVoiceInterface::GetHoldAndTalkingNum()
{
    int iCount = 0;
    YLList<HoldInfo>::iterator it = m_listHoldInfo.begin();
    for (; it != m_listHoldInfo.end(); ++it)
    {
        HoldInfo& holdInfo = (*it);
        if (HOLD_TALK_HOLD == holdInfo.eState
                || HOLD_TALK_TALKING == holdInfo.eState)
        {
            iCount++;
        }
    }

    return iCount;
}

// 去掉指定话路信息
bool CVoiceInterface::RemoveSession(int iCallID)
{
    TALK_INFO("Remove Session [%d]", iCallID);
    YLList<HoldInfo>::iterator it = m_listHoldInfo.begin();
    for (; it != m_listHoldInfo.end(); ++it)
    {
        HoldInfo& holdInfo = (*it);

        if (holdInfo.iCallID == iCallID)
        {
            if (HOLD_TALK_STOP != holdInfo.eState)
            {
                bool bStopTalk = true;
#ifdef IF_BT_SUPPORT_PHONE
                //http://bugfree.yealink.com/Bug.php?BugID=78338
                if (talklogic_IsBluetoothSessionByCallID(iCallID)
                        && _BTMobileManager.IsMobileTalkSessionExist(iCallID))
                {
                    bStopTalk = false;
                }
#endif

                if (bStopTalk)
                {
                    TALK_INFO("VoiceInterface:stop talk[%d]", iCallID);
                    // Hold状态的话路要去Stop掉，VPM才会去释放资源
                    voice_StopTalk(iCallID);
                }
            }
            m_listHoldInfo.erase(it);

            return true;
        }
    }

    return false;
}

// 停掉超过最大hold数量的话路
bool CVoiceInterface::StopMoreHoldSession(int iExceptCallID/* = -1*/)
{
    if (GetHoldAndTalkingNum() >= m_iHoldMax)
    {
        CBaseSession* pHostSession = _SessionManager.GetTopHostSessionByCallID(iExceptCallID);

        // 获取第一路hold的话路
        YLList<HoldInfo>::iterator it = m_listHoldInfo.begin();
        for (; it != m_listHoldInfo.end(); ++it)
        {
            HoldInfo& holdInfo = (*it);
            if (HOLD_TALK_HOLD != holdInfo.eState
                    || holdInfo.iCallID == iExceptCallID)
            {
                continue;
            }

            // 如果两路有相同的父session,则不进行hold
            // vpm只支持4路talk,当有一个三方会议,有两路单路通话,再来一路来电时,要去找一种hold的话路,把它stop
            // 这时会找到会议中的一路,当会议进行resume时,又需要再找一路进行stop,而此时肯定不能找会议中的参与
            // 进行stop,所以定了这个规则
            CBaseSession* pTmpSession = _SessionManager.GetTopHostSessionByCallID(holdInfo.iCallID);
            if (NULL == pTmpSession
                    || pHostSession != pTmpSession)
            {
                //视频话路跳过
                CCallInfo* pCallInfo = talklogic_GetCallInfoByCallId(holdInfo.iCallID);
                if (NULL == pCallInfo)
                {
                    continue;
                }

                TALK_INFO("VoiceInterface:Stop more hold session[%d]", holdInfo.iCallID);

                CBaseRoutine* pRoutine = _SessionManager.FindRoutineByCallID(holdInfo.iCallID);
                if (NULL != pRoutine)
                {
                    pRoutine->StopTalk(holdInfo.iCallID);
                }

                holdInfo.eState = HOLD_TALK_STOP;
                return true;
            }
        }
    }

    return false;
}

// 播放铃声,因为vpm中,铃声也是占用一路
bool CVoiceInterface::PlayRing(const char* pRingPath)
{
    if (NULL == pRingPath)
    {
        return false;
    }

    StopMoreHoldSession();
    voice_PlayRingFromFilePath(pRingPath);
    return true;
}

//FarMute
bool CVoiceInterface::SetFarMute(int iCallID, const ipvp_speech_t& talkParam, bool bFarMute)
{
    //蓝牙话路
#ifdef IF_BT_SUPPORT_PHONE
    if (talklogic_IsBluetoothSessionByCallID(iCallID))
    {
        if (PT_CP920 == devicelib_GetPhoneType())
        {
            // cp920 vpm支持far mute
            voice_SetBtFarMute(iCallID, bFarMute);
            return true;
        }
        else
        {
            //蓝牙远程静音为将声音通道在话机与手机之间切换
            return Bluetooth_SwitchChannel(!bFarMute, iCallID);
        }
    }
#endif
    voice_SetFarMute(iCallID, talkParam, bFarMute);
    return true;
}

//reset talk
void CVoiceInterface::ResetTalk(int iCallID, const ipvp_speech_t& talkParam)
{
    HoldInfo* pInfo = GetStateInfo(iCallID);
    if (NULL != pInfo)
    {
        voice_ResetTalk(iCallID, talkParam);
        return;
    }

    AddSession(iCallID);
}
