#ifndef __SECURITY_CLASSIFICATION_H__
#define __SECURITY_CLASSIFICATION_H__

#include <ylstring.h>
#include "singletonclassdefine.h"

// 安全等级
struct SecurityLevel
{
    // 账号当前的级别
    yl::string strAccountLevel;

    // 账号覆盖的级别
    yl::string strOverrideLevel;

    // 账号允许设置的级别列表
    YLList<yl::string> lstAllowLevels;

    void Clear()
    {
        strAccountLevel = "";
        strOverrideLevel = "";
        lstAllowLevels.clear();
    }
};


// 通话与Element一一对应
class CSecurityClassification
{
    DECLARE_SINGLETON_CLASS(CSecurityClassification)

public:
    // 初始化
    void Init();

    // 该功能是否可用
    bool IsShowSecurity(int iAccount);

    // 获取账号的安全等级
    bool GetAccountSecurityLevel(yl::string& strCurLevel, int iAccount);

    // 获取账号的覆盖安全等级：通话中设置的等级
    bool GetAccountOverrideLevel(yl::string& strCurLevel, int iAccount);

    // 获取账号的可设置列表
    bool GetAllowSecurityLevels(YLList<yl::string>& lstAllowLevels, int iAccount);

    // 通话中修改账号的安全等级
    bool ModifyAccountOverrideLevel(int iAccount, const yl::string& strLevel);

    // 获取话路对应的安全等级
    yl::string GetTalkSecurityLevel(int iSessId);

    // 获取本地会议中安全等级最低的相关信息
    int GetLeastLevelInfoFromLocalConf(int iSessId, yl::string& strLevel);

    // 添加话路对应的安全等级
    void AddTalkSecurityLevel(int iSessId, const yl::string& strLevel);

    // 删除话路对应的安全等级
    void DeleteTalkSecurityLevel(int iSessId);

    // 解析XML
    bool ParseXML(int iAccount, LPCSTR pContent);

    // 某个安全等级是否在可允许设置的列表中
    bool IsInAllowSecurityLevels(const yl::string& strLevel, YLList<yl::string>& lstAllowLevels);

    // 组装修改的XML
    yl::string FormatActionXml(const yl::string& strLevel);

    // 解析话路对应的安全等级
    yl::string ParseTalkSecurityLevel(const yl::string& strContent);

    // 安全等级的级别数
    int TranslateSecurityLevel(const yl::string& strLevel);

    // 账号状态变更
    bool OnAccountStatusChange(int iAccountID);

private:
    // 一个账号对应于一个安全等级
    YLVector<SecurityLevel> m_vecSecurityLev;

    // 话路对应的安全等级
    YLHashMap<int, yl::string> m_mapTalkLev;
};

// 获取全局唯一管理对象
#define _SecurityClassification GET_SINGLETON_INSTANCE(CSecurityClassification)

#endif // __SECURITY_CLASSIFICATION_H__
