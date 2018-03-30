#ifndef __TALK_UI_DEFINE_H__
#define __TALK_UI_DEFINE_H__

//mod-QT
#include <QtGui>
#include "uicommon.h"

#define DLG_BaseTalkUI              "CDlgBaseTalkUI"

//Talk UI Action Identity，主要为TalkUI内部用于统一处理的Action字符串
#define TALK_UI_ACTION_IME              IME_ACTION          // IME切换功能
#define TALK_UI_ACTION_DELETE           IME_DEL_ACTION  // 删除字符功能
#define TALK_UI_ACTION_LINE             "Line"          // 选择账号功能
#define TALK_UI_ACTION_POOL             "Pool"          // Pool功能
#define TALK_UI_ACTION_TEXT_CHANGE      "TextChange"    // 号码改变
#define TALK_UI_ACTION_SEND             "Send"          // 呼出   
#define TALK_UI_ACTION_REDIAL           "Redial"        // 重拨
#define TALK_UI_ACTION_CANCEL           "Cancel"        // 退出(X键)
//!!!!!, 为何多定义出这个
#define TALK_UI_ACTION_ENDCALL          "ENDCALL"       // 退出（FN按键）
#define TALK_UI_ACTION_NEWCALL          "New Call"      // 创建新通话，进入拨号界面
#define TALK_UI_ACTION_ANSWER           "Answer"        // 接听来电
#define TALK_UI_ACTION_REJECT           "Reject"        // 拒绝来电
#define TALK_UI_ACTION_IGNORE           "Ignore"        // 忽略来电
#define TALK_UI_ACTION_DECLINE          "Decline"       // Decline,所有设备挂断
#define TALK_UI_ACTION_DECLINE_IGNORE   "Decline_Ignore"// Decline 场景 忽略来电
#define TALK_UI_ACTION_FORWARD          "Forward"       // 转移来电
#define TALK_UI_ACTION_SWAP             "Swap"          // 两路通话切换
#define TALK_UI_ACTION_HOLD             "Hold"          // 通话中Hold
#define TALK_UI_ACTION_PRIHOLD          "PriHold"       // 通话中 Private Hold
#define TALK_UI_ACTION_RESUME           "Resume"        // 通话中unhold
#define TALK_UI_ACTION_MUTE_VOICE       "Mute Voice"    // 通话中Mute发送声音
#define TALK_UI_ACTION_TRANSFER         "Transfer"      // Transfer
#define TALK_UI_ACTION_BTRANSFER            "B Transfer"        // Blind Transfer
#define TALK_UI_ACTION_SECURITY         "Security"      //查看通话安全等级
#define TALK_UI_ACTION_FAIL_TRANSFER    "Fail_Transfer" // Transfer Fail的时候的Transfer
#define TALK_UI_ACTION_CONFERENCE       "Conference"    // Conference
#define TALK_UI_ACTION_SPLIT            "Split"         // 拆开会议 
#define TALK_UI_ACTION_RESIZE_VIDEO     "Resize Video"  // Resize视频大小
#define TALK_UI_ACTION_STOP_VIDEO       "Stop Video"    // 停止本地视频
#define TALK_UI_ACTION_START_VIDEO      "Start Video"   // 开启本地视频
#define TALK_UI_ACTION_OPENDOOR         "Open Door"     // DoorPhone开门
#define TALK_UI_ACTION_OPENFAVORITE     "OpenPage Favorite"     // 打开Favorite界面
#define TALK_UI_ACTION_OPENHISTORY      "OpenPage History"      // 打开History界面
#define TALK_UI_ACTION_OPENDIRECTORY    "OpenPage Directory"    // 打开Directory界面
#define TALK_UI_ACTION_OPEN_REMOTE_DIRECTORY    "OpenPage Remote Directory" // 打开Remote Directory界面
#define TALK_UI_ACTION_OPEN_LDAP_DIRECTORY  "OpenPage LDAP Directory"   // 打开LDAP Directory界面
#define TALK_UI_ACTION_OPEN_BST_DIRECTORY   "OpenPage BST Directory"    // 打开BST Directory界面
#define TALK_UI_ACTION_OPEN_BST_HISTORY "OpenPage BST History"  // 打开BST History界面
#define TALK_UI_ACTION_OPENDSSKEY       "OpenPage Dsskey"       // 打开Dsskey界面
#define TALK_UI_ACTION_AUTOREDIAL       "AutoRedial"            //进入Auto Redial
#define TALK_UI_ACTION_OK               "OK" // 按OK键
#define TALK_UI_ACTION_SMS              "SMS" // 按OK键 //!!!!!
#define TALK_UI_ACTION_CONF_MGR         "ConfMgr" // T46进入ConferenceManger界面
#define TALK_UI_ACTION_PARK             "Park"          // Park当前通话
#define TALK_UI_ACTION_GROUP_PARK       "GroupPark"     // GroupPark当前通话
#define TALK_UI_ACTION_RETRIEVE_PARK    "RetrievePark"  // 取回被Park的通话
#define TALK_UI_ACTION_SWITCH           "Switch"        // 切换话路
//!!!!! 添加相关的注释
#define TALK_UI_ACTION_REMOVE           "Remove"        //
#define TALK_UI_ACTION_FARMUTE          "Far Mute"      //
#define TALK_UI_ACTION_UNMUTE           "Unmute"        //
#define TALK_UI_ACTION_ACD_CALL_INFO    "Call Info"     //ACD Call Info
#define TALK_UI_ACTION_ACD_DISPCODE     "DispCode"      //ACD Disposition Code
#define TALK_UI_ACTION_ACD_TRACE        "ACDTrace"      //ACD Trace
#define TALK_UI_ACTION_ACD_EMERGESCAL   "ACDEmergEscal" //ACD EmergEscal
#define TALK_UI_ACTION_DPICKUP          "DPickup"
#define TALK_UI_ACTION_GPICKUP          "GPickup"
#define TALK_UI_ACTION_RTP_STATUS       "RTP Status"
#define TALK_UI_ACTION_RTP_STATUS_SWITCH        "RTP Status Switch"
#if IF_BUG_23548
#define TALK_UI_ACTION_CALL_BACK        "Call Back"
#endif
#define TALK_UI_ACTION_CALL_INITIATION  "Call Initiation"
#define TALK_UI_ACTION_CALL_PUSH        "Call Push"

#define TALK_UI_ACTION_RECORD               "Record"
#define TALK_UI_ACTION_STARTRECORD          "Start Record"
#define TALK_UI_ACTION_STOPRECORD           "Stop Record"
#define TALK_UI_ACTION_PAUSERECORD          "Pause Record"
#define TALK_UI_ACTION_RESUMERECORD         "Resume Record"
#define TALK_UI_ACTION_BACK                 "Back"

//USB开始录音
#define TALK_UI_ACTION_USB_BEGIN_RECORD         "beginrecord"
//USB停止录音
#define TALK_UI_ACTION_USB_STOP_REDORD          "stoprecord"
//USB暂停录音
#define TALK_UI_ACTION_USB_PAUSE_RECORD         "pauserecord"
//USB停止录音
#define TALK_UI_ACTION_USB_RESUME_REDORD        "resumerecord"

#define TALK_UI_ACTION_FARHOLD          "Far Hold"      // 通话中FarHold

// 拨号界面弹出全键盘
#define DIALPAD_SHOW_KEYPAD "showkeypad"

//提示区域类型
//主要由于通话中在界面靠下位置用于显示各种提示，但由于会有多种提示出现，因此区分类型
//规则定义如下：
//              DTMF    VolumeBar   Network     Custom
//Dial          No      Yes         No          No
//Ring          No      Yes         Yes         Animation
//Connect       No      Yes         Yes         Animation
//VoiceTalk     Yes     Yes         Yes         Transfer Failed
//VideoTalk     Yes     Yes         Yes         Transfer Failed
//FullTalk      No      No          Yes         Transfer Failed
//VoiceConf     Yes     Yes         Yes         No
//VideoConf     No      No          Yes         No
//MixConf       Yes     Yes         Yes         No
//Finished      No      Yes         No          No
enum TALK_TIP_TYPE
{
    TALK_TIP_UNKNOWN = -1,      //未知类型
    TALK_TIP_DTMF,              //DTMF类型
    TALK_TIP_VOLUMEBAR,         //音量条类型
    TALK_TIP_NETWORK,           //网络提示类型
    TALK_TIP_CUSTOM,            //各子界面自定义
    TALK_TIP_CCSTATUS,          //显示CallCenter状态
};

//通话UI的类型，用于区分各类型UI，以便设置位置与参数
enum TALK_UI_TYPE
{
    TALK_UI_UNKNOWN = -1,       //未知类型
    TALK_UI_NORMAL_DIAL,        //拨号类型
    TALK_UI_FULL_DIAL,          //全屏拨号类型
    TALK_UI_CONNECT,            //连接回铃类型
    TALK_UI_RING,               //来电响铃类型
    TALK_UI_RING_IN_CONF,       //会议中来电类型
    TALK_UI_VOICE_TALK,         //音频通话类型
    TALK_UI_VOICE_CONF,         //音频会议类型
    TALK_UI_FINISHED,           //通话结束类型
    TALK_UI_PARK_NOTIFY,        //Park提示界面
};

//回调的参数类型
enum TALKWIDGET_CALLBACK_TYPE
{
    //Dialing
    TCT_DIALING_BEGIN = 0x00001000,

    TCT_DIALING_DELETE,         //DELETE 动作
    TCT_DIALING_DPICKUP,        //DPICKUP 动作
    TCT_DIALING_GPICKUP,        //GPICKUP 动作
    TCT_DIALING_SEND,           //SEND 动作
    TCT_DIALING_OPENDSSKEY,     //打开DSSKEY
    TCT_DIALING_TEXTCHANGE,     //编辑框字符变化
    TCT_DIALING_TRANSFERBYCALLID,   //通过通话ID transfer 通话

    //ConferenceManager
    TCT_CONFERENCEMANAGER_BEGIN = 0x00008000,
    TCT_CONFERENCE_FARMUTE0,    //FARMUTE 动作 并且是UnMute
    TCT_CONFERENCE_FARMUTE1,    //FARMUTE 动作 并且是Mute
    TCT_CONFERENCE_REMOVE_CONF_PARTY, //REMOVE CONF PARTY 移除通话方操作


};


//测试宏定义
//#define TALKUI_DEBUG_TEST

#ifdef _T48
#define MAX_CONF_PARTY_NUM              2       // 可以设置的最大联系人数量
#else
#ifdef _T4X
#define MAX_CONF_PARTY_NUM              4       // 可以设置的最大联系人数量
#else
#define MAX_CONF_PARTY_NUM              2       // 可以设置的最大联系人数量
#endif // _T4X
#endif // _T48

#define MSGBOX_ID_SECURITY  100
#define MSGBOX_ID_REMOVE    101

#endif //__TALK_UI_DEFINE_H__
