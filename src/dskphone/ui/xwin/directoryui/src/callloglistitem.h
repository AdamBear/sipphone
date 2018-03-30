#ifndef __CALL_LOG_LIST_ITEM_H__
#define __CALL_LOG_LIST_ITEM_H__

#include "contactlistitem.h"

enum
{
    LIST_ITEM_SUB_TYPE_CALL_LOG         = 0x00000100,
};


class CCallLogListItem : public CContactListItem
{
public:
    explicit CCallLogListItem(IListViewItemAction * pAction = NULL);
    virtual ~CCallLogListItem();

    static int GetCallLogListItemType();
    static CCallLogListItem * GetCallLogListItem(xListViewItem * pItem);

    static xListViewItem * CreateCallLogListItem();

public:
    void SetCallType(int nType);
protected:
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    virtual void UpdateTextView(bool bFocus);
    void UpdateMissCallNameColor(bool bFocus);
protected:
    bool m_bMissCall;
#endif
};
typedef CCallLogListItem * CCallLogListItemPtr;

#endif
