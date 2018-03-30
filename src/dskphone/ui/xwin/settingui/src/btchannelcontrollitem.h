#ifndef _BT_CHANNEL_CONTROLL_ITEM_H_
#define _BT_CHANNEL_CONTROLL_ITEM_H_

#ifdef IF_SUPPORT_BLUETOOTH
#include "baseui/include/comboboxlistitem.h"

enum
{
    LIST_ITEM_BT_CHANNEL_CONTROLL   = 0x00001600,
};

class xTextView;

class CBTChannelControllItem : public CComboBoxListItem
{
public:
    explicit CBTChannelControllItem(IListViewItemAction * pAction = NULL);
    virtual ~CBTChannelControllItem();

    static int GetBTChannelControllItemType();
    static CBTChannelControllItem * GetBTChannelControllItem(xListViewItem * pItem);
    static xListViewItem * CreateBTChannelControllItem();

public:
    virtual void loadChildrenElements(xml_node & node);
    virtual void SetItemFocus(bool bFocus);
    void SetExtraName(const yl::string & strExtraName);
    void SetExtraValue(const yl::string & strExtraValue);
protected:
    xTextView * m_pExtraName;
    xTextView * m_pExtraValue;
};
#endif

#endif //_BT_CHANNEL_CONTROLL_ITEM_H_
