#include "font2ab_.h"

CFont2ab_::CFont2ab_(void)
{
}

CFont2ab_::~CFont2ab_(void)
{
}

void CFont2ab_::InitData()
{
    SetName(IME_2aB);
    SetTitle(IME_2aB);

    m_qstrArry0                 = "0";
    m_qstrArry1                 = "1";
    m_qstrArry2                 = "2abcABC";
    m_qstrArry3                 = "3defDEF";
    m_qstrArry4                 = "4ghiGHI";
    m_qstrArry5                 = "5jklJKL";
    m_qstrArry6                 = "6mnoMNO";
    m_qstrArry7                 = "7pqrsPQRS";
    m_qstrArry8                 = "8tuvTUV";
    m_qstrArry9                 = "9wxyzWXYZ";
    m_qstrArry10                = ".*:/@+-$[]";
    m_qstrArry11                = "#";
}

//加载文件数据
void CFont2ab_::LoadIMEFile()
{
    //判断 是否有加载文件成功
    if (NULL != _IMEPROXY.pwText_2aB[5])
    {
        m_qstrArry1 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_2aB[0]);
        m_qstrArry2 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_2aB[1]);
        m_qstrArry3 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_2aB[2]);
        m_qstrArry4 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_2aB[3]);
        m_qstrArry5 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_2aB[4]);
        m_qstrArry6 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_2aB[5]);
        m_qstrArry7 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_2aB[6]);
        m_qstrArry8 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_2aB[7]);
        m_qstrArry9 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_2aB[8]);
        m_qstrArry0 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_2aB[9]);
        m_qstrArry10 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_2aB[10]);
        m_qstrArry11 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_2aB[11]);
    }
    else
    {
        m_qstrArry0                 = "0";
        m_qstrArry1                 = "1";
        m_qstrArry2                 = "2abcABC";
        m_qstrArry3                 = "3defDEF";
        m_qstrArry4                 = "4ghiGHI";
        m_qstrArry5                 = "5jklJKL";
        m_qstrArry6                 = "6mnoMNO";
        m_qstrArry7                 = "7pqrsPQRS";
        m_qstrArry8                 = "8tuvTUV";
        m_qstrArry9                 = "9wxyzWXYZ";
        m_qstrArry10                = ".*:/@+-$[]";
        m_qstrArry11                = "#";
    }
}
