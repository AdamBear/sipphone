#ifndef LDAP_MODLDAP_H_
#define LDAP_MODLDAP_H_

// 此为LDAP模块的接口文件
#include "ldapdatadefine.h"

// LDAP搜索初始化.
// 如果初始化成功则返回true, 否则返回false.
void LDAP_Init();

//请求搜索
bool LDAP_ReqSearch(const yl::string & strKey);

//获取联系人列表数据
bool LDAP_GetContactList(LdapListData * pData, const yl::string & strKey);

//获取匹配信息
bool LDAP_GetContactInfoByKey(const yl::string & strKey, LdapContactData & stData);

// 获取LDAP配置名.
// 如果获取成功则返回配置的LDAP名, 否则返回空.
yl::string LDAP_GetName();

bool LDAP_IsEnable();

#if IF_FEATURE_34162
//重新设置LDAP配置
void LDAP_ReSetLdapOption();

/*******************************************************************
** 函数描述:    账号的注册方式
** 参数:      [in] 错误Code
** 返回:      [out] true 鉴权错误，false 不是鉴权错误
********************************************************************/
bool LDAP_IsAuthError(int nErrorCode);

/*******************************************************************
** 函数描述:    账号的注册方式
** 参数:      [in] nErrorCode : 错误Code
** 返回:      [out] strErrorTip : 错误信息提示，通过接口向LDAP库获取
********************************************************************/
bool LDAP_GetErrorStringByCode(int nErrorCode, yl::string & strErrorTip);

#endif //#if IF_FEATURE_34162

//获取LDAP联系人超级搜索列表数据
bool LDAP_GetContactSuperSearchList(YLVector<SuperDirData> & output, const yl::string & strKey);

#endif  // LDAP_MODLDAP_H_
