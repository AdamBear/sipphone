#include "treelist.h"
#include "treelistdelegate.h"
#include <algorithm>

CTreeDataNode::CTreeDataNode(int nId, int nParentId, bool bHadChildren)
    : m_nId(nId)
    , m_nParentId(nParentId)
    , m_bHadChildren(bHadChildren)
    , m_bExpanded(false)
{
}

CTreeDataNode::~CTreeDataNode()
{

}

void CTreeDataNode::SetHadChildren(bool bHadChildren)
{
    m_bHadChildren = bHadChildren;
}

void CTreeDataNode::SetExpanded(bool bExpanded)
{
    m_bExpanded = bExpanded;
}

int CTreeDataNode::GetId() const
{
    return m_nId;
}

int CTreeDataNode::GetParentId() const
{
    return m_nParentId;
}

bool CTreeDataNode::IsHadChildren() const
{
    return m_bHadChildren;
}

bool CTreeDataNode::IsExpanded() const
{
    return m_bExpanded;
}

CTreeListDelegate::CTreeListDelegate()
    : m_nUISelectId(0)
    , m_nMaxTreeId(0)
{
}

CTreeListDelegate::~CTreeListDelegate(void)
{
    Clear();
}

void CTreeListDelegate::Clear()
{
    MAP_TREE_DATA_NODE::iterator iter = m_mapTreeNode.begin();

    for (; iter != m_mapTreeNode.end(); ++iter)
    {
        CTreeDataNodePtr pTreeNode = iter->second;

        if (NULL != pTreeNode)
        {
            delete pTreeNode;
        }
    }

    m_mapTreeNode.clear();
    m_vecTreeId.clear();
}

int CTreeListDelegate::GetTreeId()
{
    ++m_nMaxTreeId;

    return m_nMaxTreeId;
}

void CTreeListDelegate::GetParentTreeIdSet(VEC_TREE_ID & vecTreeId, int nTreeId)
{
    int nLoop = 0;
    CTreeDataNodePtr pTreeDataNode = GetTreeDataNode(nTreeId);

    while (NULL != pTreeDataNode)
    {
        ++nLoop;

        // 防止死循环
        if (nLoop > 1000)
        {
            break;
        }

        int nTempParentId = pTreeDataNode->GetParentId();

        if (0 == nTempParentId)
        {
            break;
        }

        vecTreeId.push_back(nTempParentId);
        pTreeDataNode = GetTreeDataNode(nTempParentId);
    }
}

CTreeDataNodePtr CTreeListDelegate::GetTreeDataNode(int nTreeId)
{
    MAP_TREE_DATA_NODE::iterator iter = m_mapTreeNode.find(nTreeId);

    if (iter != m_mapTreeNode.end())
    {
        return iter->second;
    }

    return NULL;
}

CTreeDataNodePtr CTreeListDelegate::GetSelectTreeDataNode()
{
    MAP_TREE_DATA_NODE::iterator iter = m_mapTreeNode.find(m_nUISelectId);

    if (iter != m_mapTreeNode.end())
    {
        return iter->second;
    }

    return NULL;
}

CTreeDataNodePtr CTreeListDelegate::GetMaxExpandedNode()
{
    VEC_TREE_ID::reverse_iterator iterId = m_vecTreeId.rbegin();

    for (; iterId != m_vecTreeId.rend(); ++iterId)
    {
        CTreeDataNodePtr pTreeDataNode = GetTreeDataNode(*iterId);

        if (NULL != pTreeDataNode && pTreeDataNode->IsExpanded())
        {
            return pTreeDataNode;
        }
    }

    return NULL;
}

void CTreeListDelegate::ExpandNode(CTreeDataNodePtr pNode)
{
    if (NULL == pNode || pNode->IsExpanded())
    {
        return;
    }

    VEC_TREE_ID vecTreeId;
    MAP_TREE_DATA_NODE::iterator iter = m_mapTreeNode.begin();

    int nTreeId = pNode->GetId();
    VEC_TREE_ID::iterator iterId = std::find(m_vecTreeId.begin(), m_vecTreeId.end(), nTreeId);

    if (iterId == m_vecTreeId.end())
    {
        vecTreeId.push_back(nTreeId);
    }

    for (; iter != m_mapTreeNode.end(); ++iter)
    {
        CTreeDataNodePtr pTempNode = iter->second;

        if (NULL != pTempNode && pTempNode->GetParentId() == nTreeId)
        {
            int nTempTreeId = pTempNode->GetId();

            iterId = std::find(m_vecTreeId.begin(), m_vecTreeId.end(), nTempTreeId);

            if (iterId == m_vecTreeId.end())
            {
                vecTreeId.push_back(nTempTreeId);
            }
        }
    }

    if (vecTreeId.empty())
    {
        return;
    }

    std::sort(vecTreeId.begin(), vecTreeId.end(), std::less<int>());

    iterId = std::find(m_vecTreeId.begin(), m_vecTreeId.end(), nTreeId);

    if (iterId == m_vecTreeId.end())
    {
        return;
    }

    ++iterId;

    pNode->SetExpanded(true);
    m_vecTreeId.insert(iterId, vecTreeId.begin(), vecTreeId.end());
}

void CTreeListDelegate::CurlNode(CTreeDataNodePtr pNode)
{
    if (NULL == pNode || !pNode->IsExpanded())
    {
        return;
    }

    int nTreeId = pNode->GetId();
    int nParentId = pNode->GetParentId();

    VEC_TREE_ID::iterator iterId = m_vecTreeId.begin();

    while (iterId != m_vecTreeId.end())
    {
        CTreeDataNodePtr pTempNode = GetTreeDataNode(*iterId);

        if (NULL == pTempNode)
        {
            iterId = m_vecTreeId.erase(iterId);
            continue;
        }

        int nTempTreeId = pTempNode->GetId();

        if (nTempTreeId <= nTreeId)
        {
            ++iterId;
            continue;
        }

#if IF_FEATURE_MUTI_CALLLOG
        if (!IsIdenticalTypeNote(nTreeId, nTempTreeId))
        {
            //相同类型才收缩
            ++iterId;
            continue;
        }
#endif

        int nTempParentId = pTempNode->GetParentId();

        if (nTempParentId == nParentId)
        {
            break;
        }

        iterId = m_vecTreeId.erase(iterId);
    }

    pNode->SetExpanded(false);
}

void CTreeListDelegate::CurlAllNode()
{
    int nLoop = 0;
    CTreeDataNodePtr pNode = GetMaxExpandedNode();

    while (NULL != pNode)
    {
        ++nLoop;

        // 防止死循环
        if (nLoop > 1000)
        {
            break;
        }

        CurlNode(pNode);

        pNode = GetMaxExpandedNode();
    }
}

bool CTreeListDelegate::OnLastItemNext()
{
    return false;
}

bool CTreeListDelegate::OnFirstItemPrevious()
{
    return false;
}

void CTreeListDelegate::OnCleanContent(CFrameListBase * pFrameList)
{

}

bool CTreeListDelegate::IsFrameListAttach(CFrameListBase * pFrameList)
{
    return IsAttach(pFrameList);
}

void CTreeListDelegate::OnFrameListDetach(CFrameListBase * pFrameList)
{
    Detach(pFrameList);
}

void CTreeListDelegate::OnFrameListAttach(CFrameListBase * pFrameList)
{
    Attach(pFrameList);
}

void CTreeListDelegate::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    if (NULL == pFrameList || pFrameList != m_pFrameList)
    {
        return;
    }

    RefreshTreeListItem(nDataCount, nDataIndex);
}

void CTreeListDelegate::OnItemExpand(CTreeList * pTreeList, CTreeItem * pTreeItem)
{
    if (NULL == pTreeList || NULL == pTreeItem || pTreeList != m_pFrameList)
    {
        return;
    }

    int nTreeId = pTreeItem->GetId();

    CTreeDataNodePtr pNode = GetTreeDataNode(pTreeItem->GetId());

    if (NULL == pNode || pNode->IsExpanded())
    {
        return;
    }

    ExpandNode(pNode);

    RefreshTreeListItem(pTreeList->GetMaxPageItemCount(), m_nPageDataIndex);
}

void CTreeListDelegate::OnItemCurl(CTreeList * pTreeList, CTreeItem * pTreeItem)
{
    if (NULL == pTreeList || NULL == pTreeItem || pTreeList != m_pFrameList)
    {
        return;
    }

    CTreeDataNodePtr pNode = GetTreeDataNode(pTreeItem->GetId());

    if (NULL == pNode || !pNode->IsExpanded())
    {
        return;
    }

    CurlNode(pNode);

    RefreshTreeListItem(pTreeList->GetMaxPageItemCount(), m_nPageDataIndex);
}

void CTreeListDelegate::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    CTreeList * pTreeList = (CTreeList *)pFrameList;
    CTreeItem * pTreeItem = (CTreeItem *)pItem;

    if (NULL == pTreeList || NULL == pTreeItem
            || pTreeList != m_pFrameList || LIST_ITEM_ACTION_SELECT != nAction)
    {
        return;
    }

    int nTreeId = pTreeItem->GetId();

    if (nTreeId != m_nUISelectId)
    {
        m_nUISelectId = nTreeId;
    }

    int nSelIndex = pTreeList->GetIndexById(m_nUISelectId);

    if (INVALID_ITEM_INDEX != nSelIndex)
    {
        pTreeList->SetItemFocusByIndex(nSelIndex);
    }
}

IFrameListDelegatePtr CTreeListDelegate::GetDelegate() const
{
    return (IFrameListDelegatePtr)this;
}

int CTreeListDelegate::GetSelectId() const
{
    return m_nUISelectId;
}
