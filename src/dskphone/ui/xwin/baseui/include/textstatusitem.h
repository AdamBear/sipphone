#ifndef __TEXTSTATUS_ITEM__
#define __TEXTSTATUS_ITEM__

#include <baseui/include/baseuidefine.h>
#include "xwindow/xWindow.h"
#include "xwindow/xlistviewitem.h"
#include "baseui/include/menulistitem.h"

enum
{
    LIST_ITEM_SUB_TYPE_TEXTSTATUS = 0x00040001,
};

class CTextStatusItem : public CMenuListItem
{
public:
    CTextStatusItem(IListViewItemAction * pAction = NULL);
    virtual ~CTextStatusItem();

public:
    static int GetStatusItemType();
    static CTextStatusItem * GetStatusItem(xListViewItem * pItem);

    static xListViewItem * CreateStatusItem();

    virtual void loadChildrenElements(xml_node & node);

    // 设置标题
    //void SetMenu(const yl::string & strMenu);
    // 设置标题布局方式
    virtual void SetGravity(int nAlign);
    //设置标题颜色
    //void SetColor(xColor xColorText);
    //设置标题文字大小
    //void SetFontSize(int nSize);

public:
    //设置显示内容
    void SetIcon(xPixmap & xPixIcon);

protected:
    //xTextView * m_pText;    //显示的内容
    xImageView * m_pCheckIcon;  //check图标
    xLinearLayout * m_pItemLinearLayout; //item线性布局
};

typedef CTextStatusItem * CTextStatusItemPtr;

#endif //__TEXTSTATUS_ITEM__
