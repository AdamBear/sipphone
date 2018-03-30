/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2010-10-21
*
*******************************************************************/
#ifndef __DECT_TYPE_H__
#define __DECT_TYPE_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdarg.h>
#include <inttypes.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <syslog.h>
#include <semaphore.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/socket.h> 
#include <sys/time.h>

#include <linux/netlink.h>

#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netinet/ip6.h>

/************************************************************************/
/*                                                                      */
/************************************************************************/
typedef  unsigned char          u8;
typedef    signed char          s8;
typedef  unsigned short         u16;
typedef    signed short         s16;
typedef  unsigned int           u32;
typedef    signed int           s32;
typedef  unsigned long long     u64;
typedef    signed long long     s64;

typedef unsigned char           BYTE;
typedef unsigned short          WORD;
typedef unsigned int            DWORD;
/*typedef	int                     BOOL;*/

typedef unsigned char           D_BYTE;
typedef unsigned short          D_WORD;
typedef unsigned int            D_DWORD;
typedef unsigned int            D_UINT;
typedef int                     D_INT;
typedef unsigned char           D_UCHAR;
typedef char                    D_CHAR;

/*
* Some types definition. They are defind somewhere, but difficult to find the place
*/
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  (!FALSE)
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/

/**
* container_of - cast a member of a structure out to the containing structure
*
* @ptr:	the pointer to the member.
* @type:	the type of the container struct this is embedded in.
* @member:	the name of the member within the struct.
*
*/
#ifndef offset_of
#define offset_of(TYPE, MEMBER) ({TYPE * __ptr=(TYPE*)0;(size_t) &((TYPE *)__ptr)->MEMBER;})
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({			\
    type *_fake_p = NULL;                          \
    const typeof( ((type *)_fake_p)->member ) *__mptr = (ptr);	\
    (type *)( (char *)__mptr - offset_of(type,member) );})
#endif

#ifndef class_of
#define class_of(ptr, type, member) ({			\
    type *_fake_p = NULL;                          \
    const typeof( ((type *)_fake_p)->member ) *__mptr = (ptr);	\
    (type *)( (char *)__mptr - offset_of(type,member) );})
#endif

/*
* Check at compile time that something is of a particular type.
* Always evaluates to 1 so you may use it easily in comparisons.
*/
#define typecheck(type,x) \
    ({	type __dummy; \
    typeof(x) __dummy2; \
    (void)(&__dummy == &__dummy2); \
    1; \
})

/**
* TABLESIZE - 
*
* @type:
* @member:
*
*/
#define TABLESIZE(array) (int)( (sizeof(array)/sizeof(array[0])))
#define ARRAY_SIZE(x)    ((int)(sizeof(x) / sizeof((x)[0])))
#define ALIGN(x,a)       (((x)+(a)-1)&~((a)-1))

/*
* ..and if you can't take the strict
* types, you can specify one yourself.
*
* Or not use min/max at all, of course.
*/
// #define min_t(type,x,y) \
//     ({ type __x = (x); type __y = (y); __x < __y ? __x: __y; })
// #define max_t(type,x,y) \
//     ({ type __x = (x); type __y = (y); __x > __y ? __x: __y; })
// 
// #define max(a,b)    ((a) > (b) ? (a) : (b))
// #define min(a,b)    ((a) < (b) ? (a) : (b))
// 
// #define MIN(A, B)   ((A < B) ? A : B)
// #define MAX(A, B)   ((A > B) ? A : B)

/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /* __DECT_TYPE_H__ */
