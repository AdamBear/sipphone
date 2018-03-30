#include <qnamespace.h>
#include "inputdialogfullenshift.h"

CInputDialogFullEnShift::CInputDialogFullEnShift(void)
{
    //回车按键
    m_pInputButtonEnter             = NULL;
}

CInputDialogFullEnShift::~CInputDialogFullEnShift(void)
{
}

INPUT_DLGTYPE CInputDialogFullEnShift::GetDlgType()
{
    return INPUT_DLGTYPE_ENCAPITAL;
}

//临时方案开始
void CInputDialogFullEnShift::LoadBkGroundPic()
{
    m_qCurrPixMap = THEME_GET_BMP(fromQString(g_qstrDialogFullEnShiftBackGroundWithEnterPic).c_str());
}

void CInputDialogFullEnShift::SetInputDlgStyle(INPUT_DIALOG_STYLE enStyle)
{
    m_qCurrPixMap.detach();
    if (INPUT_DIALOG_NEWLINE == enStyle)
    {
    }
    else if (INPUT_DIALOG_DIAL == enStyle)
    {
        m_qCurrPixMap = THEME_GET_BMP(fromQString(g_qstrDialogFullEnShiftBackGroundPic).c_str());
        m_pInputButtonEnter->SetPressIcon(g_qstrFullKeyBoardEnSendPic);
    }
    else if (INPUT_DIALOG_SEARCH == enStyle)
    {
    }
    else
    {
        m_qCurrPixMap = THEME_GET_BMP(fromQString(g_qstrDialogFullEnShiftBackGroundWithEnterPic).c_str());
        m_pInputButtonEnter->SetPressIcon(g_qstrFullKeyBoardEnEnterPic);
    }
    CBaseInputDlg::SetInputDlgStyle(enStyle);
}

void CInputDialogFullEnShift::NewButton()
{
    QRect v_Rect;
    v_Rect.setRect(13, 7, 59, 56);

    //字母Q
    CreateButton(v_Rect, Qt::Key_Q, "Q", "Q", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftQ);

    //字母我
    v_Rect.setRect(13 + 59 + 6, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_W, "W", "W", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftW);

    //字母E
    v_Rect.setRect(13 + (59 + 6) * 2, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_E, "E", "E", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftE);

    //字母R
    v_Rect.setRect(13 + (59 + 6) * 3, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_R, "R", "R", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftR);

    //字母T
    v_Rect.setRect(13 + (59 + 6) * 4, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_T, "T", "T", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftT);

    //字母Y
    v_Rect.setRect(13 + (59 + 6) * 5, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_Y, "Y", "Y", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftY);

    //字母U
    v_Rect.setRect(13 + (59 + 6) * 6, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_U, "U", "U", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftU);

    //字母I
    v_Rect.setRect(13 + (59 + 6) * 7, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_I, "I", "I", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftI);

    //字母O
    v_Rect.setRect(13 + (59 + 6) * 8, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_O, "O", "O", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftO);

    //字母P
    v_Rect.setRect(13 + (59 + 6) * 9, 7, 59, 56);
    CreateButton(v_Rect, Qt::Key_P, "P", "P", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftP);

    //字母A
    v_Rect.setRect(47, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_A, "A", "A", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftA);

    //字母S
    v_Rect.setRect(47 + 59 + 6, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_S, "S", "S", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftS);

    //字母D
    v_Rect.setRect(47 + (59 + 6) * 2, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_D, "D", "D", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftD);

    //字母F
    v_Rect.setRect(47 + (59 + 6) * 3, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_F, "F", "F", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftF);

    //字母G
    v_Rect.setRect(47 + (59 + 6) * 4, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_G, "G", "G", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftG);

    //字母H
    v_Rect.setRect(47 + (59 + 6) * 5, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_H, "H", "H", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftH);

    //字母J
    v_Rect.setRect(47 + (59 + 6) * 6, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_J, "J", "J", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftJ);

    //字母K
    v_Rect.setRect(47 + (59 + 6) * 7, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_K, "K", "K", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftK);

    //字母L
    v_Rect.setRect(47 + (59 + 6) * 8, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_L, "L", "L", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftL);

    //字符"
    v_Rect.setRect(47 + (59 + 6) * 9, 7 + 56 + 6, 59, 56);
    CreateButton(v_Rect, Qt::Key_QuoteDbl, "\"", "\"", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardQuoteDbl);

    //字母Z
    v_Rect.setRect(13 + (59 + 6), 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_Z, "Z", "Z", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftZ);

    //字母X
    v_Rect.setRect(13 + (59 + 6) * 2, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_X, "X", "X", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftX);

    //字母C
    v_Rect.setRect(13 + (59 + 6) * 3, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_C, "C", "C", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftC);

    //字母V
    v_Rect.setRect(13 + (59 + 6) * 4, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_V, "V", "V", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftV);

    //字母B
    v_Rect.setRect(13 + (59 + 6) * 5, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_B, "B", "B", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftB);

    //字母N
    v_Rect.setRect(13 + (59 + 6) * 6, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_N, "N", "N", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftN);

    //字母M
    v_Rect.setRect(13 + (59 + 6) * 7, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_M, "M", "M", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnShiftM);

    //字母;
    v_Rect.setRect(13 + (59 + 6) * 8, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_Semicolon, ";", ";", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardSemicolon);

    //字符:
    v_Rect.setRect(13 + (59 + 6) * 9, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_Colon, ":", ":", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardColon);

    //字符$
    v_Rect.setRect(13 + (59 + 6) * 10, 7 + (56 + 6) * 2, 59, 56);
    CreateButton(v_Rect, Qt::Key_Dollar, "$", "$", Qt::white, Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardDollar);

    //空白
    v_Rect.setRect(13 + 124 + 6, 7 + (56 + 6) * 3, (59 + 6) * 5 + 59, 56);
    CreateButton(v_Rect, Qt::Key_Space, "", "", QColor(0, 0, 0, 0), Qt::black, QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardSpacePic);

    //左移
    v_Rect.setRect(13 + (59 + 6) * 8, 7 + (56 + 6) * 3, 59, 56);
    CreateButton(v_Rect, Qt::Key_Left, "", "", QColor(0, 0, 0, 0), Qt::black, QColor(0, 0, 0, 0), QColor(30, 158, 221), "", g_qstrFullKeyBoardLeftPic);

    //右移
    v_Rect.setRect(13 + (59 + 6) * 9, 7 + (56 + 6) * 3, 59, 56);
    CreateButton(v_Rect, Qt::Key_Right, "", "", QColor(0, 0, 0, 0), Qt::black, QColor(0, 0, 0, 0), QColor(30, 158, 221), "", g_qstrFullKeyBoardRightPic);

    //隐藏
    v_Rect.setRect(13 + (59 + 6) * 10, 7 + (56 + 6) * 3, 124, 56);
    CreateButton(v_Rect, PHONE_KEY_INPUTMETHOD_HIDEFULLKEYBOARD, "", "", QColor(0, 0, 0, 0), Qt::black, QColor(0, 0, 0, 0), QColor(30, 158, 221), "", g_qstrFullKeyBoardEnHidePic);

    //删除键
    v_Rect.setRect(13 + (59 + 6) * 10, 7, 124, 56);
    CreateButton(v_Rect, Qt::Key_Backspace, "", "", QColor(0, 0, 0, 0), QColor(0, 0, 0, 0),
                 QColor(0, 0, 0, 0), Qt::white, "", g_qstrFullKeyBoardEnDeletePic, KButtonType_LongPress);


    //回车
    v_Rect.setRect(47 + (59 + 6) * 10, 7 + 56 + 6, 90, 56);
    m_pInputButtonEnter         = CreateButton(v_Rect, Qt::Key_Return, "", "", QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), QColor(0, 0, 0, 0), QColor(30, 158, 221), "", g_qstrFullKeyBoardEnEnterPic);

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
