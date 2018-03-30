/********************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:linlh@yealink.com
*
*    HISTORY:
*
*    DATE:2017-11-22
*
*******************************************************************/

#ifndef __DURL_TOOL_H__
#define __DURL_TOOL_H__

/*******************************************************************
*    WARNING:must include durl.h before the SFB function call!
*            if no, will be unpredictable errors!
*******************************************************************/

/*
*----------------------------------------------------------------------
* DURL option module
*----------------------------------------------------------------------
*/
#define DURL_DHCP_OPTION_15     15      /* DHCP option 15 */
#define DURL_DHCP_OPTION_43     43      /* DHCP option 43 */
#define DURL_DHCP_OPTION_120    120     /* DHCP option 120 */

#ifdef __cplusplus
extern "C" {
#endif
/*
*----------------------------------------------------------------------
* durl tool APIs
*----------------------------------------------------------------------
*/

/**
* @brief  :trans dhcp option into string
*
* @param  :option,DHCP option id(According to the macro definition)
*          input,origin option string
*          buf,return the converted string
*          buf_size,size of converted string
*
* @warning:
*
* @return :0 as success,otherwise failed.
*
*/
DURL_API int durl_dhcp_option_to_string(int option, const char *input, char *buf, size_t buf_size);

/**
* @brief  :get a random number, and calculated uuid(Universally Unique Identifier)
*
* @param  :buf,input nput an array, it will return a uuid
*          buf_size,Buffer size
*
* @warning:The buf size must more than or equal to 64
*
* @return :0 as success,otherwise failed.
*
*/
DURL_API int durl_get_rand_uuid(char *buf, size_t buf_size);

/**
* @brief  :get http header line information
*
* @param  :header,use to parse
*          keyword,keyword use to match
*          buf,the matched offset pointer of header
*          buf_size,Buffer size
*
* @warning:
*
* @return :0 as success,otherwise failed.
*
*/
DURL_API int durl_get_http_header_line(const char *header, const char *keyword, char *buf, size_t buf_size);

/**
* @brief  :calculate the MD5 value
*
* @param  :input,Need to encrypt the string
*          buf,Encrypted string
*          buf_size,Buffer size
*
* @warning:
*
* @return :0 as success,otherwise failed.
*
*/
DURL_API int durl_md5_encrypt(const unsigned char *input, unsigned char *buf, size_t buf_size);

#ifdef __cplusplus
}
#endif

#endif /* END __DURL_TOOL_H__ */