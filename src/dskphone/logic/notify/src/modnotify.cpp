#ifdef IF_FEATURE_DIAGNOSE
#include "../include/modnotify.h"
#include "notifymanager.h"

void notify_Init()
{
    g_objNotifyManager.Init();
}

bool notify_Notify2ReqApp(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return g_objNotifyManager.Notify2ReqApp(bSync, uMsg, wParam, lParam);
}

bool notify_Notify2ReqAppEx(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam, int nExtraSize,
                            LPCVOID lpExtraData)
{
    return g_objNotifyManager.Notify2ReqAppEx(bSync, uMsg, wParam, lParam, nExtraSize, lpExtraData);
}
#endif // IF_FEATURE_DIAGNOSE
