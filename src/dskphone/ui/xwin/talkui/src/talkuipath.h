#ifndef _TALKUI_PATH_H
#define _TALKUI_PATH_H

#include "talkuilogic/talkuilogiccondition.h"
#include "baseui/include/t2xpicpath.h"

namespace talkui
{

#define PIC_BASE_PATH "images/"
#define PIC_TALK_CONTACT_PATH PIC_BASE_PATH "contacts/"
#define PIC_TALK_ICON_PATH PIC_BASE_PATH "icon/"

#define PIC_THEME_PATH "Theme1/"

#if IF_TALKUI_CONTACT_PHOTO
//默认联系人头像
#define PIC_TALKUI_DEFAULT_CONTACT_IMAGE    PIC_TALK_CONTACT_PATH "Theme#contacts@default_contact_image.png"

//会议联系人头像
#define     PIC_CONTACTS_CONFERENCE             PIC_TALK_CONTACT_PATH "Theme#contacts@conference.png"

//默认蓝牙联系人头像
#define PIC_CONTACTS_IMAGE_BLUETOOTH_DEFAULT    PIC_TALK_CONTACT_PATH "Theme#contacts@bluetooth_default.png"
#endif

#if IF_TALKUI_BG_PICTURE
//talk
#define PIC_TALK_BACKGROUND_DIALING     PIC_THEME_PATH  "Theme#Theme1@background_dialing.jpg"
#define PIC_TALK_TALKFRAME_BACKGROUND     PIC_GENERAL_PATH  "Theme#general@talk_background.9.png"
#define PIC_DIAL_TALKFRAME_BACKGROUND     PIC_THEME_PATH  "Theme#Theme1@background_dialing.9.png"

#define PIC_TALK_DIAL_SEARCH_FOCUS_ITEM_BG        PIC_THEME_PATH "Theme#Theme1@talk_item_background.9.png"
#endif

#if IF_TALKUI_CONTACT_PHOTO
//联系人头像圆角遮罩
#define     PIC_TALK_ROUNDRECT_MASK             PIC_BASE_PATH "roundrect_mask.png"
#endif

#if IF_TALKUI_BG_PICTURE
//TalkUIFrame 背景
#define     PIC_TALK_FRAME_THUMBNAIL_BACK   PIC_BASE_PATH "talk_frame_thumbnail_back.png"
//TalkUIFrame 背景 缩略模式 不带sessionlist
#define     PIC_TALK_FRAME_THUMBNAIL_NO_SESSIONLIST_BACK    PIC_BASE_PATH "talk_frame_thumbnail_no_sessionlist_back.png"
#endif

#if IF_TALKUI_BG_PICTURE
//拨号界面
#define     PIC_TALK_DIAL_BACKGROUND                                PIC_BASE_PATH "talk_dial_background.png"
//拨号键盘 数字
#define     PIC_TALK_DIAL_KEYPAD_DIGIT                              PIC_BASE_PATH "talk_dial_keypad_digit.png"
//字符
#define     PIC_TALK_DIAL_KEYPAD_CHAR                               PIC_BASE_PATH "talk_dial_keypad_char.png"
//按钮遮罩
#define     PIC_TALK_DIAL_KEYPAD_BTN_MASK                           PIC_BASE_PATH "talk_dial_keypad_btn_mask.png"

//添加新字符背景
#define     PIC_TALK_DIAL_SYMBOLPAD                                 PIC_BASE_PATH "talk_dial_symbolpad.png"
//添加新遮罩
#define     PIC_TALK_DIAL_SYMBOLPAD_BTN_MASK                        PIC_BASE_PATH "talk_dial_symbolpad_btn_mask.png"

//发送按钮遮罩
#define     PIC_TALK_DIAL_SEND_BTN_MASK                             PIC_BASE_PATH "talk_dial_send_btn_mask.png"

//dsskey按钮icon
#define     PIC_TALK_DIAL_DSSKEY_BTN_ICON                           PIC_BASE_PATH "talk_dial_dsskey_btn_icon.png"

//删除按钮
#define     PIC_TALK_DIAL_DELETE_BTN                                PIC_BASE_PATH "talk_dial_btn_delete.png"

//删除按钮
#define     PIC_TALK_DIAL_DELETE_BTN_MASK                           PIC_BASE_PATH "talk_dial_btn_delete_mask.png"


//pickup按钮
#define     PIC_TALK_DIAL_PICKUP_BTN                                PIC_BASE_PATH "talk_dial_btn_pickup.png"

//pickup按钮遮罩
#define     PIC_TALK_DIAL_PICKUP_BTN_MASK                           PIC_BASE_PATH "talk_dial_btn_pickup_mask.png"

//拨号按钮上的图标
//拨号
#define     PIC_TALK_DIAL_ICON_SEND                                 PIC_BASE_PATH "talk_dial_icon_send.png"
//transfer
#define     PIC_TALK_DIAL_ICON_TRANSFER                             PIC_BASE_PATH "talk_dial_icon_transfer.png"
//dpickup
#define     PIC_TALK_DIAL_ICON_DIPCIKUP                             PIC_BASE_PATH "talk_dial_icon_dpickup.png"
//callpark
#define     PIC_TALK_DIAL_ICON_CALLPARK                             PIC_BASE_PATH "talk_dial_icon_callpark.png"
//gpark
#define     PIC_TALK_DIAL_ICON_GPARK                                PIC_BASE_PATH "talk_dial_icon_gpark.png"
//Bluetooth
#define     PIC_TALK_DIAL_ICON_BLUETOOTH                            PIC_BASE_PATH "talk_dial_icon_bluetooth.png"
//forward
#define     PIC_TALK_DIAL_ICON_FORWARD                              PIC_BASE_PATH "talk_dial_icon_forward.png"
//Conference
#define     PIC_TALK_DIAL_ICON_CONFERENCE                           PIC_BASE_PATH "talk_dial_icon_conference.png"
//Conference
#define     PIC_TALK_DIAL_ICON_RETRIEVEPARK                         PIC_BASE_PATH "talk_dial_icon_retrievepark.png"

//拨号界面联系人
//顶部
#define     PIC_TALK_DIAL_CONTACT_CLICK_BG_TOP                      PIC_BASE_PATH "talk_dial_contact_click_bg_top.png"
//中间
#define     PIC_TALK_DIAL_CONTACT_CLICK_BG_MIDDLE                       PIC_BASE_PATH "talk_dial_contact_click_bg_middle.png"
//底部
#define     PIC_TALK_DIAL_CONTACT_CLICK_BG_BOTTOM                       PIC_BASE_PATH "talk_dial_contact_click_bg_bottom.png"

// http://10.2.1.199/Bug.php?BugID=77810
//拨号界面 通话状态联系人
#define     PIC_TALK_DIAL_TALKING_STATUS_CONTACT                        PIC_BASE_PATH "talk_dial_talking_status_contact.png"
#endif

#if IF_TALKUI_BG_PICTURE
//通话子界面背景图
//缩略模式 普通通话的背景图
#define     PIC_TALKUI_BACKGROUND_NORMAL_THUMBNAIL          PIC_BASE_PATH "talkui_background_normal_thumbnail.png"
//缩略模式 普通通话 没有sessionlist的背景图
//使用同一张图片即可
#define     PIC_TALKUI_BACKGROUND_NORMAL_THUMBNAIL_NO_SESSIONLIST           PIC_TALKUI_BACKGROUND_NORMAL_THUMBNAIL
//缩略模式 会议的背景图
#define     PIC_TALKUI_BACKGROUND_CONF_THUMBNAIL            PIC_BASE_PATH "talkui_background_conf_thumbnail.png"
//缩略模式 会议 没有sessionlist的背景图
#define     PIC_TALKUI_BACKGROUND_CONF_THUMBNAIL_NO_SESSIONLIST         PIC_TALKUI_BACKGROUND_CONF_THUMBNAIL
//rtpstatus 界面的背景图
#define     PIC_TALKUI_BACKGROUND_RTPSTATUS         PIC_BASE_PATH "talkui_background_rtpstatus.png"
//rtpstatus 界面的背景图 不带sessionlist
#define     PIC_TALKUI_BACKGROUND_RTPSTATUS_NO_SESSIONLIST          PIC_TALKUI_BACKGROUND_RTPSTATUS

//acdcallinfo 界面的背景图
#define     PIC_TALKUI_BACKGROUND_ACDCALLINFO           PIC_TALKUI_BACKGROUND_RTPSTATUS
//acdcallinfo 界面的背景图 不带sessionlist
#define     PIC_TALKUI_BACKGROUND_ACDCALLINFO_NO_SESSIONLIST            PIC_TALKUI_BACKGROUND_ACDCALLINFO

//pickmenu界面背景
#define     PIC_TALKUI_BACKGROUND_PICKUPMENU            PIC_BASE_PATH "talkui_background_pickupmenu.png"
#endif

#if IF_TALKUI_BG_PICTURE
//提示信息
//录音提示背景
#define     PIC_TALKUI_NOTIFY_RECORDTIME_BG         PIC_BASE_PATH "talk_notify_recordtime_bg.png"
//dtmf提示背景
//dtmf在title的时候背景
#define     PIC_TALKUI_DTMF_ON_TITLE_BG         PIC_BASE_PATH "talk_dtmf_on_title_bg.png"

//dtmf在talkui上的时候背景
#define     PIC_TALKUI_DTMF_ON_TALKUI_BG            PIC_BASE_PATH "talk_dtmf_on_talkui_bg.png"

//rtpstatusitem/pickupmenuitem/scamenuitem 选中背景图片
#define     PIC_TALKUI_FRAME_ITEM_FOCUS_BG          PIC_BASE_PATH "talk_frame_item_focus_bg.png"
#endif

//http://10.2.1.199/Bug.php?BugID=75535
#if IF_TALKUI_BG_PICTURE
//connect界面  取消按钮点击图片
#define     PIC_TALKUI_CONNECT_BUTTON_CANCEL_BACKGROUND_PRESS               PIC_BASE_PATH "talk_connect_button_cancel_background_press.png"
//connect界面 取消按钮点击图标
#define     PIC_TALKUI_CONNECT_BUTTON_CANCEL_ICON_PRESS                     PIC_BASE_PATH "talk_connect_button_cancel_icon_press.png"
//connect界面 取消按钮正常图标
#define     PIC_TALKUI_CONNECT_BUTTON_CANCEL_ICON_NORMAL                    PIC_BASE_PATH "talk_connect_button_cancel_icon_normal.png"

//xwindow 相关
//拨号界面
//拨号界面 搜索框选中
#define PIC_DIAL_SEARCHWIDGET_FOCUS_ITEM_BACKGROUND  PIC_BASE_PATH "dialing_searchwidget_focus_item_background.png"

//finish界面 错误提示图标
//可以写在配置文件里
#define     PIC_TALK_ICON_FINISH_ERROR                              PIC_BASE_PATH "talk_icon_finish_error.png"

//confmanagerui
#define     PIC_TALK_CONF_ITEM_BACKGROUND PIC_BASE_PATH "conf_item_background.png"

/////////select line///////////////
#define     PIC_TALK_SELECTLIEN_ITEM_BACKGROUND PIC_BASE_PATH "item_background.png"

#endif

//动画
#if IF_TALKUI_CONTACT_PHOTO
#define PIC_ANIMATION_PATH       PIC_COMMON_PATH "animation/"
//来电动画
#if IF_TALKUI_CONTACT_PHOTO_ANIMATION
#define     PIC_TALK_ANIMATION_CONTACT_RINGING0                             PIC_ANIMATION_PATH "Theme#animation@scaleincoming_0.png"
#define     PIC_TALK_ANIMATION_CONTACT_RINGING1                             PIC_ANIMATION_PATH "Theme#animation@scaleincoming_1.png"
#define     PIC_TALK_ANIMATION_CONTACT_RINGING2                             PIC_ANIMATION_PATH "Theme#animation@scaleincoming_2.png"
#define     PIC_TALK_ANIMATION_CONTACT_RINGING3                             PIC_ANIMATION_PATH "Theme#animation@scaleincoming_3.png"
#define     PIC_TALK_ANIMATION_CONTACT_RINGING4                             PIC_ANIMATION_PATH "Theme#animation@scaleincoming_4.png"
#define     PIC_TALK_ANIMATION_CONTACT_RINGING5                             PIC_ANIMATION_PATH "Theme#animation@scaleincoming_5.png"
#define     PIC_TALK_ANIMATION_CONTACT_RINGING6                             PIC_ANIMATION_PATH "Theme#animation@scaleincoming_6.png"
#define     PIC_TALK_ANIMATION_CONTACT_RINGING7                             PIC_ANIMATION_PATH "Theme#animation@scaleincoming_7.png"
#define     PIC_TALK_ANIMATION_CONTACT_RINGING8                             PIC_ANIMATION_PATH "Theme#animation@scaleincoming_8.png"
#define     PIC_TALK_ANIMATION_CONTACT_RINGING9                             PIC_ANIMATION_PATH "Theme#animation@scaleincoming_9.png"
#define     PIC_TALK_ANIMATION_CONTACT_RINGING10                             PIC_ANIMATION_PATH "Theme#animation@scaleincoming_10.png"
#define     PIC_TALK_ANIMATION_CONTACT_RINGING11                             PIC_ANIMATION_PATH "Theme#animation@scaleincoming_11.png"
#define     PIC_TALK_ANIMATION_CONTACT_RINGING12                             PIC_ANIMATION_PATH "Theme#animation@scaleincoming_12.png"
#endif

#define     PIC_TALK_ANIMATION_RINGING1                             PIC_ANIMATION_PATH "Theme#animation@incoming1.png"
#define     PIC_TALK_ANIMATION_RINGING2                             PIC_ANIMATION_PATH "Theme#animation@incoming2.png"
#define     PIC_TALK_ANIMATION_RINGING3                             PIC_ANIMATION_PATH "Theme#animation@incoming3.png"

//去电动画
#if IF_TALKUI_CONTACT_PHOTO_ANIMATION
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT0         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_0.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT1         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_1.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT2         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_2.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT3         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_3.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT4         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_4.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT5         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_5.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT6         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_6.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT7         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_7.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT8         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_8.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT9         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_9.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT10         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_10.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT11         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_11.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT12         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_12.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT13         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_13.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT14         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_14.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT15         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_15.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT16         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_16.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT17         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_17.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT18         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_18.png"
#define     PIC_TALK_ANIMATION_CONTACT_CONNECT19         PIC_ANIMATION_PATH "Theme#animation@rotatecalling_19.png"
#endif

#define     PIC_TALK_ANIMATION_CONNECT1                             PIC_ANIMATION_PATH "Theme#animation@calling1.png"
#define     PIC_TALK_ANIMATION_CONNECT2                             PIC_ANIMATION_PATH "Theme#animation@calling2.png"
#define     PIC_TALK_ANIMATION_CONNECT3                             PIC_ANIMATION_PATH "Theme#animation@calling3.png"
#define     PIC_TALK_ANIMATION_CONNECT4                             PIC_ANIMATION_PATH "Theme#animation@calling4.png"
#define     PIC_TALK_ANIMATION_CONNECT5                             PIC_ANIMATION_PATH "Theme#animation@calling5.png"
#define     PIC_TALK_ANIMATION_CONNECT6                             PIC_ANIMATION_PATH "Theme#animation@calling6.png"

//通话中有来电
#define     PIC_TALK_ANIMATION_TALK_INCOMINGCALL1                   PIC_ANIMATION_PATH "Theme#animation@talkincoming1.png"
#define     PIC_TALK_ANIMATION_TALK_INCOMINGCALL2                   PIC_ANIMATION_PATH "Theme#animation@talkincoming2.png"
#define     PIC_TALK_ANIMATION_TALK_INCOMINGCALL3                   PIC_ANIMATION_PATH "Theme#animation@talkincoming3.png"
#else
//动画
//来电动画
#define     PIC_TALK_ANIMATION_RINGING1                             PIC_BASE_PATH "CallIn_01.dob"
#define     PIC_TALK_ANIMATION_RINGING2                             PIC_BASE_PATH "CallIn_02.dob"
#define     PIC_TALK_ANIMATION_RINGING3                             PIC_BASE_PATH "CallIn_03.dob"
#define     PIC_TALK_ANIMATION_RINGING4                             PIC_BASE_PATH "CallIn_04.dob"

//去电动画
#define     PIC_TALK_ANIMATION_CONNECT1                             PIC_BASE_PATH "Connecting_01.dob"
#define     PIC_TALK_ANIMATION_CONNECT2                             PIC_BASE_PATH "Connecting_02.dob"
#define     PIC_TALK_ANIMATION_CONNECT3                             PIC_BASE_PATH "Connecting_03.dob"
#define     PIC_TALK_ANIMATION_CONNECT4                             PIC_BASE_PATH "Connecting_04.dob"
#define     PIC_TALK_ANIMATION_CONNECT5                             PIC_BASE_PATH "Connecting_05.dob"
#endif

//通话中有来电  显示目标账号的图标，直接写在配置里
// #define      PIC_TALK_TO_LINE_ARROW                                  PIC_BASE_PATH "to_line_arrow.png"

#if IF_TALKUI_CONTACT_PHOTO
//HOLD图片
#define     PIC_TALK_HOLD_SMALL                                         PIC_TALK_ICON_PATH "Theme#icon@hold_small.png"
//HELD图片
#define     PIC_TALK_HELD_SMALL                                         PIC_TALK_ICON_PATH "Theme#icon@hold_small.png"
//Mute图片
#define     PIC_TALK_MUTE_SMALL                                         PIC_TALK_ICON_PATH "Theme#icon@mute_icon_small.png"

//有联系人头像时，hold held mute 的状态
//HOLD图片
#define     PIC_TALK_HOLD                                           PIC_TALK_ICON_PATH "Theme#icon@hold.png"
//HELD图片
#define     PIC_TALK_HELD                                           PIC_TALK_ICON_PATH "Theme#icon@hold.png"
//Mute图片
#define     PIC_TALK_MUTE                                           PIC_TALK_ICON_PATH "Theme#icon@mute_icon.png"
#else
//HOLD图片
#define     PIC_TALK_HOLD_SMALL                                         PIC_BASE_PATH "LocalHold.dob"
//HELD图片
#define     PIC_TALK_HELD_SMALL                                         PIC_TALK_HOLD_SMALL
//Mute图片
#define     PIC_TALK_MUTE_SMALL                                         PIC_BASE_PATH "Mute.dob"

//HOLD图片
#define     PIC_TALK_HOLD                                           PIC_TALK_HOLD_SMALL
//HELD图片
#define     PIC_TALK_HELD                                           PIC_TALK_HELD_SMALL
//Mute图片
#define     PIC_TALK_MUTE                                           PIC_TALK_MUTE_SMALL
#endif

#if IF_TALKUI_CONTACT_PHOTO
//加密图标
#define     PIC_TALK_ICON_ENCRY                                     PIC_TALK_ICON_PATH "Theme#icon@encrypt.png"
//高清图标
#define     PIC_TALK_ICON_HD                                        PIC_TALK_ICON_PATH "Theme#icon@HD.png"
#define     PIC_TALK_ICON_AMR                                     PIC_TALK_ICON_PATH "Theme#icon@AMR.png"
#define     PIC_TALK_ICON_OPUS                                    PIC_TALK_ICON_PATH "Theme#icon@OPUS.png"

#define     PIC_TALK_ICON_HD_ITEM                                        PIC_TALK_ICON_PATH "Theme#icon@HDConfManager.png"
#define     PIC_TALK_ICON_AMR_ITEM                                     PIC_TALK_ICON_PATH "Theme#icon@AMRConfManager.png"
#define     PIC_TALK_ICON_OPUS_ITEM                                    PIC_TALK_ICON_PATH "Theme#icon@OPUSConfManager.png"

//静音图标
#define     PIC_TALK_ICON_SILENCE                                   PIC_TALK_ICON_PATH "Theme#icon@ring_mute.png"
#else
//加密图标
#define     PIC_TALK_ICON_ENCRY                                     PIC_BASE_PATH "encryption.dob"
//高清图标
#define     PIC_TALK_ICON_HD_DEPART                           PIC_BASE_PATH "VoiceHD.dob"
#define     PIC_TALK_ICON_HD                                        PIC_BASE_PATH "VoiceHD2.dob"
//静音图标
#define     PIC_TALK_ICON_SILENCE                                   PIC_BASE_PATH "NoRing.dob"

#define     PIC_TALK_ICON_AMR                                        PIC_BASE_PATH "VoiceAMR.dob"
#define     PIC_TALK_ICON_OPUS                                        PIC_BASE_PATH "VoiceOPUS.dob"
#endif

#if IF_TALKUI_CONTACT_PHOTO
//通道图标
//免提通道图标
#define         PIC_TALKUI_ICON_TAB_CHANNEL_HANDFREE        PIC_TALK_ICON_PATH "Theme#icon@hand_free.png"
//手柄通道图标
#define         PIC_TALKUI_ICON_TAB_CHANNEL_HANDSET     PIC_TALK_ICON_PATH "Theme#icon@hand_set.png"
//耳麦通道图标
#define         PIC_TALKUI_ICON_TAB_CHANNEL_HEADSET     PIC_TALK_ICON_PATH "Theme#icon@head_set_big.png"
#else
//通道图标
//免提通道图标
#define         PIC_TALKUI_ICON_TAB_CHANNEL_HANDFREE        PIC_BASE_PATH "HandFree.dob"
//手柄通道图标
#define         PIC_TALKUI_ICON_TAB_CHANNEL_HANDSET     PIC_BASE_PATH "talk_small.dob"
//耳麦通道图标
#define         PIC_TALKUI_ICON_TAB_CHANNEL_HEADSET     PIC_BASE_PATH "headset_small.dob"
#endif

#if IF_TALKUI_CONTACT_PHOTO
// 要改成 跟 t4xpath 统一
//蓝牙图标
////蓝牙耳机
#define     PIC_TALK_ICON_BLUETOOTH_HEADSET                                 PIC_TALK_ICON_PATH "Theme#icon@bluetooth_headset.png"
//蓝牙手机
#define     PIC_TALK_ICON_BLUETOOTH_MOBILE                                  PIC_TALK_ICON_PATH "Theme#icon@bluetooth_mobile_icon.png"
//蓝牙未连接
#define     PIC_TALK_ICON_BLUETOOTH_UNCONNECTED                                 PIC_TALK_ICON_PATH "bluetooth_dongle_insert.dob"
#else
////蓝牙耳机
#define     PIC_TALK_ICON_BLUETOOTH_HEADSET                                 PIC_BASE_PATH "bluetooth_headset.dob"
//蓝牙手机
#define     PIC_TALK_ICON_BLUETOOTH_MOBILE                                  PIC_BASE_PATH "idle_bluetooth_phone_connected.dob"
//蓝牙未连接
#define     PIC_TALK_ICON_BLUETOOTH_UNCONNECTED                                 PIC_BASE_PATH "bluetooth_dongle_insert.dob"
#endif

//其他图标
#if IF_TALKUI_CONTACT_PHOTO
//  t4xpicpath.h  PIC_ICON_RECORD  录音相关的图标有5个
//bsft录音
#define     PIC_ICON_RECORD                                     PIC_TALK_ICON_PATH "Theme#icon@record_start.png"
//无法录音
#define     PIC_CANNOT_RECORD                                   PIC_TALK_ICON_PATH "Theme#icon@cannot_be_recorded.png"
//无法停止录音
#define     PIC_CANNOT_STOPPED_RECORD                   PIC_TALK_ICON_PATH "Theme#icon@cannot_be_stopped.png"
//无法开始录音
#define     PIC_CANNOT_STARTED_RECORD                   PIC_TALK_ICON_PATH "Theme#icon@cannot_be_started.png"
//录音已满
#define     PIC_RECORD_FULL                                         PIC_TALK_ICON_PATH "Theme#icon@record_full.png"

//本地录音
#define     PIC_RECORD_NORMAL                                   PIC_TALK_ICON_PATH "Theme#icon@record_normal.png"
//录音暂停
#define     PIC_RECORD_PAUSE                                        PIC_TALK_ICON_PATH "Theme#icon@record_pause.png"
#else
//bsft录音
#define     PIC_ICON_RECORD                                     PIC_BASE_PATH "record_start.dob"
//无法录音
#define     PIC_CANNOT_RECORD                                   PIC_BASE_PATH "cannot_be_recorded.dob"
//无法停止录音
#define     PIC_CANNOT_STOPPED_RECORD                   PIC_BASE_PATH "cannot_be_stopped.dob"
//无法开始录音
#define     PIC_CANNOT_STARTED_RECORD                   PIC_BASE_PATH "cannot_be_started.dob"
//录音已满
#define     PIC_RECORD_FULL                                         PIC_BASE_PATH "record_full.dob"

//本地录音
#define     PIC_RECORD_NORMAL                                   PIC_BASE_PATH "record_normal.dob"
//录音暂停
#define     PIC_RECORD_PAUSE                                        PIC_BASE_PATH "record_pause.dob"

#endif


}       //namespace talkui

#endif
