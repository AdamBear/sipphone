#ifndef _TALKUILOGIC_ACTION_DEFINE_H
#define _TALKUILOGIC_ACTION_DEFINE_H

#include "talkuilogic/talkuilogiccondition.h"

#ifndef IME_ACTION
#define IME_ACTION "IME"
#endif

#ifndef IME_DEL_ACTION
#define IME_DEL_ACTION    "IMEDELETE"
#endif

//Talk UI Action Identity，主要为TalkUI内部用于统一处理的Action字符串
#define TALK_UI_ACTION_IME              IME_ACTION          // IME切换功能
#define TALK_UI_ACTION_DELETE           IME_DEL_ACTION  // 删除字符功能
#define TALK_UI_ACTION_LINE             "Line"          // 选择账号功能
#define TALK_UI_ACTION_POOL             "Pool"          // Pool功能
#define TALK_UI_ACTION_TEXT_CHANGE      "TextChange"    // 号码改变
#define TALK_UI_ACTION_SEND             "Send"          // 呼出   
#define TALK_UI_ACTION_REDIAL           "Redial"        // 重拨
#define TALK_UI_ACTION_CANCEL           "Cancel"        // 退出(X键)

// Audio-Video Switch
#define TALK_UI_ACTION_ADIAL            "AudioDial"
#define TALK_UI_ACTION_AUDIO            "Audio"
#define TALK_UI_ACTION_VIDEO            "Video"
#define TALK_UI_ACTION_START_SHARING    "StartSharing"
#define TALK_UI_ACTION_STOP_SHARING     "StopSharing"
#define TALK_UI_ACTION_UPGRADE_VIDEO    "UpgradeVideo"

//!!!!!, 为何多定义出这个
#define TALK_UI_ACTION_ENDCALL          "ENDCALL"       // 退出（FN按键）
#define TALK_UI_ACTION_NEWCALL          "New Call"      // 创建新通话，进入拨号界面
#define TALK_UI_ACTION_ANSWER           "Answer"        // 接听来电
#define TALK_UI_ACTION_REJECT           "Reject"        // 拒绝来电
#define TALK_UI_ACTION_IGNORE           "Ignore"        // 忽略来电
#define TALK_UI_ACTION_FORWARD          "Forward"       // 转移来电
#define TALK_UI_ACTION_SWAP             "Swap"          // 两路通话切换
#define TALK_UI_ACTION_HOLD             "Hold"          // 通话中Hold
#define TALK_UI_ACTION_PRIHOLD          "PriHold"       // 通话中 Private Hold
#define TALK_UI_ACTION_BACK             "Back"          //返回  confmanger返回  POOL 界面返回
#define TALK_UI_ACTION_RESUME           "Resume"        // 通话中unhold
#define TALK_UI_ACTION_MUTE_VOICE       "Mute Voice"    // 通话中Mute发送声音
#define TALK_UI_ACTION_TRANSFER         "Transfer"      // Transfer
#define TALK_UI_ACTION_FAIL_TRANSFER    "Fail_Transfer" // Transfer Fail的时候的Transfer
#define TALK_UI_ACTION_CONFERENCE       "Conference"    // Conference
#define TALK_UI_ACTION_SPLIT            "Split"         // 拆开会议 （单路）
#define TALK_UI_ACTION_RESIZE_VIDEO     "Resize Video"  // Resize视频大小
#define TALK_UI_ACTION_STOP_VIDEO       "Stop Video"    // 停止本地视频
#define TALK_UI_ACTION_START_VIDEO      "Start Video"   // 开启本地视频
#define TALK_UI_ACTION_OPENDOOR         "Open Door"     // DoorPhone开门
#define TALK_UI_ACTION_ENTER                        "Enter"     // Pool使用

#if IF_BUG_23548
#define TALK_UI_ACTION_CALL_BACK        "Call Back"
#endif
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
#define TALK_UI_ACTION_CONF_REMOVE      "Conf Remove"
#define TALK_UI_ACTION_CONF_FARMUTE     "Conf Far Mute"      //
#define TALK_UI_ACTION_CONF_UNMUTE      "Conf Unmute"        //
#define TALK_UI_ACTION_ACD_CALL_INFO    "Call Info"     //ACD Call Info
#define TALK_UI_ACTION_ACD_DISPCODE     "DispCode"      //ACD Disposition Code
#define TALK_UI_ACTION_ACD_TRACE        "ACDTrace"      //ACD Trace
#define TALK_UI_ACTION_ACD_EMERGESCAL   "ACDEmergEscal" //ACD EmergEscal
#define TALK_UI_ACTION_EXIT         "Exit"          //acdcallinfo 界面退出
#define TALK_UI_ACTION_DPICKUP          "DPickup"
#define TALK_UI_ACTION_GPICKUP          "GPickup"
#define TALK_UI_ACTION_RTP_STATUS       "RTP Status"
#define TALK_UI_ACTION_RTP_STATUS_SWITCH        "RTP Status Switch"
#define TALK_UI_ACTION_SELECT "Select"
#define TALK_UI_ACTION_DECLINE          "Decline"       // Decline,所有设备挂断
#define TALK_UI_ACTION_DECLINE_IGNORE   "Decline_Ignore"// Decline 场景 忽略来电

//pickupmenu界面特殊使用
#define TALK_UI_ACTION_PICKUP           "pickup"
#define TALK_UI_ACTION_DIAL "dial"

//scamenu界面特殊使用 action
#define TALK_UI_ACTION_SCAMENU_BARGEIN "bargein"
#define TALK_UI_ACTION_SCAMENU_CALLPULL "callpull"
#define TALK_UI_ACTION_SCAMENU_RETRIEVE "scamenuretrieve"
#define TALK_UI_ACTION_SCAMENU_SILENTBARGEIN "scamenusilentbargein"
#if IF_BUG_23548
#define TALK_UI_ACTION_CALL_BACK        "Call Back"
#endif

//录音使用
//开始录音
#define TALK_UI_ACTION_USB_BEGIN_RECORD "usbbeginrecord"
//停止录音
#define TALK_UI_ACTION_USB_STOP_REDORD "usbstoprecord"
//暂停录音
#define TALK_UI_ACTION_USB_PAUSE_REDORD "usbpauserecord"
//恢复录音
#define TALK_UI_ACTION_USB_RESUME_REDORD "usbresumerecord"

//截图
#define TALK_UI_ACTION_SCREENSHOT "screenshot"

#define TALK_UI_ACTION_RECORD               "Record"
#define TALK_UI_ACTION_STARTRECORD          "Start Record"
#define TALK_UI_ACTION_STOPRECORD           "Stop Record"
#define TALK_UI_ACTION_PAUSERECORD          "Pause Record"
#define TALK_UI_ACTION_RESUMERECORD         "Resume Record"
#define TALK_UI_ACTION_BACKRECORD                   "Record Back"
//invite
#define TALK_UI_ACTION_INVITE "invite"
#define TALK_UI_ACTION_MEMBER "member"
#define TALK_UI_ACTION_SECURITY "Security"

#define TALK_UI_ACTION_FLASH "Flash"

#define TALK_UI_ACTION_JOIN "Join"
#define TALK_UI_ACTION_SPLIT_ALL            "SplitAll"          // 拆开会议 （全部）
#define TALK_UI_ACTION_FARHOLD          "FarHold"
#define TALK_UI_ACTION_FARUNHOLD        "FarUnHold"

#define TALK_UI_ACTION_DIRECT       "Direct"
#define TALK_UI_ACTION_CONSULT      "Consult"

#endif      //_TALKUILOGIC_ACTION_DEFINE_H
