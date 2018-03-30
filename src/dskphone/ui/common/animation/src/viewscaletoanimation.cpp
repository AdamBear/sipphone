#include "viewscaletoanimation.h"
#include "viewmoveanimation.h"
#include "viewscaleanimation.h"
#include "xwindow/xWindowManager.h"
#if ANIMATION_DEBUG
#include "dsklog/log.h"
#endif


CViewScaleToAnimation::CViewScaleToAnimation(int nType/* = ANIMATION_TYPE_SCALE_TO*/)
	: CParallelAnimationGroup(nType)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("New CViewScaleToAnimation:%p", this);
#endif
}

CViewScaleToAnimation::~CViewScaleToAnimation()
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("Delete CViewScaleToAnimation:%p", this);
#endif
}

CViewScaleToAnimation* CViewScaleToAnimation::GetViewScaleToAnimation(CAnimationBase* pAnimation)
{
	if (NULL == pAnimation || ANIMATION_TYPE_SCALE_TO != (ANIMATION_TYPE_SCALE_TO & pAnimation->GetType()))
	{
		return NULL;
	}

	return static_cast<CViewScaleToAnimation*>(pAnimation);
}

bool CViewScaleToAnimation::InitAnimation(xView* pView)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CViewScaleToAnimation::InitAnimation(%p) Animation:%p", pView, this);
#endif

	if (NULL == pView)
	{
		return false;
	}

	ClearAnimation();

	CViewMoveAnimation* pMoveAnimation = new CViewMoveAnimation;

	if (NULL == pMoveAnimation)
	{
		return false;
	}

	pMoveAnimation->SetView(pView);

	AddAnimation(pMoveAnimation);

	CViewScaleAnimation* pScaleAnimation = new CViewScaleAnimation;

	if (NULL == pScaleAnimation)
	{
		return false;
	}

	pScaleAnimation->SetView(pView);

	AddAnimation(pScaleAnimation);

	return true;
}

void CViewScaleToAnimation::SetInterval(int nMsec)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CViewScaleToAnimation::SetInterval(%d) Animation:%p", nMsec, this);
#endif

	int nSize = m_vecAnimation.size();

	for (int i = 0; i < nSize; ++i)
	{
		CAnimationBase* pAnimation = m_vecAnimation[i];

		if (NULL != pAnimation)
		{
			pAnimation->SetInterval(nMsec);
		}
	}
}

void CViewScaleToAnimation::SetDuration(int nMsec)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CViewScaleToAnimation::SetDuration(%d) Animation:%p", nMsec, this);
#endif

	int nSize = m_vecAnimation.size();

	for (int i = 0; i < nSize; ++i)
	{
		CAnimationBase* pAnimation = m_vecAnimation[i];

		if (NULL != pAnimation)
		{
			pAnimation->SetDuration(nMsec);
		}
	}
}

void CViewScaleToAnimation::SetInterpolator(int nInterpolator)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CViewScaleToAnimation::SetInterpolator(%d) Animation:%p", nInterpolator, this);
#endif

	int nSize = m_vecAnimation.size();

	for (int i = 0; i < nSize; ++i)
	{
		CAnimationBase* pAnimation = m_vecAnimation[i];
		CViewAnimation* pViewAnimation = CViewAnimation::GetViewAnimation(pAnimation);

		if (NULL != pViewAnimation)
		{
			pViewAnimation->SetInterpolator(nInterpolator);
		}
	}
}

void CViewScaleToAnimation::SetStartRect(const chRect& rcStart)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CViewScaleToAnimation::SetStartRect(%d %d %d %d) Animation:%p",
	             rcStart.left, rcStart.top, rcStart.right, rcStart.bottom, this);
#endif

	CViewScaleAnimation* pScaleAnimation = NULL;
	CViewMoveAnimation* pMoveAnimation = NULL;
	CAnimationBase* pAnimation = NULL;

	pAnimation = GetAnimationByType(ANIMATION_TYPE_VIEW_SCALE);
	pScaleAnimation = CViewScaleAnimation::GetViewScaleAnimation(pAnimation);

	pAnimation = GetAnimationByType(ANIMATION_TYPE_VIEW_MOVE);
	pMoveAnimation = CViewMoveAnimation::GetViewMoveAnimation(pAnimation);

	if (NULL == pScaleAnimation || NULL == pMoveAnimation)
	{
		return;
	}

	chSize sizeStart(rcStart.Width(), rcStart.Height());
	chPoint ptStart(rcStart.left, rcStart.top);

	pScaleAnimation->SetStartSize(sizeStart);
	pMoveAnimation->SetStartPos(ptStart);
}

void CViewScaleToAnimation::SetEndRect(const chRect& rcEnd)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CViewScaleToAnimation::SetEndRect(%d %d %d %d) Animation:%p",
	             rcEnd.left, rcEnd.top, rcEnd.right, rcEnd.bottom, this);
#endif

	CViewScaleAnimation* pScaleAnimation = NULL;
	CViewMoveAnimation* pMoveAnimation = NULL;
	CAnimationBase* pAnimation = NULL;

	pAnimation = GetAnimationByType(ANIMATION_TYPE_VIEW_SCALE);
	pScaleAnimation = CViewScaleAnimation::GetViewScaleAnimation(pAnimation);

	pAnimation = GetAnimationByType(ANIMATION_TYPE_VIEW_MOVE);
	pMoveAnimation = CViewMoveAnimation::GetViewMoveAnimation(pAnimation);

	if (NULL == pScaleAnimation || NULL == pMoveAnimation)
	{
		return;
	}

	chSize sizeEnd(rcEnd.Width(), rcEnd.Height());
	chPoint ptEnd(rcEnd.left, rcEnd.top);

	pScaleAnimation->SetEndSize(sizeEnd);
	pMoveAnimation->SetEndPos(ptEnd);
}

int CViewScaleToAnimation::OnInterval(int nMsec)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CViewScaleToAnimation::OnInterval(%d) Animation:%p", nMsec, this);
#endif

	CViewScaleAnimation* pScaleAnimation = NULL;
	CViewMoveAnimation* pMoveAnimation = NULL;
	CAnimationBase* pAnimation = NULL;

	pAnimation = GetAnimationByType(ANIMATION_TYPE_VIEW_SCALE);
	pScaleAnimation = CViewScaleAnimation::GetViewScaleAnimation(pAnimation);

	pAnimation = GetAnimationByType(ANIMATION_TYPE_VIEW_MOVE);
	pMoveAnimation = CViewMoveAnimation::GetViewMoveAnimation(pAnimation);

	if (NULL != pScaleAnimation && NULL != pMoveAnimation)
	{
		int nScaleResult = pScaleAnimation->CViewAnimation::OnInterval(nMsec);
		int nMoveResult = pMoveAnimation->CViewAnimation::OnInterval(nMsec);

		if (INTERVAL_RESULT_PLAY_ANIMATION != nScaleResult && INTERVAL_RESULT_PLAY_ANIMATION != nMoveResult)
		{
			if (INTERVAL_RESULT_WAIT_NEXT_INTERVAL == nScaleResult || INTERVAL_RESULT_WAIT_NEXT_INTERVAL == nMoveResult)
			{
				return INTERVAL_RESULT_WAIT_NEXT_INTERVAL;
			}

			return INTERVAL_RESULT_NOT_PROCESS;
		}

		xView* pView = pScaleAnimation->GetView();

		if (NULL == pView)
		{
			pView = pMoveAnimation->GetView();
		}

		if (NULL != pView)
		{
			chPoint ptPos = pMoveAnimation->GetCurrentPos();
			chSize size = pScaleAnimation->GetCurrentSize();
			chRect rcView(ptPos.x, ptPos.y, ptPos.x + size.cx, ptPos.y + size.cy);

#if ANIMATION_DEBUG
			XWINDOW_INFO("%p Animation Geometry:%d %d %d %d ", pView, rcView.left, rcView.top,
			             rcView.right, rcView.bottom);
#endif

			g_WorkStation.LayoutViewInBoundRect(pView, rcView);
		}

		bool bScaleStop = pScaleAnimation->CheckStopAnimation();
		bool bMoveStop = pMoveAnimation->CheckStopAnimation();

		if (bScaleStop && bMoveStop)
		{
			return INTERVAL_RESULT_STOP_ANIMATION;
		}

		return INTERVAL_RESULT_PLAY_ANIMATION;
	}

	return INTERVAL_RESULT_NOT_PROCESS;
}
