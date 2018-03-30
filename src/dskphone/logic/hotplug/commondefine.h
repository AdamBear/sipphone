#ifndef _HOTPLUG_COMMON_H_
#define _HOTPLUG_COMMON_H_

#include "innermsgdefine.h"
#include <unistd.h>
#include <ylhashmap.h>
#include <ETLLib.hpp>
#include <hpdev/hpdev_api.h>
#include "record/include/common.h"
/************************************************************************/
/*
    定义热插拔设备消息及数据结构，供外部模块使用
*/
/************************************************************************/

enum HotPlugUsbDeviceType
{
    HOTPLUG_USB_USBUNKNOW = -1,
    HOTPLUG_USB_USBSTORAGE = 0,
    HOTPLUG_USB_BTDONGLE = 1,
    HOTPLUG_USB_WIFIDONGLE = 2,
    HOTPLUG_USB_USBHEADSET = 3,
    HOTPLUG_USB_PSTNBOX = 4,
    HOTPLUG_USB_EXP50 = 5,
};

enum HotPlugUsbDeviceState
{
    HOTPLUG_USB_REMOVE = 0,
    HOTPLUG_USB_ADD = 1,
};

// 设备的状态
enum USB_STORAGE_STATE
{
    STATE_NONE = -1,
    STATE_DISCONNECT,   // 设备断开
    STATE_CONNECT,      // 设置连接上
    STATE_DETECTING,    // 设备识别中
    STATE_UNRECOGNIZED, // 无法识别
    STATE_OTHER_DEVICE, // 蓝牙或者wifi dongle
};

typedef struct
{
    USB_STORAGE_STATE eNewRecordState;   // 模块状态
    USB_STORAGE_STATE eOldRecordState;   // 模块旧状态
    HotPlugUsbDeviceType eUsbDeviceType; // 设备类型
    HotPlugUsbDeviceState eUsbDeviceState; //设备状态
} HotPlugUsbModuleInfo;

#if defined(IF_SUPPORT_HDMI) || defined(IF_SUPPORT_VIDEO)
typedef struct
{
    // 扩展屏参数
    // VCS支持5种分辨率，如果显示器的分辨率VCS不支持，就赋值为0,表征当前分辨率是否有效
    int iResValid;

    // 设备分辨率
    unsigned int uResWid;
    unsigned int uResHei;
    // 帧率
    unsigned int uFrmRate;

    // 显示器的物理尺寸:mm
    unsigned int uVertSize;
    unsigned int uHoriSize;

} DevDisResInfo;

// 摄像头状态
enum CameraStatusType
{
    CST_NONE,           // 初始化使用
    CST_NOT_CONNECTED,  // 未连接
    CST_CONNECTED,      // 已经连接但是挡板是关的
    CST_READY,          // 已经连接并且挡板是开的
};

//扩展屏状态
enum HDMIStatusType
{
    HST_NONE = -1,              //初始化
    HST_DISCONNECTED = 0,       //未连接
    HST_CONNECTED = 1,          //已连接
};

// 摄像头状态变更
enum CameraMessageDefine
{
    CAMERA_MSG_NOTIFY_BEGIN = INNER_MSG_CAMERA_MESSAGE_BEGIN,

    // Camera状态改变 wParam: CameraStatusType 摄像头当前状态, lParam:CameraStatusType 旧状态
    CAMERA_MSG_STATUS_CHANGE,

    CAMERA_MSG_NOTIFY_END = INNER_MSG_CAMERA_MESSAGE_END,
};

// HDMI插拔状态变更
enum HdmiMessageDefine
{
    HDMI_MSG_NOTIFY_BEGIN = INNER_MSG_HDMI_MESSAGE_BEGIN,

    // HDMI插拔状态改变 wParam: current status lParam: old status
    HDMI_MSG_STATUS_CHANGE,

    // HDMI开关变化 wParam 0/1 关/开
    HDMI_MSG_SWITCH_CHANGE,

    HDMI_MSG_NOTIFY_END = INNER_MSG_HDMI_MESSAGE_END,
};
#endif

enum USB_CURRENT_STATE
{
    USB_CURRENT_STATE_NONE,
    USB_CURRENT_STATE_NORMAL, // usb电流正常
    USB_CURRENT_STATE_OVER,   // usb电流超限
};

// 一般只限逻辑层使用的硬件状态改变消息
enum HotplugMessageDefine
{
    HOTPLUG_MESSAGE_NOTIFY_BEGIN = INNER_MSG_HOTPLUG_MESSAGE_BEGIN,

    // HDMI插拔状态改变 wParam 0/1 拔/插， 插入时带扩展参数extra: DevDisResInfo
    HOTPLUG_HDMI_STATUS_CHANGE,

    // Camera插拔状态改变 wParam 0/1 拔/插
    HOTPLUG_CAMERA_STATUS_CHANGE,

    // Camera上传感器状态改变即挡板状态改变 wParam 0/1 关/开
    HOTPLUG_CAMERA_SENSOR_STATUS_CHANGE,

    // wparam： 0=拔,1=插  lparam：0
    HOTPLUG_USB_DEVICER_STATE_CHANGE,

    // wparam： 0=断开,1=连接上  lparam：存储设备index, Ext：StorageDir
    HOTPLUG_USB_STORAGE_STATE_CHANGING,

    // usb 过流检测消息
    // wparam: USB_CURRENT_STATE
    HOTPLUG_USB_STORAGE_CURRENT_OVER,

    ////dongle连接上，wparam: 1=Bluetooth 2=wifi lparam: 0=移除 1=插上
    //HOTPLUG_DEVICE_STATE_CHANGE,

    // wparam： 0=拔,1=插  lparam：dev_minor
    HOTPLUG_USB_PSTN_BOX_STATE_CHANGE,

    // 存储设备任务执行 wParam: StorageTaskType, lParam: storage id
    HOTPLUG_STORAGE_TASK,

    // 存储设备任务执行结果 wParam: StorageTaskType, lParam: storage id
    HOTPLUG_STORAGE_TASK_RESULT,

    // 存储设备容量改变 wParam: storage id
    HOTPLUG_USB_STORAGE_VOLUME_CHANGE,

    // 存储设备文件加载结果 wParam: 0/1 失败/成功, lParam: storage id
    HOTPLUG_STORAGE_FILE_LOAD_RESULE,

    // wparam： 0=断开,1=连接上 Ext：StorageDir
    // USB存储设备连接/断开, 未初始化相关资源信息
    HOTPLUG_USB_STORAGE_STATE_CHANGE_UNINIT,

    HOTPLUG_MESSAGE_NOTIFY_END = INNER_MSG_HOTPLUG_MESSAGE_END,
};

// HOTPLUG_STORAGE_TASK_RESULT消息的wParam
enum StorageTaskType
{
    // 存储容量请求 lParam: STORAGE_INVALID表示获取所有, 否则为获取某个存储设备的
    STORAGE_TASK_USB_VOLUME_GET_REQ,
    // 文件加载请求
    STORAGE_TASK_USB_FILE_LOAD_REQ,
    // mount处理
    STORAGE_TASK_USB_MOUNT_REQ,
    // umount处理
    STORAGE_TASK_USB_UMOUNT_REQ,
    // 删除所有文件
    STORAGE_TASK_USB_CLEAR_FILE,
};

#define STORAGE_HELP_THREAD     "StorageHelpThrad"

struct StorageReqFileInfo
{
    int                     iReqType;       // 请求类型:record/picture…
    yl::string              strExtName;     // 扩展名
    YLList<yl::string>      listDir;        // 路径
    YLList<yl::string>      listNewFile;    // 请求文件列表
    listRecordFileInfo      listRecordFile; // 请求结果
};
typedef YLList<StorageReqFileInfo>          listStorageReqFile;
typedef YLHashMap<int, listStorageReqFile>  mapStorageId2ReqFile;

#define STORAGE_INVALID         -1
#define STORAGE_DEFAULT         0
struct StorageDevInfo
{
    int         iDevId;         // 设备底层表示
#define  USB_TYPE_NONE          0
#define  USB_TYPE_LOCAL         1
    UINT        uType;          // 类型
    int         iIndex;         // 上层标识
#define  USB_STATUS_UNUSABLE    0
#define  USB_STATUS_USABLE      1
#define  USB_STATUS_ACTIVE      2
#define  USB_STATUS_WILL_UNMOUNT 3
    UINT        uStatus;        // 状态
    UINT        uPriority;      // 优先级
    yl::string  strMountPoint;  // 挂载节点
    yl::string  strDevNode;     // 设备节点
    yl::string  strDevName;     // 设备名称

    StorageDevInfo()
    {
        iDevId = -1;
        uType = USB_TYPE_NONE;
        iIndex = STORAGE_INVALID;
        uStatus = USB_STATUS_UNUSABLE;
        uPriority = 0;
        strMountPoint = "";
        strDevNode = "";
        strDevName = "";
    }

    StorageDevInfo& operator= (const StorageDevInfo& sStorageInfo)
    {
        if (this != &sStorageInfo)
        {
            iDevId = sStorageInfo.iDevId;
            uType = sStorageInfo.uType;
            iIndex = sStorageInfo.iIndex;
            uStatus = sStorageInfo.uStatus;
            uPriority = sStorageInfo.uPriority;
            strMountPoint = sStorageInfo.strMountPoint;
            strDevNode = sStorageInfo.strDevNode;
            strDevName = sStorageInfo.strDevName;
        }

        return *this;
    }
};
typedef YLList<StorageDevInfo>  listStorageDevInfo;

struct StorageVolumeInfo
{
    int                     iId;            // 存储设备id
    double                  dUsedPercent;   // 使用占比
    yl::string              strStorageDir;  // 存储路径
    unsigned long long      ulTotalSize;    // 总容量
    unsigned long long      ulFreeSize;     // 可用容量
    unsigned long long      ulUsedSize;     // 已使用容量

    StorageVolumeInfo()
    {
        iId = STORAGE_INVALID;
        dUsedPercent = 0.0;
        strStorageDir = "";
        ulTotalSize = 0;
        ulFreeSize = 0;
        ulUsedSize = 0;
    }

    StorageVolumeInfo& operator= (const StorageVolumeInfo& sVolumeInfo)
    {
        if (this != &sVolumeInfo)
        {
            iId = sVolumeInfo.iId;
            dUsedPercent = sVolumeInfo.dUsedPercent;
            strStorageDir = sVolumeInfo.strStorageDir;
            ulTotalSize = sVolumeInfo.ulTotalSize;
            ulFreeSize = sVolumeInfo.ulFreeSize;
            ulUsedSize = sVolumeInfo.ulUsedSize;
        }

        return *this;
    }
};
typedef YLList<StorageVolumeInfo> listVolumeInfo;

#endif  // _HOTPLUG_COMMON_H_
