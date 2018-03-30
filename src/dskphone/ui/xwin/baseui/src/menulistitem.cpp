#include "menulistitem.h"
#include "baseuicommon.h"
#include "baseui/include/modbaseui.h"


CMenuListItem::CMenuListItem(IListViewItemAction * pAction/* = NULL*/)
    : xListViewItem(LIST_ITEM_TYPE_MENU, pAction)
    , m_pText(NULL)
    , m_bShowIndex(false)
    , m_nMenuId(0)
{
}

CMenuListItem::~CMenuListItem()
{
}

int CMenuListItem::GetMenuItemType()
{
    return LIST_ITEM_TYPE_MENU;
}

CMenuListItem * CMenuListItem::GetMenuListItem(xListViewItem * pItem)
{
    if (NULL == pItem || LIST_ITEM_TYPE_MENU != pItem->GetType())
    {
        return NULL;
    }

    return (CMenuListItem *)pItem;
}

xListViewItem * CMenuListItem::CreateMenuListItem()
{
    CMenuListItem * pListItem = new CMenuListItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent(BASE_UI_LAYOUT_FILE_MENU_LIST_ITEM);
    }

    return pListItem;
}

void CMenuListItem::loadChildrenElements(xml_node & node)
{
    xListViewItem::loadChildrenElements(node);

    m_pText = static_cast<xTextView *>(getViewById(BASE_UI_ID_TEXT_MENU));
}

bool CMenuListItem::onFocusEvent(bool bFocusIn)
{
    if (NULL != m_pText)
    {
        if (bFocusIn)
        {
            m_pText->EnableScroll(true);
            m_pText->SetOverLengthStyle(OVERLENGTH_TYPE_SCROLL);
        }
        else
        {
            m_pText->SetOverLengthStyle(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS);
        }
    }

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    UpdateTextView(bFocusIn);
#endif

    return xListViewItem::onFocusEvent(bFocusIn);
}

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
void CMenuListItem::SetItemFocus(bool bFocus)
{
    xListViewItem::SetItemFocus(bFocus);

    UpdateTextView(bFocus);
}
#endif

void CMenuListItem::SetShowIndex(bool bShowIndex)
{
    m_bShowIndex = bShowIndex;
}

// 设置标题
void CMenuListItem::SetMenu(const yl::string & strMenu)
{
    if (NULL == m_pText)
    {
        return;
    }

    yl::string strShowMenu = BaseUI_GetRealShowText(strMenu, m_bShowIndex, GetDataIndex() + 1);

    m_pText->SetText(strShowMenu);
}

void CMenuListItem::SetId(int nMenuId)
{
    m_nMenuId = nMenuId;
}

int CMenuListItem::GetId() const
{
    return m_nMenuId;
}

void CMenuListItem::SetGravity(int nAlign)
{
    if (NULL == m_pText)
    {
        return;
    }
    m_pText->SetGravity(nAlign);
}

void CMenuListItem::SetColor(xColor xColorText)
{
    if (m_pText == NULL)
    {
        return;
    }
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    SetTextColor(xColorText, xColorText, xColorText);
    m_pText->SetTextColor(xColorText);
#endif
}


void CMenuListItem::SetFontSize(int nSize)
{
    if (m_pText == NULL)
    {
        return;
    }
    m_pText->SetFontPointSize(nSize);
}

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
void CMenuListItem::UpdateTextView(bool bFocus)
{
    UpdateTextViewColor(m_pText, bFocus);
}

void CMenuListItem::UpdateTextViewColor(xTextView* pTextView, bool bFocus)
{
    if (NULL == pTextView || !pTextView->isVisible())
    {
        return;
    }

    if (bFocus)
    {
        pTextView->SetTextColor(m_colorFocus);
    }
    else
    {
        pTextView->SetTextColor(m_colorNormal);
    }
}
#endif
