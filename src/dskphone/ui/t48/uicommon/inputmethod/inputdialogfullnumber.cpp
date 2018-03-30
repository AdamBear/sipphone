#include <qnamespace.h>
#include "inputdialogfullnumber.h"

CInputDialogFullNumber::CInputDialogFullNumber(void)
{
    //回车按键
    m_pInputButtonEnter             = NULL;
}

CInputDialogFullNumber::~CInputDialogFullNumber(void)
{
}

INPUT_DLGTYPE CInputDialogFullNumber::GetDlgType()
{
    return INPUT_DLGTYPE_NUM;
}

void CInputDialogFullNumber::LoadBkGroundPic()
{
    m_qCurrPixMap = THEME_GET_BMP(fromQString(g_qstrDialogFullNumberBackGoundWithEnterPic).c_str());
}

void CInputDialogFullNumber::SetInputDlgStyle(INPUT_DIALOG_STYLE enStyle)
{
    m_qCurrPixMap.detach();
    if (INPUT_DIALOG_NEWLINE == enStyle)
    {
    }
    else if (INPUT_DIALOG_DIAL == enStyle)
    {
        m_qCurrPixMap = THEME_GET_BMP(fromQString(g_qstrDialogFullNumberBackGoundPic).c_str());
        m_pInputButtonEnter->SetPressIcon(g_qstrFullKeyBoardNumSend);
    }
    else if (INPUT_DIALOG_SEARCH == enStyle)
    {
    }
    else
    {
        m_qCurrPixMap = THEME_GET_BMP(fromQString(g_qstrDialogFullNumberBackGoundWithEnterPic).c_str());
        m_pInputButtonEnter->SetPressIcon(g_qstrFullKeyBoardNumEnter);
    }
    CBaseInputDlg::SetInputDlgStyle(enStyle);
}

void CInputDialogFullNumber::NewButton()
{
    QRect v_Rect;
    v_Rect.setRect(13, 7, 59, 56);

    //~
    CreateButton(v_Rect, Qt::Key_AsciiTilde, "~", "~", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardAsciiTilde);

    //!
    v_Rect.setRect(13 + (59 + 6), 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_Exclam, "!", "!", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardExclam);

    //@
    v_Rect.setRect(13 + (59 + 6) * 2, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_At, "@", "@", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardAt);

    //#
    v_Rect.setRect(13 + (59 + 6) * 3, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_NumberSign, "#", "#", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardNumberSign);


    //%
    v_Rect.setRect(13 + (59 + 6) * 4, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_Percent, "%", "%", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardPercent);

    //^
    v_Rect.setRect(13 + (59 + 6) * 5, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_AsciiCircum, "^", "^", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardAsciiCircum);

    //&
    v_Rect.setRect(13 + (59 + 6) * 6, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_Ampersand, "&", "&", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardAmpersand);

    //*
    v_Rect.setRect(13, 7 + (56 + 6), 59, 56);
    CreateButton(v_Rect, Qt::Key_Asterisk, "*", "*", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardAsterisk);

    //-
    v_Rect.setRect(13 + (59 + 6), 7 + (56 + 6), 59, 56);
    CreateButton(v_Rect, Qt::Key_Minus, "-", "-", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardMinus);

    //_
    v_Rect.setRect(13 + (59 + 6) * 2, 7 + (56 + 6), 59, 56);
    CreateButton(v_Rect, Qt::Key_Underscore, "_", "_", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardUnderscore);

    //=
    v_Rect.setRect(13 + (59 + 6) * 3, 7 + (56 + 6), 59, 56);
    CreateButton(v_Rect, Qt::Key_Equal, "=", "=", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEqual);

    //+
    v_Rect.setRect(13 + (59 + 6) * 4, 7 + (56 + 6), 59, 56);
    CreateButton(v_Rect, Qt::Key_Plus, "+", "+", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardPlus);

    //(
    v_Rect.setRect(13 + (59 + 6) * 5, 7 + (56 + 6), 59, 56);
    CreateButton(v_Rect, Qt::Key_ParenLeft, "(", "(", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardParenLeft);

    //)
    v_Rect.setRect(13 + (59 + 6) * 6, 7 + (56 + 6), 59, 56);
    CreateButton(v_Rect, Qt::Key_ParenRight, ")", ")", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardParenRight);

    //<
    v_Rect.setRect(13, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_Less, "<", "<", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardLess);

    //>
    v_Rect.setRect(13 + (59 + 6), 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_Greater, ">", ">", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardGreater);

    //[
    v_Rect.setRect(13 + (59 + 6) * 2, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_BracketLeft, "[", "[", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardBracketLeft);

    //]
    v_Rect.setRect(13 + (59 + 6) * 3, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_BracketRight, "]", "]", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardBracketRight);

    //{
    v_Rect.setRect(13 + (59 + 6) * 4, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_BraceLeft, "{", "{", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardBraceLeft);

    //}
    v_Rect.setRect(13 + (59 + 6) * 5, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_BraceRight, "}", "}", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardBraceRight);

    //|
    v_Rect.setRect(13 + (59 + 6) * 6, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_Bar, "|", "|", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardBar);

    //;
    v_Rect.setRect(13 + (59 + 6) * 2, 7 + (56 + 6) * 3, 59, 56);
    CreateButton(v_Rect, Qt::Key_Semicolon, ";", ";", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardSemicolon);

    //:
    v_Rect.setRect(13 + (59 + 6) * 3, 7 + (56 + 6) * 3, 59, 56);
    CreateButton(v_Rect, Qt::Key_Colon, ":", ":", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardColon);

    //"
    v_Rect.setRect(13 + (59 + 6) * 4, 7 + (56 + 6) * 3, 59, 56);
    CreateButton(v_Rect, Qt::Key_QuoteDbl, "\"", "\"", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardQuoteDbl);

    //
    v_Rect.setRect(13 + (59 + 6) * 5, 7 + (56 + 6) * 3, 59, 56);
    CreateButton(v_Rect, Qt::Key_Backslash, "\\", "\\", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardBackslash);

    //
    v_Rect.setRect(13 + (59 + 6) * 6, 7 + (56 + 6) * 3, 59, 56);
    CreateButton(v_Rect, Qt::Key_Slash, "/", "/", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardSlash);


    //1
    v_Rect.setRect(501, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_1, "1", "1", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoard1);

    //2
    v_Rect.setRect(501 + (59 + 6), 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_2, "2", "2", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoard2);

    //3
    v_Rect.setRect(501 + (59 + 6) * 2, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_3, "3", "3", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoard3);

    //4
    v_Rect.setRect(501, 7 + (56 + 6), 59, 56);
    CreateButton(v_Rect, Qt::Key_4, "4", "4", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoard4);

    //5
    v_Rect.setRect(501 + (59 + 6), 7 + (56 + 6), 59, 56);
    CreateButton(v_Rect, Qt::Key_5, "5", "5", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoard5);

    //6
    v_Rect.setRect(501 + (59 + 6) * 2, 7 + (56 + 6), 59, 56);
    CreateButton(v_Rect, Qt::Key_6, "6", "6", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoard6);

    //7
    v_Rect.setRect(501, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_7, "7", "7", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoard7);

    //8
    v_Rect.setRect(501 + (59 + 6), 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_8, "8", "8", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoard8);

    //9
    v_Rect.setRect(501 + (59 + 6) * 2, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_9, "9", "9", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoard9);


    //0
    v_Rect.setRect(501, 7 + (56 + 6) * 3, 124, 56);
    CreateButton(v_Rect, Qt::Key_0, "0", "0", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoard0);

    //.
    v_Rect.setRect(501 + (59 + 6) * 2, 7 + (56 + 6) * 3, 59, 56);
    CreateButton(v_Rect, Qt::Key_Period, ".", ".", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardPeriod);

    //delete
    v_Rect.setRect(728, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_Backspace, "", "", QColor(0, 0, 0, 0), QColor(0, 0, 0, 0),
                 QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardNumDelete, KButtonType_LongPress);

    //回车
    v_Rect.setRect(728, 7 + 56 + 6, 59, 118);
    m_pInputButtonEnter         = CreateButton(v_Rect, Qt::Key_Return, "", "", QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), QColor(30, 158, 221), "", g_qstrFullKeyBoardNumEnter);

    //隐藏
    v_Rect.setRect(728, 7 + (56 + 6) * 3, 59, 56);
    CreateButton(v_Rect, PHONE_KEY_INPUTMETHOD_HIDEFULLKEYBOARD, "", "", QColor(0, 0, 0, 0), Qt::black, QColor(0, 0, 0, 0), QColor(30, 158, 221), "", g_qstrFullKeyBoardNumHide);

    //ctr
    v_Rect.setRect(13, 7 + 56 + 6 + 56 + 6 + 56 + 6, 124, 56);
    CreateButton(v_Rect, Qt::Key_Control, "", "", QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), "", "");

}

