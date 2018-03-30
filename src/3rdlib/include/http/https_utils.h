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


/*功能：通过https 下载文件
*输入参数： pcSerUrl，http服务器的url, https://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
				     stProxy，代理服务器	
				     stTimeOUt, 超时设置
				     unMaxFileSize, 可下载的最大字节数	
				     pcLocalName， 本地存储文件名，若为NULL，
				  			则取pcSerUrl 中的文件名
					
						
*输出参数： pstAutopErr，错误信息
				 
*返回结果： SUCC:成功
				  FAIL: 失败
			注:此函数输入参数与https_recv_to_file一样，其实是解析URL后调用https_recv_to_file_2
*/
int https_recv_to_file(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
						unsigned int unMaxFileSize, char *pcLocalName, Err_st *pstErr);


/*功能：通过https 获取要下载的文件的头，即前HTTP_HEAD_SIZE 个字节
*输入参数： pcSerUrl，https服务器的url, https://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
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
int https_recv_head(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
					unsigned int unMaxFileSize,char caFtpHead[HTTP_HEAD_SIZE], 
					int *pnHeadSize, SSL** ssl, SSL_CTX **ctx,  int *pnFdData, 
					int *pnFileSize, Err_st *pstErr);



/*功能：通过https 获取要下载的文件除已下载的文件头的剩余部分，然后存到
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
int https_remain_to_ram(SSL* ssl, SSL_CTX *ctx, 
						int nFdData, char caFtpHead[HTTP_HEAD_SIZE], int nHeadSize,
						int nFileSize, char *pcRam, TimeOut_st stTimeOUt, Err_st *pstErr);

/*功能：通过https 获取要下载的文件，存放在内存中
*输入参数：pcSerUrl，http服务器的url, https://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
				    stProxy，代理服务器	
				    stTimeOUt, 超时设置
				    unMaxFileSize, 可下载的最大字节数	
				     
*输出参数：ppcRam，下载数据存放的缓冲
				  pnFileSize ，下载数据的大小
				  pstAutopErr，错误信息
				 
*返回结果：http 错误码
*注意: 使用后请释放动态申请的缓冲ppcRam !
*/
int https_recv_to_ram(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
				unsigned int unMaxFileSize,char **ppcRam, int *pnFileSize, Err_st *pstErr);

/*功能：通过https 获取要下载的文件，返回相应socket，不返回数据
*输入参数：pcSerUrl，http服务器的url, https://xyx:123456@192.168.0.139:5566/digest/000000000000.cfg
				    stProxy，代理服务器	
				    stTimeOUt, 超时设置
				    unMaxFileSize, 可下载的最大字节数	
				     
*输出参数：		  ssl，		ssl相关信息
				  ctx,
				  pnFdData,	socket
				  pnFileSize ，要下载数据的大小(http的body大小)
				  pstAutopErr，错误信息
				 
*返回结果：0:成功,-1:失败
*/
int https_open_file(char *pcSerUrl, PROXY_ST stProxy, TimeOut_st stTimeOUt, 
					unsigned int unMaxFileSize,SSL** ssl, SSL_CTX **ctx, int *pnFdData, 
					int *pnFileSize, Err_st *pstErr);

/*功能：通过https从指定的socket中读取一定长度的数据，从pcRam返回(pcRam需要应用程序申请内存)
*输入参数：			ssl,ctx,nFdData，  ssl和socket相关信息
				    stTimeOUt, 超时设置
				     pcRam:输出缓冲去，用于接收数据(应用程序分配)
*输出参数：		  
				  pcRam：读取到的数据。
				  pstAutopErr，错误信息
				 
*返回结果：接收到的数据长度， -1为失败

注意：该函数没有对ssl和socket进行释放和关闭，需要应用程序自己来关闭。
*/
int https_read_file(SSL* ssl, SSL_CTX *ctx, int nFdData,int nDesiredSize, char *pcRam, TimeOut_st stTimeOUt,
					Err_st *pstErr);

/*功能：	释放ssl的相关参数并关闭https的socket套接口
*输入参数：			nSocket:要关闭的套接口
					ssl,ctx:ssl相关参数
*输出参数：		  

				 
*返回结果：是否关闭成功：0:成功,-1失败
*/
int https_close_file(SSL* ssl, SSL_CTX *ctx, int nFdData);





/*
*功能：从ca证书中提取issuer hash值，建立以hash值为文件
			名到ca证书文件的软链接

*输入参数：pcPemCA:ca证书存储路径
						
*输出参数：
				 					 
*返回结果：0: 成功
				    -1: 文件非法
				    -2: 目录下ca 文件个数超过限制个数
				    -3: 该ca 文件已存在
*/
int install_ca(char *pcPemCA);

/*
*功能：删除ca 证书文件及其软链接

*输入参数：pcPemCA:ca证书存储路径
						
*输出参数：
				 					 
*返回结果：0: 成功
				    -1: 失败
*/
int del_ca(char *pcPemCA);

/*
*功能：遍历ca 证书文件目录，获取ca 证书的个数及各
			证书的信息

*输入参数：
						
*输出参数：pstCaDirInfo: 证书的个数和各证书的相关信息
				 					 
*返回结果：0: 成功
				    -1: 失败
*/
int ca_dir_info(ca_dir_info_st *pstCaDirInfo);


/*
*功能：获取/yealink/config/certs/server/WebServer.pem证书的信息

*输入参数：无
						
*输出参数：pstCaDirInfo:证书的相关信息

*返回结果：0: 成功
		   -1: 失败
*/
int ser_ca_info(ser_ca_info_st *pstSerCaInfo);

/*
*功能：判断web服务器证书的合法性，非法返回错误码，合法就将证书覆盖到/yealink/config/certs/server/WebServer.pem

*输入参数：pcSerCa: ca证书存储路径
						
*输出参数：无
				 					 
*返回结果：0: 成功
				    -1: 非pem格式文件
				    -2: 文件中无私钥
				    -3: 文件中无公钥
				    -4: 私钥和公钥不配对
				    -5: openssl 初始化错误
				    -6: 证书存放目录创建失败
*/
int install_ser_ca (char *pcSerCa);


#ifdef  __cplusplus
}
#endif

#endif

