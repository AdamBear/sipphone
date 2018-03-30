#pragma once

#include <winsock2.h>

#ifndef POLLIN

/* Event flag definitions for WSAPoll(). */

#define POLLRDNORM  0x0100
#define POLLRDBAND  0x0200
#define POLLIN      (POLLRDNORM | POLLRDBAND)
#define POLLPRI     0x0400

#define POLLWRNORM  0x0010
#define POLLOUT     (POLLWRNORM)
#define POLLWRBAND  0x0020

#define POLLERR     0x0001
#define POLLHUP     0x0002
#define POLLNVAL    0x0004

struct pollfd
{
    int fd;
    short events;
    short revents;
};
#endif

#define poll(a, b, c) -1

