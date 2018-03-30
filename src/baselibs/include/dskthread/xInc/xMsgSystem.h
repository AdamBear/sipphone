#ifndef __xMsgSystem_H__
#define __xMsgSystem_H__
#include "ETLLib/ETLLib.hpp"
#if LINUX_SYSTEM
typedef void * (*X_LPTHREAD_MAIN)(void * lpData);
#define DECLARE_THREAD_METHOD(method)               extern void* method(void* lpData)
#define IMPLEMENT_THREAD_METHOD(method)             void* method(void* lpData)
#define LPTHREAD_MAIN                               X_LPTHREAD_MAIN

#define xCreateThread                           chCreateThread
#define xJoinThread                             chJoinThread
#define xCancelThread                           chCancelThread

#endif



#endif //__xMsgSystem_H__
