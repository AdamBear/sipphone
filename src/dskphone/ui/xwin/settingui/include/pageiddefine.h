#ifndef __PAGE_ID_DEFINE_H__
#define __PAGE_ID_DEFINE_H__

const char * const   kszPageIDMainMenu = "mainmenu";

//call controll
const char * const   kszPageIDBSFTCallControll = "callcontrol";
const char * const   kszPageIDXSIPassword = "xsipassword";
const char * const   kszPageIDLineSelect = "lineselect";
const char * const   kszPageIDAnyWhereLocationList = "callcontrol_anywhere";
const char * const   kszPageIDAnyWhereAddLocation = "anywhere_addlocation";
const char * const   kszPageIDAnyWhereEditLocation = "anywhere_editlocation";

const char * const   kszPageIDCallControllMobility = "callcontrol_mobility";
const char * const   kszPageIDCallControllRemoteOffice = "callcontrol_remoteoffice";
const char * const   kszPageIDCallControllCFNR = "callcontrol_forwardnotreachable";
const char * const   kszPageIDCallControllHideNumber = "callcontrol_hidenumber";
const char * const   kszPageIDCallControllAnonymoRrejection = "callcontrol_anonymousrejection";
const char * const   kszPageIDCallControllSimultaneousRingList = "callcontrol_sinultaneousring";
const char * const   kszPageIDCallControllSimultaneousRingAdd = "callcontroll_simultaneousringadd";
const char * const   kszPageIDCallControllSimultaneousRingEdit =
    "callcontroll_simultaneousringedit";
const char * const   kszPageIDCallControllCallWaiting = "callcontrol_callwaiting";
const char * const   kszPageIDCallControllGNF = "callcontrol_group_night_forward";
const char * const   kszPageIDCallControllForwardSelective = "callcontrol_forward_selective";
const char * const   kszPageIDCallControllInitialACDStatus = "callcontrol_initialacdstate";
const char * const   kszPageIDCallControllSetACDStatus = "callcontrol_setacdstate";
const char * const   kszPageIDACDUnavailableReasonCode = "acd_unavailablereasoncode";
const char * const   kszPageIDCallControllCallRecord = "callcontrol_callrecord";

// Broadsoft Executive-Assistant
const char * const   kszPageIDCallControllExecutiveDetails = "callcontrol_executive";
const char * const   kszPageIDCallControllExecutiveFilter = "executivefilter";
const char * const   kszPageIDCallControllExecutiveScreen = "executivescreen";
const char * const   kszPageIDCallControllExecutiveAlter = "executivealter";
const char * const   kszPageIDCallControllExecutiveCommon = "executivecommon";
const char * const   kszPageIDCallControllExecutiveListview = "executivelistview";
const char * const   kszPageIDCallControllAssistantDetails = "callcontrol_executive_assistant";
const char * const   kszPageIDCallControllAssistantCommon = "assistantcommon";
const char * const   kszPageIDCallControllAssistantListview = "assistantlistview";

//Security-Classfication
const char * const   kszPageIDSecurityClassfication = "callcontrol_securityclassification";

//status
const char * const   kszPageIDStatus = "status";
const char * const   kszPageIDStatusPhone    = "status_phone";
const char * const   kszPageIDStatusNetwork  = "status_network";
const char * const   kszPageIDStatusNetworkIPv4  = "networkipv4status";
const char * const   kszPageIDStatusNetworkIPv6  = "networkipv6status";
const char * const   kszPageIDStatusAccount  = "status_accounts";
const char * const   kszPageIDStatusRTP  = "status_rtp";
const char * const   kszPageIDStatusMore = "status_more";

//features
const char * const   kszPageIDFeature    = "features";
const char * const   kszPageIDCallForward    = "callforward";
const char * const   kszPageIDCallForwardLineSelect  = "customforward";
const char * const   kszPageIDAlwaysForward  = "always-forward";
const char * const   kszPageIDBusyForward    = "busy-forward";
const char * const   kszPageIDNoAnswerForward    = "noanswer-forward";
const char * const   kszPageIDCallWaiting    = "call_waiting";
const char * const   kszPageIDAutoAnswer = "autoanswer";
const char * const   kszPageIDKeyAsSend  = "key_as_send";
const char * const   kszPageIDHistorySetting = "history_save";
const char * const   kszPageIDHotLine    = "hotline-number";
const char * const   kszPageIDIntercom   = "intercom";
const char * const   kszPageIDCallCompletion = "call_completion";
const char * const   kszPageIDCallAutoRedial = "autoredial";
const char * const   kszPageIDAnonymousCall  = "anonymouscall";
const char * const   kszPageIDPagingList = "multpaginglist";
const char * const   kszPageIDPagingListEdit = "paginglistedit";
const char * const   kszPageIDTraceOut   = "networktraceroute";
const char * const   kszPageIDPing   = "networkping";
const char * const   kszPageIDPingResult = "ping-result";
const char * const   kszPageIDTraceResult    = "tracerount-result";
const char* const   kszPageIDOthers = "others";
const char* const   kszPageIDGeneral = "general";
const char* const   kszPageIDDefaultAccount = "defaultaccount";

//DND
const char * const   kszPageIDPhoneDND   = "phonednd";
const char * const   kszPageIDAccountDND = "customaccountdnd";
const char * const   kszPageIDDNDLineSelect  = "customdnd";

//setting type
const char * const   kszPageIDSettingType        = "main_settings";
const char * const   kszPageIDBasicSetting       = "basic_setting_list";
const char * const   kszPageIDAdvancedSetting    = "advanced_setting_list";
const char * const   kszPageIDChangePassword     = "set_password";

//language
const char * const   kszPageIDLanguage   = "language";
//timedata
const char * const   kszPageIDTimeData   = "time_zone";
const char * const   kszPageIDSNTPSetting    = "date_and_time_sntp";
const char * const   kszPageIDSTimeDataFormat    = "date_and_time_format";
const char * const   kszPageIDManualSetting  = "date_and_time_manual";

const char * const   kszPageIDContrast   = "contrast";
//change pin
const char * const   kszPageIDChangePin  = "phone_unlock_pin";
//unlock  check
const char * const   kszPageIDPhoneUnlockCheck   = "phoneunlockcheck";
//phone lock
const char * const   kszPageIDPhoneLock  = "phonelock";

//ringtone
const char * const   kszPageIDRingTone   = "sound_ringtone";
//account set ringtone
const char * const   kszPageIDRingToneSetting    = "ringtonesetting";

//bluetooth
//#ifdef IF_SUPPORT_BLUETOOTH
const char * const   kszPageIDBluetoothMenu      = "bluetooth";
const char * const   kszPageIDBluetoothScan      = "bluetoothscan";
const char * const   kszPageIDBluetoothPaired    = "bluetoothpaired";
const char * const   kszPageIDBluetoothDevDetail = "bluetoothdevdetail";
const char * const   kszPageIDBluetoothEdit      = "bluetoothedit";
const char * const   kszPageIDBluetoothPassword  = "bluetoothpwd";
const char * const   kszPageIDBTChannelControll  = "bluetoothchannelcontroll";
//#endif
//wifi
#ifdef IF_SUPPORT_WIFI
const char * const   kszPageIDWifiMenu           = "wifi";
const char * const   kszPageIDWifiScan           = "wifiscan";
const char * const   kszPageIDWifiKnown          = "wifiknown";
const char * const   kszPageIDWifiStatus         = "wifistatus";
const char * const   kszPageIDWifiDetail         = "wifidetatil";
const char * const   kszPageIDWifiEdit           = "wifiedit";
const char * const   kszPageIDWifiAdd            = "wifiadd";
const char * const   kszPageIDWifiPassword       = "wifipwd";
const char * const   kszPageIDWifiDetail2Add     = "wifidetail2add";
#endif
//advance-setting-network
const char * const   kszPageIDWanPortIpv4StaticClient    = "wanport-ipv4-staticipclient";
const char * const   kszPageIDWanPortIpv6StaticClient    = "wanport-ipv6-staticipclient";
const char * const   kszPageIDWanPortIpv4DhcpClient  = "wanport-ipv4-dhcpipclient";
const char * const   kszPageIDWanPortIpv6DhcpClient  = "wanport-ipv6-dhcpipclient";
const char * const   kszPageIDWanPortOption          = "network_wan_port_option";
const char * const   kszPageIDNetworkVlanList    = "network_vlan_list";
const char * const   kszPageIDNetworkVlanWanPort = "network_wvlan";
const char * const   kszPageIDNetworkVlanPcPort  = "network_pcvlan";
const char * const   kszPageIDNetworkVlanDhcp    = "network_dhcpvlan";
const char * const   kszPageIDNetworkNAT = "network_nat";
const char * const   kszPageIDNetworkVPN = "network_wvpn";
const char * const   kszPageIDNetwork8021X = "network_w802.1x";
const char* const   kszPageIDNetworkWebServer = "network_wweb_server";
const char* const   kszPageIDNetworkLLDP = "network_lldp";
const char* const   kszPageIDNetworkCDP = "vlancdp";


const char* const   kszPageIDWanPortIPV4 = "network-wanport-ipv4";
const char* const   kszPageIDWanPortIPV6 = "network-wanport-ipv6";

//advance-setting-autop
const char * const   kszPageIDAutoProvision = "autoprovision";

const char * const   kszPageIDAdvAccountList = "account_list";

#ifdef IF_FEATURE_PSTN
const char * const   kszPageIDPSTNAccountList = "pstnaccountlist";
const char * const   kszPageIDAdvPSTNAccountSetting = "advancedaccountsettingpstn";
const char * const   kszPageIDPSTNFlashTime = "pstn-flashtime";
const char * const   kszPageIDPSTNCountry = "pstn-country";
#endif

// advance-reset
const char * const   kszPageIDResetConfig = "reset_to_factory";

//advanced-account
#if IF_FEATURE_CLOUD_ACCOUNT
const char * const   kszPageIDAdvCloudAccountSetting = "advancedaccountsettingcloud";
#endif
const char * const   kszPageIDAdvCommonAccountSetting = "advancedaccountsettingcommon";

//softkey-label
const char* const   kszPageIDSoftkeyLabelList = "softkeylabellist";
const char* const   kszPageIDLinekeyList = "linekeylist";
const char* const   kszPageIDDsskeySetting = "dsskeysetting";

// history
const char * const   kszPageIDHistory = "history";
const char * const   kszPageIDDirectory = "directory";

// zero-touch
const char * const   kszPageIDZeroTouch = "zerotouch";
const char * const   kszPageIDZeroStatus = "zero-status";
const char * const   kszPageIDZeroNetwork = "zero-network";
const char * const   kszPageIDZeroAutoP = "zero-autop";
const char * const   kszPageIDZeroRedirect = "zero-redirector";

//acd disposition code
const char * const   kszPageIDACDDispositionCode = "acddispositioncode";
//acd emergency escalation
const char* const   kszPageIDACDEmergencyEscalation = "acdemergencyescalation";
const char * const   kszPageIDHotelingLogin = "hotelinglogin";
const char * const   kszPageIDGuestUnlockCheck = "guestunlockcheck";

// voice mail
const char * const   kszPageIDViewVoiceMail = "view_voice_mail";
const char * const   kszPageIDVoiceMailList = "voicemaillist";
const char * const   kszPageIDVoiceMailDetail = "voicemaildetail";
const char * const   kszPageIDVoiceMailPlay = "voicemailplay";
const char * const   kszPageIDVoiceMailAddToPersonal = "voicemailaddtopersonal";
const char * const   kszPageIDVoiceMailCodeList = "set_voice_mail";

// textmessage
const char * const   kszPageIDTextMsg = "text_message_list";
const char * const   kszPageIDTextMsgNew = "text_message_new_message";
const char * const   kszPageIDTextMsgView = "textmessage-view";
const char * const   kszPageIDTextMsgSend = "textmessage-send";
const char * const   kszPageIDTextMsgInbox = "text_message_inbox";
const char * const   kszPageIDTextMsgSentbox = "text_message_sentbox";
const char * const   kszPageIDTextMsgOutbox = "text_message_outbox";
const char * const   kszPageIDTextMsgDraftbox = "text_message_draftbox";

const char * const   kszPageIDUserMode = "usermode";

//guide
#ifdef IF_SETTING_WIZARD
const char * const   kszPageIDGuideLanguaue = "guide-language";
const char * const   kszPageIDGuideTimeDate = "guide-timedate";
const char * const   kszPageIDGuideNetworkWanPort = "guide-network-wanport";
const char * const   kszPageIDGuideCommonSipLine = "guide-common-sipline";
#if IF_FEATURE_CLOUD_ACCOUNT
const char * const   kszPageIDGuideCloudSipLine = "guide-cloud-sipline";
#endif
#endif

// usb record
const char * const   kszPageIDUsbRecord = "usbrecord";
const char * const   kszPageIDUsbAudioList = "usbrecord-audiolist";
const char * const   kszPageIDUsbAudioPlayer = "usbrecord-audioplayer";
const char * const   kszPageIDUsbStorageInfo = "usbrecord-storageinfo";
const char * const   kszPageIDUsbIdleRecord = "usbrecord-idlerecord";
const char* const    kszPageIDUsbPhotoList = "usbphotolist";
const char* const    kszPageIDUsbPhotoBrowser = "usbphotobrowser";

const char* const    kszPageIDWallPaper = "wallpaper";
const char* const    kszPageIDExpWallPaper = "expwallpaper";

// edk
const char * const   kszPageIDEdkPrompt = "edkprompt";

// hotdesking
const char * const   kszPageIDHotDeskBase = "hotdeskbase";
#if IF_BUG_23550_Pingan_Menu
// hot desking data
const char * const   kszPageIDHotDeskData = "hotdeskdata";
#endif

#ifdef IF_FEATURE_GETBALANCE
// balance
const char* const   kszPageIDBalance = "balance";
#endif

// idlepwdcheck
const char * const   kszPageIDIdleResetNeedPwd = "idle-resetneedpwd";

#ifdef IF_SUPPORT_VIDEO
const char * const   kszPageIDUSBViewCamera = "usb_view_camera";
#endif

#if IF_FEATURE_METASWITCH_VOICEMAIL
const char* const   kszPageMTSWVoiceMailList = "mtsw_voicemaillist";
const char* const   kszPageMTSWVoiceMailDetail = "mtsw_voicemaildetail";
const char* const   kszPageMTSWVoiceMailPlay = "mtsw_voicemailplay";
const char* const   kszPageMTSWVoiceMailOption = "mtsw_voicemailoption";
#endif

#if IF_FEATURE_METASWITCH
const char* const   kszPageMTSWCommportal = "mtswcommportal";
#endif

#if IF_FEATURE_METASWITCH_ACD
const char* const   kszPageMTSWAcdList = "mtsw_acdlist";
const char* const   kszPageMTSWAcdDetail = "mtsw_acddetail";
const char* const   kszPageMTSWAcdMembers = "mtsw_acdmembers";
const char* const   kszPageMTSWAcdMyStatus = "mtsw_acdmystatus";
const char* const   kszPageMTSWAcdSelectDispcode = "mtsw_acdselectdispcode";
#endif

#if IF_FEATURE_GENBAND_E911
const char* const   kszPageIDE911Location = "e911_location";
const char* const   kszPageIDSetE911Location = "e911_set_location";
#endif

const char* const   kszPageIDScreenSaver = "screensaver";
const char* const   kszPageIDTransparency = "transparency";

const char* const   kszPageIDBLFRingType = "blf_ring_type";
const char* const   kszPageIDMyStatus = "mystatus";
const char* const   kszPageIDBackLight = "backlight";

#endif // __PAGE_ID_DEFINE_H__
