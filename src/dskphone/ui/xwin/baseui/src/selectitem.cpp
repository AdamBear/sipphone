#include "selectitem.h"
#include "baseuicommon.h"

#include "baseui/include/t2xpicpath.h"

CSelectItem::CSelectItem(IListViewItemAction * pAction/* = NULL*/)
    : xListViewItem(LIST_ITEM_SUB_TYPE_SELECT, pAction)
    , m_pText(NULL)
    , m_pCheckIcon(NULL)
{
}

CSelectItem::~CSelectItem()
{
}

int CSelectItem::GetSelectItemType()
{
    return LIST_ITEM_SUB_TYPE_SELECT;
}

CSelectItem * CSelectItem::GetSelectItem(xListViewItem * pItem)
{
    if (NULL == pItem || LIST_ITEM_SUB_TYPE_SELECT != pItem->GetType())
    {
        return NULL;
    }

    return (CSelectItem *)pItem;
}

xListViewItem * CSelectItem::CreateSelectItem()
{
    CSelectItem * pListItem = new CSelectItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent(BASE_UI_LAYOUT_FILE_SELECT_LIST_ITEM);
    }

    return pListItem;
}

void CSelectItem::loadChildrenElements(xml_node & node)
{
    xListViewItem::loadChildrenElements(node);

    m_pText = static_cast<xTextView *>(getViewById(BASE_UI_ID_TEXT_TITLE));
    m_pCheckIcon = static_cast<xImageView *>(getViewById(BASE_UI_ID_ICON_CHECK));
}

void CSelectItem::SetSelectText(const yl::string & strText, bool bShowCheck, bool bCheck/* = false*/)
{
    //设置名称
    if (NULL != m_pText)
    {
        m_pText->show(xView::SHOW_CMD_BRING_TO_TOP);
        m_pText->SetText(strText);
    }

    if (NULL != m_pCheckIcon)
    {
        if (bShowCheck)
        {
            m_pCheckIcon->show();
            if (bCheck)
            {
                m_pCheckIcon->setPixmap(PIC_CHECK_ON);
            }
            else
            {
                m_pCheckIcon->setPixmap(PIC_CHECK_OFF);
            }
        }
        else
        {
            m_pCheckIcon->hide(true);
        }
    }
}
