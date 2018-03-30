#ifdef IF_FEATURE_DIAGNOSE
#include "notifymanager.h"
#include <etlmsghandler/modetlmsghandler.h>
#include "dsklog/log.h"
#include "service_dsk_interface.h"

static LRESULT OnNotifyMessage(msgObject & msg)
{
	return g_objNotifyManager.OnMessage(msg);
}

CNotifyManager::CNotifyManager()
{

}

CNotifyManager::~CNotifyManager()
{
	mkit_notify_destroy(m_handleApp);
	mkit_notify_destroy(m_handleTalk);
	mkit_notify_destroy(m_handleDir);
	mkit_notify_destroy(m_handleMenu);
	etl_UnregisterMsgHandle(MKIT_MSG_INIT, MKIT_MSG_INIT, OnNotifyMessage);
	etl_UnregisterMsgHandle(MKIT_MSG_REQ_NOTIFY, MKIT_MSG_REQ_NOTIFY, OnNotifyMessage);
	etl_UnregisterMsgHandle(MKIT_MSG_DEL_NOTIFY, MKIT_MSG_DEL_NOTIFY, OnNotifyMessage);
}

NOTIFY_HANDLE CNotifyManager::GetNotifyHandle( UINT uMsg )
{
	if (uMsg >= DSK_MSG_TALK_INFO_MIN
	    && uMsg <= DSK_MSG_TALK_INFO_MAX)
	{
		return m_handleTalk;
	}
	else if (uMsg >= DSK_MSG_CONTACT_INFO_MIN
	         && uMsg <= DSK_MSG_CONTACT_INFO_MAX)
	{
		return m_handleDir;
	}
	else if (uMsg >= DSK_MSG_APP_INFO_MIN
	         && uMsg <= DSK_MSG_APP_INFO_MAX)
	{
		return m_handleApp;
	}

	return m_handleMenu;
}

CNotifyManager & CNotifyManager::GetInstance()
{
	static CNotifyManager m_instance;
	return m_instance;
}

// 主线程处理msgkit的默认消息
LRESULT CNotifyManager::OnMessage(msgObject & objMessage)
{
	switch (objMessage.message)
	{
	case MKIT_MSG_INIT:
	{
		APP_INFO("MKIT_MSG_INIT [%ld]", mkGetCurrent());
	}
	break;
	case MKIT_MSG_REQ_NOTIFY:
	{
		APP_INFO("Add  Request Notify[%u] event[%x] count[%d]", objMessage.target, objMessage.wParam, objMessage.lParam);

		if (objMessage.wParam == 0)
		{
			mkit_notify_add(m_handleApp, objMessage.target, objMessage.lParam);
			mkit_notify_add(m_handleMenu, objMessage.target, objMessage.lParam);
			mkit_notify_add(m_handleTalk, objMessage.target, objMessage.lParam);
			mkit_notify_add(m_handleDir, objMessage.target, objMessage.lParam);
		}
		else
		{
			mkit_notify_add(GetNotifyHandle(objMessage.wParam), objMessage.target, objMessage.lParam);
		}
	}
	break;
	case MKIT_MSG_DEL_NOTIFY:
	{
		APP_INFO("Del  Request Notify [%u] event[%x]", objMessage.target, objMessage.wParam);
		if (objMessage.wParam == 0)
		{
			mkit_notify_del(m_handleApp, objMessage.target);
			mkit_notify_del(m_handleMenu, objMessage.target);
			mkit_notify_del(m_handleTalk, objMessage.target);
			mkit_notify_del(m_handleDir, objMessage.target);
		}
		else
		{
			mkit_notify_del(GetNotifyHandle(objMessage.wParam), objMessage.target);
		}
	}
	break;
	default:
		break;
	}
	return 0;
}

// 初始化
void CNotifyManager::Init()
{
	mkservice_init(NULL, 0);
	mkit_notify_create(&m_handleApp, mkGetHandle(), 8);
	mkit_notify_create(&m_handleMenu, mkGetHandle(), 8);
	mkit_notify_create(&m_handleTalk, mkGetHandle(), 8);
	mkit_notify_create(&m_handleDir, mkGetHandle(), 8);

	etl_RegisterMsgHandle(MKIT_MSG_INIT, MKIT_MSG_INIT, OnNotifyMessage);
	etl_RegisterMsgHandle(MKIT_MSG_REQ_NOTIFY, MKIT_MSG_REQ_NOTIFY, OnNotifyMessage);
	etl_RegisterMsgHandle(MKIT_MSG_DEL_NOTIFY, MKIT_MSG_DEL_NOTIFY, OnNotifyMessage);
}

// 发送notify ,无扩展参数
bool CNotifyManager::Notify2ReqApp(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return etl_NotifyReqApp(bSync, GetNotifyHandle(uMsg), uMsg, wParam, lParam);
}

// 发送notify ,带扩展参数
bool CNotifyManager::Notify2ReqAppEx(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                     int nExtraSize, LPCVOID lpExtraData)
{
//	APP_INFO("notify[%x] w[%d] l[%d]", uMsg, wParam, lParam);
	return etl_NotifyReqAppEx(bSync, GetNotifyHandle(uMsg), uMsg, wParam, lParam, nExtraSize, lpExtraData);
}
#endif // IF_FEATURE_DIAGNOSE
