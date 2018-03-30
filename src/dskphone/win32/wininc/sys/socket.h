#pragma once

#ifndef __MAP_LINUX_WIN32_SOCKET_H__
#define __MAP_LINUX_WIN32_SOCKET_H__

#include <WinSock2.h>
#include <WS2tcpip.h>

typedef int pid_t;
typedef unsigned int u_32;
typedef u_short sa_family_t;

struct sockaddr_nl
{
    sa_family_t nl_family; /*AF_NETLINK*/
    unsigned short nl_pad; /* 0 */
    pid_t nl_pid; /* 进程pid */
    u_32 nl_groups; /* 多播组掩码*/
};

//struct sockaddr

#define AF_NETLINK AF_INET
#define PF_NETLINK PF_INET
#define NETLINK_KOBJECT_UEVENT 0
#define SO_RCVBUFFORCE SO_RCVBUF
#define SO_PASSCRED SO_ERROR

#if defined(_MSC_VER) && (_MSC_VER < 1600)
struct cmsghdr
{
    socklen_t cmsg_len;
    int       cmsg_level;
    int       cmsg_type;
};
#endif

#ifndef CMSG_SPACE
#define CMSG_SPACE(a) (int)a
#endif

typedef void * ptr_t;

struct iovec
{
    ptr_t iov_base; /* Starting address */
    size_t iov_len; /* Length in bytes */
};

struct msghdr
{
    void  * msg_name;
    socklen_t msg_namelen;
    struct iovec  * msg_iov;
    int  msg_iovlen;
    void  * msg_control;
    socklen_t msg_controllen;
    int  msg_flags;
};

#define __u32 unsigned int

struct ucred
{
    __u32 pid;
    __u32 uid;
    __u32 gid;
};

#define recvmsg(a, b, c) -1

#ifdef CMSG_FIRSTHDR
#undef CMSG_FIRSTHDR
#endif

#define CMSG_FIRSTHDR(a) (struct cmsghdr*)a

#define SCM_CREDENTIALS 0

#ifdef CMSG_DATA
#undef CMSG_DATA
#endif

#define CMSG_DATA(a) (struct ucred*)a

#endif