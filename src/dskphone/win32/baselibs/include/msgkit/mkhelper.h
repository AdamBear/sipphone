/* ******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2013-03-02
*
* for export APIs header file , helper layer
*
*    <<<<!!code MUST be isolated!! >>>>
*
****************************************************************** */
#ifndef __MK_HELPER_H__
#define __MK_HELPER_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include "mkmessage.h"
#include "mkservice.h"

#if !WIN32/* MKIT_WIN32 */
#include <unistd.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/shm.h>
#endif
/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
/************************************************************************/
/* type  define                                                         */
/************************************************************************/
#if WIN32
/* windows */
#undef MAKEWORD
#undef MAKELONG
#undef LOWORD
#undef HIWORD
#undef LOBYTE
#undef HIBYTE
#else
typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned int   DWORD;
typedef unsigned long  LONG;
#endif

/************************************************************************/
/* micro define                                                         */
/************************************************************************/
/*
* for compatible
*/
#ifndef max
#define max(_a,_b) ((_a)>(_b)?(_a):(_b))
#endif

#ifndef min
#define min(_a,_b) ((_a)<(_b)?(_a):(_b))
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(_a) (sizeof(_a)/sizeof(_a[0]))
#endif
#ifndef TABLESIZE
#define TABLESIZE(_a) (sizeof(_a)/sizeof(_a[0]))
#endif

/*
* for magic
*/
#ifndef MAGIC
#define MAGIC(_a,_b,_c,_d)   ((_a)<<0|(_b)<<8|(_c)<<16|(_d)<<24)
#endif

/*
* for byte ops
*/
#ifndef MAKEWORD
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD)(b)) & 0xff))   | ((WORD) ((BYTE)(((DWORD)(a)) & 0xff)))   << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD)(b)) & 0xffff)) | ((DWORD)((WORD)(((DWORD)(a)) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)(((DWORD)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD)(w)) >> 8) & 0xff))
#endif

/*
* for ALIGN
*/
#ifndef ALIGN
#if WIN32/* MKIT_WIN32 */
#define ALIGN(x, a)            (x)
#else
#define ALIGN(x, a)            ALIGN_MASK(x, (typeof(x))(a) - 1)
#endif
#define ALIGN_MASK(x, mask)    (((x) + (mask)) & ~(mask))
#endif

#if WIN32/* MKIT_WIN32 */
#ifdef PATH_MAX
#if (PATH_MAX>256)
#undef  PATH_MAX
#define PATH_MAX 256
#endif
#else
#define PATH_MAX 256
#endif
#endif

/*key path*/
#define MKSHMEM_KEY_SERVICE         MAGIC('M','S','R','V')  /*shm key for service*/
#define MKSHMEM_KEY_SYSTEM          MAGIC('K','S','Y','S')  /*shm key for system*/


/************************************************************************/
/* APIs for helper   !!code MUST be isolated!!                          */
/************************************************************************/
/**
* @brief  :helper function for console parse
*
* @param  :private_data,user private handle
*          func,command callback
*          help,help callback
*
* @warning:
*
* @return :0 as success,otherwise failed.
*
*/
/*command callback type*/
typedef int (*mkcmd_cb)(void * private_data, int argc, const char * argv[]);

/*command list*/
typedef struct
{
    int         argc;
    mkcmd_cb    cmd;
    const char * name;
    const char * help;
} mkcmd_t;


MK_DECL(int)  mkcon_run(void * private_data, const char * prompt, const mkcmd_t * cmd, int cmd_nr);
MK_DECL(int)  mkcon_exe(void * private_data, const char * cmdline, const mkcmd_t * cmd, int cmd_nr);

/**
* @brief  :helper function for named shared memory
*
* @param  :magic,magic of key
*          size,real size on create,return size on get
*          shmwr,1==create or get as writable,0==get as read only
*
* @warning:
*
* @return :address as success,NULL as  failed.
*
*/
MK_DECL(void *) mkshm_get(int magic, int * size, int shmwr);
MK_DECL(void)   mkshm_put(void * shm);
MK_DECL(int)    mkshm_del(int magic);

/**
* @brief  :reboot control
*
* @param  :
*
* @warning:do NOT try to call this APIs as NOT authorized!!
*
* @return :
*
*/
MK_DECL(int) mksys_reboot(int flush, int hung);

/**
* @brief  :read/write lock
*
* @param  :
*
* @warning:no body will clean the lock file on tmp*!!
*
* @return :
*
*/

#if WIN32/* MKIT_WIN32 */
#define rw_lock_create(name)
#define rw_lock_destroy(lock , name)
#define rw_lock_try(lock, cmd, type)
#define rw_lock_read(lock)
#define rw_lock_write(lock)
#define rw_lock_try_read(lock)
#define rw_lock_try_write(lock)
#define rw_lock_free(lock)
#else
static inline int rw_lock_create(const char * name)
{
    char path[PATH_MAX];
    snprintf(path, sizeof(path) - 1, TEMP_PATH "ipc_rw_lock_%s", name);
    return open(path, O_CREAT | O_RDWR | O_TRUNC);
}

static inline void rw_lock_destroy(int lock, const char * name)
{
    if (lock > 0)
    {
        close(lock);
    }
    if (name)
    {
        unlink(name);
    }
}

static inline int rw_lock_try(int lock, int cmd, int type)
{
    struct flock rwlock;
    rwlock.l_type   = type;
    rwlock.l_start  = 0;
    rwlock.l_whence = SEEK_SET;
    rwlock.l_len    = 0;
    return fcntl(lock, cmd, &rwlock);
}

#define rw_lock_read(lock) \
    rw_lock_try(lock,F_SETLKW,F_RDLCK)

#define rw_lock_write(lock) \
    rw_lock_try(lock,F_SETLKW,F_WRLCK)

#define rw_lock_try_read(lock) \
    rw_lock_try(lock,F_SETLK,F_RDLCK)

#define rw_lock_try_write(lock) \
    rw_lock_try(lock,F_SETLK,F_WRLCK)

#define rw_lock_free(lock) \
    rw_lock_try(lock,F_SETLK,F_UNLCK)
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __MK_HELPER_H__ */
