#ifndef _TALKUILOGIC_CONDITION_H
#define _TALKUILOGIC_CONDITION_H

//test 测试
//开始测试为1  没开启测试为0
#define TALKUI_DEBUG_TEST 0
//test 测试结束

//会议人员数量
#define MAX_CONF_PARTY_NUM 4

//是否显示dsskey
#define IF_TALKUI_DSSKEY 0

//是否有显示头像功能
#define IF_TALKUI_CONTACT_PHOTO 0

#if IF_TALKUI_CONTACT_PHOTO
//头像是否有动画（头像动画显示时，另一个动画不显示）
#define IF_TALKUI_CONTACT_PHOTO_ANIMATION    0
#endif

//是否有绘制背景功能
#define IF_TALKUI_BG_PICTURE 0

//通话背景是否是与idle相同的风格
#define IF_TALKUI_BG_IDLE_STYLE 0

//拨号背景绘制方式
#define IF_TALKUI_BIG_DIAL_BG 0

//全屏类型界面是否需要单独绘制背景
#define IF_TALKUI_DIFF_BG 0

//是否存在多个账号
#define IF_TALKUI_MULTY_ACCOUNT 0

//只有一路通话时是否显示页码
#define IF_TALKUI_ONE_PAGE_SHOW_NOTE 1

//是否支持蓝牙
#ifdef IF_SUPPORT_BLUETOOTH
#define IF_TALKUI_BLUETOOTH_ENABLE 1
#else
#define IF_TALKUI_BLUETOOTH_ENABLE 0
#endif          //ifdef IF_SUPPORT_BLUETOOTH

//是否支持蓝牙手机
#define IF_TALKUI_BLUETOOTH_BT_ENABLE 1

//是否支持wifi
#ifdef IF_SUPPORT_WIFI
#define IF_TALKUI_WIFI_ENABLE 1
#else
#define IF_TALKUI_WIFI_ENABLE 0
#endif          //ifdef IF_SUPPORT_WIFI

//是否支持USB
#if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)
#define IF_TALKUI_USB_ENABLE 1
#else
#define IF_TALKUI_USB_ENABLE 0
#endif          //if (defined IF_USB_RECORD) && (defined IF_USB_SUPPORT)

// IF_TALKUI_PSTN
#ifndef IF_TALKUI_PSTN
#ifdef IF_FEATURE_PSTN
#define IF_TALKUI_PSTN 1
#else
#define IF_TALKUI_PSTN 0
#endif //IF_FEATURE_PSTN
#endif
//IF_TALKUI_PSTN

//是否支持PC Mode
// IF_TALKUI_LINE_DEVICE
#ifndef IF_TALKUI_LINE_DEVICE
#ifdef IF_SUPPORT_LINE_DEVICE
#define IF_TALKUI_LINE_DEVICE 1
#else
#define IF_TALKUI_LINE_DEVICE 0
#endif //IF_SUPPORT_LINE_DEVICE
#endif
//IF_TALKUI_LINE_DEVICE

//是否支持五方会议
// IF_TALKUI_FIVE_PARTY
#ifndef IF_TALKUI_FIVE_PARTY
#ifdef IF_SUPPORT_FIVE_PARTY
#define IF_TALKUI_FIVE_PARTY 1
#else
#define IF_TALKUI_FIVE_PARTY 0
#endif //IF_SUPPORT_FIVE_PARTY
#endif
//IF_TALKUI_FIVE_PARTY

// IF_TALKUI_EXECUTIVE
#ifndef IF_TALKUI_EXECUTIVE
#ifdef IF_FEATURE_BROADSOFT_EXECUTIVE_ASSISTANT
#define IF_TALKUI_EXECUTIVE 1
#else
#define IF_TALKUI_EXECUTIVE 0
#endif //IF_TALKUI_EXECUTIVE
#endif
//IF_TALKUI_EXECUTIVE

//免提键仅用于拨号，常见于CP
// IF_TALKUI_HANDFREE_DIAL
#define IF_TALKUI_HANDFREE_DIAL 1

//挂机是否退出通话，常见于CP
#define IF_TALKUI_ONHOOK_EXITCALL 1

//挂机只退出conf manager而不挂断通话，常见于CP
#define IF_TALKUI_ONHOOK_EXIT_CONFMANAGER 1

//拨号界面收到账号改变的消息是否呼出，常见于CP
#define IF_TALKUI_DIAL_CHANGEACCOUNT_CALLOUT 1

#include "ylvector.h"
#include "yllist.h"
#define talkui_vector YLVector
#define talkui_list YLList

//incoming call信息是否合并在一个文本中
#define IF_TALKUI_INCOMING_CALL_COMBINE  1
//hold、mute信息是否与talk公用控件位置冲突
#define IF_TALKUI_CONFLICT_STYLE        1
//trans fail信息是否合在一行中显示
#define IF_TALKUI_TRANS_FAIL_INFO_IN_ONE_LINE        1

//autoredial信息是否能完整显示下
#define IF_AUTO_REDIAL_SHOW_FULL     1

//通话界面显示行数变化时，字体大小是否需要发生变化
#define IF_TALKUI_FONT_SIZE_CHANGE      1

#if IF_TALKUI_FONT_SIZE_CHANGE
//是否在变化为两行时就改变字体大小，否则为三行
#define IF_TALKUI_FONT_SIZE_CHANGE_WHEN_TWO 0

#define RINGINGUI_FONT_SIZE_BIG         20
#define RINGINGUI_FONT_SIZE_SMALL   9

#define RINGINGUI_MARGIN_BOTTOM_BIG         6
#define RINGINGUI_MARGIN_BOTTOM_SMALL   0
#endif


//来电界面的目标信息中名字和号码是否分开显示
#define IF_TALKUI_RINGUI_TARGET_SPLIT      0

//HD（AMS、OPUS）图标是否单独显示在通话界面里，而不是title上
#define IF_HD_ICON_DEPART           0

//finish提示信息是否在超长的时候需要换行
#define IF_FINISH_HINT_WRAP          0

//在通话管理界面的item中显示的HD（AMS、OPUS）图标是否使用特殊的图标
#define IF_CONF_MANAGER_ITEM_HD_SPECIAL       0

//title是否使用固定样式的图标列表
#define IF_TITLE_ICON_FIXED          0

//网络会议界面的显示方式：0、显示成员列表；1、仅显示member
#define IF_NETCONF_UI_STYLE     1

//transfer fail 时，提示信息是否无视头像显示状态进行显示
#define IF_TRANSFER_FAIL_HINT_STYLE     0

//会议中安全等级提示是否显示在title
#define IF_CONF_SECURITY_IN_TITLE           0

//调试时使用测试数据 宏定义
//非调试模式  需要把宏注释
//#define IF_TALKUI_DEBUG

#endif
