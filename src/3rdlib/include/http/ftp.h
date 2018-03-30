/******************************************************************************************************
Copyright (C) 2008 ,Yealink Network Technology Co.,Ltd. 

File Name:ftp.h
Author:xuyx                 
Version: 1.0              
Date:2008.06.27

History:

Abstract:
	
*******************************************************************************************************/

#ifndef FTP_H_
#define FTP_H_

#ifdef    __cplusplus
extern "C" {
#endif

#include "error.h"

#define FTP_SUCC            0   //操作成功成功


#define FTP_FAIL            0x80    //操作失败

#define FTP_ERR_DESC_SIZE   50       //FTP错误码对应的错误描述字符串大小

#define FTP_HEAD_SIZE 512  //ftp 下载的文件头大小

/*功能：封装ftp命令
*输入参数：s1，命令字
				 s2，命令参数
				 stream，指向控制socket 的FILE指针
				 
*输出参数：buf，服务器返回的命令结果字符串
				 
*返回结果：数字表示结果，如550 File unavailable. Overwrite 
				not allowed by user profile中的550
*/
int ftpcmd(const char *s1, const char *s2, FILE *stream, char *buf);

/*功能：通过ftp 获取要下载的文件的头，即前FTP_HEAD_SIZE 个字节
*输入参数： host，ftp服务器的ip地址
				  port，ftp服务器的端口
				  user，ftp服务器的登录用户名
				  password，登录密码
				  server_path，要下载的文件
				  stTimeOUt, 超时设置	
				   unMaxFileSize, 可下载的最大字节数
						
*输出参数：caFtpHead，下载的文件头的缓冲
				 pnHeadSize，实际下载的文件头大小
				 pnFdData，数据socket
				 pnFileSize，要下载的文件大小
				 pstAutopErr，错误信息
				 
*返回结果：指向控制链路socket 的FILE指针
*/
FILE* ftp_recv_head(const char *host, int port, const char *user, const char *password,
				char *server_path, TimeOut_st stTimeOUt, unsigned int unMaxFileSize, 
				char caFtpHead[FTP_HEAD_SIZE], int *pnHeadSize,int *pnFdData, 
				int *pnFileSize, Err_st *pstAutopErr);

/*功能：通过ftp 获取要下载的文件除已下载的文件头的剩余部分，然后存到
			内存，该函数在ftp_recv_head函数后调用
*输入参数：nFdData，数据socket
				 control_stream，指向控制链路socket 的FILE指针
				 caFtpHead，下载的文件头的缓冲
				 nHeadSize，实际下载的文件头大小
				 pcRam，存放下载数据的内存缓冲	
				 stTimeOUt, 超时设置
				  
*输出参数：pstAutopErr，错误信息
				 
*返回结果：总共接收到多少字节的数据
*/
int ftp_recv_to_ram(int nFdData, FILE *control_stream,
						char caFtpHead[FTP_HEAD_SIZE], int nHeadSize,
						char *pcRam, TimeOut_st stTimeOUt, 
						Err_st *pstAutopErr);


/*功能：通过ftp 下载文件,然后保存到本地
*输入参数： pcSerName，ftp服务器的ip地址
				  nSerPort，ftp服务器的端口
				  pcUser， 用户名
				  pcPswd, 用户密码
				  pcRemoteName，要下载的文件
				  pcLocalName， 本地存储文件名，若为NULL，
				  			则和pcRemoteName 一样
				  stTimeOUt, 超时设置
				  unMaxFileSize, 可下载的最大字节数	
						
*输出参数： pstAutopErr，错误信息
				 
*返回结果： SUCC:成功
				  FAIL: 失败
*/
int ftp_receive(char *pcSerName, int nSerPort, char *pcUser, char *pcPswd, 
				char *pcRemoteName, char *pcLocalName, TimeOut_st stTimeOUt, 
				unsigned int unMaxFileSize, Err_st	 *pstAutopErr);

#ifdef    __cplusplus
}
#endif    /* end of __cplusplus */

#endif /*FTP_H_*/
