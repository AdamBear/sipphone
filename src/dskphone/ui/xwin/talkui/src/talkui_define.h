#ifndef __TALK_UI_DEFINE_H__
#define __TALK_UI_DEFINE_H__

//#include "uimanager/rectdefine.h"
#include "uimanager/uimanager_common.h"

namespace talkui
{

#define WIDGET_TalkUITitle "talkuititle"

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

// http://10.2.1.199/Bug.php?BugID=76311
//长按时间间隔
#define LONGPRESS_TIME_SPAN 1000

////通话窗口ID 开始
//#define WINDOW_LEVEL_TALKUI_BEGIN 100
//
////xwindow中需要使用的 窗口等级ID
//enum WINDOW_LEVEL_TYPE
//{
//  WINDOW_LEVEL_DLGTALKUIFRAME = WINDOW_LEVEL_TALKUI_BEGIN,    //dlgtalkuiframe
//  WINDOW_LEVEL_TITLEBAR,                                      //titlebar
//  WINDOW_LEVEL_DLGTALKINGUI,                                  //dlgtalkingui
//  WINDOW_LEVEL_DLGDIALINGUI,                              //dlgdialingui
//
//
//};

}       //namespace talkui

#endif //__TALK_UI_DEFINE_H__
