#pragma once

#include <tchar.h>
#include <fcntl.h>

#include <sys/time.h>

#include <io.h>
#include <process.h>
#include <direct.h>

#define MAJOR(a) (int)a
#define MINOR(a) (int)a
#define mknod(a,b,c) -1
typedef unsigned int mode_t;

#define S_IRWXU (S_IREAD | S_IWRITE | S_IEXEC)
#define S_IRWXG (S_IREAD | S_IWRITE | S_IEXEC)
#define S_IRWXO (S_IREAD | S_IWRITE | S_IEXEC)

#define mkdir(name, mode)   mkdir(name)
