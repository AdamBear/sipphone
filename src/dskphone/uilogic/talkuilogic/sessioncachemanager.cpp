#include "sessioncachemanager.h"

CTalkUISessionCacheManager & CTalkUISessionCacheManager::GetInstance()
{
    static CTalkUISessionCacheManager s_objSessionCacheManager;

    return s_objSessionCacheManager;
}

CTalkUISessionCacheManager::CTalkUISessionCacheManager()
{
    m_listSessionCacheData.clear();
}


CTalkUISessionCacheManager::~CTalkUISessionCacheManager()
{

}

bool CTalkUISessionCacheManager::Init()
{
    m_listSessionCacheData.clear();

    return true;
}

void CTalkUISessionCacheManager::Uninit()
{

}

yl::string CTalkUISessionCacheManager::GetProcessorNameTrans(int iSessionID,
        const yl::string & strProcessorNameSrc)
{
    talkui_list<CTalkUISessionCacheData>::iterator iteratorList = m_listSessionCacheData.begin();

    for (; m_listSessionCacheData.end() != iteratorList; iteratorList++)
    {
        CTalkUISessionCacheData & objSessionCacheData = *iteratorList;

        if (objSessionCacheData.iSessionID == iSessionID)
        {
            return objSessionCacheData.GetProcessorNameDstByProcessorNameSrc(strProcessorNameSrc);
        }

    }

    return strProcessorNameSrc;
}

void CTalkUISessionCacheManager::AddProcessorNameTrans(int iSessionID,
        const yl::string & strProcessorNameSrc, const yl::string & strProcessorNameDst)
{
    talkui_list<CTalkUISessionCacheData>::iterator iteratorList = m_listSessionCacheData.begin();

    for (; m_listSessionCacheData.end() != iteratorList; iteratorList++)
    {
        CTalkUISessionCacheData & objSessionCacheData = *iteratorList;

        if (objSessionCacheData.iSessionID == iSessionID)
        {
            objSessionCacheData.AddProcessorNameTrans(strProcessorNameSrc, strProcessorNameDst);
            return ;
        }

    }

    CTalkUISessionCacheData objSessionCacheDataAdd;
    objSessionCacheDataAdd.iSessionID = iSessionID;
    objSessionCacheDataAdd.AddProcessorNameTrans(strProcessorNameSrc, strProcessorNameDst);

    m_listSessionCacheData.push_back(objSessionCacheDataAdd);

}

void CTalkUISessionCacheManager::DeleteProcessorNameTrans(int iSessionID,
        const yl::string & strProcessorNameSrc)
{
    talkui_list<CTalkUISessionCacheData>::iterator iteratorList = m_listSessionCacheData.begin();

    for (; m_listSessionCacheData.end() != iteratorList; iteratorList++)
    {
        CTalkUISessionCacheData & objSessionCacheData = *iteratorList;

        if (objSessionCacheData.iSessionID == iSessionID)
        {
            objSessionCacheData.DeleteProcessorNameTrans(strProcessorNameSrc);
            break;
        }

    }
}

//根据sessionID 清除session数据缓存
void CTalkUISessionCacheManager::ClearSessionCache(int iSessionID)
{
    talkui_list<CTalkUISessionCacheData>::iterator iteratorList = m_listSessionCacheData.begin();

    for (; m_listSessionCacheData.end() != iteratorList; iteratorList++)
    {
        CTalkUISessionCacheData & objSessionCacheData = *iteratorList;

        if (objSessionCacheData.iSessionID == iSessionID)
        {
            m_listSessionCacheData.erase(iteratorList);
            break;
        }
    }

}

//清除所有的session数据缓存
void CTalkUISessionCacheManager::ClearAllSessionCache()
{
    m_listSessionCacheData.clear();
}

