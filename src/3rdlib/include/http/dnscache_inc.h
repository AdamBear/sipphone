#ifndef __DNSCACHE_H__
#define __DNSCACHE_H__

#ifdef  __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#endif
#include <sys/types.h>



#include <errno.h>





#define MAX_DNS_CACHE	20 


struct dns_answer
{
	char ip[48];
	int  ttl;
};

typedef struct  
{
	char szDomain[256];
	unsigned int iTTL;
	unsigned long iLastTime;
	int iType;
	struct dns_answer DnsAnswer[4];
}DOMAIN_INFO;


int MyGetAddrInfo(const char* pszDomain, char* pszBuffer, int iType);
int ClearAllDnsCache(void);

#ifdef  __cplusplus
}
#endif

#endif 
