#include "animationbase.h"
#include "unifiedtimer.h"
#if ANIMATION_DEBUG
#include "dsklog/log.h"
#endif


CAnimationBase::CAnimationBase(int nType/* = ANIMATION_TYPE_BASE*/)
	: m_nType(nType)
	, m_nState(ANIMATION_STATE_WAITING)
	, m_nDeletionPolicy(DELETION_POLICY_DELETE_WHEN_STOPPED)
	, m_nInterval(100)
	, m_nDuration(0)
	, m_nElapsed(0)
	, m_nAddUpInterval(0)
	, m_pTimer(NULL)
	, m_pParent(NULL)
{
}

CAnimationBase::~CAnimationBase()
{
}

void CAnimationBase::UnInit()
{
	if (NULL != m_pTimer)
	{
		m_pTimer->Stop();
		m_pTimer->ResetAnimation();
		delete m_pTimer;
		m_pTimer = NULL;
	}
}

void CAnimationBase::Release()
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationBase::Release() Animation:%p", this);
#endif

	UnInit();

	delete this;
}

void CAnimationBase::Start(int nDeletionPolicy/* = DELETION_POLICY_DELETE_WHEN_STOPPED*/)
{
	if (ANIMATION_STATE_WAITING != m_nState)
	{
#if ANIMATION_DEBUG
		XWINDOW_INFO("CAnimationBase::Start(%d) Animation:%p State:%d", nDeletionPolicy, this, m_nState);
#endif
		return;
	}

#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationBase::Start(%d) Animation:%p", nDeletionPolicy, this);
#endif

	m_nDeletionPolicy = nDeletionPolicy;

	OnStart(true);
}

bool CAnimationBase::Stop()
{
	if (ANIMATION_STATE_STOPPED == m_nState)
	{
#if ANIMATION_DEBUG
		XWINDOW_INFO("CAnimationBase::Stop() Animation:%p State:%d", this, m_nState);
#endif
		return false;
	}

#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationBase::Stop() Animation:%p Elapsed:%d Duration:%d",
	             this, m_nElapsed, m_nDuration);
#endif

	m_nState = ANIMATION_STATE_STOPPED;

	CAnimationBase* pParent = m_pParent;

	OnStop();

	if (NULL != pParent)
	{
		pParent->OnChildStop(this);
	}

	return true;
}

void CAnimationBase::Pause()
{
	if (ANIMATION_STATE_RUNNING != m_nState)
	{
#if ANIMATION_DEBUG
		XWINDOW_INFO("CAnimationBase::Pause() Animation:%p State:%d", this, m_nState);
#endif
		return;
	}

#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationBase::Pause() Animation:%p Elapsed:%d Duration:%d",
	             this, m_nElapsed, m_nDuration);
#endif

	m_nState = ANIMATION_STATE_PAUSED;

	OnPause();
}

void CAnimationBase::Resume()
{
	if (ANIMATION_STATE_PAUSED != m_nState)
	{
#if ANIMATION_DEBUG
		XWINDOW_INFO("CAnimationBase::Resume() Animation:%p State:%d", this, m_nState);
#endif
		return;
	}

#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationBase::Resume() Animation:%p Elapsed:%d Duration:%d",
	             this, m_nElapsed, m_nDuration);
#endif

	m_nState = ANIMATION_STATE_RUNNING;

	OnResume();
}

void CAnimationBase::SetInterval(int nMsec)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationBase::SetInterval(%d) Animation:%p", nMsec, this);
#endif

	m_nInterval = nMsec;
}

void CAnimationBase::SetDuration(int nMsec)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationBase::SetDuration(%d) Animation:%p", nMsec, this);
#endif

	m_nDuration = nMsec;
}

void CAnimationBase::EnsurePlayEnd(bool bPlayEnd)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationBase::EnsurePlayEnd(%d) Animation:%p", (bPlayEnd ? 1 : 0), this);
#endif
}

void CAnimationBase::SetDeletionPolicy(int nDeletionPolicy)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationBase::SetDeletionPolicy(%d) Animation:%p", nDeletionPolicy, this);
#endif

	m_nDeletionPolicy = nDeletionPolicy;
}

void CAnimationBase::SetParent(CAnimationBase* pParent)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationBase::SetParent(%p) Animation:%p", pParent, this);
#endif

	m_pParent = pParent;
}

void CAnimationBase::OnStart(bool bStartTimer)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationBase::OnStart(%d) Animation:%p Interval:%d", (bStartTimer ? 1 : 0), this, m_nInterval);
#endif

	if (m_nInterval <= 0)
	{
		return;
	}

	m_nState = ANIMATION_STATE_RUNNING;
	m_nElapsed = 0;
	m_nAddUpInterval = 0;

	if (!bStartTimer)
	{
		signalStateChanged.emitSignal((WPARAM)this, ANIMATION_STATE_RUNNING, 0, NULL);

		return;
	}

	if (NULL == m_pTimer)
	{
		m_pTimer = new CUnifiedTimer(this);

		if (NULL == m_pTimer)
		{
			return;
		}
	}

	signalStateChanged.emitSignal((WPARAM)this, ANIMATION_STATE_RUNNING, 0, NULL);

	m_pTimer->Start(m_nInterval);
}

void CAnimationBase::OnStop()
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationBase::OnStop() Animation:%p DeletionPolicy:%d Interval:%d Duration:%d Elapsed:%d AddUpInterval:%d",
	             this, m_nDeletionPolicy, m_nInterval, m_nDuration, m_nElapsed, m_nAddUpInterval);
#endif

	if (NULL != m_pTimer)
	{
		m_pTimer->Stop();
	}

	signalStateChanged.emitSignal((WPARAM)this, ANIMATION_STATE_STOPPED, 0, NULL);

	if (DELETION_POLICY_DELETE_WHEN_STOPPED == m_nDeletionPolicy)
	{
		Release();
	}
}

void CAnimationBase::OnPause()
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationBase::OnPause() Animation:%p Interval:%d Duration:%d Elapsed:%d AddUpInterval:%d",
	             this, m_nInterval, m_nDuration, m_nElapsed, m_nAddUpInterval);
#endif

	if (NULL != m_pTimer)
	{
		m_pTimer->Stop();
	}

	signalStateChanged.emitSignal((WPARAM)this, ANIMATION_STATE_PAUSED, 0, NULL);
}

void CAnimationBase::OnResume()
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationBase::OnResume() Animation:%p Interval:%d Duration:%d Elapsed:%d AddUpInterval:%d",
	             this, m_nInterval, m_nDuration, m_nElapsed, m_nAddUpInterval);
#endif

	signalStateChanged.emitSignal((WPARAM)this, ANIMATION_STATE_RUNNING, 0, NULL);

	if (NULL != m_pTimer)
	{
		m_pTimer->Start(m_nInterval);
	}
}

int CAnimationBase::OnInterval(int nMsec)
{
	if (ANIMATION_STATE_RUNNING != m_nState)
	{
#if ANIMATION_DEBUG
		XWINDOW_INFO("CAnimationBase::OnInterval(%d) Animation:%p State:%d", nMsec, this, m_nState);
#endif
		return INTERVAL_RESULT_NOT_PROCESS;
	}

	int nInterval = nMsec;

	if (nMsec < m_nInterval)
	{
		m_nAddUpInterval += nMsec;

		if (m_nAddUpInterval < m_nInterval)
		{
			return INTERVAL_RESULT_WAIT_NEXT_INTERVAL;
		}

		nInterval = m_nAddUpInterval;
	}

	m_nAddUpInterval = 0;
	m_nElapsed += nInterval;

	if (m_nElapsed >= m_nDuration)
	{
		m_nElapsed = m_nDuration;
	}

#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationBase::OnInterval(%d) Animation:%p Duration:%d Elapsed:%d", nMsec, this, m_nDuration, m_nElapsed);
#endif

	return INTERVAL_RESULT_PLAY_ANIMATION;
}

bool CAnimationBase::CheckStopAnimation()
{
	if (ANIMATION_STATE_STOPPED == m_nState)
	{
		return true;
	}

	if (m_nElapsed >= m_nDuration)
	{
		Stop();

		return true;
	}

	return false;
}

void CAnimationBase::OnChildStop(CAnimationBase* pChild)
{
#if ANIMATION_DEBUG
	XWINDOW_WARN("CAnimationBase::OnChildStop(%p) Animation:%p", pChild, this);
#endif
}

bool CAnimationBase::IsRunning() const
{
	return (ANIMATION_STATE_RUNNING == m_nState);
}
