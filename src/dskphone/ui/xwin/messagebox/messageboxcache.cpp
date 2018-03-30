#include "messageboxcache.h"
#include "messageboxfactory.h"

CMessageBoxCache::CMessageBoxCache()
{
    m_mapCache.clear();
}

CMessageBoxCache & GetMessageBoxCacheInstance()
{
    static CMessageBoxCache s_MessageBoxCache;
    return s_MessageBoxCache;
}

CMessageBoxCache::~CMessageBoxCache()
{

}

CMessageBoxBase * CMessageBoxCache::GetMessageBoxByType(MESSAGEBOX_TYPE eMsgType)
{
    iterCacheMap iter = m_mapCache.find(eMsgType);
    if (iter != m_mapCache.end())
    {
        CMessageboxFactory * pMessageBoxFactory = (iter->second);
        if (NULL != pMessageBoxFactory)
        {
            return pMessageBoxFactory->GetMsgBox();
        }
    }
    else
    {
        CMessageboxFactory * pMessageBoxFactory = new CMessageboxFactory(eMsgType);
        if (NULL != pMessageBoxFactory)
        {
            m_mapCache.insert(eMsgType, pMessageBoxFactory);
            return pMessageBoxFactory->GetMsgBox();
        }
    }

    return NULL;
}

void CMessageBoxCache::PutMessageBox(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int nMsgType = pMessageBox->GetType();
    iterCacheMap iter = m_mapCache.find(nMsgType);
    if (iter != m_mapCache.end())
    {
        CMessageboxFactory * pMessageBoxFactory = (iter->second);
        if (NULL != pMessageBoxFactory)
        {
            pMessageBoxFactory->PutMsgBox(pMessageBox);
        }
    }
    else
    {
#ifndef _WIN32
#warning deletelater
#endif
        delete (pMessageBox);
    }
}

