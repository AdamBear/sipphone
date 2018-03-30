#ifndef __appSocket_h__
#define __appSocket_h__

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#ifdef  __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////
// class httpFile
class httpFile
{
public:
	httpFile();
	~httpFile();
	int OpenHttpFile(const char* lpszURL, int nTimeout);
	void CloseHttpFile();
	int ReadFile(void * pBuffer, int nSize, int nCount, int nTimeout);
protected:
	int m_nSocket;
};

//////////////////////////////////////////////////////////////////////////
// class httpsFile
class httpsFile
{
public:
	httpsFile();
	~httpsFile();

	int OpenHttpsFile(const char* lpszURL, int nTimeout);
	void CloseHttpsFile();
	int ReadFile(void * pBuffer, int nSize, int nCount, int nTimeout);
protected:
	SSL*		m_ssl;
	SSL_CTX*	m_ctx;
	int			m_nSocket;
};


#ifdef  __cplusplus
}
#endif

#endif
