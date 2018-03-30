/*******************************************************************
*   Copyright(c) 2007-2010 Xiamen Yealink Network Technology Co,.Ltd
*
*    DESCRIPTION: IPPGAING line管理接口
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2011-5-13
*
*******************************************************************/
#ifndef __LINE_H__
#define __LINE_H__

#include "reject.h"
#include "ippaging.h"

typedef struct PAGING_LINE_T
{
    struct list_head   node;

#define LINE_FL_START   1
#define LINE_FL_REJECT  2
#define LINE_FL_SENDING 4
#define LINE_FL_PAGING  8
    int flag;
    int sockfd;
    int channel;

    char local_ip_address[MAX_IP_SIZE];
    int local_udp_port;
    char remote_ip_address[MAX_IP_SIZE];
    int remote_udp_port;
    char paging_ip_address[MAX_IP_SIZE];/*ip address of paging phone*/

    char hold_ip_address[MAX_IP_SIZE];
    struct list_head reject_group;

} PAGING_LINE;

int line_create(PAGING_LINE ** line);
int line_destory(PAGING_LINE * line);
int line_reset(struct list_head * head);
int line_add(struct list_head * head, PAGING_LINE * line);
int line_remove(struct list_head * head, PAGING_LINE * line);
int line_find(struct list_head * head, PAGING_LINE ** line, PAGING_PARAM * param);
int line_find_address(struct list_head * head, const char * strIP, int iPort);

#endif
