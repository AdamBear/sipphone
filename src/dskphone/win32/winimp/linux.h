#pragma once

#ifdef __cplusplus
extern "C" {
#endif

__inline int fsync(int fd)
{
    return 0;
}
__inline int sync(void)
{
    return 0;
}

extern char * crypt(const char * key, const char * salt);

#   define bzero(addr, size) memset(addr, 0, size)
#   define __builtin_ffs(x) x
#   define __builtin_clz(x) x
#   define __builtin_ctz(x) x
#   define __builtin_popcount(x) x
#   define __builtin_parity(x) x
#   define mkdir(filename, mode) mkdir(filename)

extern FILE * popen(const char * filename, const char * flag);
extern int pclose(FILE * file);

extern int syscall(int, ...);
extern int ioctl(int fd, int iOperator, unsigned long uPoint);

extern char * strsep(char ** stringp, const char * delim);

extern int gettimeofday(struct timeval * tp, void * tzp);
extern size_t getline(char ** lineptr, size_t * n, FILE * stream);

extern unsigned int __inet_aton(const char * c_ipaddr, struct in_addr * ul_ipaddr);
extern char  * __inet_ntoa(struct in_addr in);

#define inet_aton __inet_aton
#define inet_ntoa __inet_ntoa

#ifndef strcasecmp
#   define strcasecmp stricmp
#endif
#ifndef strcasecmp
#   define strncasecmp  strnicmp
#endif

#define __NR_gettid 224
#define SYS_gettid  225

#ifdef __cplusplus
}
#endif
