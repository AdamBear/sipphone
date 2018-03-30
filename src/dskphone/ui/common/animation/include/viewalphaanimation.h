#ifndef __VIEW_ALPHA_ANIMATION_H__
#define __VIEW_ALPHA_ANIMATION_H__

#include "xwindow/xwindowdefine.h"
#include "viewanimation.h"

#if IF_VIEW_ALPHA
class CViewAlphaAnimation : public CViewAnimation
{
public:
    explicit CViewAlphaAnimation(int& nAlpha, int nType = ANIMATION_TYPE_VIEW_ALPHA);
    virtual ~CViewAlphaAnimation();

    static CViewAlphaAnimation* GetViewAlphaAnimation(CAnimationBase* pAnimation);

public:
    void SetStartAlpha(BYTE byAlpha);
    void SetEndAlpha(BYTE byAlpha);

    int GetCurrentAlpha();

protected:
    virtual void UpdateViewProperty();

protected:
    int& m_nAlpha;
    int m_nStartAlpha;
    int m_nEndAlpha;
};
#endif

#endif
