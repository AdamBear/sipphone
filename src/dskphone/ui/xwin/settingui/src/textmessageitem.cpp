#include "textmessageitem.h"
#include "baseui/include/baseuicommon.h"
#include "xwindow/xWindow.h"


CTextMessageItem::CTextMessageItem(IListViewItemAction * pAction/* = NULL*/)
    : xListViewItem(LIST_ITEM_TYPE_TEXT_MESSAGE, pAction)
    , m_pTextName(NULL)
    , m_pTextDate(NULL)
    , m_pIcon(NULL)
    , m_pTextUrgent(NULL)
{
}

CTextMessageItem::~CTextMessageItem()
{
}

int CTextMessageItem::GetTextMessageItemType()
{
    return LIST_ITEM_TYPE_TEXT_MESSAGE;
}

CTextMessageItem * CTextMessageItem::GetTextMessageItem(xListViewItem * pItem)
{
    if (NULL == pItem || LIST_ITEM_TYPE_TEXT_MESSAGE != pItem->GetType())
    {
        return NULL;
    }

    return (CTextMessageItem *)pItem;
}

xListViewItem * CTextMessageItem::CreateTextMessageItem()
{
    CTextMessageItem * pListItem = new CTextMessageItem;

    if (NULL != pListItem)
    {
        pListItem->loadContent("setting/textmessageitem.xml");
    }

    return pListItem;
}

void CTextMessageItem::loadChildrenElements(xml_node & node)
{
    xListViewItem::loadChildrenElements(node);

    m_pTextName = static_cast<xTextView *>(getViewById("txtName"));
    m_pTextDate = static_cast<xTextView *>(getViewById("txtDate"));
    m_pIcon = static_cast<xImageView *>(getViewById("picIcon"));
    m_pTextUrgent = static_cast<xTextView*>(getViewById("txtUrgent"));
}

// 设置账号名称
void CTextMessageItem::SetName(const yl::string & strName)
{
    if (NULL == m_pTextName)
    {
        return;
    }
    m_pTextName->SetText(strName);
}

// 设置标题
void CTextMessageItem::SetDateString(const yl::string & strDate)
{
    if (NULL == m_pTextDate)
    {
        return;
    }
    m_pTextDate->SetText(strDate);
}

// 设置标题
void CTextMessageItem::SetStatusIcon(const yl::string & strIcon)
{
    if (NULL == m_pIcon)
    {
        return;
    }
    m_pIcon->setPixmap(strIcon);
}

void CTextMessageItem::SetUrgent(bool bUrgent)
{
    if (NULL == m_pTextUrgent)
    {
        return;
    }

    if (bUrgent)
    {
        m_pTextUrgent->show();
    }
    else
    {
        m_pTextUrgent->hide();
    }
}

