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
#define DL_GET_METHOD 0 /* http get模式*/
#define DL_POST_METHOD 1
#define DL_PUT_METHOD 2
#define DL_DELETE_METHOD 3
#define DL_HEAD_METHOD 4 /* 只请求页面的首部，用于检测url的有效性 */

#define DL_POST_DETAILS_METHOD 10 /* post文件内容，模式同DL_POST_METHOD，但不会要求服务器进行保存 */

#define DL_CA_INFO_SIZE 64
#define DL_MAX_CA_NUM 10

#define DL_CLIENT_CERT 0x1 /* 客户端证书，带私钥 */

#define DL_DEFAULT_SSL_CIPHER_STR "RSA:!EXP:!LOW:!NULL:!MD5:@STRENGTH"

typedef struct _dl_config dl_handle; /* 局部主结构体 */

typedef int dl_stream; /* dl_open接口使用的句柄，使用后需要调用dl_close进行清除 */

typedef struct _dl_url_info
{
    int method;    /* http模式(默认自匹配,上传默认post);ftp上传方式的选择1 APPEND 2 STOR(覆盖) */
    int limitmode; /* ftp上传超过文件限制后的选择:1 DELETE(覆盖) 2 STOP(退出) */
    int recvlen;   /* 需要读取的长度，从body首字节开始计算，为0表示全部读取(default) */
    int http_auth; /* http鉴权方式 */
    char *url;     /* url地址, 格式固定为网络协议://ip or domain/xxx(如 http://www.yealink.com) */
    char *proxyurl;
    char *proxy_url;
    char *user;    /* 用户名，置空表示匿名访问 */
    char *passwd;  /* 密码，置空表示使用空密码访问 */
    char *header;  /* http特有功能 */
    char *body;    /* http特有功能，上传文件时，该部分内容无效，建议置空避免浪费内存 */

    int keepalive; /* 默认为不对连接进行保留，0 使用短连接，1 申请长连接(仅curl库版本libdownload支持)
                    http特有功能，使用长连接，并保持，如连接被占用则libdownload库另申请一条短连接暂替 */
} dl_url_info_t;

/* 用途: 该结构体用于存放url接口请求的对应响应信息 */
typedef struct _dl_response
{
    int code;           /* 响应的错误码，值对应dl_errcode_e枚举表 */
    char code_msg[256]; /* 对应的错误描述 */
    char new_url[512];  /* http重定向地址 */
} dl_response_t;

typedef struct _dl_ca_detail
{
    unsigned char version;                     /* 版本 */
    char serial_num[DL_CA_INFO_SIZE];          /* 序列号 */
    char signature_algorithm[DL_CA_INFO_SIZE]; /* 签名算法 */
    char pubkey_algorithm[DL_CA_INFO_SIZE];    /* 公钥算法 */
    int pubkey_bits;                           /* 公钥长度 */
    char priv_key_algor[DL_CA_INFO_SIZE];      /* 私钥算法 */
    int priv_key_bits;                         /* 私钥长度 */
    char issued_by[DL_CA_INFO_SIZE];           /* 颁发者Common Name */
    char issued_to[DL_CA_INFO_SIZE];           /* 持有者Common Name */
    unsigned long hash;                        /* 证书的hash值 */
    char validity_begin[DL_CA_INFO_SIZE];      /* 有效期开始时间 GMT */
    char expire_date[DL_CA_INFO_SIZE];         /* 到期时间 GMT */
    char file_path[DL_CA_INFO_SIZE];           /* 证书路径，用于保存证书路径 */
    unsigned char extensions_num;              /* 扩展项个数 */
    unsigned short data_len;                   /* 保留 */
    void *data_ptr;                            /* 保留 */
} dl_ca_detail_t;

typedef struct _dl_ca_detail_table_t
{
    int cnt;
    dl_ca_detail_t ca_infos[DL_MAX_CA_NUM];
} dl_ca_detail_table_t;

typedef enum _dl_http_auth {
    DL_HTTP_AUTH_NONE = 0,  /* 无效的鉴权定义 */
    DL_HTTP_MUTIL_AUTH = 1, /* 自动识别鉴权方式 */
    DL_HTTP_BASIC_AUTH,
    DL_HTTP_DIGEST_AUTH,
    DL_HTTP_NTLM_AUTH,
    DL_HTTP_NEGOTIATE,          /* 协商鉴权 */
    DL_HTTP_SIP_XSI_AUTH,       /* SIP XSI鉴权(basic 64变种) */
    DL_HTTP_NO_TRY_REDIRECT,    /* 不进行重定向 */
    DL_HTTP_NO_TRY_AUTH,        /* 不进行401鉴权，return success and 401 response code  */
    DL_HTTP_FUZE_HOTELING_AUTH, /* HOTELINE 鉴权(非规范鉴权方式，特制) */
    DL_HTTP_END,
} dl_http_auth_e; /* 用于http鉴权设定 */

typedef enum _dl_options {
    DL_OPTIONS_USER_AGENT = 0,          /* 用户代理信息 */
    DL_OPTIONS_SSL_CA_TRUST_PATH,       /* 受信任的证书路径 */
    DL_OPTIONS_SSL_CA_SERVER_FILE,      /* 服务端证书文件 */
    DL_OPTIONS_SSL_METHOD,              /* SSL加密方式 */
    DL_OPTIONS_SSL_TRUST,               /* 是否启用ssl信任 */
    DL_OPTIONS_SSL_CHECK_CN,            /* 校验服务端host，在不启用SSL信任时无效 */
    DL_OPTIONS_MAX_REDIRECT,            /* 最大重定向次数，仅http有效 */
    DL_OPTIONS_MAX_FILESIZE,            /* 最大下载文件，当url文件超过限定大小，则返回错误 */
    DL_OPTIONS_TIMEOUT,                 /* 请求超时时间 */
    DL_OPTIONS_USEIPV6,                 /* 支持IPV6，默认为0 不支持 */
    DL_OPTIONS_SSL_CA_TRUST_USE_PATH,   /* 新增信任和服务器证书的使用路径，原证书路径option只用在安装、显示和删除上 */
    DL_OPTIONS_SSL_CA_SERVER_USE_FILE,  /* 网络连接请求使用的证书文件 */
    DL_OPTIONS_SSL_CA_UC_WEB_CERT_PATH, /* sip register cert install path */
    DL_OPTIONS_MAC_ADDR,                /* 物理地址 */
    DL_OPTIONS_AUTH_TIMES,              /* 鉴权时间 */
    DL_OPTIONS_SSL_PEM_PASSWORD,        /* 证书密码 */
    DL_OPTION_CONNECT_TIMEOUT,          /* 连接超时时间，仅作用于网络建立过程 */
    DL_OPTION_READ_TIMEOUT,             /* 数据读取时间，该项与TIMEOUT存在重叠，已失效 */
    DL_OPTION_ENV_RANDOMNESS,           /* 环境变量randomness */
    DL_OPTIONS_SSL_CIPHER_STR,          /* ssl算法字符串 */
    DL_OPTIONS_TRANS_DATA_TRACE,        /* 传输数据打印 */
    DL_OPTION_CONFIG_PATH_PREFIX,       /* 设置配置文件的前缀路径，比如.\\config\\,.\\confog_PART_A\\,.\\config_PART_B\\ */
    DL_OPTION_RESOLV_FILE,              /* 设置DNS配置文件路径，WINDOWS不支持此配置 */
    DL_OPTION_HOSTS_FILE,               /* 设置hosts文件，默认为/etc/hosts, WINDOWS不支持此配置 */
    DL_OPTION_RESOLV_NOSYS,             /* 开启并使用非系统接口执行DNS请求 */
    DL_OPTION_ADDITION_BODY,            /* FUZE Hoteling鉴权时增加的额外body */
    DL_OPTION_CLIENT_CERT_ALGORITHM,    /* 话机设备证书签名算法 */

    DL_OPTION_LOW_SPEED_LIMIT, /* 最低传输速度，单位byte/s，默认100byte/s */
    DL_OPTION_LOW_SPEED_TIME,  /* 最低传输速度超时时间，默认不超时 */

    DL_OPTION_NET_PREFERRED, /* 网络优先级，0 IPV6(默认)，1 IPV4 */

    DL_OPTION_PROXY_MOTHOH, /* 代理服务器模式 */
    DL_OPTION_PROXY_HOST,   /* 代理服务器host */
    DL_OPTION_PROXY_PORT,   /* 代理服务器端口 */

    DL_OPTIONS_RING_CFG,
    DL_OPTION_USER_CFG,
    DL_OPTIONS_END
} dl_options_e;

/* 网络传输请求错误码，对应CURL中的错误码 */
typedef enum _dl_err_code {
    DL_SUCCESS = 0,

    /* 逻辑处理定义错误码 */
    DL_ERRUNDEFINED = -1,   /* 未定义错误 */
    DL_ERRPARAMETER = -2,   /* 错误参数 */
    DL_ERRCURLINIT = -3,    /* curl创建失败 */
    DL_ERRAUTH = -4,        /* 认证出错 */
    DL_ERRDISIPV6 = -5,     /* ipv6 被禁止 */
    DL_ERRPAHD = -6,        /* 接收到无效的数据，无效错误码 */
    DL_ERRURLP = -7,        /* 错误的端口，无效错误码 */
    DL_ERRFTP = -8,         /* FTP出错，无效错误码 */
    DL_ERRTFTP = -9,        /* TFTP出错，无效错误码 */
    DL_ERRTLSVERSION = -10, /* TLS版本不兼容，无效错误码*/
    DL_ERRINVALID_GRANT = -26,      /* 无效的用户凭证 */

    /* CURL内部对应的错误码信息 */
    DL_ERRUNINIT = 2,                 /* 未初始化 */
    DL_ERRURLH = 3,                   /* 错误的URL */
    DL_ERRPROXYHOST = 5,              /* 代理服务器解析失败 */
    DL_ERRHOST = 6,                   /* 错误的HOST，域名解析失败 */
    DL_ERRCONN = 7,                   /* 无法连接 */
    DL_ERRFTP_WRONG_PASSWD = 11,      /* FTP密码错误 */
    DL_ERR_NOFULLY = 23,              /* 未读取完整，当有指定读取长度是，该错误码可认为正确 */
    DL_ERRMEM = 27,                   /* 内存空间不够 */
    DL_ERRTIMEOVER = 28,              /* 请求超时 */
    DL_ERRPOSTDATA = 34,              /* http post失败 */
    DL_ERRSSLCONNECT = 35,            /* SSL握手失败 */
    DL_ERRREDIRECT = 47,              /* 重定向出错 */
    DL_ERRSERVERCERT = 51,            /* 服务器端证书错误，SSL证书或指纹出错 */
    DL_ERRGOT_NOTING = 52,            /* 服务器无有效数据返回 */
    DL_ERRSSL_ENGINE = 53,            /* SSL加密引擎为发现 */
    DL_ERRSSL_SETFAILED = 54,         /* SSL设置默认方式失败 */
    DL_ERRWRDT = 55,                  /* 发送数据出错 */
    DL_ERRRDDT = 56,                  /* 读数据出错 */
    DL_ERRCLINETCERT = 58,            /* 客户端证书错误 */
    DL_ERRSSL_CIPHER = 59,            /* SSL无法使用指定的密码 */
    DL_ERRSSL_CACERT = 60,            /* SSL CA证书问题 */
    DL_ERRFILESIZE = 63,              /* 文件大小超出 */
    DL_ERRLOGIN_DENIED = 67,          /* 用户名密码登录失败 */
    DL_ERRFTP_NOUSER = 74,            /* FTP 用户名不存在 */
    DL_ERRSSL_CACERT_BADFILE = 77,    /* SSL无法加载CA，CA错误或丢失 */
    DL_ERRSSL_SHUTDOWN = 80,          /* SSL关闭连接失败 */
    DL_ERRSOCK = 81,                  /* Socket未准备好 */
    DL_ERRSSL_CRL_BADFILE = 82,       /* SSL无法加载CRL文件 */
    DL_ERRSSL_ISSUER = 83,            /* SSL发行人错误 */
    DL_ERRNO_CONNECTION = 89,         /* 无可用的连接 */
    DL_SSL_PINNEDPUBKEYNOTMATCH = 90, /* SSL公钥未匹配 */
    DL_SSL_INVALIDCERTSTATUS = 91,    /* SSL无效的证书 */

    /* HTTP对应的响应码 */
    DL_ERR301PARSE = 301, /* HTTP重定向解析出错 需更换协议 */
    DL_ERR301 = 301,      /* 301临时重定向 */
    DL_ERR302 = 302,      /* 302重定向 */
    DL_ERR400 = 400,      /* 无效查询 */
    DL_ERR401 = 401,      /* HTTP鉴权失败 */
    DL_ERR403 = 403,      /* 被禁止 */
    DL_ERR408 = 408,      /* 请求超时 */
    DL_ERR500 = 500,      /* 服务器出错 */
    DL_ERR501 = 501,      /* 服务器未实现方法 */
    DL_ERR503 = 503,      /* 服务器超负荷 */
    DL_OK_201 = 201,      /* 资源成功被创建 */
    DL_OK_200 = 200       /* 成功读取数据 */
} dl_err_code_e;

typedef enum _dl_cert_err_code {
    DL_VERIFY_OK = 0,
    DL_ERR_PARAMETER,         /* 错误参数 */
    DL_ERR_INTERNAL,          /* 内部出错，包括库、上下文初始化、内存申请出错、打开文件路径出错等 */
    DL_ERR_SIGNATURE_FAILURE, /* 签名算法错误 */
    DL_ERR_NOT_YET_VAILD,     /* 证书有效期未开始 */
    DL_ERR_HAS_EXPIRED,       /* 证书过期 */
    DL_ERR_CERT_CHAIN,        /* 证书链不全或者证书只有一个item但非自签名证书 */
    /* 客户端证书专属 */
    DL_ERR_METHOD,             /* 错误的SSL方法 */
    DL_ERR_NO_PUBLIC_KEY,      /* 证书无公钥 */
    DL_ERR_NO_PRIVATE_KEY,     /* 证书无私钥 */
    DL_ERR_PRIV_KEY_NOT_MATCH, /* 公钥私钥不匹配，如私钥密码错误 */
    DL_ERR_UNKNOWN,            /* 未知错误 */
    DL_ERR_UC_CERT_EXIST,      /* UC证书已经存在 */
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
    DL_SSL_DEFAULT = 3, /* 默认为自匹配，与DL_SSLV23一致 */
    DL_TLSV1 = 0,
    DL_SSLV2 = 1,
    DL_SSLV3 = 2,
    DL_SSLV23 = 3,   /* 自匹配 */
    /* DL_TLSV10, */ /* 不使用TLS1.0 */
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
/*          对外结构体命名转译                                            */
/************************************************************************/
/* FIXME:修改头文件规范，由于旧版本的命名已被外部使用，进行重定义，建议不再使用 */
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
* 功    能： 初始化配置
* 入口参数：
* 出口参数：
* 返回值：
* 备    注：
*/
DLLEXPORT void dl_config_init(void);

/*
* void dl_exit()
* 功    能： 释放下载库缓存数据
* 入口参数：
* 出口参数：
* 返回值：
* 备    注：
*/
DLLEXPORT void dl_exit(void);

/*
* const char *dl_get_version()
* 功    能： 获取当前版本号
* 入口参数：
* 出口参数：
* 返回值：   版本号字符串
* 备    注：
*/
DLLEXPORT const char *dl_get_version(void);

/*
* int dl_log_init(dl_log_type_e type, dl_log_new_mode_e mode, const char *file);
* 功    能： 初始化log
* 入口参数： type : log类型
*            mode : log模式
*            file : 保存log文件的绝对路径
* 出口参数：
* 返回值：
* 备    注:  log模块修改后新添加的接口
*/
DLLEXPORT int dl_log_init(dl_log_type_e type, dl_log_new_mode_e mode, const char *file);

/*
* int dl_set_log_module_level(dl_log_module_e module, dl_log_level_e level);
* 功    能： 根据模块设置log等级
* 入口参数： module:模块编号，level:模块log等级
* 出口参数：
* 返回值：
* 备    注：
*/
DLLEXPORT int dl_set_log_module_level(dl_log_module_e module, dl_log_level_e level);
/*
* int dl_set_option(dl_options_e dl_opt, void *value);
* 功    能： 设置配置选项
* 入口参数： dl_opt:要设置的选项， value:要设置的值
* 出口参数：
* 返回值：   0 成功 -1 失败
* 备    注：
*/
DLLEXPORT int dl_set_option(dl_options_e dl_opt, const void *value);

/*
* int dl_get_option(dl_options_e dl_opt, void *buf, int size);
* 功    能： 读取配置选项
* 入口参数： dl_opt:要读取的选项，size: 读取数据的大小
* 出口参数： buf: 读取数据缓冲区
* 返回值：   0 成功 -1 失败
* 备    注：
*/
DLLEXPORT int dl_get_option(dl_options_e dl_opt, void *buf, int size);

/*
* char *dl_http_find_header_by_name(dl_http_handler handler, const char *name)
* 功    能： 获取HTTP头域
* 入口参数： handle: http句柄, name:要获取的头域名
* 出口参数：
* 返回值：   返回该头域指针，若查找不到返回NULL;
* 备    注： 内存由内部释放，调用dl_http_close会释放掉内存。
             (请勿在调用close后再获取，或者close后再次使用该指针)
*/
DLLEXPORT char *dl_http_find_header_by_name(dl_stream handle, const char *name);

/************************************************************************/
/*                        NEW APIS                                      */
/************************************************************************/

/*
* 功能：申请dl_handle结构体指针（使用结束后需释放）
* 输入：NULL
* 输出：NULL
* 返回：handle指针，NULL表示获取失败
* 备注：用于替换dl_config_init接口，建议不再使用dl_config_init接口
*/
DLLEXPORT dl_handle *dl_easy_init(void);

/*
* 功能：用于设置handle结构体成员，以及设置全局变量
* 输入：handle 结构体指针，为空表示设置全局；非空时并全局对应变量为空时，也会设到全局
* 输入：opt 枚举表定义了对应的功能
* 输入：value 需要写入的值
* 返回：0 成功，非0失败
* 备注：
*/
DLLEXPORT int dl_easy_setopt(dl_handle *handle, dl_options_e opt, const void *value);

/*
* 功能：用于获取handle的结构体体成员
* 输入：handle结构体指针，为空表示读取全局，非空表示读取指针对应的成员
* 输入：opt 枚举表定义了对应的功能
* 输入：size 需要保存的长度
* 输出：buf 读取缓冲区
* 返回：0 成功，非0失败
* 备注：
*/
DLLEXPORT int dl_easy_getinfo(dl_handle *handle, dl_options_e opt, void *buf, int size);

/*
* 功能：用于清除handle局部变量
* 输入：handle结构体指针，不能为空
* 输出：NULL
* 返回：NULL
* 备注：
*/
DLLEXPORT void dl_easy_cleanup(dl_handle *handle);

/*
* 功能：下载指定的url对应内容
* 输入：handle结构体指针，为空时表示使用全局或默认参数进行下载
* 输入：url_info url对应的信息，指针不能为空，内部成员根据结构体备注而定
* 输入：dl_recv_callback 回调函数
* 输出：out 读取缓冲区指针地址，内部申请空间，需释放
* 输出：res 接口执行的信息，建议非空
* 返回：0成功，非0失败
* 备注：
*/
DLLEXPORT int dl_recv_stream(dl_handle *handle, dl_url_info_t *url_info, char **out, dl_response_t *res);

/*
* 功能：下载指定的url对应内容至文件中
* 输入：handle结构体指针，为空时表示使用全局或默认参数进行下载
* 输入：url_info url对应的信息，指针不能为空，内部成员根据结构体备注而定
* 输入：filename 需要保存的文件名，带绝对路径，不能为空
* 输出：res 接口执行的信息，建议非空
* 返回：0成功，非0失败
* 备注：
*/
DLLEXPORT int dl_download(dl_handle *handle, dl_url_info_t *url_info, const char *file, dl_response_t *res);

/*
* 功能：上传指定文件到服务器
* 输入：handle结构体指针，为空时表示使用全局或默认参数进行下载
* 输入：url_info url对应的信息，指针不能为空，内部成员根据结构体备注而定
* 输入：filename 需要上传的文件，带绝对路径，不能为空
* 输入：snedname 上传至服务器的名称，为空时则使用filename对应的文件名参数进行发送
* 输出：res 接口执行的信息，建议非空
* 返回：0成功，非0失败
* 备注：
*/
DLLEXPORT int dl_upload(dl_handle *handle, dl_url_info_t *url_info, const char *file, const char *sendname, dl_response_t *res);

/*
* 功能: 创建curl download异步线程，并将数据流阻塞，等待dl_stream_recv读取
* 输入: url_info 请求所需的参数结构体
* 输入: dl_stream 输出内部申请的处理句柄
* 输出: res 错误码及错误描述
* 返回: 0 正常，-1失败（详细错误：res中的错误码及描述）
* 备注: 使用结束需调用dl_stream_close
*/
DLLEXPORT int dl_open(dl_stream *handle, dl_url_info_t *url_info, dl_response_t *res);

/*
* 功能: 读取url数据流
* 输入: dl_stream dl_stream_open接口申请的处理句柄
* 输入: len 需要读取的长度
* 输出: buf 数据存储缓存区
* 输出: res 错误码及错误描述
* 返回: 0 正常，-1失败（详细错误：res中的错误码及描述）
*/
DLLEXPORT int dl_read(dl_stream handle, char *buf, int len, dl_response_t *res);

/*
* 功能: 关闭创建的tcp连接及示范相应资源
* 输入: dl_stream dl_stream_open接口申请的处理句柄
* 返回: NULL
*/
DLLEXPORT void dl_close(dl_stream handle);

/*
* 功能: 计算输入字符串的md5值
* 输入: input 需要计算的字符串，类型unsigned char
* 输出: outbuf 得到的MD5值，注该缓存区长度为16
* 返回: NULL
* 备注: 该接口直接return Curl_md5it接口
*/
DLLEXPORT void dl_Curl_md5it(unsigned char *outbuf, const unsigned char *input);

/*
* 功能: 打印出Libdownload中内存使用情况
* 输入: info 输出内存的占用信息
* 输入: dump 输出当前未释放的内存申请记录
* 输入: check 输出内存校验情况
* 返回: 无
*/
DLLEXPORT void dl_memory_dump(int info, int dump, int check);

/*
* 功能: 释放Libdownload库申请的内存
* 输入: p 需要是否的指针
* 备注: 输入的指针需是Libdownload分配空间的，另Libdownload申请的资源需要使用该接口释放
*/
DLLEXPORT void dl_memory_free(void *p);

/************************************************************************/
/*                        证书操作接口或证书鉴权                        */
/************************************************************************/

/*
* int dl_install_ca(const char *file)
* 功    能： 安装证书
* 入口参数： file: 文件绝对路径
* 出口参数：
* 返回值：   -1:失败 -2:已经存在 -3:出厂已预制
* 备    注：
*/
DLLEXPORT int dl_install_ca(const char *file);

/*
* int dl_del_ca(const char *file)
* 功    能： 删除证书
* 入口参数： file: 文件绝对路径
* 出口参数：
* 返回值：   0成功 -1 失败
* 备    注：
*/
DLLEXPORT int dl_del_ca(const char *file);

/*
* int dl_del_ser_ca()
* 功    能： 删除服务器证书"server certificate"
* 入口参数：
* 出口参数：
* 返回值：   0成功 -1 失败
* 备    注： 路径由Option DL_OPTIONS_SSL_CA_SERVER_FILE指定
*/
DLLEXPORT int dl_del_ser_ca();

/*
* int dl_install_ser_ca(const char * file)
* 功    能： 安装WEB服务器证书
* 入口参数： file: 文件绝对路径
* 出口参数：
* 返回值：   0成功 -1 非PEM格式 -2 文件无私钥 -3 文件无公钥
*                   -4 不配对 -5 初始化失败 -6 创建目录失败
                    -7 已经存在 -8 出厂已预制
* 备    注：
*/
DLLEXPORT int dl_install_ser_ca(const char *file);

/**
* @breif 安装设备证书，支持上传单独证书(不含私钥)、单独私钥和完整的设备证书(含私钥)
* @param file:证书或私钥文件
* @ret 0 成功(即至少符合三种情况的一种)
*     -1 失败 导入的文件不存在或格式错误或内部错误
*
* 注意:1.本接口不判断证书的x509信息是否与已存在的预置、自定义证书相同，因为分开上传，可能会出现同一个x509信息证书，但是不同私钥的问题!!
*      2.本接口不判断证书匹不匹配
*/
DLLEXPORT int dl_install_ser_ca_separate(const char *file);

/**
* @breif 比较两个证书是否相同
* @param filepath1 证书1全路径
* @param filepath2 证书2全路径
* @ret 0 两个证书一样
*      其他值，两个证书不一样
*
*/
DLLEXPORT int dl_ca_cmp(const char *filepath1, const char *filepath2);

/*
* int dl_get_trust_ca_info(dl_ca_info_table *table)
* 功    能： 获取信任证书信息
* 入口参数：
* 出口参数： table: 信任证书文件信息表
* 返回值：   0 成功 -1 错误参数 -2 未设置信任目录或无目录
* 备    注：
*/
DLLEXPORT int dl_get_trust_ca_info(dl_ca_info_table_t *table);

/*
* int dl_get_ser_ca_info(dl_ca_info_t *info)
* 功    能： 获取服务器证书信息
* 入口参数：
* 出口参数： info: 服务器证书信息
* 返回值：   0 成功 -1 错误参数 -2 未设置服务器证书或无服务器证书 -3 设备证书私钥和公钥不匹配
* 备    注：
*/
DLLEXPORT int dl_get_ser_ca_info(dl_ca_info_t *info);

/*
* int dl_get_ca_info(const char *file, dl_ca_info_t *info)
* 功    能： 获取证书信息
* 入口参数： file: 证书文件
* 出口参数： info: 证书文件信息
* 返回值：   0 成功 -1 错误参数 -2 非证书文件
* 备    注：
*/
DLLEXPORT int dl_get_ca_info(const char *file, dl_ca_info_t *info);

/*
* int dl_get_ca_info_ex(const char *file, dl_ca_detail_t *info, int cert_flags, dl_response_t *res)
* 功    能： 获取证书信息
* 入口参数： file: 证书文件 cert_type: 证书标识位，默认为0即普通CA，客户端证书设置DL_CLIENT_CERT标识
* 出口参数： info: 证书文件信息 res: 错误信息，错误码对照_dl_cert_err_code
* 返回值：   0 成功 -1 失败
* 备    注：
*/
DLLEXPORT int dl_get_ca_info_ex(const char *file, dl_ca_detail_t *info, int cert_flags, dl_response_t *res);

/*
* int dl_get_trust_ca_info_ex(dl_ca_detail_table_t *table， dl_response_t *res)
* 功    能： 获取信任证书信息
* 入口参数：
* 出口参数： table: 信任证书文件信息表 res 错误信息,错误码对照dl_cert_err_code
* 返回值：   0 成功 -1 失败
* 备    注：
*/
DLLEXPORT int dl_get_trust_ca_info_ex(dl_ca_detail_table_t *table, dl_response_t *res);

/*
* int dl_cert_verify(const char *file, const char *ca_path, const char *ca_file, int cert_flags, dl_response_t *res)
* 功    能： 检验X509格式的证书是否有效可信
* 入口参数： file: 要校验的证书的路径
             ca_path: 指定信任证书RootCA所在路径，CA证书以hash值命名存放，可通过dl_get_ca_info_ex获取证书的hash值信息，然后将文件重命名为$hash和$hash.0
             ca_file: 指定信任证书RootCA文件
             cert_flags: 证书标识位，默认为0即普通CA，客户端证书设置DL_CLIENT_CERT标识
* 出口参数： res: 错误信息，错误码对照dl_cert_err_code
* 返回值：   0成功 -1 失败
* 备    注：@refer openssl/apps/verify.c
            证书非RootCA时，需要指定ca_path和ca_file来指定RootCA，使用证书链来验证证书是否有效可信；
            验证客户端证书时，还会验证私钥和公钥是否匹配等；
*/
DLLEXPORT int dl_cert_verify(const char *file, const char *ca_path, const char *ca_file, int cert_flags, dl_response_t *res);

/**
* int dl_generate_client_cert(const char *ca_file, const char *cakey_file, const char *clientkey_file, char *clientcert_file, const char *subject, int key_bit)
* 功能: 生成客户端证书
* 入口参数: ca_file RootCA cakey_file RootCA的私钥 clientkey_file客户端私钥 subject 客户端证书的subject key_bit私钥位数
* 出口参数: clientcert_file 生成的客户端证书的路径
* 返回值: 0 成功 -1 失败
* 备    注：@refer openssl/apps/ca.c req.c
            RootCA及其私钥必须存在，客户端私钥clientkey_file可以由接口生成，生成时key_bit指定私钥长度
*/
DLLEXPORT int dl_generate_client_cert(const char *ca_file, const char *cakey_file, const char *clientkey_file, char *clientcert_file, const char *subject, int key_bit);

/**
* int dl_cert_build_csr_req_file(char *csr_file, char *key_file, char *subject, int key_bit)
* 功能: 生成证书CSR文件
* 入口参数: ca_csr 证书CSR文件 clientkey_file客户端私钥 subject 客户端证书的subject key_bit私钥位数
* 出口参数: csr_file 生成的客户端证书CSR文件路径
* 返回值: 0 成功 -1 失败
* 备    注：
*/
DLLEXPORT int dl_cert_build_csr_req_file(char *csr_file, char *clientkey_file, char *subject, int key_bit);

/*
* int dl_permission_verify(const char *ext_key_usage)
* 功   能：检验当前是否有ext_key_usage相应的证书授权
* 入口参数：ext_key_usage: 要验证的权限，例如Lync是”1.3.6.1.6.4.147.7.4.11”，详见宏定义DL_PERMISSION_LYNC
* 出口参数：验证是否通过
* 返回值：  0验证通过，-1未通过无权限
* 备   注：
*/
DLLEXPORT int dl_permission_verify(const char *ext_key_usage);

/*
* int dl_permission_verify_with_path(const char *license_dat_path, const char *ext_key_usage)
* 功   能：检验当前是否有ext_key_usage相应的证书授权
* 入口参数：license_dat_path: license dat文件的全路径
* 入口参数: ext_key_usage: 要验证的权限，例如Lync是”1.3.6.1.6.4.147.7.4.11”，详见宏定义DL_PERMISSION_LYNC
* 出口参数：验证是否通过
* 返回值：  0验证通过，-1未通过无权限
* 备   注：
*/
DLLEXPORT int dl_permission_verify_with_path(const char *license_dat_path, const char *ext_key_usage);

/*
* int dl_get_all_permission(int type, char *ext_key_usage, int usage_len)
* 功   能：获取当前环境下的所有权限
* 入口参数：type: 获取类型，0出厂权限，1证书权限
* 入口参数：usage_len: 存放ext_key_usage权限字符串的大小
* 出口参数：ext_key_usage: 权限字符串，类似于”1.2.3.4,1.2.3.5”
* 返回值：  0获取成功，-1获取失败
* 备   注：该接口用于网页UI显示权限状态
*/
DLLEXPORT int dl_get_all_permission(dl_permission_type_e type, char *ext_key_usage, int usage_len);

/*
* int dl_get_all_permission(int type, char *ext_key_usage, int usage_len)
* 功   能：获取当前环境下的所有权限
* 入口参数：license_dat_path: license dat文件的全路径
* 入口参数：type: 获取类型，0出厂权限，1证书权限
* 入口参数：usage_len: 存放ext_key_usage权限字符串的大小
* 出口参数：ext_key_usage: 权限字符串，类似于”1.2.3.4,1.2.3.5”
* 返回值：  0获取成功，-1获取失败
* 备   注：该接口用于网页UI显示权限状态
*/
DLLEXPORT int dl_get_all_permission_with_path(const char *license_dat_path, dl_permission_type_e type, char *ext_key_usage, int usage_len);

/*
* int dl_get_license_expire(dl_permission_type_e type, int *expire_ret)
* 功   能： 获取当前环境下功能许可的有效期限
* 入口参数：type: 获取类型，0出厂权限，1证书权限
* 出口参数：expire_ret: 有效时间字段，单位为分钟，-1表示永久权限，0表示期限0，即无权限了，>0值为证书有效时长
* 返回值：  0获取成功，-1获取失败
* 备   注：该接口用于网页UI获取证书的有效时长，UI网页通过内部计时更新使用的时间，并与有效时长比较，如果超过了有效时长，
*         则权限到期，部分功能要禁用掉
*/
DLLEXPORT int dl_get_license_expire(dl_permission_type_e type, int *expire_ret);

/*
* int dl_get_license_expire_with_path(const char *license_dat_path, dl_permission_type_e type, int *expire_ret)
* 功   能： 获取当前环境下功能许可的有效期限
* 入口参数：license_dat_path: license dat文件的全路径
* 入口参数：type: 获取类型，0出厂权限，1证书权限
* 出口参数：expire_ret: 有效时间字段，单位为分钟，-1表示永久权限，0表示期限0，即无权限了，>0值为证书有效时长
* 返回值：  0获取成功，-1获取失败
* 备   注：该接口用于网页UI获取证书的有效时长，UI网页通过内部计时更新使用的时间，并与有效时长比较，如果超过了有效时长，
*         则权限到期，部分功能要禁用掉
*/
DLLEXPORT int dl_get_license_expire_with_path(const char *license_dat_path, dl_permission_type_e type, int *expire_ret);

/*
* int dl_get_license_expire_with_env(dl_permission_type_e type, int *expire_ret)
* 功   能： 获取当前环境下功能许可的有效期限,先获取环境变量中赋予的时长，如果为0，则读取证书的时长，
            用于替换dl_get_license_expire
* 入口参数：ext_key_usage:所需要确认时长的权限，该参数仅用在读取factory时长的时候使用，读取license直接读取时间字段
* 出口参数：expire_ret: 有效时间字段，单位为分钟，-1表示永久权限，0表示期限0，即无权限了，>0值为证书有效时长
* 返回值：  0获取成功，-1获取失败
* 备   注：该接口用于网页UI获取证书的有效时长，UI网页通过内部计时更新使用的时间，并与有效时长比较，如果超过了有效时长，
*         则权限到期，部分功能要禁用掉
*/
DLLEXPORT int dl_get_license_expire_with_env(const char *ext_key_usage, int *expire_ret);

/*
* int dl_get_license_expire_with_path_env(const char *license_dat_path, const char *ext_key_usage, int *expire_ret)
* 功   能： 获取当前环境下功能许可的有效期限,先获取环境变量中赋予的时长，如果为0，则读取证书的时长，
            用于替换dl_get_license_expire_with_path
* 入口参数：license_dat_path: license dat文件的全路径
* 入口参数：ext_key_usage:所需要确认时长的权限，该参数仅用在读取factory时长的时候使用，读取license直接读取时间字段
* 出口参数：expire_ret: 有效时间字段，单位为分钟，-1表示永久权限，0表示期限0，即无权限了，>0值为证书有效时长
* 返回值：  0获取成功，-1获取失败
* 备   注：该接口用于网页UI获取证书的有效时长，UI网页通过内部计时更新使用的时间，并与有效时长比较，如果超过了有效时长，
*         则权限到期，部分功能要禁用掉
*/

DLLEXPORT int dl_get_license_expire_with_path_env(const char *license_dat_path, const char *ext_key_usage, int *expire_ret);

/*
* int dl_get_permission_name(const char *ext_key_usage, char *usage_name, int usage_len)
* 功   能：获取权限描述对应的权限名称
* 入口参数：ext_key_usage: 权限描述，形如”1.2.3.4”
* 入口参数：usage_len: 存放usage_name权限名称的大小
* 出口参数：usage_name: 权限名称，形如”Lync”
* 返回值：  0获取成功，-1获取失败
* 备   注：该接口用于网页UI显示权限状态
*/
DLLEXPORT int dl_get_permission_name(const char *ext_key_usage, char *usage_name, int usage_len);

/*
* int dl_get_license_addition_info(const char *ext_key_usage, char *usage_name, int usage_len)
* 功   能：获取证书的附加在netscap common中的信息
* 入口参数：license_dat_path: 证书全路径
* 入口参数：key: 附加信息使用key=value形式记录
* 入口参数: env: 是否带环境变量获取(有些信息直接可以从环境中直接获取)
* 出口参数：buf: 存放值的缓冲区
* 出口参数: buflen: 缓冲区长度
* 返回值：  0获取成功，-1获取失败
* 备   注：此接口并不验证证书是哪种类型的授权证书，应用需要自己先去验证这个
*/
DLLEXPORT int dl_get_license_addition_info(const char *license_dat_path, const char *key, char *buf, int buflen, int env);

/*
* int dl_get_encrypt_ca_info_ex(const char *file, dl_ca_detail_t *info, dl_response_t *res);
* 功    能： 获取加密证书信息
* 入口参数： file: 加密证书文件
* 出口参数： info: 证书文件信息 res: 错误信息
* 返回值：   0 成功 -1 失败
* 备    注：该接口用于网页UI显示证书状态和验证上传的dat文件是否有效。(此接口已废弃，新版本请不要使用)
*/
DLLEXPORT int dl_get_encrypt_ca_info_ex(const char *file, dl_ca_detail_t *info, dl_response_t *res);

/*
* int dl_get_encrypt_ca_info_ex_path(const char *file, dl_ca_detail_t *info, dl_response_t *res);
* 功    能： 获取加密证书信息
* 入口参数： file: 加密证书文件
* 出口参数： info: 证书文件信息 res: 错误信息
* 返回值：   0 成功 -1 失败
* 备    注：该接口用于网页UI显示证书状态和验证上传的dat文件是否有效;
*           此版本的接口不会与已存在的license.dat进行比较，如果要进行比较，请直接使用dl_ca_cmp
*/

DLLEXPORT int dl_get_encrypt_ca_info_ex_path(const char *file, dl_ca_detail_t *info, dl_response_t *res);

/*
* unsigned long dl_get_ca_hash();
* 功    能： 获取证书中的hash值
* 入口参数： 证书路径
* 出口参数： 无
* 返回值：   hash值
* 备    注：
*/
DLLEXPORT unsigned long dl_get_ca_hash(const char *file);


/************************************************************************/
/*                        兼容以前的接口,不建议再使用                        */
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
}dl_http_ex_type;               /* 用于http拓展 */

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
    unsigned char cfg_idx;      /* 配置索引 若section 为 Alertinfo0 则 cfg_idx = 0*/
    unsigned char ring_idx;     /* 铃声索引 若Ringer = 1 则 ring_idx = 1 */
    char text[126];             /* 若Text = test 则 text = "test" */
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
    DL_LOG_MODE_SYS  =    0x0001, /* 置此位 则LOG将输出到SYSLOG */
    DL_LOG_MODE_CONS =    0x0002, /* 置此位 则LOG将输出到终端 */
    DL_LOG_MODE_TIME =    0x0004, /* 置此位 LOG将包含时间 */
};

typedef int (*dl_log_redirect_cb)(const char *msg, int len);

/*
* int dl_open_old(dl_conn_info *conf, dl_handler *hander, char *new_url, dl_response_t res)
* 功    能： libdownload Open统一操作接口
* 入口参数： conf,主要包含用户的一些配置
* 出口参数： handler,用于用户调用dl_close dl_read
            new_url,用于保存http重定向后的url
            res,用于保存错误码和错误信息
* 返回值：   0 成功 -1 失败
*/
DLLEXPORT int dl_open_old(dl_conn_info *conf, dl_handler *handler, char *new_url, dl_response_t *res);

/*
* int dl_download_old(dl_conn_info *conf, char *new_url, dl_response_t *res)
* 功    能： libdownload Download统一操作接口
* 入口参数： conf,文件下载配置
            local_path, 本地保存路径
* 出口参数： new_url,重定向后新的url
            res,错误码和错误信息
* 返回值：   成功,0;失败,-1;
*/
DLLEXPORT int dl_download_old(dl_conn_info *conf, char *local_path, char *new_url, dl_response_t *res);


/*
* int dl_upload_old(dl_conn_info *conf, char *local_path, char *remote_name,
*        char *new_url, dl_response_t *res, char *http_header, int ftp_postmode, int ftp_limitmode);
* 功    能： libdownload Upload统一操作接口
* 入口参数： conf,文件上传配置
            local_path,本地需要上传的文件
            remote_name,tftp/ftp上传后的文件名，如果为空，则默认和本地文件名一致
            http_header,上传http文件需要附带的额外头部
            param_ex1, 上传方式：如果是ftp上传 1 APPEND 2 STOR(覆盖)
                                如果是http上传 1.POST 2 PUT
            param_ex2, ftp上传超过文件限制后的选择:1 DELETE(覆盖) 2 STOP(退出)
* 出口参数： new_url,重定向后新的url
            res,错误码和错误信息
* 返回值：   成功,0;失败,-1;
*/
DLLEXPORT int dl_upload_old(dl_conn_info *conf, char *local_path, char *remote_name,
        char *new_url, dl_response_t *res, char *http_header, int param_ex1, int param_ex2);

/*
* int dl_log_init_old(int log_mode, dl_log_redirect_cb redir_cb);
* 功    能： 初始化log
* 入口参数：log_mode : log模式(已不支持DL_LOG_MODE_SYS，DL_LOG_MODE_CONS同时设置，优先输出到SYS)
*           redir_cb : 回调函数指针 msg为log字符串, len为字符串长度(已不支持设置)
* 出口参数：
* 返回值：
* 备    注： 本接口建议不使用，请使用dl_log_new_init。
*
*/
DLLEXPORT int dl_log_init_old(int log_mode, dl_log_redirect_cb redir_cb);

DLLEXPORT int dl_log_new_init(dl_log_type_e type, dl_log_new_mode_e mode, const char *file);

/*
* int dl_http_open(dl_http_handler *handler, dl_url_info *url_info, char *new_url, dl_response_t *ret_response)
* 功    能： 打开HTTP连接
* 入口参数： url_info:连接的URL
* 出口参数： handler:http句柄指针 ret_response:错误和响应信息 new_url:重定向地址
* 返回值：   0 成功 -1 失败
* 备    注：
*/
DLLEXPORT int dl_http_open(dl_http_handler *handler, dl_url_info *url_info, char *new_url, dl_response_t *ret_response);

/*
* int dl_http_open_ex(dl_http_handler *handler, dl_url_info_ex *url_info_ex, char *new_url, dl_response_t *ret_response)
* 功    能： 打开HTTP连接
* 入口参数： url_info_ex:带扩展信息的URL
* 出口参数： handler:http句柄指针 ret_response:错误和响应信息 new_url:重定向地址
* 返回值：   0 成功 -1 失败
* 备    注：
*/
DLLEXPORT int dl_http_open_ex(dl_http_handler *handler, dl_url_info_ex *url_info_ex, char *new_url, dl_response_t *ret_response);

/*
* int dl_http_read(dl_http_handler handler, char *buffer, int len, dl_response_t *res)
* 功    能： 读取HTTP content数据
* 入口参数： buffer:接收数据缓存 len:buffer长度
* 出口参数： res:返回错误码和信息
* 返回值：   读到的字节数
* 备    注：
*/
DLLEXPORT int dl_http_read(dl_http_handler handler, char *buffer, int len, dl_response_t *res);


/*
* void dl_http_close(dl_http_handler handler)
* 功    能： 关闭http连接
* 入口参数： handler: http句柄
* 出口参数：
* 返回值：
* 备    注：
*/
DLLEXPORT void dl_http_close(dl_http_handler handler);


/*
* int dl_http_recv_to_file(dl_url_info *url_info, char *filename, char *new_url, dl_response_t *res)
* 功    能： 直接下载文件
* 入口参数： url_info:URL信息 filename:下载文件名 为空 使用URL的文件名
* 出口参数： new_url:重定向地址, res:错误码和信息
* 返回值：   0 成功 -1 失败
* 备    注：
*/
DLLEXPORT int dl_http_recv_to_file(dl_url_info *url_info, char *filename, char *new_url, dl_response_t *res);

/*
* int dl_http_recv_to_file_ex(dl_url_info_ex *url_info_ex, char *filename, char *new_url, dl_response_t *res)
* 功    能：直接下载文件
* 入口参数： url_info_ex:带扩展的URL信息 filename:下载文件名 为空 使用URL的文件名
* 出口参数： new_url:重定向地址, res:错误码和信息
* 返回值：   0 成功 -1 失败
* 备    注：
*/
DLLEXPORT int dl_http_recv_to_file_ex(dl_url_info_ex *url_info_ex, char *filename, char *new_url, dl_response_t *res);

/*
* int dl_http_post_file(dl_http_handler *handler, dl_url_info *url_info, char *filename, char *new_url, dl_response_t *ret_response)
* 功    能： 使用post方法上传文件
* 入口参数： url_info:连接的URL  filename:要上传的文件路径
* 出口参数： handler:http句柄指针 ret_response:错误和响应信息 new_url:重定向地址
* 返回值：   0 成功 -1 失败
* 备    注：
*/
DLLEXPORT int dl_http_post_file(dl_http_handler *handler, dl_url_info *url_info, char *filename, char *new_url, dl_response_t *ret_response);

/*
* int dl_distinctive_ringtone_download(const char *url, const char *info, int *index, char *filepath, int pathlen)
* 功    能： 下载区别振铃
* 入口参数： url:下载的URL info:铃声信息 filepath:文件路径缓存, pathlen:缓存长度
* 出口参数： index:铃声索引
* 返回值：   0使用默认 1 找到区别振铃
* 备    注：优先查找URL 后判断info
*/
DLLEXPORT int dl_distinctive_ringtone_download(const char *url, const char *info, int *index, char *filepath, int pathlen);


/*
* int dl_tftp_open(dl_tftp_handler *handler, const char *host, int port, const char *file_path, dl_response_t *res)
* 功    能： 初始化TFTP
* 入口参数： host:tftp域名或者IP port:端口号 为0 则使用默认 remote_name:要下载文件名
* 出口参数： handler:tftp处理句柄 设置成申请到的指针 res:错误和响应信息
* 返回值：   0 成功 -1 失败
* 备    注：
*/
DLLEXPORT int dl_tftp_open(dl_tftp_handler *handler, const char *host, int port, const char *file_path, dl_response_t *res);


/*
* int dl_tftp_read(dl_tftp_handler handler,char * buffer,int len,dl_response_t * res)
* 功    能： 读取下载文件数据
* 入口参数： handler:TFTP句柄 buffer:接收数据缓存 len:buffer长度
* 出口参数： res:返回错误码和信息
* 返回值：   读到的字节数
* 备    注：
*/
DLLEXPORT int dl_tftp_read(dl_tftp_handler handler, char *buffer, int len, dl_response_t *res);


/*
* void dl_tftp_close(dl_tftp_handler handler)
* 功    能： 关闭tftp
* 入口参数： handler: tftp句柄
* 出口参数：
* 返回值：
* 备    注：
*/
DLLEXPORT void dl_tftp_close(dl_tftp_handler handler);


/*
* int dl_tftp_recv_to_file(char *host, int port, const char *file_path, const char *local_name, dl_response_t *res)
* 功    能： 直接下载文件
* 入口参数： host:域名或者ip port:端口号 为0使用默认 local_name:要保存的文件名
* 出口参数： res:错误码和信息
* 返回值：   0 成功 -1 失败
* 备    注：
*/
DLLEXPORT int dl_tftp_recv_to_file(const char *host, int port, const char *file_path, const char *local_name, dl_response_t *res);


/*
* int dl_tftp_send_file(char *host, int port, const char *remote_name, const char *local_name, dl_response_t *res)
* 功    能： 直接上传文件
* 入口参数： host:域名或者ip port:端口号 为0使用默认 local_name:要上传的文件名 remote_name:服务器上保存的文件名
* 出口参数： res:错误码和信息
* 返回值：   0 成功 -1 失败
* 备    注： file_path:服务器上保存文件名 为空时则使用原文件名
*/
DLLEXPORT int dl_tftp_send_file(const char *host, int port, const char *file_path, const char *local_name, dl_response_t *res);

/*
* int dl_ftp_open(dl_ftp_handler *handler, const char *host, int port, const char *user, const char *passwd, const char *file_path, dl_response_t * res)
* 功    能： 建立FTP连接
* 入口参数： host:ftp域名或者IP port:端口号 为0 则使用默认 user:用户名 passwd:密码 file_path:要下载文件路径
* 出口参数： handler:ftp处理句柄 设置成申请到的指针 res:错误和响应信息
* 返回值：   0 成功 -1 失败
* 备    注：
*/
DLLEXPORT int dl_ftp_open(dl_ftp_handler *handler, const char *host, int port, const char *user, const char *passwd, const char *file_path, dl_response_t * res);


/*
* int dl_ftp_read(dl_ftp_handler handler, char *buf, int len, dl_response_t *res)
* 功    能： 读取下载文件数据
* 入口参数： handler: ftp句柄 buffer:接收数据缓存 len:buffer长度
* 出口参数： res:返回错误码和信息
* 返回值：   读到的字节数
* 备    注：
*/
DLLEXPORT int dl_ftp_read(dl_ftp_handler handler, char *buf, int len, dl_response_t *res);


/*
* void dl_ftp_close(dl_ftp_handler handler)
* 功    能： 关闭ftp连接
* 入口参数： handler: ftp句柄
* 出口参数：
* 返回值：
* 备    注：
*/
DLLEXPORT void dl_ftp_close(dl_ftp_handler handler);


/*
* int dl_ftp_recv_to_file(const char * host,int port,const char * user, const char *passwd, const char *file_path, const char *local_name, dl_response_t *res)
* 功    能： 直接下载文件
* 入口参数： host:域名或者ip port:端口号 为0使用默认 user:用户名 passwd:密码 file_path:下载文件路径 local_name:保存文件名 为空时则使用原文件名 并保存到当前路径
* 出口参数： res:错误码和信息
* 返回值：   0 成功 -1失败
* 备    注：
*/
DLLEXPORT int dl_ftp_recv_to_file(const char *host, int port, const char *user, const char *passwd, const char *file_path, const char *local_name, dl_response_t *res);


/*
* int dl_ftp_send_file(const char * host,int port,const char * user, const char *passwd, const char *file_path, const char *local_name, dl_response_t *res)
* 功    能： 使用ftp上传文件
* 入口参数： host:域名或者ip port:端口号 为0使用默认 user:用户名 passwd:密码 remote_name:服务器上保存的文件名，为空时则使用本地文件名
             local_name:要上传的文件 postmode:ftp上传方式的选择1 APPEND 2 STOR(覆盖) limitmode:超过文件限制后的选择:1 DELETE(覆盖) 2 STOP(退出)
* 出口参数： res:错误码和信息
* 返回值：   0 成功 -1失败
* 备    注： file_path:服务器上保存文件名 为空时则使用原文件名
*/
DLLEXPORT int dl_ftp_send_file(const char *host, int port, const char *user, const char *passwd,
                        const char *file_path, const char *local_name, int postmode,
                        int limitmode, dl_response_t *res);

#ifndef WIN32
/*
* int dl_dns_resolv(const char *hostname, int request_type, int dl_class, struct reply *res);
* 功    能： 执行DNS查询
* 入口参数： hostname:域名
             request_type:请求类型 (DL_TYPE_AAAA/DL_TYPE_A/DL_TYPE_PTR/DL_TYPE_CNAME)
             dl_class:协议族类型，当前只支持DL_CLASS_INET
             nretry: 重试次数
             timeout:超时时间
* 出口参数： res:  查询结果
* 返回值：   成功,0;失败,-1;
*/

DLLEXPORT int dl_dns_resolv_timeout(const char *hostname, int request_type, int dl_class, struct reply *res, int nretry, int timeout);


/*
* int dl_dns_resolv(const char *hostname, int request_type, int dl_class, struct reply *res);
* 功    能： 执行DNS查询，内部超时和尝试次数由options设置决定
* 入口参数： hostname:域名
             request_type:请求类型 (DL_TYPE_AAAA/DL_TYPE_A/DL_TYPE_PTR/DL_TYPE_CNAME)
             dl_class:协议族类型，当前只支持CLASS_INET
* 出口参数： res:  查询结果
* 返回值：   成功,0;失败,-1;
*/
DLLEXPORT int dl_dns_resolv(const char *hostname, int request_type, int dl_class, struct reply *res);

/*
* int dl_dns_set_resolv_seq(int *resolv_seq_arr, int count);
* 功    能： 设置dns查询优先级
* 入口参数： resolv_seq_arr: 优先级定义数组，当前的支持RESOLV_METHOD_STATIC和RESOLV_METHOD_DYNAMIC方法
             count: 数组大小
* 出口参数：
* 返回值：   成功,0;失败,-1;
*/
DLLEXPORT int dl_dns_set_resolv_seq(int *resolv_seq_arr, int count);

/*
* int dl_dns_set_nretry(int nretry);
* 功    能： 设置dns查询重试次数(对于每一个nameserver)，如果不设置，默认为2次
* 入口参数： nretry 重试次数
* 出口参数：
* 返回值：   成功,0;失败,-1;
*/
DLLEXPORT int dl_dns_set_nretry(int nretry);

/*
* int dl_dns_set_nretry(int nretry);
* 功    能： 设置dns查询超时时间(对于每一个nameserver)，如果不设置，默认为5秒
* 入口参数： timeout 超时时间，单位秒
* 出口参数：
* 返回值：   成功,0;失败,-1;
*/
DLLEXPORT int dl_dns_set_timeout(int timeout);

/*
* int dl_dns_reload_resolv_conf();
* 功    能： 重新load dns配置文件--当DNS配置文件有更新的时候需要重新调用此函数，通过DL_OPTION_RESOLV_FILE
             选项设置，如果调用下载库的接口去连接域名形式的服务器则可不必调用此将接口，否则的话，就需要自行调用
* 入口参数：
* 出口参数：
* 返回值：   成功,0;失败,-1;
*/
DLLEXPORT int dl_dns_reload_resolv_conf();

/*
* int dl_dns_resolv_conf_destroy();
* 功    能： 清除dns配置
* 入口参数：
* 出口参数：
* 返回值：   成功,0;失败,-1;
*/
DLLEXPORT int dl_dns_resolv_conf_destroy();

/*
* int dl_dns_hosts_conf_destroy();
* 功    能： 清除dns hosts配置
* 入口参数：
* 出口参数：
* 返回值：   成功,0;失败,-1;
*/
DLLEXPORT int dl_dns_hosts_conf_destroy();

/*
* int dl_dns_reload_hosts_conf();
* 功    能： 重新load dns hosts配置文件--当DNS配置文件有更新的时候需要重新调用此函数，通过DL_OPTION_HOSTS_FILE
             选项设置，如果调用下载库的接口去连接域名形式的服务器则可不必调用此将接口，否则的话，就需要自行调用
* 入口参数：
* 出口参数：
* 返回值：   成功,0;失败,-1;
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
    ERRHOST = -1,               /* 错误的HOST */
    ERRSOCK = -2,               /* 无法创建套接字 */
    ERRCONN = -3,               /* 无法连接 */
    ERRWRDT = -4,               /* 发送数据出错 */
    ERRPAHD = -5,               /* 接收到无效的数据 */
    ERRPARAMETER = -6,          /* 错误参数 */
    ERRMEM = -7,                /* 内存空间不够 */
    ERRRDDT = -8,               /* 读数据出错 */
    ERRURLH = -9,               /* 错误的URL */
    ERRURLP = -10,              /* 错误的端口 */
    ERRAUTH = -11,              /* 认证出错 */
    ERRREDIRECT = -12,          /* 重定向出错 */
    ERRDISIPV6 = -13,           /* ipv6 被禁止 */
    ERRUNINIT = -14,            /* 未初始化 */
    ERRFTP = -15,               /* FTP出错 */
    ERRTFTP = -16,              /* TFTP出错 */
    ERR301PARSE = -17,          /* HTTP重定向解析出错 需更换协议 */
    ERRFTP_NOUSER = -18,        /* FTP 用户名不存在 */
    ERRFTP_WRONG_PASSWD = -19,  /* FTP密码错误 */
    ERR400 = 400,               /* 无效查询 */
    ERR403 = 403,               /* 被禁止 */
    ERR408 = 408,               /* 请求超时 */
    ERR500 = 500,               /* 服务器出错 */
    ERR501 = 501,               /* 服务器未实现方法 */
    ERR503 = 503,               /* 服务器超负荷 */
    OK_201 = 201,               /* 资源成功被创建 */
    OK_200 = 200                /* 成功读取数据 */
};

#define PERMISSION_VERSION_AWARD "1.3.6.1.6.4.147.7.4.15"
#define UPGRADE_AWARD_KEY        "upgrade_award"


#ifdef __cplusplus
}
#endif

#endif
