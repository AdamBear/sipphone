/******************************************************************************************************
Copyright (C) 2008 ,Yealink Network Technology Co.,Ltd. 

File Name:tftp.h
Author:xuyx                 
Version: 1.0              
Date:2008.06.27

History:

Abstract:
	
*******************************************************************************************************/
#ifndef WIN32//暂不支持WINDOWS平台
#if (!M_VIDEOPHONE)//VIDEOPHONE平台下ftp使用的是ETL中的。

#ifndef __TFTP_H
#define	__TFTP_H 1

#include "error.h"


#ifdef    __cplusplus
extern "C" {
#endif

/*
 *custom declaration
 */

typedef unsigned int       DWORD;

typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
/*
 * Trivial File Transfer Protocol (IEN-133)
 */
#define	SEGSIZE	512 		/* data segment size */
#define PKTSIZE SEGSIZE+4   /* full packet size */

/*
 * Packet types.
 */
#define	RRQ	1				/* read request */
#define	WRQ	2				/* write request */
#define	DATA 3			    /* data packet */
#define	ACK	4				/* acknowledgement */
#define	ERROR 5 			/* error code */

struct	tftphdr 
{
	short	th_opcode;			/* packet type */
	union 
	{
		unsigned short	tu_block;	/* block # */
		short	tu_code;		/* error code */
		char	tu_stuff[1];		/* request packet stuff */
	} th_u;
	char	th_data[1];			/* data or error string */
#ifndef WIN32
};
#else
} __attribute__((packed));
#endif

#define	th_block	th_u.tu_block
#define	th_code		th_u.tu_code
#define	th_stuff	th_u.tu_stuff
#define	th_msg		th_data

/*
 * Error codes.
 */
#define	EUNDEF		0		/* not defined */
#define	ENOTFOUND	1		/* file not found */
#define	EACCESS		2		/* access violation */
#define	ENOSPACE	3		/* disk full or allocation exceeded */
#define	EBADOP		4		/* illegal TFTP operation */
#define	EBADID		5		/* unknown transfer ID */
#define	EEXISTS		6		/* file already exists */
#define	ENOUSER		7		/* no such user */



/*功能：设置tftp 的超时时间和超时次数
*输入参数： nTimeOut，超时时间
				  nNumberTimeOut，超时次数		
						
*输出参数：
				 
*返回结果：
*/
int tftp_set_timeout(int nTimeOut, int nNumberTimeOut);

/*功能：通过tftp 获取要下载的文件的头，即前SEGSIZE 个字节
*输入参数： pcSerName，tftp服务器的ip地址
				  pcSerPort，tftp服务器的端口
				  pcRemoteName，要下载的文件
				  stTimeOUt, 超时设置	
						
*输出参数：caHeadBuf，下载的文件头的缓冲
				 adresse_serveur，tftp服务器sockaddr_in 地址
				 pnPeer，tftp服务器的socket 句柄
				 pstAutopErr，错误信息
				 
*返回结果：实际接收到的文件头字节数
*/
int tftp_recv_head(char *pcSerName, char *pcSerPort, char *pcRemoteName, TimeOut_st stTimeOUt,
	char caHeadBuf[SEGSIZE], struct sockaddr_in *adresse_serveur, int *pnPeer, Err_st *pstAutopErr);

/*功能：通过tftp 获取要下载的文件,调用此函数前需先调用tftp_recv_head获取
			要下载的文件头
*输入参数： pcLocalName，文件下载后存到本地的文件名
				  caHeadBuf，已下载的文件头缓冲
				  nHeadBufSize，已下载的文件头大小
				  to1，tftp服务器sockaddr_in 地址
				  peer，tftp服务器的socket 句柄
				  stTimeOUt, 超时设置	
				  unMaxFileSize, 可下载的最大字节数
				  
*输出参数：pstAutopErr，错误信息
				 
*返回结果：SUCC:成功
				 FAIL: 失败
*/
int tftp_recv_to_file(char *pcLocalName, char caHeadBuf[SEGSIZE], int nHeadBufSize, 
						struct sockaddr_in *to1, int peer, TimeOut_st stTimeOUt, 
						unsigned int unMaxFileSize, Err_st *pstAutopErr);

/*功能：通过tftp 获取要下载的文件,保存到内存中,调用
			此函数前需先调用tftp_recv_head获取	要下载的文
			件头
*输入参数： caHeadBuf，已下载的文件头缓冲
				    nHeadBufSize，已下载的文件头大小
				    to1，tftp服务器sockaddr_in 地址
				    peer，tftp服务器的socket 句柄
				    stTimeOUt, 超时设置	
				    unMaxFileSize, 可下载的最大字节数
				  
*输出参数：ppcRam，下载数据存放的缓冲
				    pnFileSize ，下载数据的大小
				    pstAutopErr，错误信息
				 
*返回结果：SUCC:成功
				 FAIL: 失败
*/
int tftp_recv_to_ram(char **ppcRam, int *pnFileSize, char caHeadBuf[SEGSIZE],
						int nHeadBufSize, struct sockaddr_in *to1, int peer, 
						TimeOut_st stTimeOUt, unsigned int unMaxFileSize, 
						Err_st *pstAutopErr);

/*功能：通过tftp 下载文件
*输入参数： pcSerName，tftp服务器的ip地址
				  pcSerPort，tftp服务器的端口
				  pcRemoteName，要下载的文件
				  pcLocalName， 本地存储文件名，若为NULL，
				  			则和pcRemoteName 一样
				  stTimeOUt, 超时设置
				  unMaxFileSize, 可下载的最大字节数
				  
*输出参数： pstAutopErr，错误信息
				 
*返回结果： SUCC:成功
				  FAIL: 失败
*/
int tftp_receive(char *pcSerName, char *pcSerPort, char *pcRemoteName, char *pcLocalName, 
					TimeOut_st stTimeOUt, unsigned int unMaxFileSize, Err_st *pstAutopErr);


/*功能：客户端发往服务器的错误消息
*输入参数：peer，tftp服务器的socket 句柄	
				  to，tftp服务器sockaddr_in 地址
				  error， 错误码
				  commentaire， 错误描述
						
*输出参数：无
				 
*返回结果：无
*/
void tftp_send_error(int peer,struct sockaddr_in *to,int error,char *commentaire);


#ifdef  __cplusplus
}
#endif

#endif //tftp.h
#endif//#if(!M_VIDEOPHONE)
#endif//#ifndef WIN32

