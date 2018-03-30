#ifndef __PING_AND_TRACEOUT_ITEM_H__
#define __PING_AND_TRACEOUT_ITEM_H__

#include "xwindow/xlistviewitem.h"

enum
{
    LIST_ITEM_TYPE_PING_AND_TRACE           = 0x10000000,
};


class xTextElement;
class CPingAndTraceoutItem : public xListViewItem
{
public:
    explicit CPingAndTraceoutItem(IListViewItemAction * pAction = NULL);
    virtual ~CPingAndTraceoutItem();

    static int GetPingAndTraceoutItemType();
    static CPingAndTraceoutItem * GetPingAndTraceoutListItem(xListViewItem * pItem);

    static xListViewItem * CreatePingAndTraceoutListItem();

public:
    virtual void loadChildrenElements(xml_node & node);
    // 设置焦点状态
    virtual void SetItemFocus(bool bFocus);
public:
    void SetValue(const yl::string & strValue);

protected:
    xTextView   *   m_pText;                // 文本
};
typedef CPingAndTraceoutItem * CPingAndTraceoutItemPtr;

#endif
