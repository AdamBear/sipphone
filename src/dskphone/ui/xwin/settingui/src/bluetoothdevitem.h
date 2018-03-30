#ifndef __BLUETOOTH_DEV_ITEM_H__
#define __BLUETOOTH_DEV_ITEM_H__

#ifdef IF_SUPPORT_BLUETOOTH
#include "xwindow/xlistviewitem.h"

enum
{
    LIST_ITEM_TYPE_BLUETOOTH_DEV    = 0x11000000,
};


class xImageView;
class xTextView;
class CBluetoothDevItem : public xListViewItem
{
public:
    explicit CBluetoothDevItem(IListViewItemAction * pAction = NULL);
    virtual ~CBluetoothDevItem();

    static int GetBluetoothDevItemType();
    static CBluetoothDevItem * GetBluetoothDevItem(xListViewItem * pItem);

    static xListViewItem * CreateBluetoothDevItem();

public:
    virtual void loadChildrenElements(xml_node & node);
    virtual bool onFocusEvent(bool bFocusIn);

public:
    void ShowConnectStatusIcon(bool bShowConnectStatusIcon);

public:
    // 设置设备图标
    void SetDevIcon(const yl::string & strDevIcon);
    // 设置设备名称
    void SetName(const yl::string & strName);
    // 设置连接上设备成功后的图标
    void SetConnectStatusIcon(const yl::string & strConnectStatusIcon);
    // 设置设备Mac信息
    void SetMac(const yl::string & strMac);

protected:
    xImageView * m_pDevIcon;
    xTextView * m_pName;
    xImageView * m_pConnectStatusIcon;
    xTextView * m_pMac;
};
typedef CBluetoothDevItem * CBluetoothDevItemPtr;
#endif

#endif//__BLUETOOTH_DEV_LIST_ITEM_H__
