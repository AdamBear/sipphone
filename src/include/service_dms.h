/* ******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2016-01-02
*
* for export APIs header file , config serice
*
****************************************************************** */
#ifndef __SERVICE_DMS_H__
#define __SERVICE_DMS_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/
//#include <mkmessage.h>
//#include <mkservice.h>

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/************************************************************************/
/* basic message define                                                 */
/************************************************************************/
/***********************************************************************/
/* notification define id from 0x130000 to 0x1300ff (sent by dmServer)   */
/***********************************************************************/

/*
* usage   : to notify show message
* id      : 0
* target  : dmserver
* wparam  : duration(seconds)
* lparam  : none
* data_ptr: message buffer
* data_len: message buffer len
*/
#define DMS_MSG_NOTIFY_SHOW_MESSAGE           MKSRV_MSG(MKSRV_ID_DMS, 0)

/************************************************************************/
/* common define                                                        */
/************************************************************************/

typedef enum
{
    DMS_DATA_TYPE_NONE = -1,
    DMS_DATA_TYPE_JSON,
    DMS_DATA_TYPE_XML,
    DMS_DATA_TYPE_PLAIN,
    DMS_DATA_TYPE_MAX
}dms_data_type_e;

typedef struct
{
    char name[64];              /* alert name */
    char severity[16];          /* alert severity */
    char description[256];      /* alert description */
    unsigned int time;          /* alert action time */
    char module[16];            /* alert module name */
    dms_data_type_e data_type;  /* data type, plain/xml/json, etc. */
    char data[1024];            /* extern data */
}dms_alert_t;

#ifdef __cplusplus
}
#endif

#endif /* __SERVICE_DMS_H__ */

