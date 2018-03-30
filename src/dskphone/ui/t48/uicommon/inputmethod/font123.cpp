#include "font123.h"

CFont123::CFont123(void)
{
}

CFont123::~CFont123(void)
{
}

void CFont123::InitData()
{
    SetName(QString(IME_123));
    //这边的title设置成"123"
    SetTitle(QString(IME_TITLE_123));

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
    m_qstrArry10                = "*";
    m_qstrArry11                = "#";
}


//加载文件数据
void CFont123::LoadIMEFile()
{
    //判断 是否有加载文件成功
    if (NULL != _IMEPROXY.pwText_123[5])
    {
        m_qstrArry1 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123[0]);
        m_qstrArry2 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123[1]);
        m_qstrArry3 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123[2]);
        m_qstrArry4 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123[3]);
        m_qstrArry5 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123[4]);
        m_qstrArry6 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123[5]);
        m_qstrArry7 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123[6]);
        m_qstrArry8 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123[7]);
        m_qstrArry9 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123[8]);
        m_qstrArry0 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123[9]);
        m_qstrArry10 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123[10]);
        m_qstrArry11 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123[11]);
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
        m_qstrArry10                = "*";
        m_qstrArry11                = "#";
    }
}
