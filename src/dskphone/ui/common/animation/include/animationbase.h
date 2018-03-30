#ifndef __ANIMATION_BASE_H__
#define __ANIMATION_BASE_H__

#include "animationdefine.h"
#include "xwindow-kernel/include/xsignal.h"


class CUnifiedTimer;
class CAnimationGroup;
class CParallelAnimationGroup;
class CSequentialAnimationGroup;

class CAnimationBase
{
	friend class CUnifiedTimer;
	friend class CAnimationGroup;
	friend class CParallelAnimationGroup;
	friend class CSequentialAnimationGroup;

public:
	explicit CAnimationBase(int nType = ANIMATION_TYPE_BASE);
	virtual ~CAnimationBase();

public:
	enum ANIMATION_STATE
	{
		ANIMATION_STATE_WAITING,
		ANIMATION_STATE_STOPPED,
		ANIMATION_STATE_PAUSED,
		ANIMATION_STATE_RUNNING,
	};

	enum DELETION_POLICY
	{
		DELETION_POLICY_KEEP_WHEN_STOPPED,
		DELETION_POLICY_DELETE_WHEN_STOPPED,
	};

	enum INTERVAL_RESULT
	{
		INTERVAL_RESULT_NOT_PROCESS,
		INTERVAL_RESULT_WAIT_NEXT_INTERVAL,
		INTERVAL_RESULT_PLAY_ANIMATION,
		INTERVAL_RESULT_STOP_ANIMATION,
	};

public:
	virtual void UnInit();
	virtual int OnInterval(int nMsec);

	void Release();

	void Start(int nDeletionPolicy = DELETION_POLICY_DELETE_WHEN_STOPPED);
	bool Stop();
	void Pause();
	void Resume();

	bool CheckStopAnimation();

	virtual void SetInterval(int nMsec);
	virtual void SetDuration(int nMsec);
	virtual void EnsurePlayEnd(bool bPlayEnd);

	void SetDeletionPolicy(int nDeletionPolicy);

	inline int GetType() const		{ return m_nType; }
	inline int GetState() const		{ return m_nState; }
	inline int GetInterval() const	{ return m_nInterval; }

	CAnimationBase* GetParent() const			{ return m_pParent; }

	bool IsRunning() const;

protected:
	virtual void OnStart(bool bStartTimer);
	virtual void OnStop();
	virtual void OnPause();
	virtual void OnResume();
	virtual void OnChildStop(CAnimationBase* pChild);

	void SetParent(CAnimationBase* pParent);

public:
	xSignal signalStateChanged;

protected:
	int m_nType;
	int m_nState;
	int m_nDeletionPolicy;
	int m_nInterval;
	int m_nDuration;
	int m_nElapsed;
	int m_nAddUpInterval;

	CAnimationBase* m_pParent;
	CUnifiedTimer* m_pTimer;
};

#endif
