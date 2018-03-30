#ifndef __WIFI_NET_ITEM_H__
#define __WIFI_NET_ITEM_H__

#ifdef IF_SUPPORT_WIFI
#include "xwindow/xlistviewitem.h"

enum
{
    LIST_ITEM_TYPE_WIFI_NET = 0x12000000,
};


class xImageView;
class xTextView;
class CWifiNetItem : public xListViewItem
{
public:
    explicit CWifiNetItem(IListViewItemAction * pAction = NULL);
    virtual ~CWifiNetItem();

    static int GetWifiNetItemType();
    static CWifiNetItem * GetWifiNetItem(xListViewItem * pItem);

    static xListViewItem * CreateWifiNetItem();

public:
    virtual void loadChildrenElements(xml_node & node);

public:
    void ShowConnectStatusIcon(bool bShowConnectStatusIcon);

public:
    // 设置网络信号强度图标
    void SetSignalStrengthIcon(const yl::string & strStrengthIcon);
    // 设置网络名称
    void SetName(const yl::string & strName);
    // 设置连接状态图标
    void SetConnectStatusIcon(const yl::string & strConnectStatusIcon);
    // 设置网络加密图标
    void SetEncryptIcon(const yl::string & strEncryptIcon);
    virtual bool onFocusEvent(bool bFocusIn);

protected:
    xImageView * m_pStrengthIcon;
    xTextView * m_pName;
    xImageView * m_pConnectStatusIcon;
    xImageView * m_pEncryptIcon;
};
typedef CWifiNetItem * CWifiNetItemPtr;
#endif

#endif//__WIFI_NET_ITEM_H__
