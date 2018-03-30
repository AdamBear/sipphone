#ifndef ERROR_H_
#define ERROR_H_

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef SUCC
#define SUCC 0
#endif

#ifndef FAIL
#define FAIL 0xff
#endif

#define ASSERT(x)                                                       \
{                                                                       \
    if (0 == (x))                                                           \
    {                                                                   \
        ERR("NULL \n");                               \
    }                                                                   \
}

#define ERR_NO_SPECIFIC		  0x3FFF   //HTTP出错
#define ERR_XML_FORMAT 0X3FFE  //XML文件错
#define ERR_MALLOC 0X3FFd  //内存分配出错
#define ERR_HTTP		  0x3FFC   //HTTP出错

#define MAX_ERROR_DESC 200   //错误描述缓冲

#define AUTOP_ERR_DESC_SIZE 100  //错误描述的最大字节数
typedef struct
{        
	  int nErrorCode;  //错误码
	  char caErrDesc[AUTOP_ERR_DESC_SIZE]; //错误描述
}Err_st;

/*因为该文件被应用程序引用，未加宏HTTP_ADD_TIMEOUT*/
typedef struct
{        
	  int nDnsParse;  //域名解析超时时间，为0时用系统默认超时时间
	  int nConnect; //与服务器建立连接的超时时间，为0时用系统默认超时时间
	  int nRecv; //从服务器接收数据的超时时间
}TimeOut_st;

#define AP_DEBUG_OFF		0
#define AP_DEBUG_ERROR		1
#define AP_DEBUG_WARN		2
#define AP_DEBUG_TRACE		3
#define AP_DEBUG_INFO		4
#define AP_DEBUG_LOUD		5


#ifdef _DEBUG
extern int	APDebugLevel;

#ifdef WIN32
#if 1//pjb test
#define WARN printf

#define TRACE printf

#define INFO printf

#define ERR printf
#else
#define DBGPRINT(Level, fmt,...)		 \
{											\
	if (Level <= APDebugLevel)				\
	{										\
	printf("%s(%d): "fmt ,  __FILE__ , __LINE__ , __VA_ARGS__);				\
		fflush(stdout); \
	}										\
}

#define ERR(fmt,...)	 \
{											\
	if (AP_DEBUG_ERROR <= APDebugLevel)				\
	{										\
		printf("\033[1;31m%s(%d): "fmt"\033[0m",  __FILE__, __LINE__ , __VA_ARGS__);				\
		fflush(stdout); \
	}										\
}

#define WARN(fmt, ...)	DBGPRINT(AP_DEBUG_WARN, fmt, __VA_ARGS__)

#define TRACE(fmt, ...)	DBGPRINT(AP_DEBUG_TRACE, fmt, __VA_ARGS__)

#define INFO(fmt, ...)	DBGPRINT(AP_DEBUG_INFO, fmt, __VA_ARGS__)	
#endif
#else /*LINUX*/
#if HTTP_SYSLOG
#include <syslog.h>
#define DBGPRINT(Level, fmt, args...)		 \
{											\
	if (Level <= APDebugLevel)				\
	{										\
		syslog(LOG_DEBUG, "%s(%d): "fmt ,  __FILE__, __LINE__ , ## args);		\
	}										\
}

#define ERR(fmt, args...)	 \
{											\
	if (AP_DEBUG_ERROR <= APDebugLevel)				\
	{										\
		syslog(LOG_DEBUG, "%s(%d): "fmt,  __FILE__, __LINE__ , ## args);				\
	}										\
}

#else
#define DBGPRINT(Level, fmt , args...)		 \
{											\
	if (Level <= APDebugLevel)				\
	{										\
		printf("%s(%d): "fmt ,  __FILE__, __LINE__ , ## args);				\
		fflush(stdout); \
	}										\
}

#define ERR(fmt, args...)	 \
{											\
	if (AP_DEBUG_ERROR <= APDebugLevel)				\
	{										\
		printf("\033[1;31m%s(%d): "fmt"\033[0m",  __FILE__, __LINE__ , ## args);				\
		fflush(stdout); \
	}										\
}
#endif

#define WARN(fmt, args...)	DBGPRINT(AP_DEBUG_WARN, fmt, ## args)

#define TRACE(fmt, args...)	DBGPRINT(AP_DEBUG_TRACE, fmt, ## args)

#define INFO(fmt, args...)	DBGPRINT(AP_DEBUG_INFO, fmt, ## args)	
#endif

#else
#define DBGPRINT(Level, fmt, args...)
#define ERR(fmt, args...)
#define WARN(fmt, args...)
#define TRACE(fmt, args...)
#define INFO(fmt, args...)
#endif

#define AUTOP_SUCC            0x0000   //操作成功成功

#define ERR_AUTOP_HTTP		  0x01FF   //HTTP出错

#define ERR_AUTOP_FTP		  0x02FF   //FTP出错
#define ERR_AUTOP_TFTP		  0x03FF   //TFTP出错

#define ERR_AUTOP_CFG         0x10FF   //解析配置文件出错
#define ERR_AUTOP_FW          0x11FF   //固件升级错误

#define AUTOP_FAIL            0xFFFF    //操作失败

//ES: error string
#define ES_OOM   "out of memory"
#define ES_CREATE_SOCKET "fail to create socket"
#define ES_CONNECT "fail to connect"
#define ES_TIMEOUT "timeout=%d number of timeout=%d"
#define ES_SEND  "error:send"
#define ES_SELECT "error:select"
#define ES_DATA   "no desired DATA frame, opcode=%d, errcode=%d"
#define ES_ACK   "no desired ACK frame, opcode=%d, errcode=%d, desc:%s"
#define ES_OPEN   "can not open file %s"
#define ES_WRITE  "can not write data to local file"
#define ES_READ   "can not read data from local file %s"
#define ES_STATUS "can not get status of local file %s"
#define ES_GETHOSTBYNAME "gethostbyname error,host:%s"
#define ES_GETADDRINFO   "getaddrinfo error, host:%s"
#define ES_HOST   "host address %s is error"
#define ES_PORT   "invalid port number:%s"
#define ES_FDOPEN "error to open control stream"
#define ES_FTP_SEND "error while sending data to ftp server!"
#define ES_URL      "invalid url (must start with 'http://')"
#define ES_HTTP_PORT      "invalid port in url"
#define ES_HTTP_NULL      "Null data pointer"
#define ES_HTTP_QUERY      "error while querying http server, error code=%d"
#define ES_HTTP_READ       "error while receiving data from http server"
#define ES_HTTP_LEN       " No/Bad length in header"

#define ES_UPD_NO_NEED "partion %d does not need update"
#define ES_UPD_LEN     "size of buffer is less than the desired"
#define ES_UPD_PROTOCOL     "protocol of packet is error"
#define ES_UPD_INVALID     "invalid packet"
#define ES_UPD_NO_FW       "no firmware section"
#define ES_UPD_HW_VER     "hardware version is error"
#define ES_UPD_SER_TYPE   "configuration server type must be tftp,ftp or http"

void autop_err(int nErrCode, char *pcErrDesc, Err_st *pstErr);

#ifdef  __cplusplus
}
#endif

#endif

