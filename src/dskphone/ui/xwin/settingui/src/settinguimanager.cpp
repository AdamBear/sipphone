#include "settinguimanager.h"
#include "delegatefactory.h"
#include "uicallbackfun.h"

#include "settinguilogic/include/settingdefine.h"
#include "dsklog/log.h"

CSettingUIManager * CSettingUIManager::s_SettingUIManager = NULL;

CSettingUIManager * CSettingUIManager::GetInstance()
{
    if (NULL == s_SettingUIManager)
    {
        s_SettingUIManager = new CSettingUIManager();
    }

    return s_SettingUIManager;
}

void CSettingUIManager::ReleaseInstance()
{
    if (NULL != s_SettingUIManager)
    {
        delete s_SettingUIManager;
        s_SettingUIManager = NULL;
    }
}

CSettingUIManager::CSettingUIManager()
{
}

CSettingUIManager::~CSettingUIManager()
{
}

void CSettingUIManager::Init()
{
}

void CSettingUIManager::UnInit()
{
}

void CSettingUIManager::Clear()
{
    LIST_SET_UI_DELEGATE_BASE_PTR::iterator it = m_listPageDelegate.begin();
    for (; it != m_listPageDelegate.end(); ++it)
    {
        CSettingUIDelegateBasePtr pSetUIDelegate = (*it);
        if (NULL != pSetUIDelegate)
        {
            pSetUIDelegate->UnInit();
            delete pSetUIDelegate;
            pSetUIDelegate = NULL;
        }
    }

    m_listPageDelegate.clear();
}

bool CSettingUIManager::IsPageDelegateCreate(const yl::string & strPageID)
{
    return (NULL == GetPageDelegate(strPageID)) ? false : true;
}

CSettingUIDelegateBase * CSettingUIManager::GetPageDelegate(const yl::string & strPageID)
{
    LIST_SET_UI_DELEGATE_BASE_PTR::iterator it = m_listPageDelegate.begin();
    for (; it != m_listPageDelegate.end(); ++it)
    {
        CSettingUIDelegateBasePtr pSetUIDelegate = (*it);
        if (NULL != pSetUIDelegate)
        {
            if (0 == strPageID.compare(pSetUIDelegate->GetPageID()))
            {
                return pSetUIDelegate;
            }
        }
    }

    return NULL;
}

bool CSettingUIManager::MoveToTop(const yl::string & strPageID)
{
    CSettingUIDelegateBasePtr pFindSetUIDelegate = NULL;
    LIST_SET_UI_DELEGATE_BASE_PTR::iterator it = m_listPageDelegate.begin();
    for (; it != m_listPageDelegate.end(); ++it)
    {
        CSettingUIDelegateBasePtr pSetUIDelegate = (*it);
        if (NULL != pSetUIDelegate)
        {
            if (0 == strPageID.compare(pSetUIDelegate->GetPageID()))
            {
                pFindSetUIDelegate = pSetUIDelegate;
                m_listPageDelegate.erase(it);
                break;
            }
        }
    }

    if (NULL != pFindSetUIDelegate)
    {
        m_listPageDelegate.push_back(pFindSetUIDelegate);
        return true;
    }

    return false;
}

CSettingUIDelegateBase * CSettingUIManager::GetTopDelegate()
{
    if (0 >= m_listPageDelegate.size())
    {
        return NULL;
    }

    return m_listPageDelegate.back();
}

bool CSettingUIManager::AddDelegate(CSettingUIDelegateBase * pDelegate)
{
#if IF_SETTING_BACK_DEBUG
    SETTINGUI_INFO("[CSettingUIManager::AddDelegate(*)] before add, totNum[%d]"
                   , m_listPageDelegate.size());
#endif
    if (NULL == pDelegate)
    {
        return false;
    }

    m_listPageDelegate.push_back(pDelegate);
#if IF_SETTING_BACK_DEBUG
    SETTINGUI_INFO("[CSettingUIManager::AddDelegate(*)] before after, totNum[%d]"
                   , m_listPageDelegate.size());
#endif
    return true;
}

bool CSettingUIManager::DelDelegate(CSettingUIDelegateBase * pDelegate)
{
#if IF_SETTING_BACK_DEBUG
    SETTINGUI_INFO("[CSettingUIManager::DelDelegate(*)] before del [%p], totNum[%d]"
                   , pDelegate, m_listPageDelegate.size());
#endif
    LIST_SET_UI_DELEGATE_BASE_PTR::iterator it = m_listPageDelegate.begin();
    for (; it != m_listPageDelegate.end(); ++it)
    {
        CSettingUIDelegateBasePtr pSetUIDelegate = (*it);
        if (NULL != pSetUIDelegate && pSetUIDelegate == pDelegate)
        {
            pSetUIDelegate->UnInit();
            delete pSetUIDelegate;
            pSetUIDelegate = NULL;
            m_listPageDelegate.erase(it);
#if IF_SETTING_BACK_DEBUG
            SETTINGUI_INFO("[CSettingUIManager::DelDelegate(*)] after del, totNum[%d]"
                           , m_listPageDelegate.size());
#endif
            return true;
        }
    }
    return false;
}

bool CSettingUIManager::DelDelegateFromTop(CSettingUIDelegateBase * pEndDelegate)
{
#if IF_SETTING_BACK_DEBUG
    SETTINGUI_INFO("[CSettingUIManager::DelDelegateFromTop(*)] before del [%p], totNum[%d]"
                   , pEndDelegate, m_listPageDelegate.size());
#endif
    if (NULL != pEndDelegate && IsPageDelegateCreate(pEndDelegate->GetPageID()))
    {
        while (m_listPageDelegate.size() > 0)
        {
            CSettingUIDelegateBasePtr pSetUIDelegate = m_listPageDelegate.back();

            m_listPageDelegate.pop_back();

            if (NULL == pSetUIDelegate)
            {
                continue;
            }

            pSetUIDelegate->UnInit();
            delete pSetUIDelegate;

            if (pSetUIDelegate == pEndDelegate)
            {
                break;
            }

            pSetUIDelegate = NULL;
        }

    }

#if IF_SETTING_BACK_DEBUG
    SETTINGUI_INFO("[CSettingUIManager::DelDelegateFromTop(*)] after del, totNum[%d]"
                   , m_listPageDelegate.size());
#endif
    return true;
}

bool CSettingUIManager::DelDelegateRange(CSettingUIDelegateBase * pBeginDelegate,
        CSettingUIDelegateBase * pEndDelegate, bool bCheck)
{
    if (NULL == pEndDelegate || NULL == pBeginDelegate)
    {
#if IF_SETTING_BACK_DEBUG
        SETTINGUI_INFO("[CSettingUIManager::DelDelegateRange] delegate is null");
#endif
        return true;
    }

    bool bTheSame = (pBeginDelegate == pEndDelegate);
    if (bCheck)
    {
        if (!IsPageDelegateCreate(pBeginDelegate->GetPageID())
                || !IsPageDelegateCreate(pEndDelegate->GetPageID()))
        {
            SETTINGUI_WARN("[CSettingUIManager::DelDelegateRange] start and end delegate not all be in list. Please re-confirm.");
            return false;
        }
    }

    if (bTheSame)
    {
        return DelDelegate(pBeginDelegate);
    }

    LIST_SET_UI_DELEGATE_BASE_PTR::iterator it = m_listPageDelegate.begin();
    bool bFindEdge = false;
    bool bDealEnd = false;
    for (; it != m_listPageDelegate.end();)
    {
        CSettingUIDelegateBasePtr pSetUIDelegate = (*it);
        if (NULL != pSetUIDelegate)
        {
            if (pSetUIDelegate == pBeginDelegate || pSetUIDelegate == pEndDelegate)
            {
                if (!bFindEdge)
                {
                    bFindEdge = true;
                }
                else
                {
                    bDealEnd = true;
                }
            }
            // 删除Delegate
            if (bFindEdge)
            {
                pSetUIDelegate->UnInit();
                delete pSetUIDelegate;
                pSetUIDelegate = NULL;
                it = m_listPageDelegate.erase(it);
            }
            else
            {
                ++it;
            }

            if (bDealEnd)
            {
                return true;
            }
        }
        else
        {
            ++it;
        }
    }

    return true;
}

bool CSettingUIManager::DelDelegate(int iNum, int eDelType /*= DEL_TYPE_FROM_START*/,
                                    int iStIndx /*= 0*/)
{
#if IF_SETTING_BACK_DEBUG
    SETTINGUI_INFO("[CSettingUIManager::DelDelegate(int)] before del, totNum[%d]"
                   , m_listPageDelegate.size());
#endif
    switch (eDelType)
    {
    case DEL_TYPE_FROM_START:
        {
            YLList<CSettingUIDelegateBase*>::iterator it = m_listPageDelegate.begin();

            for (; it != m_listPageDelegate.end(); ++it)
            {
                CSettingUIDelegateBasePtr pSetUIDelegate = (*it);

                if (NULL != pSetUIDelegate)
                {
                    pSetUIDelegate->UnInit();
                    delete pSetUIDelegate;
                    pSetUIDelegate = NULL;
                }
            }

            m_listPageDelegate.clear();
        }
        break;
    case DEL_TYPE_FROM_TOP:
        {
            int iListSize = m_listPageDelegate.size();
            while (iNum > 0 && iListSize > 0)
            {
                CSettingUIDelegateBasePtr pSetUIDelegate = m_listPageDelegate.back();
                if (NULL != pSetUIDelegate)
                {
                    pSetUIDelegate->UnInit();
                    delete pSetUIDelegate;
                    pSetUIDelegate = NULL;
                    m_listPageDelegate.pop_back();
                    --iListSize;
                    --iNum;
                }
                else
                {
                    break;
                }
            }
        }
        break;
    case DEL_TYPE_CUSTOM:
        {
            LIST_SET_UI_DELEGATE_BASE_PTR::iterator it = m_listPageDelegate.begin();
            for (int iCur = 0; it != m_listPageDelegate.end() && 0 < iNum; ++iCur)
            {
                CSettingUIDelegateBasePtr pSetUIDelegate = (*it);
                if (NULL != pSetUIDelegate)
                {
                    if (iCur >= iStIndx)
                    {
                        pSetUIDelegate->UnInit();
                        delete pSetUIDelegate;
                        pSetUIDelegate = NULL;
                        it = m_listPageDelegate.erase(it);
                        --iNum;
                    }
                }
                else
                {
                    ++it;
                }
            }
        }
        break;
    default:
        break;
    }

#if IF_SETTING_BACK_DEBUG
    SETTINGUI_INFO("[CSettingUIManager::DelDelegate(int)] after del, totNum[%d]"
                   , m_listPageDelegate.size());
#endif

    return true;
}

