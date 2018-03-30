#ifndef __VIEW_MOVE_ANIMATION_H__
#define __VIEW_MOVE_ANIMATION_H__

#include "viewanimation.h"


class CViewMoveAnimation : public CViewAnimation
{
public:
	explicit CViewMoveAnimation(int nType = ANIMATION_TYPE_VIEW_MOVE);
	virtual ~CViewMoveAnimation();

	static CViewMoveAnimation* GetViewMoveAnimation(CAnimationBase* pAnimation);

public:
	void SetStartPos(const chPoint& ptPos);
	void SetEndPos(const chPoint& ptPos);

	chPoint GetCurrentPos();

protected:
	virtual void UpdateViewProperty();

protected:
	chPoint m_ptStartPos;
	chPoint m_ptEndPos;
};

#endif
