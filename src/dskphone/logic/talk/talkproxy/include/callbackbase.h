#pragma once

#include <cstddef>
#include "dskthread/msgobject.h"

#define CLS_LOG_NAME(cls)       cls(__FUNCTION__)

#if (__cplusplus >= 201103L) || defined(WIN32) // || __GXX_EXPERIMENTAL_CXX0X__
#   define IF_SUPPORT_CPP11_FUNCTION_TEMPLATE          1
#   define IF_SUPPORT_STD_ALGORITHM                    1
#else
// support cpp11(201103L) function template
#   define IF_SUPPORT_CPP11_FUNCTION_TEMPLATE          0
// using std algorithm or yl algorithm template
/* warrring: 基于空间考虑，std实现编译增大15KB */
#   define IF_SUPPORT_STD_ALGORITHM                    0
#endif

#if IF_SUPPORT_STD_ALGORITHM
#   include <algorithm>

namespace app
{
using std::find;
using std::unique;
using std::transform;
using std::stable_sort;
}
#else // IF_SUPPORT_STD_ALGORITHM
#   include "ylstringalgorithm.h"
#   include "ylalgorithm.h"
#   include "stable_sort.hpp"

namespace app
{
using yl::find;
using yl::unique;
using yl::transform;
using yl::stable_sort;
}
#endif // IF_SUPPORT_STD_ALGORITHM

// empty class for callback category
struct CCallBackBase
{
};

struct SignalParams;
struct CListenInfo;
class CScopedTimer;

#if IF_SUPPORT_CPP11_FUNCTION_TEMPLATE
#   ifdef new
#       undef new
#       include <functional>
#       define new DEBUG_NEW
#   else
#       include <functional>
#   endif

typedef std::function<int(CScopedTimer&)>						  TimerCallBackFunctor;
typedef std::function<int(const SignalParams&, CListenInfo*)>     ListenCallBackFunctor;

#   define MAKE_TIMER_CALL_BACK_FUN(obj, cb)      std::bind(cb, obj, std::placeholders::_1)
#   define MAKE_LISTEN_CALL_BACK_FUN(obj, cb)     std::bind(cb, obj, std::placeholders::_1, std::placeholders::_2)

#else // IF_SUPPORT_CPP11_FUNCTION_TEMPLATE

namespace app
{
/* wapper for member function */
template<typename T, typename... ARGS>
class CMemCallBackWapper
{
public:
    typedef T*      object_point;
    typedef int(T::*CallBackFunctor)(ARGS...);

    int operator()(ARGS&& ... args) const
    {
        if (!operator bool())
        {
            return false;
        }

        return (m_pObject->*m_pCallBack)(std::forward<ARGS>(args)...);
    }

    explicit CMemCallBackWapper(object_point obj = NULL, CallBackFunctor cb = NULL)
        : m_pObject(obj), m_pCallBack(cb)
    {}

    operator bool() const
    {
        return m_pObject && m_pCallBack;
    }

protected:
    object_point        m_pObject;
    CallBackFunctor     m_pCallBack;
};
}

typedef app::CMemCallBackWapper<CCallBackBase, CScopedTimer&>							TimerCallBackFunctor;
typedef app::CMemCallBackWapper<CCallBackBase, const SignalParams&, CListenInfo*>       ListenCallBackFunctor;

#   define MAKE_TIMER_CALL_BACK_FUN(obj, cb)      TimerCallBackFunctor(static_cast<TimerCallBackFunctor::object_point>(obj), static_cast<TimerCallBackFunctor::CallBackFunctor>(cb))
#   define MAKE_LISTEN_CALL_BACK_FUN(obj, cb)     ListenCallBackFunctor(static_cast<ListenCallBackFunctor::object_point>(obj), static_cast<ListenCallBackFunctor::CallBackFunctor>(cb))

#endif // IF_SUPPORT_CPP11_FUNCTION_TEMPLATE

#define MAKE_TIMER_CALL_BACK(cb)          MAKE_TIMER_CALL_BACK_FUN(this, &cb)
#define MAKE_LISTEN_CALL_BACK(cb)         MAKE_LISTEN_CALL_BACK_FUN(this, &cb)
