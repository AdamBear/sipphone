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
#ifndef __DECT_DH_H__
#define __DECT_DH_H__

#include "dect_msg.h"

/************************************************************************/
/*     for DH packet type                                              */
/************************************************************************/
//#pragma  pack(push,1)

/************************************************************************/
/*                                                                      */
/************************************************************************/

#define   DH_CMD_HELLO                                  0
#define   DH_CMD_INIT                                   1
#define   DH_CMD_INIT_RESPONSE                          2
#define   DH_CMD_EST_DATA_CALL                          3
#define   DH_CMD_EST_DATA_CALL_RESPONSE                 4
#define   DH_CMD_DISC_DATA_CALL                         5
#define   DH_CMD_DISC_DATA_CALL_RESPONSE                6
#define   DH_CMD_HS_EST_DATA_CALL                       7// HS established the data call link
#define   DH_CMD_HS_EST_DATA_CALL_RESPONSE              8
#define   DH_CMD_HS_DISC_DATA_CALL                      9// HS disconnected the data call link
#define   DH_CMD_HS_DISC_DATA_CALL_RESPONSE             10
#define   DH_CMD_DATA_READ                              11
#define   DH_CMD_DATA_READ_RESPONSE                     12
#define   DH_CMD_DATA_WRITE                             13
#define   DH_CMD_DATA_WRITE_RESPONSE                    14
#define   DH_CMD_DEINIT                                 15
#define   DH_CMD_DEINIT_RESPONSE                        16


#define   DH_RET_FAILURE                                -1
#define   DH_RET_ERR_HS_LINK_BUSY                       -30
#define   DH_RET_ERR_DATA_HANDLE_CLOSED                 -31
#define   DH_RET_ERR_SEND_BUSY                          -32
#define   DH_RET_ERR_DATA_HANDLE_EXSIT                  -33
#define   DH_RET_ERR_EST_TIME_OUT                       -34
#define   DH_RET_ERR_SEND_TIME_OUT                      -35
#define   DH_RET_ERR_DATA_HANDLE_DEINIT                 -36
#define   DH_RET_ERR_HS_NOT_ALIVE                       -37

#define   DH_HSMSG_MAX_HEADER_SIZE     48
#define   DH_HSMSG_MAX_RAW_SIZE        (1024)
#define   DH_HSMSG_MAX_DAT_SIZE        (DH_HSMSG_MAX_RAW_SIZE+8) /*add compress hdr.*/
#define   DH_HSMSG_MAX_SIZE            (DH_HSMSG_MAX_DAT_SIZE + DH_HSMSG_MAX_HEADER_SIZE)

#define   DH_MAX_DATA_PACKET_SIZE                        DH_HSMSG_MAX_SIZE
#define   DH_MAX_BODY_SIZE                               (DH_MAX_DATA_PACKET_SIZE + 128 /* sizeof header of DH_DATA_CALL_WRITE.*/) 
#define   DH_MAX_SIZE                                    (DH_MAX_BODY_SIZE + 16 /* sizeof(msg_hdr) */) 


#define   DH_INIT_MODE_1                                1
#define   DH_INIT_MODE_2                                2  /* not used now*/


typedef struct
{
    D_INT cmd;
    D_INT bearertype;
    D_INT handset;
    D_INT hs_mask;
    D_INT result;
    D_INT reason;
    D_WORD u16_length;
    D_BYTE pdata[64];
} DH_API_Result;


typedef struct {
    int handset;
    unsigned char bearertype;
} DH_DATA_CALL_EST;

typedef struct {
    int handset;
} DH_DATA_CALL_DISC;

typedef struct {
    int handset;
    int result;
    
    /* length of data buffer */
    D_WORD u16_len;
    D_WORD u16_reserved;
    D_BYTE pdata[DH_MAX_DATA_PACKET_SIZE];
} DH_DATA_CALL_WRITE;

typedef struct {
    int handset;
    int result;
    
    /* length of data buffer */
    D_WORD u16_len;
    D_WORD u16_reserved;
    D_BYTE pdata[DH_MAX_DATA_PACKET_SIZE];
} DH_DATA_CALL_READ;

typedef struct
{
    /* length of data buffer */
    D_WORD u16_len;
    D_BYTE pdata[64];
} DH_Generic;


/*
* dh body
*/
typedef union
{
  DH_Generic Generic;
  DH_API_Result Response;
  DH_DATA_CALL_EST Est;
  DH_DATA_CALL_DISC Disc;
  DH_DATA_CALL_WRITE Write;
  DH_DATA_CALL_READ Read;
} dh_body;


/*
* dh msg
*/
typedef struct
{
    msg_hdr  hdr;
    dh_body body;
} dh_msg;

/************************************************************************/
/*                                                                      */
/************************************************************************/
#endif /* __DECT_DH_H__ */
