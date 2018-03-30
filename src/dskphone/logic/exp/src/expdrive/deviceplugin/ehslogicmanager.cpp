#include "ehslogicmanager.h"
#ifdef _EHS_SUPPORT
#include "etlmsghandler/modetlmsghandler.h"
#if IF_FEATURE_EVENT_MODULE
#include "extended/event/include/modevent.h"
#endif

CEHSLogicManager & CEHSLogicManager::GetInstance()
{
	static CEHSLogicManager instance;
	return instance;
}

CEHSLogicManager::~CEHSLogicManager()
{

}

CEHSLogicManager::CEHSLogicManager()
{
	m_iEHSId = -1;
}

void CEHSLogicManager::Init()
{
	etl_RegisterMsgHandle(EXP_IN, EXP_IN, OnEHSManagerMsg);
	etl_RegisterMsgHandle(EXP_OUT, EXP_OUT, OnEHSManagerMsg);
}

bool CEHSLogicManager::InitEHSData()
{
	int iEHSIndex = GetEhsIndex();
	if (iEHSIndex != -1)
	{
		msgPostMsgToThread(msgGetThreadByName(VP_PHONE_THREAD_NAME), DEV_MSG_EHS_INSERT, iEHSIndex, 0);
	}

	return true;
}

LRESULT CEHSLogicManager::OnEHSManagerMsg(msgObject & msg)
{
	DEVICE_TYPE_DEF expType = static_cast<DEVICE_TYPE_DEF>(msg.lParam);

	if (expType != DT_EXP_EHS)
	{
		return FALSE;
	}

	EXP_INFO("EXP msg from virtual-drive is msg [%d], exptype is [%d] exp id[%d]",
		msg.message - EXP_BEG,
		expType - DT_ALL,
		msg.wParam);

	ExpID expId = msg.wParam;

	if (msg.message == EXP_IN)
	{
		g_EHSLogicManager.InitEHSByMsg(expId);
		g_EHSLogicManager.EhsPluginProcess(expId);
	}
	else if (msg.message == EXP_OUT)
	{
		g_EHSLogicManager.EhsRemoveProcess(expId);
	}

	return TRUE;
}

void CEHSLogicManager::EhsPluginProcess(ExpID expId)
{
	PostMsgToApp(DEV_MSG_EHS_INSERT, expId, 0);
	RegisterEHS(expId);
	//EHS ²åÈë
	event_SendConnectedDeviceEvent(EVENT_PERIPHERAL_INFORMATION);
	return;
}

void CEHSLogicManager::EhsRemoveProcess(ExpID expId)
{
	PostMsgToApp(DEV_MSG_EHS_REMOVE, expId, 0);
	RegisterEHS(-1);
	//EHS °Î³ö
	event_SendConnectedDeviceEvent(EVENT_PERIPHERAL_INFORMATION);
	return;
}

void CEHSLogicManager::EHSCtrl(int iExpID, int iCtrlCode)
{
#ifdef TEST_EXP 
	return;
#else // TEST_EXP
	g_RJDrive.EhsCtrl(iExpID, iCtrlCode);
#endif
}

void CEHSLogicManager::InitEHSByMsg(const ExpID expId)
{
	g_RJDrive.InitExpByIndex(expId);
}

int CEHSLogicManager::GetEhsIndex() const
{
	return m_iEHSId;
}
#endif