#include "fontcustom.h"
#include "keyboardimemanager.h"

CFontCustom::CFontCustom()
{

}

CFontCustom::~CFontCustom()
{

}

void CFontCustom::InitData()
{
    SetName(FONT_CUSTOM_KEYBOARD);
    SetTitle(FONT_CUSTOM_KEYBOARD);
}

QString CFontCustom::GetFirstChar(int iKeyCode)
{
    //删除字符
    if (Qt::Key_Backspace == iKeyCode)
    {
        return "";
    }

    return GetFontFromIme(iKeyCode);
}

QString CFontCustom::GetFontData(QObject* pObject, int iKeyCode, int iKeyboardStatus)
{
    //删除字符
    if (Qt::Key_Backspace == iKeyCode)
    {
        return "";
    }

    return GetFontFromIme(iKeyCode);
}

QString CFontCustom::GetFontFromIme(int iKeyCode)
{
    if (Qt::Key_Space == iKeyCode)
    {
        return " ";
    }

    if (PHONE_KEY_INPUTMETHOD_CUSTOM_KEYBOARD == iKeyCode)
    {
        return g_KeyBoardImeManager.GetSelectedChar();
    }

    static QString s_sqstrCustomData[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "#"};
    QString v_qstrData   = "";

    int iIndex = iKeyCode - Qt::Key_0;

    if (iIndex >= 0 && iIndex <= 9)
    {
        v_qstrData = s_sqstrCustomData[iIndex];
    }
    else if (iKeyCode == Qt::Key_Asterisk)
    {
        v_qstrData = s_sqstrCustomData[10];
    }
    else if (iKeyCode == Qt::Key_NumberSign)
    {
        v_qstrData = s_sqstrCustomData[11];
    }

    return v_qstrData;
}
