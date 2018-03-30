#ifndef __COMMON_UNITS_DEFINE_H__
#define __COMMON_UNITS_DEFINE_H__

#include <interfacedefine.h>
#include "devicelib/phonedevice.h"

/////////////////////// TaskAction 专用线程名 ///////////////////////////
// 本地搜索和超级搜索复用
#define TA_THREAD_LOCAL_SEARCH          "threadLocalSearch"
// ldap联系人搜索
#define TA_THREAD_LDAP                  "threadLDAP"
// usb耳麦
#define TA_THREAD_USB_HEADSET           "threadUsbHeadset"
// 存储设备辅助线程
#define TA_THREAD_STORAGE_HELP          "threadStorageHelp"
// exp 更新列表线程
#define TA_THREAD_EXP_UPDATE_LIST       "threadExpUpdateList"
// exp drawer
#define TA_THREAD_EXP_DRAWER            "threadExpDrawer"
// 热拔插检测线程
#define TA_THREAD_SCAN_HOTPLUG          "threadScanHotplug"
// rj口exp插入检测线程
#define TA_THREAD_RJEXP_INSERT_SCAN     "threadRjExpInsertScan"
// usb口exp插入检测线程
#define TA_THREAD_USBEXP_INSERT_SCAN    "threadUsbExpInsertScan"

///////////////////////////Status Manager /////////////////////////////
//enum POWER_FLASH_REASON
//{
//    PFR_NETWORK_UNAVAIL = 0,
//    PFR_ACCOUNT_UNAVAIL = 1,
//};

//// 电源灯策略
//enum POWER_LIGHT_POLICY
//{
//    PLP_AUTO = -1,
//    PLP_OFF = 0,
//    PLP_ON,
//    PLP_FLASH_SLOW1,    // 300ms
//    PLP_FLASH_SLOW2,    // 1000ms
//};



#define WILDCARD_DEVNAME "$DEVNAME"

//////////////////////////////////////////////////////////////////////////

// 文件类型.
enum PHFileType
{
    PHFILE_TYPE_NONE = 0,  // 不正确的文件类型.
    PHFILE_TYPE_CONTACT_IMAGE = 1,  // 联系人头像.
    PHFILE_TYPE_PHOTO_FILE = 2,  // 图像文件.
    PHFILE_TYPE_RING_FILE = 3,  // 铃声文件.
    PHFILE_TYPE_WALLPAPER_FILE = 4,  // 壁纸文件.
    PHFILE_TYPE_SCREENSAVER_FILE = 5,  // 屏保文件.
    PHFILE_TYPE_EXP_WALLPAPER_FILE = 6,  // EXP壁纸文件.
    PHFILE_TYPE_MAX,
};

// 测试模式类型
enum TestMode_Type
{
    TST_NONE = -1,
    TST_NORMAL = 0,
    TST_FULLVOICE,
    TST_PREBURNING,
    TST_CHECKFLAG,
    TST_AUDIODEVICE,
    TST_VIEWSTATUS,
    TST_SETVOICE,
    //半成品
    TST_SFP,
    TST_ERROR_TRIGGER,
    TST_MANAUAL_KEY,
    TST_WIFI,
};

enum IMAGE_SCALED_TYPE
{
    NOMAL_IMAGE = 0,
    WALL_PAPER_IMAGE
};

enum IMAGE_SCALE_RETURN_TYPE
{
    IMAGE_SIZE_ERROR = -1,  //图片大小有误
    IMAGE_SIZE_OVER = 0,    //图片过大
    IMAGE_TYPE_ERROR,       //图片类型错误
    IMAGE_TYPE_CANNOT_READ, //图片无法读取
    IMAGE_SIZE_HAVE_ZERO,   //图片大小存在0
    IMAGE_SCALED_SUCCESS,   //成功保存
    IMAGE_SCALED_FAILE,     //保存失败
    IMAGE_NO_ENOUGH_SPACE   //空间不足
};

// 恢复出厂状态
enum Factory_State
{
    FACSTATE_NONE = -1,   // 未设置
    FACSTATE_DISABLE,     // 不能进行恢复出厂
    FACSTATE_ENABLE,      // 可以进行恢复出厂
};

// sysdata目录中的配置文件.
const char * const kszSysConfigPath = "Resource:";
// vardata目录中的配置文件.
const char * const kszVarConfigPath = "Booked:";
// userdata目录中的配置文件.
const char * const kszUserConfigPath = "Config:";

// CommonUnits消息
enum CommonUnitsMessageDefine
{
    COMMONUNITS_MESSAGE_BEGIN = INNER_MSG_CALLCOMPLETION_FIRST,

    // 通知check-sync要更新配置了
    COMMONUNITS_MESSAGE_CHECKSYNC_UPDATECONFIG,

    COMMONUNITS_MESSAGE_END = INNER_MSG_CALLCOMPLETION_LAST
};
enum CONFIG_PROCESS_TYPE
{
    INI_FILE_PROCESS = 0,
    CFG_SERVER_PROCESS = 1,
    NONE_PROCESS = 2
};
// 获取配置处理的机制 ini文件机制/cfgserver配置服务机制
inline CONFIG_PROCESS_TYPE GET_CONFIG_PROCESS_TYPE()
{
#ifdef WIN32
    return CFG_SERVER_PROCESS;
#else
    static CONFIG_PROCESS_TYPE eType = NONE_PROCESS;
    if (eType != NONE_PROCESS)
    {
        return eType;
    }
    PHONE_TYPE_DEF phone_type = devicelib_GetPhoneType();
    switch (phone_type)
    {
    case PT_T46S:
    case PT_T48S:
    case PT_T27S:
    case PT_T49:
    case PT_T43P:
    case PT_T43S:
    case PT_T54S:
    case PT_T52:
    case PT_T40G:
    case PT_CP920:
        eType = CFG_SERVER_PROCESS;
        break;
    default:
        eType = INI_FILE_PROCESS;
        break;
    }
    return eType;
#endif
}

// Enum. Reset option.
enum ResetOptionType
{
    RESET_OPTION_TYPE_NONE = 0,  // Invalid reset option.
    RESET_OPTION_CONFIG_ONLY = 1 << 0,  // Reset config only.
    RESET_OPTION_USERDATA_ONLY = 1 << 1,  // Reset user data only.
    RESET_OPTION_ALL = RESET_OPTION_CONFIG_ONLY | RESET_OPTION_USERDATA_ONLY,  // Reset
    RESET_OPTION_USERDATA_USERCFG = 4,
    // config and user data.
    RESET_OPTION_TYPE_MAX
};

enum AMR_LICENSE_RESULT
{
    AMR_LICENSE_NOT_INSTALL = 0,
    AMR_LICENSE_INSTALL_SUCCESS,
    AMR_LICENSE_INSTALL_FAIL,
};

enum SYSTEM_LOG_TYPE
{
    LOG_LOCAL_MODE = 0,
    LOG_SERVER_MODE = 1,
    LOG_AUTO_MODE = 2,
};

typedef void(*FuncSetSysLog)(int iModuleID, int iLogLevel);


enum DSK_PHONE_TYPE
{
    DSK_PHONE_TYPE_NONE = -1,  //未知
    DSK_PHONE_TYPE_BLACK = 0,    //黑白屏(T19\T21\T23\T27\T40\T41\T42\CP920\CP860)
    DSK_PHONE_TYPE_COLOR = 1,    //彩屏(T29\T46\T48\T54)
    DSK_PHONE_TYPE_NOSCREEN = 2, //无屏幕(dect)
    DSK_PHONE_TYPE_UNKNOW = 3,
};

#endif // __COMMON_UNITS_DEFINE_H__

