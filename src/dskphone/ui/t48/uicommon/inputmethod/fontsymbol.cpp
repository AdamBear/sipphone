#include "fontsymbol.h"

CFontSymbol::CFontSymbol(void)
{
}

CFontSymbol::~CFontSymbol(void)
{
}

void CFontSymbol::InitData()
{
    SetName(QString(IME_symbol));
    //这边的title设置成"Symbol"
    SetTitle(QString(IME_TITLE_symbol));

    m_qstrArry0                 = "_";
    m_qstrArry1                 = ".";
    m_qstrArry2                 = "/";
    m_qstrArry3                 = "@";
    m_qstrArry4                 = ":";
    m_qstrArry5                 = ",";
    m_qstrArry6                 = "'";
    m_qstrArry7                 = ";";
    m_qstrArry8                 = "[";
    m_qstrArry9                 = "]";
    m_qstrArry10                = "*";
    m_qstrArry11                = "#";
}

void CFontSymbol::LoadIMEFile()
{
    //判断 是否有加载文件成功
    if (NULL != _IMEPROXY.pwText_symbol[5])
    {
        m_qstrArry1 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_symbol[0]);
        m_qstrArry2 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_symbol[1]);
        m_qstrArry3 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_symbol[2]);
        m_qstrArry4 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_symbol[3]);
        m_qstrArry5 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_symbol[4]);
        m_qstrArry6 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_symbol[5]);
        m_qstrArry7 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_symbol[6]);
        m_qstrArry8 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_symbol[7]);
        m_qstrArry9 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_symbol[8]);
        m_qstrArry0 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_symbol[9]);
        m_qstrArry10 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_symbol[10]);
        m_qstrArry11 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_symbol[11]);
    }
    else
    {
        m_qstrArry0                 = "_";
        m_qstrArry1                 = ".";
        m_qstrArry2                 = "/";
        m_qstrArry3                 = "@";
        m_qstrArry4                 = ":";
        m_qstrArry5                 = ",";
        m_qstrArry6                 = "'";
        m_qstrArry7                 = ";";
        m_qstrArry8                 = "[";
        m_qstrArry9                 = "]";
        m_qstrArry10                = "*";
        m_qstrArry11                = "#";
    }
}
