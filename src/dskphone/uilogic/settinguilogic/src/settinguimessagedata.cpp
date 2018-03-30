#include "settinguimessagedata.h"


CSettingUIMessageData::CSettingUIMessageData()
    : m_iMsgId(-1)
    , m_lParam(-1)
    , m_wParam(-1)
    , m_iPriority(-1)
    , m_eMethod(MESSAGE_METHOD_INVALID)
    , m_strDstThread("")
    , m_strVarDataID("")
    , m_strLparamID("")
    , m_strWparamID("")
{
}

CSettingUIMessageData::CSettingUIMessageData(const CSettingUIMessageData & objMsgData)
    : m_iMsgId(objMsgData.m_iMsgId)
    , m_lParam(objMsgData.m_lParam)
    , m_wParam(objMsgData.m_wParam)
    , m_iPriority(objMsgData.m_iPriority)
    , m_eMethod(objMsgData.m_eMethod)
    , m_strDstThread(objMsgData.m_strDstThread)
    , m_strVarDataID(objMsgData.m_strVarDataID)
    , m_strLparamID(objMsgData.m_strLparamID)
    , m_strWparamID(objMsgData.m_strWparamID)
{
}

CSettingUIMessageData::~CSettingUIMessageData()
{
}

// CSettingUIMessageData& CSettingUIMessageData::operator = (const CSettingUIMessageData& objMsgData)
// {
//  if (this != &objMsgData)
//  {
//      m_iPriority = objMsgData.m_iPriority;
//      m_iMsgId = objMsgData.m_iMsgId;
//      m_lParam = objMsgData.m_lParam;
//      m_wParam = objMsgData.m_wParam;
//      m_eMethod = objMsgData.m_eMethod;
//      m_strWparamID = objMsgData.m_strWparamID;
//      m_strVarDataID = objMsgData.m_strVarDataID;
//      m_strLparamID = objMsgData.m_strLparamID;
//      m_strDstThread = objMsgData.m_strDstThread;
//  }
//
//  return (*this);
// }

bool CSettingUIMessageData::GetMessageData(CSettingUIMessageData & objMsgData)
{
    if (this != &objMsgData)
    {
        objMsgData = *this;
    }
    return true;
}

bool CSettingUIMessageData::SetMessageData(const CSettingUIMessageData & objMsgData)
{
    if (this != &objMsgData)
    {
        *this = objMsgData;
    }
    return true;
}

void CSettingUIMessageData::Reset()
{
    m_iMsgId = -1;
    m_lParam = -1;
    m_wParam = -1;
    m_iPriority = -1;
    m_eMethod = MESSAGE_METHOD_INVALID;
    m_strDstThread = "";
    m_strVarDataID = "";
    m_strLparamID = "";
    m_strWparamID = "";
}

int CSettingUIMessageData::GetMsgID() const
{
    return m_iMsgId;
}

void CSettingUIMessageData::SetMsgID(int iMsgId)
{
    m_iMsgId = iMsgId;
}

int CSettingUIMessageData::GetlParam() const
{
    return m_lParam;
}

void CSettingUIMessageData::SetlParam(int ilParam)
{
    m_lParam = ilParam;
}

int CSettingUIMessageData::GetwParam() const
{
    return m_wParam;
}

void CSettingUIMessageData::SetwParam(int iwParam)
{
    m_wParam = iwParam;
}

int CSettingUIMessageData::GetPriority() const
{
    return m_iPriority;
}

void CSettingUIMessageData::SetPriority(int iPriority)
{
    m_iPriority = iPriority;
}

int CSettingUIMessageData::GetSendMethod() const
{
    return m_eMethod;
}

void CSettingUIMessageData::SetSendMethod(int iSendMethod)
{
    if (MESSAGE_METHOD_SEND <= iSendMethod && iSendMethod <= MESSAGE_METHOD_NOTIFYAPPEX)
    {
        m_eMethod = (MESSAGE_METHOD)iSendMethod;
    }
}

const yl::string & CSettingUIMessageData::GetLparamID() const
{
    return m_strLparamID;
}

void CSettingUIMessageData::SetLparamID(const yl::string & strLparamID)
{
    m_strLparamID = strLparamID;
}

const yl::string & CSettingUIMessageData::GetWparamID() const
{
    return m_strWparamID;
}

void CSettingUIMessageData::SetWparamID(const yl::string & strWparamID)
{
    m_strWparamID = strWparamID;
}

const yl::string & CSettingUIMessageData::GetDstThread()
{
    return m_strDstThread;
}

void CSettingUIMessageData::SetDstThread(const yl::string & strDstThread)
{
    m_strDstThread = strDstThread;
}

const yl::string & CSettingUIMessageData::GetVarDataID()
{
    return m_strVarDataID;
}

void CSettingUIMessageData::SetVarDataID(const yl::string & strVarDataID)
{
    m_strVarDataID = strVarDataID;
}
