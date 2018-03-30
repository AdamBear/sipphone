#ifndef __SETTINGUI_WIFI_NET_ITEM_DATA_H__
#define __SETTINGUI_WIFI_NET_ITEM_DATA_H__

#ifdef IF_SUPPORT_WIFI
#include "settinguilogic/src/settinguibaseitemdata.h"


enum
{
    // 第1层子类，使用3、4位
    SETTING_ITEM_WIFI_NET       = 0x00060000,
};

class CSettingUIWifiNetItemData : public CSettingUIItemDataBase
{
public:
    explicit CSettingUIWifiNetItemData();
    virtual ~CSettingUIWifiNetItemData();

    static int GetWifiNetDataItemType();
    static CSettingUIWifiNetItemData * GetWifiNetDataItem(CSettingUIItemDataBase * pDataItem);
    static CSettingUIItemDataBase * CreateWifiNetDataItem();

public:
    // 网络信号强度图标
    const yl::string & GetSignalStrengthIcon();
    void SetSignalStrengthIcon(const yl::string & strDevIcon);
    // 网络名称 用基本类的strDisplayName
    // 连接状态图标
    const yl::string & GetConnectStatusIcon();
    void SetConnectStatusIcon(const yl::string & strConnectStatusIcon);
    // 网络加密图标
    const yl::string & GetEncryptIcon();
    void SetEncryptIcon(const yl::string & strEncryptIcon);

protected:
    yl::string  m_strSignalStrengthIcon;
    yl::string  m_strConnectStatusIcon;
    yl::string  m_strEncryptIcon;
};

typedef     CSettingUIWifiNetItemData   *   CSettingUIWifiNetItemDataPtr;
#endif

#endif //__SETTINGUI_WIFI_NET_ITEM_DATA_H__
