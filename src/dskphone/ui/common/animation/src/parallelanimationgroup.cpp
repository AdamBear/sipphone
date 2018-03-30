#include "parallelanimationgroup.h"
#if ANIMATION_DEBUG
#include "dsklog/log.h"
#endif


CParallelAnimationGroup::CParallelAnimationGroup(int nType/* = ANIMATION_TYPE_PARALLEL_GROUP*/)
	: CAnimationGroup(nType)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("New CParallelAnimationGroup:%p", this);
#endif
}

CParallelAnimationGroup::~CParallelAnimationGroup()
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("Delete CParallelAnimationGroup:%p", this);
#endif
}

CParallelAnimationGroup* CParallelAnimationGroup::GetParallelAnimation(CAnimationBase* pAnimation)
{
	if (NULL == pAnimation || ANIMATION_TYPE_PARALLEL_GROUP != (ANIMATION_TYPE_PARALLEL_GROUP & pAnimation->GetType()))
	{
		return NULL;
	}

	return static_cast<CParallelAnimationGroup*>(pAnimation);
}

void CParallelAnimationGroup::OnStart(bool bStartTimer)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CParallelAnimationGroup::OnStart(%d) Animation:%p Interval:%d", (bStartTimer ? 1 : 0), this, m_nInterval);
#endif

	int nSize = m_vecAnimation.size();
	int nInterval = 0;
	int i = 0;

	for (; i < nSize; ++i)
	{
		CAnimationBase* pAnimation = m_vecAnimation[i];

		if (NULL == pAnimation)
		{
			continue;
		}

		int nTmpInterval = pAnimation->GetInterval();

		if (nTmpInterval <= 0)
		{
			continue;
		}

		if (nInterval <= 0 || nInterval > nTmpInterval)
		{
			nInterval = nTmpInterval;
		}
	}

	m_nInterval = nInterval;

	CAnimationGroup::OnStart(bStartTimer);

	for (int i = 0; i < nSize; ++i)
	{
		CAnimationBase* pAnimation = m_vecAnimation[i];

		if (NULL != pAnimation)
		{
			pAnimation->OnStart(false);
		}
	}
}

int CParallelAnimationGroup::OnInterval(int nMsec)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CParallelAnimationGroup::OnInterval(%d) Animation:%p", nMsec, this);
#endif

	VEC_ANIMATION vecRunningAnimation;
	int nSize = m_vecAnimation.size();
	int i = 0;

	for (; i < nSize; ++i)
	{
		CAnimationBase* pAnimation = m_vecAnimation[i];

		if (NULL == pAnimation || !pAnimation->IsRunning())
		{
			continue;
		}

		vecRunningAnimation.push_back(pAnimation);
	}

	nSize = vecRunningAnimation.size();

	int nGroupResult = INTERVAL_RESULT_NOT_PROCESS;

	for (i = 0; i < nSize; ++i)
	{
		CAnimationBase* pAnimation = vecRunningAnimation[i];

		if (NULL == pAnimation)
		{
			continue;
		}

		int nResult = pAnimation->OnInterval(nMsec);

		if (INTERVAL_RESULT_PLAY_ANIMATION == nResult)
		{
			nGroupResult = nResult;
		}
		else if (INTERVAL_RESULT_WAIT_NEXT_INTERVAL == nResult && INTERVAL_RESULT_NOT_PROCESS == nGroupResult)
		{
			nGroupResult = INTERVAL_RESULT_WAIT_NEXT_INTERVAL;
		}
	}

	if (INTERVAL_RESULT_NOT_PROCESS == nGroupResult)
	{
		return INTERVAL_RESULT_STOP_ANIMATION;
	}

	return nGroupResult;
}
