#ifndef __NAME_NUMBER_CALL_LOG_LIST_ITEM_H__
#define __NAME_NUMBER_CALL_LOG_LIST_ITEM_H__

#if IS_COLOR
#include "callloglistitem.h"

enum
{
    LIST_ITEM_SUB_TYPE_CALL_LOG_NAME_NUMBER         = 0x00000011,
};

class CNameNumberCallLogListItem : public CCallLogListItem
{
public:
    explicit CNameNumberCallLogListItem(IListViewItemAction * pAction = NULL);
    virtual ~CNameNumberCallLogListItem();

    static int GetNameNumberCallLogListItemType();
    static CNameNumberCallLogListItem * GetNameNumberCallLogListItem(xListViewItem * pItem);

    static xListViewItem * CreateNameNumberCallLogListItem();

public:
    virtual bool onFocusEvent(bool bFocusIn);

    virtual void onLoadChildElement(xml_node & nodeChild);
    void SetNumber(const yl::string& strNumber);

protected:
#if IF_ITEM_SUPPORT_MUTI_TEXT_COLOR
    virtual void UpdateTextView(bool bFocus);
    void UpdateNumberTextColor(bool bFocus);
#endif

protected:
    xTextView* m_pTextNumber;
};
typedef CNameNumberCallLogListItem* CNameNumberCallLogListItemPtr;

#endif
#endif //__NAME_NUMBER_CALL_LOG_LIST_ITEM_H__
