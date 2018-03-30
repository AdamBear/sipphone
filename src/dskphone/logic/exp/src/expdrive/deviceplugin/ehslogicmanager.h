#ifndef __EHS_MANAGER_H__
#define __EHS_MANAGER_H__
#ifdef _EHS_SUPPORT
#include "exp_struct.h"
#include "rjdrive.h"
#include "exp/src/expdrive/expdriveheader/expscannerdefine.h"
#include "exp/include/explogicdefine.h"

class CEHSLogicManager 
{
private:
	CEHSLogicManager();
	~CEHSLogicManager();
	CEHSLogicManager(const CEHSLogicManager &);

public:
	static CEHSLogicManager & GetInstance();

	void Init();

	bool InitEHSData();
	
	static LRESULT OnEHSManagerMsg(msgObject & msg);

	void EhsPluginProcess(ExpID expId);

	void EhsRemoveProcess(ExpID expId);

	int GetEhsIndex() const;

	void InitEHSByMsg(const ExpID expId);

	void EHSCtrl(int iExpID, int iCtrlCode);

private:
	inline void RegisterEHS(ExpID eId)
	{
		m_iEHSId = eId;
	}

private:

	ExpID m_iEHSId;
};

#define g_EHSLogicManager  CEHSLogicManager::GetInstance()

#endif
#endif//__EHS_MANAGER_H__
