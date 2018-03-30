#ifndef __FEATURES_DEFINE_H__
#define __FEATURES_DEFINE_H__

// 通用的功能定义

// 韩语输入法
//#define IF_FEATURE_KOREANIME

// 双耳麦
//#define IF_FEATURE_DUALHEADSET

// 耳麦优先
// http://10.2.1.199/Bug.php?BugID=21686
//#define IF_FEATURE_HEADSET_PRIOR

// 通话中耳麦键是否可用
// http://192.168.1.99/Bug.php?BugID=19141
//#define IF_FEATURE_HEADSETKEY_IN_TALK

// tiptel adapter (dsskey/speaker only/mic only)
//#define IF_FEATURE_TIPTEL_ADAPTER

// blf
//#define IF_FEATURE_BLF

// blflist
// #define IF_FEATURE_BLFLIST

// auto blflist
// #define IF_FEATURE_AUTOBLFLIST

// acd
//#define IF_FEATURE_ACD

// Genesys Acd
//#define IF_FEATURE_GENESYS_ACD

// pushxml & xmlbrowser
//#define IF_FEATURE_XMLBROWSER

// hotdesking
//#define IF_FEATURE_HOTDESKING
// #define IF_ADV_HOTDESKING

// 获取余额
// http://192.168.1.99/Bug.php?BugID=10337
//#define IF_FEATURE_GETBALANCE

// 重定向
#define IF_FEATURE_REDIRECTSERVER

// tr069
#define IF_FEATURE_TR069

// broadsoft呼叫记录
//#define IF_FEATURE_BROADSOFT_CALLLOG

// broadsoft联系人
#define IF_FEATURE_BROADSOFT_CONTACT

// bla
#define IF_FEATURE_BROADSOFT_SCA

// BSFT基础功能
//#define IF_FEATURE_BROADSOFT_BASE

//anyWhere
//#define IF_FEATURE_BROADSOFT_ANYWHERE

// CLID
//#define IF_FEATURE_BROADSOFT_CLID

// Call Park
#define IF_FEATURE_BROADSOFT_CALLPARK

// Meet-Me Conference
//#define IF_FEATURE_BROADSOFT_MEET_ME

// Priority Alert-Simultaneous Ringing Personal
//#define IF_FEATURE_BROADSOFT_PRIORITY_ALERT

// Remote Office
//#define IF_FEATURE_BROADSOFT_REMOTE_OFFICE

// Call Center
//#define IF_FEATURE_BROADSOFT_CALL_CENTER

// BSFT AOC NOTIFY
#define IF_FEATURE_BROADSOFT_AOC_NOTIFY

// AUTOTEST
// #define IF_FEATURE_AUTOTEST

// DSSKEY
// #define IF_FEATURE_DSSKEY

//// RTCP-XR
//#define IF_FEATURE_RTCPXR
//
////IDLESCREEN
//#define IF_FEATURE_IDLESCREEN

///////////////////////////目前用配置实现的项/////////////////////////////
// PCCW特制:在IDLE界面按FWD按键跳到Fram进行设置
//#define IF_FEATURE_ALWAYS_SETFWD

// 是否允许FWD到国际号码，通过前缀识别是否国际号码，前缀可配置，默认为“00”
#define IF_FEATURE_FWD_INTERNATIONAL

// 有多个预先设置好转移号码的DSS key：FWD，当用户需要离开时，按下其中一个DSS key，所有来电就会“Forward”到该DSS key设置的号码
// http://192.168.1.99/Bug.php?BugID=18263
//#define IF_FEATURE_MULTIFWD

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

// Joyce：通话中按OK键的时候将Flash（SIP_MSG_OEM_FLASH）发给SIP处理
#define IF_FEATURE_OK_TO_FLASH

// Intelbras T60/T65 耳机按键特制
// http://192.168.1.99/Bug.php?BugID=11499
// http://192.168.1.99/Bug.php?BugID=19088
#define IF_FEATURE_HEADSET_AS_CANCEL

//是否支持autop上传自定义语言文件
//#define   IF_FEATURE_USER_LANGUAGE

//是否支持绑定UME功能宏
#define IF_SUPPORT_UME

// 临时测试模式版本，TouchCheck测试完画红线，测试校准准确度
//#define IF_FEATURE_TOUCHCHECK

#define IF_BUG_23550_Pingan_Menu    0

#define SUPPORT_SMS     0

#define IF_BUG_22687        0

//是否支持自动翻译
#define IF_SUPPORT_AUTO_TRANSLATE   0

//翻译扩展台
//#define IF_FEATURE_TRANSLATE_EXP

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


#endif // __FEATURES_DEFINE_H__

