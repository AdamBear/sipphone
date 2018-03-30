
#ifndef __ETLSOCKET_H__
#define __ETLSOCKET_H__

#ifndef __ETL_H__
#error ETLSocket.h requires ETL.h to be included first
#endif

#if LINUX_SYSTEM
#include "xInc/xSocket.h"
#else
#include "wInc/wSocket.h"
#endif

namespace ETL_SocketHelper
{

//////////////////////////////////////////////////////////////////////////
// declare block in "xInc/xSocket.h" or "wInc/wSocket.h"
EXTERN_DECLARE(yl::string sockGetLocalIPAddress(LPCSTR lpszAdaptName));
EXTERN_DECLARE(int sockClose(SOCKET so));
EXTERN_DECLARE(BOOL sockListen(SOCKET sock, int nBacklog));
EXTERN_DECLARE(BOOL sockSetBlocking(SOCKET s, BOOL bBlocking));
EXTERN_DECLARE(BOOL sockAsyncConnect(SOCKET s, const struct sockaddr * to, int nTimeout));
EXTERN_DECLARE(SOCKET sockAccept(SOCKET s, struct sockaddr * addr, int * addrlen));
EXTERN_DECLARE(void sockShutdown(SOCKET s));
EXTERN_DECLARE(int sockPeekRecvLength(SOCKET s));
EXTERN_DECLARE(int sockRecvFrom(SOCKET s, void * buf, int len, int flags, struct sockaddr * from,
                                int * fromlen));
EXTERN_DECLARE(int sockSendTo(SOCKET s, const void * buf, int len, int flags,
                              const struct sockaddr * to, int tolen));
EXTERN_DECLARE(int sockRecv(SOCKET s, void * buf, int len, int flags));
EXTERN_DECLARE(int sockSend(SOCKET s, const void * buf, int len, int flags));

EXTERN_DECLARE(int sockInet_pton(int af, const char * src, void * dst));
EXTERN_DECLARE(const char * sockInet_ntop(int af, const void * src, char * dst, socklen_t cnt));
// end declare block
//////////////////////////////////////////////////////////////////////////

#define     IN_ALEN         sizeof(in_addr)
#define     IN_ALEN_TEXT    IN_ALEN * 4  //like 192.168.000.207

#define ETH_ALEN            6
#define ETH_ALEN_TEXT       ETH_ALEN * 3 // like 00:15:65

EXTERN_DECLARE(int sockGetHostIPs(LPCSTR lpszHostName, chListStringA & arrIPs));
EXTERN_DECLARE(BOOL sockIsValidIPv4(LPCSTR lpszIPv4));

//////////////////////////////////////////////////////////////////////////
// class chSocketAddr
class chSocketAddr
{
public:
    BOOL CreateSocketAddr(in_addr inAddr, int nPort);
    BOOL CreateSocketAddr(LPCSTR lpszIPv4, int nPort);
    BOOL CreateSocketAddr(LPCSTR lpszIPv4WithPort);

    BOOL CreateSocketAddr(in6_addr in6Addr, int nPort);

    int Family() const
    {
        return m_unionAddrObject.addr.sa_family;
    }
    int Size() const
    {
        return (Family() == PF_INET6) ? sizeof(sockaddr_in6) : sizeof(sockaddr);
    }

    yl::string GetIPString(BOOL bWithPort) const;
    yl::string GetIPv6(BOOL bWithPort) const;
    yl::string GetIPv4(BOOL bWithPort) const;
    int GetPort() const;

    BOOL operator == (const chSocketAddr & other) const;
public:
    chSocketAddr();
    chSocketAddr(in_addr inAddr, int nPort);
    chSocketAddr(LPCSTR lpszIPv4, int nPort);
    chSocketAddr(LPCSTR lpszIPv4WithPort);
public:
    union
    {
        sockaddr        addr;
        sockaddr_in     addr_in;
        sockaddr_in6    addr_in6;
    } m_unionAddrObject;
};

//////////////////////////////////////////////////////////////////////////
// class chSocket
class chSocket
{
public:
    BOOL CreateSocket(int af, int type, int protocol);
    void DestroySocket();
    BOOL Bind(const chSocketAddr & objAddr);
    BOOL IsValidSocket();
    operator SOCKET() const;

protected:
    int socketSelect(int nTimeout);

private:
    chSocket & operator = (const chSocket &);
    chSocket(const chSocket &);

public:
    chSocket();
    ~chSocket();

public:
    SOCKET          m_hSocket;
};

//////////////////////////////////////////////////////////////////////////
// class chSocketUDP : public chSocket
class chSocketUDP : public chSocket
{
public:
    BOOL CreateSocket(int af = PF_INET);
    BOOL CreateSocket(const chSocketAddr & objAddr);
    BOOL CreateSocket(LPCSTR lpszIP, int nPort);
    // loop to receive till no data receive
    int ReceiveFrom(chByteArray & refBuffer, chSocketAddr * pSrcAddr, int nTimeout);
    BOOL SendTo(const void * pBuffer, int nSize, const chSocketAddr & dstAddr);

public:
    int SelectAndReceive(int nTimeout);
    int socketReceive();

public:
    chSocketUDP();
public:
    chByteArray     m_udpPacketBuffer;
    chSocketAddr    m_addrPacketSrc;
};

//////////////////////////////////////////////////////////////////////////
// class chSocketTCP : public chSocket
class chSocketTCP : public chSocket
{
    friend class chSocketTCPServer;
public:
    BOOL CreateSocket(int af = PF_INET);
    BOOL CreateSocket(const chSocketAddr & objAddr);
    BOOL CreateSocket(LPCSTR lpszIP, int nPort);
    void DestroySocket();

    BOOL ConnectTo(const chSocketAddr & objAddr, int nTimeout);
    BOOL ConnectTo(LPCSTR lpszIP, int nPort, int nTimeout);
    BOOL CheckTCPConnected();
    BOOL IsTCPConnected();
    void Disconnect();

    // receive date block ,return blocks readed
    int Receive(LPVOID lpBuffer, int nSize, int nCount, int nTimeout);
    BOOL Send(const void * pBuffer, int nSize);
protected:
    int ReceiveStream(chByteArray & refBuffer, int nTimeout);
    // reveive data to inner buffer and copy 0 bytes if timeout
    BOOL ReceivePacket(LPVOID lpBuffer, int nPacketSize, int nTimeout);
public:
    int SelectAndReceive(int nReceiveCount, int nTimeout);
    int socketReceive(int nReceiveSize);
    // reveive data to inner buffer and copy 0 bytes if timeout return 0 or nPacketSize
    int PeekPacket(LPVOID lpBuffer, int iOffset, int nPacketSize, int nTimeout);
    void DropPacket(int nPacketSize);

public:
    chSocketTCP();
    ~chSocketTCP();
protected:
    BOOL            m_bIsConnected;
    chByteArray     m_bufLastPacket;
public:
    chByteArray     m_tcpPacketBuffer;
};

//////////////////////////////////////////////////////////////////////////
// class chSocketTCPServer : public chSocketTCP
class chSocketTCPServer : public chSocketTCP
{
public:
    BOOL SocketListen(int nBacklog);
    BOOL AcceptOnSocket(chSocketTCP & sockClient, chSocketAddr * pClientAddr, int nTimeout);
};

//////////////////////////////////////////////////////////////////////////
// class chSocketPacketTCP : public chSocketTCP
class chSocketPacketTCP : public chSocketTCP
{
public:
    BOOL ReceiveOnePacket(chByteArray & refBuffer, int nTimeout);
    BOOL SendOnePacket(const void * pBuffer, int nSize);
};

};
using namespace ETL_SocketHelper;


#if IF_EXPORT_SYMBOLS
#include "ETLSocket.cpp"
#endif

#endif //__ETLSOCKET_H__

