#include "baseui/framelist.h"

#include "groupselector.h"
#include "directorymgr.h"
#include "directorycommon.h"
#include "contactgroupdelegate.h"
#include "settingui/src/settingitembase.h"
#include "qtcommon/qmisc.h"
#include "uikernel/languagehelper.h"

namespace
{
#define GROUP_SELECT_POP_BOX_ITEM_HEIGHT 47
}

CGroupSelector::CGroupSelector(void)
    : m_nContactType(DIRECTORY_TYPE_LOCAL)
    , m_nSelIndex(-1)
{
    m_bItemFocus = false;
    m_bDefaultFocus = false;
}

CGroupSelector::~CGroupSelector(void)
{
    ClearGroupNode();
}

void CGroupSelector::ClearGroupNode()
{
    VEC_SIMPLE_GROUP_NODE::iterator iter = m_vecSimpleGroupNode.begin();

    for (; iter != m_vecSimpleGroupNode.end(); ++iter)
    {
        ST_SIMPLE_GROUP_NODE_PTR pGroupNode = *iter;

        if (NULL == pGroupNode)
        {
            continue;
        }

        delete pGroupNode;
    }

    m_vecSimpleGroupNode.clear();
}

void CGroupSelector::Init(int nContactType, int nExcludeGroupId, bool bRootInclude)
{
    ClearGroupNode();

    CGroupDelegatePtr pGroupDelegate = g_DirectoryMgr.GetGroupDelegate();
    CContactGroupDelegate * pContactDelegate = CContactGroupDelegate::GetGroupDelegate(pGroupDelegate);

    if (NULL == pContactDelegate)
    {
        return;
    }

    VEC_CONTACT_GROUP_NODE vecGroupNode;

    pContactDelegate->GetAdditiveGroup(vecGroupNode, true, nContactType);

    VEC_CONTACT_GROUP_NODE::iterator iter = vecGroupNode.begin();

    for (; iter != vecGroupNode.end(); ++iter)
    {
        CContactGroupNodePtr pGroupNode = *iter;

        if (NULL == pGroupNode || pGroupNode->GetGroupId() == nExcludeGroupId || pGroupNode->GetGroupId() == kszFavoriteGroupId)
        {
            continue;
        }

        if (!bRootInclude && 0 == pGroupNode->GetParentId())
        {
            continue;
        }

        ST_SIMPLE_GROUP_NODE_PTR pSimpleGroupNode = new ST_SIMPLE_GROUP_NODE;

        if (NULL == pSimpleGroupNode)
        {
            continue;
        }

        pSimpleGroupNode->nGroupId = pGroupNode->GetGroupId();
        pSimpleGroupNode->strGroupName = pGroupNode->GetName();

        m_vecSimpleGroupNode.push_back(pSimpleGroupNode);
    }

    if (m_vecSimpleGroupNode.size() > 0)
    {
        m_nSelIndex = 0;
    }
}

int CGroupSelector::GetSelectGroupId()
{
    ST_SIMPLE_GROUP_NODE_PTR pGroupNode = NULL;

    if (m_nSelIndex >= 0 && m_nSelIndex < m_vecSimpleGroupNode.size())
    {
        pGroupNode = m_vecSimpleGroupNode[m_nSelIndex];
    }

    if (NULL != pGroupNode)
    {
        return pGroupNode->nGroupId;
    }

    return knInvalidRId;
}

int CGroupSelector::GetGroupAmount()
{
    return m_vecSimpleGroupNode.size();
}

void CGroupSelector::OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList)
    {
        return;
    }

    if (!m_pFrameList->IsCanReuseItem(CSettingRadioItem::GetSettingRadioItemType()))
    {
        m_pFrameList->ClearContent();
    }

    int nItemIndex = 0;
    int nSize = m_vecSimpleGroupNode.size();
    int nMaxPageItemCount = m_pFrameList->GetMaxPageItemCount();

    m_pFrameList->SetTotalDataCount(nSize);

    for (; nItemIndex < nMaxPageItemCount && nDataIndex < nSize; ++nItemIndex, ++nDataIndex)
    {
        ST_SIMPLE_GROUP_NODE_PTR pGroupNode = m_vecSimpleGroupNode[nDataIndex];

        if (NULL == pGroupNode)
        {
            continue;
        }

        bool bAdd = false;
        CSettingRadioItem * pRadioItem = (CSettingRadioItem *)m_pFrameList->GetItemByIndex(nItemIndex);

        if (NULL == pRadioItem)
        {
            bAdd = true;
            pRadioItem = new CSettingRadioItem;

            if (NULL == pRadioItem)
            {
                continue;
            }

            pRadioItem->SetChangeStyleOnSelect(false);
            pRadioItem->setGeometry(0, 0, 0, GROUP_SELECT_POP_BOX_ITEM_HEIGHT);
        }

        pRadioItem->ClearContent();
        pRadioItem->CListItem::SetAction(this);

        pRadioItem->SetLabText(toQString(pGroupNode->strGroupName, true));
        pRadioItem->SetDataIndex(nDataIndex);

        if (nDataIndex == m_nSelIndex)
        {
            pRadioItem->SetFocus(true);
        }
        else
        {
            pRadioItem->SetFocus(false);
        }

        if (bAdd && !m_pFrameList->AddItem(pRadioItem))
        {
            pRadioItem->Release();
        }
    }

    int nItemAmount = m_pFrameList->GetTotalItemCount();

    for (int i = nItemIndex; i < nItemAmount; ++i)
    {
        m_pFrameList->DelItemByIndex(nItemIndex);
    }

    m_pFrameList->update();
}

void CGroupSelector::OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction)
{
    CFrameList * pList = (CFrameList *)pFrameList;

    if (NULL == pList || pList != m_pFrameList || NULL == pItem || LIST_ITEM_ACTION_SELECT != nAction)
    {
        return;
    }

    m_nSelIndex = pItem->GetDataIndex();
}
