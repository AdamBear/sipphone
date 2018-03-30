#pragma once

typedef int pid_t;
typedef int uid_t;
typedef int gid_t;

#define O_NONBLOCK 0
#define SIGCHLD 0
#define S_IFIFO 0

#define fork() -1

#include <process.h>

#define mkfifo(a,b) -1