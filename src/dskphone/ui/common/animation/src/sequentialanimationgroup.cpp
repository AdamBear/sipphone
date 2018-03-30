#include "sequentialanimationgroup.h"
#include "unifiedtimer.h"
#if ANIMATION_DEBUG
#include "dsklog/log.h"
#endif


CSequentialAnimationGroup::CSequentialAnimationGroup(int nType/* = ANIMATION_TYPE_SEQUENTIAL_GROUP*/)
	: CAnimationGroup(nType)
	, m_nActiveIndex(0)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("New CSequentialAnimationGroup:%p", this);
#endif
}

CSequentialAnimationGroup::~CSequentialAnimationGroup()
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("Delete CSequentialAnimationGroup:%p", this);
#endif
}

CSequentialAnimationGroup* CSequentialAnimationGroup::GetSequentialAnimation(CAnimationBase* pAnimation)
{
	if (NULL == pAnimation || ANIMATION_TYPE_SEQUENTIAL_GROUP != (ANIMATION_TYPE_SEQUENTIAL_GROUP & pAnimation->GetType()))
	{
		return NULL;
	}

	return static_cast<CSequentialAnimationGroup*>(pAnimation);
}

void CSequentialAnimationGroup::OnStart(bool bStartTimer)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CSequentialAnimationGroup::OnStart(%d) Animation:%p Interval:%d", (bStartTimer ? 1 : 0), this, m_nInterval);
#endif

	m_nActiveIndex = -1;

	StartNextAnimation();

	CAnimationGroup::OnStart(bStartTimer);
}

bool CSequentialAnimationGroup::StartNextAnimation()
{
	int nNextIndex = m_nActiveIndex + 1;
	int nSize = m_vecAnimation.size();
	int nInterval = 0;

#if ANIMATION_DEBUG
	XWINDOW_INFO("CSequentialAnimationGroup::StartNextAnimation() Animation:%p Size:%d NextIndex:%d",
	             this, nSize, nNextIndex);
#endif

	for (; nNextIndex < nSize; ++nNextIndex)
	{
		CAnimationBase* pAnimation = m_vecAnimation[nNextIndex];

		if (NULL == pAnimation || ANIMATION_STATE_WAITING != pAnimation->GetState())
		{
			continue;
		}

		int nTmpInterval = pAnimation->GetInterval();

		if (nTmpInterval > 0)
		{
			m_nActiveIndex = nNextIndex;
			nInterval = nTmpInterval;

			pAnimation->OnStart(false);

			break;
		}
	}

	if (nNextIndex < 0 || nNextIndex >= nSize)
	{
		return false;
	}

	m_nInterval = nInterval;

	return true;
}

bool CSequentialAnimationGroup::IsLastActiveAnimation()
{
	int nNextIndex = m_nActiveIndex + 1;
	int nSize = m_vecAnimation.size();

	for (; nNextIndex < nSize; ++nNextIndex)
	{
		CAnimationBase* pAnimation = m_vecAnimation[nNextIndex];

		if (NULL != pAnimation && ANIMATION_STATE_WAITING == pAnimation->GetState())
		{
			return false;
		}
	}

	return true;
}

int CSequentialAnimationGroup::OnInterval(int nMsec)
{
	int nSize = m_vecAnimation.size();

#if ANIMATION_DEBUG
	XWINDOW_INFO("CParallelAnimationGroup::OnInterval(%d) Animation:%p Size:%d ActiveIndex:%d",
	             nMsec, this, nSize, m_nActiveIndex);
#endif

	if (m_nActiveIndex < 0 || m_nActiveIndex >= nSize)
	{
		Stop();

		return INTERVAL_RESULT_STOP_ANIMATION;
	}

	CAnimationBase* pAnimation = m_vecAnimation[m_nActiveIndex];
	bool bLastAnimation = IsLastActiveAnimation();

	if (NULL != pAnimation)
	{
		int nResult = pAnimation->OnInterval(nMsec);

		if (INTERVAL_RESULT_WAIT_NEXT_INTERVAL == nResult || INTERVAL_RESULT_PLAY_ANIMATION == nResult)
		{
			return nResult;
		}

		if (bLastAnimation)
		{
			return INTERVAL_RESULT_STOP_ANIMATION;
		}
	}

	if (StartNextAnimation())
	{
		if (NULL != m_pTimer)
		{
			m_pTimer->Stop();
			m_pTimer->Start(m_nInterval);
		}

		return INTERVAL_RESULT_PLAY_ANIMATION;
	}

	return INTERVAL_RESULT_STOP_ANIMATION;
}
