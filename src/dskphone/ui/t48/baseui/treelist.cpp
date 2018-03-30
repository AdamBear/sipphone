#include "treelist.h"

CTreeList::CTreeList(QWidget * parent/* = NULL*/)
    : CFrameList(parent)
{
    setObjectName("CTreeList");
}

CTreeList::~CTreeList()
{

}

bool CTreeList::DelItemById(int nItemId, bool bRefresh/* = false*/)
{
    int nIndex = GetIndexById(nItemId);

    if (INVALID_ITEM_INDEX == nIndex)
    {
        return false;
    }

    return DelItemByIndex(nIndex, bRefresh);
}

int CTreeList::GetIndexById(int nItemId)
{
    VEC_LIST_ITEM & vecItem = GetListItem();
    VEC_LIST_ITEM::iterator iter = vecItem.begin();

    for (int i = 0; iter != vecItem.end(); ++iter, ++i)
    {
        CTreeItemPtr pItem = (CTreeItemPtr) * iter;

        if (NULL != pItem && pItem->GetId() == nItemId)
        {
            return i;
            break;
        }
    }

    return INVALID_ITEM_INDEX;
}

CTreeItemPtr CTreeList::GetItemById(int nItemId)
{
    VEC_LIST_ITEM & vecItem = GetListItem();
    VEC_LIST_ITEM::iterator iter = vecItem.begin();

    for (; iter != vecItem.end(); ++iter)
    {
        CTreeItemPtr pItem = (CTreeItemPtr) * iter;

        if (NULL != pItem && pItem->GetId() == nItemId)
        {
            return pItem;
        }
    }

    return NULL;
}

void CTreeList::OnItemFocusChange(CListItem * pItem, bool bPreFocus)
{
    CFrameList::OnItemFocusChange(pItem, bPreFocus);

    // 第一次点击选中，以后的点击才处理展开、收起
    if (!bPreFocus)
    {
        return;
    }

    CTreeItemPtr pTreeItem = (CTreeItemPtr)pItem;

    if (NULL == pTreeItem)
    {
        return;
    }

    if (!pTreeItem->IsHadChildren())
    {
        return;
    }

    pTreeItem->SetExpand(!pTreeItem->IsExpand());

    ITreeListDelegate * pTreeDelegate = (ITreeListDelegate *)m_pDelegate;

    if (NULL == pTreeDelegate)
    {
        return;
    }

    if (pTreeItem->IsExpand())
    {
        pTreeDelegate->OnItemExpand(this, pTreeItem);
    }
    else
    {
        pTreeDelegate->OnItemCurl(this, pTreeItem);
    }
}
