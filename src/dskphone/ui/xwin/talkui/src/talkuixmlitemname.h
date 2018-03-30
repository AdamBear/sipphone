#ifndef _TALKUI_XML_ITEM_NAME_H
#define _TALKUI_XML_ITEM_NAME_H

namespace talkui
{
//talkmenu item共用文本显示控件
#define TALKUI_NORMAL_ITEM_TEXT "showText"

//////////////////talkframe///////////////
#define TALKUI_UI_FRAME "dlgtalkuiframe"
//有dsskey时talk背景区域
#define TALKUI_UI_BACKGROUND "dlgframebackground"
//有dsskey时子界面可用区域
#define TALKUI_UIFRAME_NORMAL "dlgframelayoutnormal"
//无dsskey时界面区域
#define TALKUI_UIFRAME_FULLSCREEN "dlgframefullscreen"
//////////////////talkframe///////////////

//////////////////titlebar///////////////
//titlebar 名称
#define TALKUI_TITLEBAR_ID "talkuititlebar"
#define TALKUI_MENUBAR_ID "menutitle"

//通道图标
#define TALKUI_TITLEBAR_CHANNELICON_ID "channelicon"
//通话Title
#define TALKUI_TITLEBAR_TITLECONTENT_ID "titlecontent"
//其他通话图标
#define TALKUI_TITLEBAR_ICON_ID "talkicon"
//显示的页码
#define TALKUI_TITLEBAR_PAGENUM_ID "pagenum"

//录制的图标+时间的整体
#define TALKUI_TITLEBAR_RECORD_WIDGET "talkrecord"
//录制的图标
#define TALKUI_TITLEBAR_RECORD_ICON "talkrecordicon"
//录制的时间
#define TALKUI_TITLEBAR_RECORD_TIME "talkrecordtime"
//////////////////titlebar///////////////

//////////////////menu titlebar///////////////
#define TALKUI_TITLEBAR_MENU_TITLECONTENT_ID "menutitlecontent"
#define TALKUI_TITLEBAR_MENU_PAGENUM_ID "menupagenum"
//////////////////menu titlebar///////////////

//talkuiframe 的滚动条
#define TALKUI_FRAME_SCROLLBAR "dlgtalkuiframescrollbar"

//dtmf条
#define TALKUI_DTMF "dtmfedit"
//网络掉线提示
#define TALKUI_NETWORK_HINT "networknote"
//ccstatus提示
#define TALKUI_CCSTATUS_HINT "ccstatuslabel"

//////////////////拨号界面控件ID///////////////
//拨号界面编辑框
#define TALKUI_DIAL_LINEEDIT "diallineedit"
#define TALKUI_DIAL_SEARCHLIST "searchlist"
#define TALKUI_DIAL_SEARCHLIST_SCROLLBAR "scrollCtrl"

#define TALKUI_DIAL_SEARCH_ITEM_NAME "textName"
#define TALKUI_DIAL_SEARCH_ITEM_NUMBER "textNum"
//////////////////拨号界面控件ID///////////////

//////////////////talk公用///////////////
#define TALKUI_TALK_BIG_HINT "bighint"
#define TALKUI_TALK_HINT_1 "hint1"
#define TALKUI_TALK_HINT_2 "hint2"
#define TALKUI_TALK_HINT_3 "hint3"
#define TALKUI_TALK_TALK_TIME "talktimepiece"

#define TALKUI_TALK_CONTACT_IMAGE_FRAME "contactimageframe"

//联系人头像
#define TALKUI_TALK_CONTACT_IMAGE "contactimage"
//hold、mute等状态下的半透明灰色遮罩
#define TALKUI_TALK_CONTACT_COVER "contactcover"
//T54T52类型将图片显示出圆形的遮罩
#define TALKUI_TALK_CONTACT_MASK "contactimagemask"

#define TALKUI_TALK_STATUS_FRAME "statusframe"
#define TALKUI_TALK_STATUS_ICON "statusicon"
#define TALKUI_TALK_STATUS_HINT "statushint"

#define TALKUI_TALK_HD_ICON "hdicon"
//////////////////talk公用///////////////

//////////////////talking///////////////
#define TALKUI_TALK_NO_CONTACT_STATUS_FRAME "nocontactstatusframe"
#define TALKUI_TALK_NO_CONTACT_STATUS_ICON "nocontactstatusicon"
#define TALKUI_TALK_NO_CONTACT_STATUS_HINT "nocontactstatushint"

#define TALKUI_TALKING_TRANSFAIL_HINT "transfailhint"

#define TALKUI_TALKING_INCOMING_FRAME "talkincomingframe"

#define TALKUI_TALKING_INCOMING_ANIMATION "talkincomingcallanimation"
#define TALKUI_TALKING_INCOMING_CALL_HINT "talkincomingcallhint"
#define TALKUI_TALKING_INCOMING_CALL_ARROW "talkincomingcallarrow"
#define TALKUI_TALKING_INCOMING_CALL_TARGET "talkincomingcalltarget"
//////////////////talking///////////////

//////////////////ringing///////////////
#define TALKUI_RINGING_INFO_FRAME "ringinginfoframe"

#define TALKUI_RINGING_ARROW "ringingarrow"
#define TALKUI_RINGING_TARGET "ringingtarget"
#define TALKUI_RINGING_ARROW_RIGHT "ringingarrowright"

#define TALKUI_RINGING_TARGET_NUMBER "ringingtargetnumber"

#define TALKUI_RINGING_CONTACT_ANIMATION "ringingcontactanimation"
#define TALKUI_RINGING_ANIMATION "ringinganimation"

#define TALKUI_RINGING_MUTE_ICON "muteicon"
//////////////////ringing///////////////

//////////////////connect///////////////
#define TALKUI_CONNECT_CONATCT_ANIMATION "connectcontactanimation"
#define TALKUI_CONNECT_ANIMATION "connectanimation"
//////////////////connect///////////////

//////////////////finish///////////////
#define TALKUI_FINISH_HINT_FRAME "finishhintframe"
#define TALKUI_FINISH_ICON "finishicon"
#define TALKUI_FINISH_HINT "finishhint"
//////////////////finish///////////////

//////////////////auto redial///////////////
#define TALKUI_AUTO_REDIAL_HINT_FRAME "autoredialhintframe"
#define TALKUI_AUTO_REDIAL_ICON "autoredialicon"
#define TALKUI_AUTO_REDIAL_HINT "autoredialhint"
//////////////////auto redial///////////////

//////////////////rtp status///////////////
#define TALKUI_RTPSTATUS_LIST "rtpstatuslist"
#define TALKUI_RTP_SCROLLBAR "rtpscrollbar"
//////////////////rtp status///////////////

//////////////////acd info///////////////
#define TALKUI_ACDINFO_LIST "acdInfolist"
#define TALKUI_ACDINFO_SCROLLBAR "acdscrollbar"
//////////////////acd info///////////////

//////////////////conf manager///////////////
#define TALKUI_CONF_LIST "conflist"
#define TALKUI_CONF_SCROLLBAR "scrollCtrl"
//////////////////conf manager///////////////

//////////////////conf manager item///////////////
//联系人名称
#define TALKUI_CONFMANAGER_ITEM_CONTACTNAME "confmanageritemcontactname"
//呼叫状态的提示语
#define TALKUI_CONFMANAGER_ITEM_CALLINGHINT "confmanageritemcallinghint"
//图标
#define TALKUI_CONFMANAGER_ITEM_STATUSICON "confmanageritemstatusicon"
//////////////////conf manager item///////////////

//////////////////net conf ui///////////////
#define TALKUI_NETCONF_LIST "netconflist"
#define TALKUI_MEMBER_HINT "memberhint"

#define TALKUI_MEMBER_HINT_FRMAE "netconfmemberhintframe"
//////////////////net conf ui///////////////

/////////////talk menu////////
#define TALKUI_MENU_LIST "talkmenulist"

}       //namespace talkui


#endif
