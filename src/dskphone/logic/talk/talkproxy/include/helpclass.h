#pragma once

#include <cstddef>
#include "singletonclassdefine.h"


namespace app
{

/* 栈对象 */
struct StackObjectBase
{
private:

#ifdef new
#   undef new
    static void* operator new (size_t);
    static void* operator new[](size_t);
    static void* operator new (size_t, void*);
    static void* operator new[](size_t, void*);

#   define new DEBUG_NEW
#else
    static void* operator new (size_t);
    static void* operator new[](size_t);
    static void* operator new (size_t, void*);
    static void* operator new[](size_t, void*);
#endif

    static void operator delete (void*);
    static void operator delete[](void*);
    static void operator delete (void*, void*);
    static void operator delete[](void*, void*);
};

class noncopyable
{
private:
    noncopyable(const noncopyable&);
    noncopyable& operator=(const noncopyable&);
};

/* 单例对象 */
template<typename T>
struct SingletonObject
{
public:
    static T& GetInstance() DECLARE_ATTR
    {
        static T s_Instance;
        return s_Instance;
    }
};

template<typename T>
struct AutoRefGuard
{
    typedef T size_type;

    explicit inline AutoRefGuard(T& t) : value(t)
    {
        ++value;
    }

    inline ~AutoRefGuard()
    {
        --value;
    }

    T& value;
};
}

typedef app::AutoRefGuard<size_t> AUTO_REF_COUNT;
