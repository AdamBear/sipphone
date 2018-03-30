#include "myclasses.h"

CMyMutex::CMyMutex()
{

}

void CMyMutex::Lock()
{
    chCriticalSection::Lock();
}

void CMyMutex::Unlock()
{
    chCriticalSection::Unlock();
}

//CLock::~CLock()
//{
//  if (NULL != m_pMutex)
//  {
//      m_pMutex.Unlock();
//  }
//}
//
//CLock::CLock(chMutex* pMutex)
//:m_pMutex(pMutex)
//{
//  if (NULL != m_pMutex)
//  {
//      m_pMutex.Lock();
//  }
//}
