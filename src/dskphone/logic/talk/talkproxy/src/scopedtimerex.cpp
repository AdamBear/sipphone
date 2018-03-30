#include "scopedtimerex.h"
#include "dsklog/log.h"

void CScopedTimerEx::Kill()
{
    bool bIsOn = IsOn();
    CScopedTimer::Kill();

    if (bIsOn && m_pCleanUpCallBack)
    {
        m_pCleanUpCallBack(*this);
    }
}

size_t CScopedTimerEx::Pop() const
{
    size_t value = INVALID_PARAM;
    int pos = m_vecParam.size() - 1;
    if (pos < 0)
    {
        return value;
    }

    if (Get(pos, value))
    {
        m_vecParam.removeAt(pos);
    }

    return value;
}

CScopedTimerEx& CScopedTimerEx::Push(size_t value)
{
    m_vecParam.push_back(value);
    return *this;
}

bool CScopedTimerEx::Get(int index, size_t& value) const
{
    if (index < 0 || index > m_vecParam.size())
    {
        APP_ERR("CScopedTimerEx::Get overflow id[%d] index[%d][%d]", TIMER_ID(m_uInterval), index, m_vecParam.size());
        return false;
    }

    value = m_vecParam[index];
    return true;
}

