#include "fontnormalshift.h"

#define  FONTNORMAL_SHIFT_COUNT         95

CFontNormal_Shift::CFontNormal_Shift(void)
{
}

CFontNormal_Shift::~CFontNormal_Shift(void)
{
}

void CFontNormal_Shift::InitData()
{
    SetName(FONT_NORMAL_SHIFT);
    SetTitle(FONT_NORMAL_SHIFT);
}

QString CFontNormal_Shift::GetFirstChar(int iKeyCode)
{
    //删除字符
    if (Qt::Key_Backspace == iKeyCode)
    {
        return "";
    }

    if ((Qt::Key_Space <= iKeyCode) && (Qt::Key_AsciiTilde >= iKeyCode))
    {
        return GetFontABC(iKeyCode);
    }
    return "";
}

QString CFontNormal_Shift::GetFontData(QObject* pObject, int iKeyCode, int iKeyboardStatus)
{
    //删除字符
    if (Qt::Key_Backspace == iKeyCode)
    {
        return "";
    }

    if ((Qt::Key_Space <= iKeyCode) && (Qt::Key_AsciiTilde >= iKeyCode))
    {
        return GetFontABC(iKeyCode);
    }

    return "";

}

QString CFontNormal_Shift::GetFontABC(int iKeyCode)
{
    static QString s_sqstrData[] = {" ", "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?", "@", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "[", "\\", "]", "^", "_", "`", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "{", "|", "}", "~"};
    QString v_qstrData          = "";

    int iIndex = iKeyCode - Qt::Key_Space;
    if (iIndex < 0 || iIndex > FONTNORMAL_SHIFT_COUNT - 1)
    {
        return "";
    }
    v_qstrData                  = s_sqstrData[iIndex];

    return v_qstrData;
}
