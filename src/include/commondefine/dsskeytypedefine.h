#ifndef _DSSKEY_TYPE_DEFINE_HEADER_
#define _DSSKEY_TYPE_DEFINE_HEADER_

enum DSSKEY_MODULE_TYPE//管理模块类型
{
	DMT_LINEKEY_MODULE = 0,
	DMT_MEMKEY_MODULE = 1,
	DMT_PROGRAMEKEY_MODULE = 2,
	DMT_NOTIFYKEY_MODULE = 3,
	DMT_EXP20_MODULE = 20,
	DMT_EXP38_MODULE = 38,
	DMT_EXP39_MODULE = 39,
	DMT_EXP40_MODULE = 40,
	DMT_EHS = 41,
	DMT_EXP50_MODULE = 50,
	DMT_EXPCM69_MODULE = 69,
	DMT_NON = 0xff,
};

enum DssKey_Type
{
	DT_NONE = -1,
	DT_NA           = 0,                // "N/A"
	DT_CONF         = 1,                    // "Conference"
	DT_FWD          = 2,                        // "Forward"
	DT_TRAN         = 3,                    // "Transfer"
	DT_HOLD         = 4,                    // "Hold"
	DT_DND          = 5,                        // "DND"
	DT_REDIAL       = 6,                    // "Redial"
	DT_CALLRETURN   = 7,                // "Call Return"
	DT_SMS          = 8,                        // "SMS"
	DT_PICKUP       = 9,                    // "Pick Up"
	DT_CALLPARK     = 10,               // "Call Park"
	DT_CUSTOM       = 11,                   // "DTMF"
	DT_VOICEMAIL    = 12,               // "Voice Mail"
	DT_SPEEDDIAL    = 13,               // "Speed Dial"
	DT_INTERCOM     = 14,               // "Intercom"
	DT_LINE         = 15,               // "Line"
	DT_BLF          = 16,                       // "BLF"
	DT_URL          = 17,                       // "URL"
	DT_GROUPLISTEN  = 18,               // "Group Listen"
	DT_PUBLICHOLD   = 19,               // "Public Hold"
	DT_PRIVATEHOLD  = 20,               // "Private Hold"
	DT_BLA          = 21,                       // "Share Line"
	DT_XMLGROUP     = 22,               // "XML Group"
	DT_GROUPPICKUP  = 23,               // "Group Pickup"
	DT_PAGING       = 24,                   // "Paging"
	DT_RECORD       = 25,                   // "Record"
	DT_VOICERECORD  = 26,
	DT_XMLBROWSER   = 27,               // "XML Browser"
	DT_IDLE_CALLLIST = 28,
	DT_IDLE_CONTACTS = 29,
	DT_IDLE_MENU = 30,
	DT_IDLE_SWITCHACCOUNT = 31,
	DT_IDLE_NEWSMS = 32,
	DT_IDLE_STATUS = 33,
	DT_HOTDESKING   = 34,               // "Hot Desking"
	DT_URLRECORD    = 35,               // "URL Record"
	DT_BUTTON       = 36,
	DT_EXP_SWITCH   = 37,               // EXP39翻页Dsskey
	DT_LDAP         = 38,                   // "LDAP"
	DT_BLFLIST      = 39,                   // "BLF List"
	DT_PREFIX       = 40,                   // "Prefix"
	DT_ZERO         = 41,                   // "Zero-sp-touch"
	DT_ACD          = 42,                       // "ACD"
	DT_LOCALDIR = 43,
	DT_BSDIR = 44,
	DT_LOCALGROUP   = 45,               // "Local Group"
	DT_BSGROUP      = 46,                   // "BroadSoft Group"
	DT_IDLE_REMOTE = 47,
	DT_GET_BALANCE = 48,
	DT_TIPTEL_BUTTON = 49,

	// yzh test
	DT_KEYPAD_LOCK = 50,
	DT_IDLE_SWITCHACCOUNT_UP = 51,
	DT_IDLE_SWITCHACCOUNT_DOWN = 52,
	DT_IDLE_NETWORKCALLLOG = 53,
	DT_NETWORKCALLLOG_SINGLE = 54,
	DT_MEET_ME_CONFERENCE = 55,

	DT_BSFT_RETRIEVE_PARK = 56,

	DT_HOTELING = 57,
	DT_ACD_TRACE = 58,
	DT_DISPCODE = 59,
	DT_ESCALATE = 60,
	DT_FAVORITES    = 61,               // "Favorites"

	DT_NETWORK_FAVORITE = 62,         // "NetWork Favorite"
	DT_UC_FAVORITE = 63,
	DT_BUDDIES = 64,
	DT_MY_STATUS = 65,
	DT_PAGING_LIST = 66,
	DT_MTSWDIR = 67,
	DT_MTSW_LOGOUT = 68,
	DT_MESSAGE_LIST = 69,
	DT_MTSW_CALLPARK = 70,

	// http://192.168.1.99/Bug.php?BugID=27306
	// 8x8 EFK功能需求开发
	DT_CONF_SERV = 71,
	DT_MISSED_CALL = 72,

	DT_CUSTOM_KEY = 73,

	DT_CAMERA = 74,
#if IF_FEATURE_METASWITCH
	DT_MTSW_LOGIN = 75,
#else
	DT_AUTO_ANSWER = 75,
#endif
#ifdef IF_FEATURE_GENBAND
	DT_BLFGROUP  = 76,
	DT_WIFI = 81,
#else
	DT_WIFI = 76,
	DT_BLFGROUP  = 81,
#endif

#if IF_FEATURE_FAVORITE_DIR
	DT_FAVORITE_CONTACT = 81,
#endif //IF_FEATURE_FAVORITE_DIR

#if IF_BUG_33083
	DT_XMLBROWSER_INFO = 77,
	DT_BTLINE = 83,
#else
	DT_BTLINE = 77,
	DT_XMLBROWSER_INFO = 83,
#endif
	DT_USB = 78,
#if IF_BUG_PLCMDIR
	DT_PLCM_PHONEBOOK = 79,
	DT_EXT_DISPLAY = 84,
#else
	DT_EXT_DISPLAY = 79,
	DT_PLCM_PHONEBOOK = 84,
#endif
	DT_BLUETOOTH = 80,

	DT_UNPARK = 82,//77,

#ifdef IF_FEATURE_PSTN
	DT_LOCAL_FAVORITE = 85,
#endif // IF_FEATURE_PSTN

	DT_MEDIA_STATS = 85,    // 音视频统计
	DT_3RD_VMR = 86,        // 第三方VMR
	DT_VIDEO_CONF = 87,     // 视频会议

#if IF_FEATURE_HIDE_DSSKEY
	DT_KEYEVENT = 101,
#endif //IF_FEATURE_HIDE_DSSKEY

	DT_SRTP = 101,
#if IF_FEATURE_METASWITCH
	DT_AUTO_ANSWER = 103,
#else
	DT_MTSW_LOGIN = 103,
#endif

	// 200 以后的由Lync所用
	DT_LYNC_START = 200,
	DT_LYNC_PRESENCE = 201,	// Lync Presence 支持用户调整位置
	DT_BOSSADMIN = 202,		// Lync BossAdmin
	DT_LYNC_FAVORITE = 203,	// Lync Favorite
	DT_LYNC_GROUP = 204,	// Lync Group
	DT_INTERCOM_LIST = 205,	// "Intercom list"

	DT_SLA_LINE = 206,	// "Sla 线路"

	DT_LYNC_LAST = 207,
	DT_LYNC_END = 299,
	///////end lync ///////////

	// 300 以后的由Android 使用
	DT_ANDROID_START = 300,

	DT_ANDROID_END = 399,

	// 虚拟KEY_EVENT 类型，占位
	DT_KEYEVENT = 512,

	DT_LAST

};

#endif