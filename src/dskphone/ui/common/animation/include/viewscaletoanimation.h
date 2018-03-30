#ifndef __VIEW_SCALE_TO_ANIMATION_H__
#define __VIEW_SCALE_TO_ANIMATION_H__

#include "xwindow/xWindow.h"
#include "parallelanimationgroup.h"


class CViewScaleToAnimation : public CParallelAnimationGroup
{
public:
	explicit CViewScaleToAnimation(int nType = ANIMATION_TYPE_SCALE_TO);
	virtual ~CViewScaleToAnimation();

	static CViewScaleToAnimation* GetViewScaleToAnimation(CAnimationBase* pAnimation);

public:
	virtual void SetInterval(int nMsec);
	virtual void SetDuration(int nMsec);
	virtual void SetInterpolator(int nInterpolator);
	virtual int OnInterval(int nMsec);

	bool InitAnimation(xView* pView);
	void SetStartRect(const chRect& rcStart);
	void SetEndRect(const chRect& rcEnd);
};

#endif
