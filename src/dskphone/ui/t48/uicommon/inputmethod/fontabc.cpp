#include "fontabc.h"

CFontabc::CFontabc(void)
{

}

CFontabc::~CFontabc(void)
{
}

void CFontabc::InitData()
{
    SetName(IME_abc);
    SetTitle(IME_abc);

    m_qstrArry0                 = " ";
    m_qstrArry1                 = "";
    m_qstrArry2                 = "abc";
    m_qstrArry3                 = "def";
    m_qstrArry4                 = "ghi";
    m_qstrArry5                 = "jkl";
    m_qstrArry6                 = "mno";
    m_qstrArry7                 = "pqrs";
    m_qstrArry8                 = "tuv";
    m_qstrArry9                 = "wxyz";
    m_qstrArry10                = ".*:/@+-$[]";
    m_qstrArry11                = "#";
}

//加载文件数据
void CFontabc::LoadIMEFile()
{
    //判断 是否有加载文件成功
    if (NULL != _IMEPROXY.pwText_abc[5])
    {
        m_qstrArry1 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_abc[0]);
        m_qstrArry2 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_abc[1]);
        m_qstrArry3 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_abc[2]);
        m_qstrArry4 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_abc[3]);
        m_qstrArry5 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_abc[4]);
        m_qstrArry6 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_abc[5]);
        m_qstrArry7 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_abc[6]);
        m_qstrArry8 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_abc[7]);
        m_qstrArry9 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_abc[8]);
        m_qstrArry0 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_abc[9]);
        m_qstrArry10 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_abc[10]);
        m_qstrArry11 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_abc[11]);
    }
    else
    {
        m_qstrArry0                 = " ";
        m_qstrArry1                 = "";
        m_qstrArry2                 = "abc";
        m_qstrArry3                 = "def";
        m_qstrArry4                 = "ghi";
        m_qstrArry5                 = "jkl";
        m_qstrArry6                 = "mno";
        m_qstrArry7                 = "pqrs";
        m_qstrArry8                 = "tuv";
        m_qstrArry9                 = "wxyz";
        m_qstrArry10                = ".*:/@+-$[]";
        m_qstrArry11                = "#";
    }
}

