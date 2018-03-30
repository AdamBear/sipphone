#ifndef __UIANDLOGIC_COMMON_H__
#define __UIANDLOGIC_COMMON_H__

#include <yllist.h>
#include <ylvector.h>
#include "interfacedefine.h"
#include "talklogic_common.h"
#include "broadsoft/callcenter/include/cccommon.h"
#include "innermsgdefine.h"
#include "talk/dialplan/include/dialplan_data.h"
#include "uiandlogic_base.h"
#include "dsskey/include/dsskeystructure.h"
#include "talk/talkhelp/include/talkhelp_data.h"

#ifdef IF_SUPPORT_FIVE_PARTY
#define LOCAL_CONF_MAX_PARTY  4
#else
#define LOCAL_CONF_MAX_PARTY  2
#endif

enum TALK_CALLOUT_RESULT
{
    TALK_CALLOUT_FAIL,
    TALK_CALLOUT_SUCCESS,
    TALK_CALLOUT_BLOCK,
    TALK_CALLOUT_IP_DIRECT_FORBIDDEN,
};

#define TALK_MAX_NAME_LEN 256

// Session类型
enum SESSION_STATE
{
    SESSION_UNKNOWN = 0,
    SESSION_DIALING,             // 拨号
    SESSION_PREDIAL,             // 预拨号
    SESSION_PRETRAN,             // Transfer拨号
    SESSION_PRE_RETRIEVEPARK,    // Retrieve拨号界面
    SESSION_BTDIALING,           //蓝牙拨号

    SESSION_CONNECTING,          // 呼出连接
    SESSION_RINGBACK,            // 呼出回铃
    SESSION_CONNECTING_TRAN,     // tran之后的呼出连接
    SESSION_RINGBACK_TRAN,       // tran之后的呼出回铃
    SESSION_BLACONNECTING,       // BLA呼出
    SESSION_NETWORKCONF_CONNECT, // 网络会议呼出
    SESSION_BTCONNECTING,        //蓝牙呼出连接

    SESSION_LOCALCONF,           // 本地会议

    SESSION_BETRANSFERRED,       // 被转移

    SESSION_RINGING,             // 来电响铃
    SESSION_RECALL,              // 盲转失败后的回拨
    SESSION_BTRINGING,           //蓝牙来电响铃

    SESSION_TALKING,             // 通话中
    SESSION_NETWORKCONF,         // 网络会议
    SESSION_TRANSFERRED,         // 转移等待状态
    SESSION_AOC_TALKING,         // AocTalk界面
    SESSION_LINEDEVICE,          // 外接设备话路
    SESSION_BTTALKING,           //蓝牙通话中

    SESSION_FINISHED,            // 通话完成
    SESSION_AUTO_REDIAL,         // 自动回拨
    SESSION_BTFINISHED,          //蓝牙通话完成

    SESSION_ATSCONF,             // 华为网络会议
    SESSION_PARK_NOTIFY,         // Retrieve拨号界面
    SESSION_AOC_FINISH,          // AocFinish界面
};

// Routine类型
enum ROUTINE_STATE
{
    ROUTINE_UNKNOWN = 0,
    ROUTINE_PREDIAL,             // 预拨号
    ROUTINE_DIALING,             // 拨号
    ROUTINE_PRETRAN,             // Transfer拨号
    ROUTINE_CONNECTING,          // 呼出连接
    ROUTINE_RINGBACK,            // 呼出回铃
    ROUTINE_RINGING,             // 来电响铃
    ROUTINE_TALKING,             // 通话中
    ROUTINE_FINISHED,            // 通话完成
    ROUTINE_NETWORKCONF_CONNECT, // 网络会议呼出
    ROUTINE_NETWORKCONF,         // 网络会议
    ROUTINE_ATSCONF,             // 华为会议
    ROUTINE_BLACONNECTING,       // BLA呼出
    ROUTINE_AUTOREDIAL,          // 自动重拨
    ROUTINE_RECALL,              // 盲转失败后的回拨
    ROUTINE_RETRIEVE_CALLPARK,   // RetrieveCallPark输入界面
    ROUTINE_PARK_NOTIFY,         // ParkNotify界面

    ROUTINE_BTDIALING,           //蓝牙拨号
    ROUTINE_BTPREDIAL,           //蓝牙预拨号
    ROUTINE_BTCONNECTING,        //蓝牙呼出连接
    ROUTINE_BTRINGING,           //蓝牙来电响铃
    ROUTINE_BTTALKING,           //蓝牙通话中
    ROUTINE_BTFINISHED,          //蓝牙通话完成
    ROUTINE_AOC_FINISH,          // AocFinish界面
    ROUTINE_LINE_DEVICE,         // 外接设备


    /* 内部标记 */
    ROUTINE_DIALING_CLS,         // 拨号基类
    ROUTINE_CONNECTING_CLS,      // 呼出基类
    ROUTINE_TALKING_CLS,         // 通话基类
    ROUTINE_FINISHED_CLS,        // 完成基类
    ROUTINE_RINGING_CLS,         // 来电基类

    ROUTINE_LAST,                // 最后一个
};

#define _ISTALKROUTINESTATE(x) (x == ROUTINE_TALKING || x == ROUTINE_BTTALKING)

// ui和logic之间的Action类型
enum TALK_ACTION_TYPE
{
    TALK_ACTION_NONE = 0,
    TALK_ACTION_KEYPRESS,              // 有按键按下 wParam:int 键值   lParam:0    ExtraData:yl::string*类型:Softkey Action String
    TALK_ACTION_TEXT_CHANGE,           // 号码改变    wParam:0        lParam:0    ExtraData:DataDial2Logic*
    TALK_ACTION_SEND,                  // 呼出      wParam:0        lParam:0    ExtraData:DataDial2Logic*
    TALK_ACTION_DPICKUP,               // DPickup呼出   wParam:0        lParam:0    ExtraData:DataDial2Logic*
    TALK_ACTION_GPICKUP,               // GPickup呼出   wParam:0        lParam:0    ExtraData:NULL
    TALK_ACTION_CANCEL,                // 退出      wParam:0/1/2(1表示通道键引发,2表示网页发送handup消息挂断)  lParam:0    ExtraData:None
    TALK_ACTION_NEWCALL,               // 创建新通话 wParam:0        lParam:0    ExtraData:None
    TALK_ACTION_ANSWER,                // 接听通话    wParam:0/1(1表示通道键引发)  lParam:0    ExtraData:None
    TALK_ACTION_REJECT,                // 拒绝通话    wParam:0/1(1表示通道键引发)  lParam:0    ExtraData:None
    TALK_ACTION_SILENCE,               // 来电铃声静音wParam:0      lParam:0    ExtraData:None
    TALK_ACTION_FWD_TO,                // 来电fwdto wParam:0        lParam:0    ExtraData:None
    TALK_ACTION_HOLD,                  // 通话中Hold   wParam:0        lParam:0    ExtraData:None
    TALK_ACTION_RESUME,                // 通话中unholdwParam:0     lParam:0    ExtraData:None
    TALK_ACTION_SWAP,                  // 通话中swap   wParam:0        lParam:0    ExtraData:None
    TALK_ACTION_MUTE,                  // 通话中Mute   wParam:0/1 是否设置keep-mute        lParam:0    ExtraData:None
    TALK_ACTION_TRAN,                  // Transfer    wParam:(defalut:0)int callid    lParam:0    ExtraData:DataDial2Logic*
    TALK_ACTION_CONF,                  // Conference  wParam:0        lParam:0    ExtraData:DataDial2Logic*
    TALK_ACTION_SPLIT,                 // 拆开会议    wParam:0        lParam:0    ExtraData:None
    TALK_ACTION_FOCUSE_CHANGE,         // 焦点Session改变wParam:0/1(up/down)    lParam:0    ExtraData:None
    TALK_ACTION_SCREEN_CLICK,          // 点击屏幕事件    wParam:0        lParam:0    ExtraData:None VPX系列需要使用这种类型
    TALK_ACTION_VOLUME_CHANGE,         // 调节音量事件    wParam:int 音量值 lParam:0   ExtraData:None
    TALK_ACTION_VOLUMEBAR_SHOW,        // 音量条显示事件wParam:0        lParam:0 ExtraData:None
    TALK_ACTION_REDIAL,                // Redial    wParam:0        lParam:0    ExtraData:None
    TALK_ACTION_GROUP_LISTEN,          // 群听    wParam:0        lParam:0    ExtraData:None
    TALK_ACTION_HOLD_PUBLIC,           // public    wParam:0        lParam:0    ExtraData:None
    TALK_ACTION_HOLD_PRIVATE,          // private   wParam:0        lParam:0    ExtraData:None
    TALK_ACTION_HOLD_SWITCH,           // 更改hold状态 wParam:0     lParam:0    ExtraData:None
    TALK_ACTION_WND_FOCUSE_CHANGE,     // 窗口焦点改变   wParam:0=非焦点,1=焦点 lParam:0    ExtraData
    TALK_ACTION_ACCOUNT_CHANGE,        // 账号改变 wParam:int=账号ID lParam:0/非0=号码不为空是否呼出   ExtraData
    TALK_ACTION_OK,                    // 按键为OK, wParam:0     lParam:0    ExtraData:None
    TALK_ACTION_REMOVE_CONF_PARTY,     // 踢出会议中的某个话路 wParam:callID lParam:0 ExtraData:None
    TALK_ACTION_CONF_FAR_MUTE,         // 会议中某个参与方静音 wParam:callID lParam:1:mute 0:unmute   ExtraData:None
    TALK_ACTION_ENTER_CONF_MANAGER,    //进入会议管理
    TALK_ACTION_EXIT_CONF_MANAGER,     //退出会议管理
    TALK_ACTION_DTMF,                  // DTMF事件, wparam:按键
    TALK_ACTION_PARK,                  // Park一路通话      wParam:0       lParam:0    ExtraData:DataDial2Logic*
    TALK_ACTION_GROUP_PARK,            // GroupPark一路通话 wParam:0       lParam:0    ExtraData:DataDial2Logic*
    TALK_ACTION_RETRIEVE_PARK,         // 取回一路通话        wParam:0       lParam:0    ExtraData:NULL
    TALK_ACTION_OEM_FLASH,             // joyce要求通话中ok键发flash
    TALK_ACTION_SMS,                   // sms
    TALK_ACTION_CALLINFO,              // RingUI界面按Call Info键的处理
    TALK_ACTION_MENU_BACK,             // MENU返回
    TALK_ACTION_MENU_ENTER,            // MENU某项的选中    wParam:index lParam:0 ExtraData
    TALK_ACTION_MENU_AUTO_QUIT,        // MENU自动退出
    TALK_ACTION_EXIT_FAILD_INFO,       // 退出transfer失败提示
    TALK_ACTION_TRAN_TO,               // Transfer To  wParam:SessionID    lParam:0    ExtraData:None
    TALK_ACTION_MUTE_VIDEO,            // Mute视频 wParam:0      lParam:0    ExtraData:文件路径
    TALK_ACTION_TO_MOBILE,             //声音通道切换至手机 wParam:0  lParam:0    ExtraData:None
    TALK_ACTION_TO_SIP,                //声音通道切换回话机 wParam:0  lParam:0    ExtraData:None
    TALK_ACTION_PREFIX_PARK,           // 拨号界面第三个softkey当prefix的作用,通话界面当park作用
    TALK_ACTION_CALL_BACK,
    TALK_ACTION_RECORD_START,
    TALK_ACTION_RECORD_STOP,
    TALK_ACTION_RECORD_PAUSE,
    TALK_ACTION_RECORD_RESUME,
    TALK_ACTION_RECORD_BACK,
    TALK_ACTION_CALL_INITIATION,       // Call Initiation
    TALK_ACTION_CALL_PUSH,             // Call Push
    TALK_ACTION_DIAL_ON_HOLD,          // hold之后按数字键进入拨号
    TALK_ACTION_OPENDOOR,
    TALK_ACTION_CALL_DECLINE,          // Call Decline功能 wParam:true/false decline操作/ignore操作      lParam:0    ExtraData:NULL
    TALK_ACTION_TRAN_BIND_SPONSOR,
    TALK_ACTION_FLASH,
    TALK_ACTION_KEY_PAD,               // 全键盘界面 wParam:0/1 退出/进入 lParam:0    ExtraData:None

    TALK_ACTION_REQUEST_VIDEO,         // 语音通话中请求切换到视频通话
    TALK_ACTION_RESPOND_VIDEO,         // 响应视频切换的操作结果
    TALK_ACTION_VIDEO_SHARING,
    TALK_ACTION_CONF_FAR_HOLD,         // 会议中某个参与方Hold wParam:callID lParam:1:Hold 0:unHold ExtraData:None
    TALK_ACTION_REMOVE_NET_CONF_PARTY, // 踢出网络会议中的某个话路 ExtraData: userID
    TALK_ACTION_RECORD_RESULT,
    TALK_ACTION_PLAY_TONE,
    TALK_ACTION_CURRENT_CALLOUT,
    TALK_ACTION_JOIN,                  // （会议优化）直接将来电加入到会议中
    TALK_ACTION_SPLIT_ALL,             //（会议优化）将整个会议拆分，相对于TALK_ACTION_SPLIT则是单个拆分
    TALK_ACTION_FAR_RESUME,            //（会议优化）单独resume

};

enum TALK_ANSWER_TYPE
{
    TAT_NONE    = 0,
    TAT_CHANNEL = 1,    // 通道键触发
    TAT_AUDIO   = 2,    // 只开声音通道
    TAT_DSK_EXTERN = 3, // 外部DSK_MSG_HANG_UP消息触发
};

// 拨号界面类型
enum DIAL_UI_TYPE
{
    DIAL_UI_UNKNOWN = -1,
    DIAL_UI_DIALING,          // 拨号状态
    DIAL_UI_PREDIAL,          // 预拨号状态
    DIAL_UI_RINGFORWARD,      // 来电转接状态
    DIAL_UI_TRANSFER,         // Transfer输入状态
    DIAL_UI_CONFERENCE,       // Conference输入状态
    DIAL_UI_DPICKUP,          // DPickup的输入状态，由DPickup的dsskey引发
    DIAL_UI_CALLPARK,         // CallPark输入状态
    DIAL_UI_GROUPCALLPARK,    // GroupCallPark输入状态
    DIAL_UI_RETRIEVE_CALLPARK,// RetrieveCallPark输入状态
    DIAL_UI_BLUETOOTH_DIALING,// 蓝牙拨号状态
    DIAL_UI_BLUETOOTH_PREDIAL,// 蓝牙预拨号状态

#if IF_BUG_27033 || IF_BUG_30965
    DIAL_UI_PAGING,           // Paging的输入状态，由Paging的dsskey引发
    DIAL_UI_INTERCOM,         // Intercom的输入状态，由Intercom的dsskey引发
    DIAL_UI_UNPARK,           // Un-Park的输入状态
#endif
#if IF_FEATURE_DIAL_ON_HOLD
    DIAL_UI_ON_HOLD,          // 通话hold之后，按下输入键进入拨号
#endif
};

enum CALL_RECORD_STATUE
{
    CALL_RECORD_UNKNOWN = -1,
    CALL_RECORD_NORMAL,
    CALL_RECORD_START,
    CALL_RECORD_PAUSE,
};

// 通话界面类型
enum TALKING_UI_TYPE
{
    TALKING_UI_UNKNOWN = -1,
    TALKING_UI_NORMAL,      // 普通通话状态
    TALKING_UI_TRANSFERRED, // 转移后的通话状态
    TALKING_UI_TRANSFERRING,// 咨询转的通话状态
    TALKING_UI_BETRANSFER,  // 被转移的通话状态
    TALKING_UI_NETWORKCONF, // 网络会议状态
    TALKING_UI_LINEDEVICE,  // 外接设备话路
};

// 通话routine的状态
enum TALK_STATE
{
    TS_TALK = 0, // 和对方通话中
    TS_HOLD,     // Hold对方
    TS_HELD,     // 对方Hold
    TS_HOLD_HELD,// Hold对方且被对方Hold
    TS_HOLD_OR_HELD,
    TS_UNKNOW,
};


//错误信息类型
enum ERROR_INFO_TYPE
{
    EIT_UNKNOWN = -1,
    EIT_INVALID_CODEC_TOCONF, // codec不一致，无法建立conf
    EIT_NOT_AVALIABLE_G722_SESSION_TOCONF, // 会议中超过两路G722话路，无法建立conf
    EIT_NOT_AVALIABLE_SRTP_SESSION_TOCONF, // 会议中超过两路G722话路，无法建立conf
    EIT_USB_WARNING_SPACE,  // 可录制时间不足
    EIT_USB_SPACE_NOTENOUGH,    // 空间不足,保存录音
    EIT_NOT_AVALIABLE_VIDEO_RESOLUSTION,    // 会议中超过视频分辨能力
    EIT_NOT_AVALIABLE_OPUS_SESSION_TOCONF, // 会议中超过两路OPUS话路，无法建立conf
    ETI_ALREADY_PARKED_TO_THIS_SLOT,    //call park key已有一路park，通话中再次点击该park不生效
};

// 定义自动重拨状态
enum AUTOREDIAL_STATUS
{
    AUTOREDIAL_IDLE = 0,
    AUTOREDIAL_TIP,
    AUTOREDIAL_COUNTDOWN,
};

// URLRecod错误图标类型
enum URL_RECORD_ERROR
{
    URE_NONE = 0,           // 没有错误图标
    URE_CANNOT_BE_RECORDED, // 不能录音
    URE_CANNOT_BE_STOPPED,  // 不能停止
    URE_CANNOT_BE_STARTED,  // 不能开始
    URE_FULL,               // 满了
};

// PreTransfer界面按Blf的处理方式
enum DSSKEY_TRANS_TYPE
{
    DTT_NEW_CALL       = 0,       // 按下BLF后，相当于已经输入号码
    DTT_CALL_OUT       = 1,       // 按下BLF后，相当于输入号码后并按"OK"或"#"键呼出
    DTT_BLIND_TRANSFER = 2,       // 按下BLF后，直接盲转
#if IF_BUG_30961
    DTT_FON_CALL_OUT   = 3,       // 与DTT_CALL_OUT相似，但是挂机时不将通话转出而是挂断当前话路
#endif
    DTT_AUTO           = 4,       // 通话界面下点击盲转，transfer界面下咨询转，拨号界面，新话路呼出
};

enum ICON_TYPE
{
    IT_UNKNOWN,

    IT_RECORD,
    // url record
    IT_CANNOT_BE_RECORDED,
    IT_CANNOT_BE_STOPPED,
    IT_CANNOT_BE_STARTED,
    IT_RECORD_NORMAL,
    IT_RECORD_PAUSE,
    IT_RECORD_FULL,

    IT_ENCRY,
    IT_MUTE,

    IT_STATE_MISSCALL,
    IT_STATE_AA,
    IT_STATE_DND,
    IT_STATE_FWD,
    IT_STATE_LOCK,
    IT_STATE_RING_MUTE,
    IT_STATE_VOICEMAIL,
    IT_STATE_TEXTMESSAGE,
    IT_STATE_VPN,
    IT_STATE_INTERNET,
    II_ACCOUNT_LS_DISABLE,
    II_ACCOUNT_LS_LOGING,
    II_ACCOUNT_LS_FORWARD,
    II_ACCOUNT_LS_DND,
    II_ACCOUNT_LS_ACD_LOGIN,
    II_ACCOUNT_LS_ACD_LOGOUT,
    II_ACCOUNT_LS_ACD_UNAVAILABLE,
    II_ACCOUNT_LS_ACD_WRAPUP,
    II_ACCOUNT_LS_HEADSET,
    II_ACCOUNT_LS_SCA_REGISTED,
    II_ACCOUNT_LS_CLOUD_REGISTED,
    II_ACCOUNT_LS_REGISTED,
    II_ACCOUNT_LS_REGISTEDFAIL,
    II_ACCOUNT_LS_UNREGISTER,

    IT_STATE_TALK_MUTE,

    IT_USB_DEVICE,
    IT_USB_DEVICE_DETECTING,
    IT_USB_IDLE_RECORD,
    IT_USB_IDLE_RECORD_PAUSE,
    IT_STATE_PSTN,
    IT_STATE_BLUETOOTH,
    IT_STATE_BLUETOOTH_CONNECTED,
    IT_STATE_BLUETOOTH_PHONE,
    IT_STATE_BLUETOOTH_PHONE_CONNECTED,
    IT_STATE_WIFI,
    IT_STATE_WIFI_CONNECTED
};

enum DialRuleMatch
{
    DRM_NONE        = 0,
    DRM_DIALRULE    = 0x01 << 0,  // dial rule模式
    DRM_DIGITMAP    = 0x01 << 1,  // digitmap模式
    DRM_ALL         = DRM_DIALRULE | DRM_DIGITMAP,
};

//错误信息
struct Error_Info
{
    Error_Info()
    {
        eErrorType = EIT_UNKNOWN;
        strErrorTip = "";
        iErrorShowTime = -1;
    }

    //错误类型
    ERROR_INFO_TYPE eErrorType;
    //错误提示
    yl::string strErrorTip;
    //提示时长
    int iErrorShowTime;
};

// 呼出时的参数(for CallOut function(Dsskey callout))
struct CallOut_Param
{
    CallOut_Param()
    {
        iAccountID = AUTO_SELECT_LINE_ID;
        bConferenceDirect = false;
        nCallId = -1;
        bNeedPrefix = false;
        bForceToCallOut = false;
        bConfOnePress = false;

        iPickUpBLFIndex = -1;
        bIntercom = false;
        eCallOutType = TCT_UNKNOWN;
        bAudioOnly = true;
        eTransferType = DTT_NEW_CALL;
        bBindDsskey = true;
        iSponsorDsskeyID = INVALID_DSSKEY_ID;
        eCallOutTrigger = COT_DEFAULT;
        bDelOldLineSession = true;
        iErrorInfo = TALK_ERROR_NONE;
        iBLAIndex = -1;
        bSilentBarginIn = false;
    }

    yl::string          strNumber;
    yl::string          strName;
    int                 iAccountID;
    bool                bConferenceDirect;
    int                 nCallId;
    bool                bNeedPrefix;
    bool                bForceToCallOut;
    bool                bConfOnePress;
    int                 iSponsorDsskeyID;
    bool                bDelOldLineSession;
    int                 iErrorInfo;
    int                 iBLAIndex;
    bool                bSilentBarginIn;

    int                 iPickUpBLFIndex;
    bool                bIntercom;
    yl::string          strAddLocalNum;         // 如果不为空,则在发给sip的消息中添加local_number项
    CDialPlanMask       tDialPlanMask;          // 是否需要匹配替换规则，比如呼叫记录中呼出不再匹配
    TALK_CALLOUT_TYPE   eCallOutType;           // 呼出类型
    yl::string          m_strCode;              //ACD DispCode
#ifdef IF_FEATURE_BROADSOFT_MEET_ME
    yl::string          m_strAutoDTMF;          // Meet-me时发送DTMF字符串
#endif //#ifdef IF_FEATURE_BROADSOFT_MEET_ME
    yl::string          strRemoteName;          //用于pickup的远程名
    bool                bAudioOnly;
    DSSKEY_TRANS_TYPE   eTransferType;          // 转移类型
    bool                bBindDsskey;

    ROUTINE_STATE       eRoutineType;           //切换tineroutine
    yl::string          strCallParkNumber;      // 被Park通话的号码
    yl::string          strCallParkDiaplayName; // 被Park通话的联系DisplayName

    CallOutTrigger      eCallOutTrigger;        //触发类型
#if IF_FEATURE_CTRIP_CTI
    yl::string          strXUUI;
#endif
};

// ui发送给logic的数据
struct DataUI2Logic
{
    DataUI2Logic()
    {
        eAction = TALK_ACTION_NONE;
        wActionParam = 0;
        lActionParam = 0;
        pData = NULL;
    }

    // 发送给logic的Action类型
    TALK_ACTION_TYPE eAction;
    //第一个Action的参数
    WPARAM wActionParam;
    //第二个Action的参数
    LPARAM lActionParam;
    // 发送给logic的数据
    void* pData;
};


// 发送给logic的数据
struct DataDial2Logic
{
    DataDial2Logic()
    {
        strNumber = "";
        strName = "";
    }

    // 号码
    yl::string   strNumber;
    // 名称
    yl::string   strName;
};

// 用于与UI交互的自动重拨数据变量
struct AUTOREDIAL_DATA
{
    AUTOREDIAL_DATA()
    {
        iRemainRedialTimes = 0;
        iRedialInterval = 0;
        eStatus = AUTOREDIAL_IDLE;
        strNumber = "";
    }

    AUTOREDIAL_DATA& operator=(const AUTOREDIAL_DATA& data)
    {
        if (this != &data)
        {
            iRemainRedialTimes = data.iRemainRedialTimes;
            iRedialInterval = data.iRedialInterval;
            eStatus = data.eStatus;
            strNumber = data.strNumber;
        }

        return *this;
    }

    // 剩余的重拨次数
    int iRemainRedialTimes;
    // 自动重拨间隔
    int iRedialInterval;
    // 重拨状态
    AUTOREDIAL_STATUS eStatus;
    // 重拨号码
    yl::string  strNumber;
};

// 网络会议参与者信息
struct DataNetConfUserInfo
{
    DataNetConfUserInfo()
    {
        strUserName = "";
        strUserID = "";
        bConfCreator = false;
    }

    yl::string strUserName; // 界面显示与会者名称
    yl::string strUserID;   // 删除成员时需要传给逻辑层userid
    bool bConfCreator;      // 是否为会议建立者

    bool operator == (const DataNetConfUserInfo& rhs)
    {
        return strUserID == rhs.strUserID && strUserName == rhs.strUserName;
    }
};

#if IF_BUG_22457_Record
enum SIP_RECORD_STATUS
{
    SIP_RECORD_STATUS_UNKNOWN   = -1,
    SIP_RECORD_STATUS_ON_FAILED  =  0, /*开启录音失败*/
    SIP_RECORD_STATUS_ON_SUCCESS =  1, /*开启录音成功*/
    SIP_RECORD_STATUS_OFF_FAILED =  2, /*关闭录音失败*/
    SIP_RECORD_STATUS_OFF_SUCCESS =  3, /*关闭录音成功*/
};
#endif

#if IF_BUG_27033 || IF_BUG_30965
enum DIAL_DSSKEY_TYPE
{
    DDT_NONE = 0,
    DDT_PICKUP,
#if IF_FEATURE_PAGING
    DDT_PAGING,
#endif
    DDT_INTERCOM,
    DDT_UNPARK,
};
#endif

#ifdef IF_FEATURE_BROADSOFT_CALLRECORDING
enum CALL_RECORD_ACTION_RESULT
{
    CALL_RECORD_NONE = 0,
    CALL_RECORD_ACTION_PROCESSING,
    CALL_RECORD_START_SUCCESS,
    CALL_RECORD_PAUSE_SUCCESS,
    CALL_RECORD_RESUME_SUCCESS,
    CALL_RECORD_STOP_SUCCESS,
    CALL_RECORD_START_FAILED,
    CALL_RECORD_PAUSE_FAILED,
    CALL_RECORD_RESUME_FAILED,
    CALL_RECORD_STOP_FAILED,
};
#endif

enum TALK_DELAY_OPERATE
{
    TDO_NONE = 0,
    TDO_BLIND_TRANSFER,
};

#endif // __UIANDLOGIC_COMMON_H__
