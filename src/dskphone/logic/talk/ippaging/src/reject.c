/*******************************************************************
*   Copyright(c) 2007-2010 Xiamen Yealink Network Technology Co,.Ltd
*
*    DESCRIPTION: IPPGAING line管理接口实现
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2011-5-13
*
*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reject.h"


int reject_create(REJECT_IP ** reject)
{
    REJECT_IP * new_reject;

    /*alloc memory*/
    new_reject = calloc(1, sizeof(REJECT_IP));
    if (new_reject == NULL)
    {
        printf("not mem!\n");
        return -1;
    }

    memset(new_reject, 0, sizeof(REJECT_IP));

    /*DONE*/
    *reject = new_reject;

    return 0;
}

int reject_destory(REJECT_IP * reject)
{
    free(reject);
    reject = NULL;

    return 0;
}

int reject_add(struct list_head * head, REJECT_IP * reject)
{
    if (NULL == reject)
    {
        printf("reject is NULL");
        return -1;
    }

    list_add(&(reject->node), head);
    return 0;
}

int reject_remove(struct list_head * head, REJECT_IP * reject)
{
    list_del(&(reject->node));
    return 0;
}

int reject_find(struct list_head * head, REJECT_IP ** reject, char * ip, int iChannel)
{
    struct list_head * pos;
    REJECT_IP * new_reject;

    if (head == NULL || head->next == NULL || head->next == head)
    {
//      printf("reject group is empty.\n");
        return -1;
    }

    list_for_each(pos, head)
    {
        new_reject = list_entry(pos, REJECT_IP, node);

        if (strcmp(new_reject->reject_ip, ip) == 0
                && new_reject->channel == iChannel)
        {
//          printf("find new_reject->reject_ip[%s].\n", new_reject->reject_ip);
            *reject = new_reject;
            return 0;
        }
    }

    *reject = NULL;
    /*not found*/
    return -1;
}

