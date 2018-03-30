//#if (defined _USE_SKIA) || (defined _XWIN)

#ifndef T4X_PIC_PATH_H
#define T4X_PIC_PATH_H

#include "picpath.h"

// 定义menu图片路径
#define PIC_MENU_APPLICATION    PIC_MENU_PREFIX "menu_applications.png"
#define PIC_MENU_DIRECTORY  PIC_MENU_PREFIX "menu_directory.png"
#define PIC_MENU_DISPLAY    PIC_MENU_PREFIX "menu_display.png"
#define PIC_MENU_FEATURES   PIC_MENU_PREFIX "menu_features.png"
#define PIC_MENU_MESSAGE    PIC_MENU_PREFIX "menu_message.png"
#define PIC_MENU_NETWORK    PIC_MENU_PREFIX "menu_network.png"
#define PIC_MENU_SETTING    PIC_MENU_PREFIX "menu_setting.png"
#define PIC_MENU_STATUS PIC_MENU_PREFIX "menu_status.png"
#define PIC_MENU_HISTORY    PIC_MENU_PREFIX "menu_history.png"
#define PIC_MENU_SELECTED   PIC_MENU_PREFIX "menu_selected.png"

// 定义wifi图片路径
#define PIC_WIFI_SIGNAL_0   PIC_WIFI_PREFIX "wifi_1.png"
#define PIC_WIFI_SIGNAL_1   PIC_WIFI_PREFIX "wifi_2.png"
#define PIC_WIFI_SIGNAL_2   PIC_WIFI_PREFIX "wifi_3.png"
#define PIC_WIFI_SIGNAL_3   PIC_WIFI_PREFIX "wifi_4.png"
#define PIC_WIFI_SIGNAL_4   PIC_WIFI_PREFIX "wifi_5.png"
#define PIC_WIFI_LOCK_CONNECTED_0   PIC_WIFI_PREFIX "wifi_lock_connected_1.png"
#define PIC_WIFI_LOCK_CONNECTED_1   PIC_WIFI_PREFIX "wifi_lock_connected_2.png"
#define PIC_WIFI_LOCK_CONNECTED_2   PIC_WIFI_PREFIX "wifi_lock_connected_3.png"
#define PIC_WIFI_LOCK_CONNECTED_3   PIC_WIFI_PREFIX "wifi_lock_connected_4.png"
#define PIC_WIFI_LOCK_CONNECTED_4   PIC_WIFI_PREFIX "wifi_lock_connected_5.png"
#define PIC_WIFI_LOCK_UNCONNECTED_0     PIC_WIFI_PREFIX "wifi_lock_unconnected_1.png"
#define PIC_WIFI_LOCK_UNCONNECTED_1     PIC_WIFI_PREFIX "wifi_lock_unconnected_2.png"
#define PIC_WIFI_LOCK_UNCONNECTED_2     PIC_WIFI_PREFIX "wifi_lock_unconnected_3.png"
#define PIC_WIFI_LOCK_UNCONNECTED_3     PIC_WIFI_PREFIX "wifi_lock_unconnected_4.png"
#define PIC_WIFI_LOCK_UNCONNECTED_4     PIC_WIFI_PREFIX "wifi_lock_unconnected_5.png"
#define PIC_WIFI_NOLOCK_UNCONNECTED_0   PIC_WIFI_PREFIX "wifi_nolock_unconnected_1.png"
#define PIC_WIFI_NOLOCK_UNCONNECTED_1   PIC_WIFI_PREFIX "wifi_nolock_unconnected_2.png"
#define PIC_WIFI_NOLOCK_UNCONNECTED_2   PIC_WIFI_PREFIX "wifi_nolock_unconnected_3.png"
#define PIC_WIFI_NOLOCK_UNCONNECTED_3   PIC_WIFI_PREFIX "wifi_nolock_unconnected_4.png"
#define PIC_WIFI_NOLOCK_UNCONNECTED_4   PIC_WIFI_PREFIX "wifi_nolock_unconnected_5.png"

#define PIC_WIFI_CONNECTED  PIC_WIFI_PREFIX "wifi_connected.png"
#define PIC_WIFI_CONNECTING PIC_WIFI_PREFIX "wifi_connecting.png"
#define PIC_WIFI_LOCK   PIC_WIFI_PREFIX "wifi_lock.png"

// 定义widget图片路径
//radiobutton
#define PIC_RADIO_OFF   PIC_WIDGET_PREFIX "radio_off.png"
#define PIC_RADIO_ON    PIC_WIDGET_PREFIX "radio_on.png"
#define PIC_RADIO_DISABLE   PIC_WIDGET_PREFIX "radio_disable.png"
//dtmf
#define PIC_DTMF_BACKGROUND PIC_WIDGET_PREFIX "dtmf_background.9.png"
#define PIC_SEARCH_BACKGROUND   PIC_WIDGET_PREFIX "search_background.9.png"
//dialing search
//优化速度 暂时不使用
//http://10.3.5.199/Bug.php?BugID=61705
//#define       PIC_DIALING_SEARCH_BACKGROUND           PIC_WIDGET_PATH "dialing_search_background.png"
//dialing
//优化速度 暂时不使用
//http://10.3.5.199/Bug.php?BugID=61705
//#define       PIC_DIALING_BACKGROUND          PIC_WIDGET_PATH "dialing_background.png"
//定义popmenu图片路径
#define PIC_POPMENU_BKG PIC_WIDGET_PREFIX "option_background.9.png"

#define PIC_POPMEU_SEL_ITEM PIC_WIDGET_PREFIX "option_selected.9.png"

#define PIC_USB_AUDIO_PROCESSBAR_EMPTY  PIC_WIDGET_PREFIX "usb_audio_processbar_empty.png"
#define PIC_USB_AUDIO_PROCESSBAR_FULL   PIC_WIDGET_PREFIX "usb_audio_processbar_full.png"
#define PIC_USB_AUDIO_PROGRESSBAR_CURPOS    PIC_WIDGET_PREFIX "usb_audio_processbar_curpos.png"

// 定义animation图片路径
#define PIC_ANIMATION_ONE   PIC_ANIMATION_PREFIX "calling1.png"
#define PIC_ANIMATION_TWO   PIC_ANIMATION_PREFIX "calling2.png"
#define PIC_ANIMATION_THREE PIC_ANIMATION_PREFIX "calling3.png"
#define PIC_ANIMATION_FOUR  PIC_ANIMATION_PREFIX "calling4.png"
#define PIC_ANIMATION_FIVE  PIC_ANIMATION_PREFIX "calling5.png"
#define PIC_ANIMATION_SIX   PIC_ANIMATION_PREFIX "calling6.png"
#define PIC_ANIMATION_SEVEN PIC_ANIMATION_PREFIX "incoming1.png"
#define PIC_ANIMATION_EIGHT PIC_ANIMATION_PREFIX "incoming2.png"
#define PIC_ANIMATION_NINE  PIC_ANIMATION_PREFIX "incoming3.png"
#define PIC_ANIMATION_TEN   PIC_ANIMATION_PREFIX "incoming4.png"
#define PIC_ANIMATION_ELEVEN    PIC_ANIMATION_PREFIX "incoming5.png"
#define PIC_ANIMATION_TWELVE    PIC_ANIMATION_PREFIX "incoming6.png"

#define PIC_ANIMATION_TALK_INCOMING_1   PIC_ANIMATION_PREFIX "talkincoming1.png"
#define PIC_ANIMATION_TALK_INCOMING_2   PIC_ANIMATION_PREFIX "talkincoming2.png"
#define PIC_ANIMATION_TALK_INCOMING_3   PIC_ANIMATION_PREFIX "talkincoming2.png"

#define PIC_ANIMATION_SCALE_PIC         PIC_ANIMATION_PREFIX "scaleincoming.png"
#define PIC_ANIMATION_ROTATE_PIC            PIC_ANIMATION_PREFIX "rotatecalling.png"

//定义arrows图片路径
#define PIC_ARROWS_COMBOX   PIC_ARROWS_PREFIX "combox_icon.png"
#define PIC_ARROWS_DATETIME_DOWN    PIC_ARROWS_PREFIX "datetime_down1.png"
#define PIC_ARROWS_DATETIME_UP  PIC_ARROWS_PREFIX "datetime_up1.png"
#define PIC_ARROWS_DROP_DOWN    PIC_ARROWS_PREFIX "drop_down.png"
#define PIC_ARROWS_LEFT_ARROW   PIC_ARROWS_PREFIX "left_arrow.png"
#define PIC_ARROWS_RIGHT_ARROW  PIC_ARROWS_PREFIX "right_arrow.png"
#define PIC_ARROWS_LEFT_DISABLE_ARROW   PIC_ARROWS_PREFIX "left_arrow_disabled.png"
#define PIC_ARROWS_RIGHT_DISABLE_ARROW  PIC_ARROWS_PREFIX "right_arrow_disabled.png"
#define PIC_ARROWS_SCROLLDOWN1  PIC_ARROWS_PREFIX "scrolldown1.png"
#define PIC_ARROWS_SCROLLDOWN2  PIC_ARROWS_PREFIX "scrolldown2.png"
#define PIC_ARROWS_SCROLLDOWN   PIC_ARROWS_PREFIX "scroll_down.png"
#define PIC_ARROWS_SCROLLUP PIC_ARROWS_PREFIX "scroll_up.png"

//定义contacts图片路径
#define PIC_CONTACTS_DEFAULT_IMAGE  PIC_CONTACTS_PREFIX "default_contact_image.png"
#define PIC_CONTACTS_DEFAULT    PIC_CONTACTS_PREFIX "default_photo.png"
#define PIC_CONTACTS_ACCOUNT2   PIC_CONTACTS_PREFIX "accounts2.png"
#define PIC_CONTACTS_ACCOUNT    PIC_CONTACTS_PREFIX "accounts.png"
#define PIC_CONTACTS_CONFERENCE PIC_CONTACTS_PREFIX "conference.png"
#define PIC_CONTACTS_CONTACT    PIC_CONTACTS_PREFIX "contact_icon.png"
#define PIC_CONTACTS_GROUP_ICON PIC_CONTACTS_PREFIX "contact_group.png"
#define PIC_CONTACTS_IMAGE_BLUETOOTH_DEFAULT    PIC_CONTACTS_PREFIX "bluetooth_default.png"

//定义dss图标路径
#define PIC_DSS_EDK PIC_DSS_PREFIX "dss_edk.png"
#define PIC_DSS_TALK    PIC_DSS_PREFIX "dss_talk.png"  //通话功能
#define PIC_DSS_SPEED_DIAL  PIC_DSS_PREFIX "dss_speed_dial.png"     //快捷呼出
#define PIC_DSS_BLF_IDLE    PIC_DSS_PREFIX "dss_blf_idle.png"           // BLF 空闲
#define PIC_DSS_BLF_CALLOUT PIC_DSS_PREFIX "dss_blf_callout.png"        // BLF 回铃
#define PIC_DSS_BLF_RING    PIC_DSS_PREFIX "dss_blf_ringing.png"        // BLF 响铃
#define PIC_DSS_BLF_TALKING PIC_DSS_PREFIX "dss_blf_talking.png"        // BLF 通话中
#define PIC_DSS_BLF_HOLD    PIC_DSS_PREFIX "dss_blf_hold.png"           // BLF 保持
#define PIC_DSS_BLF_PARK    PIC_DSS_PREFIX "dss_blf_park.png"           // BLF 保持
#define PIC_DSS_BLF_UNKNOWN PIC_DSS_PREFIX "dss_blf_unknown.png"        // BLF 未知
#define PIC_DSS_LINE    PIC_DSS_PREFIX "dss_line.png"                   // Line线路 正常
#define PIC_DSS_LINE_UNREGISTERED   PIC_DSS_PREFIX "dss_line_unregistered.png"//  Line线路 未注册
#define PIC_DSS_LINE_FAIL   PIC_DSS_PREFIX "dss_line_fail.png"          // Line线路 注册失败
#define PIC_DSS_LINE_REGISTERING    PIC_DSS_PREFIX "dss_line_registering.png"  // Line线路 注册中
#define PIC_DSS_LINE_REGISTERED PIC_DSS_PREFIX "dss_line_registered.png"   // Line线路 注册完成
#define PIC_DSS_LINE_USING  PIC_DSS_PREFIX "dss_line_using.png"         // Line线路 正被使用
#define PIC_DSS_LINE_FORWARD    PIC_DSS_PREFIX "dss_line_forward.png"           // Line线路 转接
#define PIC_DSS_LINE_DND    PIC_DSS_PREFIX "dss_line_dnd.png"               // Line线路 免打扰
#define PIC_DSS_URL PIC_DSS_PREFIX "dss_URL.png"                    //网络检测
#define PIC_DSS_URL_GREEN   PIC_DSS_PREFIX "dss_URL_green.png"          // 网络检测成功
#define PIC_DSS_URL_RED PIC_DSS_PREFIX "dss_URL_red.png"                // 网络检测不可用
#define PIC_DSS_BLA_IDLE    PIC_DSS_PREFIX "dss_BLA_idle.png"                   //BLA
#define PIC_DSS_BLA_SEIZED  PIC_DSS_PREFIX "dss_BLA_seized.png"             // 占线
#define PIC_DSS_BLA_PROGRESSING PIC_DSS_PREFIX "dss_BLA_progressing.png"            // 回铃    
#define PIC_DSS_BLA_ALERTING    PIC_DSS_PREFIX "dss_BLA_alerting.png"               // 振铃 
#define PIC_DSS_BLA_ACTIVE  PIC_DSS_PREFIX "dss_BLA_active.png"             // 接通    
#define PIC_DSS_BLA_HELD    PIC_DSS_PREFIX "dss_BLA_held.png"                   // public hold，群组内任何人都可以retrieve
#define PIC_DSS_BLA_HELD_PRIVATE    PIC_DSS_PREFIX "dss_BLA_held_private.png"           // prive hold，只有hold的那个人能retrieve 
#define PIC_DSS_BLA_BRIDGE_ACTIVE   PIC_DSS_PREFIX "dss_BLA_bridge_active.png"      // 多方通话，至少有两方处于通话状态
#define PIC_DSS_BLA_BRIDGE_HELD PIC_DSS_PREFIX "dss_BLA_bridge_held.png"            // 多方通话，群组成员都按hold     
#define PIC_DSS_BLA_PARK    PIC_DSS_PREFIX "dss_BLA_park.png"                   // 有电话Park进来  
#define PIC_DSS_BLA_UNKNOWN PIC_DSS_PREFIX "dss_BLA_unknown.png"                // BLA不成功
#define PIC_DSS_XML_BROWSER PIC_DSS_PREFIX "dss_XML_browser_normal.png"
#define PIC_DSS_NONE    PIC_DSS_PREFIX "dss_none.png"
#define PIC_DSS_INTERCOM    PIC_DSS_PREFIX "dss_intercom.png"
#define PIC_DSS_INTERCOM_RINGING    PIC_DSS_PREFIX "dss_intercom_ringing.png"
#define PIC_DSS_INTERCOM_TALKING    PIC_DSS_PREFIX "dss_intercom_talking.png"
#define PIC_DSS_INTERCOM_HOLD   PIC_DSS_PREFIX "dss_intercom_hold.png"
#define PIC_DSS_INTERCOM_UNKNOWN    PIC_DSS_PREFIX "dss_intercom_unknow.png"
#define PIC_DSS_PICKUP  PIC_DSS_PREFIX "dss_pickup.png"
#define PIC_DSS_GROUPPICKUP PIC_DSS_PREFIX "dss_grouppickup.png"
#define PIC_DSS_CALLPARK    PIC_DSS_PREFIX "dss_callpark.png"                   //通话暂留
#define PIC_DSS_CALLPARK_RINGING    PIC_DSS_PREFIX "dss_callpark_ringing.png"           //通话暂留响铃
#define PIC_DSS_CALLPARK_MUNUAL_TALK    PIC_DSS_PREFIX "dss_callpark_manual_talk.png"       //通话暂留手动接起通话
#define PIC_DSS_CALLPARK_AUTO_TALK  PIC_DSS_PREFIX "dss_callpark_auto_talk.png"     //通话暂留自动接起通话
#define PIC_DSS_CALLPARK_FAIL   PIC_DSS_PREFIX "dss_callpark_failed.png"            //通话暂留失败
#define PIC_DSS_DIRECTORY       PIC_DSS_PREFIX "dss_directory.png" //地址簿
#define PIC_DSS_GET_BALANCE     PIC_DSS_PREFIX "dss_getbalance.png" //GetBalance
#define PIC_DSS_DTMF    PIC_DSS_PREFIX "dss_dtmf.png"
#define PIC_DSS_LOCALGROUP  PIC_DSS_PREFIX "dss_localgroup.png"     // 地址簿
#define PIC_DSS_XMLGROUP    PIC_DSS_PREFIX "dss_xmlgroup.png"           // xml地址簿
#define PIC_DSS_LDAP    PIC_DSS_PREFIX "dss_ldap.png"               // ldap地址簿
#define PIC_DSS_CONFERENCE  PIC_DSS_PREFIX "dss_conference.png"     // 会议
#define PIC_DSS_HOLD    PIC_DSS_PREFIX "dss_hold.png"               // 通话暂停
#define         PIC_DSS_PUBLIC_HOLD             PIC_DSS_HOLD                        // 通话暂停
#define         PIC_DSS_PRIVATE_HOLD            PIC_DSS_HOLD                        // 通话暂停
#define PIC_DSS_TRANSFER    PIC_DSS_PREFIX "dss_transfer.png"           // 呼叫转移
#define PIC_DSS_FORWARD PIC_DSS_PREFIX "dss_forward.png"
#define PIC_DSS_DND PIC_DSS_PREFIX "dss_dnd.png"
#define PIC_DSS_DND_OFF PIC_DSS_PREFIX "dss_dnd_off.png"
#define PIC_DSS_REDIAL  PIC_DSS_PREFIX "dss_redial.png"
#define PIC_DSS_CALLRETURN  PIC_DSS_PREFIX "dss_callreturn.png"     // 回拨
#define PIC_DSS_SMS PIC_DSS_PREFIX "dss_sms.png"
#define PIC_DSS_RECORD  PIC_DSS_PREFIX "dss_record.png"
#define PIC_DSS_RECORDING   PIC_DSS_PREFIX "dss_recording.png"      // 正在录音
#define PIC_DSS_PAGING  PIC_DSS_PREFIX "dss_paging.png"
#define PIC_DSS_GROUP_LISTENING PIC_DSS_PREFIX "dss_group_listening.png"
#define PIC_DSS_ZERO_TOUCH  PIC_DSS_PREFIX "dss_zero_touch.png"
#define PIC_DSS_ACD PIC_DSS_PREFIX "dss_acd.png"
#define PIC_DSS_HOTDESKING  PIC_DSS_PREFIX "dss_hotdesking.png"
#define PIC_DSS_VOICE_MAIL  PIC_DSS_PREFIX "dss_voice_mail.png"
#define PIC_DSS_HOTELING_LOGININ    PIC_DSS_PREFIX "dss_hoteling_in.png"
#define PIC_DSS_HOTELING_LOGINOUT   PIC_DSS_PREFIX "dss_hoteling_out.png"
#define PIC_DSS_FAVORITES   PIC_DSS_PREFIX "dss_favorites.png"      // 收藏夹
#define PIC_DSS_UC_ONLINE   PIC_DSS_PREFIX "dss_uc_online.png"      // 在线
#define PIC_DSS_UC_AWAY PIC_DSS_PREFIX "dss_uc_away.png"            // 离开
#define PIC_DSS_UC_BUSY PIC_DSS_PREFIX "dss_uc_busy.png"            // 忙碌
#define PIC_DSS_UC_OFFLINE  PIC_DSS_PREFIX "dss_uc_offline.png"     // 离线
#define PIC_DSS_KEYPAD_LOCK PIC_DSS_PREFIX "dss_keypad_lock.png"
#define PIC_DSS_BUDDIES PIC_DSS_PREFIX "dss_buddies.png"
#define PIC_DSS_ACD_TRACE   PIC_DSS_PREFIX "dss_originated_trace.png"
#define PIC_DSS_DISPCODE    PIC_DSS_PREFIX "dss_disposition_code.png"
#define PIC_DSS_ESCALATE    PIC_DSS_PREFIX "dss_emergency_escalation.png"
#define PIC_DSS_MEET_ME PIC_DSS_PREFIX "dss_meetme_conference.png"
#define PIC_DSS_STATUS  PIC_DSS_PREFIX "dss_status.png"
#define PIC_DSS_ACD_AVAILABLE   PIC_DSS_PREFIX "dss_acd_available.png"
#define PIC_DSS_ACD_UNAVAILABLE PIC_DSS_PREFIX "dss_acd_unavailable.png"
#define PIC_DSS_ACD_WARP_UP PIC_DSS_PREFIX "dss_acd_wrapup.png"
#define PIC_DSS_ACD_LOGOUT  PIC_DSS_PREFIX "dss_acd_logout.png"
#define PIC_DSS_RETRIEVE_PARK   PIC_DSS_PREFIX "dss_retrieve_park.png"
#define PIC_DSS_MOBILE_BLUE_GREEN   PIC_DSS_PREFIX "dss_mobile_blue_green.png"
#define PIC_DSS_MOBILE_BLUE_RED     PIC_DSS_PREFIX "dss_mobile_blue_red.png"
#define PIC_DSS_MOBILE_BLUE_CONNETING   PIC_DSS_PREFIX "dss_mobile_blue_conneting.png"
#define PIC_DSS_EDITING             PIC_DSS_PREFIX "dss_editing.png"

#define PIC_DSS_PREFIX_ICON     PIC_DSS_PREFIX "dss_prefix.png"

#define PIC_DSS_MTSW_LOGOUT         PIC_DSS_PREFIX "dss_mtsw_logout.png"
#define PIC_DSS_MTSW_MESSAGELIST    PIC_DSS_PREFIX "dss_mtsw_messagelist.png"

//Dsskey 背景路径
#define PIC_DSSBG_FRAME_BG_LEFT_HEIGHT_TRANS        PIC_DSSBG_PREFIX "dssbg_left_frame_height_trans.9.png"
#define PIC_DSSBG_FRAME_BG_RIGHT_HEIGHT_TRANS       PIC_DSSBG_PREFIX "dssbg_right_frame_height_trans.9.png"
#define PIC_DSSBG_FRAME_BG_LEFT_80_TRANS            PIC_DSSBG_PREFIX "dssbg_left_frame_80_trans.9.png"
#define PIC_DSSBG_FRAME_BG_RIGHT_80_TRANS           PIC_DSSBG_PREFIX "dssbg_right_frame_80_trans.9.png"
#define PIC_DSSBG_FRAME_BG_LEFT_60_TRANS            PIC_DSSBG_PREFIX "dssbg_left_frame_60_trans.9.png"
#define PIC_DSSBG_FRAME_BG_RIGHT_60_TRANS           PIC_DSSBG_PREFIX "dssbg_right_frame_60_trans.9.png"
#define PIC_DSSBG_FRAME_BG_LEFT_40_TRANS            PIC_DSSBG_PREFIX "dssbg_left_frame_40_trans.9.png"
#define PIC_DSSBG_FRAME_BG_RIGHT_40_TRANS           PIC_DSSBG_PREFIX "dssbg_right_frame_40_trans.9.png"
#define PIC_DSSBG_FRAME_BG_LEFT_20_TRANS            PIC_DSSBG_PREFIX "dssbg_left_frame_20_trans.9.png"
#define PIC_DSSBG_FRAME_BG_RIGHT_20_TRANS           PIC_DSSBG_PREFIX "dssbg_right_frame_20_trans.9.png"
#define PIC_DSSBG_FRAME_BG_LEFT_LOW_TRANS           PIC_DSSBG_PREFIX "dssbg_left_frame_low_trans.9.png"
#define PIC_DSSBG_FRAME_BG_RIGHT_LOW_TRANS          PIC_DSSBG_PREFIX "dssbg_right_frame_low_trans.9.png"

//定义general图片路径
#define     PIC_GENERAL_DLG_SETTING             ""
#define PIC_GENERAL_MENUFOCUS   PIC_GENERAL_PREFIX "menu_focus_background.png"
#define PIC_GENERAL_INIT    PIC_GENERAL_PREFIX "init_background.png"
#define PIC_GENERAL_NOTICEBLANK PIC_GENERAL_PREFIX "notice_blank.png"
#define PIC_GENERAL_TALKBACKGROUND  PIC_GENERAL_PREFIX "talk_background.9.png"
#define PIC_TITLE_TAG_SELECTED  PIC_GENERAL_PREFIX "title_sel_item.9.png"
#define PIC_GENERAL_ICON_COVER  PIC_GENERAL_PREFIX "icon_cover.9.png"
#define PIC_GENERAL_CONTACT_COVER   PIC_GENERAL_PREFIX "contact_cover.png"
#define PIC_GENERAL_CONTACT_CIRCLE  PIC_GENERAL_PREFIX "contact_circle.png"
//定义message图片路径
#define PIC_GENERAL_MESSAGE_UNREAD  PIC_GENERAL_PREFIX "message_in_unread.png"
#define PIC_GENERAL_MESSAGE_READ    PIC_GENERAL_PREFIX "message_in_read.png"
#define PIC_GENERAL_MESSAGE_DRAFT   PIC_GENERAL_PREFIX "message_draft.png"
#define PIC_GENERAL_MESSAGE_SENDING PIC_GENERAL_PREFIX "message_out_sending.png"
#define PIC_GENERAL_MESSAGE_SEND    PIC_GENERAL_PREFIX "message_out_sent.png"

#define PIC_GENERAL_VOICE_MESSAGE_UNREAD    PIC_GENERAL_PREFIX "voice_message_in_unread.png"
#define PIC_GENERAL_VOICE_MESSAGE_READ  PIC_GENERAL_PREFIX "voice_message_in_read.png"
#define PIC_GENERAL_VIDEO_MESSAGE_UNREAD    PIC_GENERAL_PREFIX "video_message_in_unread.png"
#define PIC_GENERAL_VIDEO_MESSAGE_READ  PIC_GENERAL_PREFIX "video_message_in_read.png"

///---
#define PIC_GENERAL_NOTICEWARNING   PIC_GENERAL_PREFIX "notice_warning.png"
#define PIC_GENERAL_OPTION_BACKGROUND   PIC_GENERAL_PREFIX "option_background.png"
#define PIC_GENERAL_OPTION_SELECTED PIC_GENERAL_PREFIX "option_selected.png"
#define PIC_GENERAL_PROGRESS    PIC_GENERAL_PREFIX "progressing.png"
#define PIC_GENERAL_SOFTKEY PIC_GENERAL_PREFIX "softkey_normal.png"
#define PIC_GENERAL_STATUSBAR   PIC_GENERAL_PREFIX "statusbar.png"
#define PIC_GENERAL_TITLE   PIC_GENERAL_PREFIX "title_small.png"
#define PIC_GENERAL_TITLE_BIG   PIC_GENERAL_PREFIX "title_big.png"
#define     PIC_VOICE_MAIL_ITEM_NORMAL_BG                       ""
#define     PIC_VOICE_MAIL_ITEM_PRESS_BG                        ""
#define PIC_GENERAL_WELCOME PIC_GENERAL_PREFIX "welcome.png"
#define PIC_GENERAL_SHADE   PIC_GENERAL_PREFIX "popupbox_shade.png"
#define PIC_GENERAL_SELECTMENU  PIC_GENERAL_PREFIX "menu_selected_backgroud.png"
#define PIC_GENERAL_BACKGROUND  PIC_GENERAL_PREFIX "init_background.png"

//option弹框选择图标
#define     PIC_MSGBOX_OPTION_NORMAL        PIC_MSGBOX_PREFIX "msgbox_option_normal.png"
#define     PIC_MSGBOX_OPTION_SELECTED      PIC_MSGBOX_PREFIX "msgbox_option_selected.png"

//定义icon图片路径
#define PIC_ICON_TO_LIINE_ARROW PIC_ICON_PREFIX "to_line_arrow.png"
#define PIC_ICON_TO_LIINE_ARROW_DOUBLE  PIC_ICON_PREFIX "to_line_arrow_double.png"

//填充红色
#define     PAGE_ICON_RED               PIC_ICON_PREFIX "dsskey_page_red.png"
//填充绿色
#define     PAGE_ICON_GREEN             PIC_ICON_PREFIX "dsskey_page_green.png"
//填充灰色
#define     PAGE_ICON_GRAY              PIC_ICON_PREFIX "dsskey_page_gray.png"
//灰色环图标
#define     PAGE_ICON_GRAY_CIRCLE       PIC_ICON_PREFIX "dsskey_page_gray_circle.png"
#define     PAGE_ICON_GRAY_CIRCLE_80_TRANS      PIC_ICON_PREFIX "dsskey_page_gray_circle_80_trans.png"
#define     PAGE_ICON_GRAY_CIRCLE_60_TRANS      PIC_ICON_PREFIX "dsskey_page_gray_circle_60_trans.png"
#define     PAGE_ICON_GRAY_CIRCLE_40_TRANS      PIC_ICON_PREFIX "dsskey_page_gray_circle_40_trans.png"
#define     PAGE_ICON_GRAY_CIRCLE_20_TRANS      PIC_ICON_PREFIX "dsskey_page_gray_circle_20_trans.png"
#define     PAGE_ICON_GRAY_CIRCLE_LOW_TRANS     PIC_ICON_PREFIX "dsskey_page_gray_circle_low_trans.png"
//填充橙色
#define     PAGE_ICON_ORANGE            PIC_ICON_PREFIX "dsskey_page_orange.png"

#define PIC_ICON_VOLUME_HANDFREE    PIC_ICON_PREFIX "volumebar_icon_handfree.png"
#define PIC_ICON_VOLUME_HANDSET PIC_ICON_PREFIX "volumebar_icon_handset.png"
#define PIC_ICON_VOLUME_HEADSET PIC_ICON_PREFIX "volumebar_icon_headset.png"
#define PIC_ICON_AUTOANSWER PIC_ICON_PREFIX "auto_answer.png"
#define PIC_ICON_ADD    PIC_ICON_PREFIX "btn_add_bg01.png"
#define PIC_ICON_MINUS  PIC_ICON_PREFIX "btn_minus_bg01.png"
#define PIC_ICON_CALLIN PIC_ICON_PREFIX "call_in.png"
#define PIC_ICON_CALLOUT    PIC_ICON_PREFIX "call_out.png"
#define PIC_ICON_DECTLINE   PIC_ICON_PREFIX "decline.png"
#define PIC_ICON_DEFAULT    PIC_ICON_PREFIX "default.png"
#define PIC_ICON_DND    PIC_ICON_PREFIX "DND.png"
#define PIC_ICON_DND_LARGE  PIC_ICON_PREFIX "DND_large.png"
#define PIC_ICON_ENCRYPT    PIC_ICON_PREFIX "encrypt.png"
#define PIC_ICON_CHANNEL_ENCRYPT    PIC_ICON_PREFIX "encrypt_channel.png"
#define PIC_ICON_FWD    PIC_ICON_PREFIX "forward_big.png"

#define PIC_ICON_FWD_ICON   PIC_ICON_PREFIX "forward_small.png"
#define PIC_ICON_HANDFREE   PIC_ICON_PREFIX "hand_free.png"
#define PIC_ICON_HANDSET    PIC_ICON_PREFIX "hand_set.png"
#define PIC_ICON_HEADSET    PIC_ICON_PREFIX "head_set_big.png"
#define PIC_ICON_HEADSET_MINI   PIC_ICON_PREFIX "head_set_small.png"
#define PIC_ICON_HOLD   PIC_ICON_PREFIX "hold.png"
#define PIC_ICON_HOLD_SMALL PIC_ICON_PREFIX "hold_small.png"
#define PIC_ICON_LOCK   PIC_ICON_PREFIX "lock_icon.png"
#define PIC_ICON_MISSCALL   PIC_ICON_PREFIX "missedcall_big.png"
#define PIC_ICON_MISSCALL_ICON  PIC_ICON_PREFIX "missedcall_small.png"
#define PIC_ICON_MSG_READ   PIC_ICON_PREFIX "msg_readed.png"
#define PIC_ICON_MSG_SEND   PIC_ICON_PREFIX "msg_send.png"
#define PIC_ICON_MSG_UNREAD PIC_ICON_PREFIX "msg_unread.png"
#define PIC_ICON_MSG_UNSEND PIC_ICON_PREFIX "msg_unsend.png"
#define PIC_ICON_MUTE   PIC_ICON_PREFIX "mute_icon.png"
#define PIC_ICON_MUTE_SMALL PIC_ICON_PREFIX "mute_icon_small.png"
#define PIC_ICON_NET1   PIC_ICON_PREFIX "net1.png"
#define PIC_ICON_NET2   PIC_ICON_PREFIX "net2.png"
#define PIC_ICON_REMOTEDIR  PIC_ICON_PREFIX "remotedir.png"
#define PIC_ICON_RINGMUTE   PIC_ICON_PREFIX "ring_mute.png"
#define PIC_ICON_IDLE_MUTE  PIC_ICON_PREFIX "talk_mute.png"
#define PIC_ICON_SWITCH_ACCOUNT PIC_ICON_PREFIX "icon_switch_account.png"
#define PIC_ICON_TEXTMESSAGE    PIC_ICON_PREFIX "text_message.png"
#define PIC_ICON_VOICEMAIL  PIC_ICON_PREFIX "voice_mail.png"
#define PIC_ICON_VPN    PIC_ICON_PREFIX "VPN.png"
#define PIC_ICON_BLUE_TOOTH PIC_ICON_PREFIX "blue_tooth_icon.png"
#define PIC_ICON_SEARCH_BLUE_TOOTH  PIC_ICON_PREFIX "blue_tooth_search.png"
#define PIC_ICON_WIFI_TOPBAR_ON PIC_ICON_PREFIX "wifi_topbar_on.png"
#define PIC_ICON_WIFI_TOPBAR_CONNECT    PIC_ICON_PREFIX "wifi_topbar_connect.png"
#define PIC_ICON_RECORD PIC_ICON_PREFIX "record.png"
#define PIC_CANNOT_RECORD   PIC_ICON_PREFIX "cannot_be_recorded.png"
#define PIC_CANNOT_STOPPED_RECORD   PIC_ICON_PREFIX "cannot_be_stopped.png"
#define PIC_CANNOT_STARTED_RECORD   PIC_ICON_PREFIX "cannot_be_started.png"
#define PIC_RECORD_FULL PIC_ICON_PREFIX "record_full.png"
#define PIC_RECORD_NORMAL   PIC_ICON_PREFIX "record_normal.png"
#define PIC_RECORD_PAUSE    PIC_ICON_PREFIX "record_pause.png"
#define PIC_ICON_HD PIC_ICON_PREFIX "HD.png"
#define PIC_ICON_HD_CONF_MANAGER    PIC_ICON_PREFIX "HDConfManager.png"
#define PIC_ICON_CHANNEL_HD PIC_ICON_PREFIX "HD_channel.png"
#define PIC_ICON_BLUE_TOOTH_TYPE_COMMON PIC_ICON_PREFIX "bluetooth_common.png"
#define PIC_ICON_BLUE_TOOTH_TYPE_HEADSET    PIC_ICON_PREFIX "bluetooth_headset.png"
#define PIC_ICON_BLUE_TOOTH_MOBILE  PIC_ICON_PREFIX "bluetooth_mobile_icon.png"
#define PIC_ICON_BLUE_TOOTH_CONNECT PIC_ICON_PREFIX "bluetooth_connect.png"
#define PIC_ICON_BLUE_TOOTH_MOBILE_CONNECT  PIC_ICON_PREFIX "bluetooth_mobile_connect.png"
#define PIC_ICON_BLUE_TOOTH_DEVICE_CONNECT  PIC_ICON_PREFIX "bluetooth_device_connect.png"
#define PIC_ICON_BLUE_TOOTH_MOBILE_AUTO_CONNECT     PIC_ICON_PREFIX "bluetooth_mobile_auto.png"
#define PIC_ICON_BLUE_TOOTH_HEADSET_AUTO_CONNECT    PIC_ICON_PREFIX "bluetooth_headset_auto.png"

#define PIC_ICON_USB_PHOTO_LIST_ICON    PIC_ICON_PREFIX "usb_photo_icon.png"
#define PIC_ICON_USB_RECORD_LIST_ICON   PIC_ICON_PREFIX "usb_record_icon.png"

#define PIC_ICON_PRESENCE_AWAY  PIC_ICON_PREFIX "icon_presence_away.png"
#define PIC_ICON_PRESENCE_BUSY  PIC_ICON_PREFIX "icon_presence_busy.png"
#define PIC_ICON_PRESENCE_CHATTY    PIC_ICON_PREFIX "icon_presence_chatty.png"
#define PIC_ICON_PRESENCE_EXTENDEDAWAY  PIC_ICON_PREFIX "icon_presence_extended_away.png"
#define PIC_ICON_PRESENCE_OFFLINE   PIC_ICON_PREFIX "icon_presence_offline.png"
#define PIC_ICON_PRESENCE_ONLINE    PIC_ICON_PREFIX "icon_presence_online.png"
#define PIC_ICON_PRESENCE_UNKNOWN   PIC_ICON_PREFIX "icon_presence_unknown.png"

#define     PIC_ICON_PRESENCE_NEW_OFFLINE                       PIC_ICON_PREFIX "icon_presence_new_offline.png"
#define     PIC_ICON_PRESENCE_NEW_ONLINE                        PIC_ICON_PREFIX "icon_presence_new_online.png"
#define     PIC_ICON_PRESENCE_NEW_AWAY                          PIC_ICON_PREFIX "icon_presence_new_away.png"
#define     PIC_ICON_PRESENCE_NEW_BUSY                          PIC_ICON_PREFIX "icon_presence_new_busy.png"
#define     PIC_ICON_PRESENCE_NEW_UNKNOWN                       PIC_ICON_PREFIX "icon_presence_new_unknown.png"

#define PIC_ICON_PLAY   PIC_ICON_PREFIX "play.png"
#define PIC_ICON_PAUSE  PIC_ICON_PREFIX "pause.png"
#define PIC_ICON_STOP   PIC_ICON_PREFIX "stop.png"
#define PIC_ICON_TITLEBAR_USB_CONNECT   PIC_ICON_PREFIX "titlebar_usb_connect.png"
#define PIC_ICON_TITLEBAR_USB_DETECTING PIC_ICON_PREFIX "titlebar_usb_detecting.png"
#define PIC_ICON_TITLEBAR_USB_DETECTFAILED  PIC_ICON_PREFIX "titlebar_usb_detectfailed.png"
#define PIC_ICON_TITLEBAR_USB   PIC_ICON_PREFIX "titlebar_usb.png"
#define PIC_ICON_RECORD_START   PIC_ICON_PREFIX "record_start.png"
#define PIC_ICON_RECORD_PAUSE   PIC_ICON_PREFIX "record_pause.png"
#define PIC_IDLE_USB_RECORD_STARTRECORD PIC_ICON_PREFIX "idle_usb_start_record.png"
#define PIC_IDLE_USB_RECORD_STOPRECORD  PIC_ICON_PREFIX "idle_usb_stop_record.png"
#define PIC_IDLE_USB_RECORD_PAUSERECORD PIC_ICON_PREFIX "idle_usb_pause_record.png"

#define PIC_ICON_STATUS_ONE_WORD_HINTS_BG PIC_ICON_PREFIX "status_one_word_hints_bg.png"
#define PIC_ICON_STATUS_MORE_WORDS_HINTS_BG PIC_ICON_PREFIX "status_more_words_hints_bg.png"

#define PIC_ICON_SEARCH_LOGO PIC_ICON_PREFIX "search_logo.png"

//定义scroll图片路径
#define PIC_SCROLL_SCROLL   PIC_SCROLL_PREFIX "scroll_block.9.png"
#warning T52 T54 中性版本也改用点9绘制
#define PIC_SCROLL_SCROLLBAR    PIC_SCROLL_PREFIX "scroll_bar.9.png"
#define PIC_SCROLL_SCROLLBAR_USE_POINT  PIC_SCROLL_PREFIX "scroll_bar.9.png"

// 定义音量条图片路径
#define PIC_VOLUM_DIAL_BG   PIC_VOLUME_PREFIX "volume_dial_background.png"

#define PIC_VOLUM_BG    PIC_VOLUME_PREFIX "volume_dial_background_do9.9.png"
#define PIC_VOLUM_TALK_BG   PIC_VOLUME_PREFIX "volume_talk_background.png"

//定义主题1图片路径
#define PIC_THEME1_BACKGROUND   PIC_THEME1_PREFIX "background_normal.jpg"
#define PIC_THEME1_BACKGROUND_LINE_1    PIC_THEME1_PREFIX "background_normal_line_1.png"
#define PIC_THEME1_BACKGROUND_LINE_2    PIC_THEME1_PREFIX "background_normal_line_2.png"
#define PIC_THEME1_BACKGROUND_LINE_3    PIC_THEME1_PREFIX "background_normal_line_3.png"
#define PIC_THEME1_BACKGROUND_LINE_4    PIC_THEME1_PREFIX "background_normal_line_4.png"
#define PIC_THEME1_BACKGROUND_WITH_LINE PIC_THEME1_PREFIX "background_with_line.png"
#define PIC_THEME1_BACKGROUND_DIALING   PIC_THEME1_PREFIX "background_dialing.png"
#define PIC_THEME1_BACKGROUND_9_DIALING PIC_THEME1_PREFIX "background_dialing.9.png"
#define PIC_THEME1_APPLICATIONS PIC_THEME1_PREFIX "Applications.png"
#define PIC_THEME1_DIRECTORY    PIC_THEME1_PREFIX "directory.png"
#define PIC_THEME1_DISPLAY  PIC_THEME1_PREFIX "display.png"
#define PIC_THEME1_FEATURES PIC_THEME1_PREFIX "features.png"
#define PIC_THEME1_HISTORY  PIC_THEME1_PREFIX "history.png"
#define PIC_THEME1_ITEMBACKGROUND   PIC_THEME1_PREFIX "item_background.9.png"
#define PIC_THEME1_LINEKEY  PIC_THEME1_PREFIX "linekey_normal.9.png"
#define PIC_THEME1_LINEKEY_CUTLINE  PIC_THEME1_PREFIX "linekey_normal_cutline.png"
#define     PIC_THEME1_LONG_LINEKEY                         PIC_THEME1_LINEKEY
#define PIC_THEME1_EXTEND_LINEKEY   PIC_THEME1_PREFIX "extend_linekey_normal.png"
#define PIC_THEME1_DISABLED_LINEKEY PIC_THEME1_PREFIX "linekey_disabled.9.png"
#define PIC_THEME1_DISABLED_LINEKEY_CUTLINE PIC_THEME1_PREFIX "linekey_disabled_cutline.png"
#define     PIC_THEME1_LONG_DISABLED_LINEKEY                PIC_THEME1_DISABLED_LINEKEY
#define PIC_THEME1_EXTEND_DISABLED_LINEKEY  PIC_THEME1_PREFIX "extend_linekey_disabled.png"
#define PIC_THEME1_HIGHLIGHT_LINEKEY    PIC_THEME1_PREFIX "linekey_highlight.9.png"
#define PIC_THEME1_HIGHLIGHT_LINEKEY_CUTLINE    PIC_THEME1_PREFIX "linekey_highlight_cutline.png"
#define     PIC_THEME1_LONG_HIGHLIGHT_LINEKEY               PIC_THEME1_HIGHLIGHT_LINEKEY
#define PIC_THEME1_EXTEND_HIGHLIGHT_LINEKEY PIC_THEME1_PREFIX "extend_linekey_highlight.png"
#define PIC_THEME1_DIAL_LINEKEY PIC_THEME1_PREFIX "linekey_dial.9.png"
#define PIC_THEME1_DIAL_LINEKEY_CUTLINE PIC_THEME1_PREFIX "linekey_dial_cutline.png"
#define     PIC_THEME1_LONG_DIAL_LINEKEY                    PIC_THEME1_DIAL_LINEKEY
#define PIC_THEME1_EXTEND_DIAL_LINEKEY  PIC_THEME1_PREFIX "extend_linekey_dial.png"
#define PIC_THEME1_LINEKEY_PAGE PIC_THEME1_PREFIX "linekey_page_normal.png"
#define PIC_THEME1_MESSAGE  PIC_THEME1_PREFIX "message.png"
#define PIC_THEME1_SCROLLBAR    PIC_THEME1_PREFIX "scrollbar_normal.png"
#define PIC_THEME1_SCROLLDOWN   PIC_THEME1_PREFIX "scroll_down.png"
#define PIC_THEME1_SCROLLUP PIC_THEME1_PREFIX "scroll_up.png"
#define PIC_THEME1_SETTING  PIC_THEME1_PREFIX "setting.png"
#define PIC_THEME1_SOFTKEY  PIC_THEME1_PREFIX "softkey_normal.png"
#define PIC_THEME1_STATUS   PIC_THEME1_PREFIX "status.png"
#define PIC_THEME1_TITLEBIG PIC_THEME1_PREFIX "titlebar.9.png"
#define PIC_THEME1_TITLEBIG_TRANS           PIC_THEME1_PREFIX "titlebar_transparency.9.png"
#define PIC_THEME1_TITLEBIG_TRANS_80_TRANS  PIC_THEME1_PREFIX "titlebar_transparency_80_trans.9.png"
#define PIC_THEME1_TITLEBIG_TRANS_60_TRANS  PIC_THEME1_PREFIX "titlebar_transparency_60_trans.9.png"
#define PIC_THEME1_TITLEBIG_TRANS_40_TRANS  PIC_THEME1_PREFIX "titlebar_transparency_40_trans.9.png"
#define PIC_THEME1_TITLEBIG_TRANS_20_TRANS  PIC_THEME1_PREFIX "titlebar_transparency_20_trans.9.png"
#define PIC_THEME1_TITLEBIG_TRANS_LOW_TRANS PIC_THEME1_PREFIX "titlebar_transparency_low_trans.9.png"

#define     PIC_THEME1_TITLESMALL                           PIC_THEME1_TITLEBIG
#define PIC_THEME1_SECOND_MENU_BG   PIC_THEME1_PREFIX "secondary_menu_bg.png"
#define PIC_THEME1_TITLEBAR_CUTLINE PIC_THEME1_PREFIX "titlebar_cutline.png"
#define     PIC_THEME1_TITLELITTLE                          PIC_THEME1_TITLEBIG
#define PIC_SOFTKEY_BACKGROUND  PIC_THEME1_PREFIX "softkey_normal.9.png"
#define PIC_SOFTKEY_BACKGROUND_TRANS    PIC_THEME1_PREFIX "softkey_normal_transparency.9.png"
#define PIC_SOFTKEY_BACKGROUND_TRANS_80_TRANS   PIC_THEME1_PREFIX "softkey_normal_transparency_80_trans.9.png"
#define PIC_SOFTKEY_BACKGROUND_TRANS_60_TRANS   PIC_THEME1_PREFIX "softkey_normal_transparency_60_trans.9.png"
#define PIC_SOFTKEY_BACKGROUND_TRANS_40_TRANS   PIC_THEME1_PREFIX "softkey_normal_transparency_40_trans.9.png"
#define PIC_SOFTKEY_BACKGROUND_TRANS_20_TRANS   PIC_THEME1_PREFIX "softkey_normal_transparency_20_trans.9.png"
#define PIC_SOFTKEY_BACKGROUND_TRANS_LOW_TRANS  PIC_THEME1_PREFIX "softkey_normal_transparency_low_trans.9.png"
#define PIC_SOFTKEY_CUTLINE PIC_THEME1_PREFIX "softkey_normal_cutline.png"

#define PIC_LOAD_IMAGE_FAIL     PIC_ICON_PREFIX "browse_image_fail.png"

#endif //T4X_PIC_PATH_H

//#endif


