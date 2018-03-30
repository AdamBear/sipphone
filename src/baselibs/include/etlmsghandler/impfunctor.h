#ifndef __IMP_FUNCTOR_H__
#define __IMP_FUNCTOR_H__

#include "ETLLib.hpp"
#include "dskthread/msgobject.h"
// 仿函数基类
class FunctorImpBase
{
public:
    virtual ~FunctorImpBase() {};
    virtual bool operator()(msgObject & msg) = 0;
    virtual bool operator==(const FunctorImpBase & functor) const = 0;
    // 执行深拷贝
    virtual FunctorImpBase * clone()
    {
        return NULL;
    };

    virtual void * dump_addr()
    {
        return NULL;
    }
};

// 仿函数:全局函数指针
class FreeFunctionImp : public FunctorImpBase
{
public:
    typedef LRESULT(*FreeFunctionType)(msgObject &);

    FreeFunctionImp(FreeFunctionType func)
        : m_function(func)
    {}

    virtual bool operator()(msgObject & msg)
    {
        return m_function(msg);
    }

    virtual bool operator==(const FunctorImpBase & functor) const
    {
        FreeFunctionImp freefunctor = (FreeFunctionImp &)functor;
        return (freefunctor.m_function == m_function);
    }

    virtual FunctorImpBase * clone()
    {
        return new FreeFunctionImp(*this);
    }

    //virtual void * dump_addr()
    //{
    //    return (void *)m_function;
    //}
private:
    FreeFunctionType    m_function;
};

// 仿函数:成员函数指针
template<typename T>
class MemberFunctionImp : public FunctorImpBase
{
public:
    typedef bool (T::*MemberFunctionType)(msgObject &);

    MemberFunctionImp(MemberFunctionType func, T * obj)
        : m_function(func)
        , m_pObject(obj)
    {}

    virtual bool operator()(msgObject & msg)
    {
        return (m_pObject->*m_function)(msg);
    }

    virtual bool operator==(const FunctorImpBase & functor) const
    {
        MemberFunctionImp memfunctor = (MemberFunctionImp &)functor;
        return (memfunctor.m_function == m_function && memfunctor.m_pObject == m_pObject);
    }

    virtual FunctorImpBase * clone()
    {
        return new MemberFunctionImp(*this);
    }

    //virtual void * dump_addr()
    //{
    //    return (void *) (m_function);
    //}

private:
    MemberFunctionType  m_function;
    T         *         m_pObject;
};

#endif // __IMP_FUNCTOR_H__
