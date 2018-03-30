#ifndef __DISTINCTIVE_RINGER_H__
#define __DISTINCTIVE_RINGER_H__

#include <stdio.h>
#include "../ETLLib/ETLLib.h"
#include <stdlib.h>
#include <http.h>

#define DR_TRACE	printf
#define DR_MAX_RING_FILE_PERMIT		10		//最大允许的缓冲区的位置
#define DR_CACHE_BUFFER_SIZE		200*1024		//最大允许的缓冲区的长度， 200 KB
#define DR_URL_LEN					100		//最大允许的URL的长度
#define DR_BUFFER_LEN				2048

#ifndef WIN32
	#define DR_CACHE_BUFFER_PATH		"/tmp/"    //缓冲区的位置
	#if M_VIDEOPHONE
		#define DR_ALERT_INFO_CFG_FILE		"/rundata/config/user/user.ini"
	#elif M_T3X
		#define DR_ALERT_INFO_CFG_FILE		"/phone/config/user.ini"
	#elif M_T6X
		#define DR_ALERT_INFO_CFG_FILE		"/phone/config/user.ini"
	#else 
	    #define DR_ALERT_INFO_CFG_FILE      "/yealink/config/Setting/Setting.cfg"
	#endif	
#else
	#define DR_ALERT_INFO_CFG_FILE		"test.ini"
	#define DR_CACHE_BUFFER_PATH		"c:\\"    //缓冲区的位置
#endif

#define DR_SESSION_ALERT_INFO		"AlertInfo"
#define DR_KEY_Text					"Text"
#define DR_KEY_RINGER				"Ringer"

enum DR_ERROR_CODE
{
	DR_DOWNLOAD_SUCCESS = 0,
	DR_ERR_FILE_SIZE_EXCEEDED = -30,
	DR_ERR_INVALID_FILE_FORMAT,
	DR_ERR_WRITE_LOCAL_FILE_FAIL,
	DR_ERR_NO_DOWNLOAD,
	DR_ERR_DOWNLOAD_FAIL,
};

#ifdef WIN32
#define  strcasecmp		stricmp
#define  strncasecmp	strnicmp
#include <assert.h>
#endif
#endif
