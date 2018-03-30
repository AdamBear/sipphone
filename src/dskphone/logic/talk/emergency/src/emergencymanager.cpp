#include "emergency_inc.h"


LRESULT CEmergencyManager::OnMessage(msgObject & msg)
{
    return _Emergency.OnMessageImp(msg);
}


CEmergencyManager::CEmergencyManager() : m_iWindow(STOP_TIMER_VALUE)
{
    memset(m_arrMode, 0, sizeof(CBaseEmergency *) * MAX_EM_MODE_COUNT);
    SingleMsgReg(CONFIG_MSG_BCAST_CHANGED, CEmergencyManager::OnMessage);
    SingleMsgReg(TM_TIMER, CEmergencyManager::OnMessage);
}

void CEmergencyManager::Init()
{
    int iIndex = 0;
    // 注意优先级 从高到低
#ifdef IF_FEATURE_EMERGENCY
    m_arrMode[iIndex++] = new CEmergencyMode;
#endif
    m_arrMode[iIndex++] = new CWhitelistMode;

    chASSERT(iIndex <= MAX_EM_MODE_COUNT && MAX_EM_MODE_COUNT > 0);

    for (int i = 0; i < MAX_EM_MODE_COUNT; ++i)
    {
        CBaseEmergency * pMode = m_arrMode[i];
        if (pMode)
        {
            pMode->Init();
        }
    }
}

bool CEmergencyManager::OnMessageImp(msgObject & msg)
{
    switch (msg.message)
    {
    case CONFIG_MSG_BCAST_CHANGED:
        {
            if (msg.wParam == ST_EMERGENCY || msg.wParam == ST_LOCK)
            {
                return ReloadConfig(EM_ALL_MOD, msg.lParam);
            }
        }
        break;
    case TM_TIMER:
        {
            if (msg.wParam == TIMER_ID(m_iWindow))
            {
                TALK_WARN("finish emergency Windows");
                timerKillThreadTimer(TIMER_ID(m_iWindow));
                m_iWindow = STOP_TIMER_VALUE;
            }
        }
        break;
    default:
        break;
    }

    return false;
}

int CEmergencyManager::IsMatched(const char * cszTarget, int iTypeMask, bool IsCharCmp /*= false*/)
{
    if (textEMPTY(cszTarget))
    {
        return EM_NONE;
    }

    // 匹配模式改变了也要重新匹配
    if (tCacheInfo.m_strLastMatched != cszTarget
            || tCacheInfo.m_bSemiMatch != IsCharCmp
            || tCacheInfo.m_iLastMode != iTypeMask)
    {
        tCacheInfo.clear();
        for (int i = 0; i < MAX_EM_MODE_COUNT; ++i)
        {
            CBaseEmergency * pMode = m_arrMode[i];
            // 非贪婪匹配，依优先级找到第一个就返回
            if (pMode != NULL && (pMode->GetType() & iTypeMask)
                    && pMode->IsMatched(cszTarget, iTypeMask, &tCacheInfo, IsCharCmp))
            {
                break;
            }
        }

        tCacheInfo.m_strLastMatched = cszTarget;
        tCacheInfo.m_bSemiMatch = IsCharCmp;
        tCacheInfo.m_iLastMode = iTypeMask;
    }

    return tCacheInfo.m_iLastMatchedMode;
}

bool CEmergencyManager::CallEmergency()
{
    //TALK_ERR("fator error, this function should not used anymore");
    return tCacheInfo.m_iLastMatchedMode != EM_NONE && !tCacheInfo.m_strLastMatched.empty()
           && talklogic_CallOut(tCacheInfo.m_strLastMatched, tCacheInfo.m_strLastMatched);
}

LPCSTR CEmergencyManager::GetAssertedId()
{
#ifdef IF_FEATURE_EMERGENCY
    CEmergencyMode * pEmergencyDpMode = dynamic_cast<CEmergencyMode *>(GetEmergencyMode(EM_ADV));
    if (pEmergencyDpMode != NULL)
    {
        return pEmergencyDpMode->GetAssertedId();
    }
#endif

    return NULL;
}

bool CEmergencyManager::ReloadConfig(int iMode, int iType)
{
    for (int i = 0; i < MAX_EM_MODE_COUNT; ++i)
    {
        CBaseEmergency * pMode = m_arrMode[i];
        if (pMode)
        {
            pMode->Reload(iMode, iType);
        }
    }

    // 清除缓存
    tCacheInfo.clear();
    return true;
}

bool CEmergencyManager::IsEmergencyMode(LPCSTR lpNum, bool IsCharCmp/* = false*/)
{
    return IsMatched(lpNum, EM_ADV, IsCharCmp) & EM_ADV;
}

int CEmergencyManager::GetPosIndex(LPCSTR lpNum)
{
    return (IsMatched(lpNum, EM_ADV) & EM_ADV) ? tCacheInfo.m_iLastMatchedIndex : -1;
}

CBaseEmergency * CEmergencyManager::GetEmergencyMode(int iMode)
{
    for (int i = 0; i < MAX_EM_MODE_COUNT; ++i)
    {
        CBaseEmergency * pMode = m_arrMode[i];
        if (pMode && (pMode->GetType() & iMode))
        {
            return pMode;
        }
    }

    return NULL;
}

bool CEmergencyManager::IsEmpty(int iMode)
{
    CBaseEmergency * pMode = GetEmergencyMode(iMode);
    return pMode == NULL ? true : pMode->IsEmpty();
}

void CEmergencyManager::StartWindows()
{
    m_iWindow = configParser_GetConfigInt(kszEmergencyCallWindows);
    if (m_iWindow != 0)
    {
        TALK_WARN("Start emergency Windows timer=[%d]", m_iWindow);
        m_iWindow *= 60 * 1000;
        timerSetThreadTimer(TIMER_ID(m_iWindow), m_iWindow);
    }
    else
    {
        m_iWindow = STOP_TIMER_VALUE;
    }
}
