#ifndef __DETAIL_LIST_ITEM_BASE_H__
#define __DETAIL_LIST_ITEM_BASE_H__

#include <ylvector.h>
#include <baseui/include/baseuidefine.h>
#include "xwindow/xlistviewitem.h"
#include "textstatusrender.h"

enum
{
    LIST_ITEM_TYPE_DETAIL           = 0x01000000,
    LIST_ITEM_TYPE_DETAIL_MARK      = 0x00FF0000,
};


class xTextView;
class CDetailListItemBase : public xListViewItem
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    , public CTextStatusRender
#endif
{
public:
    explicit CDetailListItemBase(IListViewItemAction * pAction = NULL);
    virtual ~CDetailListItemBase();

    static int GetDetailListItemBaseType();

public:
    virtual void loadChildrenElements(xml_node & node);
    virtual void Release();
    virtual bool onFocusEvent(bool bFocusIn);
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    virtual void SetItemFocus(bool bFocus);
#endif

public:
    // 设置标题
    void SetTitle(const yl::string & strTitle);

    void SetShowIndex(bool bShowIndex);

    void SetShowColon(bool bShowColon);

    // 设置Detail文字
    virtual void SetValue(const yl::string & strValue);

    // 获取Detail文字
    virtual yl::string GetValue() const;

    int GetDetailSubType() const;
protected:
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    virtual void UpdateTextView(bool bFocus);
    void UpdateTextViewColor(xTextView* pTextView, bool bFocus);
#endif
protected:
    xTextView * m_pTitle;
    bool m_bShowIndex;
    bool m_bShowColon;              // 是否显示冒号
};
typedef CDetailListItemBase * CDetailListItemBasePtr;
typedef YLVector<CDetailListItemBase *> VEC_DETAIL_LIST_ITEM;

#endif
