#ifndef __SETTINGUI_BLUETOOTH_DEV_ITEM_DATA_H__
#define __SETTINGUI_BLUETOOTH_DEV_ITEM_DATA_H__

#include "settinguilogic/src/settinguibaseitemdata.h"

#ifdef IF_SUPPORT_BLUETOOTH
enum
{
    // 第1层子类，使用3、4位
    SETTING_ITEM_BLUETOOTH_DEV      = 0x00050000,
};

class CSettingUIBluetoothDevItemData : public CSettingUIItemDataBase
{
public:
    explicit CSettingUIBluetoothDevItemData();
    virtual ~CSettingUIBluetoothDevItemData();

    static int GetBluetoothDevDataItemType();
    static CSettingUIBluetoothDevItemData * GetBluetoothDevDataItem(CSettingUIItemDataBase * pDataItem);
    static CSettingUIItemDataBase * CreateBluetoothDevDataItem();

public:
    // 设备图标
    const yl::string & GetDevIcon();
    void SetDevIcon(const yl::string & strDevIcon);
    // 设备名称 用基本类的strDisplayName
    // 连接状态图标
    const yl::string & GetConnectStatusIcon();
    void SetConnectStatusIcon(const yl::string & strConnectStatusIcon);
    // 设备Mac信息
    const yl::string & GetMac();
    void SetMac(const yl::string & strMac);

protected:
    yl::string  m_strDevIcon;
    yl::string  m_strConnectStatusIcon;
    yl::string  m_strMac;
};

typedef     CSettingUIBluetoothDevItemData   *  CSettingUIBluetoothDevItemDataPtr;
#endif

#endif //__SETTINGUI_BLUETOOTH_DEV_LIST_ITEM_DATA_H__
