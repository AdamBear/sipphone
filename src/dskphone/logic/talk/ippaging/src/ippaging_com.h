/*******************************************************************
*   Copyright(c) 2007-2010 Xiamen Yealink Network Technology Co,.Ltd
*
*    DESCRIPTION: IPPGAING功能内部接口及定义
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2011-5-16
*
*******************************************************************/
#ifndef __IPPAGING_COM_H__
#define __IPPAGING_COM_H__

#include <pthread.h>

#define MAX_MSG_SIZE    1024
#define LOOP_PORT_MIN   65000
#define LOOP_PORT_MAX   65534


typedef struct IPPAGING_T
{
    int max_fd;
    fd_set fdr;

#define PAGING_FL_NONE  0
#define PAGING_FL_IN    1
#define PAGING_FL_OUT   2
    int paging_flag;

    int loop_recv_fd;
    int loop_recv_port;
    struct sockaddr_in loop_recv_addr;

    int loop_send_fd;
    int loop_send_port;
    struct sockaddr_in loop_send_addr;

    int paging_in_fd;
    struct sockaddr_in paging_in_addr;

    pthread_mutex_t lock;
    pthread_t recv_thread_id;
    pthread_t send_thread_id;
    pthread_t monitor_thread_id;

    struct list_head paging_group;

    PAGING_PARAM * send_param;
    PAGING_EVENT_CB do_event[PAGING_EVENT_ID_MAX];
} IPPAGING;

extern int paging_use_debug;

#if DEBUG_ALL
#define DEBUG(fmt,args...)   printf("ippaging[%s:%4d]-DEBUG-"fmt"\n",__FUNCTION__,__LINE__,##args)
#define INFO(fmt,args...)    printf("ippaging[%s:%4d]-INFO-"fmt"\n",__FUNCTION__,__LINE__,##args)
#define WARN(fmt,args...)    printf("\033[1;33mippaging[%s:%4d]-WARN-"fmt"\033[0m\n",__FUNCTION__,__LINE__,##args)
#define ERR(fmt,args...)     printf("\033[1;31mippaging[%s:%4d]-ERR-"fmt"\033[0m\n",__FUNCTION__,__LINE__,##args)
#define PERR(fmt)            perror("\033[1;31m[IPPAGING-PERR]:"fmt"\033[0m:")
#elif defined(WIN32)
#include "dsklog/log.h"
#define DEBUG(fmt,...) WIN_DEBUG(fmt, ##__VA_ARGS__)
#define INFO(fmt,...) WIN_INFO(fmt, ##__VA_ARGS__)
#define WARN(fmt,...) WIN_WARN(fmt, ##__VA_ARGS__)
#define ERR(fmt,...) WIN_ERR(fmt, ##__VA_ARGS__)
#define PERR(fmt) WIN_ERR(fmt, ##__VA_ARGS__)
#else
#define DEBUG(fmt,args...)   do{if(paging_use_debug>=PAGING_DEBUG_LEVEL_DEBUG)printf("[IPPAGING-DEBUG]:"fmt"\n",##args);}while(0)
#define INFO(fmt,args...)    do{if(paging_use_debug>=PAGING_DEBUG_LEVEL_INFO) printf("[IPPAGING-INFO]:"fmt"\n",##args);}while(0)
#define WARN(fmt,args...)    do{if(paging_use_debug>=PAGING_DEBUG_LEVEL_WARN) printf("\033[1;33m[IPPAGING[%s:%4d]-WARN]:"fmt"\033[0m\n",__FUNCTION__,__LINE__,##args);}while(0)
#define ERR(fmt,args...)     do{if(paging_use_debug>=PAGING_DEBUG_LEVEL_ERR)  printf("\033[1;31m[IPPAGING[%s:%4d]-ERR]:"fmt"\033[0m\n",__FUNCTION__,__LINE__,##args);}while(0)
#define PERR(fmt)            do{if(paging_use_debug>=PAGING_DEBUG_LEVEL_ERR)  perror("\033[1;31m[IPPAGING-PERR]:"fmt"\033[0m:");}while(0)
#endif

#endif
