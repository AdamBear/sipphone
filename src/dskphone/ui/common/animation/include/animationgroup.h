#ifndef __ANIMATION_GROUP_H__
#define __ANIMATION_GROUP_H__

#include "ylvector.h"
#include "animationbase.h"


typedef YLVector<CAnimationBase*> VEC_ANIMATION;

class CAnimationGroup : public CAnimationBase
{
public:
	explicit CAnimationGroup(int nType = ANIMATION_TYPE_GROUP);
	virtual ~CAnimationGroup();

	static CAnimationGroup* GetAnimationGroup(CAnimationBase* pAnimation);

public:
	virtual void UnInit();
	virtual void EnsurePlayEnd(bool bPlayEnd);

	void AddAnimation(CAnimationBase* pAnimation);
	void RemoveAnimation(CAnimationBase* pAnimation);

	CAnimationBase* GetAnimationByIndex(int nIndex);
	int GetAnimationCount();

protected:
	virtual void OnStop();
	virtual void OnChildStop(CAnimationBase* pChild);

	CAnimationBase* GetAnimationByType(int nType);
	void ClearAnimation();

protected:
	VEC_ANIMATION m_vecAnimation;
};

#endif
