///////////////////////////////////////////////////////////
//  CBTData.cpp
//  Implementation of the Class CBTData
//  Created on:      25-三月-2014 18:36:39
//  Original author: songk
///////////////////////////////////////////////////////////
#ifdef IF_SUPPORT_BLUETOOTH
#include "bluetooth_inc.h"
#include "xmlparser.hpp"
#include "path_define.h"

//蓝牙手机联系人同步开关保存文件
#define BLUETOOTH_CONFIG_FILE  CONFIG_DATA_PATH"/bluetoothconfig.xml"

bool CompareByScanIndex(const DeviceInfo & lParam, const DeviceInfo & rParam)
{
    if (lParam.eState == WIRE_DEVICE_CONNECTED_STATE)
    {
        return true;
    }
    else if (rParam.eState == WIRE_DEVICE_CONNECTED_STATE)
    {
        return false;
    }

    return lParam.iScanIndex < rParam.iScanIndex;
}

CBTData::CBTData()
    : m_ulLastSupportACDevHandle(BT_DEVICE_HANDLE_INVAILED)
{
    m_listDevice.clear();
    m_listWaitingDevice.clear();
}

CBTData::~CBTData()
{

}

#if IS_COVER_TEST
//添加设备到链表，测试时使用
void CBTData::AddDevice(DeviceInfo devInfo)
{
    if (NULL == GetDeviceInfoByHandle(devInfo.devHandle))
    {
        DeviceInfo * pDeviceInfo = new DeviceInfo;
        *pDeviceInfo = devInfo;
        m_listDevice.push_back(pDeviceInfo);
    }
}
#endif //IS_COVER_TEST

// 判断是否是已匹配上的状态
bool CBTData::IsDevicePaired(WIRELESS_DEVICE_STATE eState)
{
    return (WIRE_DEVICE_PAIRED_STATE == eState
            || WIRE_DEVICE_CONNECTED_STATE == eState
            || WIRE_DEVICE_CONNECTING_STATE == eState
            || WIRE_DEVICE_DISCONNECT_STATE == eState);
}

//初始化设备信息
bool CBTData::InitDeviceInfo()
{
    ClearDeviceList();
    ClearScanDeviceList();
    GetDeviceInDriver(m_listDevice);
    //从文件获取蓝牙联系人自动同步状态
    return _BTData.LoadConfig();
}

// 显示蓝牙列表
void CBTData::ShowBluetoothList(const DeviceInfo * pDevice/* = NULL*/)
{
    int iIndex = 0;
    for (LIST_BT_DEVICE::iterator itBegin = m_listDevice.begin(); itBegin != m_listDevice.end();
            ++itBegin, ++iIndex)
    {
        DeviceInfo * pTempInfo = (*itBegin);
        if (NULL == pTempInfo)
        {
            continue;
        }

        if (NULL == pDevice
                || (NULL != pDevice
                    && pDevice->devHandle == pTempInfo->devHandle)
           )
        {
            WIRE_INFO("name[%s], handle[%d], mac [%s], state[%s], type[%d], autoconnect[%d], synccontact[%d], bScanedDev[%d],  index[%d]",
                      pTempInfo->strName.c_str(),
                      pTempInfo->devHandle,
                      pTempInfo->strMac.c_str(),
                      DeviceStateToString(pTempInfo->eState),
                      pTempInfo->eType,
                      pTempInfo->bAutoConnect,
                      pTempInfo->bSyncContact,
                      pTempInfo->bScanedDev,
                      iIndex
                     );
        }
    }
}

// 清空并销毁一个列表内存
void CBTData::ClearAndDeleteDeviceList(LIST_BT_DEVICE & listDevice)
{
    for (LIST_BT_DEVICE::iterator it = listDevice.begin(); it != listDevice.end(); ++it)
    {
        DeviceInfo * pDevice = *it;
        SAFE_DELETE(pDevice);
    }

    listDevice.clear();
}

//清空设备列表，并删除内存
void CBTData::ClearDeviceList()
{
    ClearAndDeleteDeviceList(m_listDevice);
}

//清空扫描列表，并删除内存
void CBTData::ClearScanDeviceList()
{
    //清除掉扫描设备
    for (LIST_BT_DEVICE::iterator itBeg = m_listDevice.begin();
            itBeg != m_listDevice.end();)
    {
        DeviceInfo * pDevice = *itBeg;
        if (NULL == pDevice)
        {
            continue;
        }

        if (pDevice->bScanedDev)
        {
            pDevice->bScanedDev = false;
        }

        if (WIRE_DEVICE_INIT_STATE == pDevice->eState)
        {
            itBeg = m_listDevice.erase(itBeg);
            SAFE_DELETE(pDevice);
        }
        else
        {
            ++itBeg;
        }
    }
}

//拷贝链表
bool CBTData::CopyDeviceList(UI_LIST_BT_DEVICE & dstList, LIST_BT_DEVICE & srcList,
                             DEVICE_LIST_TYPE eType)
{
    LIST_BT_DEVICE::iterator itBegin = srcList.begin();
    for (; itBegin != srcList.end(); ++itBegin)
    {
        DeviceInfo * pTempInfo = (*itBegin);
        if (NULL == pTempInfo)
        {
            continue;
        }

        if (DEVICE_LIST_ALL == eType)
        {
            dstList.push_back(*pTempInfo);
        }
        else if ((DEVICE_LIST_PAIRED == eType)
                 && (IsDevicePaired(pTempInfo->eState) || pTempInfo->bPaired))
        {
            dstList.push_back(*pTempInfo);
        }
        else if ((DEVICE_LIST_SCAN == eType)
                 && !IsDevicePaired(pTempInfo->eState))
        {
            dstList.push_back(*pTempInfo);
        }
    }
    return true;
}

bool CBTData::CopyScanedDeviceList(UI_LIST_BT_DEVICE & dstList, LIST_BT_DEVICE & srcList,
                                   int iStartPos, int iNumber)
{
    // 起始位置大于列表个数 --- 获取的区段内没有数据
    if (srcList.size() < iStartPos + 1)
    {
        return false;
    }

    // 拷贝扫描列表
    UI_LIST_BT_DEVICE listTemp;
    for (LIST_BT_DEVICE::iterator iter = srcList.begin();
            iter != srcList.end(); ++ iter)
    {
        DeviceInfo * pDevInfo = *iter;
        if (pDevInfo->bScanedDev)
        {
            listTemp.push_back(*pDevInfo);
        }
    }

    // 对扫描列表排序
    Sort(listTemp, CompareByScanIndex);

    // 按区间获取扫描列表
    int iIndex = 0;     // 链表当前位置
    for (UI_LIST_BT_DEVICE::iterator itBegin = listTemp.begin();
            itBegin != listTemp.end();
            ++ itBegin, ++ iIndex)
    {
        DeviceInfo & pTempInfo = (*itBegin);

        // 从iStartPos位置开始取数据
        if (iIndex < iStartPos)
        {
            continue;
        }

        if (dstList.size() >= iNumber)
        {
            break;
        }

        dstList.push_back(pTempInfo);
    }

    return dstList.size() > 0;
}

// 添加新设备到链表
bool CBTData::CopyBluetoothInfo(DeviceInfo * pDeviceInfo, const DeviceProperty & btDevProperty)
{
    if (NULL == pDeviceInfo)
    {
        return false;
    }

    pDeviceInfo->eType = btDevProperty.dev_type;
    pDeviceInfo->strMac = btDevProperty.dev_property.bt_dev_addr;

    // 如果别名为空，则使用name
    pDeviceInfo->strName = (0 == strcmp(btDevProperty.dev_property.alias,
                                        "")) ? btDevProperty.dev_property.name : btDevProperty.dev_property.alias;
    if (btDevProperty.dev_property.paired)
    {
        pDeviceInfo->bPaired = true;
        if (_BTDriver.IsDeviceConnectedInDriver(pDeviceInfo->devHandle))
        {
            pDeviceInfo->eState = WIRE_DEVICE_CONNECTED_STATE;
        }
        else
        {
            // 如果不是匹配状态，则更新状态，如不能将连接状态覆盖了
            if (!IsDevicePaired(pDeviceInfo->eState))
            {
                pDeviceInfo->eState = WIRE_DEVICE_PAIRED_STATE;
            }
        }
    }
    else
    {
        pDeviceInfo->eState = WIRE_DEVICE_INIT_STATE;
    }
    return true;
}

//返回设备列表
void CBTData::GetDeviceList(UI_LIST_BT_DEVICE & dstList,
                            DEVICE_LIST_TYPE eType/* = DEVICE_LIST_ALL */)
{
    CopyDeviceList(dstList, m_listDevice, eType);
}

// 获取自动连接的个数
int CBTData::GetAutoConnectNum()
{
    int iNum = 0;
    for (LIST_BT_DEVICE::iterator itBegin = m_listDevice.begin(); itBegin != m_listDevice.end();
            ++itBegin)
    {
        DeviceInfo * pDeviceInfo = (*itBegin);
        if (NULL != pDeviceInfo
                && IsDevicePaired(pDeviceInfo->eState)
                && pDeviceInfo->bAutoConnect
                && IsTypeSupportAC(pDeviceInfo->eType)
           )
        {
            iNum++;
        }
    }
    return iNum;
}

//返回扫描设备列表
bool CBTData::GetScanedDeviceList(UI_LIST_BT_DEVICE & dstList, int iStartPos, int iNumber)
{
    return CopyScanedDeviceList(dstList, m_listDevice, iStartPos, iNumber);
}

// 获取扫描列表大小
int CBTData::GetScanedDeviceListSize()
{
    int iCount = 0;
    for (LIST_BT_DEVICE::iterator itBegin = m_listDevice.begin();
            itBegin != m_listDevice.end();
            ++ itBegin)
    {
        DeviceInfo * pDevInfo = (*itBegin);
        if (pDevInfo && pDevInfo->bScanedDev)
        {
            iCount ++;
        }
    }

    return iCount;
}

//更新自动连接设备
void CBTData::UpdateAutoConnectDevice(unsigned long ulDeviceHandle, bool bAutoConnect /* = true */)
{
    for (LIST_BT_DEVICE::iterator itBegin = m_listDevice.begin(); itBegin != m_listDevice.end();
            ++itBegin)
    {
        DeviceInfo * pDeviceInfo = (*itBegin);

        if (NULL == pDeviceInfo
                || !_BTData.IsTypeSupportAC(pDeviceInfo->eType))
        {
            continue;
        }

        if (ulDeviceHandle == BT_DEVICE_HANDLE_INVAILED
                || ulDeviceHandle == pDeviceInfo->devHandle)
        {
            pDeviceInfo->bAutoConnect = bAutoConnect;
            continue;
        }

        pDeviceInfo->bAutoConnect = !bAutoConnect;
    }
}

// 按序号获取已匹配的设备信息
DeviceInfo * CBTData::GetSupportAutoConnectDev(int nIndex)
{
    int iNum = 0;
    for (LIST_BT_DEVICE::iterator itBegin = m_listDevice.begin();
            itBegin != m_listDevice.end();
            ++itBegin)
    {
        DeviceInfo * devInfo = (*itBegin);
        if (NULL != devInfo
                && IsDevicePaired(devInfo->eState)
                && devInfo->bAutoConnect
                && IsTypeSupportAC(devInfo->eType))
        {
            if ((nIndex == iNum)
                    || (-1 == nIndex))
            {
                return devInfo;
            }
            else
            {
                iNum++;
            }
        }
    }
    return NULL;
}

//移除unknowtype device
bool CBTData::RemoveUnknownDevice()
{
    LIST_BT_DEVICE::iterator itBegin = m_listDevice.begin();
    for (; itBegin != m_listDevice.end();)
    {
        DeviceInfo * pTempInfo = (*itBegin);
        if (NULL == pTempInfo
                || pTempInfo->eState != WIRE_DEVICE_INIT_STATE)
        {
            ++ itBegin;
            continue;
        }

        // 如果还是不支持的设备，包括未知类型，则删除掉
        if (!IsSupportDevice(pTempInfo->eType))
        {
            itBegin = m_listDevice.erase(itBegin);
            SAFE_DELETE(pTempInfo);
            continue;
        }

        ++itBegin;
    }

    return true;
}

// 扫描结束后，更新unknown type的设备信息，因为只有在扫描结束后才能更新
// bool CBTData::UpdateScanedDeviceInfo()
// {
//  LIST_BT_DEVICE::iterator itBegin = m_listDevice.begin();
//  for (; itBegin != m_listDevice.end();)
//  {
//      DeviceInfo *pTempInfo = (*itBegin);
//
//      // 只更新扫描到的设备
//      if (NULL == pTempInfo
//          || !pTempInfo->bScanedDev)
//      {
//          ++itBegin;
//          continue;
//      }
//
//      // 更新类型
//      pTempInfo->eType = _BTDriver.GetDeviceType(pTempInfo->devHandle);
//
//      // 如果还是unknown type，则删除掉
//      if (BT_UNKNOWN_TYPE == pTempInfo->eType)
//      {
//          _BTDriver.RemoveDevice(pTempInfo->devHandle);
//          itBegin = m_listDevice.erase(itBegin);
//          SAFE_DELETE(pTempInfo);
//          continue;
//      }
//
//      // 对于非unknown type，重新更新数据
//      DeviceProperty devProperty;
//      _BTDriver.GetDeviceProperty(pTempInfo->devHandle, devProperty);
//
//      CopyBluetoothInfo(pTempInfo, devProperty);
//
//      ++itBegin;
//  }
//
//  RemoveUnknownDevice();
//  return true;
// }

#if 0
// 打印设备信息
void CBTData::PrintDevProperty(const btkit_dev_t * property, btkit_dev_type_t type)
{
    if (property == NULL)
    {
        return;
    }

    WIRE_INFO("\tdev property:");
    WIRE_INFO("\taddr:      %s", property->bt_dev_addr);
    WIRE_INFO("\tname:      %s", property->name);
    WIRE_INFO("\tclass:     0x%x type %d", property->bt_class, type);
    WIRE_INFO("\talias:     %s", property->alias);
    WIRE_INFO("\tvendor:    0x%x", property->vendor);
    WIRE_INFO("\tvendor_source: 0x%x", property->vendor_source);
    WIRE_INFO("\tproduct:   0x%x", property->product);
    WIRE_INFO("\tversion:   0x%x", property->version);
    WIRE_INFO("\tparied:    %s", property->paired ? "True" : "False");
    WIRE_INFO("\ttrusted:   %s", property->trusted ? "True" : "False");
    WIRE_INFO("\tblocked:   %s", property->blocked ? "True" : "False");
    WIRE_INFO("\tconnected: %s", property->connected ? "True" : "False");
    WIRE_INFO("\n");
}
#endif // 0

//添加新的设备
bool CBTData::AddDevice(unsigned long ulDeviceHandle, int iScanIndex)
{
    if (m_listDevice.size() > (BT_MAX_SCAN + BT_MAX_SAVE))
    {
        return false;
    }

    // 获取设备属性
    DeviceProperty devProperty;
    _BTDriver.GetDeviceProperty(ulDeviceHandle, devProperty);

    if (!IsSupportDevice(devProperty.dev_type))
    {
        WIRE_INFO("Found device, but the type is not surported, is[%d]", devProperty.dev_type);
        return false;
    }

    DeviceInfo * pDeviceInfo = GetDeviceInfoByHandle(ulDeviceHandle);
    if (NULL == pDeviceInfo)
    {
        pDeviceInfo = new DeviceInfo;
        m_listDevice.push_back(pDeviceInfo);
    }

    // 拷贝设备配置信息
    pDeviceInfo->devHandle = ulDeviceHandle;
    CopyBluetoothInfo(pDeviceInfo, devProperty);

    // 防止同一设备, 扫描下标变动
    if (!pDeviceInfo->bScanedDev)
    {
        pDeviceInfo->iScanIndex = iScanIndex;
        pDeviceInfo->bScanedDev = true;
    }

    // 记录本次扫描到的设备句柄
    m_listScanedDevHandle.push_back(ulDeviceHandle);

    WIRE_INFO("Found Device:Mac[%s], Handle[%ld], type[%d], Scan index[%d]",
              pDeviceInfo->strMac.c_str(),
              pDeviceInfo->devHandle,
              pDeviceInfo->eType,
              pDeviceInfo->iScanIndex
             );

    return true;
}

#ifdef IF_BLUETOOTH_SUPPORT_REMOTE_PAIR_REQ
//添加新的设备
bool CBTData::AddDevice(unsigned long ulDeviceHandle)
{
    if (m_listDevice.size() > (BT_MAX_SCAN + BT_MAX_SAVE))
    {
        return false;
    }

    DeviceInfo *pDeviceInfo = GetDeviceInfoByHandle(ulDeviceHandle);
    if (NULL == pDeviceInfo)
    {
        pDeviceInfo = new DeviceInfo;
        m_listDevice.push_back(pDeviceInfo);
    }

    // 获取设备属性
    DeviceProperty devProperty;
    _BTDriver.GetDeviceProperty(ulDeviceHandle, devProperty);

    // 构造设备信息
    pDeviceInfo->devHandle = ulDeviceHandle;
    pDeviceInfo->strMac = devProperty.dev_property.bt_dev_addr;
    pDeviceInfo->strName = (0 == strcmp(devProperty.dev_property.alias, "")) ?
                           devProperty.dev_property.name : devProperty.dev_property.alias;
    if (pDeviceInfo->strName.empty())
    {
        pDeviceInfo->strName = pDeviceInfo->strMac;
    }
    pDeviceInfo->eType = devProperty.dev_type;

    WIRE_INFO("Add Device:Mac[%s], Handle[%ld], type[%d], name[%s]",
              pDeviceInfo->strMac.c_str(),
              pDeviceInfo->devHandle,
              pDeviceInfo->eType,
              pDeviceInfo->strName.c_str()
             );

    return true;
}
#endif // IF_BLUETOOTH_SUPPORT_REMOTE_PAIR_REQ

//获取设备类型
btkit_dev_type_t CBTData::GetDeviceType(unsigned long ulDeviceHandle)
{

    DeviceInfo * pDevice = GetDeviceInfoByHandle(ulDeviceHandle);
    if (NULL == pDevice)
    {
        return BT_UNKNOWN_TYPE;
    }

    return pDevice->eType;
}

//删除设备
bool CBTData::DeleteDevice(unsigned long ulDeviceHandle, bool bDeleteAll/*= false*/)
{
    bool bDelete = true;
    LIST_BT_DEVICE::iterator it = m_listDevice.begin();
    for (; it != m_listDevice.end();)
    {
        DeviceInfo * pDevice = *it;
        if (NULL == pDevice)
        {
            ++it;
            continue;
        }

        if (pDevice->devHandle == ulDeviceHandle
                || bDeleteAll)
        {
            WLAsyncHelper_BtAsync(BT_TASK_DELETEDEVICE, pDevice->devHandle);

            if (pDevice->devHandle == GetLastSupportACDevHandle())
            {
                SetLastSupportACDevHandle(BT_DEVICE_HANDLE_INVAILED);
            }

            it = m_listDevice.erase(it);
            SAFE_DELETE(pDevice);

            if (bDeleteAll)
            {
                continue;
            }
            break;
        }
        ++it;
    }

    // 保存最新设备列表
    SaveBluetoothConfig();

    return bDelete;
}

// 获取指定序号的蓝牙设备信息
DeviceInfo * CBTData::GetDeviceInfoByHandle(unsigned long ulDeviceHandle)
{
    LIST_BT_DEVICE::iterator it = m_listDevice.begin();
    for (; it != m_listDevice.end(); ++it)
    {
        DeviceInfo * pDevice = *it;
        if (NULL != pDevice
                && ulDeviceHandle == pDevice->devHandle)
        {
            return pDevice;
        }
    }
    return NULL;
}

//获取设备Handle
unsigned long CBTData::GetDeviceHandleByMac(const yl::string & strMac)
{
    LIST_BT_DEVICE::iterator it = m_listDevice.begin();
    for (; it != m_listDevice.end(); ++it)
    {
        DeviceInfo * pDevice = *it;
        if (NULL != pDevice
                && strMac == pDevice->strMac)
        {
            return pDevice->devHandle;
        }
    }

    return BT_DEVICE_HANDLE_INVAILED;
}

//添加设备到等待对列
void CBTData::AddDeviceToWaittingConnectList(unsigned long ulDevHandle)
{
    // 压入链表，等待连接
    WIRE_WARN("There is already one device connecting to device, add[%d] to waitting list",
              ulDevHandle);
    m_listWaitingDevice.push_back(ulDevHandle);
}

//是否在等待队列中
bool CBTData::IsInWaittingList(unsigned long ulDevHandle)
{
    if (BT_DEVICE_HANDLE_INVAILED == ulDevHandle)
    {
        return false;
    }

    for (YLList<unsigned long>::iterator itBeg = m_listWaitingDevice.begin();
            itBeg != m_listWaitingDevice.end();
            ++itBeg)
    {
        if (ulDevHandle == (*itBeg))
        {
            return true;
        }
    }

    return false;
}

//是否支持的类型
bool CBTData::IsSupportDevice(btkit_dev_type_t eType)
{
    if (false
#ifdef IF_BT_SUPPORT_HEADSET
            || _ISBTHEADSET(eType)
#endif
#ifdef IF_BT_SUPPORT_PHONE
            || _ISBTPHONE(eType)
#endif
       )
    {
        return true;
    }

    return false;
}

//获取驱动中的设备信息
bool CBTData::GetDeviceInDriver(LIST_BT_DEVICE & listDevice)
{
    YLList<DeviceProperty> listProperty;
    if (!_BTDriver.IterateDevice(listProperty))
    {
        return false;
    }

    for (YLList<DeviceProperty>::iterator itBeg = listProperty.begin(); itBeg != listProperty.end();
            ++ itBeg)
    {
        DeviceProperty devProperty = (*itBeg);
        if (devProperty.dev_type == BT_UNKNOWN_TYPE)
        {
            WLAsyncHelper_BtAsync(BT_TASK_DELETEDEVICE, devProperty.dev_hnd);
            continue;
        }
        if (IsSupportDevice(devProperty.dev_type))
        {
            DeviceInfo * pDeviceInfo = new DeviceInfo;
            pDeviceInfo->eType = devProperty.dev_type;
            pDeviceInfo->strMac = devProperty.dev_property.bt_dev_addr;
            pDeviceInfo->strName = (0 == strcmp(devProperty.dev_property.alias,
                                                "")) ? devProperty.dev_property.name : devProperty.dev_property.alias;
            pDeviceInfo->devHandle = devProperty.dev_hnd;

            //设备状态,需要在设备信息管理中再次对状态进行细化处理。
            if (devProperty.dev_property.paired)
            {
                pDeviceInfo->bPaired = true;
                if (_BTDriver.IsDeviceConnectedInDriver(pDeviceInfo->devHandle))
                {
                    pDeviceInfo->eState = WIRE_DEVICE_CONNECTED_STATE;
                }
                else
                {
                    if (!IsDevicePaired(pDeviceInfo->eState))
                    {
                        pDeviceInfo->eState = WIRE_DEVICE_PAIRED_STATE;
                    }
                }
            }
            else
            {
                pDeviceInfo->eState = WIRE_DEVICE_INIT_STATE;
            }

            listDevice.push_back(pDeviceInfo);

            // 更新profile开关状态
            UpdateProfilesActivity(devProperty.dev_hnd, PROFILE_MOBILE);
            UpdateProfilesActivity(devProperty.dev_hnd, PROFILE_A2DP);
        }
    }

    WIRE_INFO("Device In Driver:");
    ShowBluetoothList();

    return true;
}

//从等待对列取出一个设备
unsigned long CBTData::GetWaittingDevice(bool bPopOther/* = true*/)
{
    if (m_listWaitingDevice.size() == 0)
    {
        return BT_DEVICE_HANDLE_INVAILED;
    }

    unsigned long ulHandle = m_listWaitingDevice.back();
    if (bPopOther)
    {
        m_listWaitingDevice.clear();
    }

    WIRE_INFO("Get Waitting Device[%d] pop Other[%d]", ulHandle, bPopOther);
    return ulHandle;
}

// 打印设备信息
void CBTData::PrintDevProperty(const DeviceProperty & property)
{
    WIRE_INFO("\tdev property:");
    WIRE_INFO("\tdev_handle:      %s", property.dev_hnd);
    WIRE_INFO("\ttype:      %d", property.dev_type);
    WIRE_INFO("\taddr:      %s", property.dev_property.bt_dev_addr);
    WIRE_INFO("\tname:      %s", property.dev_property.name);
    WIRE_INFO("\tclass:     0x%x", property.dev_property.bt_class);
    WIRE_INFO("\talias:     %s", property.dev_property.alias);
    WIRE_INFO("\tvendor:    0x%x", property.dev_property.vendor);
    WIRE_INFO("\tvendor_source: 0x%x", property.dev_property.vendor_source);
    WIRE_INFO("\tproduct:   0x%x", property.dev_property.product);
    WIRE_INFO("\tversion:   0x%x", property.dev_property.version);
    WIRE_INFO("\tparied:    %s", property.dev_property.paired ? "True" : "False");
    WIRE_INFO("\ttrusted:   %s", property.dev_property.trusted ? "True" : "False");
    WIRE_INFO("\tblocked:   %s", property.dev_property.blocked ? "True" : "False");
    WIRE_INFO("\tconnected: %s", property.dev_property.connected ? "True" : "False");
    WIRE_INFO("\n");
}

// 设置是否自动连接
bool CBTData::SetAutoConnect(bool bAutoConnect, unsigned long ulDeviceHandle)
{
    DeviceInfo * pDeviceInfo = GetDeviceInfoByHandle(ulDeviceHandle);
    if (NULL == pDeviceInfo)
    {
        return false;
    }

    WIRE_INFO("set mac[%s] autoconnect= %d", pDeviceInfo->strMac.c_str(), bAutoConnect);
    pDeviceInfo->bAutoConnect = bAutoConnect;
    return true;
}

//设置profile开关
bool CBTData::SetProfileEnable(unsigned long ulDeviceHandle, bool bEnable, int iProfile)
{
    DeviceInfo *pDeviceInfo = GetDeviceInfoByHandle(ulDeviceHandle);
    if (NULL == pDeviceInfo)
    {
        return false;
    }

    bool bHandle = false;
    switch (iProfile)
    {
    case PROFILE_MOBILE:
        if (pDeviceInfo->bMobileEnable != bEnable)
        {
            pDeviceInfo->bMobileEnable = bEnable;
            bHandle = true;
        }
        break;
    case PROFILE_A2DP:
        if (pDeviceInfo->a2dpInfo.bEnable != bEnable)
        {
            pDeviceInfo->a2dpInfo.bEnable = bEnable;
            bHandle = true;
        }
        break;
    case PROFILE_PBA:
        if (pDeviceInfo->bSyncContact != bEnable)
        {
            pDeviceInfo->bSyncContact = bEnable;
            bHandle = true;
        }
        break;
    default:
        break;
    }

    WIRE_INFO("set device[%s][%s], profile[%d], bEnable[%d], result[%d]",
              pDeviceInfo->strName.c_str(), pDeviceInfo->strMac.c_str(), iProfile, bEnable, bHandle);

    if (bHandle)
    {
        // 设置Net标识
        if (iProfile == PROFILE_MOBILE
                || iProfile == PROFILE_A2DP)
        {
            _BTDriver.SetProfileEnable(pDeviceInfo->devHandle, iProfile, bEnable);
        }
        else
        {
            // 同步状态保存到文件
            _BTData.SaveBluetoothConfig();
        }
    }

    return bHandle;
}

//获取profile开关
bool CBTData::GetProfileEnable(unsigned long ulDeviceHandle, int iProfile)
{
    DeviceInfo *pDeviceInfo = GetDeviceInfoByHandle(ulDeviceHandle);
    if (NULL == pDeviceInfo)
    {
        return false;
    }

    bool bEnable = false;
    switch (iProfile)
    {
    case PROFILE_MOBILE:
        bEnable = pDeviceInfo->bMobileEnable;
        break;
    case PROFILE_A2DP:
        bEnable = pDeviceInfo->a2dpInfo.bEnable;
        break;
    case PROFILE_PBA:
        bEnable = pDeviceInfo->bSyncContact;
        break;
    default:
        break;
    }

    WIRE_INFO("Get dev[%d] profile [%d] enable [%d]", ulDeviceHandle, iProfile, bEnable);
    return bEnable;
}

// 设置a2dp状态
bool CBTData::SetDevA2dpState(unsigned long ulHandle, LOGIC_A2DP_STATE eState)
{
    DeviceInfo* pDevice = _BTData.GetDeviceInfoByHandle(ulHandle);
    if (!pDevice
            || eState <= LOGIC_A2DP_STATE_BEGIN
            || eState >= LOGIC_A2DP_STATE_END)
    {
        return false;
    }

    if (pDevice->a2dpInfo.eState == eState)
    {
        return true;
    }

    pDevice->a2dpInfo.eState = eState;

    static char* szA2dpState[] = { "disconnect", "connected", "playing", "paused" };
    WIRE_INFO("set device[%s][%s], a2dp state[%s]",
              pDevice->strName.c_str(), pDevice->strMac.c_str(), szA2dpState[eState]);

    return true;
}

//获取设备状态
WIRELESS_DEVICE_STATE CBTData::GetDeviceState(unsigned long ulDeviceHandle)
{
    DeviceInfo * pDevice = GetDeviceInfoByHandle(ulDeviceHandle);
    return (NULL == pDevice) ? WIRE_DEVICE_INIT_STATE : pDevice->eState;
}

// 改变设备状态
bool CBTData::SetDeviceState(unsigned long ulDeviceHandle, WIRELESS_DEVICE_STATE eState)
{
    DeviceInfo * pDeviceInfo = GetDeviceInfoByHandle(ulDeviceHandle);
    if (NULL == pDeviceInfo
            || (pDeviceInfo->eState == eState))
    {
        return false;
    }

    WIRE_INFO("Change BT[%s] State [%s]==>[%s]", pDeviceInfo->strMac.c_str(),
              DeviceStateToString(pDeviceInfo->eState), DeviceStateToString(eState));

    pDeviceInfo->eState = eState;
    return true;
}

bool CBTData::IsDeviceInList(const yl::string & strMac, LIST_BT_DEVICE & srcList)
{
    for (LIST_BT_DEVICE::iterator itBeg = srcList.begin();
            itBeg != srcList.end();
            ++itBeg)
    {
        DeviceInfo * pDevice = *itBeg;

        if (NULL == pDevice)
        {
            continue;
        }

        if (strMac == pDevice->strMac)
        {
            return true;
        }
    }

    return false;
}

// 更新profile开关状态
void CBTData::UpdateProfilesActivity(unsigned long ulHandle, int iProfiles /* PROFILE_MOBILE | PROFILE_A2DP */)
{
    DeviceInfo *pDeviceInfo = GetDeviceInfoByHandle(ulHandle);
    if (!pDeviceInfo)
    {
        return;
    }

    if (iProfiles & PROFILE_MOBILE)
    {
        pDeviceInfo->bMobileEnable = _BTDriver.IsProfileEnable(ulHandle, PROFILE_MOBILE);
        WIRE_INFO("Update mobile profile status[%d]", pDeviceInfo->bMobileEnable);
    }

    if (iProfiles & PROFILE_A2DP)
    {
        pDeviceInfo->a2dpInfo.bEnable = _BTDriver.IsProfileEnable(ulHandle, PROFILE_A2DP);
        WIRE_INFO("Update a2dp profile status[%d]", pDeviceInfo->a2dpInfo.bEnable);
    }
}

// 保存蓝牙相关配置信息
// 主要有: 设备mac, 是否同步联系人, 最后一次连接的手机信息
bool CBTData::SaveBluetoothConfig()
{
    if (m_listDevice.size() == 0)
    {
        return false;
    }

    xml_document doc;

    // 设置Xml文件头.
    xml_node nodeDecl = doc.prepend_child(node_declaration);
    nodeDecl.append_attribute("version", "1.0");
    nodeDecl.append_attribute("encoding", "utf-8");

    // 增加根节点.
    xml_node rootNode = doc.append_child("bluetooth");
    if (!rootNode)
    {
        return false;
    }

    LIST_BT_DEVICE::iterator itBegin = m_listDevice.begin();
    for (; itBegin != m_listDevice.end(); ++itBegin)
    {
        DeviceInfo * pDeviceInfo = (*itBegin);
        if (NULL == pDeviceInfo
                || pDeviceInfo->strMac.empty()
                || !_ISBTPHONE(pDeviceInfo->eType)
                || !IsDevicePaired(pDeviceInfo->eState))
        {
            continue;
        }

        xml_node deviceNode = rootNode.append_child("device");
        if (!deviceNode)
        {
            continue;
        }

        deviceNode.append_attribute("mac", pDeviceInfo->strMac.c_str());
        deviceNode.append_attribute("synccontact", pDeviceInfo->bSyncContact ? "1" : "0");
        // 读取最后一次连接的手机, 及该手机是否为最后连接的设备
        if (pDeviceInfo->devHandle == GetLastSupportACDevHandle())
        {
            deviceNode.append_attribute("last_connect_phone", true);
            deviceNode.append_attribute("is_last_connect", m_bLastDevIsSupportAC ? true : false);
        }
        deviceNode.append_attribute("connect_success_times", pDeviceInfo->uConnectSuccessTimes);
    }

    doc.save_file(BLUETOOTH_CONFIG_FILE);
    WIRE_INFO("Save Bluetooth file");
    return true;
}

// 加载配置文件,读取之前匹配过的信息，主要是是否同步联系人及最后一次连接的手机信息
bool CBTData::LoadConfig()
{
    WIRE_INFO("Load Bluetooth file");
    xml_document doc;
    if (!doc.load_file(BLUETOOTH_CONFIG_FILE))
    {
        return false;
    }

    xml_node rootNode = doc.child("bluetooth");
    if (!rootNode)
    {
        return false;
    }

    xml_node childNode = rootNode.child("device");
    while (childNode)
    {
        xml_attribute attr = childNode.attribute("mac");
        if (!attr
                || 0 == strcmp(attr.value(), ""))
        {
            childNode = childNode.next_sibling("device");
            continue;
        }

        yl::string strMac = attr.value();

        xml_attribute attrSyncContact = childNode.attribute("synccontact");
        xml_attribute attrLastConnectPhone = childNode.attribute("last_connect_phone");
        xml_attribute attrIsLastConenctDev = childNode.attribute("is_last_connect");
        if (attr)
        {
            bool bSyncContact = (0 == strcmp(attrSyncContact.value(), "1"));

            LIST_BT_DEVICE::iterator it = m_listDevice.begin();
            for (; it != m_listDevice.end(); ++it)
            {
                DeviceInfo * pDeviceInList = *it;

                if (pDeviceInList
                        && strMac == pDeviceInList->strMac)
                {
                    pDeviceInList->bSyncContact = bSyncContact;

                    // 将最后一次连接的手机, 及该手机是否为最后连接的设备写入
                    if (0 == strcmp(attrLastConnectPhone.value(), "1"))
                    {
                        pDeviceInList->bAutoConnect = true;
                        SetLastSupportACDevHandle(pDeviceInList->devHandle);
                        m_bLastDevIsSupportAC = (0 == strcmp(attrIsLastConenctDev.value(), "1")) ? true : false;
                    }
                    pDeviceInList->uConnectSuccessTimes = atoi(childNode.attribute("connect_success_times").value());
                }
            }
        }

        childNode = childNode.next_sibling("device");
    }
    return true;
}

// 通过handle比较设备本地信息和最新扫描获取到的信息
bool CBTData::CompareDevInfo(unsigned long ulHandle)
{
    DeviceInfo * pDeviceInfo = _BTData.GetDeviceInfoByHandle(ulHandle);
    if (pDeviceInfo == NULL)
    {
        return true;
    }

    DeviceProperty devProperty;
    _BTDriver.GetDeviceProperty(ulHandle, devProperty);

    if ((0 != strcmp(devProperty.dev_property.name, pDeviceInfo->strName.c_str())
            && 0 != strcmp(devProperty.dev_property.alias, pDeviceInfo->strName.c_str()))
            || pDeviceInfo->strMac != devProperty.dev_property.bt_dev_addr
            || pDeviceInfo->eType != devProperty.dev_type)
    {
        return false;
    }

    return true;
}

// 校正本地扫描列表的设备信息, 移除不是本次扫描到的设备
void CBTData::CheckScanedDevList()
{
    /*if (m_listScanedDevHandle.size() <= 0)
    {
        return ;
    }*/

    for (LIST_BT_DEVICE::iterator iterDevInfo = m_listDevice.begin();
            iterDevInfo != m_listDevice.end();)
    {
        DeviceInfo * pDevInfo = *iterDevInfo;
        if (NULL == pDevInfo)
        {
            ++iterDevInfo;
            continue;
        }


        // 筛选出列表中不是本次被扫描到设备
        bool bIsDevFound = false;
        for (YLList<unsigned long>::ListIterator iterHandle = m_listScanedDevHandle.begin();
                iterHandle != m_listScanedDevHandle.end();
                ++ iterHandle)
        {
            unsigned long ulHandle = *iterHandle;
            if (pDevInfo->devHandle == ulHandle)
            {
                ++ iterDevInfo;
                bIsDevFound = true;
                break;
            }
        }

        // 不是被扫描到的认为不存在, 从列表移除
        if (!bIsDevFound)
        {
            if (pDevInfo->eState != WIRE_DEVICE_INIT_STATE)
            {
                // 不是初始状态的设备, 则把被扫描到的标识置为false
                pDevInfo->bScanedDev = false;
                ++ iterDevInfo;
                continue;
            }
            else
            {
                // 初始状态的设备, 则删除设备信息
                iterDevInfo = m_listDevice.erase(iterDevInfo);
                WIRE_INFO("###########delete device..mac[%s]", pDevInfo->strMac.c_str());
                SAFE_DELETE(pDevInfo);
            }
        }
    }
}

// 清空记录扫描到的设备handle列表
void CBTData::ClearScanedDevHandleList()
{
    m_listScanedDevHandle.clear();
}

// 判断是否支持自动连接的类型
bool CBTData::IsTypeSupportAC(btkit_dev_type_t eType)
{
    return _ISBTPHONE(eType);
}

// 设置最后一个连接上的设备
void CBTData::SetLastSupportACDevHandle(unsigned long ulHandle)
{
    if (!IsTypeSupportAC(GetDeviceType(ulHandle)))
    {
        m_bLastDevIsSupportAC = false;
        return;
    }

    m_bLastDevIsSupportAC = true;
    m_ulLastSupportACDevHandle = ulHandle;
}

bool CBTData::GetPairedDevice(DeviceInfo& info)
{
	LIST_BT_DEVICE::iterator itBegin = m_listDevice.begin();
	for (; itBegin != m_listDevice.end(); ++itBegin)
	{
		DeviceInfo * pTempInfo = (*itBegin);
		if (NULL == pTempInfo)
		{
			continue;
		}

		if (pTempInfo->eState == WIRE_DEVICE_CONNECTED_STATE)
		{
			info = (*pTempInfo);
			return true;
		}
	}

	return false;
}

#endif // IF_SUPPORT_BLUETOOTH

