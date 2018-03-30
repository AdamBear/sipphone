#ifndef __NOTIFY_MANAGER_H__
#define __NOTIFY_MANAGER_H__

#ifdef IF_FEATURE_DIAGNOSE
#include <ETLLib.hpp>
#include <msgkit/mkmessage.h>
#include "ylhashmap.h"
#include "ETLLib.hpp"


typedef mkit_notify_t NOTIFY_HANDLE;
class CNotifyManager
{
public:
    static CNotifyManager & GetInstance();

    // 主线程处理msgkit的默认消息
    LRESULT OnMessage(msgObject & objMessage);

    // 初始化
    void Init();

    // 发送notify ,无扩展参数
    bool Notify2ReqApp(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // 发送notify ,带扩展参数
    bool Notify2ReqAppEx(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam,
                         int nExtraSize, LPCVOID lpExtraData);

private:
    CNotifyManager();
    ~CNotifyManager();

	NOTIFY_HANDLE GetNotifyHandle(UINT uMsg);

private:

    // 存放线程对应的通知notify的句柄链表:目前分为4部分
	NOTIFY_HANDLE m_handleApp;
	NOTIFY_HANDLE m_handleMenu;
	NOTIFY_HANDLE m_handleTalk;
	NOTIFY_HANDLE m_handleDir;
};

#define g_objNotifyManager (CNotifyManager::GetInstance())

#endif // IF_FEATURE_DIAGNOSE

#endif // __NOTIFY_MANAGER_H__