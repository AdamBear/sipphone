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

#include "line.h"


int line_create(PAGING_LINE ** line)
{
    PAGING_LINE * new_line;

    /*alloc memory*/
    new_line = calloc(1, sizeof(PAGING_LINE));
    if (new_line == NULL)
    {
        printf("not mem!\n");
        return -1;
    }

    memset(new_line, 0, sizeof(PAGING_LINE));

    INIT_LIST_HEAD(&new_line->reject_group);

    /*DONE*/
    *line = new_line;

    return 0;
}

int line_destory(PAGING_LINE * line)
{
    struct list_head * pos;
    list_for_each(pos, &line->reject_group)
    {
        REJECT_IP * reject;
        reject = list_entry(pos, REJECT_IP, node);
        reject_remove(&line->reject_group, reject);
        reject_destory(reject);
    }

    free(line);
    line = NULL;

    return 0;
}

int line_add(struct list_head * head, PAGING_LINE * line)
{
    if (NULL == line)
    {
        printf("line is NULL");
        return -1;
    }

    list_add(&(line->node), head);
    return 0;
}

int line_remove(struct list_head * head, PAGING_LINE * line)
{
    list_del(&(line->node));
    return 0;
}

int line_find(struct list_head * head, PAGING_LINE ** line, PAGING_PARAM * param)
{
    struct list_head * pos;
    PAGING_LINE * new_line;

    if (head == NULL || head->next == NULL || head->next == head)
    {
        printf("line group is empty.\n");
        return -1;
    }
    list_for_each(pos, head)
    {
        new_line = list_entry(pos, PAGING_LINE, node);
        if (strcmp(new_line->remote_ip_address, param->remote_ip_address) == 0
                && new_line->remote_udp_port == param->remote_udp_port
                && param->paging_channel == new_line->channel)
        {
            printf("find group_ip_address[%s],remote_udp_port[%d] iChannel[%d] in group.\n",
                   param->remote_ip_address, param->remote_udp_port, param->paging_channel);
            *line = new_line;
            return 0;
        }
    }

    *line = NULL;
    /*not found*/
    return -1;
}

int line_find_address(struct list_head * head, const char * strIP, int iPort)
{
    struct list_head * pos;
    PAGING_LINE * new_line;

    if (head == NULL || head->next == NULL || head->next == head)
    {
        printf("line group is empty.\n");
        return -1;
    }
    list_for_each(pos, head)
    {
        new_line = list_entry(pos, PAGING_LINE, node);
        if (strcmp(new_line->remote_ip_address, strIP) == 0
                && new_line->remote_udp_port == iPort)
        {
            return 0;
        }
    }
    return -1;
}
