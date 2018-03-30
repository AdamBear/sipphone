#ifndef _MYCLASSES_H
#define _MYCLASSES_H

#include <ETLLib.hpp>

class CMyMutex : public chCriticalSection
{
public:
    CMyMutex();

public:
    void Lock();
    void Unlock();
};

//class CLock
//{
//public:
//  explicit CLock(chMutex* pMutex);
//
//  ~CLock();
//
//private:
//  chMutex* m_pMutex;
//};

#endif
