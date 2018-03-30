#include "textstatusitem.h"
#include "baseuicommon.h"

#include "baseui/include/t2xpicpath.h"

namespace
{
#define ITEM_UI_ID_LINEARLAYOUT "itemLinearLayout"
}

CTextStatusItem::CTextStatusItem(IListViewItemAction * pAction/* = NULL*/)
    : CMenuListItem(pAction)
    , m_pCheckIcon(NULL)
    , m_pItemLinearLayout(NULL)
{
    m_nType = LIST_ITEM_SUB_TYPE_TEXTSTATUS;
}

CTextStatusItem::~CTextStatusItem()
{
}

int CTextStatusItem::GetStatusItemType()
{
    return LIST_ITEM_SUB_TYPE_TEXTSTATUS;
}

CTextStatusItem * CTextStatusItem::GetStatusItem(xListViewItem * pItem)
{
    if (NULL == pItem || LIST_ITEM_SUB_TYPE_TEXTSTATUS != pItem->GetType())
    {
        return NULL;
    }

    return (CTextStatusItem *)pItem;
}

xListViewItem * CTextStatusItem::CreateStatusItem()
{
    CTextStatusItem * pListItem = new CTextStatusItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent(BASE_UI_LAYOUT_FILE_TEXTSTATUS_LIST_ITEM);
    }

    return pListItem;
}

void CTextStatusItem::loadChildrenElements(xml_node & node)
{
    xListViewItem::loadChildrenElements(node);

    m_pText = static_cast<xTextView *>(getViewById(BASE_UI_ID_TEXT_TITLE));
    m_pCheckIcon = static_cast<xImageView *>(getViewById(BASE_UI_ID_ICON_CHECK));
    m_pItemLinearLayout = static_cast<xLinearLayout *>(getViewById(ITEM_UI_ID_LINEARLAYOUT));
}

void CTextStatusItem::SetIcon(xPixmap & xPixIcon)
{
    if (NULL != m_pCheckIcon)
    {
        m_pCheckIcon->setPixmap(xPixIcon);
    }
}

void CTextStatusItem::SetGravity(int nAlign)
{
    if (NULL == m_pItemLinearLayout)
    {
        return;
    }

    FrameLayoutParams* layoutInParent = FrameLayoutParams::GetLayoutParams(m_pItemLinearLayout->m_pLayoutInParent);
    if (layoutInParent != NULL)
    {
        layoutInParent->layout_gravity = nAlign;
    }
}



