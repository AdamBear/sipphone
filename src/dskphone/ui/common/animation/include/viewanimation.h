#ifndef __VIEW_ANIMATION_H__
#define __VIEW_ANIMATION_H__

#include "ETLLib/ETLLib.hpp"
#include "xwindow/xWindow.h"
#include "animationbase.h"


#if IF_USE_QT_LIB
class QEasingCurve;
#endif

class CViewAnimation : public CAnimationBase, public xSlotHandler
{
public:
    explicit CViewAnimation(int nType = ANIMATION_TYPE_VIEW);
    virtual ~CViewAnimation();

    static CViewAnimation* GetViewAnimation(CAnimationBase* pAnimation);

public:
    virtual void UnInit();
    virtual void SetInterpolator(int nInterpolator);
    virtual void EnsurePlayEnd(bool bPlayEnd);
    virtual int OnInterval(int nMsec);

    void SetView(xView* pView);

    xView* GetView() const;

protected:
    virtual void OnStop();

    virtual void UpdateViewProperty()       = 0;

    void slotOnViewAppear(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

    double GetInterpolatorValue();

    void ClearView();

protected:
    xView* m_pView;
#if IF_USE_QT_LIB
    QEasingCurve* m_pEasingCurve;
#endif
    bool m_bEnsurePlayEnd;
};

#endif
