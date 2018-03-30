/******************************************************************************************************
Copyright (C) 2008 ,Yealink Network Technology Co.,Ltd. 

File Name:tftp.h
Author:xuyx                 
Version: 1.0              
Date:2008.06.27

History:

Abstract:
	
*******************************************************************************************************/
#ifndef WIN32//�ݲ�֧��WINDOWSƽ̨
#if (!M_VIDEOPHONE)//VIDEOPHONEƽ̨��ftpʹ�õ���ETL�еġ�

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



/*���ܣ�����tftp �ĳ�ʱʱ��ͳ�ʱ����
*��������� nTimeOut����ʱʱ��
				  nNumberTimeOut����ʱ����		
						
*���������
				 
*���ؽ����
*/
int tftp_set_timeout(int nTimeOut, int nNumberTimeOut);

/*���ܣ�ͨ��tftp ��ȡҪ���ص��ļ���ͷ����ǰSEGSIZE ���ֽ�
*��������� pcSerName��tftp��������ip��ַ
				  pcSerPort��tftp�������Ķ˿�
				  pcRemoteName��Ҫ���ص��ļ�
				  stTimeOUt, ��ʱ����	
						
*���������caHeadBuf�����ص��ļ�ͷ�Ļ���
				 adresse_serveur��tftp������sockaddr_in ��ַ
				 pnPeer��tftp��������socket ���
				 pstAutopErr��������Ϣ
				 
*���ؽ����ʵ�ʽ��յ����ļ�ͷ�ֽ���
*/
int tftp_recv_head(char *pcSerName, char *pcSerPort, char *pcRemoteName, TimeOut_st stTimeOUt,
	char caHeadBuf[SEGSIZE], struct sockaddr_in *adresse_serveur, int *pnPeer, Err_st *pstAutopErr);

/*���ܣ�ͨ��tftp ��ȡҪ���ص��ļ�,���ô˺���ǰ���ȵ���tftp_recv_head��ȡ
			Ҫ���ص��ļ�ͷ
*��������� pcLocalName���ļ����غ�浽���ص��ļ���
				  caHeadBuf�������ص��ļ�ͷ����
				  nHeadBufSize�������ص��ļ�ͷ��С
				  to1��tftp������sockaddr_in ��ַ
				  peer��tftp��������socket ���
				  stTimeOUt, ��ʱ����	
				  unMaxFileSize, �����ص�����ֽ���
				  
*���������pstAutopErr��������Ϣ
				 
*���ؽ����SUCC:�ɹ�
				 FAIL: ʧ��
*/
int tftp_recv_to_file(char *pcLocalName, char caHeadBuf[SEGSIZE], int nHeadBufSize, 
						struct sockaddr_in *to1, int peer, TimeOut_st stTimeOUt, 
						unsigned int unMaxFileSize, Err_st *pstAutopErr);

/*���ܣ�ͨ��tftp ��ȡҪ���ص��ļ�,���浽�ڴ���,����
			�˺���ǰ���ȵ���tftp_recv_head��ȡ	Ҫ���ص���
			��ͷ
*��������� caHeadBuf�������ص��ļ�ͷ����
				    nHeadBufSize�������ص��ļ�ͷ��С
				    to1��tftp������sockaddr_in ��ַ
				    peer��tftp��������socket ���
				    stTimeOUt, ��ʱ����	
				    unMaxFileSize, �����ص�����ֽ���
				  
*���������ppcRam���������ݴ�ŵĻ���
				    pnFileSize ���������ݵĴ�С
				    pstAutopErr��������Ϣ
				 
*���ؽ����SUCC:�ɹ�
				 FAIL: ʧ��
*/
int tftp_recv_to_ram(char **ppcRam, int *pnFileSize, char caHeadBuf[SEGSIZE],
						int nHeadBufSize, struct sockaddr_in *to1, int peer, 
						TimeOut_st stTimeOUt, unsigned int unMaxFileSize, 
						Err_st *pstAutopErr);

/*���ܣ�ͨ��tftp �����ļ�
*��������� pcSerName��tftp��������ip��ַ
				  pcSerPort��tftp�������Ķ˿�
				  pcRemoteName��Ҫ���ص��ļ�
				  pcLocalName�� ���ش洢�ļ�������ΪNULL��
				  			���pcRemoteName һ��
				  stTimeOUt, ��ʱ����
				  unMaxFileSize, �����ص�����ֽ���
				  
*��������� pstAutopErr��������Ϣ
				 
*���ؽ���� SUCC:�ɹ�
				  FAIL: ʧ��
*/
int tftp_receive(char *pcSerName, char *pcSerPort, char *pcRemoteName, char *pcLocalName, 
					TimeOut_st stTimeOUt, unsigned int unMaxFileSize, Err_st *pstAutopErr);


/*���ܣ��ͻ��˷����������Ĵ�����Ϣ
*���������peer��tftp��������socket ���	
				  to��tftp������sockaddr_in ��ַ
				  error�� ������
				  commentaire�� ��������
						
*�����������
				 
*���ؽ������
*/
void tftp_send_error(int peer,struct sockaddr_in *to,int error,char *commentaire);


#ifdef  __cplusplus
}
#endif

#endif //tftp.h
#endif//#if(!M_VIDEOPHONE)
#endif//#ifndef WIN32

