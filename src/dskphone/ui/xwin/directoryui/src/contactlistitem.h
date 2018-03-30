#ifndef __CONTACT_LIST_ITEM_H__
#define __CONTACT_LIST_ITEM_H__

#include <baseui/include/baseuidefine.h>
#include "xwindow/xlistviewitem.h"
#include "baseui/include/textstatusrender.h"
#include <xwindow/xwindowdefine.h>

enum
{
    LIST_ITEM_TYPE_CONTACT_BASE     = 0x03000000,
};


class xImageView;
class xTextView;

class CContactListItem : public xListViewItem
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    , public CTextStatusRender
#endif
{
public:
    explicit CContactListItem(IListViewItemAction * pAction = NULL);
    virtual ~CContactListItem();

    static int GetContactListItemType();
    static CContactListItem * GetContactListItem(xListViewItem * pItem);

    static xListViewItem * CreateContactListItem();

public:
    virtual void onLoadChildElement(xml_node & nodeChild);

// xListViewItem
public:
    // 清除内容
    virtual void ClearContent();

    // 设置焦点状态
    virtual bool onFocusEvent(bool bFocusIn);
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    virtual void SetItemFocus(bool bFocus);
#endif
public:
    // 设置Id
    void SetId(int nId);

    // 设置图标
    void SetIcon(const yl::string & strIcon);
    // 设置标题
    void SetName(const yl::string & strName);
    // 设置信息
    void SetInfo(const yl::string & strInfo);

    yl::string GetInfo() const;

    // 获得数据Id
    int GetId() const;

    void SetPhoto(const yl::string & strPhoto);

#if IF_TEXTVIEW_SUPPORT_HIGH_LIGHT_TEXT
    void SetHightLightText(const yl::string& strText);
    xColor GetHightLightTextColor();
#endif

protected:
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    virtual void UpdateTextView(bool bFocus);
    void UpdateTextViewColor(xTextView* pTextView, bool bFocus);
#endif
protected:
    // ID
    int m_nId;

    xImageView * m_pIcon;
    xTextView * m_pName;
    xTextView * m_pInfo;
};
typedef CContactListItem * CContactListItemPtr;

#endif
