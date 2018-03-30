#ifndef AUTHORITY_ID_DIFINE_H_
#define AUTHORITY_ID_DIFINE_H_

// 需求：液晶网页要配置分开
// 场景：管理员可以通过网页配置，但客户通过话机不能配置
// 提示：对应项与黑白屏项的项名要一致，否则应添加到映射表中
//       定义宏不该与configid冲突

//Account
#define kszAuthorityRegisterLineActive "account_status"
#define kszAuthorityRegisterAccountLabel "account_label"
#define kszAuthorityRegisterDisplayName "display_name"
#define kszAuthorityRegisterName "register_name"
#define kszAuthorityRegisterUserName "user_name"
#define kszAuthorityRegisterSipServer "sip_server"
#define kszAuthorityRegisterAccountPassword "account_password"
#define kszAuthorityRegisterEnabledOutboundProxyServer "enabled-outbound-proxy-server"
#define kszAuthorityRegisterOutboundProxyServer "outbound-proxy-server"
#define kszAuthorityRegisterFallBackInterval "fallback_interval"
#define kszAuthorityRegisterNATTraversal "NAT_traversal"
#define kszAuthorityRegisterSTUNServer "STUN_server"

//Status
#define kszAuthorityStatusIPv4 "status_ip"
#define kszAuthorityStatusIPv6 "status_ipv6"
#define kszAuthorityStatusMAC "status_mac"
#define kszAuthorityStatusWifiMAC "Status_WIFI_MAC"
#define kszAuthorityStatusHW "status_hw"
#define kszAuthorityStatusMore "status_more"
#define kszAuthorityCfgVer   "Status_CFG_VER"

//Features/General
#define kszFeaturesGeneralKeyAsSend "advanced_call_feature_key_as_send"
#define kszFeaturesGeneralHistoryRecord "advanced_call_feature_history_record"

//Basic/Time & Data/General
#define kszBasicGeneralTimeDataSntp "date_and_time_sntp"
#define kszBasicGeneralTimeDataManual "date_and_time_manual"
#define kszBasicGeneral "date_and_time_general"

//ForwardOption
#define kszAllwaysForward "call_forward_always_forward"
#define kszBusyForward "call_forward_busy_forward"
#define kszNoAnswerForward "call_forward_no_answer_forward"

// Reset Configuration action
#define kszResetLocalConfig         "reset_local_config"
#define kszResetWebConfig           "reset_web_config"
#define kszResetNonStaticConfig     "reset_non_static_config"
#define kszResetStaticConfig        "reset_static_config"
#define kszResetToFactory           "reset_to_factory"
#define kszResetUserDataUserCfg     "reset_userdata_usercfg"

#define kszAutoAnswerTimer          "auto_answer_timer"

#endif  // AUTHORITY_ID_DIFINE_H_
