/********************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:wangsc@yealink.com
*
*    HISTORY:
*
*    DATE:2017-11-22
*
*******************************************************************/
#ifndef __DURL_COMMON_H__
#define __DURL_COMMON_H__

/*
*----------------------------------------------------------------------
* API export
*----------------------------------------------------------------------
*/
#ifdef WIN32
#   define DURL_API __declspec(dllexport)
#else
#   define DURL_API __attribute((visibility("default")))
#endif

/*
*----------------------------------------------------------------------
* typedef local handle
*----------------------------------------------------------------------
*/
typedef unsigned long       durl_handle_t;

/*
*----------------------------------------------------------------------
* durl GLOBAL handle, if set, the APIs will work on global
*----------------------------------------------------------------------
*/
#define DURL_GLOBAL_HANDLE  0

/*
*----------------------------------------------------------------------
* DURL limit
*----------------------------------------------------------------------
*/
#define DURL_MAX_BUFFER_LEN     512     /* All the special significance of the length of the array */
#define DURL_MAX_USERINFO_LEN   256     /* The user information array maximum length */
#define DURL_MAX_URL_LEN        1024    /* The maximum length of url address */

/*
*----------------------------------------------------------------------
* Program perform result success
*----------------------------------------------------------------------
*/
#define DURL_SUCCESS            0       /* success */

/*
*----------------------------------------------------------------------
* Program perform result error code
*----------------------------------------------------------------------
*/
#define DURL_ERR_NONE                       0       /* 0 no error(success) */
#define DURL_ERR_UNDEFINED                  -1      /* -1 Undefined error */
#define DURL_ERR_PARAMETER                  -2      /* -2 Parameter error */
#define DURL_ERR_CREATE                     -3      /* -3 create error */
#define DURL_ERR_AUTHORIZED                 -4      /* -4 Unauthorized */
#define DURL_ERR_DISABLE_IPV6               -5      /* -5 ipv6 is disable */
#define DURL_ERR_INVALID_DATA               -6      /* -6 Invalid data */
#define DURL_ERR_PORT                       -7      /* -7 Port error */
#define DURL_ERR_HTTP                       -8      /* -8 FTP error */
#define DURL_ERR_FTP                        -9      /* -9 FTP error */
#define DURL_ERR_TFTP                       -10     /* -10 TFTP error */
#define DURL_ERR_TLS_VER                    -11     /* -11 TLS version error */
#define DURL_ERR_UNKNOW_PROTOCOL            -101    /* Unknow protocol */
#define DURL_ERR_UNINIT                     -102    /* Uninitialized */
#define DURL_ERR_URL_FORMAT                 -103    /* URL malformat */
#define DURL_ERR_COULDNT_RESOLVE_PROXY      -105    /* Proxy domain name resolution fail */
#define DURL_ERR_COULDNT_RESOLVE_HOST       -106    /* Domain name resolution fail */
#define DURL_ERR_COULDNT_CONNECT            -107    /* Connect error */
#define DURL_ERR_FTP_WRONG_PASSWD           -111    /* FTP wrong passwd */
#define DURL_ERR_NOFULLY                    -123    /* Recv nofully */
#define DURL_ERR_OUT_OF_MEMORY              -127    /* Out of memory */
#define DURL_ERR_OPERATION_TIMEOUT          -128    /* Operation timeout */
#define DURL_ERR_HTTP_POST                  -134    /* Http post fail */
#define DURL_ERR_SSL_CONNECT                -135    /* Wrong when connecting with SSL */
#define DURL_ERR_TOO_MANY_REDIRECTS         -147    /* Catch endless re-direct loops */
#define DURL_ERR_UNKNOW_OPTION              -148    /* undefined option */
#define DURL_ERR_PEER_FAILED_VERIFICATION   -151    /* peer's certificate or fingerprint wasn't verified fine */
#define DURL_ERR_GOT_NOTING                 -152    /* When this is a specific error */
#define DURL_ERR_SSL_ENGINE                 -153    /* SSL crypto engine not found */
#define DURL_ERR_SSL_SET_CRYPTO             -154    /* Can not set SSL crypto engine as default */
#define DURL_ERR_SEND                       -155    /* Failed sending network data */
#define DURL_ERR_RECV                       -156    /* Failure in receiving network data */
#define DURL_ERR_CLIENT_CERT                -158    /* Problem with the local certificate */
#define DURL_ERR_SSL_CIPHER                 -159    /* Couldn't use specified cipher */
#define DURL_ERR_SSL_CA_CERT                -160    /* Problem with the CA cert (path?) */
#define DURL_ERR_FILE_SIZE                  -163    /* Maximum file size exceeded */
#define DURL_ERR_LOGIN_DENIED               -167    /* User, password or similar was not accepted and we failed to login */
#define DURL_ERR_TFTP_PERM                  -169    /* Permission problem on TFTP server */
#define DURL_ERR_FTP_NOUSER                 -174    /* No such user */
#define DURL_ERR_SSL_CA_CERT_FILE           -177    /* Could not load CACERT file, missing or wrong format */
#define DURL_ERR_REMOTE_FILE_NOT_FOUND      -178    /* The resource referenced in the URL does not exist. */
#define DURL_ERR_SSL_SHUTDOWN               -180    /* Failed to shut down the SSL connection */
#define DURL_ERR_SOCKET_AGAIN               -181    /* Socket is not ready for send/recv,wait till it's ready and try again */
#define DURL_ERR_SSL_CRL_FILE               -182    /* Could not load CRL file, missing or wrong format */
#define DURL_ERR_SSL_ISSUER                 -183    /* Issuer check failed. */
#define DURL_ERR_NO_CONNECTION              -189    /* No connection available, the session will be queued */
#define DURL_ERR_PINNED_PUBKEY_NOT_MATCH    -190    /* Specified pinned public key did not match */
#define DURL_ERR_INVALID_CERT_STATUS        -191    /* Invalid certificate status */

/*
*----------------------------------------------------------------------
* HTTP server return code
*----------------------------------------------------------------------
*/
#define DURL_HTTP_OK_200        200    /* Request successful */
#define DURL_HTTP_OK_201        201    /* Create successful */

#define DURL_HTTP_ERR_301       301    /* Permanently Moved */
#define DURL_HTTP_ERR_302       302    /* Temporarily Moved */
#define DURL_HTTP_ERR_400       400    /* Invalid query */
#define DURL_HTTP_ERR_401       401    /* Unauthorized */
#define DURL_HTTP_ERR_403       403    /* Forbidden */
#define DURL_HTTP_ERR_408       408    /* Request Timeout */
#define DURL_HTTP_ERR_500       500    /* Internal Server Error */
#define DURL_HTTP_ERR_501       501    /* Not Implemented */
#define DURL_HTTP_ERR_503       503    /* Service Unavailable */

/*
*----------------------------------------------------------------------
* DURL Network security ssl versions and certificate
*----------------------------------------------------------------------
*/
/* SSL versions */
#define DURL_SSL_VER_TLSV1_0    0       /* tls1.0 */
#define DURL_SSL_VER_SSLV2      1       /* ssl2 */
#define DURL_SSL_VER_SSLV3      2       /* ssl3 */
#define DURL_SSL_VER_SSLV23     3       /* sslv23(default), Will match on its own, Negotiate with the server */
#define DURL_SSL_VER_TLSV1_1    4       /* tls1.1 */
#define DURL_SSL_VER_TLSV1_2    5       /* tls1.2 */
#define DURL_SSL_VER_TLSV1_3    6       /* tls1.3 */

/* certificate signature algorithm */
#define DURL_SSL_CERT_MD5       0       /* Message-Digest Algorithm 5 */
#define DURL_SSL_CERT_SHA_256   1       /* Secure Hash Algorithm 256 */
#define DURL_SSL_CERT_SHA_384   2       /* Secure Hash Algorithm 384 */
#define DURL_SSL_CERT_SHA_512   3       /* Secure Hash Algorithm 512 */

/*
*----------------------------------------------------------------------
* Structure member operation option
* This macro-mania to be used with durl_set_option or durl_get_option
*----------------------------------------------------------------------
*/
typedef enum                                    /* Enum id(Specify the input type)-detailed description */
{
    DURL_OPTION_USER_AGENT             = 0,     /* 0(string)-The user agent information */

    /* network */
    DURL_OPTION_MAC_ADDR               = 1,     /* 1(string)-MAC address */
    DURL_OPTION_SUPPORT_IPV6           = 2,     /* 2(int)-enable IPV6, 0 disable(default),1 enable */
    DURL_OPTION_NET_PREFERRED          = 3,     /* 3(int)-Network preferred, 0 IPV6(default), 1 IPV4 */

    /* The limit options */
    DURL_OPTION_CONNECT_TIMEOUT        = 4,     /* 4(int)-Connection timeout, 5sec(default), 1sec min, 300sec max, 0sec never timeout */
    DURL_OPTION_OPERART_TIMEOUT        = 5,     /* 5(int)-Operation timeout, 30sec(default), 1sec min, 300sec max, 0sec never timeout */
    DURL_OPTION_LOW_SPEED_TIMEOUT      = 6,     /* 6(int)-Low speed timeout, 0sec never timeout(default), 1sec min, 3000sec max */
    DURL_OPTION_LOW_SPEED_LIMIT        = 7,     /* 7(int)-Low speed limit, 100byte/s(default), 1byte/s min, none max */
    DURL_OPTION_MAX_REDIRECT           = 8,     /* 8(int)-The biggest redirection times£¬just http, 5(default), 0 min(no redirect), 20 max */
    DURL_OPTION_MAX_FILESIZE           = 9,     /* 9(unsigned long)-Transfer files maximum size, 1024 * 1024(default), 1 min, 5 * 1024 *1024 max */

    /* SSL options */
    DURL_OPTION_SSL_CIPHER_STR         = 10,    /* 10(string)-The encryption algorithm description, RSA:!EXP:!LOW:!NULL:!MD5:@STRENGTH(default) */
    DURL_OPTION_SSL_VERSION            = 11,    /* 11(int)-SSL version, Automatic matching(default), useing define(DURL_SSL_VER_xxx) */
    DURL_OPTION_SSL_VERIFY_PEER        = 12,    /* 12(int)-Check the server certificate, 0 no check(default) */
    DURL_OPTION_SSL_VERIFY_HOST        = 13,    /* 13(int)-Check the service side host, 0 no check(default) */
    DURL_OPTION_SSL_PEM_PASSWORD       = 14,    /* 14(string)-Certificate password */
    DURL_OPTION_SSL_CA_TRUST_PATH      = 15,    /* 15(string)-Trusted certificate path */
    DURL_OPTION_SSL_CA_TRUST_USE_PATH  = 16,    /* 16(string)-Trusted certificate path, used to check the server certificate(Used in the SSL handshake) */
    DURL_OPTION_SSL_CA_SERVER_FILE     = 17,    /* 17(string)-Local certificate, The server needs to check */
    DURL_OPTION_SSL_CA_SERVER_USE_FILE = 18,    /* 18(string)-Local certificate, The server needs to check(Used in the SSL handshake) */
    DURL_OPTION_SSL_CA_WEB_CERT_PATH   = 19,    /* 19(string)-sip register cert install path */
    DURL_OPTION_SSL_SIGNATURE_ALGO     = 20,    /* 20(int)-Telephone equipment certificate signature algorithm, useing define(DURL_SSL_CERT_xxx) */

    /* file or directory structure */
    DURL_OPTION_CONFIG_PATH_PREFIX     = 21,    /* 21(string)-A prefix path set the configuration file, For example.\\config\\,.\\confog_PART_A\\,.\\config_PART_B\\ */
    DURL_OPTION_TRANS_DATA_TRACE       = 22,    /* 22(int)-To transmit data to print */
    DURL_OPTION_RESOLV_NOSYS           = 23,    /* 23(int)-use host file to resolv, 0 use system(default), 1 use host file */
    DURL_OPTION_HOSTS_FILE             = 24,    /* 24(string)-Resole hosts file, /etc/hosts(default), WINDOWS no support */
    DURL_OPTION_RESOLV_FILE            = 25,    /* 25(string)-Set the DNS configuration file path, WINDOWS no support  */

    /* proxy options */
    DURL_OPTION_PROXY_TYPE             = 26,    /* 26(int)-proxy type, 0 disable(default), 1 enable */
    DURL_OPTION_PROXY_HOST             = 27,    /* 27(string)-proxy host, if NULL proxy will disable, Can be the IP or domain */
    DURL_OPTION_PROXY_PORT             = 28,    /* 28(int)-proxy port, 0 - 65535 */

    /* others */
    DURL_OPTION_ENV_RANDOMNESS         = 29,    /* 29(string)-The environment variable randomness */
    DURL_OPTION_ADDITION_BODY          = 30,    /* 30(string)-FUZE Hoteling additional data as power */

    /************************************************************************/
    /* In order to make the subsequent add format specification,            */
    /* reserved 31-99 for general Settings                                  */
    /* Greater than or equal to 100 for lync                                */
    /************************************************************************/

    /* lync account info */
    DURL_OPTION_SFB_USERINFO           = 100,    /* 100(durl_sfb_userinfo_t)-set the sfb account info */
    DURL_OPTION_SFB_EWS_USERINFO       = 101,    /* 101(durl_sfb_userinfo_t)-set the exchange account info */
    DURL_OPTION_SFB_DEV_REG_INFO       = 102,    /* 102(durl_sfb_dev_reg_info_t)-set the cilent id and device token */
    DURL_OPTION_SFB_ACCOUNT_TYPE       = 103,    /* 103(durl_sfb_acc_type_e)-set the sfb account type */

    /* lync registered */
    DURL_OPTION_SFB_ATD_URL            = 104,    /* 104(string)-lync autodiscover url */
    DURL_OPTION_SFB_ROOT_CA_URL        = 105,    /* 105(string)-Root CA url */
    DURL_OPTION_SFB_CERT_PROV_URL      = 106,    /* 106(string)-set the cert url */

    /* exchange options */
    DURL_OPTION_SFB_EWS_ATD_URL        = 107,    /* 107(string)-exchange autodiscover search url */
    DURL_OPTION_SFB_EWS_AUTH_METHOD    = 108,    /* 108(durl_http_auth_e)-exchange auth method */
    DURL_OPTION_SFB_EWS_CALENDAR_TYPE  = 109,    /* 109(int)-calendar type, 0 test(default), 1 html */

    /* others */
    DURL_OPTION_SFB_LIS_URL            = 110,    /* 110(string)-set the location url */
    DURL_OPTION_SFB_LIS_ATTR           = 111,    /* 111(durl_sfb_location_t)-location attr to send server */
    DURL_OPTION_SFB_ABWQ_ATTR          = 112,    /* 112(durl_sfb_abwq_attr_t)-address book search attr, if NULL search will fail */
    DURL_OPTION_SFB_CONTACT_INFO       = 113,    /* 113(durl_sfb_contact_info_t)-set contact info */
    DURL_OPTION_SFB_FIRMWARE_INFO      = 114,    /* 114(durl_sfb_fw_info_t)-set Automatic updates url */

    DURL_OPTION_END
} durl_option_e;

/*
*----------------------------------------------------------------------
* HTTP authentication mode
*----------------------------------------------------------------------
*/
typedef enum
{
    DURL_HTTP_AUTH_PAIRING       = 0,    /* default, will match on its own(Matching range of 1 ~ 4)  */

    /* The RFC defined criteria */
    DURL_HTTP_AUTH_BASIC         = 1,    /* Basic Access Authentication scheme, Use Base64 encoding */
    DURL_HTTP_AUTH_DIGEST        = 2,    /* Digest authentication */
    DURL_HTTP_AUTH_NTLM          = 3,    /* NT LAN Manager */
    DURL_HTTP_AUTH_NEGOTIATE     = 4,    /* negotiate */

    /* The custom authentication mode */
    DURL_HTTP_AUTH_SIP_XSI       = 5,    /* Is the evolution of the basic */
    DURL_HTTP_AUTH_FUZE_HOTELING = 6,    /* Non standard authentication, specify the authentication body */

    DURL_HTTP_AUTH_END,
} durl_http_auth_e;

/*
*----------------------------------------------------------------------
* Protocol request mode
*----------------------------------------------------------------------
*/
/* for HTTP */
typedef enum
{
    DURL_HTTP_METHOD_GET    = 0,    /* Request data from the specified resource */
    DURL_HTTP_METHOD_POST   = 1,    /* Submitted to the specified resource data to be processed */
    DURL_HTTP_METHOD_PUT    = 2,    /* Put data to server */
    DURL_HTTP_METHOD_DELETE = 3,    /* Will delete file server */
    DURL_HTTP_METHOD_HEAD   = 4,    /* Will conduct a head request, the server only return headers */
    DURL_HTTP_METHOD_END,
} durl_http_method_e;

/* for FTP */
typedef enum
{
    DURL_FTP_METHOD_NONE    = 0,    /* By the server to choose */
    DURL_FTP_METHOD_APPEND  = 1,    /* The client tell the server to use append upload, but the option in the server */
    DURL_FTP_METHOD_STOR    = 2,    /* The client tell the server to use cover upload, but the option in the server */
    DURL_FTP_METHOD_END,
} durl_ftp_method_e;

typedef enum
{
    DURL_FTP_LIMIT_NONE     = 0,    /* Server files more than limit, the client exit */
    DURL_FTP_LIMIT_DELETE   = 1,    /* Server files more than limit, Cover save */
    DURL_FTP_LIMIT_STOP     = 2,    /* Server files more than limit, Cancel the upload */
    DURL_FTP_LIMIT_END,
} durl_ftp_limit_e;

/*
*----------------------------------------------------------------------
* Input the user information
*----------------------------------------------------------------------
*/
typedef struct
{
    /* main info */
    char               url[DURL_MAX_URL_LEN];           /* If empty, will do nothing! The full URL to get/put,
                                                        Format: Protocol://ip or domain/xxx(example: http://www.yealink.com) */
    char               username[DURL_MAX_USERINFO_LEN];
    char               password[DURL_MAX_USERINFO_LEN];

    /* HTTP protocol info */
    durl_http_method_e http_mode;
    durl_http_auth_e   http_auth;                       /* HTTP authentication mode */

    /* FTP protocol info */
    durl_ftp_method_e  ftp_mode;
    durl_ftp_limit_e   ftp_limit;

    /* Internal control */
    unsigned int       no_boundary;                     /* 0 default, only http post effective, if 1,will post file info and no boundary  */
    unsigned int       recv_len;                        /* Need to read the length(0 ALL/default) */
    unsigned int       keep_alive;                      /* Set TCP keepalive; 0 no keep alive(default), 1 keep connection, if busyness, The program will build */
    unsigned int       ignore_auth;                     /* Set ignore server auth; 0 default(auto pairing), 1 ignore respone 401 */
    unsigned int       no_redirect;                     /* 0 default, only http effective, if 1, will return while server respone 301/302 */

    /* HTTP the content of the need to send */
    unsigned int       header_len;
    unsigned int       body_len;
    const char        *header;                          /* Only the HTTP effectively, This points to a linked list of headers(Separated by a \r\n)*/
    const char        *body;                            /* Only the HTTP effectively, This points to a linked list of post entries */
} durl_url_t;

/*
*----------------------------------------------------------------------
* Result detail
*----------------------------------------------------------------------
*/
/* The server result detail */
typedef struct
{
    int            code;                                /* The server retrun code */
    unsigned int   header_len;                          /* The server return the length of the head */
    unsigned int   body_len;                            /* The server return the length of the body */
    char          *header;                              /* The server return header data(Separated by a \r\n), ftp or tftp will no*/
    char          *body;                                /* The server return data content */
} durl_server_result_t;

/* Result detail */
typedef struct
{
    int                   code;                         /* The execution result code */
    char                  message[DURL_MAX_BUFFER_LEN]; /* Return info */
    char                  redirect[DURL_MAX_URL_LEN];   /* Server to inform the new address */
    durl_server_result_t *detail;                       /* The server return detail */
} durl_result_t;

/*
*----------------------------------------------------------------------
* Callback function
*----------------------------------------------------------------------
*/
/**
* @brief  :log callbak function
*
* @param  :module,log output module, form durl_init(module parameters
*          level,log level, see enum durl_log_level_e
*          fmt,log info format
*
* @warning:using vsnprintf() write output to the given output stream
*
* @return :none.
*
*/
typedef void (*durl_log_callback)(int module, int level, const char* fmt, ...);

#ifdef __cplusplus
extern "C" {
#endif
/*
*----------------------------------------------------------------------
* Common APIs,Can direct docking curl
*----------------------------------------------------------------------
*/
/**
* @brief  :init the global data, and setup global syslog callback function
*
* @param  :log_module,user defined syslog module id
*          log_cb,syslog callback
*
* @warning:You must call init, if no, the library will not run!
*          You must set callback function, if NULL or default,will NOT printf any info!
*
* @return :0 as success,otherwise failed.
*
*/
DURL_API int durl_init   (int log_module, durl_log_callback log_cb);
DURL_API int durl_cleanup(void);

/**
* @brief  :use to create durl based task instance handle, will initialize
*
* @param  :handle,use to save return result,will NOT be change if failed
*
* @warning:
*
* @return :0 as success,otherwise failed.
*
*/

DURL_API int durl_create(durl_handle_t *handle);

/**
* @brief  :use to destroy durl based task instance, and cleanup cache!
*
* @param  :handle,which instance want to be destroy
*
* @warning:if NOT NULL, we will destroy registration
*
* @return :0 as success,otherwise failed.
*
*/
DURL_API int durl_destroy(durl_handle_t handle);

/**
* @brief  :set handle members / get handle members info
*
* @param  :handle,which instance want to config
*          option,the corresponding enumeration structure members
*
* @warning:According to the durl_option_e description of use
*
* @return :0 is success, otherwise failed.
*
*/
DURL_API int durl_set_option(durl_handle_t handle, durl_option_e option, ...);
DURL_API int durl_get_option(durl_handle_t handle, durl_option_e option, ...);

/**
* @brief  :Perform specific events
*
* @param  :handle,which instance want to perform
*
* @warning:All variables by durl_set_option setting of handle;
*          if perform end, you can call durl_get_option to get internal members
*
* @return :0 is success, otherwise failed.
*
*/
DURL_API int durl_perform(durl_handle_t handle);

/*
*----------------------------------------------------------------------
* easy perform APIs
*----------------------------------------------------------------------
*/
/**
* @brief  :Perform download events
*
* @param  :handle,which instance want to perform
*          result,the API perform result, include server perform result, if NULL do not caring result!
*          url,Network request need to input parameters,if NULL will do nothing!
*          file,Need to manipulate local files,if NULL will do nothing!
*
* @warning:if call durl_perform_download_to_memory(), caller must use durl_free_result() to free result.
*
* @return :0 is success, otherwise failed.
*
*/
DURL_API int durl_download_to_file  (durl_handle_t handle, durl_result_t *result, const durl_url_t *url, const char *file);
DURL_API int durl_download_to_memory(durl_handle_t handle, durl_result_t *result, const durl_url_t *url);

/**
* @brief  :Perform upload events
*
* @param  :handle,which instance want to perform
*          result,the API perform result, include server perform result, if NULL do not caring result!
*          url,Network request need to input parameters,if NULL will do nothing!
*          from_file,local file, if NULL or not exist, will do nothing!
*          to_filename,Tell the server name needs to be saved, if NULL will use form_name(Remove the absolute path)
*
* @warning:
*
* @return :0 is success, otherwise failed.
*
*/
DURL_API int durl_upload(durl_handle_t handle, durl_result_t *result, const durl_url_t *url, const char *from_file, const char *to_filename);

/**
* @brief  :free the server result and destroy the pointer
*
* @param  :result,which instance want to be destroy
*
* @warning:if NULL, do nothing.
*
* @return :0 is success, otherwise failed.
*
*/
DURL_API int durl_free_result(durl_result_t *result);

/**
* @brief  :Controllable network data flow reading
*
* @param  :handle,which instance want to perform
*          result,the API perform result, include server perform result, if NULL do not caring result!
*          url,Network request need to input parameters,if NULL will do nothing!
*          buf,The data buffer
*          buf_size,The length of the need to read
*
* @warning:You must call durl_close() to free, if the handle is not necessary, call durl_destroy() to destroy!
*          buf_size <= The size of input parameters buf!
*          If you don't read, will be wasting server resources,
*          and the server may take the initiative to disconnect!
*
* @return :0 is success, otherwise failed.
*
*/
DURL_API int durl_open(durl_handle_t handle, durl_result_t *result, const durl_url_t *url);
DURL_API int durl_read(durl_handle_t handle, durl_result_t *result, char *buf, size_t buf_size);

/**
* @brief  :disconnect and release the cache for connect
*
* @param  :handle,which instance want to config
*
* @warning:Must be in durl_destroy () call before, if no will invalid.
*
* @return :0 is success, otherwise failed.
*
*/
DURL_API int durl_close(durl_handle_t handle);

/************************************************************************/
/* for debug                                                            */
/************************************************************************/
/**
* @brief  :dump the handle internal information
*
* @param  :handle,which instance want to config
*          buf,The data buffer, if NULL will print to log callback
*          buf_size,The length of the need to read
*
* @warning:if buf NULL, Dump the contents of the will by the log output,
*          if no set the log callback, will no print any info!
*
* @return :0 is success, otherwise failed.
*
*/
DURL_API int durl_dump(durl_handle_t handle, char *buf, size_t buf_size);

#ifdef __cplusplus
}
#endif

#endif /* END __DURL_COMMON_H__ */