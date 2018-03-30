/******************************************************************************************************
Copyright (C) 2008 ,Yealink Network Technology Co.,Ltd. 

File Name:http.h
Author:xuyx                 
Version: 1.0              
Date:2008.07.09

History:

Abstract:
	
*******************************************************************************************************/

#ifndef HTTP_H_
#define HTTP_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "module.h"


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Winsock2.h>
#include <windows.h>
#pragma comment( lib, "Iphlpapi.lib" )
#pragma comment( lib, "Ws2_32.lib" )
//#define  strncasecmp	strnicmp
//#define  strcasecmp	stricmp
#define  socket_write(a,b,c)		send(a,b,c,0)
#define  socket_read(a,b,c)			recv(a,b,c,0)
#define  socket_close(socket)		closesocket(socket);socket = -1;
#else
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/stat.h>
#define socket_write	write
#define  socket_read	read
#define  socket_close(socket)   close(socket);socket = -1;
#endif

#include "error.h"

#ifdef  __cplusplus
extern "C" {
#endif

/* return type */
typedef enum {

  /* Client side errors */
  ERRHOST=-1, /* No such host */
  ERRSOCK=-2, /* Can't create socket */
  ERRCONN=-3, /* Can't connect to host */
  ERRWRHD=-4, /* Write error on socket while writing header */
  ERRWRDT=-5, /* Write error on socket while writing data */
  ERRRDHD=-6, /* Read error on socket while reading result */
  ERRPAHD=-7, /* Invalid answer from data server */
  ERRNULL=-8, /* Null data pointer */
  ERRNOLG=-9, /* No/Bad length in header */
  ERRMEM=-10, /* Can't allocate memory */
  ERRRDDT=-11,/* Read error while reading data */
  ERRURLH=-12,/* Invalid url - must start with 'http://' */
  ERRURLP=-13,/* Invalid port in url */
  ERRAUTH=-14,/* Invalid authentication*/
  ERRCOUNT=-15,/*error*/
  ERR3xxLOOP=-16,/*3xx loop*/
  /* Return code by the server */
  ERR400=400, /* Invalid query */
  ERR403=403, /* Forbidden */
  ERR408=408, /* Request timeout */
  ERR500=500, /* Server error */
  ERR501=501, /* Not implemented */
  ERR503=503, /* Service overloaded */

  /* Succesful results */
  OK_0 = 0,   /* successfull parse */
  OK_201=201, /* Ressource succesfully created */
  OK_200=200  /* Ressource succesfully read */

} http_retcode;

typedef enum 
{
  CLOSE,  /* Close the socket after the query (for put) */
  KEEP_OPEN /* Keep it open */
} querymode;

/* beware that filename+type+rest of header must not exceed MAXBUF */
/* so we limit filename to 256 and type to 64 chars in put & get */

//#define MAXBUF 512   

#if M_NTLM_AUTH
/*在lync中get时候需要带body,所以加大发送缓冲区*/
#define MAXBUF 4096   //HTTP_INC_URL_LEN
#else
#define MAXBUF 2048   //HTTP_INC_URL_LEN
#endif

#define MAX_URL	1024

#define	HTTP_HEAD_SIZE	512 		/* http head size */

typedef struct
{
    int iGetCount;
    int iRev2xxCount;
    int iRev3xxCount;
    int iRev4xxCount;
}COUNT_INFO_ST;



#define 	RECV_ZERO  -1
#define		RECV_ALL	0

#define 	HTTP		0
#define		HTTPS		1

#define		ON_SOCKET	1
#define		OFF_SOCKET	0


typedef struct
{
	int bHttps;       /*http 与 https 标记*/
	char caMethod[10];/*http or https*/
	char *pcHttpServer;
	unsigned short nHttpPort;
	char *pcUsrName;
	char *pcPswd;
	char *pcFileName;
	char *pcUrl;
	char *pcFullUrl;
	char *pcIpAddr;/*pcHttpServer有可能为域名,保存解析后的IP地址*/
    char *pcAuthStr;/*帐号密码加密后的摘要值*/
#if M_NTLM_AUTH
	char *pcMethod;
	char *pcHeader;
	char *pcBody;
#endif
}URL_INFO_ST;

typedef struct
{
	char *pcProxyServer;
	unsigned short nProxyPort;
}PROXY_ST;

typedef struct
{
	char *pcUsrName;
	char *pcPswd;
	char *pcFileUrl;
}FILE_INFO_ST;




//仅仅适用T2x
//#if (M_T2X || M_T6X)
#define DOWNLOAD_MAX_FIRMWARESIZE 8*1024*1024
//#elif (M_T3X || M_VIDEOPHONE)
//#define DOWNLOAD_MAX_FIRMWARESIZE 60*1024*1024
//#endif

#define VP_MAX_DOWN_LOAD			60*1024*1024		/*60M*/

/* parses an url : setting the http_server and http_port global variables
 * and returning the filename to pass to http_get/put/...
 * returns a negative error code or 0 if sucessfully parsed.
 */
http_retcode http_parse_url_1_2_n(char *pcSerUrl,URL_INFO_ST *stUrlInfo, Err_st *pstErr);

/*功能：通过http 获取要下载的文件的头，即前HTTP_HEAD_SIZE 个字节
*输入参数： pcSerUrl，http服务器的url, http://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
				     stProxy，代理服务器	
				    stTimeOUt, 超时设置
				    unMaxFileSize, 可下载的最大字节数						
						
*输出参数：caFtpHead，下载的文件头的缓冲
				 pnHeadSize，实际下载的文件头大小
				 pnFdData，数据socket
				 pnFileSize，要下载的文件大小
				 pstAutopErr，错误信息
				 
*返回结果：http 错误码
*/
int http_recv_head(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
						unsigned int unMaxFileSize, char caFtpHead[HTTP_HEAD_SIZE], 
						int *pnHeadSize, int *pnFdData,int *pnFileSize, Err_st *pstErr);

/*功能：通过http 获取要下载的文件除已下载的文件头的剩余部分，然后存到
			内存，该函数在http_recv_head函数后调用
*输入参数：nFdData，数据socket
				 caFtpHead，下载的文件头的缓冲
				 nHeadSize，实际下载的文件头大小
				 nFileSize，要下载文件的总大小
				 pcRam，存放下载数据的内存缓冲				        	
				 stTimeOUt, 超时设置
				    
*输出参数：pstAutopErr，错误信息
				 
*返回结果：总共接收到多少字节的数据
*/
int http_remain_to_ram(int nFdData, char caFtpHead[HTTP_HEAD_SIZE], int nHeadSize,
					   int nFileSize, char *pcRam, TimeOut_st stTimeOUt, Err_st *pstErr);

/*功能：通过http 下载文件
*输入参数： pcSerUrl，http服务器的url, http://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
				     stProxy，代理服务器	
				     stTimeOUt, 超时设置
				     unMaxFileSize, 可下载的最大字节数	
				     pcLocalName， 本地存储文件名，若为NULL，
				  			则取pcSerUrl 中的文件名
			           
						
*输出参数： pstAutopErr，错误信息
				 
*返回结果： SUCC:成功
				  FAIL: 失败
*/
int http_recv_to_file(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
						unsigned int unMaxFileSize, char *pcLocalName, Err_st *pstErr);

/*功能：通过http 获取要下载的文件，存放在内存中
*输入参数：pcSerUrl，http服务器的url, http://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
                             stProxy，代理服务器	
				    stTimeOUt, 超时设置
				    unMaxFileSize, 可下载的最大字节数	

				  
*输出参数：ppcRam，下载数据存放的缓冲
				  pnFileSize ，下载数据的大小
				  pstAutopErr，错误信息
				  
*返回结果：http 错误码
*注意: 使用后请释放动态申请的缓冲ppcRam !
*/
int http_recv_to_ram(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
							unsigned int unMaxFileSize, char **ppcRam, int *pnFileSize, 
							Err_st *pstErr);


/*功能：			通过http 获取要下载的文件，返回相应socket，不返回数据
*输入参数：			pcSerUrl，http服务器的url, https://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
				    stProxy，代理服务器	
				    stTimeOUt, 超时设置
				    unMaxFileSize, 允许下载的最大字节数	
				     
*输出参数：			pnFdData,	socket套接口
					pnFileSize ，要下载数据的大小(http的body大小)
					pstAutopErr，错误信息
				 
*返回结果：http 错误码
*/
int http_open_file(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt,unsigned int unMaxFileSize, 
						int *pnFdData, int *pnFileSize, Err_st *pstErr);

/*功能：通过http从指定的socket中读取一定长度的数据，从pcRam返回(pcRam需要应用程序申请内存)
*输入参数：			ssl,ctx,nFdData，  ssl和socket相关信息
				    stTimeOUt, 超时设置
				    pcRam:输出缓冲去，用于接收数据(应用程序分配)
*输出参数：		  
					pcRam：读取到的数据。
					pstAutopErr，错误信息
				 
*返回结果：			接收到的数据长度

注意：socket进行关闭，需要应用程序自己来关闭。
*/
int http_read_file(int nFdData,int nDesiredSize, char *pcRam, TimeOut_st stTimeOUt, Err_st *pstErr);


/*功能：关闭http的socket套接口
*输入参数：			nSocket，要关闭的套接口
*输出参数：		  

				 
*返回结果：是否关闭成功：0:成功,-1失败
*/
int http_close_file(int nSocket);





/*功能：通过URL和INFO, 解析出本地要播放的文件, 或者下载的铃声
*输入参数： pUrl, HTTP URL地址, 如果没有URL, 则直接判断info			pInfo , Info的地址
*输出参数: 	pPath, 
		如果下载了铃声, 则返回下载的路径(完整路径), 传递字符指针就可以, 无需外部释放.	pIndex, 如果没有下载到铃声(*
		pPath=NULL), 这个参数返回的是铃声的INDEX
*返回结果：0, 表示使用默认铃声(没有INDEX和pPath); 
		1,表示已经匹配到区别振铃了
*/
int distinctive_ringtone_download(char *pUrl, char *pInfo, int *pIndex, char *pPath);

/*功能：通过http 下载文件
*输入参数： pcSerUrl 包含的信息:
							用户名:	xyx
							密码 :	123456
							文件URL:	http://192.168.0.139:5566/digest/000000000000.cfg
				     stProxy，代理服务器
				     stTimeOUt, 超时设置
				     unMaxFileSize, 可下载的最大字节数	
				     pcLocalName， 本地存储文件名，若为NULL，
				  			则取pcSerUrl 中的文件名
						
*输出参数： pstAutopErr，错误信息
				 
*返回结果： succ:成功
				  fail: 失败
*/

#if M_VIDEOPHONE
#else
int http_recv_to_file_2(FILE_INFO_ST *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
						unsigned int unMaxFileSize, char *pcLocalName, Err_st *pstErr);


int http_recv_head_2(FILE_INFO_ST *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt,
					 unsigned int unMaxFileSize, char caFtpHead[HTTP_HEAD_SIZE], 
					 int *pnHeadSize, int *pnFdData, int *pnFileSize, Err_st *pstErr);

/*功能：通过http 获取要下载的文件，存放在内存中*/
int http_recv_to_ram_2(FILE_INFO_ST *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
							unsigned int unMaxFileSize, char **ppcRam, int *pnFileSize, 
							Err_st *pstErr);

/*#if  HTTP_REDIRECT_FUN */
/*
*rps尝试url连接,用于检测url是否可用
*输入:		pcSrcUrl:要连接的URL.
*输入出:	pcNewUrl:获取后得到的新的URL.
*
*/
int http_or_s_rediect_fun(char *pcSrcUrl, URL_INFO_ST *pcNewUrl,TimeOut_st *stTimeOUt, 
							   unsigned int unMaxFileSize, char *pcLocalName);
/*#endif*/

#endif 

int http_post(char *url, int timeout, char *method, char *header, char *body, char **ram, Err_st *err);

/*
*功能:判断是不是使用静态ipv6地址。
*/
int http_is_ipv6_enable();

#if DOWNLOAD_LIB_VERSION
void get_download_lib_version(void);
#endif

#ifdef  __cplusplus
}
#endif

#endif /*HTTP_H_*/

