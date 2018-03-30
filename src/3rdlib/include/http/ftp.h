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

#define FTP_SUCC            0   //�����ɹ��ɹ�


#define FTP_FAIL            0x80    //����ʧ��

#define FTP_ERR_DESC_SIZE   50       //FTP�������Ӧ�Ĵ��������ַ�����С

#define FTP_HEAD_SIZE 512  //ftp ���ص��ļ�ͷ��С

/*���ܣ���װftp����
*���������s1��������
				 s2���������
				 stream��ָ�����socket ��FILEָ��
				 
*���������buf�����������ص��������ַ���
				 
*���ؽ�������ֱ�ʾ�������550 File unavailable. Overwrite 
				not allowed by user profile�е�550
*/
int ftpcmd(const char *s1, const char *s2, FILE *stream, char *buf);

/*���ܣ�ͨ��ftp ��ȡҪ���ص��ļ���ͷ����ǰFTP_HEAD_SIZE ���ֽ�
*��������� host��ftp��������ip��ַ
				  port��ftp�������Ķ˿�
				  user��ftp�������ĵ�¼�û���
				  password����¼����
				  server_path��Ҫ���ص��ļ�
				  stTimeOUt, ��ʱ����	
				   unMaxFileSize, �����ص�����ֽ���
						
*���������caFtpHead�����ص��ļ�ͷ�Ļ���
				 pnHeadSize��ʵ�����ص��ļ�ͷ��С
				 pnFdData������socket
				 pnFileSize��Ҫ���ص��ļ���С
				 pstAutopErr��������Ϣ
				 
*���ؽ����ָ�������·socket ��FILEָ��
*/
FILE* ftp_recv_head(const char *host, int port, const char *user, const char *password,
				char *server_path, TimeOut_st stTimeOUt, unsigned int unMaxFileSize, 
				char caFtpHead[FTP_HEAD_SIZE], int *pnHeadSize,int *pnFdData, 
				int *pnFileSize, Err_st *pstAutopErr);

/*���ܣ�ͨ��ftp ��ȡҪ���ص��ļ��������ص��ļ�ͷ��ʣ�ಿ�֣�Ȼ��浽
			�ڴ棬�ú�����ftp_recv_head���������
*���������nFdData������socket
				 control_stream��ָ�������·socket ��FILEָ��
				 caFtpHead�����ص��ļ�ͷ�Ļ���
				 nHeadSize��ʵ�����ص��ļ�ͷ��С
				 pcRam������������ݵ��ڴ滺��	
				 stTimeOUt, ��ʱ����
				  
*���������pstAutopErr��������Ϣ
				 
*���ؽ�����ܹ����յ������ֽڵ�����
*/
int ftp_recv_to_ram(int nFdData, FILE *control_stream,
						char caFtpHead[FTP_HEAD_SIZE], int nHeadSize,
						char *pcRam, TimeOut_st stTimeOUt, 
						Err_st *pstAutopErr);


/*���ܣ�ͨ��ftp �����ļ�,Ȼ�󱣴浽����
*��������� pcSerName��ftp��������ip��ַ
				  nSerPort��ftp�������Ķ˿�
				  pcUser�� �û���
				  pcPswd, �û�����
				  pcRemoteName��Ҫ���ص��ļ�
				  pcLocalName�� ���ش洢�ļ�������ΪNULL��
				  			���pcRemoteName һ��
				  stTimeOUt, ��ʱ����
				  unMaxFileSize, �����ص�����ֽ���	
						
*��������� pstAutopErr��������Ϣ
				 
*���ؽ���� SUCC:�ɹ�
				  FAIL: ʧ��
*/
int ftp_receive(char *pcSerName, int nSerPort, char *pcUser, char *pcPswd, 
				char *pcRemoteName, char *pcLocalName, TimeOut_st stTimeOUt, 
				unsigned int unMaxFileSize, Err_st	 *pstAutopErr);

#ifdef    __cplusplus
}
#endif    /* end of __cplusplus */

#endif /*FTP_H_*/
