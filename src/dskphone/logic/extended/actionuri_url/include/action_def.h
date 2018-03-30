#ifndef __ACTION_TYPE_DEFINE_H__
#define __ACTION_TYPE_DEFINE_H__

#include "ylstring.h"

#if !IF_FEATURE_EVENT_MODULE
// ActionUrl类型.
enum ActionUrlType
{
    ACTION_URL_TYPE_NONE = 0,  // 不正确的Url类型.

    ACTION_URL_SETUP_COMPLETED = 1,  // 话机启动完成, 可以开始使用的时候.
    ACTION_URL_LOGIN_ON = 2,  // 帐号注册成功.
    ACTION_URL_LOGIN_OFF = 3,  // 账号注销.
    ACTION_URL_REGISTER_FAILED = 4,  // 账号注册失败.

    // 通话部分.
    ACTION_URL_OFFHOOK = 5,  // 摘机.
    ACTION_URL_ONHOOK = 6,  // 挂机.
    ACTION_URL_INCOMING_CALL = 7,  // 话机来电.
    ACTION_URL_REJECT_INCOMING_CALL = 8,  // 话机拒绝来电.
    ACTION_URL_FORWARD_INCOMING_CALL = 9,  // 话机转接来电.
    ACTION_URL_CALL_REMOTE_CANCELED = 10,  // 来电时, 对方取消.
    ACTION_URL_ANSWER_NEW_IN_CALL = 11,  // 话机在通话中时接起新来电.
    ACTION_URL_REJECT_NEW_IN_CALL = 12,  // 话机在通话中时拒绝新来电.
    ACTION_URL_CALLOUT = 13,  // 话机拨出.
    ACTION_URL_CANCEL_CALLOUT = 14,  // 话机呼出时按cancel键.
    ACTION_URL_REMOTE_BUSY = 15,  // 话机呼出时被对方拒接.
    ACTION_URL_ESTABLISHED = 16,  // 通话建立.
    ACTION_URL_TERMINATED = 17,  // 通话挂断.

    // 话机状态开关.
    ACTION_URL_OPEN_DND = 18,  // 话机开启DND.
    ACTION_URL_CLOSE_DND = 19,  // 话机关闭DND.
    ACTION_URL_OPEN_ALWAYS_FORWARD = 20,  // 话机开启Alwasy Forward.
    ACTION_URL_CLOSE_ALWAYS_FORWARD = 21,  // 话机关闭Always Forward.
    ACTION_URl_OPEN_BUSY_FORWARD = 22,  // 话机开启Busy Forward.
    ACTION_URL_CLOSE_BUSY_FORWARD = 23,  // 话机关闭Busy Forward.
    ACTION_URL_OPEN_NO_ANSWER_FORWARD = 24,  // 话机开启No Answer Forward.
    ACTION_URL_CLOSE_NO_ANSWER_FORWARD = 25,  // 话机关闭No Answer Forward.

    // Transfer部分.
    ACTION_URL_TRANSFER_CALL = 26,  // 通话中Transfer, 输入号码, Transfer.
    ACTION_URL_BLIND_TRANSFER = 27,  // 通话中Transfer, 输入号码, Transfer.
    ACTION_URL_ATTENDED_TRANSFER = 28,  // 通话中Transfer, 输入号码, 呼出.
    ACTION_URL_TRANSFER_FINISHED = 29,  // 通话中按Transfer, 输入号码再按Transfer,
    // 对方接起.
    ACTION_URL_TRANSFER_FAILED = 30,  // 通话中按Transfer, 输入号码, 再按Transfer.

    // Hold部分.
    ACTION_URL_HOLD = 31,  // 话机执行Hold操作.
    ACTION_URL_UNHOLD = 32,  // 话机执行解Hold操作.

    // Mute部分.
    ACTION_URL_MUTE = 33,  // 话机执行Mute操作.
    ACTION_URL_UNMUTE = 34,  // 话机执行Unmute操作.

    // 其他.
    ACTION_URL_MISSED_CALL = 35,  // 话机收到新未接来电(即使话机关闭了
    // Missed Call的记录, 这条消息也照发).
    ACTION_URL_BUSY_TO_IDLE = 36,  // 话机从通话状态回到IDLE界面.
    ACTION_URL_IDLE_TO_BUSY = 37,  // 话机从IDLE界面进入通话状态.
    ACTION_URL_IP_CHANGED = 38,  // 话机IP变化.

    ACTION_URL_REMOTE_FORWARD = 39,

    ACTION_URL_REMOTE_HOLD,
    ACTION_URL_REMOTE_UNHOLD,

    ACTION_URL_AUTOP_FINISH, // 通知服务器autop完成

    ACTION_URL_HANDFREE,
    ACTION_URL_HEADSET,

    ACTION_URL_OPEN_CALLWAIT,   //开启callwaiting
    ACTION_URL_CLOSE_CALLWAIT,  //关闭callwaiting

    ACTION_URL_VIEW_MISSCALL,     //查看Missed  Calls
    ACTION_URL_VIEW_FORWARD,    //查看Forwarded Calls


    //CSTA Event

    ACTION_CSTA_BEGIN,

    ACTION_CSTA_ENTER_PRE_CALL,

    ACTION_CSAT_BEFORE_CALLOUT,

    ACTION_CSTA_RINGBACK,

    ACTION_CSTA_INCOMING_CALL,

    ACTION_CSTA_ESTABLISHED,  // 通话建立.
    ACTION_CSTA_TERMINATED,  // 通话挂断.

    ACTION_CSTA_HOLD,

    ACTION_CSTA_RETRIEVE,

    ACTION_CSTA_SINGLE_TRANSFER,

    ACTION_CSTA_ATTEND_TRANSFER,

    ACTION_CSTA_CONFERENCE,

    ACTION_CSTA_DTMF,

    ACTION_CSTA_FORWARD_CALL,

    ACTION_CSTA_CALL_FAIL,

    ACTION_CSTA_NONE,

    ACTION_URL_TYPE_MAX
};
#endif

// 各变量在URL中对应的字符串, 多个变量之间用&连接
#define VAR_MAC         "$mac"              // MAC地址
#define VAR_IP          "$ip"               // IP地址
#define VAR_PHONE_TYPE  "$model"            // 话机类型
#define VAR_FIRMWARE    "$firmware"         // 固件版本
#define VAR_ACTIVE_URL  "$active_url"       // 当前活动帐号的SIP URI
#define VAR_ACTIVE_USER "$active_user"      // 当前活动帐号的SIP URI的用户帐号部分
#define VAR_ACTIVE_HOST "$active_host"      // 当前活动帐号的SIP URI的服务器部分
#define VAR_LOCAL       "$local"            // 被呼叫者的SIP URI
#define VAR_REMOTE      "$remote"           // 呼叫者的SIP URI
#define VAR_DISPLAY_LOCAL  "$display_local" // 被呼叫者的display name
#define VAR_DISPLAY_REMOTE "$display_remote"// 呼叫者的display name
#define VAR_CALLID      "$call_id"          // 通话ID

#define VAR_FWD_LINE    "$fwd_line"         // 哪个线路设置通话转接
#define VAR_FWD_NUM     "$target"           // 通话转接目标号码
#define VAR_NOANS_FWDTIME "$fwd_time"       // 无应答转接时间
#define VAR_FWD_NUM_OLD "$cfwd_dest"        // 【兼容旧版本】通话转接目标号码
#define VAR_NOANS_FWDTIME_OLD "$no_answ_secs"   // 【兼容旧版本】无应答转接时间

// 以下几项未确定
#define VAR_CSTAID      "$csta_id"          // CSTA ID

#define VAR_EXP_MODULE  "$expansion_module" // 当前在用的扩展台型号、版本

#define VAR_ACTIVE_KEY  "$active_key"       // the function key (e.g. P1, P5, P32,..) associated with a call
#define VAR_CALLER_ID   "$callerID"
#define VAR_DIAL_NUM    "$calledNumber"

//T46中是这个
#define  kszCallNumber "$callNumber"

// YUN新增放入中性版本
#define VAR_DURATION    "$duration"         // 通话时长,单位秒
#define VAR_CALL_DIR    "$callDirection"    // 记录来电和去电,值是in,out或unknown
#define VAR_ACCOUNT_URLS "$AccountInfo"

#define VAR_DIVERSION_NUM  "$DiversionNumber"
#define VAR_EXP             "$exp"            //exp个数
#define VAR_EXP_NUM         "$exp_number"     //exp个数
#define VAR_EHS             "$ehs"            //ehs个数
#define VAR_EHS_NUM         "$ehs_number"     //ehs个数
#define VAR_USB             "$usb"            //usb个数
#define VAR_USB_NUM         "$udisk_number"   //usb个数
#define VAR_WIFIDONGEL      "$wifi"           //wifi dongle 个数   
#define VAR_WIFIDONGEL_NUM  "$wifi_number"    //wifi dongle 个数
#define VAR_BLUETOOTHDONGLE "$bluetooth" //bluetooth 
#define VAR_BLUETOOTHDONGLE_NUM "$bluetooth_number" //bluetooth 
#define VAR_USB_HEADSET     "$usbheadset"
#define VAR_USB_HEADSET_NUMBER  "$usbheadset_number"

enum ACTION_URI_TYPE
{
    ACTION_AUTOP,
    ACTION_DNDON,
    ACTION_DNDOFF,
    ACTION_ATRANS,//Attends Transfer
    ACTION_BTRANS,//Blind Transfer
    ACTIONURI_ASW,
    ACTIONURI_REDIAL,
    ACTIONURI_CALLEND,
    ACTION_SET_REMOTEIP,
    ACTION_DEL_REMOTE_IP,
    ACTIONURI_HOLD,
    ACTIONURI_UNHOLD
};


// 存储URL变量的类
class ActionUrlData
{
public:
    ActionUrlData()
    {
        m_strSIPURL = "";
        m_strSIPURLUser = "";
        m_strSIPURLHost = "";
        m_strLocal = "";
        m_strRemote = "";
        m_strDisplayLocal = "";
        m_strDisplayRemote = "";
        m_strCallID = "";

        m_strCSTAID = "";
        m_strEXPModule = "";
        m_strActiveKey = "";
        m_strCalledNum = "";
        m_strDuration = "0";
        m_strCallDir = "unknown";
        m_strFwdLine = "";
        m_strFwdNum = "";
        m_strFwdNoAnswerTime = "";
        m_strDiversionNum = "";
    }

    yl::string m_strSIPURL;     // 本地呼出时使用的号码,例如:sip:123@10.2.1.199
    yl::string m_strSIPURLUser; // 本地账号用户名
    yl::string m_strSIPURLHost; // 本地账号服务器
    yl::string m_strLocal;      // 同m_strSIPURL
    yl::string m_strDisplayLocal;   // 本地displayname
    yl::string m_strRemote;     // 对方号码,格式同m_strLocal
    yl::string m_strDisplayRemote;// 对方displayname
    yl::string m_strCallID;     // callID

    yl::string m_strCSTAID;     //
    yl::string m_strEXPModule;  //
    yl::string m_strActiveKey;  //
    yl::string m_strCalledNum;  // 呼出的号码,用通话对方号码填充
    yl::string m_strDuration;       // 通话时长
    yl::string m_strCallDir;        // 通话类型,呼入则为"in", 否则为"out"

    yl::string m_strFwdLine;        // forward账号ID
    yl::string m_strFwdNum;     // forward目标号码
    yl::string m_strFwdNoAnswerTime;// 无应答转移的时间
    yl::string m_strDiversionNum;     //中转者号码
};

#endif

