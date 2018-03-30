#include "animationgroup.h"
#if ANIMATION_DEBUG
#include "dsklog/log.h"
#endif


CAnimationGroup::CAnimationGroup(int nType/* = ANIMATION_TYPE_GROUP*/)
	: CAnimationBase(nType)
{

}

CAnimationGroup::~CAnimationGroup()
{
	ClearAnimation();
}

CAnimationGroup* CAnimationGroup::GetAnimationGroup(CAnimationBase* pAnimation)
{
	if (NULL == pAnimation || ANIMATION_TYPE_GROUP != (ANIMATION_TYPE_GROUP & pAnimation->GetType()))
	{
		return NULL;
	}

	return static_cast<CAnimationGroup*>(pAnimation);
}

void CAnimationGroup::ClearAnimation()
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationGroup::ClearAnimation");
#endif

	VEC_ANIMATION::iterator iter = m_vecAnimation.begin();

	for (; iter != m_vecAnimation.end(); ++iter)
	{
		CAnimationBase* pAnimation = *iter;

		if (NULL != pAnimation)
		{
			pAnimation->Release();
		}
	}

	m_vecAnimation.clear();
}

void CAnimationGroup::UnInit()
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationGroup::UnInit");
#endif

	ClearAnimation();

	CAnimationBase::UnInit();
}

void CAnimationGroup::EnsurePlayEnd(bool bPlayEnd)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationGroup::EnsurePlayEnd(%d) Animation:%p", (bPlayEnd ? 1 : 0), this);
#endif

	int nSize = m_vecAnimation.size();

	for (int i = 0; i < nSize; ++i)
	{
		CAnimationBase* pAnimation = m_vecAnimation[i];

		if (NULL != pAnimation)
		{
			pAnimation->EnsurePlayEnd(bPlayEnd);
		}
	}
}

void CAnimationGroup::AddAnimation(CAnimationBase* pAnimation)
{
	if (NULL == pAnimation)
	{
		return;
	}

#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationGroup::AddAnimation(%p) Animation:%p", pAnimation, this);
#endif

	pAnimation->SetDeletionPolicy(DELETION_POLICY_KEEP_WHEN_STOPPED);
	pAnimation->SetParent(this);

	m_vecAnimation.push_back(pAnimation);
}

void CAnimationGroup::RemoveAnimation(CAnimationBase* pAnimation)
{
	if (NULL == pAnimation)
	{
		return;
	}

#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationGroup::RemoveAnimation(%p) Animation:%p", pAnimation, this);
#endif

	int nSize = m_vecAnimation.size();

	for (int i = 0; i < nSize; ++i)
	{
		if (pAnimation == m_vecAnimation[i])
		{
			m_vecAnimation.removeAt(i);

			pAnimation->Release();
			break;
		}
	}
}

CAnimationBase* CAnimationGroup::GetAnimationByType(int nType)
{
	int nSize = m_vecAnimation.size();

	for (int i = 0; i < nSize; ++i)
	{
		CAnimationBase* pAnimation = m_vecAnimation[i];

		if (NULL != pAnimation && nType == pAnimation->GetType())
		{
			return pAnimation;
		}
	}

	return NULL;
}

CAnimationBase* CAnimationGroup::GetAnimationByIndex(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_vecAnimation.size())
	{
		return m_vecAnimation[nIndex];
	}

	return NULL;
}

int CAnimationGroup::GetAnimationCount()
{
	return m_vecAnimation.size();
}

void CAnimationGroup::OnStop()
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationGroup::OnStop() Animation:%p", this);
#endif

	int nSize = m_vecAnimation.size();

	for (int i = 0; i < nSize; ++i)
	{
		CAnimationBase* pAnimation = m_vecAnimation[i];

		if (NULL != pAnimation)
		{
			pAnimation->Stop();
		}
	}

	CAnimationBase::OnStop();
}

void CAnimationGroup::OnChildStop(CAnimationBase* pChild)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CAnimationGroup::OnChildStop(%p) Animation:%p State:%d", pChild, this, m_nState);
#endif

	if (ANIMATION_STATE_STOPPED == m_nState)
	{
		return;
	}

	int nSize = m_vecAnimation.size();

	for (int i = 0; i < nSize; ++i)
	{
		CAnimationBase* pAnimation = m_vecAnimation[i];

		if (NULL != pAnimation && ANIMATION_STATE_STOPPED != pAnimation->GetState())
		{
			return;
		}
	}

	Stop();
}
