#include "expshowstatus.h"
#include "expmanager.h"

//////////////////////////////CExpShowStatusBase////////////////////////////////////
CExpShowStatusBase::CExpShowStatusBase(CExpShowStatusContext * pExpShowStatusContext)
    : m_pExpShowStatusContext(pExpShowStatusContext)
{

}

CExpShowStatusBase::~CExpShowStatusBase()
{
}

//////////////////////////////CExpShowStatusNormal////////////////////////////////////
CExpShowStatusNormal::CExpShowStatusNormal(CExpShowStatusContext * pExpShowStatusContext)
    : CExpShowStatusBase(pExpShowStatusContext)
{

}

CExpShowStatusNormal::~CExpShowStatusNormal()
{

}

void CExpShowStatusNormal::EnterStatusScreenSaver(int nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    m_pExpShowStatusContext->SetExpShowStatusScreenSaver();

    if (NULL == m_pExpShowStatusContext->GetExpShowStatus())
    {
        return;
    }

    m_pExpShowStatusContext->GetExpShowStatus()->EnterStatusScreenSaver(nExpIndex);
}

void CExpShowStatusNormal::ExitStatusScreenSaver(int nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    StatusAction(nExpIndex);
}

void CExpShowStatusNormal::EnterStatusPowerSaving(int nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    m_pExpShowStatusContext->SetExpShowStatusPowerSaving();

    if (NULL == m_pExpShowStatusContext->GetExpShowStatus())
    {
        return;
    }

    m_pExpShowStatusContext->GetExpShowStatus()->EnterStatusPowerSaving(nExpIndex);
}

void CExpShowStatusNormal::ExitStatusPowerSaving(int nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    StatusAction(nExpIndex);
}

void CExpShowStatusNormal::StatusAction(int nExpIndex /*= EXP_MAX_COUNT*/)
{
    _EXPManager.RefreshMemPage(nExpIndex, 1);
}

//////////////////////////////CExpShowStatusScreenSaver////////////////////////////////////
CExpShowStatusScreenSaver::CExpShowStatusScreenSaver(CExpShowStatusContext * pExpShowStatusContext)
    : CExpShowStatusBase(pExpShowStatusContext)
{

}

CExpShowStatusScreenSaver::~CExpShowStatusScreenSaver()
{

}

void CExpShowStatusScreenSaver::EnterStatusScreenSaver(int nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    StatusAction(nExpIndex);
}

void CExpShowStatusScreenSaver::ExitStatusScreenSaver(int nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    m_pExpShowStatusContext->SetExpShowStatusNormal();

    if (NULL == m_pExpShowStatusContext->GetExpShowStatus())
    {
        return;
    }

    m_pExpShowStatusContext->GetExpShowStatus()->ExitStatusScreenSaver(nExpIndex);
}

void CExpShowStatusScreenSaver::EnterStatusPowerSaving(int nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    m_pExpShowStatusContext->SetExpShowStatusScreenSaverAndPowerSaving();

    if (NULL == m_pExpShowStatusContext->GetExpShowStatus())
    {
        return;
    }

    m_pExpShowStatusContext->GetExpShowStatus()->EnterStatusPowerSaving(nExpIndex);
}

void CExpShowStatusScreenSaver::ExitStatusPowerSaving(int nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    StatusAction(nExpIndex);
}

void CExpShowStatusScreenSaver::StatusAction(int nExpIndex /*= EXP_MAX_COUNT*/)
{
    _EXPManager.RefreshMemPage(nExpIndex, 2);
}

//////////////////////////////CExpShowStatusPowerSaving////////////////////////////////////
CExpShowStatusPowerSaving::CExpShowStatusPowerSaving(CExpShowStatusContext * pExpShowStatusContext)
    : CExpShowStatusBase(pExpShowStatusContext)
{

}

CExpShowStatusPowerSaving::~CExpShowStatusPowerSaving()
{

}

void CExpShowStatusPowerSaving::EnterStatusScreenSaver(int nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    m_pExpShowStatusContext->SetExpShowStatusScreenSaverAndPowerSaving();

    if (NULL == m_pExpShowStatusContext->GetExpShowStatus())
    {
        return;
    }

    m_pExpShowStatusContext->GetExpShowStatus()->EnterStatusScreenSaver(nExpIndex);
}

void CExpShowStatusPowerSaving::ExitStatusScreenSaver(int nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    StatusAction(nExpIndex);
}

void CExpShowStatusPowerSaving::EnterStatusPowerSaving(int nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    StatusAction(nExpIndex);
}

void CExpShowStatusPowerSaving::ExitStatusPowerSaving(int nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    m_pExpShowStatusContext->SetExpShowStatusNormal();

    if (NULL == m_pExpShowStatusContext->GetExpShowStatus())
    {
        return;
    }

    m_pExpShowStatusContext->GetExpShowStatus()->ExitStatusPowerSaving(nExpIndex);
}

void CExpShowStatusPowerSaving::StatusAction(int nExpIndex /*= EXP_MAX_COUNT*/)
{
    _EXPManager.RefreshMemPage(nExpIndex, 3);
}

//////////////////////////////CExpShowStatusScreenSaverAndPowerSaving////////////////////////////////////
CExpShowStatusScreenSaverAndPowerSaving::CExpShowStatusScreenSaverAndPowerSaving(
    CExpShowStatusContext * pExpShowStatusContext)
    : CExpShowStatusBase(pExpShowStatusContext)
{

}

CExpShowStatusScreenSaverAndPowerSaving::~CExpShowStatusScreenSaverAndPowerSaving()
{

}

void CExpShowStatusScreenSaverAndPowerSaving::EnterStatusScreenSaver(int
        nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    StatusAction(nExpIndex);
}

void CExpShowStatusScreenSaverAndPowerSaving::ExitStatusScreenSaver(int
        nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    m_pExpShowStatusContext->SetExpShowStatusPowerSaving();

    if (NULL == m_pExpShowStatusContext->GetExpShowStatus())
    {
        return;
    }

    m_pExpShowStatusContext->GetExpShowStatus()->ExitStatusScreenSaver(nExpIndex);
}

void CExpShowStatusScreenSaverAndPowerSaving::EnterStatusPowerSaving(int
        nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    StatusAction(nExpIndex);
}

void CExpShowStatusScreenSaverAndPowerSaving::ExitStatusPowerSaving(int
        nExpIndex/* = EXP_MAX_COUNT*/)
{
    if (NULL == m_pExpShowStatusContext)
    {
        return;
    }

    m_pExpShowStatusContext->SetExpShowStatusScreenSaver();

    if (NULL == m_pExpShowStatusContext->GetExpShowStatus())
    {
        return;
    }

    m_pExpShowStatusContext->GetExpShowStatus()->ExitStatusPowerSaving(nExpIndex);
}

void CExpShowStatusScreenSaverAndPowerSaving::StatusAction(int nExpIndex /*= EXP_MAX_COUNT*/)
{
    _EXPManager.RefreshMemPage(nExpIndex, 3);
}

//////////////////////////////CExpShowStatusContext////////////////////////////////////
CExpShowStatusContext::CExpShowStatusContext()
    : m_pCurrentExpShowStatus(NULL)
    , m_pExpShowStatusNormal(NULL)
    , m_pExpShowStatusScreenSaver(NULL)
    , m_pExpShowStatusPowerSaving(NULL)
    , m_pExpShowStatusScreenSaverAndPowerSaving(NULL)
{
    m_pExpShowStatusNormal = new CExpShowStatusNormal(this);
    m_pExpShowStatusScreenSaver = new CExpShowStatusScreenSaver(this);
    m_pExpShowStatusPowerSaving = new CExpShowStatusPowerSaving(this);
    m_pExpShowStatusScreenSaverAndPowerSaving = new CExpShowStatusScreenSaverAndPowerSaving(this);

    m_pCurrentExpShowStatus = m_pExpShowStatusNormal;
}

CExpShowStatusContext::~CExpShowStatusContext()
{

}

void CExpShowStatusContext::EnterStatusScreenSaver(int nExpIndex /*= EXP_MAX_COUNT*/)
{
    if (NULL == m_pCurrentExpShowStatus)
    {
        return;
    }

    m_pCurrentExpShowStatus->EnterStatusScreenSaver(nExpIndex);
}

void CExpShowStatusContext::ExitStatusScreenSaver(int nExpIndex /*= EXP_MAX_COUNT*/)
{
    if (NULL == m_pCurrentExpShowStatus)
    {
        return;
    }

    m_pCurrentExpShowStatus->ExitStatusScreenSaver(nExpIndex);
}

void CExpShowStatusContext::EnterStatusPowerSaving(int nExpIndex /*= EXP_MAX_COUNT*/)
{
    if (NULL == m_pCurrentExpShowStatus)
    {
        return;
    }

    m_pCurrentExpShowStatus->EnterStatusPowerSaving(nExpIndex);
}

void CExpShowStatusContext::ExitStatusPowerSaving(int nExpIndex /*= EXP_MAX_COUNT*/)
{
    if (NULL == m_pCurrentExpShowStatus)
    {
        return;
    }

    m_pCurrentExpShowStatus->ExitStatusPowerSaving(nExpIndex);
}

bool CExpShowStatusContext::IsInNormal()
{
    if (NULL == m_pCurrentExpShowStatus)
    {
        return false;
    }

    return m_pCurrentExpShowStatus->IsInNormal();
}
