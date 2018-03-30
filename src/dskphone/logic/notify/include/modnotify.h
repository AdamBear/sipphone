#ifndef __MOD_NOTIFY_H__
#define __MOD_NOTIFY_H__
#ifdef IF_FEATURE_DIAGNOSE

#include <ETLLib.hpp>

void notify_Init();

// 用于进程跟特定进程发送notify消息
bool notify_Notify2ReqApp(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool notify_Notify2ReqAppEx(BOOL bSync, UINT uMsg, WPARAM wParam, LPARAM lParam,
                            int nExtraSize, LPCVOID lpExtraData);

#endif // IF_FEATURE_DIAGNOSE

#endif // __MOD_NOTIFY_H__
