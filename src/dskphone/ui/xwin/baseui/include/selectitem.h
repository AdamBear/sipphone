#ifndef __SELECT_ITEM__
#define __SELECT_ITEM__

#include "xwindow/xWindow.h"
#include "xwindow/xlistviewitem.h"

enum
{
    LIST_ITEM_SUB_TYPE_SELECT = 0x00040000,
};

class CSelectItem : public xListViewItem
{
public:
    CSelectItem(IListViewItemAction * pAction = NULL);
    virtual ~CSelectItem();

public:
    static int GetSelectItemType();
    static CSelectItem * GetSelectItem(xListViewItem * pItem);

    static xListViewItem * CreateSelectItem();

    virtual void loadChildrenElements(xml_node & node);

    void setShowCheckIcon(bool bShow);

public:
    //设置显示内容
    void SetSelectText(const yl::string & strText, bool bShowCheck, bool bCheck = false);

protected:
    xTextView * m_pText;    //显示的内容
    xImageView * m_pCheckIcon;  //check图标
};

#endif //C_SELECT_ITEM
