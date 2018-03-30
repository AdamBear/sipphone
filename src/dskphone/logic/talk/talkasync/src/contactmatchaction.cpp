#include "talkasync_inc.h"

#if IF_BUG_38979
using namespace NS_TA;

CContactMatchAction::CContactMatchAction()
{
    m_eTARequestType = TA_REQUEST_SIMPLEACTION;
}

CContactMatchAction::~CContactMatchAction()
{
}

bool CContactMatchAction::Exec()
{
    bool isSuc = false;

    if (m_strMatchNum.empty())
    {
        return false;
    }

    Dir_GetCotactNameByNumber(m_strMatchNum, m_strTarget, false);

    if (!m_strTarget.empty())
    {
        isSuc = true;
    }

    return isSuc;
}

void CContactMatchAction::SetMatchNum(const yl::string& strNum)
{
    m_strMatchNum = strNum;
}

const yl::string& CContactMatchAction::GetTargetName()
{
    return m_strTarget;
}

#endif
