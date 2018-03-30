#ifndef _SETTINGUI_ACCOUNT_LOGIN_DATA_INFO_H_
#define _SETTINGUI_ACCOUNT_LOGIN_DATA_INFO_H_

#include "settinguilogic/include/common.h"
#include "account/include/account_datadefine.h"

class CCloudAccountInfo
{
public:
    CCloudAccountInfo();
    ~CCloudAccountInfo();

public:
    void SetCloudAccountInfo(const CloudAccountInfo & info);
    const CloudAccountInfo & GetCloudAccountInfo();

    void SetCloudPincode(const yl::string strPincode);
    yl::string GetCloudPinCode();

    void CleanAccountInfo();

    bool IsHasAccountInfo()
    {
        return m_bHasInfo;
    }

protected:
    CloudAccountInfo m_stCloudInfo;
    bool m_bHasInfo;
};

class CCommonAccountInfo
{
public:
    CCommonAccountInfo();
    ~CCommonAccountInfo();
public:
    void SetCommonAccountInfo(const AccountDetailData & info);
    const AccountDetailData & GetCommonAccountInfo();
    yl::string GetProxyFallbackInterval();
    void SetProxyFallbackInterval(const yl::string & strProxyFallbackInterval);
protected:
    AccountDetailData m_stAccountInfo;
    yl::string m_strProxyFallbackInterval;
};

#endif //_SETTINGUI_ACCOUNT_LOGIN_DATA_INFO_H_
