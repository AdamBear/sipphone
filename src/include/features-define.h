#ifndef __FEATURES_DEFINE_H__
#define __FEATURES_DEFINE_H__

// 通用的功能定义
// 诊断功能
//#define IF_FEATURE_DIAGNOSE

// debug模式
#define IF_FEATURE_TOOLS

// 韩语输入法
#define IF_FEATURE_KOREANIME

// 双耳麦
#define IF_FEATURE_DUALHEADSET

// 耳麦优先
// http://10.2.1.199/Bug.php?BugID=21686
#define IF_FEATURE_HEADSET_PRIOR

// 通话中耳麦键是否可用
// http://192.168.1.99/Bug.php?BugID=19141
#define IF_FEATURE_HEADSETKEY_IN_TALK

// tiptel adapter (dsskey/speaker only/mic only)
#define IF_FEATURE_TIPTEL_ADAPTER

// blf
#define IF_FEATURE_BLF

// blflist
#define IF_FEATURE_BLFLIST

// auto blflist
#define IF_FEATURE_AUTOBLFLIST

// acd
#define IF_FEATURE_ACD

// http://192.168.1.99/Bug.php?BugID=26625
// Star2Star T4x Call Center
#define IF_FEATURE_START2START_ACD                  1  // V73补丁版本规划，统一加入中性

// Start2Start ACD 依赖ACD基类
#if IF_FEATURE_START2START_ACD
#ifndef IF_FEATURE_ACD
#define IF_FEATURE_ACD
#endif
#endif

// Genesys Acd
#define IF_FEATURE_GENESYS_ACD

// pushxml & xmlbrowser
#define IF_FEATURE_XMLBROWSER

//加入中性
#define IF_FEATURE_XMLBROWSER_CUSTOM_SOFTKEY    1

// hotdesking
#define IF_FEATURE_HOTDESKING
// #define IF_ADV_HOTDESKING


// 重定向
#define IF_FEATURE_REDIRECTSERVER

// tr069
#define IF_FEATURE_TR069

// broadsoft呼叫记录
#define IF_FEATURE_BROADSOFT_CALLLOG

// broadsoft联系人
#define IF_FEATURE_BROADSOFT_CONTACT

// bla
#define IF_FEATURE_BROADSOFT_SCA

// BSFT基础功能
#define IF_FEATURE_BROADSOFT_BASE

//anyWhere
#define IF_FEATURE_BROADSOFT_ANYWHERE

// CLID
#define IF_FEATURE_BROADSOFT_CLID

// Call Park
#define IF_FEATURE_BROADSOFT_CALLPARK

// Meet-Me Conference
#define IF_FEATURE_BROADSOFT_MEET_ME

// Priority Alert-Simultaneous Ringing Personal
#define IF_FEATURE_BROADSOFT_PRIORITY_ALERT

// Remote Office
#define IF_FEATURE_BROADSOFT_REMOTE_OFFICE

// Call Center
#define IF_FEATURE_BROADSOFT_CALL_CENTER

// BSFT AOC NOTIFY
#define IF_FEATURE_BROADSOFT_AOC_NOTIFY

// AUTOTEST
#define IF_FEATURE_AUTOTEST

// DSSKEY
#define IF_FEATURE_DSSKEY

// RTCP-XR
#define IF_FEATURE_RTCPXR

//IDLESCREEN
#define IF_FEATURE_IDLESCREEN

//#define IF_FEATURE_BROADSOFT_CALL_WAITING

///////////////////////////目前用配置实现的项/////////////////////////////
// PCCW特制:在IDLE界面按FWD按键跳到Fram进行设置
#define IF_FEATURE_ALWAYS_SETFWD

// 是否允许FWD到国际号码，通过前缀识别是否国际号码，前缀可配置，默认为“00”
#define IF_FEATURE_FWD_INTERNATIONAL

// 有多个预先设置好转移号码的DSS key：FWD，当用户需要离开时，按下其中一个DSS key，所有来电就会“Forward”到该DSS key设置的号码
// http://192.168.1.99/Bug.php?BugID=18263
#define IF_FEATURE_MULTIFWD

// 话机第一路通话处于held状态时，收到新一路的Intercom来电，则不向sip发送hold信息
// http://192.168.1.99/Bug.php?BugID=18570
#define IF_FEATURE_NOT_HOLD_INTERCOM_CALL

// Ergocom-HOT T2X需要支持100个dial now；
// 拨号界面会延迟500ms匹配dial now号码，但是有的客户（Joyce）要求dialnow立即呼出
// http://192.168.1.99/Bug.php?BugID=19722
#define IF_FEATURE_100_DIANNOW

// Ergocom Cosmocom特制：GUI在话机上第一次运行时,会将第一个账号用z9hG4bK(可配置)+话机MAC填充
// UserName, AuthName,以后除非是恢复出厂配置或者升级固件,不然不会再次进行以上步骤
// http://192.168.1.99/Bug.php?BugID=4691
#define IF_FEATURE_AUTO_CONFIG_ACCOUNT

// wildix：需要删除任何跟Black list有关的东西；可配置
// http://192.168.1.99/Bug.php?BugID=19134
#define IF_FEATURE_SHOW_BLACKLIST

// Wildix：呼出或回铃状态下收到PHONE_MSG_CALLEE_ID消息，不替换显示名称
#define IF_FEATURE_NOT_REPLACE_NAME   // 未实现

// YUK：拨号界面*键只能输出*
#define IF_FEATURE_STAR_ONLY_IN_DIAL

// Joyce：匹配Dialnow号码后立即呼出
#define IF_FEATURE_DIALNOW_IMMEDIATELY

// Ergcom:自动应答提示应，每秒响2声，自动应答时间间隔可从网页配置，默认为3s
// http://10.2.1.199/Bug.php?BugID=21687
#define IF_FEATURE_SPECIAL_AATONE

// Nology-南非电信:如果连续收到2个183，则将第二个回复的183转换成180处理
// http://192.168.1.99/Bug.php?BugID=19362
#define IF_FEATURE_CHANGE183TO180

// Intelbras T60/T65 耳机按键特制
// http://192.168.1.99/Bug.php?BugID=11499
// http://192.168.1.99/Bug.php?BugID=19088
#define IF_FEATURE_HEADSET_AS_CANCEL

//是否支持autop上传自定义语言文件
#define IF_FEATURE_USER_LANGUAGE

//是否支持绑定UME功能宏
#define IF_SUPPORT_UME

// 临时测试模式版本，TouchCheck测试完画红线，测试校准准确度
//#define IF_FEATURE_TOUCHCHECK

//#define IF_BUG_23550_Pingan_Menu  0

#define SUPPORT_SMS     0

//是否支持自动翻译
#define IF_SUPPORT_AUTO_TRANSLATE   1

//翻译扩展台
#define IF_FEATURE_TRANSLATE_EXP

// Call Recording
#define IF_FEATURE_BROADSOFT_CALLRECORDING

// R20 Executive-Assistant
#define IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT

// 紧急呼叫
#define IF_FEATURE_EMERGENCY
// DigitMap
#define IF_FEATURE_DIGIT_MAP

#define IF_FEATURE_CLOUD_ACCOUNT    0

#define IF_FEATURE_AOC				0

#ifdef  _FILTER_FUNCTIONS_
#define DECLARE_ATTR __attribute__((__no_instrument_function__))
#else
#define DECLARE_ATTR
#endif
////////////////////////////////////////////////////////
// 最后再include：功能冲突项
//!!!!! 暂时注释 #include "conflictfeatures.h"

#if IS_COLOR
#define PHOTO_SHOW

#endif

//#define IF_FEATURE_BROADSOFT_CALL_FORWARD_NOT_REACHABLE
//--------------------------------------------------------------------------------[TYN BT  客户]
//http://192.168.1.99/Bug.php?BugID=26353
//TYN BT T28P  要求话机能够开放HTTP报文接口供客户自定义题

//Genband 平台相关功能
//#define IF_FEATURE_GENBAND



////////////////////////////////////////////////////////
#if defined(_CP) || defined(_T49)
// T49/CP 支持五方会议
#define IF_SUPPORT_FIVE_PARTY
#endif // 

#ifdef  _CP
// 是否支持多模
#define IF_SUPPORT_LINE_DEVICE

// 向左和向右两个切换按键
#define IF_FEATURE_LEFT_RIGHT_KEY

// 历史记录菜单
#define IF_FEATURE_HISTORY_MENU

// 单色灯
#define IF_FEATURE_ONE_STATUS_LED

// 开机设置向导
#define IF_SETTING_WIZARD


// PSTN Box功能
#define IF_FEATURE_PSTN         1
// PSTN Box的DND功能
//#define IF_FEATURE_PSTN_DND   1
// 【临时措施！】PSTN Box识别容错：对于快速拔插PSTN Box进行防抖容错，避免提示PSTN账号
#define IF_PSTN_DETECT_FAULT_TOLERANCE  1

// 是否支持Micpod扩展设备
#define IF_SUPPORT_MICPOD

#endif


#if defined(_T46) || defined(_T29)
// T46 V81新需求界面标题栏显示当前使用的分机号
#define IF_FEATURE_T46_NEW_TITLE                1

#define IF_FEATURE_T46_LINEKEY_LABEL            1
#endif

// Edk功能
#define IF_FEATURE_EDK 1

#define IF_FEATURE_SHOW_DEFAULT_ACCOUNT         1

// http://10.2.1.199/Bug.php?BugID=89686
#define IF_FEATURE_PPPOE                        0

#define IF_FEATURE_KTS                          1

#define IF_FEATURE_STATION_NAME                 1

#if IF_FEATURE_KTS
#   ifndef IF_FEATURE_INCOMEING_SUPPORT_NEWCALL
#       define IF_FEATURE_INCOMEING_SUPPORT_NEWCALL
#   endif
#   ifndef IF_FEATURE_DIAL_ON_HOLD
#       define IF_FEATURE_DIAL_ON_HOLD
#   endif
#endif

#endif // __FEATURES_DEFINE_H__

