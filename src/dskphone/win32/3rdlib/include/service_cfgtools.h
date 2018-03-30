/* ******************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:
*
*    HISTORY:
*
*    DATE:2015-01-29
*
* for export APIs header file , config serice
*
****************************************************************** */
#ifndef __SERVICE_CFGTOOLS_H__
#define __SERVICE_CFGTOOLS_H__

/************************************************************************/
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                                                                      */
/************************************************************************/

#ifdef WIN32
#define CFG_DLLEXPORT __declspec (dllexport)
#else
#define CFG_DLLEXPORT
#endif
#ifdef __cplusplus
extern "C"
{
#endif

/*
*----------------------------------------------------------------------
* micrp&limits
*----------------------------------------------------------------------
*/
#define CFG_PW_DATA_LEN             160

/*
*  cfg lrel error id
*/
#define CFG_LREL_OK                 0       /* Success */
#define CFG_LREL_ERR_IO             -1      /* IO failed */
#define CFG_LREL_ERR_MEM            -2      /* Abnormal memory */
#define CFG_LREL_ERR_PARAM          -3      /* param invalid */
#define CFG_LREL_ERR_UNSUPPORT      -4      /* method is not supported */
#define CFG_LREL_ERR_FORMAT         -5      /* file illegal */
/*
*----------------------------------------------------------------------
* type of cfg relevance
*----------------------------------------------------------------------
*/
/*
* cfg lrel for config
*/
typedef struct
{
    short   st_size;                    /* the structure size */
    char    tags[6];                    /* file tag */
    char    hw_version[7];              /* hardware version */
    char    sw_version[4];              /* software version */
} cfg_lrel_config_head_t;

/*
* cfg lrel for config
*/
typedef struct
{
    short   st_size;                    /* the structure size */

#define CFG_LREL_CRYPT_FLG_EN       0x01    /* encrypt file */
#define CFG_LREL_CRYPT_FLG_DE       0x02    /* decrypt file */
    char    flags;                      /* flags of encrypt/decrypt */

#define CFG_LREL_CRYPT_TYPE_AES     0x00    /* aes */
    char    type;                       /* encrypt/decrypt type */
    char  * key;                        /* encrypt/decrypt key, NULL use default */
    char  * in_file;                    /* need encrypt/decrypt file path */
    char  * out_file;                   /* encrypt/decrypt result file path */
} cfg_lrel_crypt_info_t;

/*
*----------------------------------------------------------------------
* types of callback
*----------------------------------------------------------------------
*/


/*
*-----------------------------------------------------------
*    public APIs
*-----------------------------------------------------------
*/

/*
*-----------------------------------------------------------
*   encrypt/decrypt APIs
*-----------------------------------------------------------
*/
/**
* @brief  : encrypt/decrypt password data
*
* @param  : src, input data to encrypt  /decrypt (MUST less than 100byte)
*           dest,output        decrpyted/encrypted data
*           privkey, private secret key
*
* @exp    : cfg_pw_data_encrypt(plaintext, ciphertext, NULL)
*           cfg_pw_data_decrypt(ciphertext, plaintext, NULL)
*           cfg_pw_data_encrypt(plaintext, ciphertext, "secret")
*           cfg_pw_data_decrypt(ciphertext, plaintext, "secret")
*
* @warning: if set privkey as NULL, means use internel secret key.
*
* @return : 0 as success,otherwise failed.
*
*/
CFG_DLLEXPORT    int cfg_pw_data_encrypt(const unsigned char * src,
        unsigned char dest[CFG_PW_DATA_LEN], const char * privkey);
CFG_DLLEXPORT    int cfg_pw_data_decrypt(const unsigned char * src,
        unsigned char dest[CFG_PW_DATA_LEN], const char * privkey);

/*
*-----------------------------------------------------------
*   relveance APIs, to encrypt/decrypt bin file
*-----------------------------------------------------------
*/
/**
* @brief  : encrypt and add head, wipe head and decrypt
*
* @param  : operate config info(ref cfg_lrel_crypt_info_t)
*
* @warning:
*
* @return : 0 is success,otherwise failed.
*
*/
CFG_DLLEXPORT    int cfg_drel_operate_config(cfg_lrel_crypt_info_t crypt_info,
        cfg_lrel_config_head_t config_head);

#ifdef WIN32

/*
*ret = 0 means success
*for xx2buf api, ret >= 0 means success, ret is the result len
*otherwise failed
*/
CFG_DLLEXPORT    int cfg_get_sfile_name(const char * original_file, char * out, int outlen);
CFG_DLLEXPORT    int cfg_sfile_encrypt(const char * infile, const char * outfile);
CFG_DLLEXPORT    int cfg_sfile_decrypt(const char * infile, const char * outfile);
CFG_DLLEXPORT    int cfg_sfile_decrypt2buf(const char * infile, char * out, int len);
CFG_DLLEXPORT    int cfg_sfile_encrypt2file(const char * in, int len, const char * outfile);
CFG_DLLEXPORT    int cfg_sfile_buf_decrypt2buf(const char * in, int len, char * out, int outlen);
CFG_DLLEXPORT    int cfg_sfile_buf_encrypt2buf(const char * in, int len, char * out, int outlen);
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* __SERVICE_CFGTOOLS_H__ */

