#include "settinguiaccountlogindatainfo.h"

CCloudAccountInfo::CCloudAccountInfo()
    : m_bHasInfo(false)
{

}

CCloudAccountInfo::~CCloudAccountInfo()
{

}

void CCloudAccountInfo::SetCloudAccountInfo(const CloudAccountInfo & info)
{
    m_stCloudInfo.m_strUserName = info.m_strUserName;
    m_stCloudInfo.m_strPassWord = info.m_strPassWord;
    m_stCloudInfo.m_bSavePass = info.m_bSavePass;
    m_bHasInfo = true;
}

const CloudAccountInfo & CCloudAccountInfo::GetCloudAccountInfo()
{
    return m_stCloudInfo;
}

void CCloudAccountInfo::SetCloudPincode(const yl::string strPincode)
{
    m_stCloudInfo.m_strPinCode = strPincode;
}

yl::string CCloudAccountInfo::GetCloudPinCode()
{
    return m_stCloudInfo.m_strPinCode;
}

void CCloudAccountInfo::CleanAccountInfo()
{
    m_stCloudInfo.Clear();
    m_bHasInfo = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
CCommonAccountInfo::CCommonAccountInfo()
    : m_strProxyFallbackInterval("")
{

}

CCommonAccountInfo::~CCommonAccountInfo()
{

}

void CCommonAccountInfo::SetCommonAccountInfo(const AccountDetailData & info)
{
    m_stAccountInfo.m_isActivated = info.m_isActivated;
    m_stAccountInfo.m_strLabel = info.m_strLabel;
    m_stAccountInfo.m_strDisplayName = info.m_strDisplayName;
    m_stAccountInfo.m_strRegisterName = info.m_strRegisterName;
    m_stAccountInfo.m_strUserName = info.m_strUserName;
    m_stAccountInfo.m_strPassword = info.m_strPassword;
    m_stAccountInfo.m_strSipServer1 = info.m_strSipServer1;
    m_stAccountInfo.m_strSipServer2 = info.m_strSipServer2;
    m_stAccountInfo.m_isOutboundEnable = info.m_isOutboundEnable;
    m_stAccountInfo.m_strOutboundProxy1 = info.m_strOutboundProxy1;
    m_stAccountInfo.m_strOutboundProxy2 = info.m_strOutboundProxy2;
    m_stAccountInfo.m_nProxyFallbackInterval = info.m_nProxyFallbackInterval;
    m_stAccountInfo.m_isStunEnable = info.m_isStunEnable;
}

const AccountDetailData & CCommonAccountInfo::GetCommonAccountInfo()
{
    return m_stAccountInfo;
}

yl::string CCommonAccountInfo::GetProxyFallbackInterval()
{
    return m_strProxyFallbackInterval;
}

void CCommonAccountInfo::SetProxyFallbackInterval(const yl::string & strProxyFallbackInterval)
{
    m_strProxyFallbackInterval = strProxyFallbackInterval;
}
