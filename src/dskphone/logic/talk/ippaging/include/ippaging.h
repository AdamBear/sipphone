/*******************************************************************
*   Copyright(c) 2007-2010 Xiamen Yealink Network Technology Co,.Ltd
*
*    DESCRIPTION: IPPGAING功能socket相关部分接口
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2011-5-13
*
*******************************************************************/
#ifndef __IPPAGING_H__
#define __IPPAGING_H__

#ifdef __cplusplus
extern "C"
{
#endif



typedef struct PAGING_PARAM_T
{
    unsigned int paging_in;             /*Paging in or Paging out*/
    unsigned int payload_type;          /*payload_type for codec*/
    int paging_channel;                 /*paging channel 1-25 Polycom*/
    char codec_name[16];                /*codec name*/

#define MAX_IP_SIZE     48
    char local_ip_address[MAX_IP_SIZE]; /*local ip address*/
    unsigned int local_udp_port;        /*local udp port*/
    char remote_ip_address[MAX_IP_SIZE];/*remote ip address*/
    unsigned int remote_udp_port;       /*remote udp port*/
    char request_ip_address[MAX_IP_SIZE];/*request paging ip, use in paging in*/
    unsigned int local_bind_port;  /*paging need bind local port == vpm remote port*/
} PAGING_PARAM;

typedef int (*PAGING_EVENT_CB)(PAGING_PARAM * param); /*callback interface for paging event */

typedef enum
{
    PAGING_DEBUG_LEVEL_ERR = 0x00,
    PAGING_DEBUG_LEVEL_WARN,
    PAGING_DEBUG_LEVEL_INFO,
    PAGING_DEBUG_LEVEL_DEBUG
} PAGING_DEBUG_LEVEL;

typedef enum
{
    SP_STOP = 0x00,
    SP_HOLD,
    SP_REJECT,
} STOPPAGE;

/*set debug level*/
int paging_set_debug_level(int debug_level);

/*initialize*/
int paging_init(void);

/*destory*/
int paging_exit(void);

/*
func: add a line to paging listen group
param:
    @local_ip_address - local ip address
    @remote_ip_address - remote ip address
    @remote_udp_port -  listen port
return:
     0  -- SUCCESS
    <0  -- FAILED
*/
int paging_add_listen(PAGING_PARAM * param);

/*
func: remove a line from paging listen group
param:
    @local_ip_address - local ip address
    @remote_ip_address - remote ip address
    @remote_udp_port    listen port
return:
     0  -- SUCCESS
    <0  -- FAILED
*/
int paging_remove_listen(PAGING_PARAM * param);


typedef enum
{
    PAGING_EVENT_ID_MIN = 0x00,
    PAGING_EVENT_ID_START,
    PAGING_EVENT_ID_STOP,
    PAGING_EVENT_ID_MAX
} PAGING_EVENT_ID;

/*
func: set event callback
param:
    @paging_event   - event id
    @cb             - event callback

return:
     0  -- SUCCESS
    <0  -- FAILED
*/
int paging_set_event_callback(unsigned int paging_event, PAGING_EVENT_CB cb);

/*
func: start a paging
     paging in: 在 remote_udp_port 监听 remote_ip_address的数据, 收到数据后转发到local_ip_address:local_udp_port
     paging out:在local_udp_port 监听 local_ip_address的数据，收到后转发到remote_ip_address:remote_udp_port
param:
    @paging_in - 0=paging out, 1=paging in
    @local_ip_address - local ip address
    @local_udp_port -   udp port(listen port for paging out, send port for paging in)
    @remote_ip_address - group ip address
    @remote_udp_port -  remote udp port
    @requeset_ip_address - ip of paging in request
return:
     0  -- SUCCESS
    <0  -- FAILED
*/
int paging_start(PAGING_PARAM * param);

/*
func: stop a paging
param:
    @paging_in - 0=paging out, 1=paging in
    @local_ip_address - local ip address
    @local_udp_port -   udp port(listen port for paging out, send port for paging in)
    @remote_ip_address - remote ip address
    @remote_udp_port    remote udp port
    @requeset_ip_address - ip of paging in request
return:
     0  -- SUCCESS
    <0  -- FAILED
*/
int paging_stop(PAGING_PARAM * param, STOPPAGE eType);

void paging_display_info(void);

#ifdef __cplusplus
}
#endif

#endif
