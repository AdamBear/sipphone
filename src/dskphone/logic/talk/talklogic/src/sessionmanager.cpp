#include "talklogic_inc.h"

#if IF_BUG_27560
// 隐藏通话信息的字符串
#define HIDE_TALK_INFO_STRING "******"
#endif

// T49 ipvp只能支持2路视频
#define MAXVIDEOSESSION 2

IMPLEMENT_GETINSTANCE(CSessionManager)

CSessionManager::CSessionManager() : m_iGenSessionID(1), m_iUnHideSessionSize(0)
{
}

CSessionManager::~CSessionManager()
{

}

// 增加Session，加入到SessionList中(加入到pBaseSession的下一个位置)
bool CSessionManager::AddSession(CBaseSession* pSession,
                                 const CBaseSession* pBaseSession/* = NULL*/,
                                 bool bCheckMax /* = true*/)
{
    if (pSession == NULL)
    {
        return false;
    }

    SignalParams sigp(pSession);
    sigp.wParam.iValue = pSession->GetSessionState();

    if (bCheckMax && modtkflt_emit(TALK_FLT_SESSION_CREATE, sigp))
    {
        delete pSession;
        pSession = NULL;
        return false;
    }

    if (pBaseSession == NULL)
    {
        m_ListCallSession.push_back(pSession);
    }
    else
    {
        // 加入到pBaseSession的下一个位置，比如BeTranSession
        LIST_CALL_SESSION::iterator it =
            app::find(m_ListCallSession.begin(), m_ListCallSession.end(), pBaseSession);

        if (it != m_ListCallSession.end())
        {
            m_ListCallSession.insert(it, pSession);
        }
    }

    AddIndex(pSession);
    return true;
}

CSingleSession* CSessionManager::AllocSession(int iCallID /*= -1*/, bool bHide /*= false*/, CBaseSession* pBaseSession /*= NULL*/)
{
    // 加入Session
    CSingleSession* pSession = new CSingleSession(iCallID, bHide);
    if (!AddSession(pSession, pBaseSession, pBaseSession == NULL))
    {
        return NULL;
    }

    return pSession;
}

// 删除Session，并处理删除Session之后的焦点
bool CSessionManager::DeleteSession(CBaseSession* pSession)
{
    if (pSession == NULL)
    {
        return false;
    }

    pSession->SetDestory();

    pSession->FwdHostSession(NULL);

    int iSessionID = pSession->GetSessionID();
    TALK_INFO("CSessionManager::DeleteSession delete begin [ID:%d]", iSessionID);
    // session删除前的操作
    pSession->deleteLater();

    LIST_CALL_SESSION::iterator it = app::find(m_ListCallSession.begin(), m_ListCallSession.end(), pSession);
    if (it != m_ListCallSession.end())
    {
        m_ListCallSession.erase(it);
    }

    RemoveIndex(pSession);

    TALK_INFO("CSessionManager::DeleteSession delete end [ID:%d]", iSessionID);

#if IF_FEATURE_PAGING
    // 尝试等待中的paging请求
    _MulticastPage.RestartWaitPage();
#endif

    return true;
}

// 通过SessionID获取Session
CBaseSession* CSessionManager::GetSession(int iSessionID)
{
    return FindSessionByID(iSessionID);
}

CSingleSession* CSessionManager::GetSingleSession(int iSessionID)
{
    CBaseSession* pSession = GetSession(iSessionID);
    if (pSession == NULL)
    {
        return NULL;
    }

    return pSession->ToSingleSession();
}

// 根据CallID找到对应的Routine
CBaseRoutine* CSessionManager::FindRoutineByCallID(int iCallID)
{
    CSingleSession* pSession = FindSessionByCallID(iCallID);
    return pSession ? pSession->GetRoutine() : NULL;
}

// 根据CallID找到对应的Session
CBaseSession* CSessionManager::GetSessionByCallID(int iCallID, bool bHostSession/* = true*/)
{
    CSingleSession* pSession = FindSessionByCallID(iCallID);
    if (pSession == NULL)
    {
        return NULL;
    }

    CBaseSession* pHostSession = pSession->GetHostSession();
    if (!bHostSession || pHostSession == NULL)
    {
        return pSession;
    }

    return pHostSession;
}

CSingleSession* CSessionManager::GetRelateSessionByCallID(int iCallID, bool bExceptSubSession/* = true*/)
{
    CBaseSession* pSession = GetSessionByCallID(iCallID, false);
    if (pSession == NULL)
    {
        return NULL;
    }

    if (bExceptSubSession && pSession->GetHostSession() != NULL)
    {
        return NULL;
    }

    return pSession->ToSingleSession();
}

// 退出所有Session(除了pExceptThisSession)
void CSessionManager::ExitAllSession(CBaseSession* pExcept/* = NULL*/)
{
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = (*it);

        if (pSession == pExcept)
        {
            continue;
        }

        if (pSession->m_bDestory)
        {
            TALK_ERR("ExitAllSession error session[%d][%s] except[%d]", pSession->GetSessionID(), pSession->GetName(), pExcept ? pExcept->GetSessionID() : INVALID_SESSION_ID);
            continue;
        }

        // 如果有调用ExitSession则必须调整迭代器
        pSession->ExitSession(true);
        ExitAllSession(pExcept);
        break;
    }
}

// 是否存在通话中的话路
bool CSessionManager::IsTalkingSessionExist()
{
    FOR_SESSION_LIST(it)
    {
        if ((*it)->IsInTalking())
        {
            return true;
        }
    }

    return false;
}

// hold所有话路(除pExceptThisSession外的)
void CSessionManager::HoldAllSession(CBaseSession* pExcept/* = NULL*/, HOLD_TYPE eHoldType/* = HT_NORMAL */)
{
    if (pExcept != NULL)
    {
        // 找到pExceptThisSession的最顶级HostSession
        pExcept = pExcept->GetThisOrTopestHostSession();
    }

    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = (*it);
        if (pSession == pExcept || pSession->GetHostSession() != NULL)
        {
            // 拥有HostSession的Session通过HostSession实现Hold
            continue;
        }

        // 除pExcept外的Session都要hold
        if (pSession != pExcept)
        {
            pSession->HoldSession(true, eHoldType);
        }
    }
}

void CSessionManager::HoldAllSession(int iExceptID, HOLD_TYPE eHoldType /*= HT_NORMAL*/)
{
    HoldAllSession(GetSession(iExceptID), eHoldType);
}

// 获取所有SessionID
bool CSessionManager::GetAllSessionID(YLVector<int>& vecSessionID, bool bExceptHide/* = true*/)
{
    vecSessionID.clear();
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = (*it);
        if (bExceptHide && pSession->IsHide())
        {
            continue;
        }

        // 获取SessionID
        vecSessionID.push_back(pSession->GetSessionID());
    }

    return vecSessionID.size() > 0;
}

// 显示被加入网络会议的话路
void CSessionManager::ShowSessionAddToConf(bool bCheckTalkState/* = false*/)
{
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = (*it);
        if (pSession->IsSingleSession() && pSession->IsHide())
        {
            CCallInfo* pInfo = pSession->GetCallInfoAsSub();
            if (pInfo != NULL && pInfo->m_bAddToNetworkConf)
            {
                pSession->SetHide(false);

                //http://bugfree.yealink.com/Bug.php?BugID=133838
                if (bCheckTalkState
                        && pInfo->m_eTalkState == TS_TALK
                        && !pInfo->IsTalkStart())
                {
                    pSession->GetRoutine()->StartTalk();
                }
            }
        }
    }
}

// 是否全部是响铃话路
bool CSessionManager::IsAllSessionRinging(bool bIgnoreAnswered/* = false*/)
{
    int iNonHideSessionCounter = 0; // 非隐藏话路数量
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = (*it);
        if (pSession->IsHide())
        {
            continue;
        }

        if (!pSession->IsSingleSession())
        {
            return false;
        }

        ++iNonHideSessionCounter;

        if (!pSession->IsInherits(ROUTINE_RINGING))
        {
            return false;
        }

        //http://bugfree.yealink.com/Bug.php?BugID=107943
        //已经接起的不算ringroutine
        if (bIgnoreAnswered && pSession->GetCallInfoAsSub()->m_bAnswered)
        {
            return false;
        }
    }

    return iNonHideSessionCounter > 0;
}

#if IF_FEATURE_PAGING
// 重新开启其它的Paging通话
bool CSessionManager::RestartOtherTalkPaging(int iExceptSessionId)
{
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = (*it);
        if (NULL == pSession
                || iExceptSessionId == pSession->GetSessionID()
                || SESSION_TALKING != pSession->GetSessionState())
        {
            continue;
        }

        CSingleSession* pSingleSession = pSession->ToSingleSession();
        if (pSingleSession == NULL
                || !pSingleSession->BeMulticastPage())
        {
            continue;
        }

        CCallInfo* pCallInfo = pSingleSession->GetCallInfo();
        if (pCallInfo->m_eTalkState == TS_TALK)
        {
            TALK_INFO("Restart Other TalkPaging CallID[%d]", pCallInfo->GetCallID());
            _MulticastPage.StartPagingTalk(pCallInfo->m_pPageParam);
            return true;
        }
    }
    return false;
}

bool CSessionManager::BeHavePageOutCall()
{
    FOR_SESSION_LIST(it)
    {
        CSingleSession* pSession = (*it)->ToSingleSession();
        if (pSession != NULL
                && pSession->BePageOut())
        {
            return true;
        }
    }
    return false;
}
#endif

// 是否存在AutoRedial话路
int CSessionManager::GetAutoRedialSessionNum()
{
    int iSessionNum = 0;
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = ((*it));
        if (NULL == pSession || !pSession->IsSingleSession())
        {
            continue;
        }

        if (pSession->IsInherits(ROUTINE_AUTOREDIAL))
        {
            ++iSessionNum;
        }
    }

    return iSessionNum;
}

// 获取首路来电Session
CSingleSession* CSessionManager::GetFirstIncomingCallSession(bool bVideo/* = false*/)
{
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = (*it);
        if (!pSession->IsSingleSession() || !pSession->IsInherits(ROUTINE_RINGING))
        {
            continue;
        }

        return pSession->ToSingleSession();
    }
    return NULL;
}

// 是否有Hold通话(包括Held状态)
bool CSessionManager::BeHaveSessionHold()
{
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = (*it);
        if (pSession == NULL)
        {
            continue;
        }

        // 会议导致的隐藏，也算视频通话中
        if (pSession->GetHostSession() == NULL && pSession->IsHide())
        {
            continue;
        }

        if (pSession->IsSingleSession()
                && pSession->IsTalkSession()
                && (pSession->IsInHold() || pSession->IsInHeld()))
        {
            return true;
        }
    }

    return false;
}

// 是否全部是Hold状态
bool CSessionManager::IsAllSessionHold()
{
    if (m_ListCallSession.size() == 0)
    {
        return false;
    }

    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = (*it);
        if (pSession->IsHide())
        {
            continue;
        }

        if (pSession->IsSingleSession()
                && pSession->IsTalkSession()
                && pSession->IsInHold())
        {
            continue;
        }

        return false;
    }

    return true;
}

// 判断除了某个Session外，其它是否全部是Hold状态
bool CSessionManager::IsAllOtherSessionHold(int nSessionID)
{
    if (m_ListCallSession.size() == 0)
    {
        return false;
    }

    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = (*it);
        if (pSession->IsHide())
        {
            continue;
        }

        if (pSession->IsSingleSession()
                && (pSession->GetSessionID() == nSessionID
                    || (pSession->IsTalkSession()
                        && pSession->IsInHold())))
        {
            continue;
        }

        return false;
    }

    return true;
}

bool CSessionManager::IsAllTalkSessionHoldOrHeld()
{
    if (m_ListCallSession.size() == 0)
    {
        return false;
    }

    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = (*it);
        if (pSession->IsHide())
        {
            continue;
        }

        if (!pSession->IsTalkSession() || pSession->IsInHoldOrHeld())
        {
            continue;
        }

        return false;
    }

    return true;
}

bool CSessionManager::IsAllowCreateLocalConf(CBaseSession* pSession)
{
    //yef:只要当前会议与会者数量没有超过最大限制就ok
    CLocalConfSession* pConfSession = GetConfSession();
    return pConfSession == NULL || pConfSession->GetSubSessionSize() < LOCAL_CONF_MAX_PARTY;
}

// 建立本地会议
bool CSessionManager::EstablishLocalConf(CBaseSession* pAddtoConfSession, bool bNego/* = true*/)
{
    if (pAddtoConfSession == NULL
            || !IsAllowCreateLocalConf(pAddtoConfSession))
    {
        TALK_INFO("not allow creat local conf");
        return false;
    }

    CSingleSession* pAnotherSession = NULL;
    int iSessionSize = GetSessionListSize(false);
    if (iSessionSize >= 2)
    {
        if (iSessionSize == 2)
        {
            CBaseSession* pSession = pAddtoConfSession->GetAnotherSession();
            if (pSession != NULL)
            {
                pAnotherSession = pSession->ToSingleSession();
            }
        }
        if (pAnotherSession != NULL)
        {
            if (pAnotherSession->IsNetConf())
            {
                return false;
            }
            CCallInfo* pAnotherCallInfo = pAnotherSession->GetCallInfo();
            CCallInfo* pCurrentCallInfo = pAddtoConfSession->GetCallInfoAsSub();
            if (pCurrentCallInfo == NULL)
            {
                return false;
            }

            char szBuf[256] = {0};

            sprintf(szBuf, "%d|%d", pAnotherCallInfo->GetCallID(), pCurrentCallInfo->GetCallID());
            //csta-syl
            pAnotherCallInfo->m_strConfId = szBuf;
            pCurrentCallInfo->m_strConfId = szBuf;

            pAnotherCallInfo->m_bIsInConf = true;
            pCurrentCallInfo->m_bIsInConf = true;
        }
    }

    // 是否要新建conf
    bool bNeedNewConf = false;
    // 判断是否已存在localconf
    CLocalConfSession* pLocalConfSession = GetConfSession();
    if (pLocalConfSession == NULL)
    {
        // step1:新建LocalConfSession
        pLocalConfSession = new CLocalConfSession;
        // 加入Sessionlist失败
        if (!AddSession(pLocalConfSession, pAddtoConfSession))
        {
            return false;
        }
        bNeedNewConf = true;
    }

    CSingleSession* pSingleSession = pAddtoConfSession->ToSingleSession();
    if (pSingleSession == NULL)
    {
        return false;
    }

    bool bConfbyNewDial = (configParser_GetConfigInt(kszConfWithPreviousCall) == 0);
    // 通过被发起方找到发起方.
    CSingleSession* pSponorSession = pSingleSession->GetSponsorSession(SPONSOR_CONF);
    bool bExistConf = pLocalConfSession->GetSubSessionSize() > 0;
    //是否总话路数大于本地会议最大话路数
    bool bRoutineNumbOverMaxConf = IsAllowAddLocalConfNumOverMax();

    YLVector<CBaseSession*> vec;

    //当前没有会议的情况，transfer绑定关系话路直接建立会议
    if (bNeedNewConf
            && pSingleSession->InTransfer()
            && NULL == pSponorSession)
    {
        CSingleSession* pSingleTranSession = NULL;
        if (pSingleSession->GetSponsorSession(SPONSOR_TRAN) != NULL)
        {
            pSingleTranSession = pSingleSession->GetSponsorSession(SPONSOR_TRAN);
        }
        else
        {
            pSingleTranSession = pSingleSession->GetFollowSession(SPONSOR_TRAN);
        }
        if (pSingleTranSession != NULL)
        {
            vec.push_back(pSingleTranSession);
        }
        vec.push_back(pAddtoConfSession);
    }
    /*单个加入会议或者是多路中两路建立会议条件：
    1.bConfbyNewDial为false，总话路数大于会议最大路数。
    2.bConfbyNewDial为true
    */
    else if (bConfbyNewDial
             || (!bConfbyNewDial && bRoutineNumbOverMaxConf))
    {

        if (NULL != pSponorSession)
        {
            // 清除其它不在会议中的会议绑定关系 http://10.2.1.199/Bug.php?BugID=93577
            ClearInvalidConfSponsor(pSponorSession, pAddtoConfSession);

            if (NULL == pSponorSession->GetHostSession())
            {
                //半咨询转不用加入会议。
                if (pSponorSession->IsInherits(ROUTINE_TALKING)
                        && !pSponorSession->GetCallInfo()->IsTransferred())
                {
                    vec.push_back(pSponorSession);
                }
            }
        }
        else if (NULL == pSponorSession
                 && pLocalConfSession->GetSubSessionSize() >= LOCAL_CONF_MAX_PARTY)
        {
            return false;
        }//当pSponorSession为空，但是会议话路数还未达到最大时，只要pAddtoConfSession加入会议即可

        vec.push_back(pAddtoConfSession);
    }
    else
    {
        FOR_SESSION_LIST(itRe)
        {
            CBaseSession* pSession = ((*itRe));
            // 不理会SubSession（比如已有会议中的参与方）
            if (pSession->GetHostSession() != NULL
                    || pSession == pLocalConfSession)
            {
                continue;
            }

            CSingleSession* pSingleSession = pSession->ToSingleSession();
            if (pSingleSession != NULL
                    && pSingleSession->IsAllowAddToConf()
                    && !pSingleSession->IsHide()
                    && !pSingleSession->IsNetConf())
            {
                // step2:将加入会议的Session host设置为pLocalConfSession
                vec.push_back(pSession);
            }
        }
    }

    //建立会议后判断是否会议是有效的
    //http://bugfree.yealink.com/Bug.php?BugID=116220
    if (vec.size() < LOCAL_CONF_MAX_PARTY)
    {
        pLocalConfSession->ExitSession();
        return false;
    }

    YLVector<CBaseSession*>::iterator it = vec.begin();
    for (; it != vec.end(); ++it)
    {
        (*it)->SetHostSession(pLocalConfSession);
    }

    // step3:加入会议后的处理
    pLocalConfSession->AfterSessionCreate();
    return true;
}

bool CSessionManager::EstablishNetworkConf(CBaseSession* pAddtoConfSession)
{
    if (pAddtoConfSession == NULL)
    {
        return false;
    }

    CSingleSession* pSingleSession = pAddtoConfSession->ToSingleSession();
    if (NULL == pSingleSession)
    {
        return false;
    }

    //先判断是否存在主持方的网络会议
    CSingleSession* pHostNetConfSession = GetHostNetConfSession(pSingleSession->GetCallInfo()->GetAccountID());
    CSingleSession* pSponsor = NULL;
    if (pHostNetConfSession != NULL)
    {
        pSponsor = pHostNetConfSession;
    }
    //Transfer绑定关系的话路直接建立net conf
    else if (pSingleSession->InTransfer())
    {
        pSponsor = pSingleSession->GetSponsorSession(SPONSOR_TRAN);
        if (pSponsor == NULL)
        {
            pSponsor = pSingleSession;
            pSingleSession = pSingleSession->GetFollowSession(SPONSOR_TRAN);
            if (NULL == pSingleSession
                    || NULL == pSingleSession->GetRoutine())
            {
                return false;
            }
        }

        if (pSponsor->GetRoutine() == NULL)
        {
            return false;
        }
    }
    //Conf绑定关系
    else
    {
        pSponsor = pSingleSession->GetSponsorSession(SPONSOR_CONF);
        if (pSponsor == NULL)
        {
            pSponsor = pSingleSession->GetFollowSession(SPONSOR_CONF);
        }

        if (NULL == pSponsor
                || NULL == pSponsor->GetRoutine())
        {
            return false;
        }
    }

    ROUTINE_STATE eSponsorState = pSponsor->GetRoutineState();
    ROUTINE_STATE eFollowState = pSingleSession->GetRoutineState();
    bool bHandle = false;

    if (eSponsorState == ROUTINE_TALKING
            && eFollowState == ROUTINE_TALKING)
    {
        CTalkRoutine* pTalkRoutine = pSponsor->GetTalkRoutine();
        if (NULL != pTalkRoutine)
        {
            pTalkRoutine->EstablishNetworkConf(pSingleSession);
            bHandle = true;
        }
    }
    else if (eSponsorState == ROUTINE_NETWORKCONF
             && eFollowState == ROUTINE_TALKING)
    {
        bHandle = pSponsor->GetRoutine()->AddToConf(pSingleSession);
    }
    else if (eSponsorState == ROUTINE_TALKING
             && eFollowState == ROUTINE_NETWORKCONF)
    {
        bHandle = pSingleSession->GetRoutine()->AddToConf(pSponsor);
    }

    return bHandle;
}

// 返回当前要建立会议的错误码
ERROR_INFO_TYPE CSessionManager::GetConferenceErrorCode()
{
    ERROR_INFO_TYPE eType = EIT_UNKNOWN;

    // 目前只有cp在使用，此处函数需要讨论 todo 如果添加opus需要处理
#ifdef IF_SUPPORT_FIVE_PARTY
    return EIT_UNKNOWN;
#endif //

    int iConfNum = GetSessionListSize(false);

    // T46下如果任意一方有G722或开启srtp，则不允许进入多余三方的会议
    if (iConfNum >= 3
            && GetCodecSessionNum("g722") > 0)
    {
        eType = EIT_NOT_AVALIABLE_G722_SESSION_TOCONF;
    }

    if (iConfNum >= 3
            && GetSRTPSessionNum() > 0)
    {
        eType = EIT_NOT_AVALIABLE_SRTP_SESSION_TOCONF;
    }

    return eType;
}

// 判断所有话路中，是否有指定的Codec
int CSessionManager::GetCodecSessionNum(const char* pCodec)
{
    if (NULL == pCodec)
    {
        return 0;
    }

    int iNum = 0;
    FOR_SESSION_LIST(it)
    {
        CSingleSession* pSession = (*it)->ToSingleSession();
        if (NULL == pSession)
        {
            continue;
        }

        CCallInfo* pCallInfo = pSession->GetCallInfo();

        if (NULL != pCallInfo
                && 0 == strcasecmp(pCallInfo->m_tVPMParam.audio.codec_name, pCodec))
        {
            iNum++;
        }
    }

    return iNum;
}

// 判断所有话路中，属于srtp话路的个数
int CSessionManager::GetSRTPSessionNum()
{
    int iNum = 0;
    FOR_SESSION_LIST(it)
    {
        CCallInfo* pCallInfo = (*it)->GetCallInfoAsSub();

        if (NULL != pCallInfo
                && 0 != strcmp((char*)pCallInfo->m_tVPMParam.audio.enc_key, "")
                && 0 != strcmp((char*)pCallInfo->m_tVPMParam.audio.dec_key, ""))
        {
            iNum++;
        }
    }

    return iNum;
}

// 通过id查找top session
CBaseSession* CSessionManager::GetTopHostSessionByCallID(int iCallID)
{
    CBaseSession* pSession = FindSessionByCallID(iCallID);
    return pSession ? pSession->GetThisOrTopestHostSession() : NULL;
}

bool CSessionManager::IsLocalConfExist()
{
    return GetConfSession() != NULL;
}

// 是否存在网络会议
bool CSessionManager::IsNetConfExist()
{
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = *it;
        if (pSession->IsHide()
                || pSession->GetHostSession() != NULL)
        {
            continue;
        }

        if (pSession->IsInherits(ROUTINE_NETWORKCONF))
        {
            return true;
        }
    }

    return false;
}


int CSessionManager::GetTalkingNumber(bool bVideo)
{
    int iNum = 0;

    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = *it;
        if (pSession->IsHide()
                || pSession->GetHostSession() != NULL)
        {
            continue;
        }

        if (pSession->IsInherits(ROUTINE_TALKING))
        {
            ++iNum;
        }
    }

    return iNum;
}

CLocalConfSession* CSessionManager::GetConfSession()
{
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = *it;
        if (pSession->IsHide())
        {
            continue;
        }

        if (pSession->IsLocalConfSession())
        {
            return pSession->ToLocalConfSession();
        }
    }

    return NULL;
}


bool CSessionManager::IsSponsorSessionInConf(CSingleSession* pSession)
{
    if (NULL == pSession)
    {
        return false;
    }

    CSingleSession* pSponsor = pSession->GetSponsorSession(SPONSOR_CONF);

    if (NULL == pSponsor)
    {
        pSponsor = pSession->GetFollowSession(SPONSOR_CONF);
    }

    if (NULL == pSponsor)
    {
        return false;
    }

    // 判断是否已存在localconf
    CLocalConfSession* pLocalConfSession = GetConfSession();
    //会议不存在时，允许
    if (NULL == pLocalConfSession)
    {
        return false;
    }

    //判断会议中的子话路是否为该话路的sponsor
    FOR_SESSION_LIST(iter)
    {
        CBaseSession* pSbuSession = (*iter);
        if (pLocalConfSession == pSbuSession->GetHostSession()
                && pSbuSession == pSponsor)
        {
            return true;
        }
    }

    return false;
}

bool CSessionManager::ClearInvalidConfSponsor(CSingleSession* pSponsorSession,
        CBaseSession* pAddtoConfSession)
{
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSessionTmp = (*it);
        if (pSessionTmp == pAddtoConfSession)
        {
            continue;
        }

        CSingleSession* pSessionAddTmp = pSessionTmp->ToSingleSession();
        if (NULL == pSessionAddTmp)
        {
            continue;
        }

        // 通过被发起方找到发起方.
        CSingleSession* pSponorSessionTemp = pSessionAddTmp->GetSponsorSession(SPONSOR_CONF);
        // 发起方一样，但是不在会议中，则解除关系
        if (NULL != pSponorSessionTemp && pSponorSessionTemp == pSponsorSession
                && !pSessionTmp->IsInLocalConf())
        {
            TALK_INFO("clear sponsor relationship[%d]", pSessionAddTmp->GetSessionID());
            pSessionAddTmp->SetSponsorSession(SPONSOR_CONF, NULL);
        }

    }

    return true;
}

bool CSessionManager::IsAllowAddLocalConfNumOverMax()
{
    int iSessionNum = 0;
    FOR_SESSION_LIST_EXPORT(it)
    {
        CBaseSession* pSession = (*it);
        if (pSession->IsHide())
        {
            continue;
        }
        //网络会议不能计算进去
        if (pSession->GetSessionState() == SESSION_NETWORKCONF
                || pSession->GetSessionState() == SESSION_NETWORKCONF_CONNECT)
        {
            continue;
        }

        if (pSession->IsLocalConfSession())
        {
            iSessionNum += pSession->GetSubSessionSize();
        }
        else
        {
            if (pSession->IsSingleSession()
                    && pSession->IsAllowAddToConf())
            {
                ++iSessionNum;
            }
        }
    }

    return iSessionNum > LOCAL_CONF_MAX_PARTY;
}

//获取对应账号主持方网络会议的session
CSingleSession* CSessionManager::GetHostNetConfSession(int nAccountID)
{
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = *it;
        if (pSession->IsHide()
                || !pSession->IsSingleSession()
                || pSession->GetCallInfoAsSub()->GetAccountID() != nAccountID)
        {
            continue;
        }

        if (pSession->GetSessionState() == SESSION_NETWORKCONF)
        {
            return pSession->ToSingleSession();
        }
    }

    return NULL;
}

int CSessionManager::GetTotalNumOfRoutineAllowAddNetConf(int nAccountID)
{
    int iTotalNum = 0;
    FOR_SESSION_LIST_EXPORT(it)
    {
        CBaseSession* pSession = *it;
        if (pSession->IsHide()
                || !pSession->IsSingleSession()
                || pSession->GetCallInfoAsSub()->GetAccountID() != nAccountID)
        {
            continue;
        }

        //主持方会议排除在外
        if (pSession->GetSessionState() == SESSION_NETWORKCONF)
        {
            continue;
        }

        if (pSession->IsAllowAddToConf())
        {
            ++iTotalNum;
        }
    }

    return iTotalNum;
}

//获取对应账号话路总数
int CSessionManager::GetSingleLineSessionListSize(int nAccountID, bool bExceptHide/* = true*/)
{
    int iSessionNum = 0;
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = ((*it));

        if (!pSession->IsSingleSession())
        {
            continue;
        }

        if (bExceptHide && pSession->IsHide())
        {
            continue;
        }

        //不同账号跳过
        if (pSession->GetCallInfoAsSub()->GetAccountID() != nAccountID)
        {
            continue;
        }

        ++iSessionNum;
    }

    return iSessionNum;
}

int CSessionManager::GetAllowAddLocalConfRoutineNum()
{
    int iCount = 0;
    FOR_SESSION_LIST_EXPORT(it)
    {
        CBaseSession* pSession = ((*it));

        if (!pSession->IsSingleSession()
                || pSession->IsHide())
        {
            continue;
        }

        //主持方会议排除在外
        if (pSession->IsInherits(ROUTINE_NETWORKCONF))
        {
            continue;
        }

        if (pSession->IsAllowAddToConf())
        {
            ++iCount;
        }
    }

    return iCount;
}

CBaseSession* CSessionManager::GetTalkSession()
{
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = (*it);
        if (pSession != NULL && pSession->IsInTalking())
        {
            return pSession;
        }
    }

    return NULL;
}

int CSessionManager::GetSessionListSize(bool bExceptHide /*= true*/) const
{
    return bExceptHide ? m_iUnHideSessionSize : m_mapSessionID.size();
}

bool CSessionManager::IsSessionExist(ROUTINE_STATE eRoutine, bool bInherits/* = true*/, CBaseSession* pExcept /*= NULL*/)
{
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = (*it);
        if (pSession == pExcept || pSession->IsHide()
                || !pSession->IsSingleSession() || pSession->GetHostSession() != NULL)
        {
            continue;
        }

        CBaseRoutine* pRoutine = pSession->GetRoutine();
        if (pRoutine == NULL)
        {
            continue;
        }

        if ((!bInherits && pRoutine->GetRoutineState() == eRoutine)
                || (bInherits && pRoutine->IsInherits(eRoutine)))
        {
            return true;
        }
    }

    return false;
}

bool CSessionManager::ExitSession(ROUTINE_STATE eRoutine, bool bInherits/* = true*/, CBaseSession* pExcept/* = NULL*/)
{
    FOR_SESSION_LIST(it)
    {
        CBaseSession* pSession = (*it);
        if (pSession == pExcept || pSession->IsHide()
                || !pSession->IsSingleSession() || pSession->GetHostSession() != NULL)
        {
            continue;
        }

        if (pSession->m_bDestory)
        {
            TALK_ERR("ExitSession error session[%d][%s] routine[%d] inherit[%d] except[%d]",
                     pSession->GetSessionID(), pSession->GetName(), eRoutine, bInherits, pExcept ? pExcept->GetSessionID() : INVALID_SESSION_ID);
            continue;
        }

        CBaseRoutine* pRoutine = pSession->GetRoutine();
        if (pRoutine == NULL)
        {
            continue;
        }

        if ((!bInherits && pRoutine->GetRoutineState() == eRoutine)
                || (bInherits && pRoutine->IsInherits(eRoutine)))
        {
            pSession->ExitSession(true);
            ExitSession(eRoutine);
            return true;
        }
    }

    return false;
}

void CSessionManager::AddIndex(CBaseSession* pSession)
{
    if (pSession == NULL)
    {
        return;
    }

    int iSessionID = pSession->GetSessionID();
    bool bSingle = pSession->IsSingleSession();

    int iCallID = -1;
    if (bSingle)
    {
        iCallID = pSession->GetCallInfoAsSub()->GetCallID();
    }

    SignalParams sigp(iSessionID);
    sigp.iCallID = iCallID;
    modtklsn_emit_sync(TALK_SIG_SESSION_PRE_CREATE, sigp);

    m_mapSessionID[iSessionID] = pSession;

    if (bSingle)
    {
        m_mapCallID[iCallID] = pSession;
    }

    if (!pSession->IsHide())
    {
        ++m_iUnHideSessionSize;
    }

    if (bSingle)
    {
        pSession->AfterSessionCreate();
    }

    TALK_INFO("AddIndex [%d][%d]", iSessionID, iCallID);
}

void CSessionManager::RemoveIndex(CBaseSession* pSession)
{
    if (pSession == NULL)
    {
        return;
    }

    int iSessionID = pSession->GetSessionID();
    int iCallID = -1;

    m_mapSessionID.erase(iSessionID);

    if (m_mapSessionID.size() == 0)
    {
        m_iGenSessionID = 1;
    }

    if (!pSession->IsHide())
    {
        --m_iUnHideSessionSize;
    }

    if (pSession->IsSingleSession())
    {
        iCallID = pSession->GetCallInfoAsSub()->GetCallID();
        m_mapCallID.erase(iCallID);
    }

    TALK_INFO("RemoveIndex [%d][%d]", iSessionID, iCallID);

    SignalParams sigp(iSessionID);
    sigp.iCallID = iCallID;
    modtklsn_emit_sync(TALK_SIG_SESSION_AFTER_DELETE, sigp);
}

void CSessionManager::OnCallIDChanged(CSingleSession* pSession, int iNewCallID)
{
    if (pSession == NULL)
    {
        return;
    }

    int iCallID = pSession->GetCallInfo()->GetCallID();
    m_mapCallID.erase(iCallID);
    m_mapCallID[iNewCallID] = pSession;

    TALK_INFO("CallID changed [%d]->[%d]", iCallID, iNewCallID);
}

CBaseSession* CSessionManager::FindSessionByID(int iSessionID)
{
    if (iSessionID == INVALID_SESSION_ID)
    {
        return NULL;
    }

    //TALK_DBG("FindSessionByID [%d] ", iSessionID);
    MAP_SESSION::iterator it = m_mapSessionID.find(iSessionID);
    if (it != m_mapSessionID.end())
    {
        return it->second;
    }

    return NULL;
}

CSingleSession* CSessionManager::FindSessionByCallID(int iCallID)
{
    //TALK_DBG("FindSessionByCallID [%d] ", iCallID);
    MAP_SESSION::iterator it = m_mapCallID.find(iCallID);
    if (it != m_mapCallID.end())
    {
        return (it->second)->ToSingleSession();
    }

    return NULL;
}

void CSessionManager::OnHideChanged(CBaseSession* pSession)
{
    if (pSession == NULL)
    {
        return;
    }

    if (pSession->IsHide())
    {
        --m_iUnHideSessionSize;
    }
    else
    {
        ++m_iUnHideSessionSize;
    }
}
