#ifndef __X_KERNEL_DEFINE_H__
#define __X_KERNEL_DEFINE_H__


// 缓存消息的最大数量
#define MAX_MSG_OBJECT_CACHE_COUNT 20

// 是否线程较验
#define IF_XKERNEL_THREAD_CHECK 0

// 是否使用多个Kernel
#define IF_MULTI_XKERNEL_ENABLE 0

// 是否支持多线程
#define IF_MULTI_THREAD_ENABLE 0

// 是否支持多线程消息
#define IF_XKERNEL_MULTI_THREAD_MSG 0

// 是否支持退出自定义处理
#define IF_XTHREAD_LOCAL_EXIT_METHOD 0

// 是否使用命名空间
#define IF_XKERNEL_NAMESPACE_ENABLE 1

#if IF_XKERNEL_NAMESPACE_ENABLE
#define BEGIN_XKERNEL_NAMESPACE     namespace xKernel {
#define END_XKERNEL_NAMESPACE       }
#define USING_XKERNEL_NAMESPACE     using namespace xKernel;
#else
#define BEGIN_XKERNEL_NAMESPACE
#define END_XKERNEL_NAMESPACE
#define USING_XKERNEL_NAMESPACE
#endif

#if WINDOWS_SYSTEM
#define DECLARE_EXPORT __declspec(dllexport)
#define DECLARE_IMPORT __declspec(dllimport)
#define DECLARE_CONST const
#else
#define DECLARE_EXPORT
#define DECLARE_IMPORT
#define DECLARE_CONST
#endif

#if XKERNEL_EXPORTS
#define XKERNEL_EXPORT DECLARE_EXPORT
#else
#define XKERNEL_EXPORT DECLARE_IMPORT
#endif

#define NULL_METHOD         (0)

//////////////////////////////////////////////////////////////////////////
/* address offset map */
#define castOBJECT(pMember, group, member)      (group*)((POINTER)pMember - (POINTER)(&((group*)pMember)->member) + (POINTER)pMember)

#if IF_DEBUG_ENABLE
#define chASSERTx(expr, ...)
#define chWARNINGx(expr, ...)
#else
#define chASSERTx(expr, ...)
#define chWARNINGx(expr, ...)
#endif

//////////////////////////////////////////////////////////////////////////
// globle object
#define REFERENCE_XGLOBLE_OBJECT(type)      type::getGlobalObject()
#define DECLARE_XGLOBLE_OBJECT(type)        static type& getGlobalObject()
#define IMPLEMENT_XGLOBLE_OBJECT(type)      IMPLEMENT_XGLOBLE_OBJECT_EX(type, pObject = pObject)
#define IMPLEMENT_XGLOBLE_OBJECT_EX(type, postAction)   type& type::getGlobalObject()   \
                                                        { \
                                                        static type* pObject = NULL; \
                                                        if(pObject == NULL) \
                                                            { \
                                                            static chCriticalSection lockThisObj;\
                                                            if(lockThisObj.IsLocked()) XWINDOW_WARN("safe re-enter of " #type " "#type"::getGlobalObject().");\
                                                            lockThisObj.Lock(); \
                                                            static type onlyGlobalObject; \
                                                            pObject = &onlyGlobalObject; \
                                                            postAction; \
                                                            lockThisObj.Unlock(); \
                                                            } \
                                                            return *pObject;\
                                                        }

#define ASSERT_XGLOBAL_OBJECT()             static void* pData = NULL; chASSERT(pData == NULL); pData = this

BEGIN_XKERNEL_NAMESPACE
enum
{
    XEVENT_TYPE_NONE        = 0,
    XEVENT_TYPE_INIT        = 1,
    XEVENT_TYPE_QUIT        = 2,
    XEVENT_TYPE_CUSTOM      = 10,
};
END_XKERNEL_NAMESPACE

#endif
