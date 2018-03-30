#ifndef __EVENT_DEFINE_H__
#define __EVENT_DEFINE_H__

#include "ylstring.h"
#include "ylvector.h"
#include "xmlparser/xmlparser.hpp"
#include "commonapi/stringhelperapi.h"
#include <service_dms.h>

typedef dms_alert_t DiagAlertInfo;

// 事件数据类型
enum EventDataType
{
    EDT_NONE = 0,           //无数据
    EDT_BASE = 1,           //基础数据
    EDT_TALK = 2,           //通话数据
    EDT_REMOTE_INFO = 3,    //对方信息数据
    EDT_FWD  = 4,           //Forward数据
    EDT_PHONE  = 5,         //话机数据
    EDT_Connected_Device = 6        //外围设备
};

// 事件模块类型
enum EventModuleType
{
    EMT_NONE = 0,           //None
    EMT_ACTION_URL = 0x01,  //Action URL
    EMT_CSTA = 0x01 << 1,   //CSTA
    EMT_CTRIP = 0x01 << 2,  //Ctrip
    EMT_DIAG = 0x01 << 3,   //Diag
    EMT_E911 = 0x01 << 4,   //E911
    EMT_TALK_INFO_REPORT = 0x01 << 5,   //通话信息上报管理平台
    EMT_ALL = 0xff,         //所有
};

// EventType类型.
enum EventType
{
    EVENT_TYPE_NONE = 0,                // 不正确的Url类型.
    EVENT_SETUP_COMPLETED = 1,          // 话机启动完成, 可以开始使用的时候.
    EVENT_LOGIN_ON = 2,                 // 帐号注册成功.
    EVENT_LOGIN_OFF = 3,                // 账号注销.
    EVENT_REGISTER_FAILED = 4,          // 账号注册失败.

    // 通话部分.
    EVENT_OFFHOOK = 5,                  // 摘机.
    EVENT_ONHOOK = 6,                   // 挂机.
    EVENT_INCOMING_CALL = 7,            // 话机来电.
    EVENT_REJECT_INCOMING_CALL = 8,     // 话机拒绝来电.
    EVENT_FORWARD_INCOMING_CALL = 9,    // 话机转接来电.
    EVENT_CALL_REMOTE_CANCELED = 10,    // 来电时, 对方取消.
    EVENT_ANSWER_NEW_IN_CALL = 11,      // 接起新来电.
    EVENT_REJECT_NEW_IN_CALL = 12,      // 拒绝新来电.
    EVENT_CALLOUT = 13,                 // 话机拨出.
    EVENT_CANCEL_CALLOUT = 14,          // 话机呼出时按cancel键.
    EVENT_REMOTE_BUSY = 15,             // 话机呼出时被对方拒接.
    EVENT_ESTABLISHED = 16,             // 通话建立.
    EVENT_TERMINATED = 17,              // 通话挂断.

    // 话机状态开关.
    EVENT_OPEN_DND = 18,                // 话机开启DND.
    EVENT_CLOSE_DND = 19,               // 话机关闭DND.
    EVENT_OPEN_ALWAYS_FORWARD = 20,     // 话机开启Alwasy Forward.
    EVENT_CLOSE_ALWAYS_FORWARD = 21,    // 话机关闭Always Forward.
    EVENT_OPEN_BUSY_FORWARD = 22,       // 话机开启Busy Forward.
    EVENT_CLOSE_BUSY_FORWARD = 23,      // 话机关闭Busy Forward.
    EVENT_OPEN_NO_ANSWER_FORWARD = 24,  // 话机开启No Answer Forward.
    EVENT_CLOSE_NO_ANSWER_FORWARD = 25, // 话机关闭No Answer Forward.

    // Transfer部分.
    EVENT_TRANSFER_CALL = 26,           // 转接
    EVENT_BLIND_TRANSFER = 27,          // 盲转
    EVENT_ATTENDED_TRANSFER = 28,       // 咨询转
    EVENT_TRANSFER_FINISHED = 29,       // 转接成功
    EVENT_TRANSFER_FAILED = 30,         // 转接失败
    // Hold部分.
    EVENT_HOLD = 31,                    // 话机执行Hold操作.
    EVENT_UNHOLD = 32,                  // 话机执行解Hold操作.
    // Mute部分.
    EVENT_MUTE = 33,                    // 话机执行Mute操作.
    EVENT_UNMUTE = 34,                  // 话机执行Unmute操作.
    // 其他.
    EVENT_MISSED_CALL = 35,             // 话机收到新未接来电
    EVENT_BUSY_TO_IDLE = 36,            // 话机从通话状态回到IDLE界面.
    EVENT_IDLE_TO_BUSY = 37,            // 话机从IDLE界面进入通话状态.
    EVENT_IP_CHANGED = 38,              // 话机IP变化.
    EVENT_REMOTE_FORWARD = 39,          // 对方转接
    EVENT_REMOTE_HOLD = 40,             // 对方Hold
    EVENT_REMOTE_UNHOLD = 41,           // 对方UnHold
    EVENT_AUTOP_FINISH = 42,            // 通知服务器autop完成

    EVENT_HANDFREE = 43,                // 免提
    EVENT_HEADSET = 44,                 // 耳麦
    EVENT_OPEN_CALLWAIT = 45,           // 开启callwaiting
    EVENT_CLOSE_CALLWAIT = 46,          // 关闭callwaiting

    // CSTA Event
    EVENT_ENTER_DIAL = 47,              // 进入拨号界面
    EVENT_END_DIAL = 48,                // 结束拨号  CSTA用作结束通话
    EVENT_RINGBACK = 49,                // 回铃
    EVENT_CONFERENCE = 50,              // 三方会议，有额外接口
    EVENT_DTMF = 51,                    // DTMF
    EVENT_CALL_FAIL = 52,               // 呼叫失败
    EVENT_BLIND_TRANSFER_FAILED = 53,       // 盲转失败
    EVENT_ATTENDED_TRANSFER_FAILED = 54,    // 咨询转失败
    EVENT_CALL_REMOTE_NO_ANSWER = 55,   // 对方无回应

    EVENT_VIEW_MISSCALL = 56,     //查看Missed  Calls
    EVENT_VIEW_FORWARD = 57,    //查看Forwarded Calls
    EVENT_CALL_BY_REPLACED = 58,        //通话被Replace
    EVENT_CALL_INFO_UPDATE = 59,        //通话显示信息更新
    EVENT_CALL_TRANSFERED_BY_REMOTE = 60, //通话被转接，主要针对被转接方发送
    EVENT_PERIPHERAL_INFORMATION = 61, //话机连接设备个数信息(EXP，EHS，USB....)

    EVENT_INCOMING_CALLS_RECEIVED = 62, //收到sip incoming call消息上报的事件
    EVENT_OUTGOING_CALLS_ATTEMPTED = 63, //尝试呼出，包括通过拨号，dsskey，sca类型等

    // autop
    EVENT_AUTOP_START,
    EVENT_AUTOP_STATUS,
    EVENT_AUTOP_END,

    EVENT_AUTOP_ZERO_TOUCH_START,
    EVENT_AUTOP_ZERO_TOUCH_STATUS,
    EVENT_AUTOP_ZERO_TOUCH_END,

    EVENT_AUTOP_RPS_START,
    EVENT_AUTOP_RPS_STATUS,
    EVENT_AUTOP_RPS_END,

    // page
    EVENT_ENTER_PAGE,
    EVENT_EXIT_PAGE,

    // contact
    //EVENT_CONTACT_NUMBER,
    EVENT_CONTACT_SEARCH,
    EVENT_CONTACT_MODIFY,

    // hotplug
    EVENT_HOTPLUG_ADD,
    EVENT_HOTPLUG_REMOVE,

    EVENT_HOTPLUG_USB_RECORDED, // 录音结束，诊断平台控制是否上传服务器

    EVENT_MEDIA_VOICE_STOP,         // 声音停止

    EVENT_SUPER_SEARCH,

    EVENT_TYPE_MAX
};

// 基本事件数据
class EventDataBase
{
public:
    EventDataBase()
    {
        m_eEventType = EVENT_TYPE_NONE;
        m_eEventDataType = EDT_BASE;
        m_wParam = -1;
        m_lParam = -1;
        m_tParam = -1;
    }

    // 转换成上报的数据
    virtual bool TransferReportData(DiagAlertInfo & tEventInfo);

    // 是否上报
    int MapReportType();

    // 是否保存统计
    bool IsSaveStatsInfo();

    EventType m_eEventType;             // 事件类型
    EventDataType m_eEventDataType;     // 事件数据类型
    int m_wParam;                           // w参数
    int m_lParam;                           // l参数
    int m_tParam;							// t参数
    yl::string  m_strExt;               // 扩展数据
};

// 通话事件数据
class TalkEventData : public EventDataBase
{
public:
    TalkEventData()
    {
        m_eEventDataType = EDT_TALK;
        m_vecCallID.clear();
    }

    // 转换成上报的数据
    virtual bool TransferReportData(DiagAlertInfo & tEventInfo);

    YLVector<int>   m_vecCallID;        // 通话CallID

    time_t          m_tTime;            // 通话开始或者结束时间
    int             m_nCallType;        // 通话类型（来电、去电、Forward）

    //int               m_nTalkType;        // 会话类型（普通通话、会议、Call Park）
    int             m_nDuration;        // 通话时长
};

// 通话对方信息数据,目前就拒接来电、未接来电事件用到,未建立Session没有对应CallInfo
class RemoteInfoData : public EventDataBase
{
public:
    RemoteInfoData()
    {
        m_eEventDataType = EDT_REMOTE_INFO;
        m_strCallID = "";
        m_strCallDir = "";
        m_strDisplayName = "";
        m_strSIPName = "";
        m_strServerName = "";
    }

    yl::string m_strCallID;         // 通话CallID
    yl::string m_strCallDir;        // 通话方向
    yl::string m_strDisplayName;    // 对方显示名
    yl::string m_strSIPName;        // 对方用户名
    yl::string m_strServerName;     // 对方服务器名
};

// Forward事件数据
class FWDEventData : public EventDataBase
{
public:
    FWDEventData()
    {
        m_eEventDataType = EDT_FWD;
        m_strFwdNum = "";
        m_strFwdNoAnswerTime = "";
    }

    yl::string m_strFwdNum;     // forward目标号码
    yl::string m_strFwdNoAnswerTime;// 无应答转移的时间
};

// 话机信息数据
class PhoneInfoEventData : public EventDataBase
{
public:
    PhoneInfoEventData()
    {
        m_eEventDataType = EDT_PHONE;
        m_strMAC = "";
        m_strPhoneType = "";
        m_strFirmware = "";
        m_strIp = "";
        m_strIpV4 = "";
        m_strIpV6 = "";
    }

    yl::string m_strMAC;        // MAC地址
    yl::string m_strPhoneType;  // 话机类型
    yl::string m_strFirmware;   // 固件号
    yl::string m_strIp;         // 话机IP
    yl::string m_strIpV4;       // IPV4
    yl::string m_strIpV6;       // IPV6
};

// 外围设备信息数据
class ConnectedDeviceEventData : public EventDataBase
{
public:
    ConnectedDeviceEventData()
    {
        m_eEventDataType = EDT_Connected_Device;
        m_strEXPNum = "";
        m_strUSBNum = "";
        m_strEHSNum = "";
        m_strWifiDongleNum = "";
        m_strBlueToothDongleNum = "";
        m_strUSBHeadSet = "";
    }

    yl::string m_strEXPNum;         // exp数量
    yl::string m_strUSBNum;         // usb是否连接上
    yl::string m_strEHSNum;         // ehs数量
    yl::string m_strWifiDongleNum;  // wifi是否可连接上
    yl::string m_strBlueToothDongleNum; //bluetooth是否连接上
    yl::string m_strUSBHeadSet;         // usb耳机是否连接上
};

//发送结果
enum SEND_RESULT
{
    SR_UNKNOW = -1,                  // 未知错误
    SR_SUCCESS,                      // 0 成功
    SR_NETWORK_UNAVAILABLE,          // 1 网络错误
    SR_DATA_EMPTY,                   // 2 没有数据
    SR_UNKNOW_EVENT,                 // 3 未知事件
    SR_NO_MODULE_PROCESS,            // 4 没有模块处理
};

/**********************************************************************************************//**
  保存基础统计数据:外部模块如果要扩展数据，需要继承此类
 **************************************************************************************************/

class EventStatsDataBase
{
public:
    // 格式化本地保存数据
    virtual void FormatXmlData(xml_node & node);

    // 事件数据转换到统计数据
    virtual void TransferStatsData(EventDataBase * pEventData);

    const char * GetEvenName(int nEvent);

    // 是否有第二关键字
    bool IsSupportSecondKey();

    // 比较第二关键字是否一致
    bool CompareSecondKey(EventStatsDataBase * pData);

public:
    EventStatsDataBase();

    EventStatsDataBase(EventType eType);

    EventType   m_eEventType;               // 事件类型
    time_t      m_nTime;
    int         m_nCnt;                     // 次数

    int         m_wParam;
    int         m_lParam;
    yl::string  m_strExt;                   // 扩展数据
};
#endif

