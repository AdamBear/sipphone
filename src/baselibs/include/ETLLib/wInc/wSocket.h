
#ifndef __wSOCKET_H__
#define __wSOCKET_H__

namespace ETL_SocketHelper
{

#define wSockGetLocalIPAddress              sockGetLocalIPAddress

#define wSockClose                          sockClose
#define wSockListen                         sockListen

#define wSockSetBlocking                    sockSetBlocking
#define wSockAsyncConnect                   sockAsyncConnect
#define wSockAccept                         sockAccept
#define wSockShutdown                       sockShutdown
#define wSockPeekRecvLength                 sockPeekRecvLength
#define wSockRecvFrom                       sockRecvFrom
#define wSockSendTo                         sockSendTo
#define wSockRecv                           sockRecv
#define wSockSend                           sockSend

#define wInet_pton                          sockInet_pton
#define wInet_ntop                          sockInet_ntop
};
using namespace ETL_SocketHelper;

#endif //__wSOCKET_H__

