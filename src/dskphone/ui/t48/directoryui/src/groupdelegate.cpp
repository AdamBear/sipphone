#include <algorithm>
#include "baseui/framelist.h"
#include "groupdelegate.h"
#include "directorycommon.h"
#include "directorymgr.h"


CGroupDelegate::CGroupDelegate(int nType)
    : m_nType(nType)
    , m_bSetting(false)
{
    m_bItemFocus = false;
    m_bDefaultFocus = false;
}

CGroupDelegate::~CGroupDelegate()
{
    m_mapCurMaxId.clear();
    m_mapMinId.clear();
    m_mapMaxId.clear();
    DIRUI_INFO("~CGroupDelegate %p", this);
}

int CGroupDelegate::GetType() const
{
    return m_nType;
}

void CGroupDelegate::SetSetting(bool bSetting)
{
    if (bSetting == m_bSetting)
    {
        return;
    }

    m_bSetting = bSetting;

    InitGroupList();
}

bool CGroupDelegate::IsSetting() const
{
    return m_bSetting;
}

void CGroupDelegate::InitGroupId(int nGroupType, int nMinId, int nMaxId)
{
    m_mapMinId[nGroupType] = nMinId;
    m_mapMaxId[nGroupType] = nMaxId;

    ResetGroupTreeId(nGroupType);
}

int CGroupDelegate::ApplyGroupTreeId(int nGroupType)
{
    int nCurMaxId = 0;
    MAP_GROUP_ID::iterator iter = m_mapCurMaxId.find(nGroupType);

    if (iter != m_mapCurMaxId.end())
    {
        nCurMaxId = iter->second;
    }

    ++nCurMaxId;

    iter = m_mapMaxId.find(nGroupType);

    if (iter != m_mapMaxId.end())
    {
        if (nCurMaxId > iter->second)
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }

    m_mapCurMaxId[nGroupType] = nCurMaxId;

    return nCurMaxId;
}

void CGroupDelegate::ResetGroupTreeId(int nGroupType)
{
    int nCurMaxId = 0;
    MAP_GROUP_ID::iterator iter = m_mapMinId.find(nGroupType);

    if (iter != m_mapMinId.end())
    {
        nCurMaxId = iter->second;
    }

    m_mapCurMaxId[nGroupType] = nCurMaxId;
}

void CGroupDelegate::SetDefaultGroup(int nGroupType, int nGroupId)
{
    CGroupTreeNodePtr pGroupNode = NULL;

    if (knInvalidRId == nGroupId)
    {
        pGroupNode = GetDefaultGroupNode(nGroupType);
    }
    else
    {
        pGroupNode = GetGroupNode(nGroupType, nGroupId);
    }

    if (NULL == pGroupNode)
    {
        return;
    }

    CurlAllNode();

    VEC_TREE_ID vecTreeId;
    int nTreeId = pGroupNode->GetId();

    GetParentTreeIdSet(vecTreeId, nTreeId);
    vecTreeId.insert(vecTreeId.begin(), nTreeId);

    VEC_TREE_ID::reverse_iterator iterBegin = vecTreeId.rbegin();
    VEC_TREE_ID::reverse_iterator iterEnd = vecTreeId.rend();

    for (; iterBegin != iterEnd; ++iterBegin)
    {
        CGroupTreeNodePtr pTempGroup = (CGroupTreeNodePtr)GetTreeDataNode(*iterBegin);

        if (NULL != pTempGroup)
        {
            ExpandNode(pTempGroup);
        }
    }

    m_nUISelectId = nTreeId;

    ReLocateCurrentPage();
}

CGroupTreeNodePtr CGroupDelegate::GetDefaultGroupNode(int nGroupType)
{
    MAP_TREE_DATA_NODE::iterator iter = m_mapTreeNode.begin();

    for (; iter != m_mapTreeNode.end(); ++iter)
    {
        CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)iter->second;

        if (NULL != pGroupNode && pGroupNode->GetType() == nGroupType && 0 == pGroupNode->GetParentId())
        {
            return pGroupNode;
        }
    }

    return NULL;
}

bool CGroupDelegate::IsGroupCanShow(int nGroupType)
{
    CGroupTreeNodePtr pGroupNode = GetDefaultGroupNode(nGroupType);

    if (NULL == pGroupNode)
    {
        return false;
    }

    VEC_TREE_ID::iterator iter = std::find(m_vecTreeId.begin(), m_vecTreeId.end(), pGroupNode->GetId());

    if (iter == m_vecTreeId.end())
    {
        return false;
    }

    return true;
}

CGroupTreeNodePtr CGroupDelegate::GetGroupNode(int nGroupType, int nGroupId)
{
    MAP_TREE_DATA_NODE::iterator iter = m_mapTreeNode.begin();

    for (; iter != m_mapTreeNode.end(); ++iter)
    {
        CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)iter->second;

        if (NULL != pGroupNode && pGroupNode->GetType() == nGroupType
                && pGroupNode->GetGroupId() == nGroupId)
        {
            return pGroupNode;
        }
    }

    return NULL;
}

bool CGroupDelegate::IsExpand(int nGroupTreeId)
{
    CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)GetTreeDataNode(nGroupTreeId);

    if (NULL != pGroupNode)
    {
        return pGroupNode->IsExpanded();
    }

    return false;
}

int CGroupDelegate::GetGroupSelectId(int nGroupType)
{
    CGroupTreeNodePtr pSelGroupNode = (CGroupTreeNodePtr)GetTreeDataNode(m_nUISelectId);

    if (NULL != pSelGroupNode && nGroupType == pSelGroupNode->GetType())
    {
        return pSelGroupNode->GetGroupId();
    }

    return 0;
}

int CGroupDelegate::GetDefaultRecentGroupId(int nGroupType, int nGroupTreeId)
{
    VEC_TREE_ID::iterator iter = std::find(m_vecTreeId.begin(), m_vecTreeId.end(), nGroupTreeId);

    if (iter == m_vecTreeId.end())
    {
        return 0;
    }

    VEC_TREE_ID::iterator iterNext = iter;

    ++iterNext;

    if (iterNext != m_vecTreeId.end())
    {
        CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)GetTreeDataNode(*iterNext);

        if (NULL != pGroupNode && pGroupNode->GetType() == nGroupType)
        {
            return pGroupNode->GetGroupId();
        }
    }

    if (iter == m_vecTreeId.begin())
    {
        return 0;
    }

    VEC_TREE_ID::iterator iterPre = iter;

    --iterPre;

    CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)GetTreeDataNode(*iterPre);

    if (NULL != pGroupNode && pGroupNode->GetType() == nGroupType)
    {
        return pGroupNode->GetGroupId();
    }

    return 0;
}

int CGroupDelegate::GetSelectGroupType()
{
    CGroupTreeNodePtr pSelGroupNode = (CGroupTreeNodePtr)GetTreeDataNode(m_nUISelectId);

    if (NULL != pSelGroupNode)
    {
        return pSelGroupNode->GetType();
    }

    return 0;
}

int CGroupDelegate::GetSelectGroupId()
{
    CGroupTreeNodePtr pSelGroupNode = (CGroupTreeNodePtr)GetTreeDataNode(m_nUISelectId);

    if (NULL != pSelGroupNode)
    {
        return pSelGroupNode->GetGroupId();
    }

    return 0;
}

bool CGroupDelegate::ReInitGroupList(int nGroupType, int nRootTreeId, bool bExpand,
                                     int nGroupSelectId)
{
    CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)GetTreeDataNode(nRootTreeId);

    if (NULL == pGroupNode)
    {
        return false;
    }

    m_vecTreeId.push_back(nRootTreeId);

    std::sort(m_vecTreeId.begin(), m_vecTreeId.end(), std::less<int>());

    if (bExpand)
    {
        ExpandNode(pGroupNode);

        CGroupTreeNodePtr pSelGroupNode = NULL;

        if (0 != nGroupSelectId)
        {
            pSelGroupNode = GetGroupNode(nGroupType, nGroupSelectId);
        }

        if (NULL != pSelGroupNode)
        {
            m_nUISelectId = pSelGroupNode->GetId();
        }
        else if (0 != nGroupSelectId)
        {
            m_nUISelectId = pGroupNode->GetId();
        }
    }

    return true;
}

void CGroupDelegate::ReLocateCurrentPage()
{
    if (NULL == m_pFrameList || m_vecTreeId.empty())
    {
        return;
    }

    VEC_TREE_ID::iterator iter = std::find(m_vecTreeId.begin(), m_vecTreeId.end(), m_nUISelectId);

    if (iter == m_vecTreeId.end())
    {
        iter = m_vecTreeId.begin();
    }

    int nPageDataIndex = std::distance(m_vecTreeId.begin(), iter);
    int nMaxPageItemCount = m_pFrameList->GetMaxPageItemCount();

    if (nMaxPageItemCount > 0)
    {
        m_nFocusIndex = nPageDataIndex % nMaxPageItemCount;
        m_nPageDataIndex = nPageDataIndex - m_nFocusIndex;
    }

    if (m_pFrameList->isVisible())
    {
        m_pFrameList->RefreshData(m_nPageDataIndex);
    }
}

void CGroupDelegate::ClearGroup(int nBeginTreeId, int nEndTreeId)
{
    MAP_TREE_DATA_NODE::iterator iter = m_mapTreeNode.begin();

    while (iter != m_mapTreeNode.end())
    {
        MAP_TREE_DATA_NODE::iterator iterTemp = iter;

        ++iter;

        CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)iterTemp->second;

        if (NULL == pGroupNode)
        {
            m_mapTreeNode.erase(iterTemp);
            continue;
        }

        int nGroupTreeId = pGroupNode->GetId();

        if (nGroupTreeId < nBeginTreeId || nGroupTreeId > nEndTreeId)
        {
            continue;
        }

        VEC_TREE_ID::iterator iterID = std::find(m_vecTreeId.begin(), m_vecTreeId.end(), nGroupTreeId);

        if (iterID != m_vecTreeId.end())
        {
            m_vecTreeId.erase(iterID);
        }

        delete pGroupNode;
        m_mapTreeNode.erase(iterTemp);
    }
}

void CGroupDelegate::ExpandToSelectGroup()
{
    int nUISelId = m_nUISelectId;
    CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)GetTreeDataNode(m_nUISelectId);
    int nGroupType = GetDefaultDirType();
    int nGroupId = 0;

    if (NULL != pGroupNode && IsGroupCanShow(pGroupNode->GetType()))
    {
        nGroupType = pGroupNode->GetType();
        nGroupId = pGroupNode->GetGroupId();
    }

    SetDefaultGroup(nGroupType, nGroupId);

    if (nUISelId != m_nUISelectId)
    {
        ReSelectGroup();
    }
}

void CGroupDelegate::ReSelectGroup()
{
    CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)GetTreeDataNode(m_nUISelectId);
    int nContactType = DIRECTORY_TYPE_NONE;
    int nGroupId = 0;

    if (NULL != pGroupNode)
    {
        nContactType = pGroupNode->GetType();
        nGroupId = pGroupNode->GetGroupId();
    }
    else
    {
        if (0 != m_nUISelectId)
        {
            return;
        }

        VEC_TREE_ID::iterator iter = std::find(m_vecTreeId.begin(), m_vecTreeId.end(), m_nUISelectId);

        if (iter == m_vecTreeId.end())
        {
            return;
        }

        nContactType = GetDefaultDirType();
        nGroupId = kszAllContactId;
    }

    if (NULL != m_pFrameList && m_pFrameList->isVisible())
    {
        CGroupDelegatePtr pGroupDelegate = g_DirectoryMgr.GetGroupDelegate();

        if (pGroupDelegate.Get() == this)
        {
            g_DirectoryMgr.OnGroupClick(nContactType, nGroupId);
        }
    }
}

bool CGroupDelegate::IsDirTypeValid(int nDirType)
{
    MAP_TREE_DATA_NODE::iterator iter = m_mapTreeNode.begin();

    for (; iter != m_mapTreeNode.end(); ++iter)
    {
        CGroupTreeNodePtr pGroupNode = (CGroupTreeNodePtr)iter->second;

        if (NULL != pGroupNode && pGroupNode->GetType() == nDirType)
        {
            return true;
        }
    }

    return false;
}
