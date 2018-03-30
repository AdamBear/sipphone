#ifndef __REJECT_H__
#define __REJECT_H__

#include "list.h"
#define MAX_IP_SIZE     48

typedef struct REJECT_IP_T
{
    int flag;
    int channel;
    char reject_ip[MAX_IP_SIZE];
    struct list_head   node;
} REJECT_IP;

int reject_create(REJECT_IP ** line);
int reject_destory(REJECT_IP * line);
int reject_reset(struct list_head * head);
int reject_add(struct list_head * head, REJECT_IP * line);
int reject_remove(struct list_head * head, REJECT_IP * line);
int reject_find(struct list_head * head, REJECT_IP ** line, char * ip, int iChannel);

#endif
