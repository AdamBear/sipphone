#ifndef _SERVICE_DSK_DEFINE_H_
#define _SERVICE_DSK_DEFINE_H_

/************************************************************************/
/*
	GUI提供外部使用的定义
	定义命名规则：dsk_、DSK_开头
*/
/************************************************************************/

/////////////////////////历史遗留的定义，考虑由web端自己定义/////////////////
//DSK Change Info
typedef struct DsskeyChangeInfo
{
#define DSSKEYCHANGEKEYLEN   64
	// key
	char  szKey[DSSKEYCHANGEKEYLEN];
	// key length
	int   iLength;
	// 对应的修改级别
	int   iLevel;
	// 修改类型
	int   iSettingType;
}Dsk_DsskeyChangeInfo;

//TR069等平台 监控通话状态事件
enum TalkEventInfo
{
	TALK_INFO_CALL_ID = 0,
	INCOMING_CALLS_RECEIVED = 1,
	INCOMING_CALLS_ANSWERED = 2,
	INCOMING_CALLS_CONNECTED = 3,
	OUTGOING_CALLS_ATTEMPTED = 4,
	OUTGOING_CALLS_ANSWERED = 5,
	OUTGOING_CALLS_CONNECTED = 6,
	TOTAL_CALL_TIME = 7,
	CALL_END_CALLID = 8,
};

// 诊断平台查询GUI信息
typedef enum
{
	DSK_DIAG_INFO_TYPE_DIAG_NONE,
	DSK_DIAG_INFO_TYPE_APP = 1,					// 1-99 程序运行信息

	DSK_DIAG_INFO_TYPE_CONFIG = 101,			// 100-199 配置资源相关信息
	DSK_DIAG_INFO_TYPE_CONFIG_DSK = 102,		// GUI动态保存信息的路径
	DSK_DIAG_INFO_TYPE_CONFIG_USER = 103,		// 用户动态上传铃声、图片的路径

	DSK_DIAG_INFO_TYPE_CONFIG_ACCOUNT_COUNT = 150,		// 账号数量

	DSK_DIAG_INFO_TYPE_LICENSE = 200,			// 200-299 配置资源相关信息

	DSK_DIAG_INFO_TYPE_TALK = 1000,				// 1000-1999 通话相关
	DSK_DIAG_INFO_TYPE_TALK_COUNT = 1001,		// 返回当前通话路数
	DSK_DIAG_INFO_TYPE_TALK_STATE = 1002,		// 返回当前通话状态： 0/1 不存在/通话中
	DSK_DIAG_INFO_TYPE_TALK_EXIST = 1003,		// 返回当前是否存在通话
	DSK_DIAG_INFO_TYPE_TALK_VOICE = 1500,		// 声音数据

	DSK_DIAG_INFO_TYPE_CONTACT = 2000,			// 2000-2999 联系人相关
	DSK_DIAG_INFO_TYPE_CONTACT_GROUP = 2001,	// 返回各类联系人组信息 l参数：联系人类型定义
	DSK_DIAG_INFO_TYPE_CONTACT_COUNT = 2002,	// 返回各类联系人数量
	DSK_DIAG_INFO_TYPE_CONTACT_INFO = 2003,		// 返回某个联系人信息 l参数：联系人int型id

	DSK_DIAG_INFO_TYPE_MENU = 3000,				// 3000-3999 菜单操作相关
	
	DSK_DIAG_INFO_TYPE_MENU_USB_INFO	= 3001,	// USB状态
	DSK_DIAG_INFO_TYPE_MENU_USB_END		= 3020,	// USB状态

	DSK_DIAG_INFO_TYPE_MENU_EXP_INFO	= 3021,

	DSK_DIAG_INFO_TYPE_MENU_BLUETOOTH_INFO = 3031,

	DSK_DIAG_INFO_TYPE_MENU_EHS_INFO	= 3041,	

	DSK_DIAG_INFO_TYPE_MENU_WIFI_INFO = 3051,

	// sfb section
	DSK_DIAG_INFO_TYPE_MENU_SFB_PRESENCE = 3601,		// sfb账号presence状态

	DSK_DIAG_INFO_TYPE_MENU_SFB_EXCHANGE_STATE = 3602,	// sfb账号exchange状态

	DSK_DIAG_INFO_TYPE_MENU_SFB_BTOE_STATE = 3610,		// sfb账号BTOE状态

	DSK_DIAG_INFO_TYPE_MENU_SFB_CAP_STATE = 3615,	// sfb账号CAP状态

	DSK_DIAG_INFO_TYPE_MENU_SFB_END = 3999,				// sfb相关

	//camera 
	DSK_DIAG_INFO_TYPE_MENU_CAMERA_INFO = 4000,
	DSK_DIAG_INFO_TYPE_MENU_CAMERA_ID_LIST,
	
	//AUDIO 
	DSK_DIAG_INFO_TYPE_MENU_AUDIO_IO_INFO = 4050,

	//VCS PHONE
	DSK_DIAG_INFO_TYPE_MENU_VCS_PHONE_INFO = 4080,

	//DECT MICPOD
	DSK_DIAG_INFO_TYPE_MENU_DECT_MIC_INFO = 4100,

	DSK_DIAG_INFO_TYPE_MAX = 10000,
}Dsk_DiagInfoType;


//////////////////////////// vcs  结构体//////////////////////////////////////
#define CODECNAMESIZE 32
// 音频统计信息
struct AudioStatsInfo
{
	char    SendCodec[CODECNAMESIZE];		
	char    RecCodec[CODECNAMESIZE];
	int		SendBitrate;
	int		RecBitrate;  
	int		SendSamplerate;
	int		RecSamplerate;

	int		SendRtpPackets;
	int		RecRtpPackets;
	int		SendRtcpPackets;
	int		RecRtcpPackets;
	int		SendLostFrac;  
	int		RecLostFrac;
	int		SendLostTotal; 
	int		RecLostTotal; 

	int		SendJitCur; 
	int		RecJitCur;  
	int		SendJitAvg;
	int		RecJitAvg;
	int		SendJitMax;
	int		RecJitMax; 
	int		SendDelay;
	int		RecDelay;
};

// 视频统计信息
struct VideoStatsInfo
{
	char    SendCodec[CODECNAMESIZE];		 /*transmit codec name*/
	char    RecCodec[CODECNAMESIZE];		 /*receive  codec name*/
	int		SendBitrate;                     /*transmit bit rate*/
	int		RecBitrate;                     /*receive  bit rate*/
	int		SendFramerate;                   /*transmit frame rate*/
	int		RecFramerate;                   /*receive  frame rate*/

	int		SendLostFrac;                   /*transmit packets lost rate, depend on remote feedback via RTCP*/
	int		RecLostFrac;                   /*receive  packets lost rate*/
	int		SendLostTotal;                  /*transmit packets lost count, depend on remote feedback via RTCP*/
	int		RecLostTotal;                  /*receive  packets lost count*/

	int		SendJitCur;                     /*transmit interarrival jitter, depend on remote feedback via RTCP*/
	int		RecJitCur;                     /*receive  interarrival jitter*/
	int		SendJitAvg;                     /*transmit average jitter, calculate by the history of RTCP*/
	int		RecJitAvg;                     /*receive  average jitter*/
	int		SendJitMax;                     /*transmit maximum jitter, calculate by the history of RTCP*/
	int		RecJitMax;                     /*receive  maximum jitter*/
	int		SendDelay;                    /*transmit round trip delay, may be the same as Rx path*/
	int		RecDelay;                    /*receive round trip delay*/

	int		SendWidth;                       /*transmit video width*/
	int		RecWidth;                       /*receive  video width*/
	int		SendHeight;                      /*transmit video height*/
	int		RecHeight;                      /*receive  video height*/
};

#define TALKNAMESIZE 64

// 每路通话信息
struct SessionInfo
{
	int		iCallId;
	// 号码
	char	szNumber[TALKNAMESIZE];
	// 名称
	char	szDispName[TALKNAMESIZE];
	// 服务器地址
	char	szServer[TALKNAMESIZE];
};

#define SESSIONSIZE 10
#define COMMON_STRING_SIZE 128

// 音视频统计
struct MediaStatsInfo
{
	int				iCallId;
	AudioStatsInfo	audio;
	VideoStatsInfo	video;
	VideoStatsInfo	share;

	// 其他信息
#define DSK_PROTOCOL_SIP 0
#define DSK_PROTOCOL_H323 1

	int				iProtocol;		// 账号协议
	char			szUserAgent[COMMON_STRING_SIZE];	// 对端设备信息

	int GetTotalRecvBitrate()
	{
		return audio.RecBitrate + video.RecBitrate + share.RecBitrate;
	}

	int GetTotalSendBitrate()
	{
		return audio.SendBitrate + video.SendBitrate + share.SendBitrate;
	}
};

// 通话信息
struct TalkStatisticsInfo
{
	// 当前有多少路通话
	int				iSessionSize;

	// 保存话路信息
	SessionInfo		stSessionArr[SESSIONSIZE];
};

// 分辨率方�?
struct DskScreenInfo
{
	int		nWidth;
	int		nHeight;
	int		nFramerate;
};

#define DEVICE_SCREEN_INFO_MAX 5
// 显示设备信息
struct DskDisplayDevInfo
{
	int		nScreen_x;	// 屏幕大小x
	int		nScreen_y;	// 屏幕大小y

	int		nFocus;		// 焦点项索�?
	int		nCount;		// 分辨率个数统�?
	DskScreenInfo	stInfoArr[DEVICE_SCREEN_INFO_MAX];	// 保存话路信息
};

#define LYNC_URL_SIZE	256
// 每个账号的Lync信息
struct LyncServiceStatus
{
	// Phone status
	char commonAreaPhone;	//是否是cap账号

#define BTOE_STATUS_UNPAIRED 0
#define BTOE_STATUS_PAIRED 1
#define BTOE_STATUS_SIGNEDIN 2
	char btoeStatus;		// 联动状态

	char licenseStatus;		// 许可证书状态

	// Server status
	char szCPSUrl[LYNC_URL_SIZE]; // Cert Provisioning Service URL

	char szUpdateServerUrl[LYNC_URL_SIZE];	// Update Server URL

	char szEdgeServer[LYNC_URL_SIZE];		// Edge Server

	char szVoiceMailUri[LYNC_URL_SIZE];		// Voice Mail URI

	char szEmail[LYNC_URL_SIZE];		// Email URI

	char szABSUrl[LYNC_URL_SIZE];			// ABS URL

	char szLISUrl[LYNC_URL_SIZE];			// LIS URL

	char szEWSUrl[LYNC_URL_SIZE];			// EWS URL/ exchange url


	////////get from sip ///////////
	char szSTSUrl[LYNC_URL_SIZE];	// STS URI

	char szFcusFactoryUrl[LYNC_URL_SIZE];	// Fcus Factory URI

	char szHomeServerUrl[LYNC_URL_SIZE];	// Home Server URL

	char szMRASUrl[LYNC_URL_SIZE];	// MRAS URL

	char szCallParkUrl[LYNC_URL_SIZE];	// CallPark Server URI

	char szQoeUrl[LYNC_URL_SIZE];

	// account type
#define ACC_TYPE_UNKNOW		0
#define ACC_TYPE_PIN		1
#define ACC_TYPE_ONPREM		2
#define ACC_TYPE_MANAGED	3
#define ACC_TYPE_FEDERATED	4
	int iAccType;

	// Sign-in Authentication Type
#define AUTHENTICATION_TYPE_UNSET 		0
#define AUTHENTICATION_TYPE_NTLM		1
#define AUTHENTICATION_TYPE_KERBEROS	2
#define AUTHENTICATION_TYPE_NEGOTIATE	3
#define AUTHENTICATION_TYPE_TLS_DSK		4
	int iSignInAuthType;
	
	// Exchange Authentication Type
#define AUTH_USE_NONE          0x0000 
#define AUTH_USE_ORG_ID        0x0001     /* org-id鉴权方式 */
#define AUTH_USE_OAUTH         0x0002     /* OAuth鉴权方式 */
#define AUTH_USE_NTLM          0x0004     /* NTLM鉴权方式 */
#define AUTH_USE_DEV_PAIRING   0x0008     /* web sign in方式 */
#define AUTH_USE_BASIC         0x0010     /* Basic鉴权方式 */
#define AUTH_USE_CACHE         0x0020     /* 数据缓存方式 */
#define AUTH_USE_CERT          0x0040     /* 证书缓存方式 */
#define AUTH_USE_ALL_METHOD    0xFFFF     /* 所有支持的鉴权方式 */
	int iSipExchangeAuthType;

	// download exchange type
	int iDlExchangeAuthType;

	//////////end get from sip//////////

	// UC Policy(features) 配置开关
	char simulRing;						// Simultaneous ringing
	char callFwd;							// Call forwarding
	char callPark;						// 
	char callTrans;						// Call transfer
	char delegation;						// Delegation
	char teamCall;						// Teamcall

	// data number
	int iCalendarNum;					// calendar number
	int iContactNum;					// contact number
	int iCalllogNum;					// calllog number
	int iVoiceMailNum;					// visual voicemail number

};

#define PAIRING_INFO_SIZE 256
struct DevicePairingData
{
	char szPincode[PAIRING_INFO_SIZE];	// 成功时的code
	char szUrl[PAIRING_INFO_SIZE];	// 成功时的URL
	int WebSignInType;		// web sign in类型（区分normal和CAP）
};

#endif // _DSK_MSG_ID_DEFINE_H_
