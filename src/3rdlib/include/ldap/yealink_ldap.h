/********************************************************************
*
*	 DESCRIPTION:Copyright(c) 2010-2020 Xiamen Yealink Network Technology Co,.Ltd
*
*	 AUTHOR:hyy
*
*	 DATE:2012-06-14
*
*	 HISTORY:
*-----------------------------------------------------------------
*    修改时间: 2013.3.9   修改者 :  hyy
*    原版本号: 3.0.0.3    新版本号: 3.0.1.0
*    修改问题:
*    根据syslog规范.pdf文档修改log格式,增加yealink_ldap_log_init接口
*------------------------------------------------------------------
*
*******************************************************************/

#ifndef _YEALINK_LDAP_H_
#define _YEALINK_LDAP_H_

#include <stdlib.h>

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef SUCC
#define SUCC		1
#endif

#ifndef FAIL
#define FAIL		0
#endif

/* 屏幕上显示的联系人名字的最大长度 */
#define YEALINK_DISPLAY_NAME_MAXSZ	100
/* 屏幕上显示的号码的最大长度 */
#define YEALINK_DISPLAY_NUMB_MAXSZ	30
/* 下载根证书名的最大长度 */
#define YEALINK_CERT_NAME_MAXSZ	100
/* 根证书的最大长度 */
#define YEALINK_CERT_NUMB_MAXSZ	8192

#define YEALINK_DISPLAY_ATTR_MAXZ    128

/*
 * LDAP Result Codes
 */
#define YEALINK_LDAP_SUCCESS                0x00

#define YEALINK_LDAP_RANGE(n,x,y)   (((x) <= (n)) && ((n) <= (y)))

#define YEALINK_LDAP_PROTOCOL_ERROR         0x02
#define YEALINK_LDAP_TIMELIMIT_EXCEEDED     0x03
#define YEALINK_LDAP_SIZELIMIT_EXCEEDED     0x04
#define YEALINK_LDAP_COMPARE_FALSE          0x05
#define YEALINK_LDAP_COMPARE_TRUE           0x06
#define YEALINK_LDAP_AUTH_METHOD_NOT_SUPPORTED  0x07
#define YEALINK_LDAP_STRONG_AUTH_NOT_SUPPORTED  LDAP_AUTH_METHOD_NOT_SUPPORTED
#define YEALINK_LDAP_STRONG_AUTH_REQUIRED   0x08
#define YEALINK_LDAP_STRONGER_AUTH_REQUIRED LDAP_STRONG_AUTH_REQUIRED
#define YEALINK_LDAP_PARTIAL_RESULTS        0x09	/* LDAPv2+ (not LDAPv3) */

#define	YEALINK_LDAP_REFERRAL               0x0a /* LDAPv3 */
#define YEALINK_LDAP_ADMINLIMIT_EXCEEDED    0x0b /* LDAPv3 */
#define	YEALINK_LDAP_UNAVAILABLE_CRITICAL_EXTENSION 0x0c /* LDAPv3 */
#define YEALINK_LDAP_CONFIDENTIALITY_REQUIRED   0x0d /* LDAPv3 */
#define	YEALINK_LDAP_SASL_BIND_IN_PROGRESS  0x0e /* LDAPv3 */

#define YEALINK_LDAP_ATTR_ERROR(n)  YEALINK_LDAP_RANGE((n),0x10,0x15) /* 16-21 */

#define YEALINK_LDAP_NO_SUCH_ATTRIBUTE      0x10
#define YEALINK_LDAP_UNDEFINED_TYPE         0x11
#define YEALINK_LDAP_INAPPROPRIATE_MATCHING 0x12
#define YEALINK_LDAP_CONSTRAINT_VIOLATION   0x13
#define YEALINK_LDAP_TYPE_OR_VALUE_EXISTS   0x14
#define YEALINK_LDAP_INVALID_SYNTAX         0x15

#define YEALINK_LDAP_NAME_ERROR(n)  YEALINK_LDAP_RANGE((n),0x20,0x24) /* 32-34,36 */

#define YEALINK_LDAP_NO_SUCH_OBJECT         0x20
#define YEALINK_LDAP_ALIAS_PROBLEM          0x21
#define YEALINK_LDAP_INVALID_DN_SYNTAX      0x22
#define YEALINK_LDAP_IS_LEAF                0x23 /* not LDAPv3 */
#define YEALINK_LDAP_ALIAS_DEREF_PROBLEM    0x24

#define YEALINK_LDAP_SECURITY_ERROR(n)  YEALINK_LDAP_RANGE((n),0x2F,0x32) /* 47-50 */

#define YEALINK_LDAP_X_PROXY_AUTHZ_FAILURE  0x2F /* LDAPv3 proxy authorization */
#define YEALINK_LDAP_INAPPROPRIATE_AUTH     0x30
#define YEALINK_LDAP_INVALID_CREDENTIALS    0x31
#define YEALINK_LDAP_INSUFFICIENT_ACCESS    0x32

#define YEALINK_LDAP_SERVICE_ERROR(n)   YEALINK_LDAP_RANGE((n),0x33,0x36) /* 51-54 */

#define YEALINK_LDAP_BUSY                   0x33
#define YEALINK_LDAP_UNAVAILABLE            0x34
#define YEALINK_LDAP_UNWILLING_TO_PERFORM   0x35
#define YEALINK_LDAP_LOOP_DETECT            0x36

#define YEALINK_LDAP_UPDATE_ERROR(n)    YEALINK_LDAP_RANGE((n),0x40,0x47) /* 64-69,71 */

#define YEALINK_LDAP_NAMING_VIOLATION       0x40
#define YEALINK_LDAP_OBJECT_CLASS_VIOLATION 0x41
#define YEALINK_LDAP_NOT_ALLOWED_ON_NONLEAF 0x42
#define YEALINK_LDAP_NOT_ALLOWED_ON_RDN     0x43
#define YEALINK_LDAP_ALREADY_EXISTS         0x44
#define YEALINK_LDAP_NO_OBJECT_CLASS_MODS   0x45
#define YEALINK_LDAP_RESULTS_TOO_LARGE      0x46 /* CLDAP */
#define YEALINK_LDAP_AFFECTS_MULTIPLE_DSAS  0x47

#define YEALINK_LDAP_OTHER                  0x50

/* LCUP operation codes (113-117) - not implemented */
#define YEALINK_LDAP_CUP_RESOURCES_EXHAUSTED    0x71
#define YEALINK_LDAP_CUP_SECURITY_VIOLATION     0x72
#define YEALINK_LDAP_CUP_INVALID_DATA           0x73
#define YEALINK_LDAP_CUP_UNSUPPORTED_SCHEME     0x74
#define YEALINK_LDAP_CUP_RELOAD_REQUIRED        0x75

#define YEALINK_LDAP_SERVER_DOWN                (-1)
#define YEALINK_LDAP_LOCAL_ERROR                (-2)
#define YEALINK_LDAP_ENCODING_ERROR             (-3)
#define YEALINK_LDAP_DECODING_ERROR             (-4)
#define YEALINK_LDAP_TIMEOUT                    (-5)
#define YEALINK_LDAP_AUTH_UNKNOWN               (-6)
#define YEALINK_LDAP_FILTER_ERROR               (-7)
#define YEALINK_LDAP_USER_CANCELLED             (-8)
#define YEALINK_LDAP_PARAM_ERROR                (-9)
#define YEALINK_LDAP_NO_MEMORY                  (-10)
#define YEALINK_LDAP_CONNECT_ERROR              (-11)
#define YEALINK_LDAP_NOT_SUPPORTED              (-12)
#define YEALINK_LDAP_CONTROL_NOT_FOUND          (-13)
#define YEALINK_LDAP_NO_RESULTS_RETURNED        (-14)
#define YEALINK_LDAP_MORE_RESULTS_TO_RETURN     (-15)    /* Obsolete */
#define YEALINK_LDAP_CLIENT_LOOP                (-16)
#define YEALINK_LDAP_REFERRAL_LIMIT_EXCEEDED    (-17)
#define	YEALINK_LDAP_X_CONNECTING               (-18)
#define YEALINK_LDAP_OPERATIONS_ERROR           (-20)

enum yealink_ldap_option
{
    YEALINK_LDAP_OPTIONS_PORT = 0,		/* 端口设置 */
    YEALINK_LDAP_OPTIONS_VERSION,		/* 版本设置 */
    YEALINK_LDAP_OPTIONS_MAXHITS,		/* 最大搜索个数设置 */
    YEALINK_LDAP_OPTIONS_MD5_ENABLE,	/* MD5使能设置 */
    YEALINK_LDAP_OPTIONS_HOST,			/* 服务器地址设置 */
    YEALINK_LDAP_OPTIONS_DN,			/* DN值设置 */
    YEALINK_LDAP_OPTIONS_USER,			/* 用户名设置 */
    YEALINK_LDAP_OPTIONS_PASSWORD,		/* 密码设置 */
    YEALINK_LDAP_OPTIONS_BASE,			/* 根目录设置 */
    YEALINK_LDAP_OPTIONS_NAME_FILTER,	/* 名字过滤条件设置 */
    YEALINK_LDAP_OPTIONS_NUMBER_FILTER,	/* 号码过滤条件设置 */
    YEALINK_LDAP_OPTIONS_NAME_ATTR,		/* 名字属性设置 */
    YEALINK_LDAP_OPTIONS_NUMBER_ATTR,	/* 号码属性设置 */
    YEALINK_LDAP_OPTIONS_DISPALY_NAME,	/* 显示名字设置 */
    YEALINK_LDAP_OPTIONS_REPLACE_EMPTY, /* 属性为空的显示内容设置 */
    YEALINK_LDAP_OPTIONS_TLS_MODE,      /* LDAP 使用模式 */
    YEALINK_LDAP_OPTIONS_TLS_CA_DIR,    /* ca 存方目录 */
    YEALINK_LDAP_OPTIONS_TLS_CERT_FILE, /* 证书路径 */
    YEALINK_LDAP_OPTIONS_NAME_SEPARATE_SYMBOL,/*名字部门公司等属性的分隔符设置*/
    YEALINK_LDAP_OPTIONS_IPADDR_MODE,      /* IP模式 0 IPv4,1 IPv6,2 IPv4&IPv6 */
	YEALINK_LDAP_OPTIONS_NTLM_ENABLE,	/* NTLM使能设置 */
    YEALINK_LDAP_OPTIONS_NETWORK_TIMEOUT,      /* TCP连接超时 */
    YEALINK_LDAP_OPTIONS_EXTRA_ATTR,      /* 名字和号码以外的属性 */
    YEALINK_LDAP_OPTIONS_DEFAULT_SEARCH_FILTER, /*默认搜索过滤条件*/
    YEALINK_LDAP_OPTIONS_SEARCH_TYPE,   /*搜索类型 0-开头匹配，1-模糊搜索*/
    YEALINK_LDAP_OPTIONS_NET_PRIORITY,  /*ipv4 & ipv6 模式下的网络优先级设置，0表示IPv6优先，1表示IPv4优先*/
    YEALINK_LDAP_OPTIONS_EXTRA_DISPLAY_ATTR, /*额外的显示属性*/
    YEALINK_LDAP_OPTIONS_TLS_CIPHER_LIST, /*SSL 模式加密算法套件 有效值和格式为openssl加密套件的设置格式*/
	YEALINK_LDAP_OPTIONS_END			/* 无作用，仅用于结束标志 */
};

typedef enum _yealink_ldap_log_mode
{
    YEALINK_LDAP_LOG_NONE =    0x0000,
    YEALINK_LDAP_LOG_SYS  =    0x0001, /* 置此位 则LOG将输出到SYSLOG */
    YEALINK_LDAP_LOG_CONS =    0x0002, /* 置此位 则LOG将输出到终端 */
    YEALINK_LDAP_LOG_TIME =    0x0004, /* 置此位 LOG将包含时间 */
} yealink_ldap_log_mode;

typedef enum _yealink_ldap_log_level
{
    YEALINK_LDAP_LOG_EMERG,     /* system is unusable */
    YEALINK_LDAP_LOG_ALERT,     /* action must be taken immediately */
    YEALINK_LDAP_LOG_CRIT,      /* critical conditions */
    YEALINK_LDAP_LOG_ERR,	    /**3*/
    YEALINK_LDAP_LOG_WARN,	    /**4*/
    YEALINK_LDAP_LOG_NOTICE,    /**5*/
    YEALINK_LDAP_LOG_INFO,	    /**6*/
    YEALINK_LDAP_LOG_DEBUG,     /**7*/
} yealink_ldap_log_level;

typedef enum {
    YEALINK_LDAP_NORMAL,
    YEALINK_LDAP_STARTTLS,
    YEALINK_LDAP_LDAPS,
} ldap_TLS_model;

typedef enum {
    YEALINK_LDAP_ERR_UNDEFINED = 0,         /* same as FAIL */
    YEALINK_LDAP_ERR_AUTH = -1,             /* authentication err */
} yealink_ldap_err;

typedef struct List
{
    char			name[YEALINK_DISPLAY_NAME_MAXSZ];
    char			numb[YEALINK_DISPLAY_NUMB_MAXSZ];
    struct List		*Next;
}yealink_ldap_node, *yealink_ldap_link;
typedef struct List_Cert
{
    char			name[YEALINK_CERT_NAME_MAXSZ];
    char			numb[YEALINK_CERT_NUMB_MAXSZ];
    int                 numblen;
    struct List_Cert	*Next;
}yealink_ldap_node_cert, *yealink_ldap_link_cert;
typedef int (*yealink_ldap_log_redirect_cb)(const char *msg, int len);


typedef struct List_num
{
    char			numb[YEALINK_DISPLAY_NUMB_MAXSZ];
    struct List_num		*Next;
    char            attr[YEALINK_DISPLAY_ATTR_MAXZ];
}yealink_ldap_node_num, *yealink_ldap_link_num;

typedef struct List_extra_attr
{
    char            attr[YEALINK_DISPLAY_ATTR_MAXZ];
    char			value[YEALINK_DISPLAY_NAME_MAXSZ];

    struct List_extra_attr *Next;
}yealink_ldap_node_extra_attr, *yealink_ldap_link_extra_attr;

typedef struct
{
    yealink_ldap_link_extra_attr extra_attr_list;
    
    void *reverse[4];
}yealink_ldap_extra;

typedef struct List_user
{
    char			name[YEALINK_DISPLAY_NAME_MAXSZ];
    int             count_num;
    yealink_ldap_link_num    num_list;
    struct List_user		*Next;
    void            *reserve;
}yealink_ldap_node_user, *yealink_ldap_link_user;


/**
* @brief   初始化配置
* @param
* @return
*/
void yealink_ldap_config_init(void);

/**
* @brief   释放配置
* @param
* @return
*/
void yealink_ldap_config_exit(void);

/**
* @brief   设置配置选项
* @param ldap_opt     [IN]    要设置的选项
* @param value        [IN]    要设置的值
* @return 0 成功 -1 失败
*/
int yealink_ldap_set_option(enum yealink_ldap_option ldap_opt, void *value);

/**
* @brief   获取版本号
* @param
* @return  版本号字符串
*/
const char *yealink_ldap_get_version(void);

/**
* @brief   初始化log
* @param log_mode   [IN]    设置log模式
* @param redir_cb   [IN]    回调函数指针 msg为log字符串, len为字符串长度
* @return FAIL
* @return SUCC
*/
int yealink_ldap_log_init(int log_mode, yealink_ldap_log_redirect_cb redir_cb);

/**
* @brief   设置打印的级别，默认TRACE_LEVEL4
* @param level        [IN]    打印级别
* @return FAIL
* @return SUCC
*/
int yealink_ldap_set_log_level(yealink_ldap_log_level level);

/**
* @brief   发送ldap bind request给服务器，请求建立连接，
* @brief   bind成功后可以发起后续的查询，读取结果，
* @brief   abandon请求。
* @param yealinkld        [OUT] 连接成功的ldap 地址
* @return FAIL, YEALINK_LDAP_ERR_AUTH
* @return SUCC
*/
int yealink_ldap_bind(void **yealinkld);

/**
* @brief   发送unbind request给服务器，请求断开连接。
* @param yealinkld    [IN]  连接成功的ldap 地址
* @return FAIL
* @return SUCC
*/
int yealink_ldap_ubind(void **yealinkld);

/**
* @brief   默认到服务器查询字符串search_key开头的联系人，当ldap.search_type为1时，搜索包含search_key字符串的联系人
* @param yealinkld     [IN]   连接成功的ldap 地址
* @param search_key    [IN]   查询关键字
* @param pmsgid        [OUT]  消息ID, 用于区分哪个search
* @return FAIL
* @return SUCC
*/
int yealink_ldap_search(void **yealinkld, int *pmsgid, char *search_key);

/**
* @brief   读取查询结果
* @param yealinkld       [IN]    连接成功的ldap 地址
* @param msgid           [IN]    消息ID, 用于区分哪个search
* @param timeout         [IN]    超时时间
* @param hp              [OUT]   输出链表首地址
* @return FAIL
* @return SUCC
*/
int yealink_ldap_result(void **yealinkld, int msgid, struct timeval *timeout, yealink_ldap_link *hp);
int yealink_ldap_result_intergated(void **yealinkld, int msgid, struct timeval *timeout, yealink_ldap_link_user *hp);

/**
* @brief   释放查询到的结果链表
* @param yealinkld       [IN]    连接成功的ldap 地址
* @param msgid           [IN]    消息ID, 用于区分哪个search
* @return FAIL
* @return SUCC
*/
int yealink_ldap_abandon(void **yealinkld, int msgid);

/**
* @brief   释放链表内存
* @param hp      [IN]  链表首地址
* @return FAIL
* @return SUCC
*/
int yealink_ldap_memfree(yealink_ldap_link result);
int yealink_ldap_memfree_integrated(yealink_ldap_link_user result);
/**
* @brief   释放链表内存
* @param hp      [IN]  链表首地址
* @return FAIL
* @return SUCC
*/
int yealink_ldap_memfree_cert(yealink_ldap_link_cert result);

/**
* @brief   未输入查询关键字时，默认发起的查询并最多返回
*             YEALINK_DEF_SEARCH_MAXSZ 个结果
* @param timeout           [IN]    超时时间
* @param hp                [OUT]   输出链表首地址
* @return FAIL, YEALINK_LDAP_ERR_AUTH
* @return SUCC
*/
int yealink_ldap_default_search(struct timeval *timeout, yealink_ldap_link *hp);
int yealink_ldap_default_search_integrated(struct timeval *timeout, yealink_ldap_link_user *hp);

/**
* @brief   Lync接口，通过LDAP下载根证书
* @param user              [IN]    用户名
* @param pw                [IN]    密码
* @param host              [IN]    域
* @param outhp             [OUT]   输出链表首地址
* @param ref               [IN/OUT]保留
* @return 0                        成功
* @return -1                       失败
*/
int yealink_ldap_get_uc_rootca(char *user, char *pw, char *host, yealink_ldap_link_cert *outhp, void *ref);


/*************************************************************
*
* Lync版本需要获取所有请求的属性值，返回的结果不需要过滤，对
* LDAP查询过程相关接口进行封装
*
*************************************************************/
typedef void* 	        ldap_h;
typedef void* 	        ldap_msg_h;
typedef void* 	        ldap_entry_h;
typedef void* 	        ldap_ber_h;
typedef char* 	        ldap_attr_h;
typedef void** 	        ldap_bervals_h;
typedef void* 	        ldap_berval_h;
typedef char* 	        ldap_val_h;
typedef unsigned long   ldap_len_h;

/**
* @brief   读取查询未解析的LDAP数据结果
* @param yealinkld           [IN]    连接成功的ldap 地址
* @param msgid               [IN]    消息ID, 用于区分哪个search
* @param timeout             [IN]    超时时间
* @param  msg                [OUT]   输出数据链表首地址
* @return FAIL
* @return SUCC
*/
int yealink_ldap_result_msg(ldap_h *yealinkld, int msgid, struct timeval *timeout, ldap_msg_h *msg);

/**
* @brief   获取第一个数据节点
* @param yealinkld           [IN]    连接成功的ldap 地址
* @param msg                 [IN]    数据链表首地址
* @return      数据节点地址
*/
ldap_entry_h yealink_ldap_first_entry(ldap_h yealinkld, ldap_msg_h msg);

/**
* @brief   获取下一个数据节点
* @param yealinkld           [IN]    连接成功的ldap 地址
* @param msg                 [IN]    数据节点地址
* @return      数据节点地址
*/
ldap_entry_h yealink_ldap_next_entry(ldap_h yealinkld, ldap_entry_h entry);

/**
* @brief   获取数据节点的第一个属性名称
* @param yealinkld           [IN]    连接成功的ldap 地址
* @param entry               [IN]    数据节点地址
* @param berout              [OUT]    属性数据节点
* @return    节点属性名称
*/
ldap_attr_h yealink_ldap_first_attribute(ldap_h yealinkld, ldap_entry_h entry, ldap_ber_h *berout);

/**
* @brief   获取数据节点的下一个属性名称
* @param yealinkld           [IN]    连接成功的ldap 地址
* @param entry               [IN]    数据节点地址
* @param berout              [IN]    属性数据节点
* @return    节点属性名称
*/
ldap_attr_h yealink_ldap_next_attribute(ldap_h yealinkld, ldap_entry_h entry, ldap_ber_h berout);

/**
* @brief   获取数据节点的属性值结构体
* @param yealinkld           [IN]    连接成功的ldap 地址
* @param entry               [IN]    数据节点地址
* @param target              [IN]    属性名称
* @return    属性值结构体指针
*/
ldap_bervals_h yealink_ldap_get_bervals(ldap_h yealinkld, ldap_entry_h entry, char *target);

/**
* @brief   获取数据节点的属性值
* @param yealinkld           [IN]    连接成功的ldap 地址
* @param entry               [IN]    数据节点地址
* @param berout              [IN]    属性值结构体指针
* @return    属性值
*/
ldap_val_h yealink_ldap_get_val(ldap_h yealinkld, ldap_entry_h entry, ldap_berval_h target);

/**
* @brief   获取数据节点的属性值数据长度
* @param yealinkld           [IN]    连接成功的ldap 地址
* @param entry               [IN]    数据节点地址
* @param berout              [IN]    属性值结构体指针
* @return    属性值数据长度
*/
ldap_len_h yealink_ldap_get_len(ldap_h yealinkld, ldap_entry_h entry, ldap_berval_h target);

/**
* @brief   释放结果数据链表
* @param   msg               [IN]    数据链表首地址
* @return
*/
void yealink_ldap_msgfree(ldap_msg_h msg);

/**
* @brief   释放数据节点
* @param   ber               [IN]    数据节点地址
* @return
*/
void yealink_ldap_ber_free(ldap_ber_h ber);

/**
* @brief   释放属性名称
* @param   attr              [IN]    名称地址
* @return
*/
void yealink_ldap_attr_free(ldap_attr_h attr);

/**
* @brief   释放属性值结构体
* @param   vals              [IN]    属性值结构体地址
* @return
*/
void yealink_ldap_bervals_free(ldap_bervals_h vals);

/**
* @brief   根据错误码返回相应的描述
* @param   err              [IN]    错误码
* @param   errbuf           [OUT]存放错误对应字符串的buf
* @param   errbuflen        [IN]存放错误对应字符串的buf的长度
* @return  0 成功， -1 参数错误， -2 buf太短
*/
int yealink_ldap_err2string(int err, char *errbuf, int errbuflen);


#ifdef  __cplusplus
}
#endif
#endif	/* _YEALINK_LDAP_H_ */

