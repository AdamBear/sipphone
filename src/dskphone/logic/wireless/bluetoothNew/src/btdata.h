///////////////////////////////////////////////////////////
//  CBTData.h
//  Implementation of the Class CBTData
//  Created on:      25-三月-2014 18:36:39
//  Original author: songk
///////////////////////////////////////////////////////////

#ifdef IF_SUPPORT_BLUETOOTH
#ifndef __BTDATA_H__
#define __BTDATA_H__

#include "wireless/bluetoothNew/include/btdefines.h"

class CBTData
{
public:
    CBTData();
    ~CBTData();
#if 0
    // 打印设备信息
    static void PrintDevProperty(const btkit_dev_t * property, btkit_dev_type_t type);
#endif // 0

    void PrintDevProperty(const DeviceProperty & property);

    //测试时使用该函数添加数据
#if IS_COVER_TEST
    void AddDevice(DeviceInfo devInfo);
#endif //IS_COVER_TEST

    // 添加新设备到链表
    bool AddDevice(unsigned long ulDeviceHandle, int iScanIndex);

#ifdef IF_BLUETOOTH_SUPPORT_REMOTE_PAIR_REQ
    // 添加新设备到链表
    bool AddDevice(unsigned long ulDeviceHandle);
#endif

    //设置蓝牙设备状态
    bool SetDeviceState(unsigned long ulDeviceHandle, WIRELESS_DEVICE_STATE eState);

    //返回设备列表
    void GetDeviceList(UI_LIST_BT_DEVICE & dstList, DEVICE_LIST_TYPE eType = DEVICE_LIST_ALL);

    //返回设备列表
    bool GetScanedDeviceList(UI_LIST_BT_DEVICE & dstList, int iStartPos, int iNumber);

    //获取扫描列表大小
    int GetScanedDeviceListSize();

    //删除设备
    bool DeleteDevice(unsigned long ulDeviceHandle, bool bDeleteAll = false);

    //获取匹配设备数
    int GetAutoConnectNum();

    //更新自动连接设备
    void UpdateAutoConnectDevice(unsigned long ulDeviceHandle = BT_DEVICE_HANDLE_INVAILED,
                                 bool bAutoConnect = true);

    //获取蓝牙设备信息
    DeviceInfo * GetDeviceInfoByHandle(unsigned long ulDeviceHandle);

    //按序号获取已匹配的设备信息
    DeviceInfo * GetSupportAutoConnectDev(int nIndex);

    //获取设备handle
    unsigned long GetDeviceHandleByMac(const yl::string & strMac);

    //初始化设备列表信息
    bool InitDeviceInfo();

    //更新扫描到的设备列表
    //bool UpdateScanedDeviceInfo();

    //移除unknowtype device
    bool RemoveUnknownDevice();

    //设置自动连接
    bool SetAutoConnect(bool bAutoConnect, unsigned long ulDeviceHandle);

    //设置profile开关
    bool SetProfileEnable(unsigned long ulDeviceHandle, bool bEnable, int iProfile);

    //获取profile开关
    bool GetProfileEnable(unsigned long ulDeviceHandle, int iProfile);

    // 设置a2dp状态
    bool SetDevA2dpState(unsigned long ulHandle, LOGIC_A2DP_STATE eState);

    //输出列表内容
    void ShowBluetoothList(const DeviceInfo * pDevice = NULL);

    //清设备列表，与内存
    void ClearDeviceList();

    //清设备列表
    void ClearScanDeviceList();

    //拷贝列表
    bool CopyDeviceList(UI_LIST_BT_DEVICE & dstList, LIST_BT_DEVICE & srcList, DEVICE_LIST_TYPE eType);

    //获取设备状态信息
    WIRELESS_DEVICE_STATE GetDeviceState(unsigned long ulDeviceHandle);

    //添加到等待连接队列
    void AddDeviceToWaittingConnectList(unsigned long ulDevHandle);

    //是否在等待列表中
    bool IsInWaittingList(unsigned long ulDevHandle);

    //获取等待连接设备数
    int GetWaittingDeviceCount()
    {
        return m_listWaitingDevice.size();
    }

    //获取等待连接设备地址
    unsigned long GetWaittingDevice(bool bPopOther = true);

    //获取设备类型
    btkit_dev_type_t GetDeviceType(unsigned long ulDeviceHandle);

    //蓝牙设备是否被支持
    bool IsSupportDevice(btkit_dev_type_t eType);

    //判断设备是否已匹配(根据状态判断)
    bool IsDevicePaired(WIRELESS_DEVICE_STATE state);

    // 保存蓝牙相关配置信息
    // 主要有: 设备mac, 是否同步联系人, 最后一次连接的手机信息
    bool SaveBluetoothConfig();

    //从文件获取蓝牙联系人自动同步状态
    bool LoadConfig();

    // 通过handle比较设备本地信息和最新扫描获取到的信息
    bool CompareDevInfo(unsigned long ulHandle);

    // 校正本地扫描列表的设备信息, 移除不是本次扫描到的设备
    void CheckScanedDevList();

    // 清空记录扫描到的设备handle列表
    void ClearScanedDevHandleList();

    // 判断是否支持自动连接的类型
    bool IsTypeSupportAC(btkit_dev_type_t eType);

    // 设置最后一个连接上的设备
    void SetLastSupportACDevHandle(unsigned long ulHandle);

    // 获取最后一个连接上的设备
    unsigned long GetLastSupportACDevHandle()
    {
        return m_ulLastSupportACDevHandle;
    }

    bool IsLastDevSupportAC()
    {
        return m_bLastDevIsSupportAC;
    }
	
	//返回设备
	bool GetPairedDevice(DeviceInfo& info);

    // 更新profile开关状态
    void UpdateProfilesActivity(unsigned long ulHandle, int iProfiles = PROFILE_MOBILE | PROFILE_A2DP);

protected:
    //获取驱动中的设备列表
    bool GetDeviceInDriver(LIST_BT_DEVICE & listDevice);

    // 添加新设备到链表
    bool CopyBluetoothInfo(DeviceInfo * pDeviceInfo, const DeviceProperty & btDevProperty);

    bool CopyScanedDeviceList(UI_LIST_BT_DEVICE & dstList, LIST_BT_DEVICE & srcList, int iStartPos,
                              int iNumber);

    //清空并删除列表内存
    void ClearAndDeleteDeviceList(LIST_BT_DEVICE & listDevice);

    bool IsDeviceInList(const yl::string & strMac, LIST_BT_DEVICE & srcList);

private:
    LIST_BT_DEVICE  m_listDevice;                   // 蓝牙列表:当次扫描的设备+配对过的设备
    YLList<unsigned long> m_listScanedDevHandle;    // 上一次扫描设备缓存
    YLList<unsigned long> m_listWaitingDevice;
    unsigned long   m_ulLastSupportACDevHandle;     // 最后一个支持自动连接的设备
    bool            m_bLastDevIsSupportAC;          // 根据类型区分最后一个连接上的设备是否支持自动连接

    //禁用比较与拷贝
private:
    bool operator == (const CBTData & rData);
    const CBTData & operator = (const CBTData & rData);
};

#endif // !defined(__BTDATA_H__)
#endif // IF_SUPPORT_BLUETOOTH
