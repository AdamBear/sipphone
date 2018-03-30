/******************************************************************************************************
Copyright (C) 2008 ,Yealink Network Technology Co.,Ltd. 

File Name:ldapsearch.h
Author:wxf
Version: 1.0
Date: 2009.06.09

History:

Abstract:

�ӿڵ���:
	1������bind;
	2������ubind, search, result,abandon, memfree��
	
	ע��:bind�ɹ�����ܷ�����������
		search�ɹ�����ܷ���result,abandon����
		result�ɹ������һ��Ҫmemfree�ͷ��ڴ�

	
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


#define YEALINK_MAX_SEARCH_SZ		50		//��ѯ�ؼ��ֵ���󳤶ȣ�//�޸�Ϊ31
#define YEALINK_MAX_HOST_NAME 		100		//���������п���������
#define YEALINK_MAX_ATTRS_NUMBER 	30		//������������ص�attributes��������
#define YEALINK_MAX_USER_NAME		100		//�û���
#define YEALINK_MAX_PSWD_NAME		100		//����
#define YEALINK_MAX_BASE_NAME		100		//��ѯ��base
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


/*���ܣ�����ldap bind request��������������������, bind�ɹ�����Է���ubind, search, result, 
		abandon����,�����ܷ�����������

*���������	
						
*���������void *yealinkld

*���ؽ����fail: bindʧ��
			succ: bind�ɹ�
*/
int yealink_ldap_bind(void **yealinkld);

/*���ܣ�����unbind request��������������Ͽ�����

*���������	void *yealinkld			
						
*���������

*���ؽ����succ 
*/
int yealink_ldap_ubind(void **yealinkld);

/*���ܣ�����������ѯ�ַ���szSearchKey��ͷ����ϵ��,search�ɹ�����Է���result��ȡ���

*���������	void *yealinkld		
			char szSearchKey: ��ѯ�ַ���
						
*���������int *pmsgid: ��ϢID, ���������ĸ�search

*���ؽ����fail: ��ѯʧ��
			succ: ��ѯ�ɹ�
*/
int yealink_ldap_search(void **yealinkld, int *pmsgid, char szSearchKey[YEALINK_MAX_SEARCH_SZ]);

/*���ܣ���ȡ��ѯ�ṹ

*���������	void *yealinkld	
			int msgid
			struct timeval *: 
				API��ʱʱ�䣬���������ʱ�䣬�����϶����أ�������С����Ϊ10000us����Ϊ������ȡ�����Ҫʱ��
						
*���������Link *hp: ��ѯ���Ľ������

*���ؽ����fail: ��ȡ���ʧ��
			succ: ��ȡ����ɹ�
*/
int yealink_ldap_result(void **yealinkld, int msgid, struct timeval *timeout, Link *hp);

/*���ܣ�֪ͨ������ȡ����ѯ���˽ӿ��ڸ����������Ͳ�ѯ���󣬷��������ڴ�������ʱ���յ�����Ϣ��Ч

*���������	void *yealinkld		
			int msgid
						
*���������

*���ؽ����succ: 
*/
int yealink_ldap_abandon(void **yealinkld, int msgid);

/*���ܣ��ͷŲ�ѯ���Ľ������

*���������	Link result:��ѯ���Ľ������		
						
*���������

*���ؽ����succ 
*/
int yealink_ldap_memfree(Link result);

#ifdef  __cplusplus
}
#endif

#endif

#endif /*LDAP_SEARCH_H_*/
