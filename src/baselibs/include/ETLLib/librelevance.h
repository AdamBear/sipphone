/*******************************************************************
*
*    DESCRIPTION:Copyright(c) 2014-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:      2014.08.21
*
*******************************************************************/
#ifndef __LIB_RELEVANCE_H__
#define __LIB_RELEVANCE_H__

/*
* for std
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
*  lrel error id
*/
#define LREL_OK                  0      /* Success */
#define LREL_ERR_IO             -1      /* IO failed */
#define LREL_ERR_MEM            -2      /* Abnormal memory */
#define LREL_ERR_PARAM          -3      /* param invalid */
#define LREL_ERR_UNSUPPORT      -4      /* method is not supported */
#define LREL_ERR_FORMAT         -5      /* file illegal */

/*
* lrel for config
*/
typedef struct
{
    short   st_size;                    /* the structure size */
    char    tags[6];                    /* file tag */
    char    hw_version[7];              /* hardware version */
    char    sw_version[4];              /* software version */
} lrel_config_head_t;

/*
* lrel for config
*/
typedef struct
{
    short   st_size;                    /* the structure size */

#define LREL_CRYPT_FLG_EN       0x01    /* encrypt file */
#define LREL_CRYPT_FLG_DE       0x02    /* decrypt file */
    char    flags;                      /* flags of encrypt/decrypt */

#define LREL_CRYPT_TYPE_AES     0x00    /* aes */
    char    type;                       /* encrypt/decrypt type */
    char  * key;                        /* encrypt/decrypt key, NULL use default */
    char  * in_file;                    /* need encrypt/decrypt file path */
    char  * out_file;                   /* encrypt/decrypt result file path */
} lrel_crypt_info_t;

#ifdef __cplusplus
extern "C"
{
#endif
/************************************************************************/
/*                                                                      */
/************************************************************************/
/*
* APIs for config:
*/

/**
* @brief  : encrypt and add head, wipe head and decrypt
*
* @param  : operate config info(ref lrel_crypt_info_t)
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
int drel_operate_config(lrel_crypt_info_t crypt_info, lrel_config_head_t config_head);

#ifdef __cplusplus
}
#endif

#endif/* __LIB_RELEVANCE_H__ */

