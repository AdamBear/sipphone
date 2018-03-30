#include "talklistener_inc.h"

CListenCenter::CListenCenter() : m_bSendAsync(false), m_uSigDepth(0)
{
    // 异步信号处理
    SingleMsgReg(TALK_MSG_INTERNAL_ASYNC_SIGNAL, CMSGSubscriber(this, &CListenCenter::OnETLMessage));
}

bool CListenCenter::OnETLMessage(msgObject& msg)
{
    return AsyncEmit();
}

bool CListenCenter::AsyncEmit()
{
    AUTO_WND_UPDATE;
    AUTO_SCOPED_TIMER_ID(m_vecASyncSigParam.size());

    TALK_DBG("CListenCenter::AsyncEmit size[%d]", m_vecASyncSigParam.size());

    for (VEC_SIG_PARAM::iterator it = m_vecASyncSigParam.begin();
            it != m_vecASyncSigParam.end(); ++it)
    {
        SigEmit(SIG_METHOD_POST, it->eSignal, *it);
    }

    m_vecASyncSigParam.clear();
    m_bSendAsync = false;

    return true;
}

int CListenCenter::SigEmit(int flag, TALK_SIGNAL_TYPE sig, const SignalParams& sigp)
{
    int iRet = false;
    MAP_SIGNAL& map = (flag & SIG_METHOD_POST) ? m_mapASyncSignals : m_mapSignals;
    MAP_SIGNAL::iterator it = map.find(sig);

    if (it == map.end() || it->second.size() == 0)
    {
        TALK_DBG("No one care about sig[%3d][%s] async[%d]", sig, GetSignalName(sig), (bool)(flag & SIG_METHOD_POST));
        return iRet;
    }

    // 消息派发
    {
        AUTO_SCOPED_TIMER_NAME_ID(GetSignalName(sig), sig);
        AUTO_REF_COUNT rc(m_uSigDepth);
        iRet = EmitImpl(flag, sigp, it->second);
    }

    TALK_DBG("emit sig[%3d][%s] flag[%d] depth[%d] ret[%d]", sig, GetSignalName(sig), flag, m_uSigDepth, iRet);
    return iRet;
}

int CListenCenter::EmitImpl(int flag, const SignalParams& sigp, const VEC_HANDLE& vec)
{
    int iRet = false;
    /* waring:其生命周期由mislistener维护，需确保其在最后执行 */
    CListenInfo* priv = GetData(sigp.iSessionID);
    for (VEC_HANDLE::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        TALK_DBG("Signal begin listener[%s] sig[%3d][%s] flg[%d]", it->obj->GetName(), sigp.eSignal, GetSignalName(sigp.eSignal), flag);
        // 执行当前Handler
        AUTO_SCOPED_TIMER_NAME_ID(it->obj->GetName(), sigp.eSignal);
        iRet |= it->cb(sigp, static_cast<CListenInfo*>(priv));

        TALK_DBG("Signal end listener[%s] sig[%3d][%s] flg[%d] ret[%d]", it->obj->GetName(), sigp.eSignal, GetSignalName(sigp.eSignal), flag, iRet);

        if (iRet & SIG_RET_BREAK)
        {
            TALK_INFO("break at listener[%s] sig[%d] session[%d][%d] reply[%d] flag[%d]", it->obj->GetName(), sigp.eSignal, sigp.iSessionID, sigp.iCallID, sigp.uReply, flag);
            break;
        }
    }

    return iRet;
}

int CListenCenter::Emit(int flag, TALK_SIGNAL_TYPE sig, const SignalParams& sigp)
{
    if (flag & SIG_METHOD_POST)
    {
        if (!m_bSendAsync)
        {
            // 异步信号：通过消息延后执行
            m_bSendAsync = etl_NotifyApp(FALSE, TALK_MSG_INTERNAL_ASYNC_SIGNAL, sig, flag);
        }

        TALK_DBG("emit async sig[%3d][%s] flag[%d] send[%d] size[%d]", sig, GetSignalName(sig), flag, m_bSendAsync, m_vecASyncSigParam.size());
        m_vecASyncSigParam.push_back(sigp);
        // 只需要异步处理
        if (flag == SIG_METHOD_POST)
        {
            return true;
        }

        flag &= ~SIG_METHOD_POST;
    }

    AUTO_WND_UPDATE;
    return SigEmit(flag, sig, sigp);
}

void CListenCenter::SortImpl(MAP_SIGNAL& map, CSequenceHelp& help)
{
    AUTO_SCOPED_TIMER;
    for (MAP_SIGNAL::iterator it = map.begin(); it != map.end(); ++it)
    {
        help.Sort(it->first, it->second, &map == &m_mapSignals);
    }
}

void CListenCenter::Register(bool bsync, TALK_SIGNAL_TYPE sig, const CallBackFunctor& cb, const CBaseTalkListener* listener, int priority)
{
    if (listener == NULL || textEMPTY(listener->GetName()))
    {
        return;
    }

    /* 注册异步的filter信号是错误的 */
    if (!bsync && sig >= TALK_SIG_FILTER_BEGIN)
    {
        TALK_DBG("error register sig[%d] name[%s]", sig, listener->GetName());
        return;
    }

    MAP_SIGNAL& map = bsync ? m_mapSignals : m_mapASyncSignals;
    map[sig].push_back(HandlerInfo(listener, cb));

    // 加入到初始化链表中
    if (app::find(m_vecInitialization.begin(), m_vecInitialization.end(), listener) == m_vecInitialization.end())
    {
        m_vecInitialization.push_back(listener);
    }
}

void CListenCenter::Sort()
{
    AUTO_SCOPED_TIMER;
    CSequenceHelp help(m_vecInitialization);
    SortImpl(m_mapSignals, help);
    SortImpl(m_mapASyncSignals, help);

    TALK_DBG("Sort listener[%d] sync[%d] async[%d]", m_vecInitialization.size(), m_mapSignals.size(), m_mapASyncSignals.size());
    // 排序完成就不再使用了
    m_vecInitialization.clear();
}

CListenInfo* CListenCenter::GetData(int iSessionID, bool bAutoCreate/* = false*/)
{
    /* 容错 */
    if (iSessionID == INVALID_HANDLE)
    {
        return NULL;
    }

    MAP_DATA::iterator it = m_mapData.find(iSessionID);
    if (it != m_mapData.end())
    {
        return it->second;
    }

    if (bAutoCreate)
    {
        TALK_DBG("CListenCenter Alloc Private Data session[%d]", iSessionID);
        return m_mapData[iSessionID] = new CListenInfo(iSessionID);
    }

    //TALK_DBG("CListenCenter Get Private Data Error session[%d]", iSessionID);
    return NULL;
}

void CListenCenter::EraseData(int iSessionID)
{
    if (iSessionID == INVALID_HANDLE)
    {
        return;
    }

    MAP_DATA::iterator it = m_mapData.find(iSessionID);
    if (it != m_mapData.end())
    {
        delete it->second;
        m_mapData.erase(it);

        TALK_DBG("CListenCenter Free Private Data session[%d]", iSessionID);
    }
}

