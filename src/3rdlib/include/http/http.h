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
/*��lync��getʱ����Ҫ��body,���ԼӴ��ͻ�����*/
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
	int bHttps;       /*http �� https ���*/
	char caMethod[10];/*http or https*/
	char *pcHttpServer;
	unsigned short nHttpPort;
	char *pcUsrName;
	char *pcPswd;
	char *pcFileName;
	char *pcUrl;
	char *pcFullUrl;
	char *pcIpAddr;/*pcHttpServer�п���Ϊ����,����������IP��ַ*/
    char *pcAuthStr;/*�ʺ�������ܺ��ժҪֵ*/
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




//��������T2x
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

/*���ܣ�ͨ��http ��ȡҪ���ص��ļ���ͷ����ǰHTTP_HEAD_SIZE ���ֽ�
*��������� pcSerUrl��http��������url, http://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
				     stProxy�����������	
				    stTimeOUt, ��ʱ����
				    unMaxFileSize, �����ص�����ֽ���						
						
*���������caFtpHead�����ص��ļ�ͷ�Ļ���
				 pnHeadSize��ʵ�����ص��ļ�ͷ��С
				 pnFdData������socket
				 pnFileSize��Ҫ���ص��ļ���С
				 pstAutopErr��������Ϣ
				 
*���ؽ����http ������
*/
int http_recv_head(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
						unsigned int unMaxFileSize, char caFtpHead[HTTP_HEAD_SIZE], 
						int *pnHeadSize, int *pnFdData,int *pnFileSize, Err_st *pstErr);

/*���ܣ�ͨ��http ��ȡҪ���ص��ļ��������ص��ļ�ͷ��ʣ�ಿ�֣�Ȼ��浽
			�ڴ棬�ú�����http_recv_head���������
*���������nFdData������socket
				 caFtpHead�����ص��ļ�ͷ�Ļ���
				 nHeadSize��ʵ�����ص��ļ�ͷ��С
				 nFileSize��Ҫ�����ļ����ܴ�С
				 pcRam������������ݵ��ڴ滺��				        	
				 stTimeOUt, ��ʱ����
				    
*���������pstAutopErr��������Ϣ
				 
*���ؽ�����ܹ����յ������ֽڵ�����
*/
int http_remain_to_ram(int nFdData, char caFtpHead[HTTP_HEAD_SIZE], int nHeadSize,
					   int nFileSize, char *pcRam, TimeOut_st stTimeOUt, Err_st *pstErr);

/*���ܣ�ͨ��http �����ļ�
*��������� pcSerUrl��http��������url, http://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
				     stProxy�����������	
				     stTimeOUt, ��ʱ����
				     unMaxFileSize, �����ص�����ֽ���	
				     pcLocalName�� ���ش洢�ļ�������ΪNULL��
				  			��ȡpcSerUrl �е��ļ���
			           
						
*��������� pstAutopErr��������Ϣ
				 
*���ؽ���� SUCC:�ɹ�
				  FAIL: ʧ��
*/
int http_recv_to_file(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
						unsigned int unMaxFileSize, char *pcLocalName, Err_st *pstErr);

/*���ܣ�ͨ��http ��ȡҪ���ص��ļ���������ڴ���
*���������pcSerUrl��http��������url, http://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
                             stProxy�����������	
				    stTimeOUt, ��ʱ����
				    unMaxFileSize, �����ص�����ֽ���	

				  
*���������ppcRam���������ݴ�ŵĻ���
				  pnFileSize ���������ݵĴ�С
				  pstAutopErr��������Ϣ
				  
*���ؽ����http ������
*ע��: ʹ�ú����ͷŶ�̬����Ļ���ppcRam !
*/
int http_recv_to_ram(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
							unsigned int unMaxFileSize, char **ppcRam, int *pnFileSize, 
							Err_st *pstErr);


/*���ܣ�			ͨ��http ��ȡҪ���ص��ļ���������Ӧsocket������������
*���������			pcSerUrl��http��������url, https://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
				    stProxy�����������	
				    stTimeOUt, ��ʱ����
				    unMaxFileSize, �������ص�����ֽ���	
				     
*���������			pnFdData,	socket�׽ӿ�
					pnFileSize ��Ҫ�������ݵĴ�С(http��body��С)
					pstAutopErr��������Ϣ
				 
*���ؽ����http ������
*/
int http_open_file(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt,unsigned int unMaxFileSize, 
						int *pnFdData, int *pnFileSize, Err_st *pstErr);

/*���ܣ�ͨ��http��ָ����socket�ж�ȡһ�����ȵ����ݣ���pcRam����(pcRam��ҪӦ�ó��������ڴ�)
*���������			ssl,ctx,nFdData��  ssl��socket�����Ϣ
				    stTimeOUt, ��ʱ����
				    pcRam:�������ȥ�����ڽ�������(Ӧ�ó������)
*���������		  
					pcRam����ȡ�������ݡ�
					pstAutopErr��������Ϣ
				 
*���ؽ����			���յ������ݳ���

ע�⣺socket���йرգ���ҪӦ�ó����Լ����رա�
*/
int http_read_file(int nFdData,int nDesiredSize, char *pcRam, TimeOut_st stTimeOUt, Err_st *pstErr);


/*���ܣ��ر�http��socket�׽ӿ�
*���������			nSocket��Ҫ�رյ��׽ӿ�
*���������		  

				 
*���ؽ�����Ƿ�رճɹ���0:�ɹ�,-1ʧ��
*/
int http_close_file(int nSocket);





/*���ܣ�ͨ��URL��INFO, ����������Ҫ���ŵ��ļ�, �������ص�����
*��������� pUrl, HTTP URL��ַ, ���û��URL, ��ֱ���ж�info			pInfo , Info�ĵ�ַ
*�������: 	pPath, 
		�������������, �򷵻����ص�·��(����·��), �����ַ�ָ��Ϳ���, �����ⲿ�ͷ�.	pIndex, ���û�����ص�����(*
		pPath=NULL), ����������ص���������INDEX
*���ؽ����0, ��ʾʹ��Ĭ������(û��INDEX��pPath); 
		1,��ʾ�Ѿ�ƥ�䵽����������
*/
int distinctive_ringtone_download(char *pUrl, char *pInfo, int *pIndex, char *pPath);

/*���ܣ�ͨ��http �����ļ�
*��������� pcSerUrl ��������Ϣ:
							�û���:	xyx
							���� :	123456
							�ļ�URL:	http://192.168.0.139:5566/digest/000000000000.cfg
				     stProxy�����������
				     stTimeOUt, ��ʱ����
				     unMaxFileSize, �����ص�����ֽ���	
				     pcLocalName�� ���ش洢�ļ�������ΪNULL��
				  			��ȡpcSerUrl �е��ļ���
						
*��������� pstAutopErr��������Ϣ
				 
*���ؽ���� succ:�ɹ�
				  fail: ʧ��
*/

#if M_VIDEOPHONE
#else
int http_recv_to_file_2(FILE_INFO_ST *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
						unsigned int unMaxFileSize, char *pcLocalName, Err_st *pstErr);


int http_recv_head_2(FILE_INFO_ST *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt,
					 unsigned int unMaxFileSize, char caFtpHead[HTTP_HEAD_SIZE], 
					 int *pnHeadSize, int *pnFdData, int *pnFileSize, Err_st *pstErr);

/*���ܣ�ͨ��http ��ȡҪ���ص��ļ���������ڴ���*/
int http_recv_to_ram_2(FILE_INFO_ST *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
							unsigned int unMaxFileSize, char **ppcRam, int *pnFileSize, 
							Err_st *pstErr);

/*#if  HTTP_REDIRECT_FUN */
/*
*rps����url����,���ڼ��url�Ƿ����
*����:		pcSrcUrl:Ҫ���ӵ�URL.
*�����:	pcNewUrl:��ȡ��õ����µ�URL.
*
*/
int http_or_s_rediect_fun(char *pcSrcUrl, URL_INFO_ST *pcNewUrl,TimeOut_st *stTimeOUt, 
							   unsigned int unMaxFileSize, char *pcLocalName);
/*#endif*/

#endif 

int http_post(char *url, int timeout, char *method, char *header, char *body, char **ram, Err_st *err);

/*
*����:�ж��ǲ���ʹ�þ�̬ipv6��ַ��
*/
int http_is_ipv6_enable();

#if DOWNLOAD_LIB_VERSION
void get_download_lib_version(void);
#endif

#ifdef  __cplusplus
}
#endif

#endif /*HTTP_H_*/

