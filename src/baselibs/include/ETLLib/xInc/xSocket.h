#ifndef __xSocket_h__
#define __xSocket_h__

namespace ETL_SocketHelper
{

#define INVALID_SOCKET                      (SOCKET)(~0)
#define SOCKET_ERROR                        (-1)

#define SOCKET                              int
#define xSockGetLocalIPAddress              sockGetLocalIPAddress

#define xSockClose                          sockClose
#define xSockListen                         sockListen

#define xSockSetBlocking                    sockSetBlocking
#define xSockAsyncConnect                   sockAsyncConnect
#define xSockAccept                         sockAccept
#define xSockShutdown                       sockShutdown
#define xSockPeekRecvLength                 sockPeekRecvLength
#define xSockRecvFrom                       sockRecvFrom
#define xSockSendTo                         sockSendTo
#define xSockRecv                           sockRecv
#define xSockSend                           sockSend

#define xInet_pton                          sockInet_pton
#define xInet_ntop                          sockInet_ntop
};
using namespace ETL_SocketHelper;

#endif //__xSocket_h__

