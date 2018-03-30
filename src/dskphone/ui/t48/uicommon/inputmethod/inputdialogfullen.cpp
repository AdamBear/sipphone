#include <qnamespace.h>
#include "inputdialogfullen.h"

CInputDialogFullEn::CInputDialogFullEn(void)
{
    //回车按键
    m_pInputButtonEnter             = NULL;
}

CInputDialogFullEn::~CInputDialogFullEn(void)
{
}

INPUT_DLGTYPE CInputDialogFullEn::GetDlgType()
{
    return INPUT_DLGTYPE_EN;
}

void CInputDialogFullEn::LoadBkGroundPic()
{
    m_qCurrPixMap = THEME_GET_BMP(fromQString(g_qstrDialogFullEnBackGroundWithEnterPic).c_str());
}

void CInputDialogFullEn::SetInputDlgStyle(INPUT_DIALOG_STYLE enStyle)
{
    m_qCurrPixMap.detach();
    if (INPUT_DIALOG_NEWLINE == enStyle)
    {
    }
    else if (INPUT_DIALOG_DIAL == enStyle)
    {
        m_qCurrPixMap = THEME_GET_BMP(fromQString(g_qstrDialogFullEnBackGroundPic).c_str());
        m_pInputButtonEnter->SetPressIcon(g_qstrFullKeyBoardEnSendPic);
    }
    else if (INPUT_DIALOG_SEARCH == enStyle)
    {
    }
    else
    {
        m_qCurrPixMap = THEME_GET_BMP(fromQString(g_qstrDialogFullEnBackGroundWithEnterPic).c_str());
        m_pInputButtonEnter->SetPressIcon(g_qstrFullKeyBoardEnEnterPic);
    }
    CBaseInputDlg::SetInputDlgStyle(enStyle);
}

void CInputDialogFullEn::NewButton()
{
    QRect v_Rect;
    v_Rect.setRect(13, 7, 59, 56);

    //字母Q
    CreateButton(v_Rect, Qt::Key_Q, "", "", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnQ);

    //字母W
    v_Rect.setRect(13 + 59 + 6, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_W, "w", "w", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnW);

    //字母E
    v_Rect.setRect(13 + (59 + 6) * 2, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_E, "e", "e", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnE);

    //字母R
    v_Rect.setRect(13 + (59 + 6) * 3, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_R, "r", "r", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnR);

    //字母T
    v_Rect.setRect(13 + (59 + 6) * 4, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_T, "t", "t", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnT);

    //字母Y
    v_Rect.setRect(13 + (59 + 6) * 5, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_Y, "y", "y", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnY);

    //字母U
    v_Rect.setRect(13 + (59 + 6) * 6, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_U, "u", "u", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnU);

    //字母I
    v_Rect.setRect(13 + (59 + 6) * 7, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_I, "i", "i", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnI);

    //字母O
    v_Rect.setRect(13 + (59 + 6) * 8, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_O, "o", "o", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnO);

    //字母P
    v_Rect.setRect(13 + (59 + 6) * 9, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_P, "p", "p", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnP);

    //字母A
    v_Rect.setRect(47, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_A, "a", "a", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnA);

    //字母S
    v_Rect.setRect(47 + 59 + 6, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_S, "s", "s", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnS);

    //字母D
    v_Rect.setRect(47 + (59 + 6) * 2, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_D, "d", "d", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnD);

    //字母F
    v_Rect.setRect(47 + (59 + 6) * 3, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_F, "f", "f", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnF);

    //字母G
    v_Rect.setRect(47 + (59 + 6) * 4, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_G, "g", "g", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnG);

    //字母H
    v_Rect.setRect(47 + (59 + 6) * 5, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_H, "h", "h", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnH);

    //字母J
    v_Rect.setRect(47 + (59 + 6) * 6, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_J, "j", "j", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnJ);

    //字母K
    v_Rect.setRect(47 + (59 + 6) * 7, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_K, "k", "k", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnK);

    //字母L
    v_Rect.setRect(47 + (59 + 6) * 8, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_L, "l", "l", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnL);

    //字符'
    v_Rect.setRect(47 + (59 + 6) * 9, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_Apostrophe, "'", "'", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardApostrophe);

    //字母Z
    v_Rect.setRect(13 + (59 + 6), 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_Z, "z", "z", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnZ);

    //字母X
    v_Rect.setRect(13 + (59 + 6) * 2, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_X, "x", "x", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnX);

    //字母C
    v_Rect.setRect(13 + (59 + 6) * 3, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_C, "c", "c", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnC);

    //字母V
    v_Rect.setRect(13 + (59 + 6) * 4, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_V, "v", "v", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnV);

    //字母B
    v_Rect.setRect(13 + (59 + 6) * 5, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_B, "b", "b", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnB);

    //字母N
    v_Rect.setRect(13 + (59 + 6) * 6, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_N, "n", "n", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnN);

    //字母M
    v_Rect.setRect(13 + (59 + 6) * 7, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_M, "m", "m", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnM);

    //字符,
    v_Rect.setRect(13 + (59 + 6) * 8, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_Comma, ",", ",", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardComma);

    //字母.
    v_Rect.setRect(13 + (59 + 6) * 9, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_Period, ".", ".", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardPeriod);

    //字符?
    v_Rect.setRect(13 + (59 + 6) * 10, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_Question, "?", "?", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardQuestion);


    //空白条
    v_Rect.setRect(13 + 124 + 6, 7 + (56 + 6) * 3, (59 + 6) * 5 + 59, 56);
    CreateButton(v_Rect, Qt::Key_Space, "", "", QColor(0, 0, 0, 0), Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardSpacePic);

    //left
    v_Rect.setRect(13 + (59 + 6) * 8, 7 + (56 + 6) * 3, 59, 56);
    CreateButton(v_Rect, Qt::Key_Left, "", "", QColor(0, 0, 0, 0), Qt::black, QColor(0, 0, 0, 0), QColor(30, 158, 221), "", g_qstrFullKeyBoardLeftPic);

    //right
    v_Rect.setRect(13 + (59 + 6) * 9, 7 + (56 + 6) * 3, 59, 56);
    CreateButton(v_Rect, Qt::Key_Right, "", "", QColor(0, 0, 0, 0), Qt::black, QColor(0, 0, 0, 0), QColor(30, 158, 221), "", g_qstrFullKeyBoardRightPic);

    //隐藏
    v_Rect.setRect(13 + (59 + 6) * 10, 7 + (56 + 6) * 3, 124, 56);
    CreateButton(v_Rect, PHONE_KEY_INPUTMETHOD_HIDEFULLKEYBOARD, "", "", QColor(0, 0, 0, 0), Qt::black, QColor(0, 0, 0, 0), QColor(30, 158, 221), "", g_qstrFullKeyBoardEnHidePic);

    //delete
    v_Rect.setRect(13 + (59 + 6) * 10, 7, 124, 56);
    CreateButton(v_Rect, Qt::Key_Backspace, "", "", QColor(0, 0, 0, 0), QColor(0, 0, 0, 0),
                 QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnDeletePic, KButtonType_LongPress);

    //回车
    v_Rect.setRect(47 + (59 + 6) * 10, 7 + 56 + 6, 90, 56);
    m_pInputButtonEnter = CreateButton(v_Rect, Qt::Key_Return, "", "", QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), QColor(30, 158, 221), "", g_qstrFullKeyBoardEnEnterPic);


    //左边shift
    v_Rect.setRect(13, 7 + 56 + 6 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_Shift, "", "", QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), "", "");

    //右边shift
    v_Rect.setRect(13 + (59 + 6) * 11, 7 + 56 + 6 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_Shift, "", "", QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), "", "");

    //control
    v_Rect.setRect(13, 7 + 56 + 6 + 56 + 6 + 56 + 6, 124, 56);
    CreateButton(v_Rect, Qt::Key_Control, "", "", QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), "", "");

}

