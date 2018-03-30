#include "font123diallync.h"
#include "inputmethod.h"

CFont_123_Dail_Lync::CFont_123_Dail_Lync(void)
{
}

CFont_123_Dail_Lync::~CFont_123_Dail_Lync(void)
{
}

void CFont_123_Dail_Lync::LoadIMEFile()
{
    SetName(IME_123_Dial_Lync);
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
    m_qstrArry10                = "*.:@";
    m_qstrArry11                = "#";
}

void CFont_123_Dail_Lync::InitData()
{
    //判断 是否有加载文件成功
    if (NULL != _IMEPROXY.pwText_123_Dial[5])
    {
        m_qstrArry1 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_Dial[0]);
        m_qstrArry2 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_Dial[1]);
        m_qstrArry3 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_Dial[2]);
        m_qstrArry4 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_Dial[3]);
        m_qstrArry5 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_Dial[4]);
        m_qstrArry6 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_Dial[5]);
        m_qstrArry7 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_Dial[6]);
        m_qstrArry8 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_Dial[7]);
        m_qstrArry9 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_Dial[8]);
        m_qstrArry0 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_Dial[9]);
        m_qstrArry10 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_Dial[10]);
        m_qstrArry11 = _IMEPROXY.WCHARStrToQString(_IMEPROXY.pwText_123_Dial[11]);
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
        m_qstrArry10                = "*.:@";
        m_qstrArry11                = "#";
    }
}

QString CFont_123_Dail_Lync::DealKeyPlus()
{
    return "+";
}

//根据输入Keycode获取输出
QString CFont_123_Dail_Lync::GetFontData(QObject* pObject, int iKeyCode, int iKeyboardStatus)
{
    if (Qt::Key_0 == iKeyCode)
    {
        qStartLongPressInput(pObject, iKeyCode, Qt::Key_Plus, KEY_LONG_PRESS_TIME);
    }

    return CFontPreeditBase::GetFontData(pObject, iKeyCode, iKeyboardStatus);
}


bool CFont_123_Dail_Lync::DealKeyRelease(QObject* pObject, int iKeyCode)
{
    if (Qt::Key_0 == iKeyCode)
    {
        qStopLongPressInput(iKeyCode);
    }
    return CFontPreeditBase::DealKeyRelease(pObject, iKeyCode);
}
