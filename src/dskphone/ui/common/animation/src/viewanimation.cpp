#include "viewanimation.h"
#if IF_USE_QT_LIB
#include <QtCore/qeasingcurve.h>
#endif
#if ANIMATION_DEBUG
#include "dsklog/log.h"
#endif


CViewAnimation::CViewAnimation(int nType/* = ANIMATION_TYPE_VIEW*/)
    : CAnimationBase(nType)
    , m_pView(NULL)
#if IF_USE_QT_LIB
    , m_pEasingCurve(NULL)
#endif
    , m_bEnsurePlayEnd(false)
{
#if IF_USE_QT_LIB
    m_pEasingCurve = new QEasingCurve(QEasingCurve::Linear);
#endif
}

CViewAnimation::~CViewAnimation()
{
}

CViewAnimation* CViewAnimation::GetViewAnimation(CAnimationBase* pAnimation)
{
    if (NULL == pAnimation || ANIMATION_TYPE_VIEW != (ANIMATION_TYPE_VIEW & pAnimation->GetType()))
    {
        return NULL;
    }

    return static_cast<CViewAnimation*>(pAnimation);
}

void CViewAnimation::ClearView()
{
#if ANIMATION_DEBUG
    XWINDOW_INFO("CViewAnimation::ClearView() Animation:%p View:%p", this, m_pView);
#endif

    if (NULL != m_pView)
    {
        SLOT_DISCONNECT(m_pView, signalAppear, this, CViewAnimation::slotOnViewAppear);

        m_pView = NULL;
    }
}

void CViewAnimation::UnInit()
{
#if ANIMATION_DEBUG
    XWINDOW_INFO("CViewAnimation::UnInit() Animation:%p", this);
#endif

    ClearView();

#if IF_USE_QT_LIB
    if (NULL != m_pEasingCurve)
    {
        delete m_pEasingCurve;

        m_pEasingCurve = NULL;
    }
#endif

    CAnimationBase::UnInit();
}

void CViewAnimation::SetInterpolator(int nInterpolator)
{
#if ANIMATION_DEBUG
    XWINDOW_INFO("CViewAnimation::SetInterpolator(%d) Animation:%p", nInterpolator, this);
#endif

#if IF_USE_QT_LIB
    QEasingCurve::Type eType = static_cast<QEasingCurve::Type>(nInterpolator);

    if (NULL == m_pEasingCurve)
    {
        m_pEasingCurve = new QEasingCurve(eType);
    }
    else
    {
        m_pEasingCurve->setType(eType);
    }
#endif
}

void CViewAnimation::EnsurePlayEnd(bool bPlayEnd)
{
#if ANIMATION_DEBUG
    XWINDOW_INFO("CViewAnimation::EnsurePlayEnd(%d) Animation:%p", (bPlayEnd ? 1 : 0), this);
#endif

    m_bEnsurePlayEnd = bPlayEnd;
}

void CViewAnimation::SetView(xView* pView)
{
#if ANIMATION_DEBUG
    XWINDOW_INFO("CViewAnimation::SetView(%p) Animation:%p m_pView:%p", pView, this, m_pView);
#endif

    if (NULL != m_pView || NULL == pView)
    {
        return;
    }

    m_pView = pView;

    SLOT_CONNECT(m_pView, signalAppear, this, CViewAnimation::slotOnViewAppear);
}

xView* CViewAnimation::GetView() const
{
    return m_pView;
}

double CViewAnimation::GetInterpolatorValue()
{
    double dProgress = 1.0;

    if (m_nElapsed >= m_nDuration || m_nDuration <= 0)
    {
        return dProgress;
    }

    dProgress = ((double)m_nElapsed) / m_nDuration;

#if IF_USE_QT_LIB
    if (NULL != m_pEasingCurve)
    {
        return (double)m_pEasingCurve->valueForProgress((qreal)dProgress);
    }
#endif

    return dProgress;
}


int CViewAnimation::OnInterval(int nMsec)
{
    int nResult = CAnimationBase::OnInterval(nMsec);

    if (INTERVAL_RESULT_NOT_PROCESS == nResult || INTERVAL_RESULT_WAIT_NEXT_INTERVAL == nResult)
    {
        return nResult;
    }

    UpdateViewProperty();

    if (CheckStopAnimation())
    {
        return INTERVAL_RESULT_STOP_ANIMATION;
    }

    return INTERVAL_RESULT_PLAY_ANIMATION;
}

void CViewAnimation::OnStop()
{
#if ANIMATION_DEBUG
    XWINDOW_INFO("CViewMoveAnimation::OnStop() Animation:%p State:%d Duration:%d Elapsed:%d",
                 this, m_nState, m_nDuration, m_nElapsed);
#endif

    if (ANIMATION_STATE_STOPPED != m_nState)
    {
        return;
    }

    if (m_bEnsurePlayEnd && m_nElapsed < m_nDuration)
    {
        m_nElapsed = m_nDuration;

        UpdateViewProperty();
    }

    ClearView();

    CAnimationBase::OnStop();
}

void CViewAnimation::slotOnViewAppear(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    bool bAppear = (1 == wParam);

    if (bAppear || NULL == m_pView || sender != &m_pView->signalAppear)
    {
        return;
    }

#if ANIMATION_DEBUG
    XWINDOW_INFO("CViewAnimation::slotOnViewAppear(disappear) Animation:%p m_pView:%p", this, m_pView);
#endif

    if (!Stop())
    {
        ClearView();
    }
}
