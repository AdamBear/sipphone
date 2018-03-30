#ifndef __PARALLEL_ANIMATION_GROUP_H__
#define __PARALLEL_ANIMATION_GROUP_H__

#include "animationgroup.h"


class CParallelAnimationGroup : public CAnimationGroup
{
public:
	explicit CParallelAnimationGroup(int nType = ANIMATION_TYPE_PARALLEL_GROUP);
	virtual ~CParallelAnimationGroup();

	static CParallelAnimationGroup* GetParallelAnimation(CAnimationBase* pAnimation);

public:
	virtual int OnInterval(int nMsec);

protected:
	virtual void OnStart(bool bStartTimer);
};

#endif
