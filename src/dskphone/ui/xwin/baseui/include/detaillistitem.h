#ifndef __DETAIL_LIST_ITEM_H__
#define __DETAIL_LIST_ITEM_H__

#include "detaillistitembase.h"

enum
{
    LIST_ITEM_SUB_TYPE_DETAIL_ITEM      = 0x00010000,
};


class xTextView;
class CDetailListItem : public CDetailListItemBase
{
public:
    CDetailListItem(IListViewItemAction * pAction = NULL);
    virtual ~CDetailListItem();

    static int GetDetailListItemType();
    static CDetailListItem * GetDetailListItem(xListViewItem * pItem);

    static xListViewItem * CreateDetailListItem();

public:
    virtual void loadChildrenElements(xml_node & node);

    virtual bool onFocusEvent(bool bFocusIn);

public:
    // 设置Detail文字
    virtual void SetValue(const yl::string & strValue);

    // 获取Detail文字
    virtual yl::string GetValue() const;

    void SetItemData(const yl::string & strData);
    void SetItemData(int nData);

    const yl::string & GetItemStringData() const;
    int GetData() const;

protected:
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    virtual void UpdateTextView(bool bFocus);
#endif

protected:
    xTextView * m_pValue;
    yl::string m_strData;
};
typedef CDetailListItem * CDetailListItemPtr;

#endif
