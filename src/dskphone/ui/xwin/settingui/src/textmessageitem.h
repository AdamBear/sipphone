#ifndef __TEXT_MESSAGE_ITEM_H__
#define __TEXT_MESSAGE_ITEM_H__

#include "xwindow/xlistviewitem.h"

enum
{
    LIST_ITEM_TYPE_TEXT_MESSAGE         = 0x13000000,
};


class CTextMessageItem : public xListViewItem
{
public:
    explicit CTextMessageItem(IListViewItemAction * pAction = NULL);
    virtual ~CTextMessageItem();

    static int GetTextMessageItemType();
    static CTextMessageItem * GetTextMessageItem(xListViewItem * pItem);

    static xListViewItem * CreateTextMessageItem();

public:
    virtual void loadChildrenElements(xml_node & node);
public:
    void SetName(const yl::string & strName);
    void SetDateString(const yl::string & strDate);
    void SetStatusIcon(const yl::string & strIcon);
    void SetUrgent(bool bUrgent);

protected:
    xTextView*      m_pTextName;                // 文本
    xTextView*      m_pTextDate;                // 文本
    xImageView*     m_pIcon;                    // 文本
    xTextView*      m_pTextUrgent;              // 文本
};
typedef CTextMessageItem * CTextMessageItemPtr;

#endif
