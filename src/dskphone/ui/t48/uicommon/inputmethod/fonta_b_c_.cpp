#include "fonta_b_c_.h"

CFonta_b_c_::CFonta_b_c_(void)
{
}

CFonta_b_c_::~CFonta_b_c_(void)
{
}

void CFonta_b_c_::InitData()
{
    SetName(IME_ABC);
    SetTitle(IME_ABC);

    m_qstrArry0                 = " ";
    m_qstrArry1                 = "";
    m_qstrArry2                 = "ABC";
    m_qstrArry3                 = "DEF";
    m_qstrArry4                 = "GHI";
    m_qstrArry5                 = "JKL";
    m_qstrArry6                 = "MNO";
    m_qstrArry7                 = "PQRS";
    m_qstrArry8                 = "TUV";
    m_qstrArry9                 = "WXYZ";
    m_qstrArry10                = ".*:/@+-$[]";
    m_qstrArry11                = "#";
}

//加载文件数据
void CFonta_b_c_::LoadIMEFile()
{
    //判断 是否有加载文件成功
    if (NULL != _IMEPROXY.pwText_ABC[5])
    {
        m_qstrArry1 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_ABC[0]);
        m_qstrArry2 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_ABC[1]);
        m_qstrArry3 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_ABC[2]);
        m_qstrArry4 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_ABC[3]);
        m_qstrArry5 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_ABC[4]);
        m_qstrArry6 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_ABC[5]);
        m_qstrArry7 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_ABC[6]);
        m_qstrArry8 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_ABC[7]);
        m_qstrArry9 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_ABC[8]);
        m_qstrArry0 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_ABC[9]);
        m_qstrArry10 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_ABC[10]);
        m_qstrArry11 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_ABC[11]);
    }
    else
    {
        m_qstrArry0                 = " ";
        m_qstrArry1                 = "";
        m_qstrArry2                 = "ABC";
        m_qstrArry3                 = "DEF";
        m_qstrArry4                 = "GHI";
        m_qstrArry5                 = "JKL";
        m_qstrArry6                 = "MNO";
        m_qstrArry7                 = "PQRS";
        m_qstrArry8                 = "TUV";
        m_qstrArry9                 = "WXYZ";
        m_qstrArry10                = ".*:/@+-$[]";
        m_qstrArry11                = "#";
    }
}

