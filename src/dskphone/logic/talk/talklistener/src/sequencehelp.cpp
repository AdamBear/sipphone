#include "talklistener_inc.h"

bool CSequenceHelp::CSortHelp::operator()(const HandlerInfo& l, const HandlerInfo& r)
{
    if (l.priority < r.priority)
    {
        return true;
    }
    else if (l.priority > r.priority)
    {
        return false;
    }

    /* 依据距离实现比较，距离近优先级高 */
    VEC_LISTENERS::const_iterator begin = m_vec.begin();
    VEC_LISTENERS::const_iterator end = m_vec.end();

    /* 为空不排序 */
    if (begin == end)
    {
        return false;
    }

    return app::find(begin, end, l.obj) - begin < app::find(begin, end, r.obj) - begin;
}

bool CSequenceHelp::Sort(TALK_SIGNAL_TYPE sig, VEC_HANDLE& vecHandler, bool bsync)
{
    MAP_SEQUENCE::iterator it = m_mapSequence.find(sig);
    if (it == m_mapSequence.end())
    {
        return false;
    }

    const VEC_COLLECT& vecSequence = it->second;
    VEC_LISTENERS vecListener;

    for (VEC_COLLECT::const_iterator its = vecSequence.begin(); its != vecSequence.end(); ++its)
    {
        for (VEC_LISTENERS::const_iterator itl = m_vecListeners.begin(); itl != m_vecListeners.end(); ++itl)
        {
            /* 通过匹配name字符串来实现排序 */
            if (strcmp((*itl)->GetName(), *its) == 0)
            {
                vecListener.push_back(*itl);
                break;
            }
        }
    }

    if (vecListener.size() == 0)
    {
        return false;
    }

    /* 需稳定排序支持 */
    app::stable_sort(vecHandler.begin(), vecHandler.end(), CSortHelp(vecListener));

#if IF_FEATURE_EXTEND_LOG
    yl::string strSequence;
    for (VEC_HANDLE::iterator i = vecHandler.begin(); i != vecHandler.end(); ++i)
    {
        if (i != vecHandler.begin())
        {
            strSequence += "#";
        }

        strSequence += i->obj->GetName();
    }

    LONG_LOG("sorted sync[%d] sig[%3d][%s] sequence[%s]", bsync, sig, GetSignalName(sig), strSequence.c_str());
#endif

    return true;
}

CSequenceHelp::CSequenceHelp(const VEC_LISTENERS& v) : m_vecListeners(v)
{
#define NAME(name) #name

    AUTO_SCOPED_TIMER;

    m_mapSequence[TALK_SIG_SESSION_ROUTINE_CHANGED]
    % NAME(CCallMiscListener)
    % NAME(CSCAListener)
    % NAME(CBWListener)
    % NAME(CAutoAnswerListener)
    % NAME(CVoiceListener)
    % NAME(CFWDDNDListener)
    % NAME(CDsskeyBindListener)
    % NAME(CTalkAdapterListener)
    % NAME(CTalkUpdateListener)
    % NAME(CBackLightListener);

    m_mapSequence[TALK_SIG_DIAL_CALLOUT]
    % NAME(CCommonListener)
    % NAME(CDialPlanListener)
    % NAME(CDsskeyBindListener);

    m_mapSequence[TALK_SIG_PROCESS_CALLOUT]
    % NAME(CEventListener)
    % NAME(CBWListener)
    % NAME(CDsskeyBindListener)
    % NAME(CSCAListener)
    % NAME(CCallMiscListener);

    m_mapSequence[TALK_FLT_SESSION_INCOME]
    % NAME(CEventListener)
    % NAME(CCommonListener)
    % NAME(CDsskeyBindListener)
    % NAME(CContractListener)
    % NAME(CFWDDNDListener)
    % NAME(CCallMiscListener)
    % NAME(CVoiceListener);

    m_mapSequence[TALK_SIG_SESSION_PRE_CREATE]
    % NAME(CCallMiscListener)     // 私有数据分配
    % NAME(CTalkAdapterListener); //

    m_mapSequence[TALK_SIG_SESSION_AFTER_CREATE]
    % NAME(CCallMiscListener)
    % NAME(CTalkAdapterListener)
    % NAME(CBroadsoftRecordListener);

    m_mapSequence[TALK_SIG_SESSION_SAVE_LOG]
    % NAME(CEventListener)
    % NAME(CContractListener);

    m_mapSequence[TALK_SIG_SESSION_AFTER_DELETE]
    % NAME(CCallMiscListener)  // 私有数据销毁
    % NAME(CTalkAdapterListener);

    m_mapSequence[TALK_SIG_SESSION_ANSWER]
    % NAME(CVoiceListener)
    % NAME(CTalkAdapterListener);

    m_mapSequence[TALK_SIG_SESSION_PRE_ROUTINE_CHANGED]
    % NAME(CCommonListener)
    % NAME(CVoiceListener);

    m_mapSequence[TALK_SIG_ACTION_EVENT]
    % NAME(CBroadsoftRecordListener)
    % NAME(CBWListener)
    % NAME(CCallCenterListener)
    % NAME(CCallMiscListener)
    % NAME(CDialPlanListener)
    % NAME(CDooorPhoneListener)
    % NAME(CEventListener)
    % NAME(CFWDDNDListener)
    % NAME(CMuteListener)
    % NAME(CVoiceListener)
    % NAME(CCommonListener);
}
