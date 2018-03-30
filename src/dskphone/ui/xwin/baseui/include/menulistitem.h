#ifndef __MENU_LIST_ITEM_H__
#define __MENU_LIST_ITEM_H__

#include <baseui/include/baseuidefine.h>
#include "xwindow/xlistviewitem.h"
#include "textstatusrender.h"

enum
{
    LIST_ITEM_TYPE_MENU         = 0x02000000,
    LIST_ITEM_TYPE_MENU_MASK    = 0x00FF0000,
};


class xTextElement;
class xImageView;
class CMenuListItem : public xListViewItem
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    , public CTextStatusRender
#endif
{
public:
    explicit CMenuListItem(IListViewItemAction * pAction = NULL);
    virtual ~CMenuListItem();

    static int GetMenuItemType();
    static CMenuListItem * GetMenuListItem(xListViewItem * pItem);

    static xListViewItem * CreateMenuListItem();

public:
    virtual void loadChildrenElements(xml_node & node);

    virtual bool onFocusEvent(bool bFocusIn);

#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    virtual void SetItemFocus(bool bFocus);
#endif

public:
    // 设置是否显示序号
    void SetShowIndex(bool bShowIndex);
    // 设置标题
    void SetMenu(const yl::string & strMenu);
    // 设置标题布局方式
    void SetGravity(int nAlign);
    //设置标题颜色
    void SetColor(xColor xColorText);
    //设置标题文字大小
    void SetFontSize(int nSize);

    // 设置、获取ID
    void SetId(int nMenuId);
    int GetId() const;
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
protected:
    virtual void UpdateTextView(bool bFocus);
    virtual void UpdateTextViewColor(xTextView* pTextView, bool bFocus);
#endif

protected:
    xTextView   *   m_pText;                // menu文本
    bool            m_bShowIndex;           // 是否显示序号
    int             m_nMenuId;              // menu id
};
typedef CMenuListItem * CMenuListItemPtr;

#endif
