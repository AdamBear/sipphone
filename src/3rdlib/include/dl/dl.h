/********************************************************************
*
*    DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*    AUTHOR:    wangsc@yealink.com
*
*    HISTORY:
*
*    DATE:      2017-11-05
*
*******************************************************************/

#ifndef __LIBDOWNLOAD_H__
#define __LIBDOWNLOAD_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT __attribute((visibility("default")))
#endif

#define DL_BUFFSIZE 512
#define DL_GET_METHOD 0 /* http getģʽ*/
#define DL_POST_METHOD 1
#define DL_PUT_METHOD 2
#define DL_DELETE_METHOD 3
#define DL_HEAD_METHOD 4 /* ֻ����ҳ����ײ������ڼ��url����Ч�� */

#define DL_POST_DETAILS_METHOD 10 /* post�ļ����ݣ�ģʽͬDL_POST_METHOD��������Ҫ����������б��� */

#define DL_CA_INFO_SIZE 64
#define DL_MAX_CA_NUM 10

#define DL_CLIENT_CERT 0x1 /* �ͻ���֤�飬��˽Կ */

#define DL_DEFAULT_SSL_CIPHER_STR "RSA:!EXP:!LOW:!NULL:!MD5:@STRENGTH"

typedef struct _dl_config dl_handle; /* �ֲ����ṹ�� */

typedef int dl_stream; /* dl_open�ӿ�ʹ�õľ����ʹ�ú���Ҫ����dl_close������� */

typedef struct _dl_url_info
{
    int method;    /* httpģʽ(Ĭ����ƥ��,�ϴ�Ĭ��post);ftp�ϴ���ʽ��ѡ��1 APPEND 2 STOR(����) */
    int limitmode; /* ftp�ϴ������ļ����ƺ��ѡ��:1 DELETE(����) 2 STOP(�˳�) */
    int recvlen;   /* ��Ҫ��ȡ�ĳ��ȣ���body���ֽڿ�ʼ���㣬Ϊ0��ʾȫ����ȡ(default) */
    int http_auth; /* http��Ȩ��ʽ */
    char *url;     /* url��ַ, ��ʽ�̶�Ϊ����Э��://ip or domain/xxx(�� http://www.yealink.com) */
    char *proxyurl;
    char *proxy_url;
    char *user;    /* �û������ÿձ�ʾ�������� */
    char *passwd;  /* ���룬�ÿձ�ʾʹ�ÿ�������� */
    char *header;  /* http���й��� */
    char *body;    /* http���й��ܣ��ϴ��ļ�ʱ���ò���������Ч�������ÿձ����˷��ڴ� */

    int keepalive; /* Ĭ��Ϊ�������ӽ��б�����0 ʹ�ö����ӣ�1 ���볤����(��curl��汾libdownload֧��)
                    http���й��ܣ�ʹ�ó����ӣ������֣������ӱ�ռ����libdownload��������һ������������ */
} dl_url_info_t;

/* ��;: �ýṹ�����ڴ��url�ӿ�����Ķ�Ӧ��Ӧ��Ϣ */
typedef struct _dl_response
{
    int code;           /* ��Ӧ�Ĵ����룬ֵ��Ӧdl_errcode_eö�ٱ� */
    char code_msg[256]; /* ��Ӧ�Ĵ������� */
    char new_url[512];  /* http�ض����ַ */
} dl_response_t;

typedef struct _dl_ca_detail
{
    unsigned char version;                     /* �汾 */
    char serial_num[DL_CA_INFO_SIZE];          /* ���к� */
    char signature_algorithm[DL_CA_INFO_SIZE]; /* ǩ���㷨 */
    char pubkey_algorithm[DL_CA_INFO_SIZE];    /* ��Կ�㷨 */
    int pubkey_bits;                           /* ��Կ���� */
    char priv_key_algor[DL_CA_INFO_SIZE];      /* ˽Կ�㷨 */
    int priv_key_bits;                         /* ˽Կ���� */
    char issued_by[DL_CA_INFO_SIZE];           /* �䷢��Common Name */
    char issued_to[DL_CA_INFO_SIZE];           /* ������Common Name */
    unsigned long hash;                        /* ֤���hashֵ */
    char validity_begin[DL_CA_INFO_SIZE];      /* ��Ч�ڿ�ʼʱ�� GMT */
    char expire_date[DL_CA_INFO_SIZE];         /* ����ʱ�� GMT */
    char file_path[DL_CA_INFO_SIZE];           /* ֤��·�������ڱ���֤��·�� */
    unsigned char extensions_num;              /* ��չ����� */
    unsigned short data_len;                   /* ���� */
    void *data_ptr;                            /* ���� */
} dl_ca_detail_t;

typedef struct _dl_ca_detail_table_t
{
    int cnt;
    dl_ca_detail_t ca_infos[DL_MAX_CA_NUM];
} dl_ca_detail_table_t;

typedef enum _dl_http_auth {
    DL_HTTP_AUTH_NONE = 0,  /* ��Ч�ļ�Ȩ���� */
    DL_HTTP_MUTIL_AUTH = 1, /* �Զ�ʶ���Ȩ��ʽ */
    DL_HTTP_BASIC_AUTH,
    DL_HTTP_DIGEST_AUTH,
    DL_HTTP_NTLM_AUTH,
    DL_HTTP_NEGOTIATE,          /* Э�̼�Ȩ */
    DL_HTTP_SIP_XSI_AUTH,       /* SIP XSI��Ȩ(basic 64����) */
    DL_HTTP_NO_TRY_REDIRECT,    /* �������ض��� */
    DL_HTTP_NO_TRY_AUTH,        /* ������401��Ȩ��return success and 401 response code  */
    DL_HTTP_FUZE_HOTELING_AUTH, /* HOTELINE ��Ȩ(�ǹ淶��Ȩ��ʽ������) */
    DL_HTTP_END,
} dl_http_auth_e; /* ����http��Ȩ�趨 */

typedef enum _dl_options {
    DL_OPTIONS_USER_AGENT = 0,          /* �û�������Ϣ */
    DL_OPTIONS_SSL_CA_TRUST_PATH,       /* �����ε�֤��·�� */
    DL_OPTIONS_SSL_CA_SERVER_FILE,      /* �����֤���ļ� */
    DL_OPTIONS_SSL_METHOD,              /* SSL���ܷ�ʽ */
    DL_OPTIONS_SSL_TRUST,               /* �Ƿ�����ssl���� */
    DL_OPTIONS_SSL_CHECK_CN,            /* У������host���ڲ�����SSL����ʱ��Ч */
    DL_OPTIONS_MAX_REDIRECT,            /* ����ض����������http��Ч */
    DL_OPTIONS_MAX_FILESIZE,            /* ��������ļ�����url�ļ������޶���С���򷵻ش��� */
    DL_OPTIONS_TIMEOUT,                 /* ����ʱʱ�� */
    DL_OPTIONS_USEIPV6,                 /* ֧��IPV6��Ĭ��Ϊ0 ��֧�� */
    DL_OPTIONS_SSL_CA_TRUST_USE_PATH,   /* �������κͷ�����֤���ʹ��·����ԭ֤��·��optionֻ���ڰ�װ����ʾ��ɾ���� */
    DL_OPTIONS_SSL_CA_SERVER_USE_FILE,  /* ������������ʹ�õ�֤���ļ� */
    DL_OPTIONS_SSL_CA_UC_WEB_CERT_PATH, /* sip register cert install path */
    DL_OPTIONS_MAC_ADDR,                /* �����ַ */
    DL_OPTIONS_AUTH_TIMES,              /* ��Ȩʱ�� */
    DL_OPTIONS_SSL_PEM_PASSWORD,        /* ֤������ */
    DL_OPTION_CONNECT_TIMEOUT,          /* ���ӳ�ʱʱ�䣬�����������罨������ */
    DL_OPTION_READ_TIMEOUT,             /* ���ݶ�ȡʱ�䣬������TIMEOUT�����ص�����ʧЧ */
    DL_OPTION_ENV_RANDOMNESS,           /* ��������randomness */
    DL_OPTIONS_SSL_CIPHER_STR,          /* ssl�㷨�ַ��� */
    DL_OPTIONS_TRANS_DATA_TRACE,        /* �������ݴ�ӡ */
    DL_OPTION_CONFIG_PATH_PREFIX,       /* ���������ļ���ǰ׺·��������.\\config\\,.\\confog_PART_A\\,.\\config_PART_B\\ */
    DL_OPTION_RESOLV_FILE,              /* ����DNS�����ļ�·����WINDOWS��֧�ִ����� */
    DL_OPTION_HOSTS_FILE,               /* ����hosts�ļ���Ĭ��Ϊ/etc/hosts, WINDOWS��֧�ִ����� */
    DL_OPTION_RESOLV_NOSYS,             /* ������ʹ�÷�ϵͳ�ӿ�ִ��DNS���� */
    DL_OPTION_ADDITION_BODY,            /* FUZE Hoteling��Ȩʱ���ӵĶ���body */
    DL_OPTION_CLIENT_CERT_ALGORITHM,    /* �����豸֤��ǩ���㷨 */

    DL_OPTION_LOW_SPEED_LIMIT, /* ��ʹ����ٶȣ���λbyte/s��Ĭ��100byte/s */
    DL_OPTION_LOW_SPEED_TIME,  /* ��ʹ����ٶȳ�ʱʱ�䣬Ĭ�ϲ���ʱ */

    DL_OPTION_NET_PREFERRED, /* �������ȼ���0 IPV6(Ĭ��)��1 IPV4 */

    DL_OPTION_PROXY_MOTHOH, /* ���������ģʽ */
    DL_OPTION_PROXY_HOST,   /* ���������host */
    DL_OPTION_PROXY_PORT,   /* ����������˿� */

    DL_OPTIONS_RING_CFG,
    DL_OPTION_USER_CFG,
    DL_OPTIONS_END
} dl_options_e;

/* ���紫����������룬��ӦCURL�еĴ����� */
typedef enum _dl_err_code {
    DL_SUCCESS = 0,

    /* �߼������������ */
    DL_ERRUNDEFINED = -1,   /* δ������� */
    DL_ERRPARAMETER = -2,   /* ������� */
    DL_ERRCURLINIT = -3,    /* curl����ʧ�� */
    DL_ERRAUTH = -4,        /* ��֤���� */
    DL_ERRDISIPV6 = -5,     /* ipv6 ����ֹ */
    DL_ERRPAHD = -6,        /* ���յ���Ч�����ݣ���Ч������ */
    DL_ERRURLP = -7,        /* ����Ķ˿ڣ���Ч������ */
    DL_ERRFTP = -8,         /* FTP������Ч������ */
    DL_ERRTFTP = -9,        /* TFTP������Ч������ */
    DL_ERRTLSVERSION = -10, /* TLS�汾�����ݣ���Ч������*/
    DL_ERRINVALID_GRANT = -26,      /* ��Ч���û�ƾ֤ */

    /* CURL�ڲ���Ӧ�Ĵ�������Ϣ */
    DL_ERRUNINIT = 2,                 /* δ��ʼ�� */
    DL_ERRURLH = 3,                   /* �����URL */
    DL_ERRPROXYHOST = 5,              /* �������������ʧ�� */
    DL_ERRHOST = 6,                   /* �����HOST����������ʧ�� */
    DL_ERRCONN = 7,                   /* �޷����� */
    DL_ERRFTP_WRONG_PASSWD = 11,      /* FTP������� */
    DL_ERR_NOFULLY = 23,              /* δ��ȡ����������ָ����ȡ�����ǣ��ô��������Ϊ��ȷ */
    DL_ERRMEM = 27,                   /* �ڴ�ռ䲻�� */
    DL_ERRTIMEOVER = 28,              /* ����ʱ */
    DL_ERRPOSTDATA = 34,              /* http postʧ�� */
    DL_ERRSSLCONNECT = 35,            /* SSL����ʧ�� */
    DL_ERRREDIRECT = 47,              /* �ض������ */
    DL_ERRSERVERCERT = 51,            /* ��������֤�����SSL֤���ָ�Ƴ��� */
    DL_ERRGOT_NOTING = 52,            /* ����������Ч���ݷ��� */
    DL_ERRSSL_ENGINE = 53,            /* SSL��������Ϊ���� */
    DL_ERRSSL_SETFAILED = 54,         /* SSL����Ĭ�Ϸ�ʽʧ�� */
    DL_ERRWRDT = 55,                  /* �������ݳ��� */
    DL_ERRRDDT = 56,                  /* �����ݳ��� */
    DL_ERRCLINETCERT = 58,            /* �ͻ���֤����� */
    DL_ERRSSL_CIPHER = 59,            /* SSL�޷�ʹ��ָ�������� */
    DL_ERRSSL_CACERT = 60,            /* SSL CA֤������ */
    DL_ERRFILESIZE = 63,              /* �ļ���С���� */
    DL_ERRLOGIN_DENIED = 67,          /* �û��������¼ʧ�� */
    DL_ERRFTP_NOUSER = 74,            /* FTP �û��������� */
    DL_ERRSSL_CACERT_BADFILE = 77,    /* SSL�޷�����CA��CA�����ʧ */
    DL_ERRSSL_SHUTDOWN = 80,          /* SSL�ر�����ʧ�� */
    DL_ERRSOCK = 81,                  /* Socketδ׼���� */
    DL_ERRSSL_CRL_BADFILE = 82,       /* SSL�޷�����CRL�ļ� */
    DL_ERRSSL_ISSUER = 83,            /* SSL�����˴��� */
    DL_ERRNO_CONNECTION = 89,         /* �޿��õ����� */
    DL_SSL_PINNEDPUBKEYNOTMATCH = 90, /* SSL��Կδƥ�� */
    DL_SSL_INVALIDCERTSTATUS = 91,    /* SSL��Ч��֤�� */

    /* HTTP��Ӧ����Ӧ�� */
    DL_ERR301PARSE = 301, /* HTTP�ض���������� �����Э�� */
    DL_ERR301 = 301,      /* 301��ʱ�ض��� */
    DL_ERR302 = 302,      /* 302�ض��� */
    DL_ERR400 = 400,      /* ��Ч��ѯ */
    DL_ERR401 = 401,      /* HTTP��Ȩʧ�� */
    DL_ERR403 = 403,      /* ����ֹ */
    DL_ERR408 = 408,      /* ����ʱ */
    DL_ERR500 = 500,      /* ���������� */
    DL_ERR501 = 501,      /* ������δʵ�ַ��� */
    DL_ERR503 = 503,      /* ������������ */
    DL_OK_201 = 201,      /* ��Դ�ɹ������� */
    DL_OK_200 = 200       /* �ɹ���ȡ���� */
} dl_err_code_e;

typedef enum _dl_cert_err_code {
    DL_VERIFY_OK = 0,
    DL_ERR_PARAMETER,         /* ������� */
    DL_ERR_INTERNAL,          /* �ڲ����������⡢�����ĳ�ʼ�����ڴ�����������ļ�·������� */
    DL_ERR_SIGNATURE_FAILURE, /* ǩ���㷨���� */
    DL_ERR_NOT_YET_VAILD,     /* ֤����Ч��δ��ʼ */
    DL_ERR_HAS_EXPIRED,       /* ֤����� */
    DL_ERR_CERT_CHAIN,        /* ֤������ȫ����֤��ֻ��һ��item������ǩ��֤�� */
    /* �ͻ���֤��ר�� */
    DL_ERR_METHOD,             /* �����SSL���� */
    DL_ERR_NO_PUBLIC_KEY,      /* ֤���޹�Կ */
    DL_ERR_NO_PRIVATE_KEY,     /* ֤����˽Կ */
    DL_ERR_PRIV_KEY_NOT_MATCH, /* ��Կ˽Կ��ƥ�䣬��˽Կ������� */
    DL_ERR_UNKNOWN,            /* δ֪���� */
    DL_ERR_UC_CERT_EXIST,      /* UC֤���Ѿ����� */
    DL_ERR_END
} dl_cert_err_code_e;

typedef enum _dl_signature_algorithm {
    DL_MD5,
    DL_SHA_256,
    DL_SHA_384,
    DL_SHA_512,
    DL_SHA_END
} dl_signature_algorithm_e;

typedef enum _dl_ssl_method {
    DL_SSL_DEFAULT = 3, /* Ĭ��Ϊ��ƥ�䣬��DL_SSLV23һ�� */
    DL_TLSV1 = 0,
    DL_SSLV2 = 1,
    DL_SSLV3 = 2,
    DL_SSLV23 = 3,   /* ��ƥ�� */
    /* DL_TLSV10, */ /* ��ʹ��TLS1.0 */
    DL_TLSV11 = 4,
    DL_TLSV12 = 5,
    DL_TLSV13 = 6,
    DL_METHOD_END
} dl_ssl_method_e;

typedef enum _dl_module {
    DL_MODULE_ANY,
    DL_MODULE_MEM,
    DL_MODULE_COMMON,
    DL_MODULE_HTTP,
    DL_MODULE_TFTP,
    DL_MODULE_FTP,
} dl_log_module_e;

typedef enum _dl_log_type {
    DL_LOG_TYPE_NONE = 0x00,
    DL_LOG_TYPE_SYS = 0x01,  /* output to syslogd */
    DL_LOG_TYPE_STD = 0x02,  /* output to stdout */
    DL_LOG_TYPE_FILE = 0x04, /* output to file */
    DL_LOG_TYPE_MAX = 0x07,
} dl_log_type_e;

typedef enum _dl_log_now_mode {
    DL_NEW_LOG_MODE_NONE = 0x0000,
    DL_NEW_LOG_MODE_TIME = 0x0001, /* log have precise time */
} dl_log_new_mode_e;

typedef enum _dl_log_level {
    DL_LOG_LEVEL_EMERG,  /* system is unusable */
    DL_LOG_LEVEL_ALERT,  /* action must be taken immediately */
    DL_LOG_LEVEL_CRIT,   /* critical conditions */
    DL_LOG_LEVEL_ERR,    /* 3 */
    DL_LOG_LEVEL_WARN,   /* 4 */
    DL_LOG_LEVEL_NOTICE, /* 5 */
    DL_LOG_LEVEL_INFO,   /* 6 */
    DL_LOG_LEVEL_DEBUG,  /* 7 */
} dl_log_level_e;

/************************************************************************/
/*          ����ṹ������ת��                                            */
/************************************************************************/
/* FIXME:�޸�ͷ�ļ��淶�����ھɰ汾�������ѱ��ⲿʹ�ã������ض��壬���鲻��ʹ�� */
typedef struct _dl_ca_info
{
    char issued_to[DL_CA_INFO_SIZE];
    char issued_by[DL_CA_INFO_SIZE];
    char expire_date[DL_CA_INFO_SIZE];
    unsigned long hash;
    char file_path[DL_CA_INFO_SIZE];
} dl_ca_info_t;

typedef struct _dl_ca_info_table
{
    int cnt;
    dl_ca_info_t ca_infos[DL_MAX_CA_NUM];
} dl_ca_info_table_t;

typedef dl_url_info_t dl_url_info;
typedef dl_url_info_t dl_conn_info;
//typedef dl_response_t response_t;
typedef dl_ca_info_t dl_ca_info;
typedef dl_ca_info_table_t dl_ca_info_table;
typedef dl_ca_detail_t dl_ca_info_ex;
typedef dl_ca_detail_table_t dl_ca_info_table_ex;

typedef dl_options_e dl_options;
typedef dl_err_code_e dl_err_code;
typedef dl_cert_err_code_e dl_cert_err_code;
typedef dl_signature_algorithm_e dl_signature_algorithm;
typedef dl_ssl_method_e dl_ssl_method;
typedef dl_log_module_e dl_module;
typedef dl_log_type_e DL_TYPE;
typedef dl_log_new_mode_e DL_NEW_MODE;
typedef dl_log_level_e dl_log_level;
/************************************************************************/
/*          PERMISSION                                                  */
/************************************************************************/
typedef enum _dl_permission_type {
    DL_PERMISSION_FACTORY,
    DL_PERMISSION_LICENSE,
    DL_PERMISSION_MAX,
} dl_permission_type_e;

#define DL_PERMISSION_LYNC "1.3.6.1.6.4.147.7.4.11"
#define DL_PERMISSION_VCS "1.3.6.1.6.4.147.7.4.12"

#define DL_PERMISSION_VCS_8W "1.3.6.1.6.4.147.7.4.14"
#define DL_PERMISSION_VERSION_AWARD "1.3.6.1.6.4.147.7.4.15"
#define DL_PERMISSION_VCS_MULTI_WAY "1.3.6.1.6.4.147.7.4.16"
#define DL_PERMISSION_AMR "1.3.6.1.6.4.147.7.4.17"

#define DL_PERMISSION_VC500_CP960 "1.3.6.1.6.4.147.7.4.18"
#define DL_PERMISSION_VC500_DOUBLE_SCREEN "1.3.6.1.6.4.147.7.4.19"
#define DL_PERMISSION_VC500_H265 "1.3.6.1.6.4.147.7.4.20"
#define DL_PERMISSION_VC500_FRAME60 "1.3.6.1.6.4.147.7.4.21"

#define DL_UPGRADE_AWARD_KEY "upgrade_award"

#define DL_PERMISSION_VCS_DEFINITE_WAY_KEY "vcs_way"
/************************************************************************/
/*                        APIs                                          */
/************************************************************************/
/*
* void dl_config_init()
* ��    �ܣ� ��ʼ������
* ��ڲ�����
* ���ڲ�����
* ����ֵ��
* ��    ע��
*/
DLLEXPORT void dl_config_init(void);

/*
* void dl_exit()
* ��    �ܣ� �ͷ����ؿ⻺������
* ��ڲ�����
* ���ڲ�����
* ����ֵ��
* ��    ע��
*/
DLLEXPORT void dl_exit(void);

/*
* const char *dl_get_version()
* ��    �ܣ� ��ȡ��ǰ�汾��
* ��ڲ�����
* ���ڲ�����
* ����ֵ��   �汾���ַ���
* ��    ע��
*/
DLLEXPORT const char *dl_get_version(void);

/*
* int dl_log_init(dl_log_type_e type, dl_log_new_mode_e mode, const char *file);
* ��    �ܣ� ��ʼ��log
* ��ڲ����� type : log����
*            mode : logģʽ
*            file : ����log�ļ��ľ���·��
* ���ڲ�����
* ����ֵ��
* ��    ע:  logģ���޸ĺ�����ӵĽӿ�
*/
DLLEXPORT int dl_log_init(dl_log_type_e type, dl_log_new_mode_e mode, const char *file);

/*
* int dl_set_log_module_level(dl_log_module_e module, dl_log_level_e level);
* ��    �ܣ� ����ģ������log�ȼ�
* ��ڲ����� module:ģ���ţ�level:ģ��log�ȼ�
* ���ڲ�����
* ����ֵ��
* ��    ע��
*/
DLLEXPORT int dl_set_log_module_level(dl_log_module_e module, dl_log_level_e level);
/*
* int dl_set_option(dl_options_e dl_opt, void *value);
* ��    �ܣ� ��������ѡ��
* ��ڲ����� dl_opt:Ҫ���õ�ѡ� value:Ҫ���õ�ֵ
* ���ڲ�����
* ����ֵ��   0 �ɹ� -1 ʧ��
* ��    ע��
*/
DLLEXPORT int dl_set_option(dl_options_e dl_opt, const void *value);

/*
* int dl_get_option(dl_options_e dl_opt, void *buf, int size);
* ��    �ܣ� ��ȡ����ѡ��
* ��ڲ����� dl_opt:Ҫ��ȡ��ѡ�size: ��ȡ���ݵĴ�С
* ���ڲ����� buf: ��ȡ���ݻ�����
* ����ֵ��   0 �ɹ� -1 ʧ��
* ��    ע��
*/
DLLEXPORT int dl_get_option(dl_options_e dl_opt, void *buf, int size);

/*
* char *dl_http_find_header_by_name(dl_http_handler handler, const char *name)
* ��    �ܣ� ��ȡHTTPͷ��
* ��ڲ����� handle: http���, name:Ҫ��ȡ��ͷ����
* ���ڲ�����
* ����ֵ��   ���ظ�ͷ��ָ�룬�����Ҳ�������NULL;
* ��    ע�� �ڴ����ڲ��ͷţ�����dl_http_close���ͷŵ��ڴ档
             (�����ڵ���close���ٻ�ȡ������close���ٴ�ʹ�ø�ָ��)
*/
DLLEXPORT char *dl_http_find_header_by_name(dl_stream handle, const char *name);

/************************************************************************/
/*                        NEW APIS                                      */
/************************************************************************/

/*
* ���ܣ�����dl_handle�ṹ��ָ�루ʹ�ý��������ͷţ�
* ���룺NULL
* �����NULL
* ���أ�handleָ�룬NULL��ʾ��ȡʧ��
* ��ע�������滻dl_config_init�ӿڣ����鲻��ʹ��dl_config_init�ӿ�
*/
DLLEXPORT dl_handle *dl_easy_init(void);

/*
* ���ܣ���������handle�ṹ���Ա���Լ�����ȫ�ֱ���
* ���룺handle �ṹ��ָ�룬Ϊ�ձ�ʾ����ȫ�֣��ǿ�ʱ��ȫ�ֶ�Ӧ����Ϊ��ʱ��Ҳ���赽ȫ��
* ���룺opt ö�ٱ����˶�Ӧ�Ĺ���
* ���룺value ��Ҫд���ֵ
* ���أ�0 �ɹ�����0ʧ��
* ��ע��
*/
DLLEXPORT int dl_easy_setopt(dl_handle *handle, dl_options_e opt, const void *value);

/*
* ���ܣ����ڻ�ȡhandle�Ľṹ�����Ա
* ���룺handle�ṹ��ָ�룬Ϊ�ձ�ʾ��ȡȫ�֣��ǿձ�ʾ��ȡָ���Ӧ�ĳ�Ա
* ���룺opt ö�ٱ����˶�Ӧ�Ĺ���
* ���룺size ��Ҫ����ĳ���
* �����buf ��ȡ������
* ���أ�0 �ɹ�����0ʧ��
* ��ע��
*/
DLLEXPORT int dl_easy_getinfo(dl_handle *handle, dl_options_e opt, void *buf, int size);

/*
* ���ܣ��������handle�ֲ�����
* ���룺handle�ṹ��ָ�룬����Ϊ��
* �����NULL
* ���أ�NULL
* ��ע��
*/
DLLEXPORT void dl_easy_cleanup(dl_handle *handle);

/*
* ���ܣ�����ָ����url��Ӧ����
* ���룺handle�ṹ��ָ�룬Ϊ��ʱ��ʾʹ��ȫ�ֻ�Ĭ�ϲ�����������
* ���룺url_info url��Ӧ����Ϣ��ָ�벻��Ϊ�գ��ڲ���Ա���ݽṹ�屸ע����
* ���룺dl_recv_callback �ص�����
* �����out ��ȡ������ָ���ַ���ڲ�����ռ䣬���ͷ�
* �����res �ӿ�ִ�е���Ϣ������ǿ�
* ���أ�0�ɹ�����0ʧ��
* ��ע��
*/
DLLEXPORT int dl_recv_stream(dl_handle *handle, dl_url_info_t *url_info, char **out, dl_response_t *res);

/*
* ���ܣ�����ָ����url��Ӧ�������ļ���
* ���룺handle�ṹ��ָ�룬Ϊ��ʱ��ʾʹ��ȫ�ֻ�Ĭ�ϲ�����������
* ���룺url_info url��Ӧ����Ϣ��ָ�벻��Ϊ�գ��ڲ���Ա���ݽṹ�屸ע����
* ���룺filename ��Ҫ������ļ�����������·��������Ϊ��
* �����res �ӿ�ִ�е���Ϣ������ǿ�
* ���أ�0�ɹ�����0ʧ��
* ��ע��
*/
DLLEXPORT int dl_download(dl_handle *handle, dl_url_info_t *url_info, const char *file, dl_response_t *res);

/*
* ���ܣ��ϴ�ָ���ļ���������
* ���룺handle�ṹ��ָ�룬Ϊ��ʱ��ʾʹ��ȫ�ֻ�Ĭ�ϲ�����������
* ���룺url_info url��Ӧ����Ϣ��ָ�벻��Ϊ�գ��ڲ���Ա���ݽṹ�屸ע����
* ���룺filename ��Ҫ�ϴ����ļ���������·��������Ϊ��
* ���룺snedname �ϴ��������������ƣ�Ϊ��ʱ��ʹ��filename��Ӧ���ļ����������з���
* �����res �ӿ�ִ�е���Ϣ������ǿ�
* ���أ�0�ɹ�����0ʧ��
* ��ע��
*/
DLLEXPORT int dl_upload(dl_handle *handle, dl_url_info_t *url_info, const char *file, const char *sendname, dl_response_t *res);

/*
* ����: ����curl download�첽�̣߳������������������ȴ�dl_stream_recv��ȡ
* ����: url_info ��������Ĳ����ṹ��
* ����: dl_stream ����ڲ�����Ĵ�����
* ���: res �����뼰��������
* ����: 0 ������-1ʧ�ܣ���ϸ����res�еĴ����뼰������
* ��ע: ʹ�ý��������dl_stream_close
*/
DLLEXPORT int dl_open(dl_stream *handle, dl_url_info_t *url_info, dl_response_t *res);

/*
* ����: ��ȡurl������
* ����: dl_stream dl_stream_open�ӿ�����Ĵ�����
* ����: len ��Ҫ��ȡ�ĳ���
* ���: buf ���ݴ洢������
* ���: res �����뼰��������
* ����: 0 ������-1ʧ�ܣ���ϸ����res�еĴ����뼰������
*/
DLLEXPORT int dl_read(dl_stream handle, char *buf, int len, dl_response_t *res);

/*
* ����: �رմ�����tcp���Ӽ�ʾ����Ӧ��Դ
* ����: dl_stream dl_stream_open�ӿ�����Ĵ�����
* ����: NULL
*/
DLLEXPORT void dl_close(dl_stream handle);

/*
* ����: ���������ַ�����md5ֵ
* ����: input ��Ҫ������ַ���������unsigned char
* ���: outbuf �õ���MD5ֵ��ע�û���������Ϊ16
* ����: NULL
* ��ע: �ýӿ�ֱ��return Curl_md5it�ӿ�
*/
DLLEXPORT void dl_Curl_md5it(unsigned char *outbuf, const unsigned char *input);

/*
* ����: ��ӡ��Libdownload���ڴ�ʹ�����
* ����: info ����ڴ��ռ����Ϣ
* ����: dump �����ǰδ�ͷŵ��ڴ������¼
* ����: check ����ڴ�У�����
* ����: ��
*/
DLLEXPORT void dl_memory_dump(int info, int dump, int check);

/*
* ����: �ͷ�Libdownload��������ڴ�
* ����: p ��Ҫ�Ƿ��ָ��
* ��ע: �����ָ������Libdownload����ռ�ģ���Libdownload�������Դ��Ҫʹ�øýӿ��ͷ�
*/
DLLEXPORT void dl_memory_free(void *p);

/************************************************************************/
/*                        ֤������ӿڻ�֤���Ȩ                        */
/************************************************************************/

/*
* int dl_install_ca(const char *file)
* ��    �ܣ� ��װ֤��
* ��ڲ����� file: �ļ�����·��
* ���ڲ�����
* ����ֵ��   -1:ʧ�� -2:�Ѿ����� -3:������Ԥ��
* ��    ע��
*/
DLLEXPORT int dl_install_ca(const char *file);

/*
* int dl_del_ca(const char *file)
* ��    �ܣ� ɾ��֤��
* ��ڲ����� file: �ļ�����·��
* ���ڲ�����
* ����ֵ��   0�ɹ� -1 ʧ��
* ��    ע��
*/
DLLEXPORT int dl_del_ca(const char *file);

/*
* int dl_del_ser_ca()
* ��    �ܣ� ɾ��������֤��"server certificate"
* ��ڲ�����
* ���ڲ�����
* ����ֵ��   0�ɹ� -1 ʧ��
* ��    ע�� ·����Option DL_OPTIONS_SSL_CA_SERVER_FILEָ��
*/
DLLEXPORT int dl_del_ser_ca();

/*
* int dl_install_ser_ca(const char * file)
* ��    �ܣ� ��װWEB������֤��
* ��ڲ����� file: �ļ�����·��
* ���ڲ�����
* ����ֵ��   0�ɹ� -1 ��PEM��ʽ -2 �ļ���˽Կ -3 �ļ��޹�Կ
*                   -4 ����� -5 ��ʼ��ʧ�� -6 ����Ŀ¼ʧ��
                    -7 �Ѿ����� -8 ������Ԥ��
* ��    ע��
*/
DLLEXPORT int dl_install_ser_ca(const char *file);

/**
* @breif ��װ�豸֤�飬֧���ϴ�����֤��(����˽Կ)������˽Կ���������豸֤��(��˽Կ)
* @param file:֤���˽Կ�ļ�
* @ret 0 �ɹ�(�����ٷ������������һ��)
*     -1 ʧ�� ������ļ������ڻ��ʽ������ڲ�����
*
* ע��:1.���ӿڲ��ж�֤���x509��Ϣ�Ƿ����Ѵ��ڵ�Ԥ�á��Զ���֤����ͬ����Ϊ�ֿ��ϴ������ܻ����ͬһ��x509��Ϣ֤�飬���ǲ�ͬ˽Կ������!!
*      2.���ӿڲ��ж�֤��ƥ��ƥ��
*/
DLLEXPORT int dl_install_ser_ca_separate(const char *file);

/**
* @breif �Ƚ�����֤���Ƿ���ͬ
* @param filepath1 ֤��1ȫ·��
* @param filepath2 ֤��2ȫ·��
* @ret 0 ����֤��һ��
*      ����ֵ������֤�鲻һ��
*
*/
DLLEXPORT int dl_ca_cmp(const char *filepath1, const char *filepath2);

/*
* int dl_get_trust_ca_info(dl_ca_info_table *table)
* ��    �ܣ� ��ȡ����֤����Ϣ
* ��ڲ�����
* ���ڲ����� table: ����֤���ļ���Ϣ��
* ����ֵ��   0 �ɹ� -1 ������� -2 δ��������Ŀ¼����Ŀ¼
* ��    ע��
*/
DLLEXPORT int dl_get_trust_ca_info(dl_ca_info_table_t *table);

/*
* int dl_get_ser_ca_info(dl_ca_info_t *info)
* ��    �ܣ� ��ȡ������֤����Ϣ
* ��ڲ�����
* ���ڲ����� info: ������֤����Ϣ
* ����ֵ��   0 �ɹ� -1 ������� -2 δ���÷�����֤����޷�����֤�� -3 �豸֤��˽Կ�͹�Կ��ƥ��
* ��    ע��
*/
DLLEXPORT int dl_get_ser_ca_info(dl_ca_info_t *info);

/*
* int dl_get_ca_info(const char *file, dl_ca_info_t *info)
* ��    �ܣ� ��ȡ֤����Ϣ
* ��ڲ����� file: ֤���ļ�
* ���ڲ����� info: ֤���ļ���Ϣ
* ����ֵ��   0 �ɹ� -1 ������� -2 ��֤���ļ�
* ��    ע��
*/
DLLEXPORT int dl_get_ca_info(const char *file, dl_ca_info_t *info);

/*
* int dl_get_ca_info_ex(const char *file, dl_ca_detail_t *info, int cert_flags, dl_response_t *res)
* ��    �ܣ� ��ȡ֤����Ϣ
* ��ڲ����� file: ֤���ļ� cert_type: ֤���ʶλ��Ĭ��Ϊ0����ͨCA���ͻ���֤������DL_CLIENT_CERT��ʶ
* ���ڲ����� info: ֤���ļ���Ϣ res: ������Ϣ�����������_dl_cert_err_code
* ����ֵ��   0 �ɹ� -1 ʧ��
* ��    ע��
*/
DLLEXPORT int dl_get_ca_info_ex(const char *file, dl_ca_detail_t *info, int cert_flags, dl_response_t *res);

/*
* int dl_get_trust_ca_info_ex(dl_ca_detail_table_t *table�� dl_response_t *res)
* ��    �ܣ� ��ȡ����֤����Ϣ
* ��ڲ�����
* ���ڲ����� table: ����֤���ļ���Ϣ�� res ������Ϣ,���������dl_cert_err_code
* ����ֵ��   0 �ɹ� -1 ʧ��
* ��    ע��
*/
DLLEXPORT int dl_get_trust_ca_info_ex(dl_ca_detail_table_t *table, dl_response_t *res);

/*
* int dl_cert_verify(const char *file, const char *ca_path, const char *ca_file, int cert_flags, dl_response_t *res)
* ��    �ܣ� ����X509��ʽ��֤���Ƿ���Ч����
* ��ڲ����� file: ҪУ���֤���·��
             ca_path: ָ������֤��RootCA����·����CA֤����hashֵ������ţ���ͨ��dl_get_ca_info_ex��ȡ֤���hashֵ��Ϣ��Ȼ���ļ�������Ϊ$hash��$hash.0
             ca_file: ָ������֤��RootCA�ļ�
             cert_flags: ֤���ʶλ��Ĭ��Ϊ0����ͨCA���ͻ���֤������DL_CLIENT_CERT��ʶ
* ���ڲ����� res: ������Ϣ�����������dl_cert_err_code
* ����ֵ��   0�ɹ� -1 ʧ��
* ��    ע��@refer openssl/apps/verify.c
            ֤���RootCAʱ����Ҫָ��ca_path��ca_file��ָ��RootCA��ʹ��֤��������֤֤���Ƿ���Ч���ţ�
            ��֤�ͻ���֤��ʱ��������֤˽Կ�͹�Կ�Ƿ�ƥ��ȣ�
*/
DLLEXPORT int dl_cert_verify(const char *file, const char *ca_path, const char *ca_file, int cert_flags, dl_response_t *res);

/**
* int dl_generate_client_cert(const char *ca_file, const char *cakey_file, const char *clientkey_file, char *clientcert_file, const char *subject, int key_bit)
* ����: ���ɿͻ���֤��
* ��ڲ���: ca_file RootCA cakey_file RootCA��˽Կ clientkey_file�ͻ���˽Կ subject �ͻ���֤���subject key_bit˽Կλ��
* ���ڲ���: clientcert_file ���ɵĿͻ���֤���·��
* ����ֵ: 0 �ɹ� -1 ʧ��
* ��    ע��@refer openssl/apps/ca.c req.c
            RootCA����˽Կ������ڣ��ͻ���˽Կclientkey_file�����ɽӿ����ɣ�����ʱkey_bitָ��˽Կ����
*/
DLLEXPORT int dl_generate_client_cert(const char *ca_file, const char *cakey_file, const char *clientkey_file, char *clientcert_file, const char *subject, int key_bit);

/**
* int dl_cert_build_csr_req_file(char *csr_file, char *key_file, char *subject, int key_bit)
* ����: ����֤��CSR�ļ�
* ��ڲ���: ca_csr ֤��CSR�ļ� clientkey_file�ͻ���˽Կ subject �ͻ���֤���subject key_bit˽Կλ��
* ���ڲ���: csr_file ���ɵĿͻ���֤��CSR�ļ�·��
* ����ֵ: 0 �ɹ� -1 ʧ��
* ��    ע��
*/
DLLEXPORT int dl_cert_build_csr_req_file(char *csr_file, char *clientkey_file, char *subject, int key_bit);

/*
* int dl_permission_verify(const char *ext_key_usage)
* ��   �ܣ����鵱ǰ�Ƿ���ext_key_usage��Ӧ��֤����Ȩ
* ��ڲ�����ext_key_usage: Ҫ��֤��Ȩ�ޣ�����Lync�ǡ�1.3.6.1.6.4.147.7.4.11��������궨��DL_PERMISSION_LYNC
* ���ڲ�������֤�Ƿ�ͨ��
* ����ֵ��  0��֤ͨ����-1δͨ����Ȩ��
* ��   ע��
*/
DLLEXPORT int dl_permission_verify(const char *ext_key_usage);

/*
* int dl_permission_verify_with_path(const char *license_dat_path, const char *ext_key_usage)
* ��   �ܣ����鵱ǰ�Ƿ���ext_key_usage��Ӧ��֤����Ȩ
* ��ڲ�����license_dat_path: license dat�ļ���ȫ·��
* ��ڲ���: ext_key_usage: Ҫ��֤��Ȩ�ޣ�����Lync�ǡ�1.3.6.1.6.4.147.7.4.11��������궨��DL_PERMISSION_LYNC
* ���ڲ�������֤�Ƿ�ͨ��
* ����ֵ��  0��֤ͨ����-1δͨ����Ȩ��
* ��   ע��
*/
DLLEXPORT int dl_permission_verify_with_path(const char *license_dat_path, const char *ext_key_usage);

/*
* int dl_get_all_permission(int type, char *ext_key_usage, int usage_len)
* ��   �ܣ���ȡ��ǰ�����µ�����Ȩ��
* ��ڲ�����type: ��ȡ���ͣ�0����Ȩ�ޣ�1֤��Ȩ��
* ��ڲ�����usage_len: ���ext_key_usageȨ���ַ����Ĵ�С
* ���ڲ�����ext_key_usage: Ȩ���ַ����������ڡ�1.2.3.4,1.2.3.5��
* ����ֵ��  0��ȡ�ɹ���-1��ȡʧ��
* ��   ע���ýӿ�������ҳUI��ʾȨ��״̬
*/
DLLEXPORT int dl_get_all_permission(dl_permission_type_e type, char *ext_key_usage, int usage_len);

/*
* int dl_get_all_permission(int type, char *ext_key_usage, int usage_len)
* ��   �ܣ���ȡ��ǰ�����µ�����Ȩ��
* ��ڲ�����license_dat_path: license dat�ļ���ȫ·��
* ��ڲ�����type: ��ȡ���ͣ�0����Ȩ�ޣ�1֤��Ȩ��
* ��ڲ�����usage_len: ���ext_key_usageȨ���ַ����Ĵ�С
* ���ڲ�����ext_key_usage: Ȩ���ַ����������ڡ�1.2.3.4,1.2.3.5��
* ����ֵ��  0��ȡ�ɹ���-1��ȡʧ��
* ��   ע���ýӿ�������ҳUI��ʾȨ��״̬
*/
DLLEXPORT int dl_get_all_permission_with_path(const char *license_dat_path, dl_permission_type_e type, char *ext_key_usage, int usage_len);

/*
* int dl_get_license_expire(dl_permission_type_e type, int *expire_ret)
* ��   �ܣ� ��ȡ��ǰ�����¹�����ɵ���Ч����
* ��ڲ�����type: ��ȡ���ͣ�0����Ȩ�ޣ�1֤��Ȩ��
* ���ڲ�����expire_ret: ��Чʱ���ֶΣ���λΪ���ӣ�-1��ʾ����Ȩ�ޣ�0��ʾ����0������Ȩ���ˣ�>0ֵΪ֤����Чʱ��
* ����ֵ��  0��ȡ�ɹ���-1��ȡʧ��
* ��   ע���ýӿ�������ҳUI��ȡ֤�����Чʱ����UI��ҳͨ���ڲ���ʱ����ʹ�õ�ʱ�䣬������Чʱ���Ƚϣ������������Чʱ����
*         ��Ȩ�޵��ڣ����ֹ���Ҫ���õ�
*/
DLLEXPORT int dl_get_license_expire(dl_permission_type_e type, int *expire_ret);

/*
* int dl_get_license_expire_with_path(const char *license_dat_path, dl_permission_type_e type, int *expire_ret)
* ��   �ܣ� ��ȡ��ǰ�����¹�����ɵ���Ч����
* ��ڲ�����license_dat_path: license dat�ļ���ȫ·��
* ��ڲ�����type: ��ȡ���ͣ�0����Ȩ�ޣ�1֤��Ȩ��
* ���ڲ�����expire_ret: ��Чʱ���ֶΣ���λΪ���ӣ�-1��ʾ����Ȩ�ޣ�0��ʾ����0������Ȩ���ˣ�>0ֵΪ֤����Чʱ��
* ����ֵ��  0��ȡ�ɹ���-1��ȡʧ��
* ��   ע���ýӿ�������ҳUI��ȡ֤�����Чʱ����UI��ҳͨ���ڲ���ʱ����ʹ�õ�ʱ�䣬������Чʱ���Ƚϣ������������Чʱ����
*         ��Ȩ�޵��ڣ����ֹ���Ҫ���õ�
*/
DLLEXPORT int dl_get_license_expire_with_path(const char *license_dat_path, dl_permission_type_e type, int *expire_ret);

/*
* int dl_get_license_expire_with_env(dl_permission_type_e type, int *expire_ret)
* ��   �ܣ� ��ȡ��ǰ�����¹�����ɵ���Ч����,�Ȼ�ȡ���������и����ʱ�������Ϊ0�����ȡ֤���ʱ����
            �����滻dl_get_license_expire
* ��ڲ�����ext_key_usage:����Ҫȷ��ʱ����Ȩ�ޣ��ò��������ڶ�ȡfactoryʱ����ʱ��ʹ�ã���ȡlicenseֱ�Ӷ�ȡʱ���ֶ�
* ���ڲ�����expire_ret: ��Чʱ���ֶΣ���λΪ���ӣ�-1��ʾ����Ȩ�ޣ�0��ʾ����0������Ȩ���ˣ�>0ֵΪ֤����Чʱ��
* ����ֵ��  0��ȡ�ɹ���-1��ȡʧ��
* ��   ע���ýӿ�������ҳUI��ȡ֤�����Чʱ����UI��ҳͨ���ڲ���ʱ����ʹ�õ�ʱ�䣬������Чʱ���Ƚϣ������������Чʱ����
*         ��Ȩ�޵��ڣ����ֹ���Ҫ���õ�
*/
DLLEXPORT int dl_get_license_expire_with_env(const char *ext_key_usage, int *expire_ret);

/*
* int dl_get_license_expire_with_path_env(const char *license_dat_path, const char *ext_key_usage, int *expire_ret)
* ��   �ܣ� ��ȡ��ǰ�����¹�����ɵ���Ч����,�Ȼ�ȡ���������и����ʱ�������Ϊ0�����ȡ֤���ʱ����
            �����滻dl_get_license_expire_with_path
* ��ڲ�����license_dat_path: license dat�ļ���ȫ·��
* ��ڲ�����ext_key_usage:����Ҫȷ��ʱ����Ȩ�ޣ��ò��������ڶ�ȡfactoryʱ����ʱ��ʹ�ã���ȡlicenseֱ�Ӷ�ȡʱ���ֶ�
* ���ڲ�����expire_ret: ��Чʱ���ֶΣ���λΪ���ӣ�-1��ʾ����Ȩ�ޣ�0��ʾ����0������Ȩ���ˣ�>0ֵΪ֤����Чʱ��
* ����ֵ��  0��ȡ�ɹ���-1��ȡʧ��
* ��   ע���ýӿ�������ҳUI��ȡ֤�����Чʱ����UI��ҳͨ���ڲ���ʱ����ʹ�õ�ʱ�䣬������Чʱ���Ƚϣ������������Чʱ����
*         ��Ȩ�޵��ڣ����ֹ���Ҫ���õ�
*/

DLLEXPORT int dl_get_license_expire_with_path_env(const char *license_dat_path, const char *ext_key_usage, int *expire_ret);

/*
* int dl_get_permission_name(const char *ext_key_usage, char *usage_name, int usage_len)
* ��   �ܣ���ȡȨ��������Ӧ��Ȩ������
* ��ڲ�����ext_key_usage: Ȩ�����������硱1.2.3.4��
* ��ڲ�����usage_len: ���usage_nameȨ�����ƵĴ�С
* ���ڲ�����usage_name: Ȩ�����ƣ����硱Lync��
* ����ֵ��  0��ȡ�ɹ���-1��ȡʧ��
* ��   ע���ýӿ�������ҳUI��ʾȨ��״̬
*/
DLLEXPORT int dl_get_permission_name(const char *ext_key_usage, char *usage_name, int usage_len);

/*
* int dl_get_license_addition_info(const char *ext_key_usage, char *usage_name, int usage_len)
* ��   �ܣ���ȡ֤��ĸ�����netscap common�е���Ϣ
* ��ڲ�����license_dat_path: ֤��ȫ·��
* ��ڲ�����key: ������Ϣʹ��key=value��ʽ��¼
* ��ڲ���: env: �Ƿ������������ȡ(��Щ��Ϣֱ�ӿ��Դӻ�����ֱ�ӻ�ȡ)
* ���ڲ�����buf: ���ֵ�Ļ�����
* ���ڲ���: buflen: ����������
* ����ֵ��  0��ȡ�ɹ���-1��ȡʧ��
* ��   ע���˽ӿڲ�����֤֤�����������͵���Ȩ֤�飬Ӧ����Ҫ�Լ���ȥ��֤���
*/
DLLEXPORT int dl_get_license_addition_info(const char *license_dat_path, const char *key, char *buf, int buflen, int env);

/*
* int dl_get_encrypt_ca_info_ex(const char *file, dl_ca_detail_t *info, dl_response_t *res);
* ��    �ܣ� ��ȡ����֤����Ϣ
* ��ڲ����� file: ����֤���ļ�
* ���ڲ����� info: ֤���ļ���Ϣ res: ������Ϣ
* ����ֵ��   0 �ɹ� -1 ʧ��
* ��    ע���ýӿ�������ҳUI��ʾ֤��״̬����֤�ϴ���dat�ļ��Ƿ���Ч��(�˽ӿ��ѷ������°汾�벻Ҫʹ��)
*/
DLLEXPORT int dl_get_encrypt_ca_info_ex(const char *file, dl_ca_detail_t *info, dl_response_t *res);

/*
* int dl_get_encrypt_ca_info_ex_path(const char *file, dl_ca_detail_t *info, dl_response_t *res);
* ��    �ܣ� ��ȡ����֤����Ϣ
* ��ڲ����� file: ����֤���ļ�
* ���ڲ����� info: ֤���ļ���Ϣ res: ������Ϣ
* ����ֵ��   0 �ɹ� -1 ʧ��
* ��    ע���ýӿ�������ҳUI��ʾ֤��״̬����֤�ϴ���dat�ļ��Ƿ���Ч;
*           �˰汾�Ľӿڲ������Ѵ��ڵ�license.dat���бȽϣ����Ҫ���бȽϣ���ֱ��ʹ��dl_ca_cmp
*/

DLLEXPORT int dl_get_encrypt_ca_info_ex_path(const char *file, dl_ca_detail_t *info, dl_response_t *res);

/*
* unsigned long dl_get_ca_hash();
* ��    �ܣ� ��ȡ֤���е�hashֵ
* ��ڲ����� ֤��·��
* ���ڲ����� ��
* ����ֵ��   hashֵ
* ��    ע��
*/
DLLEXPORT unsigned long dl_get_ca_hash(const char *file);


/************************************************************************/
/*                        ������ǰ�Ľӿ�,��������ʹ��                        */
/************************************************************************/

typedef int dl_http_handler;
typedef int dl_tftp_handler;
typedef int dl_ftp_handler;

typedef enum _dl_http_ex_type
{
    DL_EX_NONE = 0,
    DL_EX_MUTIL_AUTH,
    DL_EX_SIP_XSI_AUTH,
    DL_EX_NO_TRY_REDIRECT,
    DL_EX_BASIC_AUTH,
    DL_EX_NO_TRY_AUTH,          /* return success and 401 response code  */
    DL_EX_FUZE_HOTELING_AUTH,
    DL_EX_END,
}dl_http_ex_type;               /* ����http��չ */

typedef struct _dl_url_info_ex
{
    dl_url_info *url_info;
    dl_http_ex_type type;
    void *data_ptr;
    unsigned short data_len;
    unsigned long  wparam;
    unsigned long  lparam;
}dl_url_info_ex;

typedef struct _dl_uc_user_info
{
    int lid;                    /* line id */
    char useruri[512];          /* username@server */
    char authuri[512];          /* */
    char passwd[256];           /* */
}dl_uc_user_info_t;

typedef struct _dl_ring_config
{
    unsigned char cfg_idx;      /* �������� ��section Ϊ Alertinfo0 �� cfg_idx = 0*/
    unsigned char ring_idx;     /* �������� ��Ringer = 1 �� ring_idx = 1 */
    char text[126];             /* ��Text = test �� text = "test" */
} dl_ring_config;

/*************************dns related***********************************/
#ifndef WIN32
#include <arpa/inet.h>
#define DL_EVDNS_TYPE_A       1
#define DL_EVDNS_TYPE_NS      2
#define DL_EVDNS_TYPE_CNAME   5
#define DL_EVDNS_TYPE_SOA     6
#define DL_EVDNS_TYPE_PTR    12
#define DL_EVDNS_TYPE_MX     15
#define DL_EVDNS_TYPE_TXT    16
#define DL_EVDNS_TYPE_AAAA   28

#define DL_EVDNS_QTYPE_AXFR 252
#define DL_EVDNS_QTYPE_ALL  255

#define DL_EVDNS_CLASS_INET   1

#define DL_TYPE_A           DL_EVDNS_TYPE_A
#define DL_TYPE_CNAME       5
#define DL_TYPE_PTR         DL_EVDNS_TYPE_PTR
#define DL_TYPE_SOA         DL_EVDNS_TYPE_SOA
#define DL_TYPE_AAAA        DL_EVDNS_TYPE_AAAA

#define DL_CLASS_INET       DL_EVDNS_CLASS_INET

typedef int dl_handler;

/*******************/
/** Error codes 0-5 are as described in RFC 1035. */
#define DL_DNS_ERR_NONE             0
/** The name server was unable to interpret the query */
#define DL_DNS_ERR_FORMAT           -1
/** The name server was unable to process this query due to a problem with the
 * name server */
#define DL_DNS_ERR_SERVERFAILED     -2
/** The domain name does not exist */
#define DL_DNS_ERR_NOTEXIST         -3
/** The name server does not support the requested kind of query */
#define DL_DNS_ERR_NOTIMPL          -4
/** The name server refuses to reform the specified operation for policy
 * reasons */
#define DL_DNS_ERR_REFUSED          -5
/** The reply was truncated or ill-formatted */
#define DL_DNS_ERR_TRUNCATED        -65
/** An unknown error occurred */
#define DL_DNS_ERR_UNKNOWN          -66
/** Communication with the server timed out */
#define DL_DNS_ERR_TIMEOUT          -67
/** The request was canceled because the DNS subsystem was shut down. */
#define DL_DNS_ERR_SHUTDOWN         -68
/** The request was canceled via a call to evdns_cancel_request */
#define DL_DNS_ERR_CANCEL           -69
/** There were no answers and no error condition in the DNS packet.
 * This can happen when you ask for an address that exists, but a record
 * type that doesn't. */
#define DL_DNS_ERR_NODATA           -70

#define DL_DNS_ERR_TTL              -100
#define DL_DNS_ERR_EMPTY_REPONSE    -101

#define DL_DNS_IPv4_A       1
#define DL_DNS_PTR          2
#define DL_DNS_IPv6_AAAA    3

/* maximum number of addresses from a single packet */
/* that we bother recording */
#define DL_MAX_V4_ADDRS     32
#define DL_MAX_V6_ADDRS     32

#ifndef DL_HOST_NAME_MAX
#define DL_HOST_NAME_MAX    255
#endif

#define DL_MAX_LABELS       128

#define DL_MAX_NAMESERVER   6

#define    DL_RESOLV_METHOD_STATIC    (0x1<<0)
#define    DL_RESOLV_METHOD_DYNAMIC   (0x1<<1)
#define    DL_RESOLV_METHOD_MAX       2

typedef struct reply {
    unsigned int ttl;
    unsigned int type;
    unsigned int have_answer : 1;
    union {
        struct {
            unsigned int addrcount;
            unsigned int addresses[DL_MAX_V4_ADDRS];
        } a;
        struct {
            unsigned int addrcount;
            struct in6_addr addresses[DL_MAX_V6_ADDRS];
        } aaaa;
        struct {
            char name[DL_HOST_NAME_MAX];
        } ptr;
        struct {
            char cname[DL_HOST_NAME_MAX];
        }cname;
    } data;
}dl_reply_t;
#endif

#define GET_METHOD 0
#define POST_METHOD 1
#define PUT_METHOD 2
#define DELETE_METHOD 3

/* old log mode */
enum _DL_MODE {
    DL_LOG_MODE_NONE =    0x0000,
    DL_LOG_MODE_SYS  =    0x0001, /* �ô�λ ��LOG�������SYSLOG */
    DL_LOG_MODE_CONS =    0x0002, /* �ô�λ ��LOG��������ն� */
    DL_LOG_MODE_TIME =    0x0004, /* �ô�λ LOG������ʱ�� */
};

typedef int (*dl_log_redirect_cb)(const char *msg, int len);

/*
* int dl_open_old(dl_conn_info *conf, dl_handler *hander, char *new_url, dl_response_t res)
* ��    �ܣ� libdownload Openͳһ�����ӿ�
* ��ڲ����� conf,��Ҫ�����û���һЩ����
* ���ڲ����� handler,�����û�����dl_close dl_read
            new_url,���ڱ���http�ض�����url
            res,���ڱ��������ʹ�����Ϣ
* ����ֵ��   0 �ɹ� -1 ʧ��
*/
DLLEXPORT int dl_open_old(dl_conn_info *conf, dl_handler *handler, char *new_url, dl_response_t *res);

/*
* int dl_download_old(dl_conn_info *conf, char *new_url, dl_response_t *res)
* ��    �ܣ� libdownload Downloadͳһ�����ӿ�
* ��ڲ����� conf,�ļ���������
            local_path, ���ر���·��
* ���ڲ����� new_url,�ض�����µ�url
            res,������ʹ�����Ϣ
* ����ֵ��   �ɹ�,0;ʧ��,-1;
*/
DLLEXPORT int dl_download_old(dl_conn_info *conf, char *local_path, char *new_url, dl_response_t *res);


/*
* int dl_upload_old(dl_conn_info *conf, char *local_path, char *remote_name,
*        char *new_url, dl_response_t *res, char *http_header, int ftp_postmode, int ftp_limitmode);
* ��    �ܣ� libdownload Uploadͳһ�����ӿ�
* ��ڲ����� conf,�ļ��ϴ�����
            local_path,������Ҫ�ϴ����ļ�
            remote_name,tftp/ftp�ϴ�����ļ��������Ϊ�գ���Ĭ�Ϻͱ����ļ���һ��
            http_header,�ϴ�http�ļ���Ҫ�����Ķ���ͷ��
            param_ex1, �ϴ���ʽ�������ftp�ϴ� 1 APPEND 2 STOR(����)
                                �����http�ϴ� 1.POST 2 PUT
            param_ex2, ftp�ϴ������ļ����ƺ��ѡ��:1 DELETE(����) 2 STOP(�˳�)
* ���ڲ����� new_url,�ض�����µ�url
            res,������ʹ�����Ϣ
* ����ֵ��   �ɹ�,0;ʧ��,-1;
*/
DLLEXPORT int dl_upload_old(dl_conn_info *conf, char *local_path, char *remote_name,
        char *new_url, dl_response_t *res, char *http_header, int param_ex1, int param_ex2);

/*
* int dl_log_init_old(int log_mode, dl_log_redirect_cb redir_cb);
* ��    �ܣ� ��ʼ��log
* ��ڲ�����log_mode : logģʽ(�Ѳ�֧��DL_LOG_MODE_SYS��DL_LOG_MODE_CONSͬʱ���ã����������SYS)
*           redir_cb : �ص�����ָ�� msgΪlog�ַ���, lenΪ�ַ�������(�Ѳ�֧������)
* ���ڲ�����
* ����ֵ��
* ��    ע�� ���ӿڽ��鲻ʹ�ã���ʹ��dl_log_new_init��
*
*/
DLLEXPORT int dl_log_init_old(int log_mode, dl_log_redirect_cb redir_cb);

DLLEXPORT int dl_log_new_init(dl_log_type_e type, dl_log_new_mode_e mode, const char *file);

/*
* int dl_http_open(dl_http_handler *handler, dl_url_info *url_info, char *new_url, dl_response_t *ret_response)
* ��    �ܣ� ��HTTP����
* ��ڲ����� url_info:���ӵ�URL
* ���ڲ����� handler:http���ָ�� ret_response:�������Ӧ��Ϣ new_url:�ض����ַ
* ����ֵ��   0 �ɹ� -1 ʧ��
* ��    ע��
*/
DLLEXPORT int dl_http_open(dl_http_handler *handler, dl_url_info *url_info, char *new_url, dl_response_t *ret_response);

/*
* int dl_http_open_ex(dl_http_handler *handler, dl_url_info_ex *url_info_ex, char *new_url, dl_response_t *ret_response)
* ��    �ܣ� ��HTTP����
* ��ڲ����� url_info_ex:����չ��Ϣ��URL
* ���ڲ����� handler:http���ָ�� ret_response:�������Ӧ��Ϣ new_url:�ض����ַ
* ����ֵ��   0 �ɹ� -1 ʧ��
* ��    ע��
*/
DLLEXPORT int dl_http_open_ex(dl_http_handler *handler, dl_url_info_ex *url_info_ex, char *new_url, dl_response_t *ret_response);

/*
* int dl_http_read(dl_http_handler handler, char *buffer, int len, dl_response_t *res)
* ��    �ܣ� ��ȡHTTP content����
* ��ڲ����� buffer:�������ݻ��� len:buffer����
* ���ڲ����� res:���ش��������Ϣ
* ����ֵ��   �������ֽ���
* ��    ע��
*/
DLLEXPORT int dl_http_read(dl_http_handler handler, char *buffer, int len, dl_response_t *res);


/*
* void dl_http_close(dl_http_handler handler)
* ��    �ܣ� �ر�http����
* ��ڲ����� handler: http���
* ���ڲ�����
* ����ֵ��
* ��    ע��
*/
DLLEXPORT void dl_http_close(dl_http_handler handler);


/*
* int dl_http_recv_to_file(dl_url_info *url_info, char *filename, char *new_url, dl_response_t *res)
* ��    �ܣ� ֱ�������ļ�
* ��ڲ����� url_info:URL��Ϣ filename:�����ļ��� Ϊ�� ʹ��URL���ļ���
* ���ڲ����� new_url:�ض����ַ, res:���������Ϣ
* ����ֵ��   0 �ɹ� -1 ʧ��
* ��    ע��
*/
DLLEXPORT int dl_http_recv_to_file(dl_url_info *url_info, char *filename, char *new_url, dl_response_t *res);

/*
* int dl_http_recv_to_file_ex(dl_url_info_ex *url_info_ex, char *filename, char *new_url, dl_response_t *res)
* ��    �ܣ�ֱ�������ļ�
* ��ڲ����� url_info_ex:����չ��URL��Ϣ filename:�����ļ��� Ϊ�� ʹ��URL���ļ���
* ���ڲ����� new_url:�ض����ַ, res:���������Ϣ
* ����ֵ��   0 �ɹ� -1 ʧ��
* ��    ע��
*/
DLLEXPORT int dl_http_recv_to_file_ex(dl_url_info_ex *url_info_ex, char *filename, char *new_url, dl_response_t *res);

/*
* int dl_http_post_file(dl_http_handler *handler, dl_url_info *url_info, char *filename, char *new_url, dl_response_t *ret_response)
* ��    �ܣ� ʹ��post�����ϴ��ļ�
* ��ڲ����� url_info:���ӵ�URL  filename:Ҫ�ϴ����ļ�·��
* ���ڲ����� handler:http���ָ�� ret_response:�������Ӧ��Ϣ new_url:�ض����ַ
* ����ֵ��   0 �ɹ� -1 ʧ��
* ��    ע��
*/
DLLEXPORT int dl_http_post_file(dl_http_handler *handler, dl_url_info *url_info, char *filename, char *new_url, dl_response_t *ret_response);

/*
* int dl_distinctive_ringtone_download(const char *url, const char *info, int *index, char *filepath, int pathlen)
* ��    �ܣ� ������������
* ��ڲ����� url:���ص�URL info:������Ϣ filepath:�ļ�·������, pathlen:���泤��
* ���ڲ����� index:��������
* ����ֵ��   0ʹ��Ĭ�� 1 �ҵ���������
* ��    ע�����Ȳ���URL ���ж�info
*/
DLLEXPORT int dl_distinctive_ringtone_download(const char *url, const char *info, int *index, char *filepath, int pathlen);


/*
* int dl_tftp_open(dl_tftp_handler *handler, const char *host, int port, const char *file_path, dl_response_t *res)
* ��    �ܣ� ��ʼ��TFTP
* ��ڲ����� host:tftp��������IP port:�˿ں� Ϊ0 ��ʹ��Ĭ�� remote_name:Ҫ�����ļ���
* ���ڲ����� handler:tftp������ ���ó����뵽��ָ�� res:�������Ӧ��Ϣ
* ����ֵ��   0 �ɹ� -1 ʧ��
* ��    ע��
*/
DLLEXPORT int dl_tftp_open(dl_tftp_handler *handler, const char *host, int port, const char *file_path, dl_response_t *res);


/*
* int dl_tftp_read(dl_tftp_handler handler,char * buffer,int len,dl_response_t * res)
* ��    �ܣ� ��ȡ�����ļ�����
* ��ڲ����� handler:TFTP��� buffer:�������ݻ��� len:buffer����
* ���ڲ����� res:���ش��������Ϣ
* ����ֵ��   �������ֽ���
* ��    ע��
*/
DLLEXPORT int dl_tftp_read(dl_tftp_handler handler, char *buffer, int len, dl_response_t *res);


/*
* void dl_tftp_close(dl_tftp_handler handler)
* ��    �ܣ� �ر�tftp
* ��ڲ����� handler: tftp���
* ���ڲ�����
* ����ֵ��
* ��    ע��
*/
DLLEXPORT void dl_tftp_close(dl_tftp_handler handler);


/*
* int dl_tftp_recv_to_file(char *host, int port, const char *file_path, const char *local_name, dl_response_t *res)
* ��    �ܣ� ֱ�������ļ�
* ��ڲ����� host:��������ip port:�˿ں� Ϊ0ʹ��Ĭ�� local_name:Ҫ������ļ���
* ���ڲ����� res:���������Ϣ
* ����ֵ��   0 �ɹ� -1 ʧ��
* ��    ע��
*/
DLLEXPORT int dl_tftp_recv_to_file(const char *host, int port, const char *file_path, const char *local_name, dl_response_t *res);


/*
* int dl_tftp_send_file(char *host, int port, const char *remote_name, const char *local_name, dl_response_t *res)
* ��    �ܣ� ֱ���ϴ��ļ�
* ��ڲ����� host:��������ip port:�˿ں� Ϊ0ʹ��Ĭ�� local_name:Ҫ�ϴ����ļ��� remote_name:�������ϱ�����ļ���
* ���ڲ����� res:���������Ϣ
* ����ֵ��   0 �ɹ� -1 ʧ��
* ��    ע�� file_path:�������ϱ����ļ��� Ϊ��ʱ��ʹ��ԭ�ļ���
*/
DLLEXPORT int dl_tftp_send_file(const char *host, int port, const char *file_path, const char *local_name, dl_response_t *res);

/*
* int dl_ftp_open(dl_ftp_handler *handler, const char *host, int port, const char *user, const char *passwd, const char *file_path, dl_response_t * res)
* ��    �ܣ� ����FTP����
* ��ڲ����� host:ftp��������IP port:�˿ں� Ϊ0 ��ʹ��Ĭ�� user:�û��� passwd:���� file_path:Ҫ�����ļ�·��
* ���ڲ����� handler:ftp������ ���ó����뵽��ָ�� res:�������Ӧ��Ϣ
* ����ֵ��   0 �ɹ� -1 ʧ��
* ��    ע��
*/
DLLEXPORT int dl_ftp_open(dl_ftp_handler *handler, const char *host, int port, const char *user, const char *passwd, const char *file_path, dl_response_t * res);


/*
* int dl_ftp_read(dl_ftp_handler handler, char *buf, int len, dl_response_t *res)
* ��    �ܣ� ��ȡ�����ļ�����
* ��ڲ����� handler: ftp��� buffer:�������ݻ��� len:buffer����
* ���ڲ����� res:���ش��������Ϣ
* ����ֵ��   �������ֽ���
* ��    ע��
*/
DLLEXPORT int dl_ftp_read(dl_ftp_handler handler, char *buf, int len, dl_response_t *res);


/*
* void dl_ftp_close(dl_ftp_handler handler)
* ��    �ܣ� �ر�ftp����
* ��ڲ����� handler: ftp���
* ���ڲ�����
* ����ֵ��
* ��    ע��
*/
DLLEXPORT void dl_ftp_close(dl_ftp_handler handler);


/*
* int dl_ftp_recv_to_file(const char * host,int port,const char * user, const char *passwd, const char *file_path, const char *local_name, dl_response_t *res)
* ��    �ܣ� ֱ�������ļ�
* ��ڲ����� host:��������ip port:�˿ں� Ϊ0ʹ��Ĭ�� user:�û��� passwd:���� file_path:�����ļ�·�� local_name:�����ļ��� Ϊ��ʱ��ʹ��ԭ�ļ��� �����浽��ǰ·��
* ���ڲ����� res:���������Ϣ
* ����ֵ��   0 �ɹ� -1ʧ��
* ��    ע��
*/
DLLEXPORT int dl_ftp_recv_to_file(const char *host, int port, const char *user, const char *passwd, const char *file_path, const char *local_name, dl_response_t *res);


/*
* int dl_ftp_send_file(const char * host,int port,const char * user, const char *passwd, const char *file_path, const char *local_name, dl_response_t *res)
* ��    �ܣ� ʹ��ftp�ϴ��ļ�
* ��ڲ����� host:��������ip port:�˿ں� Ϊ0ʹ��Ĭ�� user:�û��� passwd:���� remote_name:�������ϱ�����ļ�����Ϊ��ʱ��ʹ�ñ����ļ���
             local_name:Ҫ�ϴ����ļ� postmode:ftp�ϴ���ʽ��ѡ��1 APPEND 2 STOR(����) limitmode:�����ļ����ƺ��ѡ��:1 DELETE(����) 2 STOP(�˳�)
* ���ڲ����� res:���������Ϣ
* ����ֵ��   0 �ɹ� -1ʧ��
* ��    ע�� file_path:�������ϱ����ļ��� Ϊ��ʱ��ʹ��ԭ�ļ���
*/
DLLEXPORT int dl_ftp_send_file(const char *host, int port, const char *user, const char *passwd,
                        const char *file_path, const char *local_name, int postmode,
                        int limitmode, dl_response_t *res);

#ifndef WIN32
/*
* int dl_dns_resolv(const char *hostname, int request_type, int dl_class, struct reply *res);
* ��    �ܣ� ִ��DNS��ѯ
* ��ڲ����� hostname:����
             request_type:�������� (DL_TYPE_AAAA/DL_TYPE_A/DL_TYPE_PTR/DL_TYPE_CNAME)
             dl_class:Э�������ͣ���ǰֻ֧��DL_CLASS_INET
             nretry: ���Դ���
             timeout:��ʱʱ��
* ���ڲ����� res:  ��ѯ���
* ����ֵ��   �ɹ�,0;ʧ��,-1;
*/

DLLEXPORT int dl_dns_resolv_timeout(const char *hostname, int request_type, int dl_class, struct reply *res, int nretry, int timeout);


/*
* int dl_dns_resolv(const char *hostname, int request_type, int dl_class, struct reply *res);
* ��    �ܣ� ִ��DNS��ѯ���ڲ���ʱ�ͳ��Դ�����options���þ���
* ��ڲ����� hostname:����
             request_type:�������� (DL_TYPE_AAAA/DL_TYPE_A/DL_TYPE_PTR/DL_TYPE_CNAME)
             dl_class:Э�������ͣ���ǰֻ֧��CLASS_INET
* ���ڲ����� res:  ��ѯ���
* ����ֵ��   �ɹ�,0;ʧ��,-1;
*/
DLLEXPORT int dl_dns_resolv(const char *hostname, int request_type, int dl_class, struct reply *res);

/*
* int dl_dns_set_resolv_seq(int *resolv_seq_arr, int count);
* ��    �ܣ� ����dns��ѯ���ȼ�
* ��ڲ����� resolv_seq_arr: ���ȼ��������飬��ǰ��֧��RESOLV_METHOD_STATIC��RESOLV_METHOD_DYNAMIC����
             count: �����С
* ���ڲ�����
* ����ֵ��   �ɹ�,0;ʧ��,-1;
*/
DLLEXPORT int dl_dns_set_resolv_seq(int *resolv_seq_arr, int count);

/*
* int dl_dns_set_nretry(int nretry);
* ��    �ܣ� ����dns��ѯ���Դ���(����ÿһ��nameserver)����������ã�Ĭ��Ϊ2��
* ��ڲ����� nretry ���Դ���
* ���ڲ�����
* ����ֵ��   �ɹ�,0;ʧ��,-1;
*/
DLLEXPORT int dl_dns_set_nretry(int nretry);

/*
* int dl_dns_set_nretry(int nretry);
* ��    �ܣ� ����dns��ѯ��ʱʱ��(����ÿһ��nameserver)����������ã�Ĭ��Ϊ5��
* ��ڲ����� timeout ��ʱʱ�䣬��λ��
* ���ڲ�����
* ����ֵ��   �ɹ�,0;ʧ��,-1;
*/
DLLEXPORT int dl_dns_set_timeout(int timeout);

/*
* int dl_dns_reload_resolv_conf();
* ��    �ܣ� ����load dns�����ļ�--��DNS�����ļ��и��µ�ʱ����Ҫ���µ��ô˺�����ͨ��DL_OPTION_RESOLV_FILE
             ѡ�����ã�����������ؿ�Ľӿ�ȥ����������ʽ�ķ�������ɲ��ص��ô˽��ӿڣ�����Ļ�������Ҫ���е���
* ��ڲ�����
* ���ڲ�����
* ����ֵ��   �ɹ�,0;ʧ��,-1;
*/
DLLEXPORT int dl_dns_reload_resolv_conf();

/*
* int dl_dns_resolv_conf_destroy();
* ��    �ܣ� ���dns����
* ��ڲ�����
* ���ڲ�����
* ����ֵ��   �ɹ�,0;ʧ��,-1;
*/
DLLEXPORT int dl_dns_resolv_conf_destroy();

/*
* int dl_dns_hosts_conf_destroy();
* ��    �ܣ� ���dns hosts����
* ��ڲ�����
* ���ڲ�����
* ����ֵ��   �ɹ�,0;ʧ��,-1;
*/
DLLEXPORT int dl_dns_hosts_conf_destroy();

/*
* int dl_dns_reload_hosts_conf();
* ��    �ܣ� ����load dns hosts�����ļ�--��DNS�����ļ��и��µ�ʱ����Ҫ���µ��ô˺�����ͨ��DL_OPTION_HOSTS_FILE
             ѡ�����ã�����������ؿ�Ľӿ�ȥ����������ʽ�ķ�������ɲ��ص��ô˽��ӿڣ�����Ļ�������Ҫ���е���
* ��ڲ�����
* ���ڲ�����
* ����ֵ��   �ɹ�,0;ʧ��,-1;
*/
DLLEXPORT int dl_dns_reload_hosts_conf();
#endif


#define GET_METHOD          0
#define POST_METHOD         1
#define PUT_METHOD          2
#define DELETE_METHOD       3

typedef dl_response_t response_t;

enum {
    SUCCESS = 0,
    ERRHOST = -1,               /* �����HOST */
    ERRSOCK = -2,               /* �޷������׽��� */
    ERRCONN = -3,               /* �޷����� */
    ERRWRDT = -4,               /* �������ݳ��� */
    ERRPAHD = -5,               /* ���յ���Ч������ */
    ERRPARAMETER = -6,          /* ������� */
    ERRMEM = -7,                /* �ڴ�ռ䲻�� */
    ERRRDDT = -8,               /* �����ݳ��� */
    ERRURLH = -9,               /* �����URL */
    ERRURLP = -10,              /* ����Ķ˿� */
    ERRAUTH = -11,              /* ��֤���� */
    ERRREDIRECT = -12,          /* �ض������ */
    ERRDISIPV6 = -13,           /* ipv6 ����ֹ */
    ERRUNINIT = -14,            /* δ��ʼ�� */
    ERRFTP = -15,               /* FTP���� */
    ERRTFTP = -16,              /* TFTP���� */
    ERR301PARSE = -17,          /* HTTP�ض���������� �����Э�� */
    ERRFTP_NOUSER = -18,        /* FTP �û��������� */
    ERRFTP_WRONG_PASSWD = -19,  /* FTP������� */
    ERR400 = 400,               /* ��Ч��ѯ */
    ERR403 = 403,               /* ����ֹ */
    ERR408 = 408,               /* ����ʱ */
    ERR500 = 500,               /* ���������� */
    ERR501 = 501,               /* ������δʵ�ַ��� */
    ERR503 = 503,               /* ������������ */
    OK_201 = 201,               /* ��Դ�ɹ������� */
    OK_200 = 200                /* �ɹ���ȡ���� */
};

#define PERMISSION_VERSION_AWARD "1.3.6.1.6.4.147.7.4.15"
#define UPGRADE_AWARD_KEY        "upgrade_award"


#ifdef __cplusplus
}
#endif

#endif
