#ifndef __wMsgSystem_H__
#define __wMsgSystem_H__
#include "ETLLib/ETLLib.hpp"
#if WINDOWS_SYSTEM
typedef DWORD(__stdcall * W_LPTHREAD_MAIN)(void * lpData);
#define DECLARE_THREAD_METHOD(method)           extern DWORD __stdcall method(void* lpData)
#define IMPLEMENT_THREAD_METHOD(method)         DWORD __stdcall method(void* lpData)
#define LPTHREAD_MAIN                           W_LPTHREAD_MAIN
#endif

#define wCreateThread                           chCreateThread
#define wJoinThread                             chJoinThread
#define wCancelThread                           chCancelThread


#endif //__wMsgSystem_H__
