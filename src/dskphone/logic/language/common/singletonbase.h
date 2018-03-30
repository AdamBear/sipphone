#pragma once

#include "singletonclassdefine.h"

template<typename T>
class SingletonBase
{
public:
    static T & GetInstance() DECLARE_ATTR
    {
        static T s_Instance;
        return s_Instance;
    }

protected:
    SingletonBase() {}
    ~SingletonBase() {}

private:
    SingletonBase(const SingletonBase &);
    SingletonBase & operator=(const SingletonBase &);
};


