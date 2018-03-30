#ifndef __SEQUENTIAL_ANIMATION_GROUP_H__
#define __SEQUENTIAL_ANIMATION_GROUP_H__

#include "animationgroup.h"


class CSequentialAnimationGroup : public CAnimationGroup
{
public:
	explicit CSequentialAnimationGroup(int nType = ANIMATION_TYPE_SEQUENTIAL_GROUP);
	virtual ~CSequentialAnimationGroup();

	static CSequentialAnimationGroup* GetSequentialAnimation(CAnimationBase* pAnimation);

public:
	virtual int OnInterval(int nMsec);

protected:
	virtual void OnStart(bool bStartTimer);

	bool StartNextAnimation();
	bool IsLastActiveAnimation();

protected:
	int m_nActiveIndex;
};

#endif
