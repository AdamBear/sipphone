#ifndef _https_utils_h
#define _https_utils_h

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define CA_INFO_BUF_SIZE 50 
#define MAX_CA_NUM 10 

typedef struct
{        
	char caIssuedTo[CA_INFO_BUF_SIZE];
	char caIssuedBy[CA_INFO_BUF_SIZE];
	char caExpireDate[CA_INFO_BUF_SIZE];
	unsigned long ulHashValue;
	char caFileName[CA_INFO_BUF_SIZE];
}ca_info_st; 

typedef struct
{        
	int nCaNum;
	ca_info_st stCaInof[MAX_CA_NUM];
}ca_dir_info_st; 

typedef struct
{        
	char caIssuedTo[CA_INFO_BUF_SIZE];
	char caIssuedBy[CA_INFO_BUF_SIZE];
	char caExpireDate[CA_INFO_BUF_SIZE];
}ser_ca_info_st; 


/*���ܣ�ͨ��https �����ļ�
*��������� pcSerUrl��http��������url, https://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
				     stProxy�����������	
				     stTimeOUt, ��ʱ����
				     unMaxFileSize, �����ص�����ֽ���	
				     pcLocalName�� ���ش洢�ļ�������ΪNULL��
				  			��ȡpcSerUrl �е��ļ���
					
						
*��������� pstAutopErr��������Ϣ
				 
*���ؽ���� SUCC:�ɹ�
				  FAIL: ʧ��
			ע:�˺������������https_recv_to_fileһ������ʵ�ǽ���URL�����https_recv_to_file_2
*/
int https_recv_to_file(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
						unsigned int unMaxFileSize, char *pcLocalName, Err_st *pstErr);


/*���ܣ�ͨ��https ��ȡҪ���ص��ļ���ͷ����ǰHTTP_HEAD_SIZE ���ֽ�
*��������� pcSerUrl��https��������url, https://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
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
int https_recv_head(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
					unsigned int unMaxFileSize,char caFtpHead[HTTP_HEAD_SIZE], 
					int *pnHeadSize, SSL** ssl, SSL_CTX **ctx,  int *pnFdData, 
					int *pnFileSize, Err_st *pstErr);



/*���ܣ�ͨ��https ��ȡҪ���ص��ļ��������ص��ļ�ͷ��ʣ�ಿ�֣�Ȼ��浽
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
int https_remain_to_ram(SSL* ssl, SSL_CTX *ctx, 
						int nFdData, char caFtpHead[HTTP_HEAD_SIZE], int nHeadSize,
						int nFileSize, char *pcRam, TimeOut_st stTimeOUt, Err_st *pstErr);

/*���ܣ�ͨ��https ��ȡҪ���ص��ļ���������ڴ���
*���������pcSerUrl��http��������url, https://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
				    stProxy�����������	
				    stTimeOUt, ��ʱ����
				    unMaxFileSize, �����ص�����ֽ���	
				     
*���������ppcRam���������ݴ�ŵĻ���
				  pnFileSize ���������ݵĴ�С
				  pstAutopErr��������Ϣ
				 
*���ؽ����http ������
*ע��: ʹ�ú����ͷŶ�̬����Ļ���ppcRam !
*/
int https_recv_to_ram(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
				unsigned int unMaxFileSize,char **ppcRam, int *pnFileSize, Err_st *pstErr);

/*���ܣ�ͨ��https ��ȡҪ���ص��ļ���������Ӧsocket������������
*���������pcSerUrl��http��������url, https://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
				    stProxy�����������	
				    stTimeOUt, ��ʱ����
				    unMaxFileSize, �����ص�����ֽ���	
				     
*���������		  ssl��		ssl�����Ϣ
				  ctx,
				  pnFdData,	socket
				  pnFileSize ��Ҫ�������ݵĴ�С(http��body��С)
				  pstAutopErr��������Ϣ
				 
*���ؽ����0:�ɹ�,-1:ʧ��
*/
int https_open_file(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
					unsigned int unMaxFileSize,SSL** ssl, SSL_CTX **ctx, int *pnFdData, 
					int *pnFileSize, Err_st *pstErr);

/*���ܣ�ͨ��https��ָ����socket�ж�ȡһ�����ȵ����ݣ���pcRam����(pcRam��ҪӦ�ó��������ڴ�)
*���������			ssl,ctx,nFdData��  ssl��socket�����Ϣ
				    stTimeOUt, ��ʱ����
				     pcRam:�������ȥ�����ڽ�������(Ӧ�ó������)
*���������		  
				  pcRam����ȡ�������ݡ�
				  pstAutopErr��������Ϣ
				 
*���ؽ�������յ������ݳ��ȣ� -1Ϊʧ��

ע�⣺�ú���û�ж�ssl��socket�����ͷź͹رգ���ҪӦ�ó����Լ����رա�
*/
int https_read_file(SSL* ssl, SSL_CTX *ctx, int nFdData,int nDesiredSize, char *pcRam, TimeOut_st stTimeOUt,
					Err_st *pstErr);

/*���ܣ�	�ͷ�ssl����ز������ر�https��socket�׽ӿ�
*���������			nSocket:Ҫ�رյ��׽ӿ�
					ssl,ctx:ssl��ز���
*���������		  

				 
*���ؽ�����Ƿ�رճɹ���0:�ɹ�,-1ʧ��
*/
int https_close_file(SSL* ssl, SSL_CTX *ctx, int nFdData);





/*
*���ܣ���ca֤������ȡissuer hashֵ��������hashֵΪ�ļ�
			����ca֤���ļ���������

*���������pcPemCA:ca֤��洢·��
						
*���������
				 					 
*���ؽ����0: �ɹ�
				    -1: �ļ��Ƿ�
				    -2: Ŀ¼��ca �ļ������������Ƹ���
				    -3: ��ca �ļ��Ѵ���
*/
int install_ca(char *pcPemCA);

/*
*���ܣ�ɾ��ca ֤���ļ�����������

*���������pcPemCA:ca֤��洢·��
						
*���������
				 					 
*���ؽ����0: �ɹ�
				    -1: ʧ��
*/
int del_ca(char *pcPemCA);

/*
*���ܣ�����ca ֤���ļ�Ŀ¼����ȡca ֤��ĸ�������
			֤�����Ϣ

*���������
						
*���������pstCaDirInfo: ֤��ĸ����͸�֤��������Ϣ
				 					 
*���ؽ����0: �ɹ�
				    -1: ʧ��
*/
int ca_dir_info(ca_dir_info_st *pstCaDirInfo);


/*
*���ܣ���ȡ/yealink/config/certs/server/WebServer.pem֤�����Ϣ

*�����������
						
*���������pstCaDirInfo:֤��������Ϣ

*���ؽ����0: �ɹ�
		   -1: ʧ��
*/
int ser_ca_info(ser_ca_info_st *pstSerCaInfo);

/*
*���ܣ��ж�web������֤��ĺϷ��ԣ��Ƿ����ش����룬�Ϸ��ͽ�֤�鸲�ǵ�/yealink/config/certs/server/WebServer.pem

*���������pcSerCa: ca֤��洢·��
						
*�����������
				 					 
*���ؽ����0: �ɹ�
				    -1: ��pem��ʽ�ļ�
				    -2: �ļ�����˽Կ
				    -3: �ļ����޹�Կ
				    -4: ˽Կ�͹�Կ�����
				    -5: openssl ��ʼ������
				    -6: ֤����Ŀ¼����ʧ��
*/
int install_ser_ca (char *pcSerCa);


#ifdef  __cplusplus
}
#endif

#endif

