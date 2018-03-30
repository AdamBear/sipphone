#include "viewalphaanimation.h"
#include "xwindow/xwindowdefine.h"
#if ANIMATION_DEBUG
#include "dsklog/log.h"
#endif


#if IF_VIEW_ALPHA
CViewAlphaAnimation::CViewAlphaAnimation(int& nAlpha, int nType/* = ANIMATION_TYPE_VIEW_ALPHA*/)
    : CViewAnimation(nType)
    , m_nAlpha(nAlpha)
    , m_nStartAlpha(0)
    , m_nEndAlpha(255)
{
#if ANIMATION_DEBUG
    XWINDOW_INFO("New CViewAlphaAnimation:%p", this);
#endif
}

CViewAlphaAnimation::~CViewAlphaAnimation()
{
#if ANIMATION_DEBUG
    XWINDOW_INFO("Delete CViewAlphaAnimation:%p", this);
#endif
}

CViewAlphaAnimation* CViewAlphaAnimation::GetViewAlphaAnimation(CAnimationBase* pAnimation)
{
    if (NULL == pAnimation || ANIMATION_TYPE_VIEW_ALPHA != (ANIMATION_TYPE_VIEW_ALPHA & pAnimation->GetType()))
    {
        return NULL;
    }

    return static_cast<CViewAlphaAnimation*>(pAnimation);
}

void CViewAlphaAnimation::SetStartAlpha(BYTE byAlpha)
{
    m_nStartAlpha = byAlpha;
#if ANIMATION_DEBUG
    XWINDOW_INFO("CViewAlphaAnimation::SetStartAlpha(%d) Animation:%p", m_nStartAlpha, this);
#endif
}

void CViewAlphaAnimation::SetEndAlpha(BYTE byAlpha)
{
    m_nEndAlpha = byAlpha;
#if ANIMATION_DEBUG
    XWINDOW_INFO("CViewAlphaAnimation::SetEndAlpha(%d) Animation:%p", m_nEndAlpha, this);
#endif
}

int CViewAlphaAnimation::GetCurrentAlpha()
{
    double dInterpolator = GetInterpolatorValue();

    int nAlpha = m_nEndAlpha - m_nStartAlpha;

    nAlpha = nAlpha * dInterpolator;

    nAlpha += m_nStartAlpha;

    if (nAlpha < 0)
    {
        nAlpha = 0;
    }
    else if (nAlpha > 255)
    {
        nAlpha = 255;
    }

    return nAlpha;
}

void CViewAlphaAnimation::UpdateViewProperty()
{
    m_nAlpha = GetCurrentAlpha();

#if ANIMATION_DEBUG
    XWINDOW_INFO("CViewAlphaAnimation::UpdateViewProperty() Animation:%p", this);
#endif
    if (NULL == m_pView)
    {
        return;
    }

    m_pView->update(true);
}

#endif
