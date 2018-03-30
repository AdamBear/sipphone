#include "viewmoveanimation.h"
#include "xwindow/xWindowManager.h"
#if ANIMATION_DEBUG
#include "dsklog/log.h"
#endif


CViewMoveAnimation::CViewMoveAnimation(int nType/* = ANIMATION_TYPE_VIEW_MOVE*/)
	: CViewAnimation(nType)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("New CViewMoveAnimation:%p", this);
#endif
}

CViewMoveAnimation::~CViewMoveAnimation()
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("Delete CViewMoveAnimation:%p", this);
#endif
}

CViewMoveAnimation* CViewMoveAnimation::GetViewMoveAnimation(CAnimationBase* pAnimation)
{
	if (NULL == pAnimation || ANIMATION_TYPE_VIEW_MOVE != (ANIMATION_TYPE_VIEW_MOVE & pAnimation->GetType()))
	{
		return NULL;
	}

	return static_cast<CViewMoveAnimation*>(pAnimation);
}

void CViewMoveAnimation::SetStartPos(const chPoint& ptPos)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CViewMoveAnimation::SetStartPos(%d, %d) Animation:%p", ptPos.x, ptPos.y, this);
#endif

	m_ptStartPos = ptPos;
}

void CViewMoveAnimation::SetEndPos(const chPoint& ptPos)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CViewMoveAnimation::SetEndPos(%d, %d) Animation:%p", ptPos.x, ptPos.y, this);
#endif

	m_ptEndPos = ptPos;
}

chPoint CViewMoveAnimation::GetCurrentPos()
{
	double dInterpolator = GetInterpolatorValue();
	int nX = m_ptEndPos.x - m_ptStartPos.x;
	int nY = m_ptEndPos.y - m_ptStartPos.y;

	nX = nX * dInterpolator;
	nY = nY * dInterpolator;

	return chPoint(m_ptStartPos.x + nX, m_ptStartPos.y + nY);
}

void CViewMoveAnimation::UpdateViewProperty()
{
	if (NULL == m_pView)
	{
		return;
	}

	chRect rcView = m_pView->geometry();
	chPoint ptPos = GetCurrentPos();
	int nWidth = rcView.Width();
	int nHeight = rcView.Height();

	rcView.left = ptPos.x;
	rcView.top = ptPos.y;
	rcView.right = rcView.left + nWidth;
	rcView.bottom = rcView.top + nHeight;

#if ANIMATION_DEBUG
	XWINDOW_INFO("CViewMoveAnimation::UpdateViewProperty() Animation:%p pView:%p Rect(%d %d %d %d)",
	             this, m_pView, rcView.left, rcView.top, rcView.right, rcView.bottom);
#endif

	g_WorkStation.LayoutViewInBoundRect(m_pView, rcView);
}
