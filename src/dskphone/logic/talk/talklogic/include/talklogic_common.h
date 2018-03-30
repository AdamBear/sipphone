#ifndef __TALKLOGIC_COMMON_H__
#define __TALKLOGIC_COMMON_H__

#include "talk/talkproxy/include/scopedelapse.h"
#include <ylstl/ylstring.h>
#include "dsklog/log.h"

#define AUTO_SELECT_LINE_ID  -1  //自动选择Sip帐号的帐号ID
#define INVALID_SESSION_ID ((int)-1)
// SessionID的最大值
#define MAX_SESSION_ID       100000
// 定义自动选择账号的标识
#define AUTO_SELECT_LINE_ID  -1
// 定义当定时器停止时的标识
#define STOP_TIMER_VALUE     -1
// 默认定时器最短时间
#define DEFAULT_TIMER_VALUE  10
// 播放tone时的默认时长(ms)
#define DEFAULT_TONE_INTERVAL 300
// 通话中hold提示间隔(ms)
#define HOLD_ALERT_INTERVAL   30*1000
// 自动退出的时长(ms)，如拨号界面自动退出
#define AUTO_QUIT_TIME       15*1000
// 自动呼出的时长(ms)，如拨号界面自动呼出
#define AUTO_DIAL_TIME       4*1000
// 来电播放提示音间隔
#define RING_INTERVAL_TIME   8*1000 // http://10.2.1.199/Bug.php?BugID=21412

// 新来电界面，设置为焦点后，无操作自动缩起的时间(ms)
#define AUTO_IGNORE_INTERVAL 10*1000
// hold防抖时间间隔(ms)
#define HOLD_DELAY_TIME      300
// 自动重拨提示超时时间
#define AUTOREDIAL_TIP_TIME  5 * 1000
//账号余额信息结构体容纳字符的最大长度
#define MAX_BALANCE_LEN     100
// 播放Tran失败铃声提示的时长(s)
#define TRAN_FAILED_RING_DURATION 60
// 进入界面就需要发送DTMF延时
#define DELAY_DTMF_TIME     200
// 逗号延迟发送DTMF事件
#define COMMA_DELAY_TIME    2*1000

// -1代表自动选择账号，-2代表不需要校验账号
#define DEFAULT_ACCOUNT_ID (-2)

//网络会议订阅信息解析节点定义
#define XML_NET_EVENT_INFO_ROOT "conference-info"
#define XML_NET_EVENT_INFO_DESCRIPTION "conference-description"
#define XML_NET_EVENT_INFO_SUBJECT "subject"
#define XML_NET_EVENT_INFO_USERS "users"
#define XML_NET_EVENT_INFO_USER "user"
#define XML_NET_EVENT_INFO_ENDPOINT "endpoint"
#define XML_NET_EVENT_INFO_DISPLAYTEXT "display-text"
#define XML_NET_EVENT_INFO_STATUS "status"

// Hold类型
enum HOLD_TYPE
{
    HT_NORMAL = 0, // 普通通话的hold
    HT_PUBLIC,     // BLA中的公共hold
    HT_PRIVATE,    // BLA中的私有hold
    HT_BTPHONE,    // 手机下发的hold
    HT_STAUTS_ONLY,     // 只改变Hold状态，只调用ipvp接口的Hold，用于音视频混合会议
    HT_EMENERGY_CALL,  /*紧急号码挂断，用在挂断紧急号码的情况 只执行本地hold流程不告知服务器 http://bugfree.yealink.com/Bug.php?BugID=135463 */
};

// session发起者类型
enum SPONSOR_TYPE
{
    SPONSOR_TRAN, // Tran发起者
    SPONSOR_CONF, // Conf发起者
    SPONSOR_ALL,  // 用于取消所有sponsor
};

// Codec类型
enum CODEC_TYPE
{
    CODEC_TYPE_UNKNOWN,
    CODEC_TYPE_ALL,
    CODEC_TYPE_VOICE,
    CODEC_TYPE_VIDEO,
    CODEC_TYPE_MIX, // 音视频混合
};

// BLA类型
enum BLA_TYPE
{
    BLA_NONE = 0,
    BLA_BROADSOFT_SCA,
    BLA_TELTRONNIC_BLA,
    BLA_DRAFT_BLA, // 当前支持Draft Bla 03， 04、05待支持
    BLA_PBXnSIP,
    BLA_GENBAND,
};

// 会议类型
enum CONF_TYPE
{
    CT_LOCAL = 0x00, // 本地混音
    CT_HUAWEI,       // 华为的会议 v81已不需要CT_HUAWEI，由于配置值功能映射，保留定义
    CT_BROADSOFT,    // Broadsoft网络混音会议
};

// 通话线路类型
enum CALL_LINE_TYPE
{
    CALL_LINE_SIP = 0,
    CALL_LINE_PSTN,
    CALL_LINE_DEVICE,
    CALL_LINE_BT
};

// 通话中OK键处理策略
enum CALL_OK_STRATEGY
{
    CALL_OK_NONE = 0,
    CALL_OK_INFO = 1, // JOYCE
    CALL_OK_STATUS = 2, // NFON
    CALL_OK_UNHOLD = 3, // FORTINET
};

struct  Balance_Info_t
{
    int iResult;
    char szName[MAX_BALANCE_LEN];
    char szAccount[MAX_BALANCE_LEN];
    char szBalance[MAX_BALANCE_LEN];
};

enum REFUSE_TYPE
{
    RT_DND = 0,
    RT_NORMAL,
    RT_BUSY,
};

struct Ring_Download_info
{
    int  iResult;
    int  iIndex;
    //Commlogik-巴西SERPRO T22P 话机死机重启问题
    //http://192.168.1.99/Bug.php?BugID=16774
    //char szPath[20];
    char szPath[128];
};

// 呼出类型
enum TALK_CALLOUT_TYPE
{
    TCT_UNKNOWN = -1,
    TCT_NORMAL = 0,
    TCT_PICKUP,
    TCT_TIPTEL_AUTO_CALLOUT,    // tiptel服务器控制话机自动呼出

    TCT_PAGING,
    TCT_INTERCOM,
    TCT_UNPARK,

    TCT_VOICE_RECORD,  // VOICE_RECORD隐藏呼出
    TCT_CALL_INITIATION, // Call Initiation
    TCT_CALL_SILENT_MONITOR, // Silent Monitor
#if IF_FEATURE_START2START_ACD
    TCT_START2START_ACD_AUTO_LOGIN, // S2S Acd 登入呼出
#endif
    TCT_CALLPARK,
    TCT_CALL_CENTER,
    TCT_PREFIX,
};

// 特征码替换成的文本
#define kszReplaceCallParkCode      "Call Park"
#define kszReplaceCallRetriveCode   "Retrieve"
#define kszReplaceGroupParkCode     "Group Park"
#if IF_BUG_24711
#define kszReplaceGPickUpCode       "FAC Replace - Group Pick Up"  // 与linekey冲突，所以前面加上前缀
#define kszReplaceDPickUpCode       "FAC Replace - Pick Up"
#else
#define kszReplaceGPickUpCode       "Group Pick Up"
#define kszReplaceDPickUpCode       "Pick Up"
#endif
#define kszReplaceVoiceMailCode     "Voice Mail"
#define kszReplaceBargeInCode       "Barge In"
#define kszReplaceCallPullCode      "Call Pull"
#if IF_BUG_23548
#define kszReplaceCallBackCode      "Call Back"
#endif

//通话信息显示方式
enum SHOW_CONTACTDETAIL_TYPE
{
    SHOW_NAME_AND_NUMBER = 0,
    SHOW_NUMBER_AND_NAME = 1,
    SHOW_ONLY_NAME = 2,
    SHOW_ONLY_NUMBER = 3,
    SHOW_FULL_CONTACT_INFO = 4,
    SHOW_NULL = 5,
};

#endif // __TALKLOGIC_COMMON_H__
