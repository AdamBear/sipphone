#include "viewscaleanimation.h"
#include "xwindow/xWindowManager.h"
#if ANIMATION_DEBUG
#include "dsklog/log.h"
#endif


CViewScaleAnimation::CViewScaleAnimation(int nType/* = ANIMATION_TYPE_VIEW_SCALE*/)
	: CViewAnimation(nType)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("New ScaleAnimatin:%p", this);
#endif
}

CViewScaleAnimation::~CViewScaleAnimation()
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("Delete ScaleAnimatin:%p", this);
#endif
}

CViewScaleAnimation* CViewScaleAnimation::GetViewScaleAnimation(CAnimationBase* pAnimation)
{
	if (NULL == pAnimation || ANIMATION_TYPE_VIEW_SCALE != (ANIMATION_TYPE_VIEW_SCALE & pAnimation->GetType()))
	{
		return NULL;
	}

	return static_cast<CViewScaleAnimation*>(pAnimation);
}

void CViewScaleAnimation::SetStartSize(const chSize& size)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CViewScaleAnimation::SetStartSize(%d %d) Animation:%p", size.cx, size.cy, this);
#endif

	m_sizeStart = size;
}

void CViewScaleAnimation::SetEndSize(const chSize& size)
{
#if ANIMATION_DEBUG
	XWINDOW_INFO("CViewScaleAnimation::SetEndSize(%d %d) Animation:%p", size.cx, size.cy, this);
#endif

	m_sizeEnd = size;
}

chSize CViewScaleAnimation::GetCurrentSize()
{
	double dInterpolator = GetInterpolatorValue();
	int nCX = m_sizeEnd.cx - m_sizeStart.cx;
	int nCY = m_sizeEnd.cy - m_sizeStart.cy;

	nCX = nCX * dInterpolator;
	nCY = nCY * dInterpolator;

	return chSize(m_sizeStart.cx + nCX, m_sizeStart.cy + nCY);
}

void CViewScaleAnimation::UpdateViewProperty()
{
	if (NULL == m_pView)
	{
		return;
	}

	chRect rcView = m_pView->clientGeometry();
	chSize size = GetCurrentSize();

	rcView.right = rcView.left + size.cx;
	rcView.bottom = rcView.top + size.cy;

#if ANIMATION_DEBUG
	XWINDOW_INFO("CViewScaleAnimation::UpdateViewProperty() Animation:%p pView:%p Rect(%d %d %d %d)",
	             this, m_pView, rcView.left, rcView.top, rcView.right, rcView.bottom);
#endif

	g_WorkStation.LayoutViewInBoundRect(m_pView, rcView);
}
