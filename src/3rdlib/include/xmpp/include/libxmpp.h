/*
 * DESCRIPTION:	Copyright(c) 2012-2012 Xiamen Yealink Network Technology Co,.Ltd
 * AUTHOR:	dsw
 * HISTORY:
 * DATE:	2012-10-16
 */
#ifndef _LIBXMPP_H
#define _LIBXMPP_H

#include"model.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdarg.h>
#include<errno.h>
#include<pthread.h>
#include<string.h>

#include"strophe.h"

#include"list_head.h"


#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * xmpp_request_t - user available requests
     * @__ROSTER_ALL:	get all contacts
     * @__ROSTER_GROUP:	get a group of contacts
     * @__ROSTER_SINGLE:	get a single contact
     *
     * @__PRESENCE_ALL:	get presence information of all contacts
     * @__PRESENCE_GROUP:	get presence information of a group of contacts
     * @__PRESENCE_SINGLE:	get presence information of a single contacts
     *
     * @__AVATAR_SINGLE:	get avatar of a user
     * @__CARD_SINGLE:	get card info of a user
     *
     * @__ADDRESSBOOK_ALL:	get addressbook
     *
     * @__UNKNOWN:		unknown request
     */
    typedef enum _xmpp_request 
    {
    	XMPP_REQUEST_ROSTER_SINGLE 	= 0x10000001,
    	XMPP_REQUEST_ROSTER_GROUP 	= 0x20000001,
    	XMPP_REQUEST_ROSTER_ALL		= 0x40000001,

    	XMPP_REQUEST_PRESENCE_SINGLE	= 0x10000002,
    	XMPP_REQUEST_PRESENCE_GROUP 	= 0x20000002,
    	XMPP_REQUEST_PRESENCE_ALL	= 0x40000002,

    	XMPP_REQUEST_AVATAR_SINGLE	= 0x10000004,

    	XMPP_REQUEST_CARD_SINGLE	= 0x10000008,

    	XMPP_REQUEST_ADDRESSBOOK_ALL	= 0x40000010,

        XMPP_REQUEST_STATUS_SET     = 0x10000020,

    	XMPP_REQUEST_UNKNOWN		= 0x11111111
    } xmpp_request_t;

    #define XMPP_REQUEST_ROSTER_MASK	  0x00000001
    #define XMPP_REQUEST_PRESENCE_MASK	  0x00000002
    #define XMPP_REQUEST_AVATAR_MASK	  0x00000004
    #define XMPP_REQUEST_CARD_MASK	  	  0x00000008
    #define XMPP_REQUEST_ADDRESSBOOK_MASK 	  0x00000010
    #define XMPP_REQUEST_STATUS_SET_MASK     0x00000020

    #define XMPP_REQUEST_SINGLE_MASK	  0x10000000
    #define XMPP_REQUEST_GROUP_MASK		  0x20000000
    #define XMPP_REQUEST_ALL_MASK		  0x40000000

    #define __REQUEST_ROSTER(req)		\
    	((req) & XMPP_REQUEST_ROSTER_MASK)

    #define __REQUEST_PRESENCE(req)		\
    	((req) & XMPP_REQUEST_PRESENCE_MASK)

    #define	__REQUEST_AVATAR(req)		\
    	((req) & XMPP_REQUEST_AVATAR_MASK)

    #define __REQUEST_CARD(req)		\
    	((req) & XMPP_REQUEST_CARD_MASK)

    #define	__REQUEST_ADDRESSBOOK(req)	\
    	((req) & XMPP_REQUEST_ADDRESSBOOK_MASK)

    #define __REQUEST_ALL(req)		\
    	((req) & XMPP_REQUEST_ALL_MASK)	

    #define __REQUEST_GROUP(req)		\
    	((req) & XMPP_REQUEST_GROUP_MASK)

    #define __REQUEST_SINGLE(req)		\
    	((req) & XMPP_REQUEST_SINGLE_MASK)

    #define __REQUEST_SET_STATE(req)    \
        ((req) & XMPP_REQUEST_STATUS_SET_MASK)
    /**
     * xmpp_errno_t - error code returned by libxmpp functions
     */
    typedef enum _xmpp_errno 
    {
    	XMPP_ERR_NONE = 0,	/* no error */
    	XMPP_ERR_NOT_CONNECTED,	/* not connected */
    	XMPP_ERR_CONN,		/* connection error */
    	XMPP_ERR_REQUEST,	/* invalid request */
    	XMPP_ERR_REPLY,		/* reply error */
    	XMPP_ERR_ACCOUNT,	/* invalid account */
    	XMPP_ERR_TIMEOUT,	/* operation timeout */
    	XMPP_ERR_INTERNAL,	/* internal error */
    	XMPP_ERR_MEM,		/* memory allocation error */
    	XMPP_ERR_ARG		/* invalid argument */
    } xmpp_errno_t;

    /** 
     * PRESENCE_TYPE_T - 可设置的presence状态
     */
    typedef enum PRESENCE_TYPE
    {
    	PRESENCE_TYPE_NONE = -1,
    	PRESENCE_TYPE_ONLINE,  // 用户在线而且准备好沟通 //AtDesk
    	PRESENCE_TYPE_CHATTY,  // 用户在线而且希望和人聊天 //chat
    	PRESENCE_TYPE_AWAY,	   // 用户在线,但是已经离开电脑10分钟 //Away
    	PRESENCE_TYPE_EXTENDED_AWAY,  // Extended Away //xa
    	PRESENCE_TYPE_BUSY,	   // 用户处于忙碌状态 //UserBusy
    	PRESENCE_TYPE_OFFLINE, // 用户处于脱机状态 //Invisible
    	PRESENCE_TYPE_UNKNOWN, // 未知状态
    	PRESENCE_TYPE_MAX,
    } PRESENCE_TYPE_T;
    /**
     * xmpp_conn_status_t - connection status
     */
    typedef enum _xmpp_conn_status 
    {
    	XMPP_CONN_UNKNOWN = 0,	/* unknown status */
    	XMPP_CONN_CONNECTED,	/* connected */
    	XMPP_CONN_DISCONNECTED,	/* disconnected */
    	XMPP_CONN_ERROR		/* error */
    } xmpp_conn_status_t;

    /**
     * xmpp_reply_status_t - server reply status
     */
    typedef enum _xmpp_reply_status 
    {
    	XMPP_REPLY_UNKNOWN = 0,	/* unknown status */
    	XMPP_REPLY_OK,		/* reply ok */
    	XMPP_REPLY_ERROR	/* reply error */
    } xmpp_reply_status_t;

    /**
     * xmpp_disconn_t - used internally
     */
    typedef enum _xmpp_disconn 
    {
    	XMPP_DISCONN_UNKNOWN = 0,/* unknown */
    	XMPP_DISCONN_OPEN,	/* connection not established when open */
    	XMPP_DISCONN_CLIENT,	/* client disconnect the connection */
    	XMPP_DISCONN_SERVER	/* server disconnect the connection */
    } xmpp_disconn_t;

    typedef struct list_head	list_head_t;

    /**
     * xmpp_log_method_t - log method: print or syslog
     */
    typedef enum _xmpp_log_method
    { 
    	XMPP_LOG_UNKNOWN = 0, 
    	XMPP_LOG_PRINT, 
    	XMPP_LOG_SYSLOG 
    } xmpp_log_method_t;

    #define XMPP_LOG_METHOD_DEFAULT	XMPP_LOG_SYSLOG

    /*
     * log level definitions
     * I don't use enum definition, because the name 'xmpp_log_level_t'
     * is aloready used by libstrophe, and I have not yet come up with a
     * new name, so use macro definitions instead.
     */
    #define XMPP_LOG_LEVEL_MIN	0

    #define XMPP_LOG_LEVEL_UNKNOWN	0
    #define	XMPP_LOG_LEVEL_DEBUG	1
    #define XMPP_LOG_LEVEL_INFO	2
    #define XMPP_LOG_LEVEL_WARN	3
    #define XMPP_LOG_LEVEL_ERROR	4

    #define XMPP_LOG_LEVEL_MAX	4

    #define XMPP_LOG_LEVEL_DEFAULT	XMPP_LOG_LEVEL_WARN
    /**
     * XMPP_CTX - opaque context object
     */
    typedef struct _XMPP_CTX 
    {
    	int		magic;
    	xmpp_ctx_t *	xmpp_ctx;	/* xmpp context object */
    	xmpp_log_t *	xmpp_log;
    	pthread_mutex_t	mutex;		/* mutex lock */
    	pthread_t	xmpp_tid;	/* xmpp thread id */
    	list_head_t	list;		/* head of conn list */
    	unsigned int	counter;	/* global counter used as stanza id */
    	/* xmpp_log_level_t	log_level; */
    	int			log_level;
    	xmpp_log_method_t	log_method;
    	void *config_ctx;
    } XMPP_CTX;

    #define XMPP_CTX_MAGIC		0x12345678
    #define XMPP_CTX_MAGIC_ERR	0x12345679
    typedef struct _XMPP_CONN XMPP_CONN;

    typedef int	(* xmpp_push_handler) (char *xml_stream, size_t stream_len); 
    typedef void (* xmpp_disconnect_handler)(XMPP_CONN *conn, int errnono, void *disconn_private_data);
    /*
     * XMPP_CONN - opaque connection object
     *
     * choose a unique conn identifier different from each connection
     * to prevent chaos of the same id bewteen current connection and
     * the previous closed connection which may confuse __get_reply().
     * choose a random number for example.
     * member:	conn->identity
     * and use this identity as the prefix of the user request id
     */
    struct _XMPP_CONN 
    {
    	int		magic;
    	list_head_t	list;

    	XMPP_CTX *	ctx;
    	xmpp_conn_t *	xmpp_conn;	/* xmpp connection object */
    	xmpp_errno_t	conn_errno;	/* error code */

    	pthread_mutex_t	mutex;
    	pthread_cond_t	cond;

    	int	open_timeout_max;
    	int	request_timeout_max;

    	int	open_timeout_flag;
    	int	request_timeout_flag;

    	int	idle_timeout;

    	char *	xml_stream;
    	size_t	stream_len;

    	int	seq;			/* sequence number */
    	int	req;			/* user request */
    	char *	id;
    	char *	contact_jid;

    	char *	jid;			/* user's jid */
    	xmpp_conn_status_t	conn_status;
    	xmpp_reply_status_t	reply_status;
    	xmpp_disconn_t		disconn;

    	xmpp_push_handler	push_handler;
    	xmpp_push_handler	push_handler_presence;	/* special handler */

    	xmpp_disconnect_handler  disconnect_handler;
    	void *disconn_private_data;
        int idle_ping_interval;
        int pingID;
        int recvPingID;
    } ;

    #define	XMPP_CONN_MAGIC			0x19871007
    #define XMPP_CONN_MAGIC_ERR		0x19850909

    #define AVATAR_TYPE_SIZE		32
    #define	XMPP_BUFSIZE			1024

    #define	XMPP_TIMEOUT_DEFAULT_OPEN	30
    #define XMPP_TIMEOUT_DEFAULT_REQUEST	30


    #define	XMPP_SLEEP_SECONDS_INIT		1
    #define	XMPP_SLEEP_SECONDS_OPEN		1
    #define XMPP_SLEEP_SECONDS_REQUEST	1

    /* default idle timeout - in seconds */
    #define XMPP_TIMEOUT_DEFAULT_IDLE	120	/* 2 minutes */

    /*
     * choose a unique conn identifier different from each connection
     * to prevent chaos of the same id bewteen current connection and
     * the previous closed connection which may confuse __get_reply().
     * choose a random number for example.
     */
    #define	ID_PREFIX			            "Yealink.com."
    #define ID_ROSTER_PREFIX		        ID_PREFIX"roster."
    #define ID_PRESENCE_PREFIX		        ID_PREFIX"presence."
    #define ID_AVATAR_PREFIX		        ID_PREFIX"avatar."
    #define ID_VCARD_PREFIX			        ID_PREFIX"vcard."
    #define ID_ADDRESSBOOK_PREFIX		    ID_PREFIX"addressbook."
    #define ID_PRESENCE_STATUS_PREFIX       ID_PREFIX"presencestatus."

    /**
     * XMPP_CONN_INFO - arguments to xmpp_open()
     */
    typedef struct _XMPP_CONN_INFO 
    {
    #ifdef __cplusplus
    	int			log_level;
    	int			log_method;
    #else
    	xmpp_log_level_t 	log_level;	/* log level */
    	xmpp_log_method_t	log_method;
    #endif
    	int			open_timeout;
    	int			request_timeout;
    	char *			altdomain;
    	unsigned short 		altport;
    	char *			jid;		/* jid of user */
    	char *			pass;		/* password */
    	xmpp_push_handler	push_handler;	/* general handler */
    	xmpp_push_handler	push_handler_presence;	/* special handler */

    	int			idle_timeout;
    	
    	int         idle_ping_interval;
    	xmpp_disconnect_handler  disconnect_handler;
    	void *disconn_private_data;
    } XMPP_CONN_INFO;

    
    #define    XMPP_OPTIONS_APPEND_PROPERIES_FIELD             0x01
    #define    XMPP_OPTIONS_SET_PROPERIES_MANUAL_FIELD    0x02
    /** 
     * xmpp_set_features - open a connection to server
     * @conn:	  opaque connection object
     * @options:  the features options
     * @switch:   0 or 1
     *
     * return:	0 on success, failure otherwise 
     *
     */
    int xmpp_set_features(XMPP_CONN *conn, int options, void *value);

    
    /** 
     * xmpp_open - open a connection to server
     * @conn:	opaque connection object
     * @conn_info:	before call this function, the caller must fill at least 
     * 		four fields:
     * 		jid:		jid of the user
     * 		pass:		password of the user account
     * 		push_handler:
     * 		push_handler_presence:
     *
     * return:	0 on success, failure otherwise 
     *
     * errors:	XMPP_ERR_ACCOUNT
     * 		XMPP_ERR_TIMEOUT
     * 		XMPP_ERR_INTERNAL
     */
    int 	xmpp_open(XMPP_CONN *conn, XMPP_CONN_INFO *conn_info);

    /** 
     * xmpp_request - make a request to server
     * @conn:
     * @req:	request command
     * @stream:	result stream are returned through this pointer
     * @stream_len:	length of returned stream
     * @name:	empty, "group_name" or "contact_jid" depending on the @req
     * 
     * return:	on success:
     * 			return value is 0;
     * 			*stream holds the stream buffer
     * 			*stream_len is set to the length of the stream 
     * 			buffer
     *
     * 		roster:
     * 			stream = XML stream which server returned
     * 			stream_len = XML stream length
     *
     * 		presence:
     * 			stream = NULL
     * 			streamm_len = 0
     *
     * 		avatar:	
     * 			stream = Type + Base-64 encoded avatar data
     * 			format: |Type: 20 bytes|Base-64 avatar|null byte|
     * 			stream_len = length of Base-64 avatar 
     *
     * 		card:
     * 			stream = XML stream which server returned
     *
     * 		addressbook:
     * 			stream = XML stream which server returned
     * 			stream_len = XML stream length
     *
     * 		on failure:
     * 			return value is -1
     * 			*stream = NULL
     * 			*stream_len = 0
     *
     * note:	the stream buffer returned is allocated dynamically,
     * 		the callee must free it after use.
     *
     * errors:	XMPP_ERR_NOT_CONNECTED
     * 		XMPP_ERR_REQUEST
     * 		XMPP_ERR_TIMEOUT
     * 		XMPP_ERR_INTERNAL
     *
     * prototypes:	xmpp_request(conn, req, stream, stream_len)
     * 		xmpp_request(conn, req, stream, stream_len, name)
     */
    int	xmpp_request(XMPP_CONN *conn, int req, 
    			char **xml_stream, size_t *stream_len, ...);

    /**
     * xmpp_close - close a xmpp connection
     *
     * return:	0 on success, failure otherwise
     * errors:	XMPP_ERR_NOT_CONNECTED
     */
    int	xmpp_close(XMPP_CONN *conn);
    	
    /* 
     * stop xmpp stub
     */
    int	xmpp_destroy();

    /*
     * get a string-represented version of libxmpp
     */
    const char *xmpp_get_version(void);

    /* helper functions for processing errors */
    xmpp_errno_t	xmpp_errno(XMPP_CONN *conn);

    /*
     * Caution! This function is not reentrant and not thread-safe
     */
    char *		xmpp_strerror(XMPP_CONN *conn);
    void		xmpp_perror(XMPP_CONN *conn);

    char *		xmpp_strerror_internal(XMPP_CONN *conn);

    /*
     * set idle timeout for this connection
     */
    void	xmpp_set_idle_timeout(XMPP_CONN *conn, int timeout);

    /*--- void	xmpp_set_open_timeout(XMPP_CONN *conn, int timeout); ---*/
    /*--- void	xmpp_set_request_timeout(XMPP_CONN *conn, int timeout); ---*/

    /* 
     * xmpp_log_set_level - set log level of libxmpp
     * @level:	if level is an invalid value
     * 			then unchanged
     * 		else if level is XMPP_LOG_LEVEL_UNKNOWN
     * 			reset to default value (XMPP_LOG_LEVEL_WARN)
     * 		else 
     * 			set to user's value
     */
    void 	xmpp_log_set_level(int level);

    /*
     * set log method as print
     */
    void	xmpp_log_set_print();

    /*
     * set log method as syslog
     */
    void	xmpp_log_set_syslog();

    void	__xmpp_log(int level, const char *file, const char *function,
    		int line, const char *format, ...);

    /* for self-test only, will be removed when commit */
    #ifdef USE_DEBUG
    #define	__DEBUG__(...)	__xmpp_log(XMPP_LOG_LEVEL_DEBUG, __FILE__,\
    			__FUNCTION__, __LINE__, __VA_ARGS__)
    #else
    #define __DEBUG__(...)	;
    #endif

    #define XMPP_DEBUG(...)	__xmpp_log(XMPP_LOG_LEVEL_DEBUG, __FILE__,	\
    			__FUNCTION__, __LINE__, __VA_ARGS__)

    #define	XMPP_INFO(...)	__xmpp_log(XMPP_LOG_LEVEL_INFO, __FILE__,	\
    			__FUNCTION__, __LINE__, __VA_ARGS__)

    #define XMPP_WARN(...)	__xmpp_log(XMPP_LOG_LEVEL_WARN, __FILE__,	\
    			__FUNCTION__, __LINE__, __VA_ARGS__)

    #define XMPP_ERROR(...)	__xmpp_log(XMPP_LOG_LEVEL_ERROR, __FILE__,	\
    			__FUNCTION__, __LINE__, __VA_ARGS__)

    #define MUTEX_INIT(mutex)	\
    	do {			\
    		if(pthread_mutex_init(mutex) != 0) {\
    			LXMP_ERR("pthread_muetx_init() error in line %d", __LINE__);\
    		}		\
    	} while(0)

    #define MUTEX_DESTROY(mutex)	\
    	do {			\
    		if(pthread_mutex_destroy(mutex) != 0) {\
    			LXMP_ERR("pthread_muetx_destroy() error in line %d", __LINE__);\
    		}		\
    	} while(0)

    #define	MUTEX_LOCK(mutex)	\
    	do {			\
    		if(pthread_mutex_lock(mutex) != 0) {\
    			LXMP_ERR("pthread_mutex_lock() error in line %d", __LINE__);\
    		}		\
    	} while(0)

    #define MUTEX_UNLOCK(mutex)	\
    	do {			\
    		if(pthread_mutex_unlock(mutex) != 0) {\
    			LXMP_ERR("pthread_mutex_unlock() error in line %d", __LINE__);\
    		} 		\
    	} while(0)

    #define COND_SIGNAL(cond)	\
    	do {			\
    		if(pthread_cond_signal(cond) != 0) {\
    			LXMP_ERR("pthread_cond_signal() error in line %d", __LINE__);\
    		}		\
    	} while(0)

    #define COND_WAIT(cond)		\
    	do {			\
    		if(pthread_cond_wait(cond) != 0) {\
    			LXMP_ERR("pthread_cond_wait() error in line %d", __LINE__);\
    		}		\
    	} while(0)

    #define COND_TIMEDWAIT(cond, mutex)	\
    	do {				\
    		if(pthread_cond_timedwait(cond, mutex) != 0) {\
    			LXMP_ERR("pthread_cond_timedwait() error in line %d", __LINE__);\
    		}			\
    	} while(0)

    #define	CTX_LOCK(ctx)		MUTEX_LOCK(&(ctx->mutex))
    #define CTX_UNLOCK(ctx)		MUTEX_UNLOCK(&(ctx->mutex))
    #define CONN_LOCK(conn)		MUTEX_LOCK(&(conn->mutex))
    #define CONN_UNLOCK(conn)	MUTEX_UNLOCK(&(conn->mutex))

    #define CONN_SIGNAL(conn)	COND_SIGNAL(&(conn->cond))
    #define CONN_TIMEDWAIT(conn)	COND_TIMEDWAIT(&(conn->cond))

    #define	REPPLY_STATUS_SET(conn, status)	\
    	((conn)->reply_status = status)

    #define REPLY_STATUS_RESET(conn)	\
    	((conn)->reply_status = XMPP_REPLY_UNKNOWN)
#ifdef __cplusplus
}
#endif	

#endif	/* libxmpp.h */
