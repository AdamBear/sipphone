#ifndef __MSG_SUBSCRIBER_H__
#define __MSG_SUBSCRIBER_H__

#include "impfunctor.h"

// 事件订阅者，提供事件响应的接口
class CMSGSubscriber
{
public:
    CMSGSubscriber()
        : m_pFunctorImpl(NULL)  {}

    CMSGSubscriber(const CMSGSubscriber & other)
        : m_pFunctorImpl(NULL)
    {
        if (other.m_pFunctorImpl != NULL)
        {
            m_pFunctorImpl = other.m_pFunctorImpl->clone();
        }
    }

    // 全局函数类型的订阅者
    explicit CMSGSubscriber(FreeFunctionImp::FreeFunctionType func)
        : m_pFunctorImpl(new FreeFunctionImp(func))  {}

    // 成员函数类型的订阅者
    template<typename T>
    explicit CMSGSubscriber(T * obj, bool (T::*function)(msgObject &))
        : m_pFunctorImpl(new MemberFunctionImp<T>(function, obj))
    {
    }

    ~CMSGSubscriber()
    {
        delete m_pFunctorImpl;
    }

    CMSGSubscriber & operator =(const CMSGSubscriber & rhs)
    {
        CMSGSubscriber tmp(rhs);
        SwapObject(m_pFunctorImpl, tmp.m_pFunctorImpl);
        return *this;
    }

public:
    // 仿函数，用于响应事件
    bool operator()(msgObject & msg) const
    {
        if (m_pFunctorImpl == NULL)
        {
            return false;
        }
        return (*m_pFunctorImpl)(msg);
    }

    // 判断是否同一个订阅者
    bool operator==(const CMSGSubscriber & subcriber) const
    {
        if (subcriber.m_pFunctorImpl == NULL || m_pFunctorImpl == NULL)
        {
            return false;
        }
        return (*subcriber.m_pFunctorImpl == *m_pFunctorImpl);
    }

    void * DumpAddr() const
    {
        return m_pFunctorImpl->dump_addr();
    }

private:
    // 仿函数信号槽接口指针
    FunctorImpBase * m_pFunctorImpl;
};

#endif // __SUBSCRIBER_H__
