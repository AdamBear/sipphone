#ifndef __VIEW_SCALE_ANIMATION_H__
#define __VIEW_SCALE_ANIMATION_H__

#include "viewanimation.h"


class CViewScaleAnimation : public CViewAnimation
{
public:
	explicit CViewScaleAnimation(int nType = ANIMATION_TYPE_VIEW_SCALE);
	virtual ~CViewScaleAnimation();

	static CViewScaleAnimation* GetViewScaleAnimation(CAnimationBase* pAnimation);

public:
	void SetStartSize(const chSize& size);
	void SetEndSize(const chSize& size);

	chSize GetCurrentSize();

protected:
	virtual void UpdateViewProperty();

protected:
	chSize m_sizeStart;
	chSize m_sizeEnd;
};

#endif
