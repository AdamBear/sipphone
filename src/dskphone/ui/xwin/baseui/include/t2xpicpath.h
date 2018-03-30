#ifndef __T2X_PIC_PATH_H__
#define __T2X_PIC_PATH_H__


#define PIC_COMMON_PATH     "images/"
#define PIC_THEME_PATH              "Theme1/"

#if IS_COLOR
#define PIC_COMMON_ANIMATION_PATH      PIC_COMMON_PATH "animation/"
#define PIC_COMMON_ICON_PATH           PIC_COMMON_PATH "icon/"
#define PIC_COMMON_WIFI_PATH           PIC_COMMON_PATH  "wifi/"
#define PIC_DEFAULT_MAINMENU_ITEM_BG    PIC_COMMON_PATH "menu/Theme#menu@menu_selected.png"
#define PIC_DEFAULT_FOCUS_ITEM_BG       PIC_THEME_PATH "Theme#Theme1@item_background.9.png"
#define PIC_DEFAULT_THEME_BG          PIC_THEME_PATH "Theme#Theme1@background_normal.jpg"
#define PIC_DEFAULT_NORMAL_OPTION_ITEM_BG        PIC_COMMON_PATH "widget/Theme#widget@option_background.9.png"
#define PIC_DEFAULT_FOCUS_OPTION_ITEM_BG        PIC_COMMON_PATH "widget/Theme#widget@option_selected.9.png"

//arrow
#define PIC_ARROW_PATH      PIC_COMMON_PATH "arrows/"
#define PIC_LEFT_RIGHT_ARROR     PIC_ARROW_PATH "Theme#arrows@combox_icon.png"

//contact
#define PIC_CONTACT_PATH       PIC_COMMON_PATH "contacts/"
#define PIC_CONTACT_NORMAL          PIC_CONTACT_PATH "Theme#contacts@contact_icon.png"
#define PIC_CONTACT_FAVORITE        PIC_CONTACT_PATH "Theme#contacts@contact_favorite_icon.png"
#define PIC_CONTACTS_DEFAULT_IMAGE  PIC_CONTACT_PATH "Theme#contacts@default_contact_image.png"

//dsskey
#define PIC_DSSKEY_PATH       PIC_COMMON_PATH "dss/"
#define PIC_THEME1_LINEKEY  PIC_THEME_PATH "Theme#Theme1@linekey_normal.9.png"
#define PIC_THEME1_LONG_LINEKEY                         PIC_THEME1_LINEKEY
#define PIC_THEME1_HIGHLIGHT_LINEKEY    PIC_THEME_PATH "Theme#Theme1@linekey_highlight.9.png"
#define PIC_THEME1_LONG_HIGHLIGHT_LINEKEY               PIC_THEME1_HIGHLIGHT_LINEKEY
#define PIC_THEME1_DISABLED_LINEKEY PIC_THEME_PATH "Theme#Theme1@linekey_disabled.9.png"
#define PIC_THEME1_LONG_DISABLED_LINEKEY                PIC_THEME1_DISABLED_LINEKEY
#define PIC_THEME1_DIAL_LINEKEY PIC_THEME_PATH "Theme#Theme1@linekey_dial.9.png"
#define PIC_THEME1_LONG_DIAL_LINEKEY                    PIC_THEME1_DIAL_LINEKEY

#define PIC_DSS_EDK PIC_DSSKEY_PATH "Theme#dss@dss_edk.png"
#define PIC_DSS_TALK    PIC_DSSKEY_PATH "Theme#dss@dss_talk.png"  //通话功能
#define PIC_DSS_SPEED_DIAL  PIC_DSSKEY_PATH "Theme#dss@dss_speed_dial.png"     //快捷呼出
#define PIC_DSS_BLF_IDLE    PIC_DSSKEY_PATH "Theme#dss@dss_blf_idle.png"           // BLF 空闲
#define PIC_DSS_BLF_CALLOUT PIC_DSSKEY_PATH "Theme#dss@dss_blf_callout.png"        // BLF 回铃
#define PIC_DSS_BLF_RING    PIC_DSSKEY_PATH "Theme#dss@dss_blf_ringing.png"        // BLF 响铃
#define PIC_DSS_BLF_TALKING PIC_DSSKEY_PATH "Theme#dss@dss_blf_talking.png"        // BLF 通话中
#define PIC_DSS_BLF_HOLD    PIC_DSSKEY_PATH "Theme#dss@dss_blf_hold.png"           // BLF 保持
#define PIC_DSS_BLF_PARK    PIC_DSSKEY_PATH "Theme#dss@dss_blf_park.png"           // BLF 保持
#define PIC_DSS_BLF_UNKNOWN PIC_DSSKEY_PATH "Theme#dss@dss_blf_unknown.png"        // BLF 未知
#define PIC_DSS_LINE    PIC_DSSKEY_PATH "Theme#dss@dss_line.png"                   // Line线路 正常
#define PIC_DSS_LINE_UNREGISTERED   PIC_DSSKEY_PATH "Theme#dss@dss_line_unregistered.png"//  Line线路 未注册
#define PIC_DSS_LINE_FAIL   PIC_DSSKEY_PATH "Theme#dss@dss_line_fail.png"          // Line线路 注册失败
#define PIC_DSS_LINE_REGISTERING    PIC_DSSKEY_PATH "Theme#dss@dss_line_registering.png"  // Line线路 注册中
#define PIC_DSS_LINE_REGISTERED PIC_DSSKEY_PATH "Theme#dss@dss_line_registered.png"   // Line线路 注册完成
#define PIC_DSS_LINE_USING  PIC_DSSKEY_PATH "Theme#dss@dss_line_using.png"         // Line线路 正被使用
#define PIC_DSS_LINE_FORWARD    PIC_DSSKEY_PATH "Theme#dss@dss_line_forward.png"           // Line线路 转接
#define PIC_DSS_LINE_DND    PIC_DSSKEY_PATH "Theme#dss@dss_line_dnd.png"               // Line线路 免打扰
#define PIC_DSS_URL PIC_DSSKEY_PATH "Theme#dss@dss_URL.png"                    //网络检测
#define PIC_DSS_URL_GREEN   PIC_DSSKEY_PATH "Theme#dss@dss_URL_green.png"          // 网络检测成功
#define PIC_DSS_URL_RED PIC_DSSKEY_PATH "Theme#dss@dss_URL_red.png"                // 网络检测不可用
#define PIC_DSS_BLA_IDLE    PIC_DSSKEY_PATH "Theme#dss@dss_BLA_idle.png"                   //BLA
#define PIC_DSS_BLA_SEIZED  PIC_DSSKEY_PATH "Theme#dss@dss_BLA_seized.png"             // 占线
#define PIC_DSS_BLA_PROGRESSING PIC_DSSKEY_PATH "Theme#dss@dss_BLA_progressing.png"            // 回铃    
#define PIC_DSS_BLA_ALERTING    PIC_DSSKEY_PATH "Theme#dss@dss_BLA_alerting.png"               // 振铃 
#define PIC_DSS_BLA_ACTIVE  PIC_DSSKEY_PATH "Theme#dss@dss_BLA_active.png"             // 接通    
#define PIC_DSS_BLA_HELD    PIC_DSSKEY_PATH "Theme#dss@dss_BLA_held.png"                   // public hold，群组内任何人都可以retrieve
#define PIC_DSS_BLA_HELD_PRIVATE    PIC_DSSKEY_PATH "Theme#dss@dss_BLA_held_private.png"           // prive hold，只有hold的那个人能retrieve 
#define PIC_DSS_BLA_BRIDGE_ACTIVE   PIC_DSSKEY_PATH "Theme#dss@dss_BLA_bridge_active.png"      // 多方通话，至少有两方处于通话状态
#define PIC_DSS_BLA_BRIDGE_HELD PIC_DSSKEY_PATH "Theme#dss@dss_BLA_bridge_held.png"            // 多方通话，群组成员都按hold     
#define PIC_DSS_BLA_PARK    PIC_DSSKEY_PATH "Theme#dss@dss_BLA_park.png"                   // 有电话Park进来  
#define PIC_DSS_BLA_UNKNOWN PIC_DSSKEY_PATH "Theme#dss@dss_BLA_unknown.png"                // BLA不成功
#define PIC_DSS_XML_BROWSER PIC_DSSKEY_PATH "Theme#dss@dss_XML_browser_normal.png"
#define PIC_DSS_NONE    PIC_DSSKEY_PATH "Theme#dss@dss_none.png"
#define PIC_DSS_INTERCOM    PIC_DSSKEY_PATH "Theme#dss@dss_intercom.png"
#define PIC_DSS_INTERCOM_RINGING    PIC_DSSKEY_PATH "Theme#dss@dss_intercom_ringing.png"
#define PIC_DSS_INTERCOM_TALKING    PIC_DSSKEY_PATH "Theme#dss@dss_intercom_talking.png"
#define PIC_DSS_INTERCOM_HOLD   PIC_DSSKEY_PATH "Theme#dss@dss_intercom_hold.png"
#define PIC_DSS_INTERCOM_UNKNOWN    PIC_DSSKEY_PATH "Theme#dss@dss_intercom_unknow.png"
#define PIC_DSS_PICKUP  PIC_DSSKEY_PATH "Theme#dss@dss_pickup.png"
#define PIC_DSS_GROUPPICKUP PIC_DSSKEY_PATH "Theme#dss@dss_grouppickup.png"
#define PIC_DSS_CALLPARK    PIC_DSSKEY_PATH "Theme#dss@dss_callpark.png"                   //通话暂留
#define PIC_DSS_CALLPARK_RINGING    PIC_DSSKEY_PATH "Theme#dss@dss_callpark_ringing.png"           //通话暂留响铃
#define PIC_DSS_CALLPARK_MUNUAL_TALK    PIC_DSSKEY_PATH "Theme#dss@dss_callpark_manual_talk.png"       //通话暂留手动接起通话
#define PIC_DSS_CALLPARK_AUTO_TALK  PIC_DSSKEY_PATH "Theme#dss@dss_callpark_auto_talk.png"     //通话暂留自动接起通话
#define PIC_DSS_CALLPARK_FAIL   PIC_DSSKEY_PATH "Theme#dss@dss_callpark_failed.png"            //通话暂留失败
#define PIC_DSS_DIRECTORY       PIC_DSSKEY_PATH "Theme#dss@dss_directory.png" //地址簿
#define PIC_DSS_GET_BALANCE     PIC_DSSKEY_PATH "Theme#dss@dss_getbalance.png" //GetBalance
#define PIC_DSS_DTMF    PIC_DSSKEY_PATH "Theme#dss@dss_dtmf.png"
#define PIC_DSS_LOCALGROUP  PIC_DSSKEY_PATH "Theme#dss@dss_localgroup.png"     // 地址簿
#define PIC_DSS_XMLGROUP    PIC_DSSKEY_PATH "Theme#dss@dss_xmlgroup.png"           // xml地址簿
#define PIC_DSS_LDAP    PIC_DSSKEY_PATH "Theme#dss@dss_ldap.png"               // ldap地址簿
#define PIC_DSS_CONFERENCE  PIC_DSSKEY_PATH "Theme#dss@dss_conference.png"     // 会议
#define PIC_DSS_HOLD    PIC_DSSKEY_PATH "Theme#dss@dss_hold.png"               // 通话暂停
#define         PIC_DSS_PUBLIC_HOLD             PIC_DSS_HOLD                        // 通话暂停
#define         PIC_DSS_PRIVATE_HOLD            PIC_DSS_HOLD                        // 通话暂停
#define PIC_DSS_TRANSFER    PIC_DSSKEY_PATH "Theme#dss@dss_transfer.png"           // 呼叫转移
#define PIC_DSS_FORWARD PIC_DSSKEY_PATH "Theme#dss@dss_forward.png"
#define PIC_DSS_DND PIC_DSSKEY_PATH "Theme#dss@dss_dnd.png"
#define PIC_DSS_DND_OFF PIC_DSSKEY_PATH "Theme#dss@dss_dnd_off.png"
#define PIC_DSS_REDIAL  PIC_DSSKEY_PATH "Theme#dss@dss_redial.png"
#define PIC_DSS_CALLRETURN  PIC_DSSKEY_PATH "Theme#dss@dss_callreturn.png"     // 回拨
#define PIC_DSS_SMS PIC_DSSKEY_PATH "Theme#dss@dss_sms.png"
#define PIC_DSS_RECORD  PIC_DSSKEY_PATH "Theme#dss@dss_record.png"
#define PIC_DSS_RECORDING   PIC_DSSKEY_PATH "Theme#dss@dss_recording.png"      // 正在录音
#define PIC_DSS_PAGING  PIC_DSSKEY_PATH "Theme#dss@dss_paging.png"
#define PIC_DSS_GROUP_LISTENING PIC_DSSKEY_PATH "Theme#dss@dss_group_listening.png"
#define PIC_DSS_ZERO_TOUCH  PIC_DSSKEY_PATH "Theme#dss@dss_zero_touch.png"
#define PIC_DSS_ACD PIC_DSSKEY_PATH "Theme#dss@dss_acd.png"
#define PIC_DSS_HOTDESKING  PIC_DSSKEY_PATH "Theme#dss@dss_hotdesking.png"
#define PIC_DSS_VOICE_MAIL  PIC_DSSKEY_PATH "Theme#dss@dss_voice_mail.png"
#define PIC_DSS_HOTELING_LOGININ    PIC_DSSKEY_PATH "Theme#dss@dss_hoteling_in.png"
#define PIC_DSS_HOTELING_LOGINOUT   PIC_DSSKEY_PATH "dss_hoteling_out.png"
#define PIC_DSS_FAVORITES   PIC_DSSKEY_PATH "Theme#dss@dss_favorites.png"      // 收藏夹
#define PIC_DSS_UC_ONLINE   PIC_DSSKEY_PATH "Theme#dss@dss_uc_online.png"      // 在线
#define PIC_DSS_UC_AWAY PIC_DSSKEY_PATH "Theme#dss@dss_uc_away.png"            // 离开
#define PIC_DSS_UC_BUSY PIC_DSSKEY_PATH "Theme#dss@dss_uc_busy.png"            // 忙碌
#define PIC_DSS_UC_OFFLINE  PIC_DSSKEY_PATH "Theme#dss@dss_uc_offline.png"     // 离线
#define PIC_DSS_KEYPAD_LOCK PIC_DSSKEY_PATH "Theme#dss@dss_keypad_lock.png"
#define PIC_DSS_BUDDIES PIC_DSSKEY_PATH "Theme#dss@dss_buddies.png"
#define PIC_DSS_ACD_TRACE   PIC_DSSKEY_PATH "Theme#dss@dss_originated_trace.png"
#define PIC_DSS_DISPCODE    PIC_DSSKEY_PATH "Theme#dss@dss_disposition_code.png"
#define PIC_DSS_ESCALATE    PIC_DSSKEY_PATH "Theme#dss@dss_emergency_escalation.png"
#define PIC_DSS_MEET_ME PIC_DSSKEY_PATH "Theme#dss@dss_meetme_conference.png"
#define PIC_DSS_STATUS  PIC_DSSKEY_PATH "Theme#dss@dss_status.png"
#define PIC_DSS_ACD_AVAILABLE   PIC_DSSKEY_PATH "Theme#dss@dss_acd_available.png"
#define PIC_DSS_ACD_UNAVAILABLE PIC_DSSKEY_PATH "Theme#dss@dss_acd_unavailable.png"
#define PIC_DSS_ACD_WARP_UP PIC_DSSKEY_PATH "Theme#dss@dss_acd_wrapup.png"
#define PIC_DSS_ACD_LOGOUT  PIC_DSSKEY_PATH "Theme#dss@dss_acd_logout.png"
#define PIC_DSS_RETRIEVE_PARK   PIC_DSSKEY_PATH "Theme#dss@dss_retrieve_park.png"
#define PIC_DSS_MOBILE_BLUE_GREEN   PIC_DSSKEY_PATH "Theme#dss@dss_mobile_blue_green.png"
#define PIC_DSS_MOBILE_BLUE_RED     PIC_DSSKEY_PATH "Theme#dss@dss_mobile_blue_red.png"
#define PIC_DSS_MOBILE_BLUE_CONNETING   PIC_DSSKEY_PATH "Theme#dss@dss_mobile_blue_conneting.png"
#define PIC_DSS_EDITING             PIC_DSSKEY_PATH "Theme#dss@dss_editing.png"

#define PIC_DSS_PREFIX_ICON     PIC_DSSKEY_PATH "Theme#dss@dss_prefix.png"
#define PIC_DSS_ENCRYPT_ICON    PIC_DSSKEY_PATH "Theme#dss@dss_encrypt.png"

#define PIC_DSS_MTSW_LOGOUT         PIC_DSSKEY_PATH "Theme#dss@dss_mtsw_logout.png"
#define PIC_DSS_MTSW_MESSAGELIST    PIC_DSSKEY_PATH "Theme#dss@dss_mtsw_messagelist.png"

//softkey
#define PIC_THEME1_SOFTKEY_TRANSPARENT_BG  PIC_THEME_PATH "Theme#Theme1@softkey_normal_transparency.9.png"
#define PIC_THEME1_SOFTKEY_NORMAL_BG  PIC_THEME_PATH "Theme#Theme1@softkey_normal.9.png"

//dsskeyline
#define PIC_THEME1_LINEKEY_CUTLINE  PIC_THEME_PATH "Theme#Theme1@linekey_normal_cutline.png"
#define PIC_THEME1_DISABLED_LINEKEY_CUTLINE PIC_THEME_PATH "Theme#Theme1@linekey_disabled_cutline.png"
#define PIC_THEME1_HIGHLIGHT_LINEKEY_CUTLINE    PIC_THEME_PATH "Theme#Theme1@linekey_highlight_cutline.png"
#define PIC_THEME1_DIAL_LINEKEY_CUTLINE PIC_THEME_PATH "Theme#Theme1@linekey_dial_cutline.png"

// dss pagebutton
//填充红色
#define     PAGE_ICON_RED               PIC_COMMON_ICON_PATH "Theme#icon@dsskey_page_red.9.png"
//填充绿色
#define     PAGE_ICON_GREEN             PIC_COMMON_ICON_PATH "Theme#icon@dsskey_page_green.9.png"
//填充灰色
#define     PAGE_ICON_GRAY              PIC_COMMON_ICON_PATH "Theme#icon@dsskey_page_gray.9.png"
//灰色环图标
#define     PAGE_ICON_GRAY_CIRCLE       PIC_COMMON_ICON_PATH "Theme#icon@dsskey_page_gray_circle.9.png"

//icon
#define PIC_ICON_PATH       PIC_COMMON_PATH "icon/"
#define PIC_CALL_LIST_OUT           PIC_ICON_PATH "Theme#icon@call_out.png"
#define PIC_CALL_LIST_IN            PIC_ICON_PATH "Theme#icon@call_in.png"
#define PIC_CALL_LIST_MISS          PIC_ICON_PATH "Theme#icon@missedcall_big.png"
#define PIC_ALL_LIST_FORWARD        PIC_ICON_PATH "Theme#icon@forward_big.png"

#if FEATURE_BROADTOUCH_UC
#define PIC_ICON_PRESENCE_AWAY  PIC_ICON_PATH "Theme#icon@icon_presence_away.png"
#define PIC_ICON_PRESENCE_BUSY  PIC_ICON_PATH "Theme#icon@icon_presence_busy.png"
#define PIC_ICON_PRESENCE_CHATTY    PIC_ICON_PATH "Theme#icon@icon_presence_chatty.png"
#define PIC_ICON_PRESENCE_EXTENDEDAWAY  PIC_ICON_PATH "Theme#icon@icon_presence_extended_away.png"
#define PIC_ICON_PRESENCE_OFFLINE   PIC_ICON_PATH "Theme#icon@icon_presence_offline.png"
#define PIC_ICON_PRESENCE_ONLINE    PIC_ICON_PATH "Theme#icon@icon_presence_online.png"
#define PIC_ICON_PRESENCE_UNKNOWN   PIC_ICON_PATH "Theme#icon@icon_presence_unknown.png"

#define PIC_ICON_PRESENCE_NEW_OFFLINE   PIC_ICON_PATH "Theme#icon@icon_presence_new_offline.png"
#define PIC_ICON_PRESENCE_NEW_ONLINE    PIC_ICON_PATH "Theme#icon@icon_presence_new_online.png"
#define PIC_ICON_PRESENCE_NEW_AWAY  PIC_ICON_PATH "Theme#icon@icon_presence_new_away.png"
#define PIC_ICON_PRESENCE_NEW_BUSY  PIC_ICON_PATH "Theme#icon@icon_presence_new_busy.png"
#define PIC_ICON_PRESENCE_NEW_UNKNOWN   PIC_ICON_PATH "Theme#icon@icon_presence_new_unknown.png"
#endif

#define PIC_LEFT_KEY                PIC_COMMON_PATH "leftkey.dob"
#define PIC_RIGHT_KEY               PIC_COMMON_PATH "rightkey.dob"
#define PIC_PLAY_LEFT               PIC_COMMON_PATH "playleft.dob"
#define PIC_PLAY_RIGHT              PIC_COMMON_PATH "playright.dob"

#define PIC_MSGBOX_PATH       PIC_COMMON_PATH "msgbox/"
#define PIC_CHECK_ON        PIC_MSGBOX_PATH "Theme#msgbox@search_option_choose.png"
#define PIC_CHECK_OFF       PIC_MSGBOX_PATH "Theme#msgbox@msgbox_option_normal.png"

// account
#define PIC_ACCOUNT_DISABLE                 PIC_DSSKEY_PATH "Theme#dss@dss_line.png"
#define PIC_ACCOUNT_LOGING                  PIC_COMMON_PATH "logging_small.dob"
#define PIC_ACCOUNT_REGISTED                PIC_DSSKEY_PATH "Theme#dss@dss_line_registered.png"
#define PIC_ACCOUNT_REGISTEDFAIL            PIC_COMMON_PATH "registedfail_small.dob"
#define PIC_ACCOUNT_UNREGISTER              PIC_COMMON_PATH "unregister_small.dob"

#define PIC_ACCOUNT_FORWARD                 PIC_COMMON_PATH "LineForward.dob"
#define PIC_ACCOUNT_DND                     PIC_COMMON_PATH "LineDND.dob"
#define PIC_ACCOUNT_HEADSET                 PIC_COMMON_PATH "Theme#icon@head_set_small.png"
#define PIC_ACCOUNT_SCA_REGISTED            PIC_COMMON_PATH "sca_registed_small.dob"
#define PIC_ACCOUNT_CLOUD_REGISTED          PIC_COMMON_PATH "cloud_registed_small.dob"

#define PIC_ACCOUNT_ACD_LOGOUT              PIC_COMMON_PATH "acd-logout.dob"
#define PIC_ACCOUNT_ACD_LOGIN               PIC_COMMON_PATH "acd-login.dob"
#define PIC_ACCOUNT_ACD_UNAVAILABLE         PIC_COMMON_PATH "acd-unavailable.dob"
#define PIC_ACCOUNT_ACD_WRAPUP              PIC_COMMON_PATH "acd-wrapup.dob"

#define PIC_ACCOUNT_ACD_LOGOUT              PIC_COMMON_PATH "acd-logout.dob"
#define PIC_ACCOUNT_ACD_LOGIN               PIC_COMMON_PATH "acd-login.dob"
#define PIC_ACCOUNT_ACD_UNAVAILABLE         PIC_COMMON_PATH "acd-unavailable.dob"
#define PIC_ACCOUNT_ACD_WRAPUP              PIC_COMMON_PATH "acd-wrapup.dob"

// idle
#define PIC_IDLE_DND                        PIC_ICON_PATH "Theme#icon@DND.png"
#define PIC_IDLE_AA                         PIC_ICON_PATH "Theme#icon@auto_answer.png"
#define PIC_IDLE_FWD                        PIC_ICON_PATH "Theme#icon@forward_small.png"
#define PIC_IDLE_LOCK                       PIC_ICON_PATH "Theme#icon@lock_icon.png"
#define PIC_IDLE_RING_MUTE                  PIC_ICON_PATH "Theme#icon@ring_mute.png"
#define PIC_IDLE_VOICEMAIL                  PIC_ICON_PATH "Theme#icon@voice_mail.png"
#define PIC_IDLE_TEXTMESSAGE                PIC_ICON_PATH "Theme#icon@text_message.png"
#define PIC_IDLE_MISSCALL                   PIC_ICON_PATH "Theme#icon@missedcall_small.png"
#define PIC_IDLE_VPN                        PIC_ICON_PATH "Theme#icon@VPN.png"
#define PIC_IDLE_INTERNET                   PIC_ICON_PATH "Theme#icon@net2.png"

#define PIC_IDLE_USB_DEVICE                 PIC_ICON_PATH "Theme#icon@titlebar_usb.png"
#define PIC_IDLE_USB_DEVICE_DETECTING       PIC_ICON_PATH "Theme#icon@titlebar_usb_detecting.png"
#define PIC_IDLE_USB_RECORD                 PIC_ICON_PATH "Theme#icon@record_start.png"
#define PIC_IDLE_USB_RECORD_PAUSE           PIC_ICON_PATH "Theme#icon@record_pause.png"

#define PIC_IDLE_TALK_MUTE                  PIC_ICON_PATH "Theme#icon@talk_mute.png"
#define PIC_IDLE_PSTN                       PIC_ICON_PATH "Theme#icon@pstn.png"

#define PIC_IDLE_BLUETOOTH                  PIC_ICON_PATH "Theme#icon@bluetooth_common.png"
#define PIC_IDLE_BLUETOOTH_CONNECTED        PIC_ICON_PATH "Theme#icon@bluetooth_headset.png"
#define PIC_IDLE_BLUETOOTH_MOBILE_CONNECTED PIC_ICON_PATH "Theme#icon@bluetooth_mobile_icon.png"

#define PIC_IDLE_WIFI                       PIC_ICON_PATH "Theme#icon@wifi_topbar_on.png"
#define PIC_IDLE_WIFI_CONNECTED             PIC_ICON_PATH "Theme#icon@wifi_topbar_connect.png"

#define PIC_IDLE_CABLE_ON                   PIC_COMMON_PATH "Theme#icon@net2.png"
#define PIC_IDLE_CABLE_OFF                  PIC_COMMON_PATH "Theme#icon@net2.png"

// bluetooth and wifi
#define PIC_BT_BLUETOOTH_ICON               PIC_COMMON_PATH "bluetooth.dob"                 // 其他类型蓝牙列表图标
#define PIC_BT_BLUETOOTH_AC_ICON            PIC_COMMON_PATH "bluetooth_auto.dob"            // 其他类型自动连接蓝牙列表图标
#define PIC_BT_HEADSET_ICON                 PIC_COMMON_PATH "bluetooth_headset.dob"         // 耳机类型蓝牙列表图标
#define PIC_BT_HEADSET_AC_ICON              PIC_COMMON_PATH "bluetooth_headset_auto.dob"    // 耳机类型蓝牙自动连接列表图标
#define PIC_BT_PHONE_ICON                   PIC_COMMON_PATH "bluetooth_mobile_icon.dob"     // 手机类型蓝牙列表图标
#define PIC_BT_PHONE_AC_ICON                PIC_COMMON_PATH "bluetooth_mobile_auto.dob"     // 手机类型蓝牙自动连接列表图标

//T52_WIFI
#define PIC_WIFI_LIST_SIGNAL_UNCONNECTED_LOCK_1      PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_lock_unconnected_2.png"
#define PIC_WIFI_LIST_SIGNAL_UNCONNECTED_LOCK_2      PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_lock_unconnected_3.png"
#define PIC_WIFI_LIST_SIGNAL_UNCONNECTED_LOCK_3      PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_lock_unconnected_4.png"
#define PIC_WIFI_LIST_SIGNAL_UNCONNECTED_LOCK_4      PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_lock_unconnected_5.png"
#define PIC_WIFI_LIST_SIGNAL_UNCONNECTED_UNLOCK_1    PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_nolock_unconnected_2.png"
#define PIC_WIFI_LIST_SIGNAL_UNCONNECTED_UNLOCK_2    PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_nolock_unconnected_3.png"
#define PIC_WIFI_LIST_SIGNAL_UNCONNECTED_UNLOCK_3    PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_nolock_unconnected_4.png"
#define PIC_WIFI_LIST_SIGNAL_UNCONNECTED_UNLOCK_4    PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_nolock_unconnected_5.png"
#define PIC_WIFI_LIST_SIGNAL_CONNECTED_LOCK_1        PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_lock_connected_2.png"
#define PIC_WIFI_LIST_SIGNAL_CONNECTED_LOCK_2        PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_lock_connected_3.png"
#define PIC_WIFI_LIST_SIGNAL_CONNECTED_LOCK_3        PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_lock_connected_4.png"
#define PIC_WIFI_LIST_SIGNAL_CONNECTED_LOCK_4        PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_lock_connected_5.png"
#define PIC_WIFI_LIST_SIGNAL_CONNECTED_UNLOCK_1      PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_2.png"
#define PIC_WIFI_LIST_SIGNAL_CONNECTED_UNLOCK_2      PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_3.png"
#define PIC_WIFI_LIST_SIGNAL_CONNECTED_UNLOCK_3      PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_4.png"
#define PIC_WIFI_LIST_SIGNAL_CONNECTED_UNLOCK_4      PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_5.png"

#define PIC_WIRELESS_DISCONNECT_ICON        PIC_COMMON_PATH "wireless_disconnect.dob"       // 设备断开连接的图标
#define PIC_WIRELESS_CONNECTING_ICON        PIC_COMMON_WIFI_PATH "Theme#wifi@wifi_connecting.png"       // 设备正在连接的图标
#define PIC_WIRELESS_ENCRYPTED_ICON         PIC_COMMON_WIFI_PATH  "Theme#wifi@wifi_lock.png"                // 设备加密图标
#define PIC_WIRELESS_CONNECTED_ICON         PIC_COMMON_WIFI_PATH "Theme#wifi@wifi_connected.png"                //蓝牙设备连接成功的图标

// usb record
#define PIC_USB_RECORD_START                PIC_COMMON_PATH "RecordStart.dob"
#define PIC_USB_RECORD_STOP                 PIC_COMMON_PATH "RecordStop.dob"
#define PIC_USB_RECORD_PAUSE                PIC_COMMON_PATH "RecordPause.dob"

#define PIC_GENERAL_MESSAGE_UNREAD          PIC_COMMON_PATH "msg-unread.dob"
#define PIC_GENERAL_MESSAGE_READ            PIC_COMMON_PATH "msg-readed.dob"
#define PIC_GENERAL_MESSAGE_DRAFT           PIC_GENERAL_MESSAGE_READ
#define PIC_GENERAL_MESSAGE_SENDING         PIC_GENERAL_MESSAGE_READ
#define PIC_GENERAL_MESSAGE_SEND            PIC_GENERAL_MESSAGE_READ

#define PIC_GENERAL_PATH                    PIC_COMMON_PATH "general/"
#define PIC_VOICE_MAIL_READ                 PIC_GENERAL_PATH "Theme#general@voicemail_in_read.png"
#define PIC_VOICE_MAIL_UNREAD               PIC_GENERAL_PATH "Theme#general@voicemail_in_unread.png"
#define PIC_ICON_PLAY                       PIC_ICON_PATH "Theme#icon@play.png"
#define PIC_ICON_PAUSE                      PIC_ICON_PATH "Theme#icon@pause.png"

// volumebar
#define PIC_VOLUME_PATH                          PIC_COMMON_PATH "volume/"
#define PIC_VOLUME_BAR_SLIDER               PIC_VOLUME_PATH "Theme#volume@volume_slider.png"
#define PIC_VOLUME_BAR_EMPTY               PIC_VOLUME_PATH "Theme#volume@volume_empty.png"
#define PIC_VOLUME_BAR_FULL                  PIC_VOLUME_PATH "Theme#volume@volume_full.png"

#define PIC_VOLUME_BAR_DIAL_BG             PIC_VOLUME_PATH "Theme#volume@volume_dial_background.png"
#define PIC_VOLUME_BAR_DIAL_BG_D9       PIC_VOLUME_PATH "Theme#volume@volume_dial_background_do9.9.png"

#define PIC_CHANNEL_ICON_HANDFREE   PIC_ICON_PATH   "Theme#icon@hand_free.png"
#define PIC_CHANNEL_ICON_HANDSET   PIC_ICON_PATH   "Theme#icon@hand_set.png"
#define PIC_CHANNEL_ICON_HEADSET_BIG   PIC_ICON_PATH   "Theme#icon@head_set_big.png"

#define PIC_TALK_DIAL_SEARCH_FOCUS_ITEM_BG        PIC_THEME_PATH "Theme#Theme1@talk_item_background.9.png"

#define PIC_ICON_STATUS_ONE_WORD_HINTS_BG           PIC_ICON_PATH "Theme#icon@status_one_word_hints_bg.png"
#define PIC_ICON_STATUS_MORE_WORDS_HINTS_BG         PIC_ICON_PATH "Theme#icon@status_more_words_hints_bg.png"

#else

#define PIC_LEFT_RIGHT_ARROR        PIC_COMMON_PATH "LeftRight.dob"

#define PIC_CONTACT_NORMAL          PIC_COMMON_PATH"Contact.dob"

#define PIC_CALL_LIST_OUT           PIC_COMMON_PATH "calllist_out.dob"
#define PIC_CALL_LIST_IN            PIC_COMMON_PATH "calllist_in.dob"
#define PIC_CALL_LIST_MISS          PIC_COMMON_PATH "calllist_miss.dob"
#define PIC_ALL_LIST_FORWARD        PIC_COMMON_PATH "calllist_forward.dob"
#define PIC_LEFT_KEY                PIC_COMMON_PATH "leftkey.dob"
#define PIC_RIGHT_KEY               PIC_COMMON_PATH "rightkey.dob"

#define PIC_CHECK_ON        PIC_COMMON_PATH "checkicon.dob"
#define PIC_CHECK_OFF       PIC_COMMON_PATH ""

// dsskey
#define PIC_DSSKEY_PATH       PIC_COMMON_PATH "dss/"
#define PIC_THEME1_LINEKEY  PIC_THEME_PATH ""
#define PIC_THEME1_LONG_LINEKEY                         PIC_THEME1_LINEKEY
#define PIC_THEME1_HIGHLIGHT_LINEKEY    PIC_THEME_PATH ""
#define PIC_THEME1_LONG_HIGHLIGHT_LINEKEY               PIC_THEME1_HIGHLIGHT_LINEKEY
#define PIC_THEME1_DISABLED_LINEKEY PIC_THEME_PATH ""
#define PIC_THEME1_LONG_DISABLED_LINEKEY                PIC_THEME1_DISABLED_LINEKEY
#define PIC_THEME1_DIAL_LINEKEY PIC_THEME_PATH ""
#define PIC_THEME1_LONG_DIAL_LINEKEY                    PIC_THEME1_DIAL_LINEKEY

#define PIC_DSS_EDK PIC_DSSKEY_PATH ""
#define PIC_DSS_TALK    PIC_DSSKEY_PATH ""  //通话功能
#define PIC_DSS_SPEED_DIAL  PIC_DSSKEY_PATH ""     //快捷呼出
#define PIC_DSS_BLF_IDLE    PIC_DSSKEY_PATH ""           // BLF 空闲
#define PIC_DSS_BLF_CALLOUT PIC_DSSKEY_PATH ""        // BLF 回铃
#define PIC_DSS_BLF_RING    PIC_DSSKEY_PATH ""        // BLF 响铃
#define PIC_DSS_BLF_TALKING PIC_DSSKEY_PATH ""        // BLF 通话中
#define PIC_DSS_BLF_HOLD    PIC_DSSKEY_PATH ""            // BLF 保持
#define PIC_DSS_BLF_PARK    PIC_DSSKEY_PATH ""            // BLF 保持
#define PIC_DSS_BLF_UNKNOWN PIC_DSSKEY_PATH ""         // BLF 未知
#define PIC_DSS_LINE_BLA PIC_COMMON_PATH "sca_registed_small.dob"
#define PIC_DSS_LINE    PIC_DSSKEY_PATH ""                    // Line线路 正常
#define PIC_DSS_LINE_UNREGISTERED   PIC_COMMON_PATH "unregister_small.dob"//  Line线路 未注册
#define PIC_DSS_LINE_FAIL   PIC_DSSKEY_PATH ""           // Line线路 注册失败
#define PIC_DSS_LINE_REGISTERING    PIC_COMMON_PATH "logging_small.dob"  // Line线路 注册中
#define PIC_DSS_LINE_REGISTERED PIC_COMMON_PATH "registed_small.dob"   // Line线路 注册完成
#define PIC_DSS_LINE_RINGING PIC_COMMON_PATH "callin.dob"
#define PIC_DSS_LINE_CALLOUT PIC_COMMON_PATH "callout.dob"
#define PIC_DSS_LINE_USING  PIC_COMMON_PATH "talk_small.dob"         // Line线路 正被使用
#define PIC_DSS_LINE_FORWARD    PIC_DSSKEY_PATH "LineForward.dob"            // Line线路 转接
#define PIC_DSS_LINE_DND    PIC_DSSKEY_PATH "LineDND.dob"                // Line线路 免打扰
#define PIC_DSS_URL PIC_DSSKEY_PATH ""                     //网络检测
#define PIC_DSS_URL_GREEN   PIC_DSSKEY_PATH ""          // 网络检测成功
#define PIC_DSS_URL_RED PIC_DSSKEY_PATH ""                 // 网络检测不可用
#define PIC_DSS_BLA_IDLE    PIC_DSSKEY_PATH ""                   //BLA
#define PIC_DSS_BLA_SEIZED  PIC_DSSKEY_PATH ""              // 占线
#define PIC_DSS_BLA_PROGRESSING PIC_DSSKEY_PATH ""             // 回铃    
#define PIC_DSS_BLA_ALERTING    PIC_DSSKEY_PATH ""                // 振铃 
#define PIC_DSS_BLA_ACTIVE  PIC_DSSKEY_PATH ""              // 接通    
#define PIC_DSS_BLA_HELD    PIC_DSSKEY_PATH ""                    // public hold，群组内任何人都可以retrieve
#define PIC_DSS_BLA_HELD_PRIVATE    PIC_DSSKEY_PATH ""            // prive hold，只有hold的那个人能retrieve 
#define PIC_DSS_BLA_BRIDGE_ACTIVE   PIC_DSSKEY_PATH ""       // 多方通话，至少有两方处于通话状态
#define PIC_DSS_BLA_BRIDGE_HELD PIC_DSSKEY_PATH ""             // 多方通话，群组成员都按hold     
#define PIC_DSS_BLA_PARK    PIC_DSSKEY_PATH ""                   // 有电话Park进来  
#define PIC_DSS_BLA_UNKNOWN PIC_DSSKEY_PATH ""                // BLA不成功
#define PIC_DSS_XML_BROWSER PIC_DSSKEY_PATH ""
#define PIC_DSS_NONE    PIC_DSSKEY_PATH ""
#define PIC_DSS_INTERCOM    PIC_DSSKEY_PATH ""
#define PIC_DSS_INTERCOM_RINGING    PIC_DSSKEY_PATH ""
#define PIC_DSS_INTERCOM_TALKING    PIC_DSSKEY_PATH ""
#define PIC_DSS_INTERCOM_HOLD   PIC_DSSKEY_PATH ""
#define PIC_DSS_INTERCOM_UNKNOWN    PIC_DSSKEY_PATH ""
#define PIC_DSS_PICKUP  PIC_DSSKEY_PATH ""
#define PIC_DSS_GROUPPICKUP PIC_DSSKEY_PATH ""
#define PIC_DSS_CALLPARK    PIC_DSSKEY_PATH ""                    //通话暂留
#define PIC_DSS_CALLPARK_RINGING    PIC_DSSKEY_PATH ""            //通话暂留响铃
#define PIC_DSS_CALLPARK_MUNUAL_TALK    PIC_DSSKEY_PATH ""        //通话暂留手动接起通话
#define PIC_DSS_CALLPARK_AUTO_TALK  PIC_DSSKEY_PATH ""      //通话暂留自动接起通话
#define PIC_DSS_CALLPARK_FAIL   PIC_DSSKEY_PATH ""             //通话暂留失败
#define PIC_DSS_DIRECTORY       PIC_DSSKEY_PATH ""  //地址簿
#define PIC_DSS_GET_BALANCE     PIC_DSSKEY_PATH ""  //GetBalance
#define PIC_DSS_DTMF    PIC_DSSKEY_PATH ""
#define PIC_DSS_LOCALGROUP  PIC_DSSKEY_PATH ""      // 地址簿
#define PIC_DSS_XMLGROUP    PIC_DSSKEY_PATH ""            // xml地址簿
#define PIC_DSS_LDAP    PIC_DSSKEY_PATH ""                // ldap地址簿
#define PIC_DSS_CONFERENCE  PIC_DSSKEY_PATH ""      // 会议
#define PIC_DSS_HOLD    PIC_DSSKEY_PATH ""                // 通话暂停
#define         PIC_DSS_PUBLIC_HOLD             PIC_DSS_HOLD                        // 通话暂停
#define         PIC_DSS_PRIVATE_HOLD            PIC_DSS_HOLD                        // 通话暂停
#define PIC_DSS_TRANSFER    PIC_DSSKEY_PATH ""            // 呼叫转移
#define PIC_DSS_FORWARD PIC_COMMON_PATH "LineForward.dob"
#define PIC_DSS_DND PIC_COMMON_PATH "LineDND.dob"
#define PIC_DSS_DND_OFF PIC_DSSKEY_PATH ""
#define PIC_DSS_REDIAL  PIC_DSSKEY_PATH ""
#define PIC_DSS_CALLRETURN  PIC_DSSKEY_PATH ""      // 回拨
#define PIC_DSS_SMS PIC_DSSKEY_PATH ""
#define PIC_DSS_RECORD  PIC_DSSKEY_PATH ""
#define PIC_DSS_RECORDING   PIC_DSSKEY_PATH ""       // 正在录音
#define PIC_DSS_PAGING  PIC_DSSKEY_PATH ""
#define PIC_DSS_GROUP_LISTENING PIC_DSSKEY_PATH ""
#define PIC_DSS_ZERO_TOUCH  PIC_DSSKEY_PATH ""
#define PIC_DSS_ACD PIC_DSSKEY_PATH ""
#define PIC_DSS_HOTDESKING  PIC_DSSKEY_PATH ""
#define PIC_DSS_VOICE_MAIL  PIC_DSSKEY_PATH ""
#define PIC_DSS_HOTELING_LOGININ    PIC_DSSKEY_PATH ""
#define PIC_DSS_HOTELING_LOGINOUT   PIC_DSSKEY_PATH ""
#define PIC_DSS_FAVORITES   PIC_DSSKEY_PATH ""       // 收藏夹
#define PIC_DSS_UC_ONLINE   PIC_DSSKEY_PATH ""       // 在线
#define PIC_DSS_UC_AWAY PIC_DSSKEY_PATH ""             // 离开
#define PIC_DSS_UC_BUSY PIC_DSSKEY_PATH ""             // 忙碌
#define PIC_DSS_UC_OFFLINE  PIC_DSSKEY_PATH ""      // 离线
#define PIC_DSS_KEYPAD_LOCK PIC_DSSKEY_PATH ""
#define PIC_DSS_BUDDIES PIC_DSSKEY_PATH ""
#define PIC_DSS_ACD_TRACE   PIC_DSSKEY_PATH ""
#define PIC_DSS_DISPCODE    PIC_DSSKEY_PATH ""
#define PIC_DSS_ESCALATE    PIC_DSSKEY_PATH ""
#define PIC_DSS_MEET_ME PIC_DSSKEY_PATH ""
#define PIC_DSS_STATUS  PIC_DSSKEY_PATH ""
#define PIC_DSS_ACD_AVAILABLE   PIC_COMMON_PATH "acd-login.dob"
#define PIC_DSS_ACD_UNAVAILABLE PIC_COMMON_PATH "acd-unavailable.dob"
#define PIC_DSS_ACD_WARP_UP PIC_COMMON_PATH "acd-wrapup.dob"
#define PIC_DSS_ACD_LOGOUT  PIC_COMMON_PATH "acd-logout.dob"
#define PIC_DSS_RETRIEVE_PARK   PIC_DSSKEY_PATH ""
#define PIC_DSS_MOBILE_BLUE_GREEN   PIC_DSSKEY_PATH "dk_bluetooth_connected.dob"
#define PIC_DSS_MOBILE_BLUE_RED     PIC_DSSKEY_PATH "dk_bluetooth_disconnect.dob"
#define PIC_DSS_MOBILE_BLUE_CONNETING   PIC_DSSKEY_PATH "dk_bluetooth_connecting.dob"
#define PIC_DSS_EDITING             PIC_DSSKEY_PATH ""

#define PIC_DSS_PREFIX_ICON     PIC_DSSKEY_PATH ""
#define PIC_DSS_ENCRYPT_ICON    PIC_DSSKEY_PATH "encryption.dob"

#define PIC_DSS_MTSW_LOGOUT         PIC_DSSKEY_PATH ""
#define PIC_DSS_MTSW_MESSAGELIST    PIC_DSSKEY_PATH ""

//dsskeyline
#define PIC_THEME1_LINEKEY_CUTLINE  PIC_THEME_PATH ""
#define PIC_THEME1_DISABLED_LINEKEY_CUTLINE PIC_THEME_PATH ""
#define PIC_THEME1_HIGHLIGHT_LINEKEY_CUTLINE    PIC_THEME_PATH ""
#define PIC_THEME1_DIAL_LINEKEY_CUTLINE PIC_THEME_PATH ""

// dss pagebutton
//填充红色
#define     PAGE_ICON_RED               PIC_COMMON_PATH ""
//填充绿色
#define     PAGE_ICON_GREEN             PIC_COMMON_PATH ""
//填充灰色
#define     PAGE_ICON_GRAY              PIC_COMMON_PATH ""
//灰色环图标
#define     PAGE_ICON_GRAY_CIRCLE       PIC_COMMON_PATH ""

// account
#define PIC_ACCOUNT_DISABLE                 PIC_COMMON_PATH "disable_small.dob"
#define PIC_ACCOUNT_LOGING                  PIC_COMMON_PATH "logging_small.dob"
#define PIC_ACCOUNT_REGISTED                PIC_COMMON_PATH "registed_small.dob"
#define PIC_ACCOUNT_REGISTEDFAIL            PIC_COMMON_PATH "registedfail_small.dob"
#define PIC_ACCOUNT_UNREGISTER              PIC_COMMON_PATH "unregister_small.dob"

#define PIC_ACCOUNT_FORWARD                 PIC_COMMON_PATH "LineForward.dob"
#define PIC_ACCOUNT_DND                     PIC_COMMON_PATH "LineDND.dob"
#define PIC_ACCOUNT_HEADSET                 PIC_COMMON_PATH "HeadSet.dob"
#define PIC_ACCOUNT_SCA_REGISTED            PIC_COMMON_PATH "sca_registed_small.dob"
#define PIC_ACCOUNT_CLOUD_REGISTED          PIC_COMMON_PATH "cloud_registed_small.dob"

#define PIC_ACCOUNT_ACD_LOGOUT              PIC_COMMON_PATH "acd-logout.dob"
#define PIC_ACCOUNT_ACD_LOGIN               PIC_COMMON_PATH "acd-login.dob"
#define PIC_ACCOUNT_ACD_UNAVAILABLE         PIC_COMMON_PATH "acd-unavailable.dob"
#define PIC_ACCOUNT_ACD_WRAPUP              PIC_COMMON_PATH "acd-wrapup.dob"

#define PIC_ACCOUNT_ACD_LOGOUT              PIC_COMMON_PATH "acd-logout.dob"
#define PIC_ACCOUNT_ACD_LOGIN               PIC_COMMON_PATH "acd-login.dob"
#define PIC_ACCOUNT_ACD_UNAVAILABLE         PIC_COMMON_PATH "acd-unavailable.dob"
#define PIC_ACCOUNT_ACD_WRAPUP              PIC_COMMON_PATH "acd-wrapup.dob"

// idle
#define PIC_IDLE_DND                        PIC_COMMON_PATH "DND.dob"
#define PIC_IDLE_AA                         PIC_COMMON_PATH "AutoAnswer.dob"
#define PIC_IDLE_FWD                        PIC_COMMON_PATH "CallForward.dob"
#define PIC_IDLE_LOCK                       PIC_COMMON_PATH "LOCK.dob"
#define PIC_IDLE_RING_MUTE                  PIC_COMMON_PATH "NoRing.dob"
#define PIC_IDLE_VOICEMAIL                  PIC_COMMON_PATH "voice_mail.dob"
#define PIC_IDLE_TEXTMESSAGE                PIC_COMMON_PATH "SMS.dob"
#define PIC_IDLE_MISSCALL                   PIC_COMMON_PATH "calllist_miss.dob"
#define PIC_IDLE_VPN                        PIC_COMMON_PATH "VPN.dob"
#define PIC_IDLE_INTERNET                   PIC_COMMON_PATH "network.dob"

#define PIC_IDLE_USB_DEVICE                 PIC_COMMON_PATH "USB.dob"
#define PIC_IDLE_USB_DEVICE_DETECTING       PIC_COMMON_PATH "usbdetect.dob"
#define PIC_IDLE_USB_RECORD                 PIC_COMMON_PATH "RecordIdle.dob"
#define PIC_IDLE_USB_RECORD_PAUSE           PIC_COMMON_PATH "RecordIdlePause.dob"

#define PIC_IDLE_TALK_MUTE                  PIC_COMMON_PATH "Mute.dob"
#define PIC_IDLE_PSTN                       PIC_COMMON_PATH "pstn.dob"

#define PIC_IDLE_BLUETOOTH                  PIC_COMMON_PATH "bluetooth_dongle_insert.dob"
#define PIC_IDLE_BLUETOOTH_CONNECTED        PIC_COMMON_PATH "idle_bluetooth_connected.dob"
#define PIC_IDLE_BLUETOOTH_MOBILE_CONNECTED PIC_COMMON_PATH "idle_bluetooth_phone_connected.dob"

#define PIC_IDLE_WIFI                       PIC_COMMON_PATH "wifi.dob"
#define PIC_IDLE_WIFI_CONNECTED             PIC_COMMON_PATH "wifi_connected.dob"

#define PIC_IDLE_CABLE_ON                   PIC_COMMON_PATH "Cable1.dob"
#define PIC_IDLE_CABLE_OFF                  PIC_COMMON_PATH "Cable2.dob"

// bluetooth and wifi
#define PIC_BT_BLUETOOTH_ICON               PIC_COMMON_PATH "bluetooth.dob"                 // 其他类型蓝牙列表图标
#define PIC_BT_BLUETOOTH_AC_ICON            PIC_COMMON_PATH "bluetooth_auto.dob"            // 其他类型自动连接蓝牙列表图标
#define PIC_BT_HEADSET_ICON                 PIC_COMMON_PATH "bluetooth_headset.dob"         // 耳机类型蓝牙列表图标
#define PIC_BT_HEADSET_AC_ICON              PIC_COMMON_PATH "bluetooth_headset_auto.dob"    // 耳机类型蓝牙自动连接列表图标
#define PIC_BT_PHONE_ICON                   PIC_COMMON_PATH "bluetooth_mobile_icon.dob"     // 手机类型蓝牙列表图标
#define PIC_BT_PHONE_AC_ICON                PIC_COMMON_PATH "bluetooth_mobile_auto.dob"     // 手机类型蓝牙自动连接列表图标

#define PIC_WIRELESS_ENCRYPTED_ICON         PIC_COMMON_PATH "encryption.dob"                // 设备加密图标
#define PIC_WIRELESS_DISCONNECT_ICON        PIC_COMMON_PATH "wireless_disconnect.dob"       // 设备断开连接的图标
#define PIC_WIRELESS_CONNECTING_ICON        PIC_COMMON_PATH "wifi_connecting.dob"       // 设备正在连接的图标
#define PIC_WIRELESS_CONNECTED_ICON         PIC_COMMON_PATH "check-flag.dob"                // 设备连接成功的图标
#define PIC_COMMON_WIFI_PATH                PIC_COMMON_PATH

// usb record
#define PIC_USB_RECORD_START                PIC_COMMON_PATH "RecordStart.dob"
#define PIC_USB_RECORD_STOP                 PIC_COMMON_PATH "RecordStop.dob"
#define PIC_USB_RECORD_PAUSE                PIC_COMMON_PATH "RecordPause.dob"

#define PIC_GENERAL_MESSAGE_UNREAD          PIC_COMMON_PATH "msg-unread.dob"
#define PIC_GENERAL_MESSAGE_READ            PIC_COMMON_PATH "msg-readed.dob"
#define PIC_GENERAL_MESSAGE_DRAFT           PIC_GENERAL_MESSAGE_READ
#define PIC_GENERAL_MESSAGE_SENDING         PIC_GENERAL_MESSAGE_READ
#define PIC_GENERAL_MESSAGE_SEND            PIC_GENERAL_MESSAGE_READ

#define PIC_VOICE_MAIL_READ                 PIC_COMMON_PATH "voicemail_read.dob"
#define PIC_VOICE_MAIL_UNREAD               PIC_COMMON_PATH "voicemail_unread.dob"

#define PIC_ICON_PLAY                       PIC_COMMON_PATH "Play.dob"
#define PIC_ICON_PAUSE                      PIC_COMMON_PATH "Pause.dob"

#endif //IS_COLOR

#endif
