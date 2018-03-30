/******************************************************************************************************
Copyright (C) 2008 ,Yealink Network Technology Co.,Ltd. 

File Name:ldapsearch.h
Author:wxf
Version: 1.0
Date: 2009.06.09

History:

Abstract:

接口调用:
	1、发起bind;
	2、发起ubind, search, result,abandon, memfree等
	
	注意:bind成功后才能发起其他请求
		search成功后才能发起result,abandon请求
		result成功后，最后一定要memfree释放内存

	
*******************************************************************************************************/
#ifndef LDAP_SEARCH_H_
#define LDAP_SEARCH_H_
#include <stdio.h>
#include <stdlib.h>

#ifndef YEALINK_LDAP_SEARCH_H
#define YEALINK_LDAP_SEARCH_H

#ifdef  __cplusplus
extern "C" {
#endif


#define YEALINK_LDAP_CFG_FILE			"/phone/config/Contacts/LDAP.cfg"
#define YEALINK_LDAP_SEC_NAME			"LDAP"

#define YEALINK_LDAP_SERVER_ADDR		"host"
#define YEALINK_LDAP_PORT				"port"
#define YEALINK_LDAP_USER				"user"
#define YEALINK_LDAP_PSWD				"pswd"
#define YEALINK_LDAP_VERSION			"version"



#define YEALINK_LDAP_BASE				"base"
#define YEALINK_LDAP_MAX_HITS			"MaxHits"
#define YEALINK_LDAP_NAME_FILTER		"NameFilter"
#define YEALINK_LDAP_NUMBER_FILTER		"NumberFilter"
#define YEALINK_LDAP_NAME_ATTRIBUTES	"NameAttr"
#define YEALINK_LDAP_NUMBER_ATTRIBUTES	"NumbAttr"
#define YEALINK_LDAP_DISPLAY_NAME		"DisplayName"


#define YEALINK_MAX_SEARCH_SZ		50		//查询关键字的最大长度，//修改为31
#define YEALINK_MAX_HOST_NAME 		100		//主机名，有可能是域名
#define YEALINK_MAX_ATTRS_NUMBER 	30		//请求服务器返回的attributes的最大个数
#define YEALINK_MAX_USER_NAME		100		//用户名
#define YEALINK_MAX_PSWD_NAME		100		//密码
#define YEALINK_MAX_BASE_NAME		100		//查询的base
#define YEALINK_MAX_NAME_FILTER		100
#define YEALINK_MAX_NUMB_FILTER		100
#define YEALINK_MAX_ATTRIBUTES		100
#define YEALINK_MAX_DISPLAY_NAME	100
#define YEALINK_MAX_DISPLAY_NUMB	30
#define YEALINK_MAX_NUMB_PER_DN		20


#ifndef succLdap
#define succLdap 0x01
#endif

#ifndef failLdap
#define failLdap 0x00
#endif

#ifndef eofLdap
#define eofLdap 0xff
#endif




struct List{
    char			name[YEALINK_MAX_DISPLAY_NAME];
    char			numb[YEALINK_MAX_DISPLAY_NUMB];
    struct List		*Next;
};

typedef struct List Node;
typedef Node *Link;

/*
#define DBGPRINT(fmt, args...)		 \
{											\
		printf("%s(%d): "fmt ,  __FILE__, __LINE__ , ## args);				\
}

#define INFO(fmt, args...)	DBGPRINT(fmt, ## args)	
*/


/*功能：发送ldap bind request给服务器，请求建立连接, bind成功后可以发起ubind, search, result, 
		abandon请求,否则不能发起以上请求

*输入参数：	
						
*输出参数：void *yealinkld

*返回结果：fail: bind失败
			succ: bind成功
*/
int yealink_ldap_bind(void **yealinkld);

/*功能：发送unbind request给服务器，请求断开连接

*输入参数：	void *yealinkld			
						
*输出参数：

*返回结果：succ 
*/
int yealink_ldap_ubind(void **yealinkld);

/*功能：到服务器查询字符串szSearchKey开头的联系人,search成功后可以发起result读取结果

*输入参数：	void *yealinkld		
			char szSearchKey: 查询字符串
						
*输出参数：int *pmsgid: 消息ID, 用于区分哪个search

*返回结果：fail: 查询失败
			succ: 查询成功
*/
int yealink_ldap_search(void **yealinkld, int *pmsgid, char szSearchKey[YEALINK_MAX_SEARCH_SZ]);

/*功能：读取查询结构

*输入参数：	void *yealinkld	
			int msgid
			struct timeval *: 
				API超时时间，即超过这个时间，函数肯定返回，建议最小配置为10000us，因为函数读取结果需要时间
						
*输出参数：Link *hp: 查询到的结果链表

*返回结果：fail: 读取结果失败
			succ: 读取结果成功
*/
int yealink_ldap_result(void **yealinkld, int msgid, struct timeval *timeout, Link *hp);

/*功能：通知服务器取消查询，此接口在给服务器发送查询请求，服务器正在处理请求时，收到此消息有效

*输入参数：	void *yealinkld		
			int msgid
						
*输出参数：

*返回结果：succ: 
*/
int yealink_ldap_abandon(void **yealinkld, int msgid);

/*功能：释放查询到的结果链表

*输入参数：	Link result:查询到的结果链表		
						
*输出参数：

*返回结果：succ 
*/
int yealink_ldap_memfree(Link result);

#ifdef  __cplusplus
}
#endif

#endif

#endif /*LDAP_SEARCH_H_*/
