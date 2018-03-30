#include "fontnormal.h"

#define  FONTNORMAL_COUNT           95

CFontNormal::CFontNormal(void)
{
}

CFontNormal::~CFontNormal(void)
{
}

void CFontNormal::InitData()
{
    SetName(FONT_NORMALS);
    SetTitle(FONT_NORMALS);
}

QString CFontNormal::GetFirstChar(int iKeyCode)
{
    //删除字符
    if (Qt::Key_Backspace == iKeyCode)
    {
        return "";
    }

    if ((Qt::Key_Space <= iKeyCode) && (Qt::Key_AsciiTilde >= iKeyCode))
    {
        return GetFontabc(iKeyCode);
    }
    return "";
}

QString CFontNormal::GetFontData(QObject* pObject, int iKeyCode, int iKeyboardStatus)
{
    //删除字符
    if (Qt::Key_Backspace == iKeyCode)
    {
        return "";
    }

    if ((Qt::Key_Space <= iKeyCode) && (Qt::Key_AsciiTilde >= iKeyCode))
    {
        return GetFontabc(iKeyCode);
    }
    return "";
}



QString CFontNormal::GetFontabc(int iKeyCode)
{
    static QString s_sqstrData[] = {" ", "!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", ".", "/", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ";", "<", "=", ">", "?", "@", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "[", "\\", "]", "^", "_", "`", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "{", "|", "}", "~"};
    QString v_qstrData          = "";
    int iIndex = iKeyCode - Qt::Key_Space;
    if (iIndex < 0 || iIndex > FONTNORMAL_COUNT - 1)
    {
        return "";
    }
    v_qstrData                  = s_sqstrData[iIndex];

    return v_qstrData;

}
