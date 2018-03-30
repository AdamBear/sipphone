/*******************************************************************
*   Copyright(c) 2007-2010 Xiamen Yealink Network Technology Co,.Ltd
*
*    DESCRIPTION: IPPGAING功能socket相关部分接口实现
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2011-5-13
*
*******************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>




#include "ippaging.h"
#include "reject.h"
#include "line.h"
#include "ippaging_com.h"

#ifdef WIN32
#include <io.h>
#   define sleep(time)      Sleep(1000 * (time))
#   define bzero(addr, size) memset(addr, 0, size);
#   define check_thread_id(id) (id.p)
#   define reset_thread_id(id) do{ id.p = 0; --id.x; }while(0)
#   define close(fd)    closesocket(fd)
#else
#include <error.h>
#   define check_thread_id(id) (id > 0)
#   define reset_thread_id(id) do{ id = -1; }while(0)
#endif

/*DEBUG等级*/
int  paging_use_debug = PAGING_DEBUG_LEVEL_DEBUG;

IPPAGING * ippaging = NULL;

unsigned short seq_nb = 0;

/*关闭recv和send两个线程使用的loop socket*/
static int close_loop_socket(void)
{
    if (ippaging->loop_send_fd > 0)
    {
        close(ippaging->loop_send_fd);
        ippaging->loop_send_fd = -1;
    }

    if (ippaging->loop_recv_fd > 0)
    {
        close(ippaging->loop_recv_fd);
        ippaging->loop_recv_fd = -1;
    }

    return 0;
}

/*创建recv和send两个线程使用的loop socket*/
static int create_loop_socket(void)
{
    int loop_send_fd = -1;
    int loop_recv_fd = -1;

    int opt = 0;

    struct sockaddr_in servaddr;

    opt = 1;
    if ((loop_send_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1
            || setsockopt(loop_send_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        PERR("socket loop_send_fd failed: ");
        return -1;
    }

    if ((loop_recv_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1
            || setsockopt(loop_recv_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        PERR("socket loop_recv_fd failed: ");
        return -1;
    }

    /*遍历LOOP_PORT_MIN->LOOP_PORT_MAX之间的可用端口*/
    for (ippaging->loop_send_port = LOOP_PORT_MIN; ippaging->loop_send_port <= LOOP_PORT_MAX;
            ippaging->loop_send_port++)
    {
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr =  htonl(INADDR_ANY);
        servaddr.sin_port = htons(ippaging->loop_send_port);
        if (bind(loop_send_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == 0)
        {
            DEBUG("ippaging->loop_send_port = %d\n", ippaging->loop_send_port);
            break;
        }
    }

    ippaging->loop_send_fd = loop_send_fd;

    for (ippaging->loop_recv_port = ippaging->loop_send_port + 1;
            ippaging->loop_recv_port <= LOOP_PORT_MAX; ippaging->loop_recv_port++)
    {
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr =  htonl(INADDR_ANY);
        servaddr.sin_port = htons(ippaging->loop_recv_port);
        if (bind(loop_recv_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == 0)
        {
            DEBUG("ippaging->loop_recv_port = %d\n", ippaging->loop_recv_port);
            break;
        }
    }

    ippaging->loop_recv_fd = loop_recv_fd;

    memset(&ippaging->loop_send_addr, 0, sizeof(struct sockaddr));
    ippaging->loop_send_addr.sin_family = AF_INET;
    ippaging->loop_send_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ippaging->loop_send_addr.sin_port = htons(ippaging->loop_send_port);

    memset(&ippaging->loop_recv_addr, 0, sizeof(struct sockaddr));
    ippaging->loop_recv_addr.sin_family = AF_INET;
    ippaging->loop_recv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    ippaging->loop_recv_addr.sin_port = htons(ippaging->loop_recv_port);

    return 0;
}

/*消息处理*/
static int msg_process(unsigned char * msg, int len, struct sockaddr_in * addr_client,
                       PAGING_LINE * line)
{
    char * request_ip_address = (char *)inet_ntoa(addr_client->sin_addr);
    unsigned char msg_vpm[MAX_MSG_SIZE] = "";
    int len_vpm = 0;
    int bRejectNotFind = 0;

    DEBUG("recv from group [%s], ip [%s], len = %d", line->remote_ip_address, request_ip_address, len);

    /*是否是RTP包*/
    if ((len > 20) && ((*msg == 0x80) || (*msg == 0x10)))
    {
        REJECT_IP * reject = NULL;

        int iChannel = 0;
        if ((*msg == 0x10))
        {
            iChannel = (*(msg + 1)) & 0x7f;
            iChannel = iChannel - 25;
        }

        //不同的组不接收
        if (line->channel != iChannel)
        {
            return 0;
        }

        bRejectNotFind = reject_find(&line->reject_group, &reject, request_ip_address, iChannel);
        if (!(line->flag & LINE_FL_START) && bRejectNotFind)
        {
            PAGING_PARAM param;
            memset(&param, 0, sizeof(PAGING_PARAM));
            strcpy(param.remote_ip_address, line->remote_ip_address);
            param.remote_udp_port = line->remote_udp_port;
            strcpy(param.request_ip_address, request_ip_address);
            param.paging_channel = iChannel;

            /*获取CODEC*/
            if (*msg == 0x80)
            {
                param.payload_type = (*(msg + 1)) & 0x7f;
            }
            else
            {
                param.payload_type = (*(msg + 20)) & 0x7f;
            }

            INFO("PAGING_EVENT_ID_START");
            INFO("param.payload_type = %d\n", param.payload_type);
            INFO("param.remote_ip_address = %s\n", param.remote_ip_address);
            INFO("param.remote_udp_port = %d\n", param.remote_udp_port);
            INFO("param.request_ip_address = %s\n", param.request_ip_address);
            INFO("param.paging_channel = %d\n", param.paging_channel);

            pthread_mutex_lock(&ippaging->lock);
            if (ippaging->do_event[PAGING_EVENT_ID_START])
            {
                ippaging->do_event[PAGING_EVENT_ID_START](&param);
            }

            line->flag |= LINE_FL_START;
            strcpy(line->paging_ip_address, request_ip_address);
            memset(line->hold_ip_address, 0, sizeof(line->hold_ip_address));
            pthread_mutex_unlock(&ippaging->lock);
        }

        pthread_mutex_lock(&ippaging->lock);

        if (!strcmp(request_ip_address, line->paging_ip_address)
                || (0 == strlen(line->paging_ip_address) && !strcmp(request_ip_address, line->hold_ip_address)))
        {
            line->flag |= LINE_FL_SENDING; /*RTP包正在发送*/
        }

        if (reject != NULL)
        {
            reject->flag |= LINE_FL_SENDING; /*RTP包正在发送*/
        }

        if ((line->flag & LINE_FL_PAGING)
                && !strcmp(request_ip_address, line->paging_ip_address))
        {
            /*paging PLCM*/
            if (*msg == 0x10)
            {
                len_vpm = (len - 26) / 2;
                msg_vpm[0] = 0x80;                      /*version*/
                msg_vpm[1] = (*(msg + 20)) & 0x7f;      /*payload type*/
                u_short *seq_p = &msg_vpm[2];
                *seq_p = htons(seq_nb);

                memcpy((msg_vpm + 4), (msg + 22), 4);   /*timestamp*/
                memcpy((msg_vpm + 8), (msg + 3), 4);    /*SSRC*/
                memcpy((msg_vpm + 12), (msg + 26), len_vpm); /*payload*/
                memcpy(msg, msg_vpm, MAX_MSG_SIZE);

                len = len_vpm + 12;
                seq_nb++;
            }
            if (sendto(ippaging->paging_in_fd, msg, len, 0, (struct sockaddr *)&ippaging->paging_in_addr,
                       sizeof(struct sockaddr_in)) < 0)
            {
                ERR("rtp paging failed.\n");
                pthread_mutex_unlock(&ippaging->lock);
                return -1;
            }
            else
            {
                DEBUG("paging to [%s] ok.", (char *)inet_ntoa(ippaging->paging_in_addr.sin_addr));
            }
        }

        pthread_mutex_unlock(&ippaging->lock);

    }

    return 0;
}

/*重置recv的fdr*/
static void fdr_reset(void)
{
    struct list_head * pos;
    PAGING_LINE * line;

    FD_ZERO(&ippaging->fdr);

    pthread_mutex_lock(&ippaging->lock);
    /*加入所有的fd*/
    list_for_each(pos, &ippaging->paging_group)
    {
        line = list_entry(pos, PAGING_LINE, node);
        FD_SET(line->sockfd, &ippaging->fdr);

        DEBUG("set socket to fdr: %d", line->sockfd);

        if (line->sockfd > ippaging->max_fd)
        {
            ippaging->max_fd = line->sockfd;
        }

        DEBUG("ippaging->max_fd = %d\n", ippaging->max_fd);
    }
    pthread_mutex_unlock(&ippaging->lock);

    DEBUG("set socket to fdr: %d", ippaging->loop_recv_fd);

    FD_SET(ippaging->loop_recv_fd, &ippaging->fdr);

    if (ippaging->loop_recv_fd > ippaging->max_fd)
    {
        ippaging->max_fd = ippaging->loop_recv_fd;
    }

    DEBUG("ippaging->max_fd = %d\n", ippaging->max_fd);
}

/*PAGING OUT的线程*/
static int send_thread(void * data)
{

    struct sockaddr_in servaddr, addr_client, remote_addr;

    int recv_sockfd = -1;
    int send_sockfd = -1;

    fd_set fdr;

    char msg[MAX_MSG_SIZE] = {0};
    int sockaddr_len = sizeof(struct sockaddr_in);
    int recv_len = 0;

    int ret = -1;
    int opt = 0;

    unsigned char msg_vpm[MAX_MSG_SIZE] = "";
    int len_vpm = 0;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (1)
    {
        FD_ZERO(&fdr);
        if (recv_sockfd > 0)
        {
            FD_SET(recv_sockfd, &fdr);
        }

        FD_SET(ippaging->loop_send_fd, &fdr);

        ret = select(99, &fdr, NULL, NULL, NULL);
        if (ret < 0)
        {
            PERR("select failed:");
            //return -1;
        }

        if (FD_ISSET(ippaging->loop_send_fd, &fdr))
        {
            DEBUG("ippaging->loop_send_fd is set.\n");

            if ((recv_len = recvfrom(ippaging->loop_send_fd, msg, MAX_MSG_SIZE, 0,
                                     (struct sockaddr *)&addr_client, &sockaddr_len)) < 0)
            {
                WARN("recv from loop fd failed.\n");
            }

            DEBUG("ippaging->paging_flag = %X\n", ippaging->paging_flag);

            /*START A PAGING OUT*/
            if (ippaging->paging_flag & PAGING_FL_OUT)
            {
                if (ippaging->send_param == NULL)
                {
                    ERR("ippaging->send_param is NULL at start paging out.");
                    sleep(5);
                    continue;
                }

                DEBUG("send_thread begin...\n");
                DEBUG("param->local_ip_address = %s\n", ippaging->send_param->local_ip_address);
                DEBUG("param->remote_ip_address = %d\n", ippaging->send_param->remote_ip_address);
                DEBUG("param->remote_udp_port = %d\n", ippaging->send_param->remote_udp_port);
                /*创建一个server socket 和 client socket*/

                opt = 1;
                if ((recv_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1
                        || setsockopt(recv_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
                {
                    PERR("socket recv_sockfd failed: ");
                    return -1;
                }

                memset(&servaddr, 0, sizeof(servaddr));
                servaddr.sin_family = AF_INET;
                servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
                servaddr.sin_port = htons(ippaging->send_param->local_udp_port);

                if (bind(recv_sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
                {
                    PERR("bind failed:");
                    return -1;
                }

                if ((send_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
                {
                    PERR("socket send_sockfd failed: ");
                    return -1;
                }

                memset(&remote_addr, 0, sizeof(remote_addr));
                remote_addr.sin_family = AF_INET;
                remote_addr.sin_addr.s_addr = inet_addr(ippaging->send_param->remote_ip_address);
                remote_addr.sin_port = htons(ippaging->send_param->remote_udp_port);
            }
            else
            {
                /*STOP A PAGING OUT*/

                if (recv_sockfd > 0)
                {
                    close(recv_sockfd);
                    recv_sockfd = -1;
                }

                if (send_sockfd > 0)
                {
                    close(send_sockfd);
                    send_sockfd = -1;

                }
            }

            continue;
        }

        if (FD_ISSET(recv_sockfd, &fdr))
        {
            if ((recv_len = recvfrom(recv_sockfd, msg, MAX_MSG_SIZE, 0, (struct sockaddr *)&addr_client,
                                     &sockaddr_len)) < 0)
            {
                WARN("recv from recv socket failed.\n");
                continue;
            }

            pthread_mutex_lock(&ippaging->lock);

            if (!ippaging->send_param)
            {
                WARN("ippaging->send_param is NULL");
                pthread_mutex_unlock(&ippaging->lock);
                continue;
            }

            if (strcmp((char *)inet_ntoa(addr_client.sin_addr), ippaging->send_param->local_ip_address))
            {
                INFO("from other client[%s]!=[%s].\n", (char *)inet_ntoa(addr_client.sin_addr),
                     (char *)ippaging->send_param->local_ip_address);
                pthread_mutex_unlock(&ippaging->lock);
                continue;
            }

            if (ippaging->send_param->paging_channel >= 1
                    && ippaging->send_param->paging_channel <= 30)
            {
                len_vpm = recv_len - 12;
                msg_vpm[0] = 0x10;                              /*version*/
                //msg_vpm[1] = 0x1a;                              /*group number*/
                msg_vpm[1] = ippaging->send_param->paging_channel + 25;
                msg_vpm[2] = 0xf2;
                msg_vpm[3] = 0x64;                              /*SSRC*/
                msg_vpm[4] = 0x25;
                msg_vpm[5] = 0xd6;
                msg_vpm[6] = 0x0d;
                memcpy((msg_vpm + 7), "YeaLink", 7);            /*paging send ID*/
                msg_vpm[20] = (*(msg + 1)) & 0x7f;              /*payload type*/
                msg_vpm[21] = 0;
                memcpy((msg_vpm + 22), (msg + 4), 4);           /*timestamp*/
                memcpy((msg_vpm + 26), (msg + 12), len_vpm);    /*payload1*/
                memcpy((msg_vpm + 26 + len_vpm), (msg + 12), len_vpm); /*payload2*/

                memcpy(msg, msg_vpm, MAX_MSG_SIZE);
                recv_len = len_vpm * 2 + 26;
            }

            pthread_mutex_unlock(&ippaging->lock);

            if (sendto(send_sockfd, msg, recv_len, 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr)) < 0)
            {
                WARN("rtp paging failed.");
                continue;
            }
            else
            {
                DEBUG("paging out ok.");
            }
        }

    }

    return 0;
}

static int monitor_thread(void * data)
{
    struct list_head * pos;
    struct list_head * sub_pos;
    PAGING_LINE * line;
    REJECT_IP * reject;

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (1)
    {
        sleep(1);

        pthread_mutex_lock(&ippaging->lock);
        list_for_each(pos, &ippaging->paging_group)
        {
            line = list_entry(pos, PAGING_LINE, node);
            if ((line->flag & LINE_FL_START) && !(line->flag & LINE_FL_SENDING))
            {
                PAGING_PARAM param;
                memset(&param, 0, sizeof(PAGING_PARAM));
                strcpy(param.remote_ip_address, line->remote_ip_address);
                param.remote_udp_port = line->remote_udp_port;
                param.paging_in = 1; /*paging in*/
                param.paging_channel = line->channel;
                if (ippaging->do_event[PAGING_EVENT_ID_STOP])
                {
                    ippaging->do_event[PAGING_EVENT_ID_STOP](&param);
                }
                line->flag &= ~LINE_FL_START;
                memset(line->hold_ip_address, 0, sizeof(line->hold_ip_address));
            }
            line->flag &= ~LINE_FL_SENDING; /*每秒清除SENDING标志位*/

            list_for_each(sub_pos, &line->reject_group)
            {
                reject = list_entry(sub_pos, REJECT_IP, node);
                if (!(reject->flag & LINE_FL_SENDING))
                {
                    reject_remove(&line->reject_group, reject);
                    reject_destory(reject);
                }
                else
                {
                    reject->flag &= ~LINE_FL_SENDING;
                }
            }
        }

        pthread_mutex_unlock(&ippaging->lock);
    }

    return 0;

}

static int recv_thread(void * data)
{
    struct list_head * pos;
    PAGING_LINE * line;
    int ret;
    int sockaddr_len = 0;
    int recv_len = 0;

    struct sockaddr_in  addr_client;

    unsigned char msg[MAX_MSG_SIZE] = "";

    DEBUG("recv_thread begin...\n");

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    while (1)
    {
        /*没有重新设置的话接收过其他fd之后就收不到ippaging->loop_recv_fd的数据*/
        fdr_reset();

        ret = select(ippaging->max_fd + 1, &ippaging->fdr, NULL, NULL, NULL);
        if (ret < 0)
        {
            PERR("err select");
            continue;
        }

        if (FD_ISSET(ippaging->loop_recv_fd, &ippaging->fdr))
        {
            DEBUG("ippaging->loop_recv_fd is set.\n");

            sockaddr_len = sizeof(struct sockaddr_in);
            if ((recv_len = recvfrom(ippaging->loop_recv_fd, msg, MAX_MSG_SIZE, 0,
                                     (struct sockaddr *)&addr_client, &sockaddr_len)) < 0)
            {
                WARN("recv from loop fd failed.\n");
            }
            continue;
        }

        pthread_mutex_lock(&ippaging->lock);

        /*循环判断哪个fd有数据可以读取*/
        list_for_each(pos, &ippaging->paging_group)
        {
            line = list_entry(pos, PAGING_LINE, node);
            if (FD_ISSET(line->sockfd, &ippaging->fdr))
            {
                sockaddr_len = sizeof(struct sockaddr_in);
                if ((recv_len = recvfrom(line->sockfd, msg, MAX_MSG_SIZE, 0, (struct sockaddr *)&addr_client,
                                         &sockaddr_len)) < 0)
                {
                    PERR("recv error");
                    continue;
                }
                pthread_mutex_unlock(&ippaging->lock);
                msg_process(msg, recv_len, &addr_client, line);
                pthread_mutex_lock(&ippaging->lock);
            }

        }

        pthread_mutex_unlock(&ippaging->lock);
    }

    return 0;
}

static int recv_thread_create(void)
{
    int ret;
    ret = pthread_create(&ippaging->recv_thread_id, 0, (void *)&recv_thread, NULL);
    if (ret < 0)
    {
        ERR("create recv thread failed.\n");
        return ret;
    }
    return 0;
}

static int recv_thread_destory(void)
{
    if (check_thread_id(ippaging->recv_thread_id))
    {
        pthread_cancel(ippaging->recv_thread_id);
        pthread_join(ippaging->recv_thread_id, NULL);
        reset_thread_id(ippaging->recv_thread_id);
    }
    return 0;
}

static int send_thread_create(void)
{
    int ret;

    ret = pthread_create(&ippaging->send_thread_id, 0, (void *)&send_thread, NULL);
    if (ret < 0)
    {
        ERR("create recv thread failed.\n");
        return ret;
    }

    return 0;
}

static int send_thread_destory(void)
{
    if (check_thread_id(ippaging->send_thread_id))
    {
        pthread_cancel(ippaging->send_thread_id);
        pthread_join(ippaging->send_thread_id, NULL);
        reset_thread_id(ippaging->send_thread_id);
    }

    return 0;
}


static int monitor_thread_create(void)
{
    int ret;

    ret = pthread_create(&ippaging->monitor_thread_id, 0, (void *)&monitor_thread, NULL);
    if (ret < 0)
    {
        ERR("create monitor thread failed.\n");
        return ret;
    }
    return 0;
}

static int monitor_thread_destory(void)
{
    if (check_thread_id(ippaging->monitor_thread_id))
    {
        pthread_cancel(ippaging->monitor_thread_id);
        pthread_join(ippaging->monitor_thread_id, NULL);
        reset_thread_id(ippaging->monitor_thread_id);
    }
    return 0;
}

/*set debug level*/
int paging_set_debug_level(int debug_level)
{
    paging_use_debug = debug_level;
    return 0;
}


/*initialize*/
int paging_init(void)
{
    /*未前只支持一个实例*/
    if (ippaging != NULL)
    {
        ERR("ipaging instance already exists.");
        return -1;
    }

    ippaging = malloc(sizeof(IPPAGING));

    if (ippaging == NULL)
    {
        ERR("malloc IPPAGING failed.\n");
        return -1;
    }

    memset(ippaging, 0, sizeof(IPPAGING));

    INIT_LIST_HEAD(&ippaging->paging_group);

    pthread_mutex_init(&ippaging->lock, NULL);

    monitor_thread_create();

    create_loop_socket();

    fdr_reset();
    recv_thread_create();
    send_thread_create();

    return 0;
}

/*destory*/
int paging_exit(void)
{
    if (ippaging == NULL)
    {
        ERR("IPPAGING NULL.");
        return -1;
    }

    pthread_mutex_lock(&ippaging->lock);

    monitor_thread_destory();

    close_loop_socket();

    recv_thread_destory();

    send_thread_destory();

    pthread_mutex_unlock(&ippaging->lock);

    if (ippaging != NULL)
    {
        free(ippaging);
        ippaging = NULL;
    }

    return 0;
}

/*
func: add a line to paging listen group
param:
    @paging_in - no use
    @local_ip_address - local ip address
    @local_udp_port -   no use
    @remote_ip_address - remote ip address
    @remote_udp_port -  listen port
return:
     0  -- SUCCESS
    <0  -- FAILED
*/
int paging_add_listen(PAGING_PARAM * param)
{
    int sockfd;
    struct sockaddr_in servaddr;
    PAGING_LINE * line;

    struct ip_mreq mreq;

    int opt = 0;

    if (NULL == param)
    {
        return -1;
    }

    pthread_mutex_lock(&ippaging->lock);
    if (line_find(&ippaging->paging_group, &line, param) == 0)
    {
        INFO("this paging line is in group already.");
        pthread_mutex_unlock(&ippaging->lock);
        return -1;
    }
    pthread_mutex_unlock(&ippaging->lock);

    if (line_create(&line))
    {
        ERR("create line failed.");
        return -1;
    }

    strcpy(line->local_ip_address, param->local_ip_address);
    line->local_udp_port = param->local_udp_port;
    strcpy(line->remote_ip_address, param->remote_ip_address);
    line->remote_udp_port = param->remote_udp_port;
    strcpy(line->remote_ip_address, param->remote_ip_address);
    line->channel = param->paging_channel;


    sockfd  =   socket(AF_INET, SOCK_DGRAM, 0);
    opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(line->remote_ip_address); //htonl(INADDR_ANY);
    servaddr.sin_port = htons(line->remote_udp_port);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))
    {
        PERR("bind failed");
        return -1;
    }

    line->sockfd = sockfd;

    INFO("add line:");
    INFO("line->sockfd = %d", line->sockfd);
    INFO("line->local_ip_address = %s", line->local_ip_address);
    INFO("line->local_udp_port = %d", line->local_udp_port);
    INFO("line->remote_ip_address = %s", line->remote_ip_address);
    INFO("line->remote_udp_port = %d", line->remote_udp_port);
    INFO("line->channel = %d", line->channel);

    mreq.imr_multiaddr.s_addr = inet_addr(line->remote_ip_address);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (setsockopt(line->sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        PERR("setsockopt: IP_ADD_MEMBERSHIP");
        return -1;
    }

    pthread_mutex_lock(&ippaging->lock);

    line_add(&ippaging->paging_group, line);

    pthread_mutex_unlock(&ippaging->lock);

    if (sendto(ippaging->loop_recv_fd, "continue", 8, 0, (struct sockaddr *)&ippaging->loop_recv_addr,
               sizeof(ippaging->loop_recv_addr)) < 0)
    {
        ERR("send continue to loop thread failed.\n");
        return -1;
    }

    return 0;
}

/*
func: remove a line from paging listen group
param:
    @paging_in - no use
    @local_ip_address - local ip address
    @local_udp_port -   no use
    @remote_ip_address - remote ip address
    @remote_udp_port    listen port for remote access
return:
     0  -- SUCCESS
    <0  -- FAILED
*/
int paging_remove_listen(PAGING_PARAM * param)
{
    PAGING_LINE * line;

    pthread_mutex_lock(&ippaging->lock);

    if (line_find(&ippaging->paging_group, &line, param))
    {
        WARN("this line is not exist.");
        pthread_mutex_unlock(&ippaging->lock);
        return -1;
    }
    /*remove from paging_line*/
    line_remove(&ippaging->paging_group, line);

    if (line->sockfd > 0)
    {
        close(line->sockfd);
        line->sockfd = -1;
    }

    line_destory(line);
    pthread_mutex_unlock(&ippaging->lock);

    if (sendto(ippaging->loop_recv_fd, "continue", 8, 0, (struct sockaddr *)&ippaging->loop_recv_addr,
               sizeof(ippaging->loop_recv_addr)) < 0)
    {
        ERR("send continue to loop thread failed.\n");
        return -1;
    }

    return 0;
}

/*
func: set event callback
param:
    @paging_event   - event id
    @cb             - event callback

return:
     0  -- SUCCESS
    <0  -- FAILED
*/
int paging_set_event_callback(unsigned int paging_event, PAGING_EVENT_CB cb)
{
    if (paging_event > PAGING_EVENT_ID_MIN && paging_event < PAGING_EVENT_ID_MAX)
    {
        pthread_mutex_lock(&ippaging->lock);
        ippaging->do_event[paging_event] = cb;
        pthread_mutex_unlock(&ippaging->lock);
        return 0;
    }
    return -1;
}

/*
func: start a paging
param:
    @paging_in - 0=paging out, 1=paging in
    @local_ip_address - local ip address
    @local_udp_port -   udp port(listen port for paging out, send port for paging in)
    @remote_ip_address - remote ip address
    @remote_udp_port     udp port for remote access
return:
     0  -- SUCCESS
    <0  -- FAILED
*/
int paging_start(PAGING_PARAM * param)
{
    PAGING_LINE * line;
    struct sockaddr_in bind_addr, remote_addr;

    if (param == NULL)
    {
        ERR("failed in paging start: PAGING_PARAM is NULL.");
        return -1;
    }

    DEBUG("paging_start:");
    DEBUG("param->paging_in = %d", param->paging_in);
    DEBUG("param->local_ip_address = %s", param->local_ip_address);
    DEBUG("param->local_udp_port = %d", param->local_udp_port);
    DEBUG("param->remote_ip_address = %s", param->remote_ip_address);
    DEBUG("param->remote_udp_port = %d", param->remote_udp_port);

    pthread_mutex_lock(&ippaging->lock);

    if (ippaging->paging_flag & (PAGING_FL_IN | PAGING_FL_OUT))
    {
        ERR("Paging already exists.");
        pthread_mutex_unlock(&ippaging->lock);
        return -1;
    }
    if (param->paging_in)
    {
        ippaging->paging_flag |= PAGING_FL_IN;

        if (line_find(&ippaging->paging_group, &line, param))
        {
            ERR("listen line not exist.");
            pthread_mutex_unlock(&ippaging->lock);
            return -1;
        }

        /**/
        ippaging->paging_in_fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (ippaging->paging_in_fd < 0)
        {
            pthread_mutex_unlock(&ippaging->lock);
            ERR("create socket failed.");
            return -1;
        }

        memset(&ippaging->paging_in_addr, 0, sizeof(struct sockaddr_in));

        ippaging->paging_in_addr.sin_family = AF_INET;

        ippaging->paging_in_addr.sin_addr.s_addr = inet_addr(param->local_ip_address);
        ippaging->paging_in_addr.sin_port = htons(param->local_udp_port);

        bind_addr.sin_family = AF_INET;
        bind_addr.sin_addr.s_addr = inet_addr(param->local_ip_address);
        bind_addr.sin_port = htons(param->local_bind_port);
        bind(ippaging->paging_in_fd, (struct sockaddr *)&bind_addr, sizeof(bind_addr));

        //strcpy(line->remote_ip_address, param->remote_ip_address);
        //line->remote_udp_port = param->remote_udp_port;
        strcpy(line->paging_ip_address, param->request_ip_address);
        memset(line->hold_ip_address, 0, sizeof(line->hold_ip_address));

        line->flag |= LINE_FL_PAGING;
    }
    else
    {

        ippaging->paging_flag |= PAGING_FL_OUT;

        DEBUG("#ippaging->paging_flag = %X\n", ippaging->paging_flag);

        if (ippaging->send_param != NULL)
        {
            ERR("Paging out already exists.");
            pthread_mutex_unlock(&ippaging->lock);
            return -1;
        }

        ippaging->send_param = malloc(sizeof(PAGING_PARAM));
        if (ippaging->send_param == NULL)
        {
            ERR("malloc PAGING_PARAM failed.\n");
            pthread_mutex_unlock(&ippaging->lock);
            return -1;
        }

        memcpy(ippaging->send_param, param, sizeof(PAGING_PARAM));

        if (sendto(ippaging->loop_send_fd, "continue", 8, 0, (struct sockaddr *)&ippaging->loop_send_addr,
                   sizeof(ippaging->loop_send_addr)) < 0)
        {
            ERR("send continue to loop thread failed.\n");
            pthread_mutex_unlock(&ippaging->lock);
            return -1;
        }

        if (param->paging_channel >= 1
                && param->paging_channel <= 30)
        {
            int req_sockfd = -1;
            unsigned char msg_req[24] = "";
            int i;

            req_sockfd = socket(AF_INET, SOCK_DGRAM, 0);

            memset(&remote_addr, 0, sizeof(remote_addr));
            remote_addr.sin_family = AF_INET;
            remote_addr.sin_addr.s_addr = inet_addr(ippaging->send_param->remote_ip_address);
            remote_addr.sin_port = htons(ippaging->send_param->remote_udp_port);

            msg_req[0] = 0x0f;                              /*version*/
            //msg_req[1] = 0x1a;                              /*group number*/
            msg_req[1] = param->paging_channel + 25;
            msg_req[2] = 0xf2;
            msg_req[3] = 0x64;                              /*SSRC*/
            msg_req[4] = 0x25;
            msg_req[5] = 0xd6;
            msg_req[6] = 0x0d;
            memcpy((msg_req + 7), "YeaLink", 7);            /*paging send ID*/
            for (i = 0; i < 31; i++)
            {
                if (sendto(req_sockfd, msg_req, 20, 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr)) < 0)
                {
                    WARN("PLCM request paging failed.");
                }
                else
                {
                    DEBUG("paging req ok.");
                }
            }
            if (req_sockfd > 0)
            {
                close(req_sockfd);
                req_sockfd = -1;
            }
        }
    }
    pthread_mutex_unlock(&ippaging->lock);

    return 0;
}

/*
func: stop a paging
param:
    @paging_in - 0=paging out, 1=paging in
    @local_ip_address - local ip address
    @local_udp_port -   udp port(listen port for paging out, send port for paging in)
    @remote_ip_address - remote ip address
    @remote_udp_port    remote udp port
return:
     0  -- SUCCESS
    <0  -- FAILED
*/
int paging_stop(PAGING_PARAM * param, STOPPAGE eType)
{
    PAGING_LINE * line = NULL;
    REJECT_IP * reject;

    DEBUG("paging_stop :");
    seq_nb = 0;
    pthread_mutex_lock(&ippaging->lock);

    if (!(ippaging->paging_flag & (PAGING_FL_IN | PAGING_FL_OUT)))
    {
        ERR("Paging no exists.");
        pthread_mutex_unlock(&ippaging->lock);
        return -1;
    }

    ippaging->paging_flag = 0;/*清除PAGING标记*/

    if (param->paging_in)
    {

        if (line_find(&ippaging->paging_group, &line, param))
        {
            ERR("line not exist.\n");
            // 此处需要解锁
            pthread_mutex_unlock(&ippaging->lock);
            return -1;
        }

        line->flag &= ~LINE_FL_PAGING;

        if (SP_HOLD == eType)
        {
            strcpy(line->hold_ip_address, line->paging_ip_address);
        }
        else if (SP_REJECT == eType)
        {
            if (0 == strlen(line->paging_ip_address))
            {
                strcpy(line->paging_ip_address, line->hold_ip_address);
            }
            if (reject_find(&line->reject_group, &reject, line->paging_ip_address, line->channel))
            {
                reject_create(&reject);
                DEBUG("paging_reject add:%s", line->paging_ip_address);
                strcpy(reject->reject_ip, line->paging_ip_address);
                reject->flag |= LINE_FL_SENDING;
                reject->channel = line->channel;
                reject_add(&line->reject_group, reject);
            }
            reject_find(&line->reject_group, &reject, line->paging_ip_address, line->channel);
        }

        memset(line->paging_ip_address, 0, sizeof(line->paging_ip_address));

        if (ippaging->paging_in_fd > 0)
        {
            close(ippaging->paging_in_fd);
            ippaging->paging_in_fd = -1;
        }

    }
    else
    {
        if (ippaging->send_param == NULL)
        {
            ERR("Paging out no exists.");
            // 此处应该是解锁
            pthread_mutex_unlock(&ippaging->lock);
            return -1;
        }

        if (sendto(ippaging->loop_send_fd, "continue", 8, 0, (struct sockaddr *)&ippaging->loop_send_addr,
                   sizeof(ippaging->loop_send_addr)) < 0)
        {
            ERR("send continue to loop thread failed.\n");
            pthread_mutex_unlock(&ippaging->lock);
            return -1;
        }

        free(ippaging->send_param);

        ippaging->send_param = NULL;
    }

    pthread_mutex_unlock(&ippaging->lock);

    return 0;
}


void paging_display_info(void)
{
    struct list_head * pos;
    PAGING_LINE * line;

    int index = 0;
    printf("\n---------------- --------------------------------------------\n");
    printf("==paging group=====\n");

    pthread_mutex_lock(&ippaging->lock);

    index = 0;
    list_for_each(pos, &ippaging->paging_group)
    {
        line = list_entry(pos, PAGING_LINE, node);
        printf("-----------------------\nLINE %d\n", index);
        printf("line->flag = %X\n", line->flag);
        printf("line->sockfd = %d\n", line->sockfd);
        printf("line->local_ip_address = %s\n", line->local_ip_address);
        printf("line->local_udp_port = %d\n", line->local_udp_port);
        printf("line->remote_ip_address = %s\n", line->remote_ip_address);
        printf("line->remote_udp_port = %d\n", line->remote_udp_port);
        printf("line->paging_ip_address = %s\n", line->paging_ip_address);
        index++;
    }

    pthread_mutex_unlock(&ippaging->lock);

    printf("=====PAGING IN=======\n");

    pthread_mutex_lock(&ippaging->lock);

    list_for_each(pos, &ippaging->paging_group)
    {
        line = list_entry(pos, PAGING_LINE, node);
        if (line->flag & LINE_FL_PAGING)
        {
            printf("-----------------------\nPAGING IN %d\n", index);
            printf("line->flag = %X\n", line->flag);
            printf("line->sockfd = %d\n", line->sockfd);
            printf("line->local_ip_address = %s\n", line->local_ip_address);
            printf("line->local_udp_port = %d\n", line->local_udp_port);
            printf("line->remote_ip_address = %s\n", line->remote_ip_address);
            printf("line->remote_udp_port = %d\n", line->remote_udp_port);
            printf("line->paging_ip_address = %s\n", line->paging_ip_address);
        }
    }

    pthread_mutex_unlock(&ippaging->lock);

    printf("=====PAGING OUT=======\n");


    if (ippaging->send_param)
    {
        printf("line->local_ip_address = %s\n", ippaging->send_param->local_ip_address);
        printf("line->local_udp_port = %d\n", ippaging->send_param->local_udp_port);
        printf("line->remote_ip_address = %s\n", ippaging->send_param->remote_ip_address);
        printf("line->remote_udp_port = %d\n", ippaging->send_param->remote_udp_port);

    }
    printf("\n------------------------------------------------------------\n");
}
