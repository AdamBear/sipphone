#ifndef __DSK_THREAD_DEFINE_H__
#define __DSK_THREAD_DEFINE_H__

// //////////////////////////////////////////////////////////////////////////
// 模块全局宏定义
#define MK_MSG_USER_DEFINECONFIG 1   // 开放线程自定义配置参数接口，为了保证兼容，需要在大版本加入
#define MK_SUPPORT_DIAGNOSE      0   //是否支持保存诊断信息

#include "msgobject.h"
#ifdef _MSGKIT
#include "msgkit/mkmessage.h"
#endif // _MSGKIT

//////////////////////////////////////////////////////////////////////////
// some marco for message processer
#ifndef BEGIN_MSG_MAP
#define BEGIN_MSG_MAP(theClass) \
 public: \
     BOOL TryThreadMessage(msgObject& objMessage) \
     { \

#define TRY_MSG_ON_METHOD(msg, func) \
     if (objMessage.message == msg && func(objMessage)) \
         return TRUE;

#define TRY_MSG_RANGE_ON_METHOD(msgFirst, msgLast, func) \
     if (objMessage.message >= msgFirst && objMessage.message <= msgLast && func(objMessage)) \
         return TRUE;

#define CHAIN_MSG_TO_BASE(theChainClass) \
     if (theChainClass::TryThreadMessage(objMessage)) \
         return TRUE;

#define CHAIN_MSG_TO_MEMBER(theChainMember) \
     if ((theChainMember).TryThreadMessage(objMessage)) \
         return TRUE;

#define CHAIN_MSG_ON_METHOD(func) \
     if (func(objMessage)) \
         return TRUE;

#define END_MSG_MAP() \
         return FALSE; \
     }
#endif


#ifndef _MSGKIT
typedef  unsigned long DSK_TARGET_T;
typedef  unsigned long DSK_HANDLE_T;
typedef  unsigned long DSK_NOTIFY_T;
typedef  void     *    DSK_MESSAGE_T;

enum eMsgCode
{
    TM_NULL = 0x0000,
    TM_MESSAGE_PROXY = 0x0001,
    TM_TIMER_PROXY = 0x0002,
    TM_SOCKET_PROXY = 0x0003,
    TM_GROUP_PROXY = 0x0004,
    TM_NOP = 0x8000,
    TM_INIT = 0x8001,
    TM_QUIT = 0x8002,
    TM_TIMER = 0x8113,
    TM_SOCKET = 0x8114,
    TM_ABOUT_TO_BLOCK = 0x8121, // 0x0000~0x9000 for ETLLib kernel message
    TM_REGISTRY_CHANGED = 0x9001,   // 0x9001~0xF000 for ETLLib user message: registry ...
    TM_WINDOW_BEGIN = 0xA001,   // 0xA001~0xB000 for ETLLib window message
    TM_ETL_MESSGE_END = 0xFFFF,
    TM_PRIVATE_MESSAGE_BEGIN = 0x10000,
    TM_PRIVATE_MESSAGE_END = 0x1FFFF,
    TM_GROUP_MESSAGE_BEGIN = 0x20000,
    TM_GROUP_MESSAGE_END = 0x2FFFF,
    TM_BROADCAST_MESSAGE_BEGIN = 0x30000,
    TM_BROADCAST_MESSAGE_END = 0x3FFFF,
};
#else
typedef  mkit_target_t DSK_TARGET_T;
typedef  mkit_handle_t DSK_HANDLE_T;
typedef  mkit_notify_t DSK_NOTIFY_T;
typedef  mkit_message_t   DSK_MESSAGE_T;

enum eMsgCode
{
    TM_NULL = MKIT_MSG_INVALID,
    TM_INIT = MKIT_MSG_INIT,
    TM_QUIT = MKIT_MSG_QUIT,
    TM_TIMER = MKIT_MSG_TIMER,
    TM_IDLE = MKIT_MSG_IDLE,    // 0x0000~0x9000 for ETLLib kernel message
    TM_MESSAGE_PROXY = (MKIT_MSG_USR_MIN + 0x001),
    TM_TIMER_PROXY = (MKIT_MSG_USR_MIN + 0x002),
    TM_OBJECT_PROXY = (MKIT_MSG_USR_MIN + 0x003),
    TM_GROUP_PROXY = (MKIT_MSG_USR_MIN + 0x004),
    TM_NOP = (MKIT_MSG_USR_MIN + 0x005),
    TM_REGISTRY_CHANGED = (MKIT_MSG_USR_MIN + 0x006),   // 0x9001~0xF000 for ETLLib user message: registry ...
    TM_WINDOW_BEGIN = (MKIT_MSG_USR_MIN + 0x007),   // 0xA001~0xB000 for ETLLib window message
    TM_THREAD_EXCUTE_END = (MKIT_MSG_USR_MIN + 0x00E),   // for taskaction excute  thread end
    TM_ETL_MESSGE_END = (MKIT_MSG_USR_MIN + 0x00F),
};
#define TM_ABOUT_TO_BLOCK TM_IDLE
#endif

enum eMsgStatue
{
    MT_INVALID = -2,
    MT_UNPOST = -1,
    MT_UNPROCESSED = 0,
    MT_PROCESSING,
    MT_REPLYED,
    MT_PROCESSED,
    MT_NEED_DST_FREE,
    MT_DAMAGED,
};

//class msgObject;
typedef LRESULT(CALLBACK * THREAD_MSG_PROC_EX)(msgObject * pMessage);
//typedef void * (*LPTHREAD_MAIN)(void * lpData);

#define __DSK_THREAD_H__
#endif //__DSK_THREAD_H__
