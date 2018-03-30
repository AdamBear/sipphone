#include "settinguierrorinfo.h"



CSettingUIErrorInfo::CSettingUIErrorInfo()
    : m_strEmptyError("")
    , m_strFormateError("")
    , m_strRangeError("")
{
}

CSettingUIErrorInfo::~CSettingUIErrorInfo()
{
}

CSettingUIErrorInfo & CSettingUIErrorInfo::operator = (const CSettingUIErrorInfo &
        objSoftkeyItemData)
{
//  if (this != &objSoftkeyItemData)
//  {
//      m_nIndex = objSoftkeyItemData.m_nIndex;
//      m_strIconPath = objSoftkeyItemData.m_strIconPath;
//      m_strDisplayName = objSoftkeyItemData.m_strDisplayName;
//      m_strAction = objSoftkeyItemData.m_strAction;
//  }

    return (*this);
}

bool CSettingUIErrorInfo::GetSoftkeyData(CSettingUIErrorInfo & objSoftkeyItemData)
{
    if (this != &objSoftkeyItemData)
    {
        objSoftkeyItemData = *this;
    }
    return true;
}

bool CSettingUIErrorInfo::SetSoftkeyData(const CSettingUIErrorInfo & objSoftkeyItemData)
{
    if (this != &objSoftkeyItemData)
    {
        *this = objSoftkeyItemData;
    }
    return true;
}

void CSettingUIErrorInfo::Reset()
{
    m_strEmptyError = "";
    m_strFormateError = "";
    m_strRangeError = "";
}

const yl::string & CSettingUIErrorInfo::GetEmptyError()
{
    return m_strEmptyError;
}

void CSettingUIErrorInfo::SetEmptyError(const yl::string & strEmptyError)
{
    m_strEmptyError = strEmptyError;
}

const yl::string & CSettingUIErrorInfo::GetFormateError()
{
    return m_strFormateError;
}

void CSettingUIErrorInfo::SetFormateError(const yl::string & strFormateError)
{
    m_strFormateError = strFormateError;
}

const yl::string & CSettingUIErrorInfo::GetRangeError()
{
    return m_strRangeError;
}

void CSettingUIErrorInfo::SetRangeError(const yl::string & strRangeError)
{
    m_strRangeError = strRangeError;
}


