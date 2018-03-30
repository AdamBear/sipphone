#include "conflictmanager.h"

CTalkConflictManager::CTalkConflictManager(void)
    : m_eCurrentPriority(CONFLICT_PRIORITY_START)
    , m_bVolumebarVisible(false)
{
    removeAllUnit();
}

CTalkConflictManager::~CTalkConflictManager(void)
{
}

CTalkConflictManager & CTalkConflictManager::GetInstance()
{
    static CTalkConflictManager s_TalkConflictManager;

    return s_TalkConflictManager;
}

void CTalkConflictManager::removeAllUnit()
{
    m_mapConflict.clear();
}

void CTalkConflictManager::addConflictUnit(CONFLICT_PRIORITY ePriority, CONFLICT_UNIT stUnit)
{
    removeConflictUnit(ePriority);
    m_mapConflict.insert(ePriority, stUnit);
}

void CTalkConflictManager::setConflictUnitShow(CONFLICT_PRIORITY ePriority, bool bShow)
{
    ConflictMap::iterator iter = m_mapConflict.find(ePriority);
    if (iter != m_mapConflict.end())
    {
        CONFLICT_UNIT& stUnit = iter->second;
        stUnit.m_bShow = bShow;
    }
}

void CTalkConflictManager::setConflictUnitEnable(CONFLICT_PRIORITY ePriority, bool bEnable)
{
    ConflictMap::iterator iter = m_mapConflict.find(ePriority);
    if (iter != m_mapConflict.end())
    {
        CONFLICT_UNIT& stUnit = iter->second;
        stUnit.m_bEnable = bEnable;
    }
}

void CTalkConflictManager::removeConflictUnit(CONFLICT_PRIORITY ePriority)
{
    ConflictMap::iterator iter = m_mapConflict.find(ePriority);
    if (iter != m_mapConflict.end())
    {
        m_mapConflict.erase(iter);
    }
}

void CTalkConflictManager::checkShowUnit()
{
    CONFLICT_PRIORITY ePriority = CONFLICT_PRIORITY_START;
    int nIndex = (int)ePriority;
    ++nIndex;

    //是否已经找到要显示的控件
    bool bFind = false;

    //按优先级顺序寻找控件
    while (true)
    {
        ConflictMap::iterator iter = m_mapConflict.find((CONFLICT_PRIORITY)nIndex);
        if (iter != m_mapConflict.end())
        {
            CONFLICT_UNIT& stUnit = iter->second;

            //如果当前优先级可以显示，并且目前还没有要显示的控件，那么当前优先级的就可以显示出来
            if (stUnit.m_bEnable && stUnit.m_bShow && !bFind)
            {
                m_eCurrentPriority = (CONFLICT_PRIORITY)nIndex;

                if (NULL != stUnit.m_ShowFunction)
                {
                    stUnit.m_ShowFunction(true);
                }
                //不论是否有函数可以调用，都需要置标志位
                bFind = true;
            }
            //如果当前优先级的不应显示，则需要隐藏
            //如果已经有优先级更高的显示出来，也要隐藏
            else
            {
                if (NULL != stUnit.m_ShowFunction)
                {
                    stUnit.m_ShowFunction(false);
                }
            }
        }

        ++nIndex;
        if (nIndex >= (int)CONFLICT_PRIORITY_END)
        {
            break;
        }
    }
}

CONFLICT_PRIORITY CTalkConflictManager::getCurrentShowUnit()
{
    return m_eCurrentPriority;
}

void CTalkConflictManager::setVolumebarVisible(bool bShow)
{
    m_bVolumebarVisible = bShow;
}

bool CTalkConflictManager::getVolumebarVisible()
{
    return m_bVolumebarVisible;
}
