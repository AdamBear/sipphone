/**********************************************************************************************//**
 * @file
 *
 * @brief   Declares the service dsk interface.
 **************************************************************************************************/

#ifndef _DSK_INTERFACE_DEFINE_H_
#define _DSK_INTERFACE_DEFINE_H_

#ifdef WIN32
#define DSK_API_DECLARE __declspec(dllexport)
#else
// cmake 默认隐藏接口
#define DSK_API_DECLARE __attribute ((visibility("default")))
#endif // WIN32

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef WIN32
#ifndef MKSRV_ID_DSK
#define MKSRV_ID_DSK              6      /*LCD-UI/desktop service*/
#endif //MKSRV_ID_DSK

#ifndef MKSRV_MSG
#define MKSRV_MSG(id,msg)         ((((id) & 0xFF) << 16)|((msg) & 0xFFFF) << 0)
#endif //MKSRV_MSG
#else // WIN32
#include <msgkit/mkservice.h>
#endif // WIN32

//////////////////////dskphone id section define//////////////////////////////////
#define DSK_MSG_SIZE        256
#define DSK_MSG_LOGIC       (DSK_MSG_SIZE*1)        // section of dskphone logic
#define DSK_MSG_COMMON      (DSK_MSG_SIZE*13)       // section of common, include web and other module
#define DSK_MSG_SCREEN      (DSK_MSG_SIZE*14)       // section of screen
#define DSK_MSG_TOOL        (DSK_MSG_SIZE*15)       // section of tool

#define DSK_MSG_APP         (DSK_MSG_SIZE*64)       // section of service:application section
#define DSK_MSG_TALK        (DSK_MSG_SIZE*68)       // section of service:talk section
#define DSK_MSG_CONTACT     (DSK_MSG_SIZE*72)       // section of service:contact section
#define DSK_MSG_MENU        (DSK_MSG_SIZE*76)       // section of service:menu section
#define DSK_MSG_CMD         (DSK_MSG_SIZE*100)      // section of service:cmd section
#define DSK_MSG_QUERY       (DSK_MSG_SIZE*120)      // section of service:query section
#define DSK_MSG_EXTENSION   (DSK_MSG_SIZE*128)      // section of extension
#define DSK_MSG_MAX         (DSK_MSG_SIZE*255)      // max limit

#define DSK_MSG_INDEX           0x0001
#define DSK_MSG(sub, id)        MKSRV_MSG(MKSRV_ID_DSK, (sub)+(id))


/************************************************************************/
/*
    消息定义命名规则：DSK_MSG_模块_具体功能 DSK_MSG_开头用于区分各个部门的定义
*/
/************************************************************************/

////////////////////////////////////message define:GUI notify//////////////////////////////////////////////
// 对外发送json格式消息字段定义：建议全小写
#define dsk_ksz_expire          "expire"            // 有效期
#define dsk_ksz_callId          "callId"            // 通话id
#define dsk_ksz_calltype        "calltype"          // 通话类型
#define dsk_ksz_callmask        "callmask"          // 通话标记
#define dsk_ksz_sessiontype     "sessiontype"       // 会话类型
#define dsk_ksz_lineId          "lineId"            // 账号id
#define dsk_ksz_time            "time"              // 时间
#define dsk_ksz_runningtimesec  "runningtimesec"    // 时间：s
#define dsk_ksz_runningtimensec "runningtimensec"   // 时间：ns
#define dsk_ksz_fromuri         "fromuri"           // 呼叫者
#define dsk_ksz_touri           "touri"             // 呼叫对象
#define dsk_ksz_path            "path"              // 路径
#define dsk_ksz_state           "state"             // 状态
#define dsk_ksz_count           "count"             // 计数
#define dsk_ksz_targe           "target"            // 目标
#define dsk_ksz_input_dev       "input_dev"         // 输入设备
#define dsk_ksz_output_dev      "output_dev"        // 输出设备
#define dsk_ksz_Register_Status "Register_Status"   // 注册状态
#define dsk_ksz_Battery_Status  "Battery_Status"    // 电池状态
#define dsk_ksz_micpod_num      "micpod_num"        // micpod 数量

/**********************************************************************************************//**
// usage: GUI程序运行相关信息：消息段起始消息
// wParam: 0
// lParam: 0
// extra: NULL
**************************************************************************************************/
#define DSK_MSG_APP_INFO_MIN                            DSK_MSG(DSK_MSG_APP, DSK_MSG_INDEX + 0)

/**********************************************************************************************//**
// usage: License证书状态变化
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
// Json消息格式：
// {
//   "expire":"Nov 28 05:49:17 2017 GMT"
//  }
**************************************************************************************************/
#define DSK_MSG_APP_INFO_LICENSE_STATE                  DSK_MSG(DSK_MSG_APP, DSK_MSG_INDEX + 2)

/**********************************************************************************************//**
// usage: GUI程序运行相关信息：自定义事件
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
**************************************************************************************************/
#define DSK_MSG_APP_INFO_DEFAULT                        DSK_MSG(DSK_MSG_APP, DSK_MSG_INDEX + 1022)

/**********************************************************************************************//**
// usage: GUI程序运行相关信息：消息段定义结尾
// wParam: 0
// lParam: 0
// extra: NULL
**************************************************************************************************/
#define DSK_MSG_APP_INFO_MAX                            DSK_MSG(DSK_MSG_APP, DSK_MSG_INDEX + 1023)

/**********************************************************************************************//**
// usage: 通话相关信息：消息段起始消息
// wParam: 0
// lParam: 0
// extra: NULL
**************************************************************************************************/
#define DSK_MSG_TALK_INFO_MIN                           DSK_MSG(DSK_MSG_TALK, DSK_MSG_INDEX + 0)

/**********************************************************************************************//**
// usage: 通话相关信息：通话建立
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)

// Json消息格式：
// {
//   "callId" : 32769,
//   "calltype" : 1,
//   "callmask" : 0,
//   "sessiontype" : 0,
//   "lineId" : 0,
//   "duration" : 0,
//   "time" : 1515490094,
//   "runningtimesec" : 619,
//   "runningtimensec" : 521518600,
//   "fromuri" : "2305@yealink.com",
//   "touri" : "2306@yealink.com"
//  }
**************************************************************************************************/
#define DSKCALLTYPENONE                 0   // 呼叫类型
#define DSKCALLTYPEOUTGO                1   // 呼出
#define DSKCALLTYPEINCOMING             2   // 来电
#define DSKCALLTYPEMISS                 3   // 未接

#define DSKSESSIONTYPECOMMON            0   // 普通通话
#define DSKSESSIONTYPENETWORKCONF       1   // bsft 网络会议
#define DSKSESSIONTYPESFBCONF           2   // sfb 会议
#define DSKSESSIONTYPEYEALINKCONF       3   // yealink会议

#define DSKCALLMASKNONE                  0      // 呼叫标记
#define DSKCALLMASKPCAUDIO              (1)     // 呼叫标记：pc audio
#define DSKCALLMASKE911                 (1<<11) // 呼叫标记：紧急呼叫
#define DSKCALLMASKVOICEMAIL            (1<<12) // 呼叫标记: 语音邮箱

#define DSK_MSG_TALK_INFO_ESTABLISHED                   DSK_MSG(DSK_MSG_TALK, DSK_MSG_INDEX + 1)

/**********************************************************************************************//**
// usage: 通话相关信息：通话结束
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)

// Json消息格式：
// {
//   "callId" : 32769,
//   "calltype" : 1,
//   "lineId" : 0,
//   "duration" : 0,
//   "time" : 1515490094,
//   "runningtimesec" : 619,
//   "runningtimensec" : 521518600,
//   "fromuri" : "2305@yealink.com",
//   "touri" : "2306@yealink.com"
//  }
**************************************************************************************************/
#define DSK_MSG_TALK_INFO_TERMINATED                    DSK_MSG(DSK_MSG_TALK, DSK_MSG_INDEX + 2)

/**********************************************************************************************//**
// usage: 通话相关信息：转接失败
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
// Json消息格式：
// {
//   "callId" : 32769,
//   "calltype" : 1,
//   "lineId" : 0,
//   "duration" : 0,
//   "time" : 1515490094,
//   "runningtimesec" : 619,
//   "runningtimensec" : 521518600,
//   "fromuri" : "2305@yealink.com",
//   "touri" : "2306@yealink.com"
//  }
**************************************************************************************************/
#define DSK_MSG_TALK_INFO_TRAN_FAILED                   DSK_MSG(DSK_MSG_TALK, DSK_MSG_INDEX + 3)

/**********************************************************************************************//**
// usage: 通话相关信息：hold失败
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
// Json消息格式：
// {
//   "callId" : 32769,
//   "calltype" : 1,
//   "lineId" : 0,
//   "duration" : 0,
//   "time" : 1515490094,
//   "runningtimesec" : 619,
//   "runningtimensec" : 521518600,
//   "fromuri" : "2305@yealink.com",
//   "touri" : "2306@yealink.com"
//  }
**************************************************************************************************/
#define DSK_MSG_TALK_INFO_HOLD_FAILED                   DSK_MSG(DSK_MSG_TALK, DSK_MSG_INDEX + 4)

/**********************************************************************************************//**
// usage: 通话相关信息：通话建立失败
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
// Json消息格式：
// {
//   "callId" : 32769,
//   "calltype" : 1,
//   "lineId" : 0,
//   "duration" : 0,
//   "time" : 1515490094,
//   "runningtimesec" : 619,
//   "runningtimensec" : 521518600,
//   "fromuri" : "2305@yealink.com",
//   "touri" : "2306@yealink.com"
//  }
**************************************************************************************************/
#define DSK_MSG_TALK_INFO_ESTABLISH_FAILED              DSK_MSG(DSK_MSG_TALK, DSK_MSG_INDEX + 5)

/**********************************************************************************************//**
// usage: 通话相关信息：会议建立失败
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
// Json消息格式：
// {
//   "callId" : 32769,
//   "calltype" : 1,
//   "lineId" : 0,
//   "duration" : 0,
//   "time" : 1515490094,
//   "runningtimesec" : 619,
//   "runningtimensec" : 521518600,
//   "fromuri" : "2305@yealink.com",
//   "touri" : "2306@yealink.com"
//  }
**************************************************************************************************/
#define DSK_MSG_TALK_INFO_CREATE_CONF_FAILED            DSK_MSG(DSK_MSG_TALK, DSK_MSG_INDEX + 6)

/**********************************************************************************************//**
// usage: 通话相关信息：通话配置相关，一般是gui自定义的一些通话功能配置事件
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
**************************************************************************************************/
#define DSK_MSG_TALK_INFO_CONFIG                        DSK_MSG(DSK_MSG_TALK, DSK_MSG_INDEX + 1021)

/**********************************************************************************************//**
// usage: 通话相关信息：自定义事件
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: dmstring(json)
**************************************************************************************************/
#define DSK_MSG_TALK_INFO_DEFAULT                       DSK_MSG(DSK_MSG_TALK, DSK_MSG_INDEX + 1022)

/**********************************************************************************************//**
// usage: 通话相关信息：消息段结尾
// wParam: 0
// lParam: 0
// extra: NULL
**************************************************************************************************/
#define DSK_MSG_TALK_INFO_MAX                           DSK_MSG(DSK_MSG_TALK, DSK_MSG_INDEX + 1023)

/**********************************************************************************************//**
// usage: 联系人相关信息：消息段起始
// wParam: 0
// lParam: 0
// extra: NULL
**************************************************************************************************/
#define DSK_MSG_CONTACT_INFO_MIN                        DSK_MSG(DSK_MSG_CONTACT, DSK_MSG_INDEX + 0)

/**********************************************************************************************//**
// usage: 通话相关信息：自定义事件
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
**************************************************************************************************/
#define DSK_MSG_CONTACT_INFO_DEFAULT                    DSK_MSG(DSK_MSG_CONTACT, DSK_MSG_INDEX + 1022)

/**********************************************************************************************//**
// usage: 通话相关信息：消息段结尾
// wParam: 0
// lParam: 0
// extra: NULL
**************************************************************************************************/
#define DSK_MSG_CONTACT_INFO_MAX                        DSK_MSG(DSK_MSG_CONTACT, DSK_MSG_INDEX + 1023)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：消息段起始
// wParam: 0
// lParam: 0
// extra: NULL
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_MIN                           DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 0)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：usb录音结束
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
// Json消息格式：
// {
//   "path" : "/tmp/udisk1/Audio Record/20180105-1419-376.wav",
//  }
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_USB_RECORDED                  DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 1)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：usb 插拔
// wParam: Dsk_DiagEventType 事件类型
// lParam:
// extra: string(json)
// Json消息格式：
// {
//   "state": 1
//  }
state值定义：
#define DSKUSBNONE              0
#define DSKUSBINSERT            1
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_USB_STATE                     DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 2)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：usb headset插拔
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0/1 拔/插
// extra: string(json)
// Json消息格式：
// {
//   "state": 1
//  }
state值定义：
#define DSKUSBNONE              0
#define DSKUSBINSERT            1
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_USB_HEADSET_STATE                     DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 3)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：exp 插拔
// wParam: Dsk_DiagEventType 事件类型
// lParam: 1
// extra: string(json)

// Json消息格式：
// {
//   "count": 1
//  }
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_EXP_STATE                     DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 20)


/**********************************************************************************************//**
// usage: 菜单操作相关信息：bluetooth 适配器状态
// wParam: Dsk_DiagEventType 事件类型
// lParam: 1
// extra: string(json)
// Json消息格式：
// {
//   "state":1
//  }
#define DSKBTDOGNONE            0
#define DSKBTDOGINSERT          1
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_BLUETOOTH_DONGLE_STATE                DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 30)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：blue 连接状态
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)

// Json消息格式：
// {
//   "state":1
//  }
#define DSKBTDISCONNECT         0
#define DSKBTCONNECT            1
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_BLUETOOTH_CONNECT_STATE       DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 31)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：wifi dongle状态
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)

// Json消息格式：
// {
//   "state":1
//  }
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_WIFI_DONGLE_STATE         DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 41)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：DND
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
// Json消息格式：
// {
//   "state": 1,
//   "lineId":-1        //-1:phone / 0-x:accountX
//  }
**************************************************************************************************/

#define DSK_MSG_MENU_INFO_DND_STATE                 DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 50)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：FWD
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
// Json消息格式：
// {
//   "state": 1,
//   "lineId":-1,       //-1:phone / 0-x:accountX
//   "target":"8888"
//  }
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_FWD_STATE             DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 51)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：camera 插拔
// wParam: Dsk_DiagEventType 事件类型
// lParam:
// extra: string(json)
// Json消息格式：
// {
//   "name":"camera event",
//   "severity":"Minor",
//   "description":"This info is activated when camera changed.",
//   "module":"UI",
//   "time" : 1515490094，
//   "state": int
//   "id":int
//   "fd":int
//   "name":stirng
//   "ip"：string
//   "mac":string
//   "fireware":string
//   "hardware":string
//  }
**************************************************************************************************/
#define DSK_MSG_MENU_CAMERA_STATE_CHANGE                        DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 200)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：Vcs phone 插拔
// wParam: Dsk_DiagEventType 事件类型
// lParam:
// extra: string(json)
// Json消息格式：
// {
//   "name":"vcs phone",
//   "severity":"Minor",
//   "description":"This info is activated when vcs phone changed.",
//   "module":"UI",
//   "time" : 1515490094，
//   "state": int
//   "ip"：string
//   "mac":string
//   "fireware":string
//   "hardware":string
//  }
**************************************************************************************************/
#define DSK_MSG_MENU_VCS_PHONE_STATE_CHANGE                     DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 400)


/**********************************************************************************************//**
// usage: 菜单操作相关信息：Audio input/output
// wParam: Dsk_DiagEventType 事件类型
// lParam:
// extra: string(json)
// Json消息格式：
// {
//   "name":"vcs phone",
//   "severity":"Minor",
//   "description":"This info is activated when audio input and output device changed.",
//   "module":"UI",
//   "time" : 1515490094，
//   "input_dev":string
//   "output_dev":string
//  }
**************************************************************************************************/
#define DSK_MSG_MENU_AUDIO_INPUT_OUTPUT_STATE_CHANGE                        DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 401)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：dect micpod
// wParam: Dsk_DiagEventType 事件类型
// lParam:
// extra: string(json)
// Json消息格式：
// {
//   "name":"dect micpod",
//   "severity":"Minor",
//   "description":"This info is activated when audio input and output device changed.",
//   "module":"UI",
//   "time" : 1515490094，
//   "Register_Status":int
//   "Battery_Status":int
//   "micpod_num":int
//  }
//  Register_Status 值定义
#define DECT_MIC_RS_UNKONOWN        0
#define DECT_MIC_RS_DEREGISTERED    1       //未注册
#define DECT_MIC_RS_REGISTERING     2       //注册中
#define DECT_MIC_RS_REGISTER_FAILED 3       //注册失败
#define DECT_MIC_RS_REGISTERED      4       //已注册
#define DECT_MIC_RS_REGISTER_CANCEL 5       //取消注册

//  Battery_Status 值定义
#define DECT_MIC_BTS_UNKONOWN       0
#define DECT_MIC_BTS_NORMAL         1       //正常状态
#define DECT_MIC_BTS_LOWER          2       //低电量
#define DECT_MIC_BTS_CHARING        3       //充电状态
#define DECT_MIC_BTS_FULL           4       //充满
**************************************************************************************************/
#define DSK_MSG_MENU_DECT_MICPOD_STATE_CHANGE                       DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 402)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：sfb exchange url
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
// Json消息格式：
// {
//   "state": 0
//  }
// 状态值定义在Dsk_ExchangeInfo
// #define DSK_EXCHANGE_STATE_DISCONNECTED          0
// #define DSK_EXCHANGE_STATE_CONNECTED             1
// #define DSK_EXCHANGE_STATE_FAILED                2
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_EXCHANGE_URL_STATE            DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 800)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：sfb exchange
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
**************************************************************************************************/
//#define DSK_MSG_MENU_INFO_EXCHANGE_CONNECT_STATE      DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 801)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：sfb exchange calendar 同步状态
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
// Json消息格式：
// {
//   "state": 0
//  }
//   状态值定义在Dsk_ExchangeInfo
// #define DSK_EXCHANGE_STATE_DISCONNECTED          0
// #define DSK_EXCHANGE_STATE_CONNECTED             1
// #define DSK_EXCHANGE_STATE_FAILED                2
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_EXCHANGE_CALENDAR_STATE           DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 802)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：sfb exchange call log 同步状态
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
// Json消息格式：
// {
//   "state": 0
//  }
// 状态值定义在Dsk_ExchangeInfo
// #define DSK_EXCHANGE_STATE_DISCONNECTED          0
// #define DSK_EXCHANGE_STATE_CONNECTED             1
// #define DSK_EXCHANGE_STATE_FAILED                2
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_EXCHANGE_CALLLOG_STATE            DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 803)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：sfb exchange VM 同步状态
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
// Json消息格式：
// {
//   "state": 0
//  }
// 状态值定义在Dsk_ExchangeInfo
// #define DSK_EXCHANGE_STATE_DISCONNECTED          0
// #define DSK_EXCHANGE_STATE_CONNECTED             1
// #define DSK_EXCHANGE_STATE_FAILED                2
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_EXCHANGE_VOICEMAIL_STATE          DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 804)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：sfb exchange outlook 联系人同步状态
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
// Json消息格式：
// {
//   "state": 0
//  }
//   状态值定义在Dsk_ExchangeInfo
// #define DSK_EXCHANGE_STATE_DISCONNECTED          0
// #define DSK_EXCHANGE_STATE_CONNECTED             1
// #define DSK_EXCHANGE_STATE_FAILED                2
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_EXCHANGE_OUTLOOK_STATE            DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 805)

/**********************************************************************************************//**
// usage: 菜单操作相关信息：自定义事件
// wParam: Dsk_DiagEventType 事件类型
// lParam: 0
// extra: string(json)
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_DEFAULT                       DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 1022)


/**********************************************************************************************//**
// usage: 菜单操作相关信息：消息段结尾
// wParam: 0
// lParam: 0
// extra: NULL
**************************************************************************************************/
#define DSK_MSG_MENU_INFO_MAX                           DSK_MSG(DSK_MSG_MENU, DSK_MSG_INDEX + 1023)


// //////////////////////数据结构定义////////////////////////////////////////
// report event information
// report event information
typedef enum
{
    DSK_DIAG_EVENT_TYPE_DIAG_COMMON = -1,               // gui custom define
    DSK_DIAG_EVENT_TYPE_DIAG_NONE,

    // 分段给各个功能模块
    // 1 - 999 app/config
    DSK_DIAG_EVENT_TYPE_APP = 1,
    DSK_DIAG_EVENT_TYPE_DEVICE_UP_TIME,                 // Is a measure of time that the device is available without crashing or needing to be
    DSK_DIAG_EVENT_TYPE_DEVICE_REBOOTED,                // Device rebooted

    // 1000 - 1999 talk
    DSK_DIAG_EVENT_TYPE_TALK = 1000,
    DSK_DIAG_EVENT_TYPE_TALK_ESTABLISHED = 1001,                // This info is activated when talk established.
    DSK_DIAG_EVENT_TYPE_TALK_TERMINATED,                // This info is activated when talk terminated.
    DSK_DIAG_EVENT_TYPE_USER_CANCELED_CALL,             // cancel call
    DSK_DIAG_EVENT_TYPE_TRAN_FAILED,                    // Failed transfer attempts
    DSK_DIAG_EVENT_TYPE_FWD_FAILED,                     // Failed call forward attempts
    DSK_DIAG_EVENT_TYPE_HOLD_FAILED,                    // Failed to hold the call
    DSK_DIAG_EVENT_TYPE_RESUME_FAILED,                  // Failed to resume held call
    DSK_DIAG_EVENT_TYPE_DISPLAY_MEETING_ROSTER_FAIL,    // Failed to display meeting roster with participants’ info
    DSK_DIAG_EVENT_TYPE_IN_CALL_NETWORK_OUTAGE,         // Call dropped due to network unavailability
    DSK_DIAG_EVENT_TYPE_DROPPED_CALL,                   // Calls being dropped due to Network connectivity problems, During Handoff for Wifi phones
    DSK_DIAG_EVENT_TYPE_DROPPED_CONF_CALL,              // Conference calls being dropped due to Network connectivity problems, device rebooted mid         DSK_DIAG_EVENT_TYPE_APP_MAX = 1000,

    // 2000 - 2009 contact
    DSK_DIAG_EVENT_TYPE_CONTACT = 2000,
    DSK_DIAG_EVENT_TYPE_AUTO_DISCOVERY_FAILED = 2001,           // Failure to resolve well know SRV records or auto discover services/server
    DSK_DIAG_EVENT_TYPE_EXCHANGE_AUTH_FAILED,           // Failure to authenticate with exchange web services
    DSK_DIAG_EVENT_TYPE_CALENDAR_SYNC_FAILED,           // Failure to sync and retrieve calendar data from Exchange failed
    DSK_DIAG_EVENT_TYPE_VM_SYNC_FAILED,                 // Failure to retrieve and list visual voicemail from Exchange
    DSK_DIAG_EVENT_TYPE_PLAYING_VOICEMAIL_FAILED,       // Failure to play visual voicemail
    DSK_DIAG_EVENT_TYPE_OUTLOOK_CONTACT_SYNC_FAILED,    // This Alarm is activated when failing to retrieve outlook contact.
    DSK_DIAG_EVENT_TYPE_CALLLOG_SYNC_FAILED,            // This Alarm is activated when failing to retrieve calllog.

    // 3000 - 3999 menu
    DSK_DIAG_EVENT_TYPE_MENU = 3000,
    DSK_DIAG_EVENT_TYPE_SIGNED_OUT = 3001,                      // sign out
    DSK_DIAG_EVENT_TYPE_USER_CANCELED_SIGN_IN,          // cancel sign-in
    DSK_DIAG_EVENT_TYPE_BOSS_ADMIN_MODE,                // Whether the device is configured in Boss or Admin mode. iValue(0/1:not configured/configured)
    DSK_DIAG_EVENT_TYPE_DEVICE_UPDATE_POLICY,           // Device Update policy for IP phones is enabled or disabled. iValue(0/1:disabled/enabled)
    DSK_DIAG_EVENT_TYPE_DEVICE_UPDATE_FAILED,           // Update on the device failed. iValue(0/1:failed/success)
    DSK_DIAG_EVENT_TYPE_USB_RECORDED,                   // This info is activated when record ended.

    //CAMERA
    DSK_DIAG_EVENT_TYPE_CAMERA = 4000,
    DSK_DIAG_EVENT_TYPE_PHONE,
    DSK_DIAG_EVENT_TYPE_AUDIO_IO,
    DSK_DIAG_EVENT_TYPE_DECT_MIC_STATE,
    DSK_DIAG_EVENT_TYPE_MAX,

} Dsk_DiagEventType;

//dsk config信息
#define DSKCONFIGINFOSIZE       64
typedef struct
{
    char    szConfigData[DSKCONFIGINFOSIZE];        // 配置路径
    char    szConfigUser[DSKCONFIGINFOSIZE];        // 用户数据路径

    char   reserved[512];                           // 预留
} Dsk_ConfigInfo;


//蓝牙信息
#define DSKBTINFOSIZE       64
typedef struct
{
#define DSKBTDOGNONE            0           // 适配器无
#define DSKBTDOGINSERT          1           // 适配器插入
    char    donglestate;                    // 适配器状态

#define DSKBTDISCONNECT         0
#define DSKBTCONNECT            1
    char    connect;                        // 连接状态

#define DSKBTTYPENONE           0           // 默认值
#define DSKBTTYPEHEADSET        1           // 连接耳机
#define DSKBTTYPEMOBILE         2           // 连接手机:通话功能、联系人功能
    char    type;

    char    szBtDongle[DSKBTINFOSIZE];      // 适配器名称
    char    szBTDongleMac[DSKBTINFOSIZE];   // 适配器地址

    char    szBTDevice[DSKBTINFOSIZE];      // 蓝牙耳机名称
    char    szBTDeviceMac[DSKBTINFOSIZE];   // 蓝牙耳机地址

    char   reserved[125];                   // 预留
} Dsk_BluetoothInfo;

//WIFI信息
#define DSKWIFIINFOSIZE     64
typedef struct
{
#define DSKWIFIDOGNONE          0           // 适配器无
#define DSKWIFIDOGINSERT        1           // 适配器插入
    char    donglestate;                    // 适配器状态

#define DSKWIFIDISCONNECT       0
#define DSKWIFICONNECT          1
    char    connect;                        // 连接状态

    char    szWifiDongleMac[DSKWIFIINFOSIZE];   // 适配器地址

    char    szWifiSSID[DSKWIFIINFOSIZE];    // Wifi名称
    char    szWifiMac[DSKWIFIINFOSIZE];     // Wifi Mac地址

	char	szProfileName[DSKWIFIINFOSIZE];	    //profile name
	int		iSignal;							//信号强度	
	int		iChannel;							//网络所在频段，与iFrequence是对应的	
	char	szSercureMode[DSKWIFIINFOSIZE];		//安全模式
	char	szEncrypt[DSKWIFIINFOSIZE];			//加密方式
	int		iSpeed;								// 网络的连接速度
	int		iFrequence;							//已连接网络的频率
	int		iQos;								//网络的Qos，驱动目前实现不了，留空
    char    reserved[106];						// 预留
} Dsk_WifiInfo;

#define DSKUSBINFOSIZE      64
typedef struct
{
#define DSKUSBNONE              0
#define DSKUSBINSERT            1
    char    state;                          // 插入状态

#define DSKUSBUNKNOWN           0
#define DSKUSBDISK              1
#define DSKUSBBT                2
#define DSKUSBWIFI              3
    char    type;                           // usb类型
    char    reserved[254];                  // 预留
} Dsk_USBInfo;

typedef struct
{
    int    count;                           // 个数

#define DSKEXP20                    20
#define DSKEXP38                    38
#define DSKEXP39                    39
#define DSKEXP40                    40
#define DSKEXP50                    50
    int     type;                           // exp类型

#define DSKEHSNONE      0
#define DSKEHSINSERT    1
    int     ehs;                            // ehs
    char    reserved[116];                  // 预留
} Dsk_EXPInfo;


#define DSKLICENSEINFOSIZE      32
typedef struct
{
#define DSKLICENSENONE              0
#define DSKLICENSEINSTALL           1
    char    state;                          // 证书状态

#define DSKPERMISSIONTYPENONE       0       // 没有许可，需要导入证书
#define DSKPERMISSIONTYPESFB        1       // 永久许可sfb
#define DSKPERMISSIONTYPEVCS        2       // 永久许可vcs

#define DSKPERMISSIONTYPEAMR        6       // amr codec许可

    char    type;                           // 授权类型

    char    szExpireData[DSKLICENSEINFOSIZE];   // 有效期
    char    reserved[479];                  // 预留
} Dsk_LicenseInfo;

typedef struct
{
#define DSK_EXCHANGE_STATE_DISCONNECTED         0
#define DSK_EXCHANGE_STATE_CONNECTED            1
#define DSK_EXCHANGE_STATE_FAILED               2
    char    urlstate;                       // 地址请求状态
    char    calendarstate;                  // 日历请求状态
    char    calllogstate;                   // 历史记录请求状态
    char    voicemailstate;                 // 语音邮件请求状态
    char    outlookstate;                   // outlook联系人请求状态

    char    reserved[123];                  // 预留
} Dsk_ExchangeInfo;

#define DSK_CAMERA_LIST_SIZE        64
typedef struct
{
    char   IDlist[2 * DSK_CAMERA_LIST_SIZE];
    char   nCount;
    char   reserved[127];                   // 预留
} Dsk_CameraIDList;

#define DSK_CAMERA_INFO_SIZE        64
typedef struct
{
#define DSK_CAMERA_STATSU_DISABLE 0
#define DSK_CAMERA_STATSU_ENABLE  1
    char    nStatus;                            // 是否启用摄像机
    char    szName[DSK_CAMERA_INFO_SIZE];       // 摄像头名称
    char    szIP[DSK_CAMERA_INFO_SIZE];         // IP地址信息
    char    szfirmWare[DSK_CAMERA_INFO_SIZE];   // 固件版本
    char    szHardWare[DSK_CAMERA_INFO_SIZE];   // 软件版本
    char    szMac[DSK_CAMERA_INFO_SIZE];        // MAC
    char    reserved[127];                  // 预留
} Dsk_CameraInfo;


#define DSK_AUDIO_IO_DEV_SIZE       128
typedef struct
{
    char szInputDev[DSK_AUDIO_IO_DEV_SIZE];   //输入设备名称
    char szOutputDev[DSK_AUDIO_IO_DEV_SIZE];  //输出设备名称
    char reserved[DSK_AUDIO_IO_DEV_SIZE];
} Dsk_Audio_IO_dev;


#define DSK_VCS_PHONE_INFO_SIZE     64
typedef struct
{
    char    szMac[DSK_VCS_PHONE_INFO_SIZE];         // 摄像头名称
    char    szIP[DSK_VCS_PHONE_INFO_SIZE];          // IP地址信息
    char    szModel[DSK_VCS_PHONE_INFO_SIZE * 2];   // 设备型号
    char    szFirmWare[DSK_VCS_PHONE_INFO_SIZE];    // 固件版本
    char    szHardWare[DSK_VCS_PHONE_INFO_SIZE];    // 软件版本
    char   reserved[128];                   // 预留
} Dsk_VcsphoneInfo;


typedef struct
{
    char    nCount;                                 //连接个数
    char    nDongleConnect;                         //dect dongle 是否连接
    char   reserved[126];                           // 预留
} Dsk_DectMicpodInfo;


#define  DSK_VCS_PHONE_IP_SIZE  64
typedef struct
{
    char    szIP[DSK_VCS_PHONE_IP_SIZE];
    char    status;//0: disable 1:enable
    char   reserved[128];
} Dsk_VcsPhoneIPinfo;
// //////////////////////接口定义////////////////////////////////////////
// 接口提供超时设置：query接口建议默认DSK_TIME_OUT_DEFAULT，cmd接口建议设置为DSK_TIME_OUT_NONE
// cmd类型接口设置为超时时，返回值需要与UI确认，一般返回值无意义
#define DSK_TIME_OUT_NONE           -1                  // 接口调用超时时间：post方式，不等待
#define DSK_TIME_OUT_WAIT           0                   // 接口调用超时时间：一直等待
#define DSK_TIME_OUT_DEFAULT        2000                // 接口调用超时时间：默认2000(ms)

/**********************************************************************************************//**
 * @fn  int dsk_request_notify(mkit_handle_t caller, int event);
 *
 * @brief   request notify to dsk service
 *
 * @param   caller  The caller.
 * @param   event   The event.  0/message id : all/message section
 *
 * @return  An int.1 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_request_notify(mkit_handle_t caller, int event);

/**********************************************************************************************//**
 * @fn  int dsk_cancel_notify(mkit_handle_t caller, int event);
 *
 * @brief   cancel notify to dsk service
 *
 * @param   caller  The caller.
 * @param   event   The event.
 *
 * @return  An int.1 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cancel_notify(mkit_handle_t caller, int event);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_phone(mkit_handle_t caller, char* data, int size, int timeout);
 *
 * @brief   Dsk query information phone.
 * @param           caller  The caller.
 * @param [in,out]  data    If non-null, the data. phone info command
 * @param           size    The size.
 * @param           timeout The time out.(ms)
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_phone_cfg(mkit_handle_t caller, char* data, int size, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_phone_status(mkit_handle_t caller, int type, int flag, int timeout);
 *
 * @brief   Dsk query information phone status.
 * @param   caller  The caller.
 * @param   type    The type.
 * @param   flag    The flag.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_phone_status(mkit_handle_t caller, int type, int flag, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_phone_active_status(mkit_handle_t caller, int type, int flag, int timeout);
 *
 * @brief   Dsk query information phone active status.
 * @param   caller  The caller.
 * @param   type    The type.
 * @param   flag    The flag.
 *
 * @return  An int. 0/1  idle/talking(talking&backlight inactive expire)
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_phone_active_status(mkit_handle_t caller, int type, int flag);
DSK_API_DECLARE int dsk_query_info_phone_active_status2(mkit_handle_t caller, int type, int flag, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_rtp_status(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout);
 *
 * @brief   Dsk query information rtp status.
 * @param           caller  The caller.
 * @param           type    The type.
 * @param           flag    The flag.
 * @param [in,out]  data    If non-null, the data.
 * @param           size    The size.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_rtp_status(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_account_count(mkit_handle_t caller, int type, int timeout);
 *
 * @brief   Dsk query account count.
 * @param   caller  The caller.
 * @param   type    The type.
 *
 * @return  An int. max count
 **************************************************************************************************/
#define DSK_ACCOUNT_PROTOCOL_SIP            0
#define DSK_ACCOUNT_PROTOCOL_H323           1
DSK_API_DECLARE int dsk_query_info_account_count(mkit_handle_t caller, int type);
DSK_API_DECLARE int dsk_query_info_account_count2(mkit_handle_t caller, int type, int timeout);

/**********************************************************************************************//**
 * @fn  int dsk_query_info_account_state(mkit_handle_t caller, int account_id, int type, int timeout);
 *
 * @brief   Dsk query information account state.
 * @param   caller      The caller.
 * @param   account_id  Identifier for the account.
 * @param   type        The type. 0/1 line status/server id
 *
 * @return  An int. account state
 **************************************************************************************************/
#define DSK_ACCOUNT_STATE_UNKNOW            -1      // 账号注册状态
#define DSK_ACCOUNT_STATE_DISABLE           0
#define DSK_ACCOUNT_STATE_REGISTERING       1
#define DSK_ACCOUNT_STATE_REGISTERED        2
#define DSK_ACCOUNT_STATE_REGISTER_FAIL     3
#define DSK_ACCOUNT_STATE_BE_UNREGISTERED   4
#define DSK_ACCOUNT_STATE_DEFAULT           5

#define DSK_ACCOUNT_STATE_TYPE_REGISTER     0       // 查询注册状态
#define DSK_ACCOUNT_STATE_TYPE_SERVERID     1       // 查询注册的服务器序号，仅供web使用
#define DSK_ACCOUNT_STATE_TYPE_FAILEDID     2       // 查询注册失败code，仅供web使用
DSK_API_DECLARE int dsk_query_info_account_state(mkit_handle_t caller, int account_id, int type, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_pstn_account_state(mkit_handle_t caller, int account_id, int type, int timeout);
 *
 * @brief   Dsk query information pstn account state.
 * @param   caller      The caller.
 * @param   account_id  Identifier for the account.
 * @param   type        The type.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_pstn_account_state(mkit_handle_t caller, int account_id, int type, int timeout);

/**********************************************************************************************//**
 * @fn  int dsk_query_info_talk_exist(mkit_handle_t caller, int timeout);
 *
 * @brief   Dsk query information talk exist.
 * @param   caller  The caller.
 *
 * @return  An int. 0 / 1  idle/session exist
 **************************************************************************************************/
#define DSK_STATE_IDLE          0       // DSK idle
#define DSK_STATE_TAKING        1       // DSK通话中
DSK_API_DECLARE int dsk_query_info_talk_exist(mkit_handle_t caller);
DSK_API_DECLARE int dsk_query_info_talk_exist2(mkit_handle_t caller, int timeout);

/**********************************************************************************************//**
 * @fn  int dsk_query_info_talk_state(mkit_handle_t caller, int call_id, int timeout);
 *
 * @brief   query information talk state.
 *
 * @param   caller  The caller.
 * @param   call_id Identifier for the call.
 *
 * @return  An int. talk state. 0/1  not find/talking
 **************************************************************************************************/
#define DSK_TALK_STATE_NONE         0       // 通话不存在
#define DSK_TALK_STATE_TAKING       1       // 通话中
DSK_API_DECLARE int dsk_query_info_talk_state(mkit_handle_t caller, int call_id);
DSK_API_DECLARE int dsk_query_info_talk_state2(mkit_handle_t caller, int call_id, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_bluetooth_info(mkit_handle_t caller, Dsk_BluetoothInfo* info, int timeout);
 *
 * @brief   Dsk query bluetooth information.
 * @param           caller  The caller.
 * @param [in,out]  info    If non-null, the information.
 *
 * @return  An int.1 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_bluetooth_info(mkit_handle_t caller, Dsk_BluetoothInfo* info);
DSK_API_DECLARE int dsk_query_info_bluetooth_info2(mkit_handle_t caller, Dsk_BluetoothInfo* info, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_wifi_info(mkit_handle_t caller, Dsk_WifiInfo* info, int timeout);
 *
 * @brief   Dsk query information WiFi information.
 * @param           caller  The caller.
 * @param [in,out]  info    If non-null, the information.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_wifi_info(mkit_handle_t caller, Dsk_WifiInfo* info);
DSK_API_DECLARE int dsk_query_info_wifi_info2(mkit_handle_t caller, Dsk_WifiInfo* info, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_usb_info(mkit_handle_t caller, Dsk_USBInfo* info, int timeout);
 *
 * @brief   Dsk query USB information.
 * @param           caller  The caller.
 * @param [in,out]  info    If non-null, the information.
 *
 * @return  An int.1 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_usb_info(mkit_handle_t caller, Dsk_USBInfo* info);
DSK_API_DECLARE int dsk_query_info_usb_info2(mkit_handle_t caller, Dsk_USBInfo* info, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_exp_info(mkit_handle_t caller, Dsk_EXPInfo* info, int timeout);
 *
 * @brief   Dsk query exp information.
 * @param           caller  The caller.
 * @param [in,out]  info    If non-null, the information.
 *
 * @return  An int.1 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_exp_info(mkit_handle_t caller, Dsk_EXPInfo* info);
DSK_API_DECLARE int dsk_query_info_exp_info2(mkit_handle_t caller, Dsk_EXPInfo* info, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_license_info(mkit_handle_t caller, Dsk_LicenseInfo* info, int timeout);
 *
 * @brief   Dsk query license information.
 * @param           caller  The caller.
 * @param [in,out]  info    If non-null, the information.
 *
 * @return  An int.1 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_license_info(mkit_handle_t caller, Dsk_LicenseInfo* info);
DSK_API_DECLARE int dsk_query_info_license_info2(mkit_handle_t caller, Dsk_LicenseInfo* info, int timeout);

/**********************************************************************************************//**
 * @fn  int dsk_query_info_sfb_presence(mkit_handle_t caller, int account_id, int timeout);
 *
 * @brief   Dsk query information sfb presence.
 * @param   caller      The caller.
 * @param   account_id  Identifier for the account.
 *
 * @return  An int. sfb presence
 **************************************************************************************************/
#define DSK_SFB_PRESENCE_STATE_NONE         0
#define DSK_SFB_PRESENCE_STATE_AVAILABLE    8
#define DSK_SFB_PRESENCE_STATE_BUSY         9
#define DSK_SFB_PRESENCE_STATE_DND          10
#define DSK_SFB_PRESENCE_STATE_BEBACK       11
#define DSK_SFB_PRESENCE_STATE_OFFWORK      12
#define DSK_SFB_PRESENCE_STATE_AWAY         13
#define DSK_SFB_PRESENCE_STATE_OFFLINE      14
DSK_API_DECLARE int dsk_query_info_sfb_presence(mkit_handle_t caller, int account_id);
DSK_API_DECLARE int dsk_query_info_sfb_presence2(mkit_handle_t caller, int account_id, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_sfb_btoe_state(mkit_handle_t caller, int timeout);
 *
 * @brief   Dsk query sfb btoe state.
 * @param   caller  The caller.
 *
 * @return  An int. BTOE state
 **************************************************************************************************/
#define DSK_SFB_BTOE_STATE_UNPAIRED     0
#define DSK_SFB_BTOE_STATE_PAIRED       1
#define DSK_SFB_BTOE_STATE_SIGNIN       2
DSK_API_DECLARE int dsk_query_info_sfb_btoe_state(mkit_handle_t caller);
DSK_API_DECLARE int dsk_query_info_sfb_btoe_state2(mkit_handle_t caller, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_sfb_cap_state(mkit_handle_t caller, int timeout);
 *
 * @brief   Dsk query information sfb capability state.
 * @param   caller  The caller.
 *
 * @return  An int. CAP switch
 **************************************************************************************************/
#define DSK_SFB_CAP_STATE_DISABLE           0
#define DSK_SFB_CAP_STATE_ENABLE            1
DSK_API_DECLARE int dsk_query_info_sfb_cap_state(mkit_handle_t caller);
DSK_API_DECLARE int dsk_query_info_sfb_cap_state2(mkit_handle_t caller, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_contact_update_state(mkit_handle_t caller, int type, int flag, int timeout);
 *
 * @brief   Dsk query information contact update state.
 * @param   caller  The caller.
 * @param   type    The type.
 * @param   flag    The flag.
 *
 * @return  An int. 0:nochange / 1:update contacts/ 2:update group / 3:update both
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_contact_update_state(mkit_handle_t caller, int type, int flag, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_contact_count(mkit_handle_t caller, int type, int group, int timeout);
 *
 * @brief   Dsk query information contact count.
 * @param   caller  The caller.
 * @param   type    The type.
 * @param   group   The group.
 *
 * @return  An int. lParam=0, return contact count
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_contact_count(mkit_handle_t caller, int type, int group, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_sfb_exchange_state(mkit_handle_t caller, int timeout);
 *
 * @brief   Dsk query information sfb exchange state.
 * @param   caller  The caller.
 *
 * @return  An int. 1 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_sfb_exchange_state(mkit_handle_t caller, Dsk_ExchangeInfo* info);
DSK_API_DECLARE int dsk_query_info_sfb_exchange_state2(mkit_handle_t caller, Dsk_ExchangeInfo* info, int timeout);

//////////////////////////////////////cmd section////////////////////////////////////////////////

/**********************************************************************************************//**
* @fn   DSK_API_DECLARE int dsk_cmd_change_phone_cfg(mkit_handle_t caller, char* data, int size, int timeout);
*
* @brief    Dsk command change phone configuration.
* @param            caller  The caller.
* @param [in,out]   data    If non-null, the data. phone cfg
* @param            size    The size.
*
* @return   An int. 1 is success,otherwise failed
**************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_change_phone_cfg(mkit_handle_t caller, char* data, int size, int timeout);

/**********************************************************************************************//**
 * @fn  int dsk_cmd_show_message(mkit_handle_t caller, char* data, int size);
 *
 * @brief   Dsk command show message.
 * @param           caller  The caller.
 * @param [in,out]  data    If non-null, the data.
 * @param           size    The size.
 *
 * @return  An int. 1 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_show_message(mkit_handle_t caller, char* data, int size, int time);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_cmd_talk_hang_up(mkit_handle_t caller, int call_id);
 *
 * @brief   Dsk command hang up call.
 * @param   caller  The caller.
 * @param   call_id Identifier for the call. 0/1/36xxx  focus talk/all talk/call_id talk
 *
 * @return  An int. 1 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_talk_hang_up(mkit_handle_t caller, int call_id, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_cmd_talk_call_out(mkit_handle_t caller, int account_id, int mask, char* data, int size);
 *
 * @brief   Dsk command talk call out.
 * @param           caller      The caller.
 * @param           account_id  Identifier for the account.
 * @param           mask        The mask.
 * @param [in,out]  data        If non-null, the data.
 * @param           size        The size.
 *
 * @return  An int. 1 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_talk_call_out(mkit_handle_t caller, int account_id, int mask, char* data, int size, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_cmd_talk_redirect_target(mkit_handle_t caller, char* data, int size);
 *
 * @brief   Dsk command talk redirect target.
 * @param           caller  The caller.
 * @param [in,out]  data    If non-null, the data. redirect command
 * @param           size    The size.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_talk_redirect_target(mkit_handle_t caller, char* data, int size, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_cmd_action_url(mkit_handle_t caller, char* data, int size);
 *
 * @brief   Dsk command action URL.
 * @param           caller  The caller.
 * @param [in,out]  data    If non-null, the data.
 * @param           size    The size.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_action_url(mkit_handle_t caller, char* data, int size, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_cmd_action_url_confirm(mkit_handle_t caller, int type, int flag, char* data, int size);
 *
 * @brief   Dsk command action URL confirm.
 * @param           caller  The caller.
 * @param           type    The type.
 * @param           flag    The flag.
 * @param [in,out]  data    If non-null, the data.
 * @param           size    The size.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_action_url_confirm(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_cmd_csta(mkit_handle_t caller, char* data, int size);
 *
 * @brief   Dsk command csta.
 * @param           caller  The caller.
 * @param [in,out]  data    If non-null, the data.
 * @param           size    The size.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_csta(mkit_handle_t caller, char* data, int size, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_cmd_push_xml(mkit_handle_t caller, char* data, int size);
 *
 * @brief   Dsk command push XML.
 * @param           caller  The caller.
 * @param [in,out]  data    If non-null, the data.
 * @param           size    The size.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_push_xml(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_cmd_request_xml(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout);
 *
 * @brief   Dsk command request XML.
 * @param           caller  The caller.
 * @param           type    The type.
 * @param           flag    The flag.
 * @param [in,out]  data    If non-null, the data.
 * @param           size    The size.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_request_xml(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_cmd_show_maintenance(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout);
 *
 * @brief   Dsk command show maintenance.
 * @param           caller  The caller.
 * @param           type    The type.
 * @param           flag    The flag.
 * @param [in,out]  data    If non-null, the data.
 * @param           size    The size.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_show_maintenance(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_cmd_save_calllog(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout);
 *
 * @brief   Dsk command save calllog.
 * @param           caller  The caller.
 * @param           type    The type.
 * @param           flag    The flag.
 * @param [in,out]  data    If non-null, the data.
 * @param           size    The size.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_save_calllog(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_cmd_delete_calllog(mkit_handle_t caller, int group, int type, char* data, int size, int timeout);
 *
 * @brief   Dsk command delete calllog.
 * @param           caller  The caller.
 * @param           group   The group.
 * @param           type    The type. 0:local call log
 * @param [in,out]  data    If non-null, the data.
 * @param           size    The size.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_delete_calllog(mkit_handle_t caller, int group, int type, char* data, int size, int timeout);


/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_cmd_set_lcd_contrast(mkit_handle_t caller, int level, int type, char* data, int size, int timeout);
 *
 * @brief   Dsk command set LCD contrast.
 * @param           caller  The caller.
 * @param           level   The level.
 * @param           type    The type.
 * @param [in,out]  data    If non-null, the data.
 * @param           size    The size.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_set_lcd_contrast(mkit_handle_t caller, int level, int type, char* data, int size, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_cmd_set_light_level(mkit_handle_t caller, int level, int type, char* data, int size, int timeout);
 *
 * @brief   Dsk command set light level.
 * @param           caller  The caller.
 * @param           level   The level.
 * @param           type    The type.
 * @param [in,out]  data    If non-null, the data.
 * @param           size    The size.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_set_light_level(mkit_handle_t caller, int level, int type, char* data, int size, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_cmd_reset_factory(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout);
 *
 * @brief   Dsk command reset factory.
 * @param           caller  The caller.
 * @param           type    The type.
 * @param           flag    The flag.
 * @param [in,out]  data    If non-null, the data.
 * @param           size    The size.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_reset_factory(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_cmd_reset_factory(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout);
 *
 * @brief   Dsk command reset factory.
 * @param           caller  The caller.
 * @param           type    The type.
 * @param           flag    The flag.
 * @param [in,out]  data    If non-null, the data.
 * @param           size    The size.
 *
 * @return  An int.
 **************************************************************************************************/
DSK_API_DECLARE int dsk_cmd_do_edk(mkit_handle_t caller, int type, int flag, char* data, int size, int timeout);


/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_camera_list(mkit_handle_t caller, Dsk_CameraIDList* Idlist)
 *
 * @brief   Dsk query camera id list.
 * @param           caller  The caller.
 * @param [in,out]  caller  Dsk_CameraIDList    If non-null, the information.
 *
 * @return  An int.1 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_camera_list(mkit_handle_t caller, Dsk_CameraIDList* Idlist);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_camera_info(mkit_handle_t caller, Dsk_CameraInfo* info);
 *
 * @brief   Dsk query camera information.
 * @param           caller  The caller.
 * @param [in,out]  info    If non-null, the information.
 *
 * @return  An int.1 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_camera_info(mkit_handle_t caller, int cameraId, Dsk_CameraInfo* info);


/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_audio_input_output_dev_info(mkit_handle_t caller, Dsk_Audio_IO_dev* info);
 *
 * @brief   Dsk query audio information.
 * @param           caller  The caller.
 * @param [in,out]  info    If non-null, the information.
 *
 * @return  An int.1 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_audio_input_output_dev_info(mkit_handle_t caller, Dsk_Audio_IO_dev* info);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_vcs_phone_dev_info(mkit_handle_t caller, Dsk_VcsphoneInfo* info);
 *
 * @brief   Dsk query vcs phone information.
 * @param           caller  The caller.
 * @param [in,out]  info    If non-null, the information.
 *
 * @return  An int.1 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_vcs_phone_dev_info(mkit_handle_t caller, Dsk_VcsphoneInfo* info);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_query_info_dect_micpod_dev_info(mkit_handle_t caller, Dsk_DectMicpodInfo* info);
 *
 * @brief   Dsk query dect micpod information.
 * @param           caller  The caller.
 * @param [in,out]  info    If non-null, the information.
 *
 * @return  An int.1 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_query_info_dect_micpod_dev_info(mkit_handle_t caller, Dsk_DectMicpodInfo* info);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_notify_vcp_phone_upgrade_info(mkit_handle_t caller, int status, int type)
 *
 * @brief   Dsk notify vcp phone upgrade info (web/autop notify gui update )
 * @param           caller  The caller.
 * @param [in,in]
    status: 0:Start Kill 1:Start Upgrade
    char data :ext rom path
    type: 0:Autop, 1:Web
 * @return  An int >= 0 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_notify_vcp_phone_upgrade_info(mkit_handle_t caller, int status, int type, char* data, int size);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_notify_check_vcp_phone_header_info(mkit_handle_t caller, char* data, int size)
 *
 * @brief   autop notify gui to check vcp fireware (autop)
 * @param           caller  The caller.
 * @param [in,in]
    data: path of header file
    size: data of size
 * @return  An int >= 0 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_notify_check_vcp_phone_header_info(mkit_handle_t caller, char* data, int size);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_notify_get_vcp_phone_ip_addr_info(mkit_handle_t caller, Dsk_VcsPhoneIPinfo* info)
 *
 * @brief   notify get gui to  vcp ip addr
 * @param           caller  The caller.
 * @param [out]
 * @return  An int >= 0 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_notify_get_vcp_phone_ip_addr_info(mkit_handle_t caller, Dsk_VcsPhoneIPinfo* info);

/**********************************************************************************************//**
 * @fn  DSK_API_DECLARE int dsk_notify_get_vcp_phone_check_result(mkit_handle_t caller)
 *
 * @brief   autop notify gui to return vcp fireware check result
 * @param           caller  The caller.
 * @param [void]
 * @return  An int >= 0 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_notify_get_vcp_phone_check_result(mkit_handle_t caller);

/**********************************************************************************************//**
* @fn DSK_API_DECLARE int dsk_notify_vcc22_upgrade(mkit_handle_t caller, int status, int type)
*
* @brief    web/autop notify gui update vcc22
* @param            caller  The caller.
* @param [in,in]
status: 0:Start Kill 1:Start Upgrade
type: 0:Autop, 1:Web
* @return   An int >= 0 is success,otherwise failed
 **************************************************************************************************/
DSK_API_DECLARE int dsk_notify_vcc22_upgrade(mkit_handle_t caller, int status, int type, char* data, int size);
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // _DSK_INTERFACE_DEFINE_H_
