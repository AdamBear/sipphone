#include "font123ip.h"

CFont_123_IP::CFont_123_IP(void)
{
}

CFont_123_IP::~CFont_123_IP(void)
{
}

void CFont_123_IP::InitData()
{
    SetName(IME_123_IP);
    //这边的title设置成"123"
    SetTitle(IME_TITLE_123);

    m_qstrArry0                 = "0";
    m_qstrArry1                 = "1";
    m_qstrArry2                 = "2";
    m_qstrArry3                 = "3";
    m_qstrArry4                 = "4";
    m_qstrArry5                 = "5";
    m_qstrArry6                 = "6";
    m_qstrArry7                 = "7";
    m_qstrArry8                 = "8";
    m_qstrArry9                 = "9";
    m_qstrArry10                = ".*:/@[]";
    m_qstrArry11                = "#";
}

//加载文件数据
void CFont_123_IP::LoadIMEFile()
{
    //判断 是否有加载文件成功
    if (NULL != _IMEPROXY.pwText_123_IP[5])
    {
        m_qstrArry1 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_IP[0]);
        m_qstrArry2 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_IP[1]);
        m_qstrArry3 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_IP[2]);
        m_qstrArry4 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_IP[3]);
        m_qstrArry5 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_IP[4]);
        m_qstrArry6 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_IP[5]);
        m_qstrArry7 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_IP[6]);
        m_qstrArry8 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_IP[7]);
        m_qstrArry9 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_IP[8]);
        m_qstrArry0 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_IP[9]);
        m_qstrArry10 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_IP[10]);
        m_qstrArry11 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_IP[11]);
    }
    else
    {
        m_qstrArry0                 = "0";
        m_qstrArry1                 = "1";
        m_qstrArry2                 = "2";
        m_qstrArry3                 = "3";
        m_qstrArry4                 = "4";
        m_qstrArry5                 = "5";
        m_qstrArry6                 = "6";
        m_qstrArry7                 = "7";
        m_qstrArry8                 = "8";
        m_qstrArry9                 = "9";
        m_qstrArry10                = ".*:/@[]";
        m_qstrArry11                = "#";
    }
}
