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
*    �޸�ʱ��: 2013.3.9   �޸��� :  hyy
*    ԭ�汾��: 3.0.0.3    �°汾��: 3.0.1.0
*    �޸�����:
*    ����syslog�淶.pdf�ĵ��޸�log��ʽ,����yealink_ldap_log_init�ӿ�
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

/* ��Ļ����ʾ����ϵ�����ֵ���󳤶� */
#define YEALINK_DISPLAY_NAME_MAXSZ	100
/* ��Ļ����ʾ�ĺ������󳤶� */
#define YEALINK_DISPLAY_NUMB_MAXSZ	30
/* ���ظ�֤��������󳤶� */
#define YEALINK_CERT_NAME_MAXSZ	100
/* ��֤�����󳤶� */
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
    YEALINK_LDAP_OPTIONS_PORT = 0,		/* �˿����� */
    YEALINK_LDAP_OPTIONS_VERSION,		/* �汾���� */
    YEALINK_LDAP_OPTIONS_MAXHITS,		/* ��������������� */
    YEALINK_LDAP_OPTIONS_MD5_ENABLE,	/* MD5ʹ������ */
    YEALINK_LDAP_OPTIONS_HOST,			/* ��������ַ���� */
    YEALINK_LDAP_OPTIONS_DN,			/* DNֵ���� */
    YEALINK_LDAP_OPTIONS_USER,			/* �û������� */
    YEALINK_LDAP_OPTIONS_PASSWORD,		/* �������� */
    YEALINK_LDAP_OPTIONS_BASE,			/* ��Ŀ¼���� */
    YEALINK_LDAP_OPTIONS_NAME_FILTER,	/* ���ֹ����������� */
    YEALINK_LDAP_OPTIONS_NUMBER_FILTER,	/* ��������������� */
    YEALINK_LDAP_OPTIONS_NAME_ATTR,		/* ������������ */
    YEALINK_LDAP_OPTIONS_NUMBER_ATTR,	/* ������������ */
    YEALINK_LDAP_OPTIONS_DISPALY_NAME,	/* ��ʾ�������� */
    YEALINK_LDAP_OPTIONS_REPLACE_EMPTY, /* ����Ϊ�յ���ʾ�������� */
    YEALINK_LDAP_OPTIONS_TLS_MODE,      /* LDAP ʹ��ģʽ */
    YEALINK_LDAP_OPTIONS_TLS_CA_DIR,    /* ca �淽Ŀ¼ */
    YEALINK_LDAP_OPTIONS_TLS_CERT_FILE, /* ֤��·�� */
    YEALINK_LDAP_OPTIONS_NAME_SEPARATE_SYMBOL,/*���ֲ��Ź�˾�����Եķָ�������*/
    YEALINK_LDAP_OPTIONS_IPADDR_MODE,      /* IPģʽ 0 IPv4,1 IPv6,2 IPv4&IPv6 */
	YEALINK_LDAP_OPTIONS_NTLM_ENABLE,	/* NTLMʹ������ */
    YEALINK_LDAP_OPTIONS_NETWORK_TIMEOUT,      /* TCP���ӳ�ʱ */
    YEALINK_LDAP_OPTIONS_EXTRA_ATTR,      /* ���ֺͺ������������ */
    YEALINK_LDAP_OPTIONS_DEFAULT_SEARCH_FILTER, /*Ĭ��������������*/
    YEALINK_LDAP_OPTIONS_SEARCH_TYPE,   /*�������� 0-��ͷƥ�䣬1-ģ������*/
    YEALINK_LDAP_OPTIONS_NET_PRIORITY,  /*ipv4 & ipv6 ģʽ�µ��������ȼ����ã�0��ʾIPv6���ȣ�1��ʾIPv4����*/
    YEALINK_LDAP_OPTIONS_EXTRA_DISPLAY_ATTR, /*�������ʾ����*/
    YEALINK_LDAP_OPTIONS_TLS_CIPHER_LIST, /*SSL ģʽ�����㷨�׼� ��Чֵ�͸�ʽΪopenssl�����׼������ø�ʽ*/
	YEALINK_LDAP_OPTIONS_END			/* �����ã������ڽ�����־ */
};

typedef enum _yealink_ldap_log_mode
{
    YEALINK_LDAP_LOG_NONE =    0x0000,
    YEALINK_LDAP_LOG_SYS  =    0x0001, /* �ô�λ ��LOG�������SYSLOG */
    YEALINK_LDAP_LOG_CONS =    0x0002, /* �ô�λ ��LOG��������ն� */
    YEALINK_LDAP_LOG_TIME =    0x0004, /* �ô�λ LOG������ʱ�� */
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
* @brief   ��ʼ������
* @param
* @return
*/
void yealink_ldap_config_init(void);

/**
* @brief   �ͷ�����
* @param
* @return
*/
void yealink_ldap_config_exit(void);

/**
* @brief   ��������ѡ��
* @param ldap_opt     [IN]    Ҫ���õ�ѡ��
* @param value        [IN]    Ҫ���õ�ֵ
* @return 0 �ɹ� -1 ʧ��
*/
int yealink_ldap_set_option(enum yealink_ldap_option ldap_opt, void *value);

/**
* @brief   ��ȡ�汾��
* @param
* @return  �汾���ַ���
*/
const char *yealink_ldap_get_version(void);

/**
* @brief   ��ʼ��log
* @param log_mode   [IN]    ����logģʽ
* @param redir_cb   [IN]    �ص�����ָ�� msgΪlog�ַ���, lenΪ�ַ�������
* @return FAIL
* @return SUCC
*/
int yealink_ldap_log_init(int log_mode, yealink_ldap_log_redirect_cb redir_cb);

/**
* @brief   ���ô�ӡ�ļ���Ĭ��TRACE_LEVEL4
* @param level        [IN]    ��ӡ����
* @return FAIL
* @return SUCC
*/
int yealink_ldap_set_log_level(yealink_ldap_log_level level);

/**
* @brief   ����ldap bind request�������������������ӣ�
* @brief   bind�ɹ�����Է�������Ĳ�ѯ����ȡ�����
* @brief   abandon����
* @param yealinkld        [OUT] ���ӳɹ���ldap ��ַ
* @return FAIL, YEALINK_LDAP_ERR_AUTH
* @return SUCC
*/
int yealink_ldap_bind(void **yealinkld);

/**
* @brief   ����unbind request��������������Ͽ����ӡ�
* @param yealinkld    [IN]  ���ӳɹ���ldap ��ַ
* @return FAIL
* @return SUCC
*/
int yealink_ldap_ubind(void **yealinkld);

/**
* @brief   Ĭ�ϵ���������ѯ�ַ���search_key��ͷ����ϵ�ˣ���ldap.search_typeΪ1ʱ����������search_key�ַ�������ϵ��
* @param yealinkld     [IN]   ���ӳɹ���ldap ��ַ
* @param search_key    [IN]   ��ѯ�ؼ���
* @param pmsgid        [OUT]  ��ϢID, ���������ĸ�search
* @return FAIL
* @return SUCC
*/
int yealink_ldap_search(void **yealinkld, int *pmsgid, char *search_key);

/**
* @brief   ��ȡ��ѯ���
* @param yealinkld       [IN]    ���ӳɹ���ldap ��ַ
* @param msgid           [IN]    ��ϢID, ���������ĸ�search
* @param timeout         [IN]    ��ʱʱ��
* @param hp              [OUT]   ��������׵�ַ
* @return FAIL
* @return SUCC
*/
int yealink_ldap_result(void **yealinkld, int msgid, struct timeval *timeout, yealink_ldap_link *hp);
int yealink_ldap_result_intergated(void **yealinkld, int msgid, struct timeval *timeout, yealink_ldap_link_user *hp);

/**
* @brief   �ͷŲ�ѯ���Ľ������
* @param yealinkld       [IN]    ���ӳɹ���ldap ��ַ
* @param msgid           [IN]    ��ϢID, ���������ĸ�search
* @return FAIL
* @return SUCC
*/
int yealink_ldap_abandon(void **yealinkld, int msgid);

/**
* @brief   �ͷ������ڴ�
* @param hp      [IN]  �����׵�ַ
* @return FAIL
* @return SUCC
*/
int yealink_ldap_memfree(yealink_ldap_link result);
int yealink_ldap_memfree_integrated(yealink_ldap_link_user result);
/**
* @brief   �ͷ������ڴ�
* @param hp      [IN]  �����׵�ַ
* @return FAIL
* @return SUCC
*/
int yealink_ldap_memfree_cert(yealink_ldap_link_cert result);

/**
* @brief   δ�����ѯ�ؼ���ʱ��Ĭ�Ϸ���Ĳ�ѯ����෵��
*             YEALINK_DEF_SEARCH_MAXSZ �����
* @param timeout           [IN]    ��ʱʱ��
* @param hp                [OUT]   ��������׵�ַ
* @return FAIL, YEALINK_LDAP_ERR_AUTH
* @return SUCC
*/
int yealink_ldap_default_search(struct timeval *timeout, yealink_ldap_link *hp);
int yealink_ldap_default_search_integrated(struct timeval *timeout, yealink_ldap_link_user *hp);

/**
* @brief   Lync�ӿڣ�ͨ��LDAP���ظ�֤��
* @param user              [IN]    �û���
* @param pw                [IN]    ����
* @param host              [IN]    ��
* @param outhp             [OUT]   ��������׵�ַ
* @param ref               [IN/OUT]����
* @return 0                        �ɹ�
* @return -1                       ʧ��
*/
int yealink_ldap_get_uc_rootca(char *user, char *pw, char *host, yealink_ldap_link_cert *outhp, void *ref);


/*************************************************************
*
* Lync�汾��Ҫ��ȡ�������������ֵ�����صĽ������Ҫ���ˣ���
* LDAP��ѯ������ؽӿڽ��з�װ
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
* @brief   ��ȡ��ѯδ������LDAP���ݽ��
* @param yealinkld           [IN]    ���ӳɹ���ldap ��ַ
* @param msgid               [IN]    ��ϢID, ���������ĸ�search
* @param timeout             [IN]    ��ʱʱ��
* @param  msg                [OUT]   ������������׵�ַ
* @return FAIL
* @return SUCC
*/
int yealink_ldap_result_msg(ldap_h *yealinkld, int msgid, struct timeval *timeout, ldap_msg_h *msg);

/**
* @brief   ��ȡ��һ�����ݽڵ�
* @param yealinkld           [IN]    ���ӳɹ���ldap ��ַ
* @param msg                 [IN]    ���������׵�ַ
* @return      ���ݽڵ��ַ
*/
ldap_entry_h yealink_ldap_first_entry(ldap_h yealinkld, ldap_msg_h msg);

/**
* @brief   ��ȡ��һ�����ݽڵ�
* @param yealinkld           [IN]    ���ӳɹ���ldap ��ַ
* @param msg                 [IN]    ���ݽڵ��ַ
* @return      ���ݽڵ��ַ
*/
ldap_entry_h yealink_ldap_next_entry(ldap_h yealinkld, ldap_entry_h entry);

/**
* @brief   ��ȡ���ݽڵ�ĵ�һ����������
* @param yealinkld           [IN]    ���ӳɹ���ldap ��ַ
* @param entry               [IN]    ���ݽڵ��ַ
* @param berout              [OUT]    �������ݽڵ�
* @return    �ڵ���������
*/
ldap_attr_h yealink_ldap_first_attribute(ldap_h yealinkld, ldap_entry_h entry, ldap_ber_h *berout);

/**
* @brief   ��ȡ���ݽڵ����һ����������
* @param yealinkld           [IN]    ���ӳɹ���ldap ��ַ
* @param entry               [IN]    ���ݽڵ��ַ
* @param berout              [IN]    �������ݽڵ�
* @return    �ڵ���������
*/
ldap_attr_h yealink_ldap_next_attribute(ldap_h yealinkld, ldap_entry_h entry, ldap_ber_h berout);

/**
* @brief   ��ȡ���ݽڵ������ֵ�ṹ��
* @param yealinkld           [IN]    ���ӳɹ���ldap ��ַ
* @param entry               [IN]    ���ݽڵ��ַ
* @param target              [IN]    ��������
* @return    ����ֵ�ṹ��ָ��
*/
ldap_bervals_h yealink_ldap_get_bervals(ldap_h yealinkld, ldap_entry_h entry, char *target);

/**
* @brief   ��ȡ���ݽڵ������ֵ
* @param yealinkld           [IN]    ���ӳɹ���ldap ��ַ
* @param entry               [IN]    ���ݽڵ��ַ
* @param berout              [IN]    ����ֵ�ṹ��ָ��
* @return    ����ֵ
*/
ldap_val_h yealink_ldap_get_val(ldap_h yealinkld, ldap_entry_h entry, ldap_berval_h target);

/**
* @brief   ��ȡ���ݽڵ������ֵ���ݳ���
* @param yealinkld           [IN]    ���ӳɹ���ldap ��ַ
* @param entry               [IN]    ���ݽڵ��ַ
* @param berout              [IN]    ����ֵ�ṹ��ָ��
* @return    ����ֵ���ݳ���
*/
ldap_len_h yealink_ldap_get_len(ldap_h yealinkld, ldap_entry_h entry, ldap_berval_h target);

/**
* @brief   �ͷŽ����������
* @param   msg               [IN]    ���������׵�ַ
* @return
*/
void yealink_ldap_msgfree(ldap_msg_h msg);

/**
* @brief   �ͷ����ݽڵ�
* @param   ber               [IN]    ���ݽڵ��ַ
* @return
*/
void yealink_ldap_ber_free(ldap_ber_h ber);

/**
* @brief   �ͷ���������
* @param   attr              [IN]    ���Ƶ�ַ
* @return
*/
void yealink_ldap_attr_free(ldap_attr_h attr);

/**
* @brief   �ͷ�����ֵ�ṹ��
* @param   vals              [IN]    ����ֵ�ṹ���ַ
* @return
*/
void yealink_ldap_bervals_free(ldap_bervals_h vals);

/**
* @brief   ���ݴ����뷵����Ӧ������
* @param   err              [IN]    ������
* @param   errbuf           [OUT]��Ŵ����Ӧ�ַ�����buf
* @param   errbuflen        [IN]��Ŵ����Ӧ�ַ�����buf�ĳ���
* @return  0 �ɹ��� -1 �������� -2 buf̫��
*/
int yealink_ldap_err2string(int err, char *errbuf, int errbuflen);


#ifdef  __cplusplus
}
#endif
#endif	/* _YEALINK_LDAP_H_ */

