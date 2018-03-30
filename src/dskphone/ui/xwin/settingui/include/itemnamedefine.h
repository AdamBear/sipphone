#ifndef _DEFINE_ITEMNAME_H_
#define _DEFINE_ITEMNAME_H_

// 配置项名定义

// status
#define STATUS_IPV4                             "Status_IP"
#define STATUS_IPV6                             "Status_IPv6"
#define STATUS_MAC                              "Status_MAC"
#define STATUS_WiFi_MAC                         "Status_WIFI_MAC"
#define STATUS_FIRMWARE                         "Status_HW"
#define STATUS_MORE                             "Status_More"
#define STATUS_MORE_RTP                         "Status_More_RTP"

// networkstatus
#define STATUS_NETWORK_IPV4                     "ipv4status"
#define STATUS_NETWORK_IPV6                     "ipv6status"
#define STATUS_NETWORK_WiFi_MAC                 "wifi_mac"
#define STATUS_NETWORK_IP_MODE                  "ipmode"
#define STATUS_NETWORK_MAC                      "mac"

// ipv4 status
#define STATUS_NETWORK_IPV4_WANTYPE             "wantype"
#define STATUS_NETWORK_IPV4_WANIP               "wanip"
#define STATUS_NETWORK_IPV4_WANMASK             "wanmask"
#define STATUS_NETWORK_IPV4_GETWAY              "gateway"
#define STATUS_NETWORK_IPV4_PRIDNS              "pridns"
#define STATUS_NETWORK_IPV4_SECDNS              "secdns"
// ipv6 status
#define STATUS_NETWORK_IPV6_WANTYPE             "wanipv6type"
#define STATUS_NETWORK_IPV6_WANIP               "wanipv6ip"
#define STATUS_NETWORK_IPV6_GETWAY              "ipv6gateway"
#define STATUS_NETWORK_IPV6_PRIDNS              "ipv6pridns"
#define STATUS_NETWORK_IPV6_SECDNS              "ipv6secdns"

// statusphone
#define STATUS_PHONE_MODEL                      "model"
#define STATUS_PHONE_HARDWARE                   "hardware"
#define STATUS_PHONE_FIRMWARE                   "firmware"
#define STATUS_PHONE_CONFIG_VERSION             "config_version"
#define STATUS_PHONE_PRODUCTID                  "productid"
#define STATUS_PHONE_MAC                        "mac"
#define STATUS_PHONE_DEVICECERT                 "devicecert"
#define STATUS_PHONE_WiFi_MAC                   "wifi_mac"
#define STATUS_PHONE_BLUTOOTH_MAC               "bluetooth_mac"

//call forward
#define ALWAYS_FORWARD                          "always-forward"
#define BUSY_FORWARD                            "busy-forward"
#define NO_ANSWER_FORWARD                       "NoAnswer-forward"

//Busy/Always/NoAnswer Forward
#define CALLFORWARD_ENABLE                      "callforward_enable"
#define CALLFORWARD_TARGET                      "callforward_target"
#define CALLFORWARD_RINGTIME                    "callforward_ringtime"
#define CALLFORWARD_ONCODE                      "callforward_oncode"
#define CALLFORWARD_OFCODE                      "callforward_offcode"

//Call Waiting
#define CALLWAITING_ENABLE                      "callwaiting_enable"
#define CALLWAITING_PLAYTONE                    "callwaiting_playtone"
#define CALLWAITING_ONCODE                      "callwaiting_oncode"
#define CALLWAITING_OFFCODE                     "callwaiting_offcode"

//Auto Answer
#define AUTOANSWER_LINE                         "autoanswer_line"
#define AUTOANSWER_STATUS                       "autoanswer_status"
#define AUTOANSWER_MUTE                         "autoanswer_mute"

//features
#define FEATURE_CALL_FORWARD                    "head-forward"
#define FEATURE_CALL_WAITING                    "call_waiting"
#define FEATURE_ANONYMOUS                       "Features_Anonymous"
#define FEATURE_DND                             "Features_DNDCode"
#define FEATURES_INTERCOM                       "intercom"
#define FEATURES_CALL_COMPLETION                "call_completion"
#define FEATURES_PAGING_LIST                    "multpaginglist"
#define FEATURES_AUTORD                         "Features_AutoRD"
#define FEATURES_DIAGNOSTICS                    "Features_Diagnostics"

//features-key as send
#define KEYASSEND_TYPE                          "keyassend_type"

//features-historysetting
#define HISTORYSETTING_RECORDENABLE             "historysetting_recordenable"

//feature-hotline
#define HOTLINE_NUMBER                          "hotline_number"
#define HOTLINE_DELAY                           "hotline_delay"

// feature-intercom
#define INTERCOM_ALLOW_ENABLE                   "intercom_allow_eable"
#define INTERCOM_MUTE_ENABLE                    "intercom_mute_eable"
#define INTERCOM_TONE_ENABLE                    "intercom_tone_eable"
#define INTERCOM_BARGE_ENABLE                   "intercom_barge_eable"

// feature-callcompletion
#define FEATURES_CALLCOMPLETION                 "callcompletion_enable"

// feature-anonymouscall
#define ANONYMOUS_LINEID                        "anonymous_line"
#define ANONYMOUS_CALLENABLE                    "anonymous_call_enabled"
#define ANONYMOUS_REJECTION_ENABLE              "anonymous_rejection_enabled"
#define ANONYMOUS_SEND_ENABLE                   "anonymous_send_enabled"
#define ANONYMOUS_CALL_ONCODE                   "anonymous_call_oncode"
#define ANONYMOUS_CALL_OFFCODE                  "anonymous_call_offcode"
#define ANONYMOUS_REJECTION_SEND_ENABLE         "anonymous_rejection_send_enabled"
#define ANONYMOUS_REJECTION_ONCODE              "anonymous_rejection_oncode"
#define ANONYMOUS_REJECTION_OFFCODE             "anonymous_rejection_offcode"

// dndcode
#define FEATURES_DND_ENABLE                     "DND"
#define FEATURES_DND_ONCODE                     "DND_On_Code"
#define FEATURES_DND_OFFCODE                    "DND_Off_Code"

// feature-PagingList-Edit
#define PAGING_LIST_EDIT_NUMBER                 "paging_list_edit_number"
#define PAGING_LIST_EDIT_ADDRESS                "paging_list_edit_address"
#define PAGING_LIST_EDIT_LABEL                  "paging_list_edit_lable"
#define PAGING_LIST_EDIT_CHANNEL                "paging_list_edit_channel"

//diagnostics-ping-url
#define DIAGNOISTICS_URL_PING                   "ping_url"
//diagnostics-traceout-url
#define DIAGNOISTICS_URL_TRACEOUT               "tracerount_url"
//setting type
#define SETTINGUI_TYPE_BASIC                    "Set_Basic"
#define SETTINGUI_TYPE_ADVANCED                 "Set_Adv"

// basic time
#define BASIC_DATETIME_TYPE                     "Basic_Time_type"
#define BASIC_SNTP_SETTING                      "Basic_Time_SNTP"
#define BASIC_MANUAL_SETTING                    "Basic_Time_Manual"
#define BASIC_TIEMDATE_FORMAT                   "Basic_Time_Format"
#define BASIC_DHCP_TIMEZONE                     "dhcptimezone"
// sntp settings
#define SNTP_TIME_ZONE                          "sntptimezone"
#define SNTP_NTP_SERVER_PRI                     "sntpserver1"
#define SNTP_NTP_SERVER_SEC                     "sntpserver2"
#define SNTP_NTP_DAYLIGHTSAVING                 "daylightsaving"
#define SNTP_NTP_LOCATION                       "timezonecity"  // add by dlw 2014-11-05
// time and date format
#define TIME_DATE_FORAMT_TIME                   "time_format_setting"
#define TIME_DATE_FORAMT_DATE                   "date_format_setting"
// dhcptimezone
#define DHCP_TIMEZONE                           "dhcptimezone_enable"

//Network-WanPort
#define WANPORT_DHCP_ENABLE                     "wanport_dhcp_enable"
#define WANPORT_DHCP_PEIDNS                     "wanport_dhcp_staticpridns"
#define WANPORT_DHCP_SECDNS                     "wanport_dhcp_staticsecdns"

//Network-WanPort-Option
#define WANPORT_IPMODE_PERFERED                 "network_wanport_modepreference"

//Network-VLAN
#define VLAN_PCPORT                             "pcport"
#define VLAN_PCPORT_STATUS                      "vlanpcport_status"
#define VLAN_WANPORT_STATUS                     "vlanwanport_status"
// vlan dhcp
#define VLAN_DHCP_STATUS                        "vlandhcp_enable"
#define VLAN_DHCP_OPTION                        "vlanpcport_option"
//network-vpn
#define  NETWORK_VPN_ACTIVE                     "network_vpn_active"
//network-nat
#define  NETWORK_NAT_STATUS                     "NAT_status"
#define  NETWORK_NAT_STUNSERVER                 "NAT_STUN_server"
#define  NETWORK_NAT_STUNPORT                   "NAT_STUN_Port"
//network-8021x
#define  NETWORK_8021X_MODEL                    "8021x_model"
#define  NETWORK_8021X_IDENTITY                 "8021x_identity"
#define  NETWORK_8021X_PASSWORD                 "8021x_password"

//flash-time
#define BASIC_SETTING_COUNTRY                   "country"
#define ADVANCDE_FLASH_TIME                     "flashtime"

#define ADVANCDE_INTERNATIONAL_FORWARD          "international-forward"

// adv account setting
#define ADV_ACCOUNT_ACTIVATION                  "line_active"
#define ADV_ACCOUNT_LABEL                       "account_label"
#define ADV_ACOCUNT_DISPALY                     "display_name"
#define ADV_ACOCUNT_REGISTER                    "register_name"
#define ADV_ACCOUNT_USER                        "user_name"
#define ADV_ACOCUNT_PASSWD                      "account_password"
#define ADV_ACOCUNT_SIP_SERVER                  "sip_server"
#define ADV_ACOCUNT_SIP_SERVER1                 "sip_server1"
#define ADV_ACOCUNT_SIP_SERVER2                 "sip_server2"
#define ADV_ACOCUNT_OUTBOUNDSTATUS              "enabled-outbound-proxy-server"
#define ADV_ACOCUNT_OUTBOUNDPROXY               "outbound_proxy_server"
#define ADV_ACOCUNT_OUTBOUNDPROXY1              "outbound_proxy_server1"
#define ADV_ACOCUNT_OUTBOUNDPROXY2              "outbound_proxy_server2"
#define ADV_ACCOUNT_PROXYFALLBACKINTERVAL       "proxy_fallback_interval"
#define ADV_ACCOUNT_NAT_STATUS                  "nat-status"

#define ADV_CLOUD_ACCOUNT_TYPE                  "adv_cloud_account_type"
#define ADV_CLOUD_ACCOUNT_LOGINTYPE             "adv_cloud_account_logintype"
#define ADV_CLOUD_ACCOUNT_USERNAME              "adv_cloud_account_username"
#define ADV_CLOUD_ACCOUNT_PASSWORF              "adv_cloud_account_password"
#define ADV_CLOUD_ACCOUNT_SAVEPAAWORD           "adv_cloud_account_savepassword"
#define ADV_CLOUD_ACCOUNT_PINCODE               "adv_cloud_account_pincode"

#define ADV_PSTN_ACCOUNT_ACTIVE                 "account_PSTN_lineactive"
#define ADV_PSTN_ACCOUNT_LABEL                  "account_PSTN_label"

//acddisposition code
#define ACD_DISPOSITION_CODE_ENTERCODE          "acd_disposition_code_entercode"
//acd emergency escalation
#define ACD_EMERGENCY_ESCALATION_ENTER_NUMBER   "acd_emergency_escalation_enternumber"
/////////////////////////////////////////////////////////////////////////////
#define STATUS_NETWORK_LANTYPE                  "lantype"
#define STATUS_NETWORK_LANIP                    "lanip"
#define STATUS_NETWORK_LANMASK                  "lanmask"

// ip conflict
#define CONFLICT_IPV4                           "conflictipv4"
#define CONFLICT_IPV4_MAC                       "conflictipv4mac"
#define CONFLICT_IPV6                           "conflictipv6"
#define CONFLICT_IPV6_MAC                       "conflictipv6mac"

// features
#define FEATURES_FORWARD                        "head-forward"
#define FEATURES_DNDCODE                        "Features_DNDCode"
#define FEATURES_ANONYMOUS                      "Features_Anonymous"
#define FEATURES_DSSKEY                         "DSSKey"

// custom
#define CUSTOM_ALWAYS_FORWARD                   "customalwaysforward"
#define CUSTOM_BUSY_FORWARD                     "custombusyforward"
#define CUSTOM_NO_ANSWER_FORWARD                "customnoanswerforward"
// always
#define ALWAYS_FORWARD_ALWAYS                   "always_forward_enabled"
#define ALWAYS_FORWARD_FORWARDTO                "always_forward_target"
#define ALWAYS_FORWARD_ONCODE                   "always_forward_oncode"
#define ALWAYS_FORWARD_OFFCODE                  "always_forward_offcode"
// custom always
#define CUSTOM_ALWAYS_FORWARD_ENABLE            "customalwaysenable"
#define CUSTOM_ALWAYS_FORWARD_FORWARDTO         "customalwaysforwardto"
#define CUSTOM_ALWAYS_FORWARD_ONCODE            "customalwaysoncode"
#define CUSTOM_ALWAYS_FORWARD_OFFCODE           "customalwaysoffcode"
// busy
#define BUSY_FORWARD_BUSY                       "busy-forward-enabled"
#define BUSY_FORWARD_FORWARDTO                  "busy-forward-target"
#define BUSY_FORWARD_ONCODE                     "busy-forward-OnCode"
#define BUSY_FORWARD_OFFCODE                    "busy-forward-OffCode"
// custom busy
#define CUSTOM_BUSY_FORWARD_ENABLE              "custombusyenable"
#define CUSTOM_BUSY_FORWARD_FORWARDTO           "custombusyforwardto"
#define CUSTOM_BUSY_FORWARD_ONCODE              "custombusyoncode"
#define CUSTOM_BUSY_FORWARD_OFFCODE             "custombusyoffcode"
// noanswer
#define NOANSWER_FORWARD_NOANSWER               "NoAnswer-forward-enabled"
#define NOANSWER_FORWARD_FORWARDTO              "NoAnswer-forward-target"
#define NOANSWER_FORWARD_AFTERRINGTIME          "NoAnswer-forward-ringtimes"
#define NOANSWER_FORWARD_ONCODE                 "NoAnswer-forward-OnCode"
#define NOANSWER_FORWARD_OFFCODE                "NoAnswer-forward-OffCode"
// custom nonaswer
#define CUSTOM_NOANSWER_FORWARD_ENABLE          "customnoanswerenable"
#define CUSTOM_NOANSWER_FORWARD_FORWARDTO       "customnoanswerforwardto"
#define CUSTOM_NOANSWER_FORWARD_AFTERRINGTIME   "customnoanswerafterringtime"
#define CUSTOM_NOANSWER_FORWARD_ONCODE          "customnoansweroncode"
#define CUSTOM_NOANSWER_FORWARD_OFFCODE         "customnoansweroffcode"
// call forward not reachable
#define CALL_FORWARD_NOT_REACHABLE              "Call-forward-not-reachable-enable"
#define CALL_FORWARD_NOT_REACHABLE_FORWARDTO    "Call-forward-not-reachable-target"
// call forward not reachable
#define CUSTOM_CALL_FORWARD_NOT_REACHABLE       "customcallforwardnotreachableenable"
#define CUSTOM_CALL_FORWARD_NOT_REACHABLE_FORWARDTO "customcallforwardnotreachableforwardto"

// autoredial
#define FEATURES_AUTOREDIAL                     "auto-redial"
#define FEATURES_REDIALINTERVAL                 "auto_redial_interval"
#define FEATURES_REDIALTIMES                    "auto_redial_times"

#define FEATURES_CUSTOM_DND_ACTIVE                      "custom_dndactive"
#define FEATURES_CUSTOM_DNDONCODE                       "custom_DND_OnCode"
#define FEATURES_CUSTOM_DNDOFFCODE                      "custom_DND_OffCode"
// calllog
#define CALLLOG_ITEM                            "calllog-group"
#define CALLLOG_DISPLAYNAME                     "calllog-display-name"
#define CALLLOG_SERVER                          "calllog-server"
#define CALLLOG_PORT                            "calllog-port"
#define CALLLOG_USERNAME                        "calllog-user-name"
#define CALLLOG_PASSWORD                        "calllog-password"
// advancedsettings
#define ADV_USER_TYPE                           "advusertype"
#define ADV_PASSWORD                            "advpassword"

// wan port option
#define WAN_PORT_IP_MODE                        "wanipmode"

// wan port ip type
#define IPV4WANTYPE                             "ipv4wantype"
#define IPV6WANTYPE                             "ipv6wantype"

// wan port ipv6
#define DHCP_IPV6_CLIENT                        "dhcpipv6client"
#define STATIC_IPV6_CLIENT                      "staticipv6client"

// advnetwork wan port
#define DHCP_CLIENT                             "dhcpipclient"
#define STATIC_CLIENT                           "staticipclient"
#define PPPOE_CLIENT                            "pppoeipclient"

// static ip client
#define STATIC_IP                               "staticip"
#define STATIC_SUBNET_MASK                      "staticsubnetmask"
#define STATIC_DEFAULT_GATEWAY                  "staticdefaultgateway"
// dns server
#define STATIC_DNS_SERVER_ENABLE                "dnsserverenable"
#define STATIC_PRI_DNS                          "staticpridns"
#define STATIC_SEC_DNS                          "staticsecdns"
// ipv6 dns server
#define STATIC_IPV6_DNS_SERVER_ENABLE           "dnsserveripv6enable"
// static ipv6 client
#define STATIC_IPV6                             "staticipv6ip"
#define STATIC_IPV6_PREFIX                      "staticipv6prefix"
#define STATIC_IPV6_DEFAULT_GATEWAY             "staticipv6defaultgateway"
#define STATIC_IPV6_PRI_DNS                     "staticipv6pridns"
#define STATIC_IPV6_SEC_DNS                     "staticipv6secdns"
// pppoe
#define PPPOE_USER                              "pppoeuser"
#define PPPOE_PASSWORD                          "pppoepwd"
// vlanmenu
#define VLAN_MENU_VLAN_DHCP                     "vlanDHCP"
#define VLAN_MENU_DHCP_OPTION                   "DHCPoption"
#define VLAN_MENU_ENABLE_LLDP                   "enableLLDP"
// lldp
#define NETWORK_LLDP_STATUS                     "lldpstatus"
#define NETWORK_LLDP_PACKET_INTERVAL            "packetinterval"
// vlanwanport
#define VLAN_WAN_STATUS                         "vlanwanstatus"
#define VLAN_WAN_VID_NUMBER                     "vlanvidwannumber"
#define VLAN_WAN_PRIORITY                       "vlanwanpriority"
// vlancdp
#define VLAN_CDP_STATUC                         "vlancdpstatus"
#define VLAN_CDP_INTERVAL                       "vlancdpinterval"
// pcport
#define PC_PORT_BRIDGE                          "bridge"
#define PC_PORT_ROUTER                          "router"
// rounter
#define ROUTERIP                                "routerip"
#define ROUTERSUBNETMASK                        "routersubnetmask"
#define DHCPSERVEREABLE                         "dhcpserverenable"
// dhcp server
#define DHCP_SERVER_STATUS                      "dhcpserverstatus"
#define DHCP_START_IP                           "dhcpstartip"
#define DHCP_END_IP                             "dhcpendip"
// time
#define MANUAL_DATE                             "manualdate"
#define MANUAL_TIME                             "manualtime"
// vlanpcport
#define VLAN_PC_STATUS                          "vlanpcstatus"
#define VLAN_PC_VID_NUMBER                      "vlanpcvidnumber"
#define VLAN_PC_PRIORITY                        "vlanpcpriority"
// webservertype
#define ENABLE_WEB_HTTP                         "webhttpswitch"
#define WEB_HTTP_PORT                           "webhttpport"
#define ENABLE_WEB_HTTPS                        "webhttpsswitch"
#define WEB_HTTPS_PORT                          "webhttpsport"
// 802
#define NET_802DOT1X_MODEL                      "8021xmodel"
#define NET_802DOT1X_IDENTITY                   "8021xidentity"
#define NET_802DOT1X_PASSWORD                   "8021xpassword"
// vpn
#define VPN_ACTIVE                              "vpnactive"

// Basicsetting
#define BASIC_SETTING_BACKLIGHT                 "Basic_Setting_BackLight"
#define BASIC_SETTING_CONTRAST                  "contrast_ctrl"
#define DISPLAY_WALLPAPER                       "wallpaper"
#define DISPLAY_SCREENSAVER                     "screensaver"

// backlightlevel
#define BACKLIGHTACTIVELEVEL                    "back_light_active_level"
#define BACKLIGHTTIME                           "back_light_time"
#define BACKLIGHTINACTIVELEVEL                  "back_light_inactive_level"
// contrast
#define CONTRAST_LEVEL                          "contrast_level"
// phonelock
#define PHONE_LOCK                              "phonelock"
#define PHONE_LOCK_SWITCH                       "phonelockswitch"
#define PHONE_LOCK_TIMEOUT                      "phonelocktimeout"
// phone unlock pin
#define CURRENTPIN                              "currentpin"
#define NEWPIN                                  "newpin"
#define CONFIRMPIN                              "confirmpin"
// phone lock check
#define ITEM_PHONE_UNLOCK_CHECK_EDIT                 "phoneunlockcheckedit"
//set password
#define CURRENTPASSWORD                         "currentpassword"
#define NEWPASSWORD                             "newpassword"
#define CONFIRMPASSWORD                         "confirmpassword"
// aes key
#define AES_KEY_SPECIFIED                       "aeskeyspecified"
#define AES_KEY_PERPHONE                        "aeskeyperphone"
// atuoprovision
#define AUTOP_URL                               "autopurl"
#define AUTOP_USERNAME                          "autopusername"
#define AUTOP_PASSWORD                          "autoppassword"
// fwd internation
#define FWD_INTERNATION                         "fwdinternational"
// usermode
#define USER_MODE_ENTRY                         "usermodeentry"
#define USER_MODE                               "usermode"
#define USER_PASSWORD                           "userpassword"
// dsskey
#define DSSKEY_CONFIG_TYPE                      "dsskeytype"
#define DSSKEY_CONFIG_KEYEVENT_TYPE             "dsskeyeventtype"
#define DSSKEY_CONFIG_LINE                      "dsskeyline"
#define DSSKEY_CONFIG_INDEX                     "dsskeyindex"
#define DSSKEY_CONFIG_LABEL                     "dsskeylabel"
#define DSSKEY_CONFIG_VALUE                     "dsskeyvalue"
#define DSSKEY_CONFIG_EXTENSION                 "dsskeyextension"
#define DSSKEY_CONFIG_LINE_VALUE                "dsskeylinevalue"
#define DSSKEY_CONFIG_USER                      "dsskeylineuser"
#define DSSKEY_CONFIG_SERVER                    "dsskeylineserver"
#define DSSKEY_CONFIG_LINE_VALUE                "dsskeylinevalue"
#define DSSKEY_CONFIG_SHORT_LABEL               "dsskeyshortlabel"
// voicemail
#define VOICEMAIL_SETITEM                       "setvoicemail"
#define VOICEMAIL_VIEWITEM                      "viewvoicemail"
#define VOICEMAIL_NAME          "voicemailname"
#define VOICEMAIL_NUMBER        "voicemailnumber"
#define VOICEMAIL_TIME          "voicemailtime"
#define VOICEMAIL_LINE          "voicemailline"
#define VOICEMAIL_DURATION      "voicemailduration"
#define VOICEMAIL_STATUS        "voicemailstatus"
#define VOICEMAIL_TYPE          "voicemailtype"
#define VOICEMAIL_FROM          "voicemailfrom"
#define VOICEMAIL_TIMEPIECE     "voicemailtimepiece"
#define ADDTOPERSONAL_NAME      "addtopersonal_name"
#define ADDTOPERSONAL_NUMBER    "addtopersonal_number"

// history
#define HISTORY_MAIN                            "call_info"
#define HISTORY_LOCAL                           "history"
#define HISTORY_NETWORK                         "networkcalllog"
// contact
#define DIRECTORY_LOCAL                         "localdirectory"
#define DIRECTORY_BLACK                         "blacklist"
#define DIRECTORY_REMOTE                            "remotedirectory"
#define DIRECTORY_BROADSOFT                     "broadsoftdirectory"
#define DIRECTORY_LDAP                          "ldap"
#define DIRECTORY_NETWORK                       "networkdirectory"
// ringtone
#define RING_TONE                               "ringtone"
// account ringtone
#define ACCOUNT_RINGTONE_ITEM                   "accountringtone"
// language
#define LANGUAGE_TYPE                           "language"
// account
#define ACCOUNT_STATUS                          "accountstatus"
// reboot
#define REBOOT                                  "Adv_Reboot"
// reset to factory
#define RESETTOFACTORY                          "Adv_ResetToFac"
#define RESETLOCALCONFIG                        "ResetLocalConfig"
#define RESETWEBCONFIG                          "ResetWebConfig"
#define RESETNONSTATICCONFIG                    "ResetNonStaticConfig"
#define RESETSTATICCONFIG                       "ResetStaticConfig"
#define RESET_USERDATA_USERCFG                  "ResetUserDataUserCfg"
#if IF_BUG_21893
// reset to default
#define DELETEDEFAULT                           "Adv_DeleteDefault"
#endif
// textmessage
// http://192.168.1.99/Bug.php?BugID=22025
#define MESSAGE_TEXT_MAIN                       "MSG_Text"
#define NOMESSAGE                               "nomessage"
#define SEND_MESSAGE_FROM                       "messagefrom"
#define SEND_MESSAGE_TO                         "messageto"
// acd
#define ACD_LOGIN_USERID                        "acduserid"
#define ACD_LOGIN_PASSWORD                      "acdpassword"
#define ACD_ITEM_STATUS                         "acdstatus"
#define ACD_ITEM_REASION                        "acdreason"
// no item
#define NOITEM                                  "noitem"
// option item
#define OPTION_ITEM_DELETE                      "optiondelete"
#define OPTION_ITEM_DELETE_ALL                  "optiondeleteall"

// broadsoft call control
#define MAINMENU_CALLCONTROL                        "CallControl"
#define BROADSOFT_CALLCONTROL_ANYWHERE              "callcontrol_anywhere"
#define BROADSOFT_CALLCONTROL_ANONYMOUSCALL         "callcontrol_hidenumber"
#define BROADSOFT_CALLCONTROL_ANONYMOUSREJECTION    "callcontrol_anonymousrejection"
#define BROADSOFT_CALLCONTROL_REMOTEOFFICE          "callcontrol_remoteoffice"
#define BROADSOFT_CALLCONTROL_SIMULTANEOUSRING      "callcontrol_sinultaneousring"
#define BROADSOFT_CALLCONTROL_FORWARD               "callcontrol_forward"
#define BROADSOFT_CALLCONTROL_FORWARD_NOT_REACHABLE "callcontrol_forwardnotreachable"
#define BROADSOFT_CALLCONTROL_DND                   "callcontrol_dnd"
#define BROADSOFT_CALLCONTROL_MOBILITY              "callcontrol_mobility"
#define BROADSOFT_CALLCONTROL_GNF                   "callcontrol_group_night_forward"

#define BROADSOFT_CALLCONTROL_INITACDSTATE          "callcontrol_initialacdstate"
#define BROADSOFT_CALLCONTROL_SETACDSTATE           "callcontrol_setacdstate"
#define BROADSOFT_CALLCONTROL_CALLWAITING           "callcontrol_callwaiting"
#define BROADSOFT_CALLCONTROL_CALLRECORD            "callcontrol_callrecord"
#define BROADSOFT_CALLCONTROL_SECURITYCLASSIFICATION    "callcontrol_securityclassification"
#define BROADSOFT_CALLCONTROL_FORWARD_SELECTIVE     "callcontrol_forward_selective"

#define BROADSOFT_CALLCONTROL_EXECUTIVE          "callcontrol_executive"
#define BROADSOFT_CALLCONTROL_EXECUTIVE_ASSISTANT          "callcontrol_executive_assistant"

//call record
#define CALLRECORD_MODE                     "callrecordmode"
#define CALLRECORD_PAUSERESUMENOTE          "callrecordpauseresumenote"
#define CALLRECORD_PLAYSTARTSTOP            "callrecordplaystartstop"
#define CALLRECORD_RECORDVM                 "callrecordvoicemail"
#define CALLRECORD_PLAYTONE                 "callrecordplaytone"
#define CALLRECORD_PLAYTONETIME             "callrecordplaytonetime"

// Executive-Assistant
#define EXECUTIVE_DETAILS_SETTINGS              "executivedetailssetting"
#define ASSISTANT_DETAILS_SETTINGS              "assistantdetailssetting"
#define EXECUTIVE_DETAILS_ALLOW_OPT             "allowopt"
#define EXECUTIVE_DETAILS_FILTER                "filter"
#define EXECUTIVE_DETAILS_SCREEN                "screen"
#define EXECUTIVE_DETAILS_ALTER                 "alter"
#define EXECUTIVE_DETAILS_VIEW_ASSISTANT        "viewassistantlist"
#define ASSISTANT_DETAILS_VIEW_EXECUTIVE        "viewexecutivelist"

#define EXECUTIVE_FILTER_SWITCH                 "filterswitch"
#define EXECUTIVE_FILTER_MODE                   "filtermode"
#define EXECUTIVE_FILTER_TYPE                   "filtertype"

#define EXECUTIVE_SCREEN_SWITCH                 "screenswitch"
#define EXECUTIVE_SCREEN_TYPE                   "screentype"
#define EXECUTIVE_SCREEN_MOBILITY_SWITCH        "mobilityswitch"
#define EXECUTIVE_SCREEN_ANYWHERE_SWITCH        "anywhereswitch"
#define EXECUTIVE_SCREEN_SCA_SWITCH             "scaswitch"

#define EXECUTIVE_ALTER_MODE                        "altermode"
#define EXECUTIVE_ALTER_TIMER_TO_NEXT               "altertimertonext"
#define EXECUTIVE_ALTER_CALLID_NAME                 "altercallidname"
#define EXECUTIVE_ALTER_CALLID_NUMBER               "altercallidnumber"
#define EXECUTIVE_ALTER_CALLID_CUSTOM_NAME          "altercallidcustomname"
#define EXECUTIVE_ALTER_CALLID_UNICODE_CUSTOM_NAME  "altercallidunicodecustomname"
#define EXECUTIVE_ALTER_CALLID_CUSTOM_NUMBER        "altercallidcustomnumber"
#define EXECUTIVE_ALTER_TIMER_RECALL_AFTER          "altertimerrecallafter"
#define EXECUTIVE_ALTER_ROLLOVER_TIMER              "alterrollovertimer"
#define EXECUTIVE_ALTER_ROLLOVER_ACTION             "alterrolloveraction"
#define EXECUTIVE_ALTER_ROLLOVER_FWD_TO             "alterrolloverfwdto"

#define ASSISTANT_DIVERT_SWITCH                 "assistantdivert"
#define ASSISTANT_DIVERT_TO                     "assistantdivertto"
#define ASSISTANT_LIST_NO_DOT                   "assistantlistno."
#define EXECUTIVE_LIST_NO_DOT                   "executivelistno."

// broadsoft anonymous
#define BROADSOFT_ANONYMOUS_LINE                "anonymous_enable"

// broadsoft anywhere
#define ANYWHERE_SELECTLINE                     "anywhereline"
#define ANYWHERE_LOCATIONLIST                   "locationlist_location"
#define ANYWHERE_NONEITEM                       "locationlist_nolocation"

#define ANYWHERE_ADDLOCATION_NAME               "addlocation_name"
#define ANYWHERE_ADDLOCATION_NUMBER             "addlocation_number"

//EDIT Location
#define ANYWHERE_EDITLOCATION_ACTIVE            "editlocation_active"
#define ANYWHERE_EDITLOCATION_NAME              "editlocation_name"
#define ANYWHERE_EDITLOCATION_NUMBER            "editlocation_number"
#define ANYWHERE_EDITLOCATION_ALTNUM            "editlocation_altphonenumber"
#define ANYWHERE_EDITLOCATION_DI                "editlocation_diversioninhibitor"
#define ANYWHERE_EDITLOCATION_AC                "editlocation_answerconfirmation"
#define ANYWHERE_EDITLOCATION_SCC               "editlocation_serverbasedcallcontrol"

//Remote Office
#define REMOTEOFFICE_SELECTLINE                 "remoteofficeselectline"
#define REMOTEOFFICE_ACTIVE                     "remoteofficeactive"
#define REMOTEOFFICE_PHONENUMBER                "remoteofficenumber"

//Call Forward Not Reachable
#define FORWARDNOTREACHABLE_SELECTLINE          "forwardnotreachableselectline"
#define FORWARDNOTREACHABLE_ACTIVE              "forwardnotreachableactive"
#define FORWARDNOTREACHABLE_PHONENUMBER         "forwardnotreachablenumber"

//Simultaneous Ring
#define SIMULTANEOUSRING_SELECTLINE             "simultaneousselectline"

#define SIMULTANEOUSRING_STATUS                 "simultaneousstatus"
#define SIMULTANEOUSRING_LOCATION               "simultaneouslocation"
#define SIMULTANEOUSRING_NONEITEM               "simultaneous_nolocation"

#define SIMULTANEOUSRING_NUMBER                 "simultaneous_number"
#define SIMULTANEOUSRING_ANSWERCONFIRM          "simultaneous_answerconfirm"

//Mobility
#define MOBILITY_SELECTLINE                     "mobilityselectline"
#define MOBILITY_ACTIVE                         "mobilityactive"
#define MOBILITY_PHONETORING                    "mobilityphonetoring"
#define MOBILITY_MOBILEPHONENUMBER              "mobilitymobilephonenumber"
#define MOBILITY_ALERTCLICKTODIALCALLS          "mobilitynumber"
#define MOBILITY_ALERTGROUPPAGINGCALLS          "mobilityanswerconfirm"
#define MOBILITY_ENABLEDIVERSIONINHIBITOR       "mobilityenablediversioninhibitor"
#define MOBILITY_REQUIREANSWERCONFIRMATION      "mobilityrequireanswerconfirmation"
#define MOBILITY_BROADWORKSCALLCONTROL          "mobilitybroadworkscallcontrol"

//Group Night Forward
#define GNF_SELECTLINE                          "groupnightforwardselectline"
#define GNF_USERNIGHTFORWARDINGMODE             "groupnightforward_usernightforwardingmode"
#define GNF_HOLIDAYNAME                         "groupnightforward_holidayname"
#define GNF_BUSSINESSNAME                       "groupnightforward_bussinessname"

//call waiting
#define CALLCONTROL_CALLWAITING_ENABLE                  "callcontroll_callwaitingactive"

// forward selective
#define FORWARD_SELECTIVE_SWITCH                            "forward_selective_switch"
#define FORWARD_SELECTIVE_FWD_TO                            "forward_selective_fwd_to"
#define FORWARD_SELECTIVE_PLAY_RING                         "forward_selective_play_ring"

//change pwd
#define SETTINGUI_CHANGE_PWD_CURRENT_PWD        "change_pwd_current_pwd"
#define SETTINGUI_CHANGE_PWD_NEW_PWD            "change_pwd_new_pwd"
#define SETTINGUI_CHANGE_PWD_CONFIRM_PWD        "change_pwd_confirm_pwd"

#define XSI_PASSWORD                            "xsipassword"
#define XSI_USERID                              "xsiuserid"

#ifdef IF_FEATURE_ACD
// ACD Login
#define ACD_USERID              "acduserid"
#define ACD_PASSWORD            "acdpassword"

// ACD Status
#define ACDSTATE_AVAILABLE          "available"
#define ACDSTATE_UNAVAILABLE        "unavailable"
#define ACDSTATE_SINGOUT            "singout"
#endif

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER

//hoteling
#define HOTELING_USERID             "hotelinguserid"
#define HOTELING_PASSWORD           "hotelingpassword"
#define HOTELING_PASSWORD_STRING    "******"
#define HOTELING_AUTOLOGIN          "hotelingautologin"
#define HOTELING_USERHOST           "hotelinguserhost"

// guest unlock check
#define ITEM_GUEST_UNLOCK_CHECK_EDIT    "guestunlockcheckedit"

//initial state
#define CALLCONTROLL_ACDSTATUSINITIAL "callcontroll_initialacdstate"

//acd state
#define ACDSTATEPAGE_SIGNIN             "signin"
#define ACDSTATEPAGE_AVAILABLE          "available"
#define ACDSTATEPAGE_UNAVAILABLE        "unavailable"
#define ACDSTATEPAGE_WRAPUP             "wrapup"
#define ACDSTATEPAGE_SIGNOUT            "signout"

//Unavailable Reason Code
#define UNAVAILABLEREASONCODE_ENTER "unavailablereasoncode_enter"
#define UNAVAILABLEREASONCODE_CODE  "unavailablereasoncode_code"

//Disposition Code
#define DISPOSITIONCODE_ENTER       "dispositioncode_enter"
#define DISPOSITIONCODE_CODE        "dispositioncode_code"

//Emergency Escalation
#define EMERGENCYESCAL_ENTER        "emergencyescal_enter"
#define EMERGENCYESCAL_NUMBER       "emergencyescal_number"

#endif

#ifdef IF_FEATURE_HOTDESKING
// HotDesk Base
#define HOTDESKBASE_AUTH            "authname"
#define HOTDESKBASE_USER            "username"
#define HOTDESKBASE_PWD             "hotdesk_password"
#define HOTDESKBASE_SERVER1         "server1"
#define HOTDESKBASE_SERVER2         "server2"
#define HOTDESKBASE_OUTBOUND1       "outbound1"
#define HOTDESKBASE_OUTBOUND2       "outbound2"
#define HOTDESKBASE_MAIN            "hotdesking"
#define HOTDESKBASE_AUTOPURL        "hotdesking_autopurl"
#endif  //IF_FEATURE_HOTDESKING

// zero
#define ZERO_UPDATENOW              "zeroupdatenow"
#define ZERO_STATUS_FIRMWARE        "zerostatusfirmware"
#define ZERO_STATUS_MAC             "zerostatusmac"
#define ZERO_STATUS_IPV4IP          "zerostatusipv4ip"
#define ZERO_STATUS_IPV4SUBNETMASK  "zerostatusipv4subnetmask"
#define ZERO_STATUS_IPV4GATEWAY     "zerostatusipv4gateway"
#define ZERO_STATUS_IPV4PRIDNS      "zerostatusipv4pridns"
#define ZERO_STATUS_IPV4SECDNS      "zerostatusipv4secdns"
#define ZERO_STATUS_IPV6IP          "zerostatusipv6ip"
#define ZERO_STATUS_IPV6PREFIX      "zerostatusipv6prefix"
#define ZERO_STATUS_IPV6GATEWAY     "zerostatusipv6gateway"
#define ZERO_STATUS_IPV6PRIDNS      "zerostatusipv6pridns"
#define ZERO_STATUS_IPV6SECDNS      "zerostatusipv6secdns"
#define ZERO_STATUS_SERVERURL       "zerostatusserverurl"
#define ZERO_STATUS_USERNAME        "zerostatususername"

#define ZERO_IPMODE                 "zeroipmode"
#define ZERO_IPV4WANTYPE            "zeroipv4wantype"
#define ZERO_IPV4IP                 "zeroipv4ip"
#define ZERO_IPV4SUBNETMASK         "zeroipv4subnetmask"
#define ZERO_IPV4GATEWAY            "zeroipv4gateway"
#define ZERO_IPV4PRIDNS             "zeroipv4pridns"
#define ZERO_IPV4SECDNS             "zeroipv4secdns"
#define ZERO_IPV4PPPOEUSER          "zeroipv4pppoeuser"
#define ZERO_IPV4PPPOEPSW           "zeroipv4pppoepsw"
#define ZERO_IPV6WANTYPE            "zeroipv6wantype"
#define ZERO_IPV6IP                 "zeroipv6ip"
#define ZERO_IPV6PREFIX             "zeroipv6prefix"
#define ZERO_IPV6GATEWAY            "zeroipv6gateway"
#define ZERO_IPV6PRIDNS             "zeroipv6pridns"
#define ZERO_IPV6SECDNS             "zeroipv6secdns"
#define ZERO_VLANACTIVE             "zerovlanactive"
#define ZERO_VLANVID                "zerovlanvid"
#define ZERO_VLANPRIORITY           "zerovlanpriority"
#define ZERO_8021XMODE              "zero8021xmode"
#define ZERO_8021XIDENTITY          "zero8021xidentity"
#define ZERO_8021XMD5PSW            "zero8021xmd5psw"
#define ZERO_VPNACTIVE              "zerovpnactive"
#define AUTOP_RESULTINFO            "autopresultinfo"
#define ZERO_IPV4STATICDNS          "zeroipv4staticdns"
#define ZERO_IPV4STATICDNS_PRI      "zeroipv4staticpridns"
#define ZERO_IPV4STATICDNS_SEC      "zeroipv4staticsecdns"
#define ZERO_IPV6STATICDNS          "zeroipv6staticdns"
#define ZERO_IPV6STATICDNS_PRI      "zeroipv6staticpridns"
#define ZERO_IPV6STATICDNS_SEC      "zeroipv6staticsecdns"
#define ZERO_VLAN_DHCP              "zerovlandhcp"
#define ZERO_VLAN_DHCP_OPTION       "zerovlandhcpoption"
#define ZERO_LLDP_STATUS            "zerolldpstatus"
#define ZERO_LLDP_PACKET_INTERVAL   "zerolldppacketinterval"
#define ZERO_CDP_STATUS             "zerocdpstatus"
#define ZERO_CDP_INTERVAL           "zerocdpinterval"
#define ZERO_NAT_STATUS             "zeronatstatus"
#define ZERO_STUN_SERVER            "zerostunserver"
#define ZERO_PORT                   "zeroport"

// Idle Need Password To Reset
#define RESET_FACTORY_USERNAME              "resetfactoryuser"
#define RESET_FACTORY_PASSWORD              "resetfactorypwd"

// redirect
#define REDIRECT_USERNAME           "redirectusername"
#define REDIRECT_PSW                "redirectpsw"

// autop code
#define AUTOP_CODE_URL              "autopcodeurl"
#define AUTOP_CODE_USERNAME         "autopcodeusername"
#define AUTOP_CODE_PSW              "autopcodepsw"

#define KEYTONE_ENABLE                    "keytone_enable"

#define WGTID_ACCOUT_STATUS         "account_status"

// Camera
#define ITEMID_CAMERA_SCENE_MODE    "scenemode"
#define ITEMID_CAMERA_WHITE_BALANCE "whitebalance"

#define ITEMID_CAMERA_RED_GAIN          "redgain"
#define ITEMID_CAMERA_BLUE_GAIN         "bluegain"
#define ITEMID_CAMERA_SHARPNESS         "sharpness"
#define ITEMID_CAMERA_BRIGHTNESS        "brightness"
#define ITEMID_CAMERA_CONTRAST          "contrast"
#define ITEMID_CAMERA_STURATION         "saturation"

#define ITEMID_CAMERA_NR2DLEVEL             "nr2dlevel"
#define ITEMID_CAMERA_NR3DLEVEL             "nr3dlevel"
#define ITEMID_CAMERA_EXPOSURECOMPENSATION              "exposurecompensation"
#define ITEMID_CAMERA_EXPOSUREFLICKER           "exposureflicker"

// TextMessage
#define ITEMID_TEXTMESSAGE_SEND_FORM        "sendfrom"
#define ITEMID_TEXTMESSAGE_SEND_TO          "sendto"

// Wallpaper
#define WALLPAPER_SELECT                    "wallpaper_select"

// Edk
#define ITEMID_EDK_PROMPT                   "edkprompt"

#ifdef IF_FEATURE_GETBALANCE
// show balance
#define BALANCE_ACCOUNT                     "balance_account"
#define BALANCE_VALUE                       "balance_value"
#endif

#ifdef IF_FEATURE_METASWITCH_ACD
// metaswitch-acd
#define MESW_DISPCODE_QUEUE                     "mtsw_dispcode_queue"
#define MESW_DISPCODE_CODE                      "mtsw_dispcode_code"
#endif

#ifdef IF_FEATURE_METASWITCH
//  metaswitch
#define MESW_USER_NAME                     "mtsw_username"
#define MESW_PASSWORD                      "mtsw_password"
#endif

#define MESW_VOICE_MAIL                     "mtsw_voicemail"
#ifdef IF_FEATURE_METASWITCH_VOICEMAIL
//  metaswitch
#define MESW_VOICE_MAIL_FROM                "mtsw_voicemail_from"
#define MESW_VOICE_MAIL_NUMBER              "mtsw_voicemail_number"
#define MESW_VOICE_MAIL_TIME                "mtsw_voicemail_time"
#define MESW_VOICE_MAIL_PLAY_STATUS         "mtsw_voicemail_playstatus"
#endif

// show balance
#define E911_LOCATION                       "e911_location"

#define DEFAULT_ACCOUNT                     "defaultaccount"

// screensaver
#define SCREEN_SAVER_WAIT_TIME              "waittime"
#define SCREEN_SAVER_SHOW_SYSTEM_CLOCK      "showsystemclock"
#define SCREEN_SAVER_TYPE                   "screensavertype"


#endif // _DEFINE_ITEMNAME_H_
