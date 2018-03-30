#include "inputdialogcustom.h"

#include "keyboardlayoutmanager.h"
#include "keyboardimemanager.h"
#include "keyboardlangmanager.h"
#include "keyboardswitchmanager.h"

#include "datastruct.h"

#include "qtcommon/qmisc.h"
#include "dsklog/log.h"
#include "mainwnd/modmainwnd.h"
#include "include/translateiddefine.h"
#include "uikernel/languagehelper.h"
#include "inputmethod/inputmethod.h"
#include "qtcommon/xmlhelper.h"

namespace
{
//绘制键盘使用

#ifdef IF_LYNC
//按钮圆角宽度
#define CUSTOM_BUTTON_FILLET_WIDTH              10
//按钮圆角高度
#define CUSTOM_BUTTON_FILLET_HEIGHT             10
//按钮阴影高度
#define CUSTOM_BUTTON_SHADOW_HEIGHT     2
#endif

//字体大小
#define CUSTOM_BUTTON_FONT_SIZE                 26
#define CUSTOM_BUTTON_ENTER_FONT_SIZE                   24
#define CUSTOM_BUTTON_HINT_FONT_SIZE            16

#define CUSTOM_BUTTON_HINT_TEXT_HEIGHT      22

#define CUSTOM_BUTTON_FONT_SEP_OFFSET       10

#define CUSTOM_BUTTON_NORMAL_TEXT_COLOR         QColor(0, 0, 0)
#define CUSTOM_BUTTON_PRESS_TEXT_COLOR          QColor(0, 0, 0)
#define CUSTOM_BUTTON_HINT_TEXT_COLOR           QColor(0xa8, 0xa8, 0xa8)

#ifdef IF_LYNC
#define CUSTOM_BUTTON_SHADOW_COLOR              QColor(4, 47, 57)
#endif

#define CUSTOM_BUTTON_BG_NORMAL_COLOR           QColor(37, 37, 37)
#define CUSTOM_BUTTON_BG_WHITE_COLOR            QColor(255, 255, 255)
#define CUSTOM_BUTTON_BG_BLUE_COLOR             QColor(0, 175, 240)


#define INPUT_KEYBOARD_BG                   PIC_INPUTMETHOD_PATH "fullkeyboard_bg.png"

#define INPUT_BUTTON_ICON_BACKSPACE         PIC_INPUTMETHOD_PATH "btn_icon_delete.png"
#define INPUT_BUTTON_ICON_HIDE              PIC_INPUTMETHOD_PATH "btn_icon_hide.png"
#define INPUT_BUTTON_ICON_SHIFT             PIC_INPUTMETHOD_PATH "btn_icon_shift.png"

#ifdef IF_LYNC
#define INPUT_BUTTON_ICON_SHIFT_CAPITAL     PIC_INPUTMETHOD_PATH "btn_icon_shift_blue.png"
#endif

#define INPUT_BUTTON_ICON_LANG              PIC_INPUTMETHOD_PATH "btn_icon_lang.png"

#define INPUT_BUTTON_ICON_LEFT              PIC_INPUTMETHOD_PATH "btn_icon_left.png"
#define INPUT_BUTTON_ICON_RIGHT             PIC_INPUTMETHOD_PATH "btn_icon_right.png"
#define INPUT_BUTTON_ICON_UP                PIC_INPUTMETHOD_PATH "btn_icon_up.png"
#define INPUT_BUTTON_ICON_DOWN              PIC_INPUTMETHOD_PATH "btn_icon_down.png"

#ifdef IF_LYNC
#define INPUT_BUTTON_ICON_BACKSPACE_PRESS   PIC_INPUTMETHOD_PATH "btn_icon_delete_pressed.png"
#define INPUT_BUTTON_ICON_SHIFT_PRESS       PIC_INPUTMETHOD_PATH "btn_icon_shift_pressed.png"
#define INPUT_BUTTON_ICON_LEFT_PRESS        PIC_INPUTMETHOD_PATH "btn_icon_left_pressed.png"
#define INPUT_BUTTON_ICON_RIGHT_PRESS       PIC_INPUTMETHOD_PATH "btn_icon_right_pressed.png"
#endif


#define INPUT_BUTTON_ICON   PIC_INPUTMETHOD_PATH "btn_icon.9.png"
#define INPUT_BUTTON_ICON_PRESS PIC_INPUTMETHOD_PATH "btn_icon_press.9.png"
}

CInputDialogCustom::CInputDialogCustom(int nImeIndex)
    : m_nImeIndex(nImeIndex)
    , m_eImeType(IME_TYPE_ERROR)
    , m_strDisplayName("")
    , m_bIsInputBoxShow(false)
    , m_nInputBoxIndex(KEYBOARD_INPUTBOX_ERROR_INDEX)
    , m_eDlgStyle(INPUT_DIALOG_NORMAL)
{
    INPUTMETHOD_INFO("CInputDialogCustom::CInputDialogCustom, Ime Index = %d", m_nImeIndex);

    setObjectName("InputDialog");
    m_listImeButton.clear();
    m_listEnterButton.clear();

    m_GlobleRect.setRect(g_iFullKeyBoardX, g_iFullKeyBoardY, g_iFullKeyBoardWidth, g_iFullKeyBoardHeight);
//#ifndef IF_LYNC
    connect(&g_InputBoxButton, SIGNAL(SignalInputBoxPress()), this, SLOT(OnInputBoxPress()), Qt::UniqueConnection);
//#endif
}

CInputDialogCustom::~CInputDialogCustom()
{

}

INPUT_DLGTYPE CInputDialogCustom::GetDlgType()
{
    return (INPUT_DLGTYPE)(INPUT_DLGTYPE_CUSTOM + m_nImeIndex);
}

void CInputDialogCustom::NewButton()
{
    //获取Ime及其ImeKey列表
    const SKeyboardIme* pKeyboardIme = g_KeyBoardImeManager.GetImeByIndex(m_nImeIndex);
    if (NULL == pKeyboardIme)
    {
        return;
    }
    m_eImeType = pKeyboardIme->m_eImeType;
    m_strDisplayName = pKeyboardIme->m_strDisplayName;
    const vecKeyboardImeKeyTable& tvecImeKeyTable = pKeyboardIme->m_vecImeKey;

    //根据Ime获取对应的Layout及其Layout列表
    yl::string strLayoutName = pKeyboardIme->m_strLayoutName;
    const SKeyboardLayout* pKeyboardLayout = g_KeyBoardLayoutManager.GetLayoutByName(strLayoutName);
    if (NULL == pKeyboardLayout)
    {
        return;
    }
    const vecKeyboardLayoutTable& tvecLayoutTable = pKeyboardLayout->m_vecKeyboardLayoutTable;

    m_listButton.clear();

    //同时遍历两个表，行数、按钮数都以少的那个为准
    vecKeyboardImeKeyTable::const_iterator iterImeKeyLine = tvecImeKeyTable.begin();
    vecKeyboardLayoutTable::const_iterator iterLayoutLine = tvecLayoutTable.begin();

    //遍历表格
    for (; iterImeKeyLine != tvecImeKeyTable.end() && iterLayoutLine != tvecLayoutTable.end()
            ; ++iterImeKeyLine, ++iterLayoutLine)
    {
        const vecKeyboardImeKeyLine& tImeKeyLine = *iterImeKeyLine;
        const SKeyboardLayoutLine& tLayoutLine = *iterLayoutLine;
        const vecKeyboardLayoutLine& tLayoutLineData = tLayoutLine.m_vecKeyboardLine;

        vecKeyboardImeKeyLine::const_iterator iterImeKey = tImeKeyLine.begin();
        vecKeyboardLayoutLine::const_iterator iterLayoutKey = tLayoutLineData.begin();

        vecButtonLine pButtonLine;

        //遍历行
        for (; iterImeKey != tImeKeyLine.end() && iterLayoutKey != tLayoutLineData.end()
                ; ++iterImeKey, ++iterLayoutKey)
        {
            const SKeyBoardImeKey& tImeKey = *iterImeKey;
            const SKeyboardLayoutKey& tLayoutKey = *iterLayoutKey;

            CCustomButton* pButton = CreateCustomButton(tImeKey, tLayoutKey);
            if (NULL == pButton)
            {
                continue;
            }
            pButtonLine.push_back(pButton);
        }

        m_listButton.push_back(pButtonLine);
    }
}

CCustomButton* CInputDialogCustom::CreateCustomButton(const SKeyBoardImeKey& tImeKey, const SKeyboardLayoutKey& tLayoutKey)
{
    CCustomButton* pInputButton = new CCustomButton(this);
    if (NULL == pInputButton)
    {
        return NULL;
    }
    //设置按钮默认属性
    pInputButton->SetParent(this);
    //默认支持长按
    pInputButton->SetLongPress(true);
    //默认长按后仅发送一次
    pInputButton->SetStopWhenEmit(true);
    //默认响应抬起
    pInputButton->SetIsResponePress(false);
    //默认普通背景
#ifndef IF_LYNC
    pInputButton->SetPressTextColor(CUSTOM_BUTTON_PRESS_TEXT_COLOR);
    pInputButton->setNormalTextColor(CUSTOM_BUTTON_NORMAL_TEXT_COLOR);
    pInputButton->SetNormalBackGround(CUSTOM_BUTTON_BG_NORMAL_COLOR);
    pInputButton->SetPressBackGround(CUSTOM_BUTTON_BG_WHITE_COLOR);
#else
    pInputButton->SetButtonBGType(BUTTON_BG_TYPE_NORMAL);
#endif
    //默认居中
    pInputButton->setIconPicAlign(Qt::AlignCenter);


    const SKeyboardKeyRect& tButtonRect = tLayoutKey.m_rectButton;
    const SKeyboardKeyRect& tButtonRespond = tLayoutKey.m_rectRespond;

    //记录按钮相对键盘的位置
    pInputButton->m_rctButton = QRect(tButtonRect.m_nLeft, tButtonRect.m_nTop, tButtonRect.m_nWidth, tButtonRect.m_nHeight);
    //记录按钮相对全局的响应位置
    pInputButton->m_rctGlobalButton = QRect(tButtonRespond.m_nLeft, tButtonRespond.m_nTop, tButtonRespond.m_nWidth, tButtonRespond.m_nHeight);

    //否则以Function优先
    if ("" != tImeKey.m_strFunction)
    {
        ManagerButtonFuction(tImeKey.m_strFunction, pInputButton);
    }
    //没有Function才去获取字符
    else
    {
        ManagerButtonChar(tImeKey, pInputButton);
    }

    //如果有DisplayName，则用其文字内容覆盖掉显示，但是其他内容不变
    if ("" != tImeKey.m_strDisplayName)
    {
        pInputButton->SetNormalText(toQString(tImeKey.m_strDisplayName));
        pInputButton->SetHintText("");
        pInputButton->SetButtonIconString("");
    }

    connect(pInputButton, SIGNAL(SignalLongPress(int)), this, SLOT(OnBtnClick(int)));
    connect(pInputButton, SIGNAL(SignalLongPressToBox(bool)), this, SLOT(OnLongPressToBox(bool)));

    return pInputButton;
}

void CInputDialogCustom::ManagerButtonFuction(const yl::string& strFunction, CCustomButton* pInputButton)
{
    if (NULL == pInputButton)
    {
        return;
    }

    //设置为功能键
    pInputButton->SetFunctionKey(true);
    pInputButton->SetHintText("");
    pInputButton->SetFunctionName(toQString(strFunction));
    if (INPUT_IME_FUNCTION_BACKSPACE == strFunction)
    {
        pInputButton->SetStopWhenEmit(false);
#ifndef IF_LYNC
        pInputButton->SetNormalIcon(INPUT_BUTTON_ICON_BACKSPACE);
        pInputButton->SetPressIcon(INPUT_BUTTON_ICON_BACKSPACE);
        pInputButton->SetNormalBackGround(CUSTOM_BUTTON_BG_NORMAL_COLOR);
        pInputButton->SetPressBackGround(CUSTOM_BUTTON_BG_WHITE_COLOR);
#else
        pInputButton->SetButtonBGType(BUTTON_BG_TYPE_DEEP);
        pInputButton->SetButtonIconString(INPUT_BUTTON_ICON_BACKSPACE);
#endif
    }
    else if (INPUT_IME_FUNCTION_ENTER == strFunction)
    {
        pInputButton->SetEnterKey(true);
        pInputButton->SetLongPress(false);
#ifndef IF_LYNC
        pInputButton->SetNormalIcon(g_qstrFullKeyBoardEnterIcon);
        pInputButton->SetPressIcon(g_qstrFullKeyBoardEnterIcon);
        pInputButton->SetNormalBackGround(CUSTOM_BUTTON_BG_NORMAL_COLOR);
        pInputButton->SetPressBackGround(CUSTOM_BUTTON_BG_WHITE_COLOR);
#else
        pInputButton->setIconPic(g_qstrFullKeyBoardEnterIcon);
        pInputButton->SetButtonBGType(BUTTON_BG_TYPE_DEEP);
#endif
        m_listEnterButton.push_back(pInputButton);
    }
    else if (INPUT_IME_FUNCTION_HIDE == strFunction)
    {
        pInputButton->SetLongPress(false);
#ifndef IF_LYNC
        pInputButton->SetNormalBackGround(CUSTOM_BUTTON_BG_NORMAL_COLOR);
        pInputButton->SetPressBackGround(CUSTOM_BUTTON_BG_BLUE_COLOR);
        pInputButton->SetNormalIcon(INPUT_BUTTON_ICON_HIDE);
        pInputButton->SetPressIcon(INPUT_BUTTON_ICON_HIDE);
#else
        pInputButton->SetButtonBGType(BUTTON_BG_TYPE_DEEP);
        pInputButton->SetButtonIconString(INPUT_BUTTON_ICON_HIDE);
#endif
    }
    else if (INPUT_IME_FUNCTION_SHIFT == strFunction)
    {
        pInputButton->SetLongPress(false);
#ifndef IF_LYNC
        pInputButton->SetShiftKey(true);
        if (IME_TYPE_CAPITAL == m_eImeType)
        {
            pInputButton->SetNormalBackGround(CUSTOM_BUTTON_BG_BLUE_COLOR);
            pInputButton->SetPressBackGround(CUSTOM_BUTTON_BG_BLUE_COLOR);
        }
        else
        {
            pInputButton->SetNormalBackGround(CUSTOM_BUTTON_BG_NORMAL_COLOR);
            pInputButton->SetPressBackGround(CUSTOM_BUTTON_BG_NORMAL_COLOR);
        }

        pInputButton->SetNormalIcon(INPUT_BUTTON_ICON_SHIFT);
        pInputButton->SetPressIcon(INPUT_BUTTON_ICON_SHIFT);
#else
        if (IME_TYPE_CAPITAL == m_eImeType)
        {
            pInputButton->SetButtonBGType(BUTTON_BG_TYPE_LIGHT);
            pInputButton->SetButtonIconString(INPUT_BUTTON_ICON_SHIFT_CAPITAL);
        }
        else
        {
            pInputButton->SetButtonBGType(BUTTON_BG_TYPE_DEEP);
            pInputButton->SetButtonIconString(INPUT_BUTTON_ICON_SHIFT);
        }
#endif
    }
    else if (INPUT_IME_FUNCTION_LANG == strFunction)
    {
#ifndef IF_LYNC
        pInputButton->SetNormalBackGround(CUSTOM_BUTTON_BG_NORMAL_COLOR);
        pInputButton->SetPressBackGround(CUSTOM_BUTTON_BG_BLUE_COLOR);
        pInputButton->SetNormalIcon(INPUT_BUTTON_ICON_LANG);
        pInputButton->SetPressIcon(INPUT_BUTTON_ICON_LANG);
#else
        pInputButton->SetButtonBGType(BUTTON_BG_TYPE_DEEP);
        pInputButton->SetButtonIconString(INPUT_BUTTON_ICON_LANG);
#endif
    }
    else if (INPUT_IME_FUNCTION_SWITCH == strFunction)
    {
#ifndef IF_LYNC
        //pInputButton->SetNormalText(toQString(m_strDisplayName));
        /*if (IME_TYPE_SYMBOL == m_eImeType)
        {
            pInputButton->SetNormalBackGround(CUSTOM_BUTTON_BG_BLUE_COLOR);
            pInputButton->SetPressBackGround(CUSTOM_BUTTON_BG_BLUE_COLOR);
        }
        else*/
        {
            pInputButton->SetNormalBackGround(CUSTOM_BUTTON_BG_NORMAL_COLOR);
            pInputButton->SetPressBackGround(CUSTOM_BUTTON_BG_WHITE_COLOR);
        }

        pInputButton->setNormalTextColor(CUSTOM_BUTTON_NORMAL_TEXT_COLOR);
        pInputButton->SetPressTextColor(CUSTOM_BUTTON_NORMAL_TEXT_COLOR);
        pInputButton->SetLongPress(false);
#else
        pInputButton->SetButtonBGType(BUTTON_BG_TYPE_DEEP);
        pInputButton->setNormalTextColor(Qt::white);
        pInputButton->SetPressTextColor(Qt::white);
#endif
        pInputButton->SetImeKey(true);

        m_listImeButton.push_back(pInputButton);
    }
    else if (INPUT_IME_FUNCTION_LEFT == strFunction)
    {
        pInputButton->SetLongPress(false);
#ifndef IF_LYNC
        pInputButton->SetNormalIcon(INPUT_BUTTON_ICON_LEFT);
        pInputButton->SetPressIcon(INPUT_BUTTON_ICON_LEFT);
        pInputButton->SetNormalBackGround(CUSTOM_BUTTON_BG_NORMAL_COLOR);
        pInputButton->SetPressBackGround(CUSTOM_BUTTON_BG_WHITE_COLOR);
#else
        pInputButton->SetButtonIconString(INPUT_BUTTON_ICON_LEFT);
#endif
    }
    else if (INPUT_IME_FUNCTION_RIGHT == strFunction)
    {
        pInputButton->SetLongPress(false);
#ifndef IF_LYNC
        pInputButton->SetNormalIcon(INPUT_BUTTON_ICON_RIGHT);
        pInputButton->SetPressIcon(INPUT_BUTTON_ICON_RIGHT);
        pInputButton->SetNormalBackGround(CUSTOM_BUTTON_BG_NORMAL_COLOR);
        pInputButton->SetPressBackGround(CUSTOM_BUTTON_BG_WHITE_COLOR);
#else
        pInputButton->SetButtonIconString(INPUT_BUTTON_ICON_RIGHT);
#endif
    }
    else if (INPUT_IME_FUNCTION_UP == strFunction)
    {
        pInputButton->SetLongPress(false);
#ifndef IF_LYNC
        pInputButton->SetNormalIcon(INPUT_BUTTON_ICON_UP);
        pInputButton->SetPressIcon(INPUT_BUTTON_ICON_UP);
        pInputButton->SetNormalBackGround(CUSTOM_BUTTON_BG_NORMAL_COLOR);
        pInputButton->SetPressBackGround(CUSTOM_BUTTON_BG_BLUE_COLOR);
#else
        pInputButton->SetButtonIconString(INPUT_BUTTON_ICON_UP);
#endif
    }
    else if (INPUT_IME_FUNCTION_DOWN == strFunction)
    {
        pInputButton->SetLongPress(false);
#ifndef IF_LYNC
        pInputButton->SetNormalIcon(INPUT_BUTTON_ICON_DOWN);
        pInputButton->SetPressIcon(INPUT_BUTTON_ICON_DOWN);
        pInputButton->SetNormalBackGround(CUSTOM_BUTTON_BG_NORMAL_COLOR);
        pInputButton->SetPressBackGround(CUSTOM_BUTTON_BG_BLUE_COLOR);
#else
        pInputButton->SetButtonIconString(INPUT_BUTTON_ICON_DOWN);
#endif
    }

    m_listFunctionButton.push_back(pInputButton);
}

void CInputDialogCustom::ManagerButtonChar(const SKeyBoardImeKey& tImeKey, CCustomButton* pInputButton)
{
    if (NULL == pInputButton)
    {
        return;
    }

    QString strTmpText = toQString(tImeKey.m_strImeList);
    if (strTmpText.size() > 1)
    {
        //如果备选字符大于一个，就把第一个设置为首选字符，第二个设置为提示字符
        pInputButton->SetNormalText(strTmpText.at(0));
        pInputButton->SetHintText(strTmpText.at(1));
    }
    else if (strTmpText.size() > 0)
    {
        //如果备选字符只有一个，就把第一个设置为首选字符，提示字符设置为空
        pInputButton->SetNormalText(strTmpText.at(0));
        pInputButton->SetHintText("");
    }
    else
    {
        //如果没有备选字符，就都设为空
        pInputButton->SetNormalText("");
        pInputButton->SetHintText("");
    }
}

void CInputDialogCustom::LoadBkGroundPic()
{
    m_qCurrPixMap = THEME_GET_BMP(INPUT_KEYBOARD_BG);

    QPainter pixPainter(&m_qCurrPixMap);

    //遍历按钮
    vecButtonTable::iterator iterButtonLine = m_listButton.begin();
    for (; iterButtonLine != m_listButton.end(); ++iterButtonLine)
    {
        vecButtonLine& tButtonLine = *iterButtonLine;
        vecButtonLine::iterator iterButtonKey = tButtonLine.begin();
        for (; iterButtonKey != tButtonLine.end(); ++iterButtonKey)
        {
            CCustomButton* pButtonKey = *iterButtonKey;
            if (NULL == pButtonKey)
            {
                continue;
            }

            if (pButtonKey->IfButtonAvailable())
            {
                paintButtonBackground(pixPainter, pButtonKey);
                paintButtonIcon(pixPainter, pButtonKey);
                paintButtonText(pixPainter, pButtonKey);
            }
        }
    }
}

void CInputDialogCustom::paintButtonBackground(QPainter& pixPainter, CCustomButton* pButtonKey, bool bPressed)
{
    if (NULL == pButtonKey)
    {
        return;
    }
#ifndef IF_LYNC
    bool isShiftKey = pButtonKey->IsShiftKey();
    bool isImeKey = pButtonKey->IsImeKey();
    if ((IME_TYPE_CAPITAL == m_eImeType
            && isShiftKey))
    {
        if (!bPressed)
        {
            bPressed = true;
        }
    }
    // draw .9png
    if (bPressed)
    {
        Theme_Draw_9_Pix(pixPainter, INPUT_BUTTON_ICON_PRESS, pButtonKey->m_rctButton);
    }
    else
    {
        Theme_Draw_9_Pix(pixPainter, INPUT_BUTTON_ICON, pButtonKey->m_rctButton);
    }

#else
    //如果按钮大小小于圆角的两倍，则直接使用矩形
    if (pButtonKey->m_rctButton.width() < CUSTOM_BUTTON_FILLET_WIDTH + CUSTOM_BUTTON_FILLET_WIDTH
            || pButtonKey->m_rctButton.height() < CUSTOM_BUTTON_FILLET_HEIGHT + CUSTOM_BUTTON_FILLET_HEIGHT)
    {
        QColor clrButton = CCustomButton::GetColorByType(pButtonKey->GetButtonBGType());
        pixPainter.setPen(clrButton);
        pixPainter.setBrush(clrButton);
        pixPainter.drawRoundedRect(pButtonKey->m_rctButton, CUSTOM_BUTTON_FILLET_WIDTH, CUSTOM_BUTTON_FILLET_HEIGHT);

        return;
    }

    //分别画四个圆角
    pixPainter.drawPixmap(
        pButtonKey->m_rctButton.x()
        , pButtonKey->m_rctButton.y()
        , THEME_GET_BMP(CCustomButton::GetFilletPixByType(pButtonKey->GetButtonBGType(), BUTTON_ANGLE_TYPE_LEFT_TOP).toUtf8().data()));

    pixPainter.drawPixmap(
        pButtonKey->m_rctButton.x()
        , pButtonKey->m_rctButton.y() + pButtonKey->m_rctButton.height() - CUSTOM_BUTTON_FILLET_HEIGHT
        , THEME_GET_BMP(CCustomButton::GetFilletPixByType(pButtonKey->GetButtonBGType(), BUTTON_ANGLE_TYPE_LEFT_BOTTOM).toUtf8().data()));

    pixPainter.drawPixmap(
        pButtonKey->m_rctButton.x() + pButtonKey->m_rctButton.width() - CUSTOM_BUTTON_FILLET_WIDTH
        , pButtonKey->m_rctButton.y()
        , THEME_GET_BMP(CCustomButton::GetFilletPixByType(pButtonKey->GetButtonBGType(), BUTTON_ANGLE_TYPE_RIGHT_TOP).toUtf8().data()));

    pixPainter.drawPixmap(
        pButtonKey->m_rctButton.x() + pButtonKey->m_rctButton.width() - CUSTOM_BUTTON_FILLET_WIDTH
        , pButtonKey->m_rctButton.y() + pButtonKey->m_rctButton.height() - CUSTOM_BUTTON_FILLET_HEIGHT
        , THEME_GET_BMP(CCustomButton::GetFilletPixByType(pButtonKey->GetButtonBGType(), BUTTON_ANGLE_TYPE_RIGHT_BOTTOM).toUtf8().data()));

    //画剩下的上中下三部分
    QRect rctTop(pButtonKey->m_rctButton.x() + CUSTOM_BUTTON_FILLET_WIDTH
                 , pButtonKey->m_rctButton.y()
                 , pButtonKey->m_rctButton.width() - CUSTOM_BUTTON_FILLET_WIDTH - CUSTOM_BUTTON_FILLET_WIDTH
                 , CUSTOM_BUTTON_FILLET_HEIGHT);

    QRect rctMiddle(pButtonKey->m_rctButton.x()
                    , pButtonKey->m_rctButton.y() + CUSTOM_BUTTON_FILLET_HEIGHT
                    , pButtonKey->m_rctButton.width()
                    , pButtonKey->m_rctButton.height() - CUSTOM_BUTTON_FILLET_HEIGHT - CUSTOM_BUTTON_FILLET_HEIGHT);

    QRect rctBottom(pButtonKey->m_rctButton.x() + CUSTOM_BUTTON_FILLET_WIDTH
                    , pButtonKey->m_rctButton.y() + pButtonKey->m_rctButton.height() - CUSTOM_BUTTON_FILLET_HEIGHT
                    , pButtonKey->m_rctButton.width() - CUSTOM_BUTTON_FILLET_WIDTH - CUSTOM_BUTTON_FILLET_WIDTH
                    , CUSTOM_BUTTON_FILLET_HEIGHT - CUSTOM_BUTTON_SHADOW_HEIGHT);

    QColor clrButton = CCustomButton::GetColorByType(pButtonKey->GetButtonBGType());
    pixPainter.fillRect(rctTop, clrButton);
    pixPainter.fillRect(rctMiddle, clrButton);
    pixPainter.fillRect(rctBottom, clrButton);

    //画底部阴影
    QRect rctShadow(pButtonKey->m_rctButton.x() + CUSTOM_BUTTON_FILLET_WIDTH
                    , pButtonKey->m_rctButton.y() + pButtonKey->m_rctButton.height() - CUSTOM_BUTTON_SHADOW_HEIGHT
                    , pButtonKey->m_rctButton.width() - CUSTOM_BUTTON_FILLET_WIDTH - CUSTOM_BUTTON_FILLET_WIDTH
                    , CUSTOM_BUTTON_SHADOW_HEIGHT);
    pixPainter.fillRect(rctShadow, CUSTOM_BUTTON_SHADOW_COLOR);
#endif
}

void CInputDialogCustom::paintButtonIcon(QPainter& pixPainter, CCustomButton* pButtonKey, bool bPressed)
{
    if (NULL == pButtonKey)
    {
        return;
    }
#ifndef IF_LYNC
    QString strButtonIcon;
    if (bPressed)
    {
        strButtonIcon = pButtonKey->GetButtonPressIcon();
    }
    else
    {
        strButtonIcon = pButtonKey->GetButtonNormalIcon();
    }

#else
    QString strButtonIcon = pButtonKey->GetButtonIconString();
#endif
    if (pButtonKey->GetIfFunctionKey()
            && "" != strButtonIcon
            && !strButtonIcon.isEmpty()
            && !strButtonIcon.isNull())
    {
        const QPixmap& pmIcon = THEME_GET_BMP(strButtonIcon.toUtf8().data());
        if (!pmIcon.isNull())
        {
            pixPainter.drawPixmap(pButtonKey->m_rctButton.x() + pButtonKey->m_rctButton.width() / 2 - pmIcon.width() / 2,
                                  pButtonKey->m_rctButton.y() + pButtonKey->m_rctButton.height() / 2 - pmIcon.height() / 2, pmIcon);
        }
    }
}

void CInputDialogCustom::paintButtonText(QPainter& pixPainter, CCustomButton* pButtonKey, bool bPressed)
{
    if (NULL == pButtonKey)
    {
        return;
    }

    bool isEnter = pButtonKey->IsEnterKey();
    const QString& strNormalText = pButtonKey->GetNormalText();
    const QString& strHintText = pButtonKey->GetHintText();

    //画首个字符
    if ("" != strNormalText)
    {
        QFont fontName = pixPainter.font();
        QPen pen = pixPainter.pen();
        pen.setColor(CUSTOM_BUTTON_NORMAL_TEXT_COLOR);
        pixPainter.setPen(pen);
        if (isEnter)
        {
            fontName.setPointSize(CUSTOM_BUTTON_ENTER_FONT_SIZE);
        }
        else
        {
            fontName.setPointSize(CUSTOM_BUTTON_FONT_SIZE);
        }

        pixPainter.setFont(fontName);

        QString strDisp = CutOffText(LANG_PARSE(strNormalText), pixPainter.fontMetrics(), pButtonKey->m_rctButton.width());
        QRect rctNormalText;
#ifndef IF_LYNC
        //如果有第二个字符，则首个字符的位置要上移一点
        if ("" != strHintText)
        {
            rctNormalText.setRect(pButtonKey->m_rctButton.left()
                                  , pButtonKey->m_rctButton.top()
                                  , pButtonKey->m_rctButton.width()
                                  , pButtonKey->m_rctButton.height() - CUSTOM_BUTTON_FONT_SEP_OFFSET - CUSTOM_BUTTON_FONT_SEP_OFFSET);
        }
        //否则画在正中央
        else
#endif
        {
            rctNormalText.setRect(pButtonKey->m_rctButton.left()
                                  , pButtonKey->m_rctButton.top()
                                  , pButtonKey->m_rctButton.width()
                                  , pButtonKey->m_rctButton.height());
        }
        pixPainter.drawText(rctNormalText, Qt::AlignCenter, strDisp);
    }
#ifndef IF_LYNC
    //画第二个字符（如果存在）
    if ("" != strHintText)
    {
        QFont fontName = pixPainter.font();
        QPen pen = pixPainter.pen();
        pen.setColor(CUSTOM_BUTTON_HINT_TEXT_COLOR);
        pixPainter.setPen(pen);
        fontName.setPointSize(CUSTOM_BUTTON_HINT_FONT_SIZE);
        pixPainter.setFont(fontName);

        QString strDisp = CutOffText(strHintText, pixPainter.fontMetrics(), pButtonKey->m_rctButton.width());
        QRect rctHintText;
        rctHintText.setRect(pButtonKey->m_rctButton.left()
                            , pButtonKey->m_rctButton.top() + (pButtonKey->m_rctButton.height() + CUSTOM_BUTTON_HINT_FONT_SIZE - CUSTOM_BUTTON_FONT_SEP_OFFSET) / 2
                            , pButtonKey->m_rctButton.width()
                            , CUSTOM_BUTTON_HINT_TEXT_HEIGHT);
        pixPainter.drawText(rctHintText, Qt::AlignCenter, strDisp);
    }
#endif
}

void CInputDialogCustom::SetImeDisplayName(const yl::string& strName)
{
    vecButtonLine::iterator iterButtonKey = m_listImeButton.begin();
    for (; iterButtonKey != m_listImeButton.end(); ++iterButtonKey)
    {
        CCustomButton* pButtonKey = *iterButtonKey;
        if (NULL == pButtonKey)
        {
            continue;
        }
        pButtonKey->SetNormalText(toQString(strName));
    }
}

void CInputDialogCustom::SetEnterType(EnterKeyType eEnterType)
{
    m_eEnterType = eEnterType;
    if (FullKeyboardEnabled())
    {
#ifdef CUSTOME_ENTERKEY
        vecButtonLine::iterator iterButtonEnterKey = m_listEnterButton.begin();
        for (; iterButtonEnterKey != m_listEnterButton.end(); ++iterButtonEnterKey)
        {
            CCustomButton* pButtonKey = *iterButtonEnterKey;
            if (NULL == pButtonKey)
            {
                continue;
            }
            pButtonKey->SetNormalText("");

            if (INPUT_DIALOG_NEWLINE == m_eDlgStyle)
            {
                pButtonKey->SetNormalIcon("");
                pButtonKey->SetPressIcon("");
                if (m_eEnterType == ENTER_TYPE_DONE)
                {
                    pButtonKey->SetNormalText(LANG_TRANSLATE(TRID_KEYBOARD_DONE));
                }
                else if (m_eEnterType == ENTER_TYPE_NEXT)
                {
                    pButtonKey->SetNormalText(LANG_TRANSLATE(TRID_NEXT));
                }
                else if (m_eEnterType == ENTER_TYPE_ICON)
                {
                    pButtonKey->SetNormalIcon(g_qstrFullKeyBoardEnterIcon);
                    pButtonKey->SetPressIcon(g_qstrFullKeyBoardEnterIcon);
                }

            }
            else if (INPUT_DIALOG_DIAL == m_eDlgStyle)
            {
                pButtonKey->SetNormalIcon(g_qstrFullKeyBoardDialIcon);
                pButtonKey->SetPressIcon(g_qstrFullKeyBoardDialIcon);
            }
            else if (INPUT_DIALOG_SEARCH == m_eDlgStyle)
            {
                pButtonKey->SetNormalIcon(g_qstrFullKeyBoardSearchIcon);
                pButtonKey->SetPressIcon(g_qstrFullKeyBoardSearchIcon);
            }
            else
            {
                pButtonKey->SetNormalIcon(g_qstrFullKeyBoardEnterIcon);
                pButtonKey->SetPressIcon(g_qstrFullKeyBoardEnterIcon);
            }
        }

        update();
#endif
    }
}
#ifndef IF_LYNC
void CInputDialogCustom::SetInputDlgStyle(INPUT_DIALOG_STYLE enStyle)
{
    m_eDlgStyle = enStyle;
    vecButtonLine::iterator iterButtonEnterKey = m_listEnterButton.begin();
    for (; iterButtonEnterKey != m_listEnterButton.end(); ++iterButtonEnterKey)
    {
        CCustomButton* pButtonKey = *iterButtonEnterKey;
        if (NULL == pButtonKey)
        {
            continue;
        }
        pButtonKey->SetNormalText("");

        if (INPUT_DIALOG_NEWLINE == enStyle)
        {
            if (FullKeyboardEnabled())
            {
#ifdef CUSTOME_ENTERKEY
                pButtonKey->SetNormalIcon("");
                pButtonKey->SetPressIcon("");
                if (m_eEnterType == ENTER_TYPE_DONE)
                {
                    pButtonKey->SetNormalText(LANG_TRANSLATE(TRID_KEYBOARD_DONE));
                }
                else if (m_eEnterType == ENTER_TYPE_NEXT)
                {
                    pButtonKey->SetNormalText(LANG_TRANSLATE(TRID_NEXT));
                }
                else if (m_eEnterType == ENTER_TYPE_ICON)
                {
                    pButtonKey->SetNormalIcon(g_qstrFullKeyBoardEnterIcon);
                    pButtonKey->SetPressIcon(g_qstrFullKeyBoardEnterIcon);
                }
#else
                pButtonKey->SetNormalIcon(g_qstrFullKeyBoardEnterIcon);
                pButtonKey->SetPressIcon(g_qstrFullKeyBoardEnterIcon);
#endif
            }
        }
        else if (INPUT_DIALOG_DIAL == enStyle)
        {
            pButtonKey->SetNormalIcon(g_qstrFullKeyBoardDialIcon);
            pButtonKey->SetPressIcon(g_qstrFullKeyBoardDialIcon);
        }
        else if (INPUT_DIALOG_SEARCH == enStyle)
        {
            pButtonKey->SetNormalIcon(g_qstrFullKeyBoardSearchIcon);
            pButtonKey->SetPressIcon(g_qstrFullKeyBoardSearchIcon);
        }
        else
        {
            pButtonKey->SetNormalIcon(g_qstrFullKeyBoardEnterIcon);
            pButtonKey->SetPressIcon(g_qstrFullKeyBoardEnterIcon);
        }
    }
}
#endif

bool CInputDialogCustom::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (m_bIsInputBoxShow && PHONE_KEY_INPUTMETHOD_CUSTOM_KEYBOARD != nKeyCode)
    {
        ResetKeyBoard(true);
    }

    return false;
}

bool CInputDialogCustom::event(QEvent* pEvent)
{
    if (NULL != pEvent
            && (pEvent->type() == QEvent::MouseButtonPress
                || pEvent->type() == QEvent::MouseMove
                || pEvent->type() == QEvent::MouseButtonRelease))
    {
        QMouseEvent* pMouseEvent = static_cast<QMouseEvent*>(pEvent);
        if (NULL == pMouseEvent)
        {
            return false;
        }

        ///MainWnd_ResetReturnToIdle();
        //当前点的点位置
        m_nGlobalPos = pMouseEvent->globalPos();

        //如果已经弹出备选框，则先给备选框处理
#ifndef IF_LYNC
        if (m_bIsInputBoxShow)
#else
        if (m_bIfOneButtonDown && m_bIsInputBoxShow)
#endif
        {
            ProcessInputBoxMouseEvent(pMouseEvent);
        }
        else
        {
            ProcessMouseEvent(pMouseEvent);
        }

        return true;
    }

    return QWidget::event(pEvent);
}

bool CInputDialogCustom::ProcessInputBoxMouseEvent(QMouseEvent *pMouseEvent)
{
    if (NULL == pMouseEvent)
    {
        return false;
    }

#ifndef IF_LYNC
    int nType = pMouseEvent->type();
    if ((nType == QEvent::MouseButtonPress))
    {
        //如果弹出扩展，优先扩展处理，如果点击了扩展外其他按键，收起扩展同时响应按键提升体验
        if (!g_InputBoxButton.ProcesseMouseMoveEvent(pMouseEvent))
        {
            m_bIsInputBoxShow = false;
            ProcessMouseEvent(pMouseEvent);
        }
    }
    else if (m_bIsInputBoxShow && QEvent::MouseButtonRelease == nType)
    {
        ResetKeyBoard(false);
    }
#else
    g_InputBoxButton.ProcesseMouseMoveEvent(pMouseEvent);

    int nType = pMouseEvent->type();
    if (m_bIfOneButtonDown && m_bIsInputBoxShow && QEvent::MouseButtonRelease == nType)
    {
        int nIndex = g_InputBoxButton.GetInputBoxHighLight();
        INPUTMETHOD_INFO("CInputDialogCustom::ProcessInputBoxMouseEvent, Get High Light Index = %d", nIndex);

        if (KEYBOARD_INPUTBOX_ERROR_INDEX >= nIndex)
        {
            //如果是KEYBOARD_INPUTBOX_ERROR_INDEX表示没有选中内容
        }
        else
        {
            m_nInputBoxIndex = nIndex;
            OnBtnClick(PHONE_KEY_INPUTMETHOD_CUSTOM_KEYBOARD);
        }

        ResetKeyBoard(false);
    }
#endif
    return true;
}

bool CInputDialogCustom::ProcessMouseEvent(QMouseEvent *pMouseEvent)
{
    if (NULL == pMouseEvent)
    {
        return false;
    }

    int nDownLine = 0;
    int nDownKey = 0;

    int nType = pMouseEvent->type();
    if (nType == QEvent::MouseButtonPress)
    {
        //如果不在键盘内按下，就不处理
        if (!m_GlobleRect.contains(m_nGlobalPos))
        {
            return false;
        }
        //如果已经有按钮按下，不再接受新的按钮按下
        else if (m_bIfOneButtonDown)
        {
            return true;
        }
    }
    else if (nType == QEvent::MouseMove)
    {
        //如果没有记录按下按钮，不需要处理移动
        if (!m_bIfOneButtonDown)
        {
            return false;
        }
        //如果已经弹出了备选框，不需要限制移动
        else if (m_bIsInputBoxShow)
        {
            return false;
        }
    }
    else if (nType == QEvent::MouseButtonRelease)
    {
        //如果没有记录按下按钮，不需要处理抬起
        if (!m_bIfOneButtonDown)
        {
            ResetKeyBoard(true);
            return false;
        }
    }

    //遍历按钮
    vecButtonTable::iterator iterButtonLine = m_listButton.begin();
    for (; iterButtonLine != m_listButton.end(); ++iterButtonLine, ++nDownLine)
    {
        //每一行开始时将KeyID置0
        nDownKey = 0;

        vecButtonLine& tButtonLine = *iterButtonLine;
        vecButtonLine::iterator iterButtonKey = tButtonLine.begin();
        for (; iterButtonKey != tButtonLine.end(); ++iterButtonKey, ++nDownKey)
        {
            CCustomButton* pButtonKey = *iterButtonKey;
            if (NULL == pButtonKey || !pButtonKey->IfButtonAvailable())
            {
                continue;
            }

            if (nType == QEvent::MouseButtonPress)
            {
                if (pButtonKey->InputButtonMousePress(m_nGlobalPos))
                {
                    INPUTMETHOD_INFO("CInputDialogCustom::ProcessMouseEvent, Button Press, Line = %d, Id = %d", nDownLine, nDownKey);
                    //记录该button信息
                    m_bIfOneButtonDown = true;
                    m_nButtonLine = nDownLine;
                    m_nButtonID = nDownKey;
                    pButtonKey->RepaintInputButton();
                    if (pButtonKey->GetIsResponePress())
                    {
                        OnBtnClick(pButtonKey->getIntBtnAction());
                    }
                    else
                    {
                        //此处使用弹出备选框，会在中文字符预输入时导致死机
//                      OnLongPressToBox(false);
                    }
                    return true;
                }
            }
            else if (nType == QEvent::MouseMove)
            {
                //一旦拖动移动到另一个按钮，则重置键盘
                if (pButtonKey->InputButtonMove(m_nGlobalPos))
                {
                    //如果新的move后已经和原按钮不是同一个按钮，则直接重置键盘，包括长按备选框
                    if ((nDownLine != m_nButtonLine || nDownKey != m_nButtonID))
                    {
                        INPUTMETHOD_INFO("CInputDialogCustom::ProcessMouseEvent, Button Move, Line = %d, Id = %d", nDownLine, nDownKey);
                        ResetKeyBoard(true);
                    }
                    return true;
                }
            }
            else if (nType == QEvent::MouseButtonRelease)
            {
                if (pButtonKey->InputButtonMouseRelease(m_nGlobalPos))
                {
                    INPUTMETHOD_INFO("CInputDialogCustom::ProcessMouseEvent, Button Release, Line = %d, Id = %d", nDownLine, nDownKey);
                    //如果抬起的点与按下的点是在同一个按钮内，则执行该按钮
                    if (m_nButtonLine == nDownLine
                            && m_nButtonID == nDownKey)
                    {
                        if (!pButtonKey->GetIsResponePress())
                        {
                            OnBtnClick(pButtonKey->getIntBtnAction());
                        }
                    }
                    //不管是否有效按下，都需要重置按下状态
                    ResetKeyBoard(false);

                    return true;
                }
            }
        }
    }
    //如果走完整个循环都没有按钮处理，则是点到空白处了
    ResetKeyBoard(true);

    return false;
}

void CInputDialogCustom::paintEvent(QPaintEvent* pe)
{
    QStylePainter p(this);

    //画背景
    if (!m_qCurrPixMap.isNull())
    {
        p.drawPixmap(0, 0, m_qCurrPixMap);
    }

    //依次画回车按钮
    vecButtonLine::iterator iterButtonEnterKey = m_listEnterButton.begin();
    for (; iterButtonEnterKey != m_listEnterButton.end(); ++iterButtonEnterKey)
    {
        CCustomButton* pButtonKey = *iterButtonEnterKey;
        if (NULL == pButtonKey)
        {
            continue;
        }
        paintButtonBackground(p, pButtonKey);
        paintButtonIcon(p, pButtonKey);
        paintButtonText(p, pButtonKey);
    }

    //依次画ime切换键
    vecButtonLine::iterator iterButtonImeKey = m_listImeButton.begin();
    for (; iterButtonImeKey != m_listImeButton.end(); ++iterButtonImeKey)
    {
        CCustomButton* pButtonKey = *iterButtonImeKey;
        if (NULL == pButtonKey)
        {
            continue;
        }
        paintButtonText(p, pButtonKey);
    }

    //只需要画按下的那一个按钮就可以
    if (m_bIfOneButtonDown
            && m_nButtonLine > -1
            && m_nButtonLine < m_listButton.size()
            && m_nButtonID > -1
            && m_nButtonID < (m_listButton[m_nButtonLine]).size())
    {
        CCustomButton* pInputButton = m_listButton[m_nButtonLine][m_nButtonID];
        //enter按钮已经画过
        if (NULL != pInputButton /*&& m_pInputButtonEnter != pInputButton*/)
        {
            paintButtonBackground(p, pInputButton, true);
            paintButtonIcon(p, pInputButton);
            paintButtonText(p, pInputButton);
        }
    }
}

void CInputDialogCustom::ResetKeyBoard(bool bClearInputBox)
{
#ifndef IF_LYNC
    //使用局部刷新即可，恢复上次按下的按键
    //检查按下按钮的index是否合法
    if (m_nButtonLine > -1
            || m_nButtonLine < m_listButton.size()
            || m_nButtonID > -1
            || m_nButtonID < (m_listButton[m_nButtonLine]).size())
    {
        CCustomButton* pButtonKey = m_listButton[m_nButtonLine][m_nButtonID];
        if (NULL != pButtonKey)
        {
            pButtonKey->ResetInputButtonDown();
            pButtonKey->RepaintInputButton();
        }
    }
#else
    vecButtonTable::iterator iterButtonLine = m_listButton.begin();
    for (; iterButtonLine != m_listButton.end(); ++iterButtonLine)
    {
        vecButtonLine& tButtonLine = *iterButtonLine;
        vecButtonLine::iterator iterButtonKey = tButtonLine.begin();
        for (; iterButtonKey != tButtonLine.end(); ++iterButtonKey)
        {
            CCustomButton* pButtonKey = *iterButtonKey;
            if (NULL == pButtonKey)
            {
                continue;
            }
            pButtonKey->ResetInputButtonDown();
        }
    }
#endif

    //清除button信息
    m_bIfOneButtonDown = false;
#ifndef IF_LYNC
    if (bClearInputBox)
    {
        m_nButtonLine = -1;
        m_nButtonID = -1;
        m_bIsInputBoxShow = false;
        m_nInputBoxIndex = KEYBOARD_INPUTBOX_ERROR_INDEX;
        g_InputBoxButton.HideInputBox();
        g_InputBoxButton.ResetInputBox();
    }
#else
    m_nButtonLine = -1;
    m_nButtonID = -1;
    m_bIsInputBoxShow = false;
    m_nInputBoxIndex = KEYBOARD_INPUTBOX_ERROR_INDEX;
    g_InputBoxButton.HideInputBox();
    g_InputBoxButton.ResetInputBox();
#endif

    g_KeyBoardImeManager.ResetSelectedChar();

    //update();

    INPUTMETHOD_INFO("CInputDialogCustom::ResetKeyBoard");
}

void CInputDialogCustom::OnLongPressToBox(bool bShowOption)
{
    INPUTMETHOD_INFO("CInputDialogCustom::OnLongPressToBox, Show Option = %d", bShowOption);

    //如果没有已经按下的按钮则不弹框
    //如果已经弹出备选框则不再弹
    if (!m_bIfOneButtonDown
            || (m_bIsInputBoxShow && !bShowOption))
    {
        return;
    }

    //检查按下按钮的index是否合法
    if (m_nButtonLine < 0
            || m_nButtonLine >= m_listButton.size()
            || m_nButtonID < 0
            || m_nButtonID >= (m_listButton[m_nButtonLine]).size())
    {
        return;
    }

    CCustomButton* pButtonKey = m_listButton[m_nButtonLine][m_nButtonID];
    if (NULL == pButtonKey)
    {
        return;
    }

    vecInputBoxString vecC;
    int nBoxType = INPUT_BOX_HORIZONTAL_TYPE;
    yl::string strKeyFuntion = "";
    //如果判断结果是不需要弹，则直接返回掉
    if (!SetCharToInputButton(vecC, bShowOption, strKeyFuntion))
    {
        return;
    }

    //置弹出状态
    m_bIsInputBoxShow = true;

    QRect rctButton = pButtonKey->m_rctButton;
    QPoint pointBtnLT = QPoint(rctButton.left() + g_iFullKeyBoardX, rctButton.top() + g_iFullKeyBoardY);

    if (!strKeyFuntion.empty() && INPUT_IME_FUNCTION_LANG == strKeyFuntion)
    {
        int nIndex = g_KeyBoardSwitchManager.GetCurrentLang();
        g_InputBoxButton.SetInputBox(pointBtnLT, vecC, INPUT_BOX_VERTICAL_TYPE, nIndex);
    }
    else
    {
        g_InputBoxButton.SetInputBox(pointBtnLT, vecC, INPUT_BOX_HORIZONTAL_TYPE, 0);
    }
    g_InputBoxButton.ShowInputBox();

#ifndef IF_LYNC
    //此处应传入当前按下的鼠标位置
    g_InputBoxButton.JudgeIndexByMousePosition(m_nGlobalPos);
#endif
}

void CInputDialogCustom::OnInputBoxPress()
{
    if (m_bIsInputBoxShow)
    {
        int nIndex = g_InputBoxButton.GetInputBoxHighLight();
        INPUTMETHOD_INFO("CInputDialogCustom::ProcessInputBoxMouseEvent, Get High Light Index = %d", nIndex);

        if (KEYBOARD_INPUTBOX_ERROR_INDEX >= nIndex)
        {
            //如果是KEYBOARD_INPUTBOX_ERROR_INDEX表示没有选中内容
        }
        else
        {
            m_nInputBoxIndex = nIndex;
            OnBtnClick(PHONE_KEY_INPUTMETHOD_CUSTOM_KEYBOARD);
        }

        ResetKeyBoard(true);
    }
}

bool CInputDialogCustom::SetCharToInputButton(vecInputBoxString& vecC, bool bShowOption, yl::string& strKeyFuntion)
{
    const SKeyboardIme* pKeyboardIme = g_KeyBoardImeManager.GetImeByIndex(m_nImeIndex);
    if (NULL == pKeyboardIme)
    {
        return false;
    }

    const vecKeyboardImeKeyTable& tvecImeKeyTable = pKeyboardIme->m_vecImeKey;
    if (m_nButtonLine < 0 || m_nButtonLine >= tvecImeKeyTable.size())
    {
        return false;
    }

    const vecKeyboardImeKeyLine& tImeKeyLine = tvecImeKeyTable[m_nButtonLine];
    if (m_nButtonID < 0 || m_nButtonID >= tImeKeyLine.size())
    {
        return false;
    }

    vecC.clear();

    const SKeyBoardImeKey& tImeKey = tImeKeyLine[m_nButtonID];

    strKeyFuntion = tImeKey.m_strFunction;

    //长按Lang功能键，弹出语言切换框
    if (INPUT_IME_FUNCTION_LANG == tImeKey.m_strFunction && bShowOption)
    {
        const vecKeyboardLang& tvecKeyboardLang = g_KeyBoardLangManager.GetKeyboardLang();
        if (tvecKeyboardLang.size() <= 0)
        {
            return false;
        }

        //只有一个备选也需弹出
        vecKeyboardLang::const_iterator iterLang = tvecKeyboardLang.begin();
        for (; iterLang != tvecKeyboardLang.end(); ++iterLang)
        {
            vecC.push_back(toQString((*iterLang).m_strLangName));
        }
        return true;
    }
    //长按Switch功能键，弹出键盘切换框
    else if (INPUT_IME_FUNCTION_SWITCH == tImeKey.m_strFunction && bShowOption)
    {
        const vecKeyboardLang& tvecKeyboardLang = g_KeyBoardLangManager.GetKeyboardLang();
        const vecKeyboardImeIndex& tvecKeyboardImeIndex = tvecKeyboardLang[g_KeyBoardSwitchManager.GetCurrentLang()].m_vecImeIndex;
        if (tvecKeyboardImeIndex.size() <= 0)
        {
            return false;
        }
        //只有一个备选也需弹出
        vecKeyboardImeIndex::const_iterator iterImeIndex = tvecKeyboardImeIndex.begin();
        for (; iterImeIndex != tvecKeyboardImeIndex.end(); ++iterImeIndex)
        {
            const SKeyboardIme* pKeyboardImeToBox = g_KeyBoardImeManager.GetImeByIndex((*iterImeIndex));
            if (NULL == pKeyboardImeToBox)
            {
                continue;
            }
            vecC.push_back(toQString(pKeyboardImeToBox->m_strDisplayName));
        }
        return true;
    }
    //其他功能按键不需要支持长按弹出
    else if (!tImeKey.m_strFunction.empty())
    {
        return false;
    }
    //其他非功能键，弹出字符备选框
    else
    {
        QString strList = toQString(tImeKey.m_strImeList);
        int nLength = strList.size();
        // 只有一个备选暂时弹出
        if (nLength <= 1)
        {
            return false;
        }

        for (int i = 0; i < nLength; ++i)
        {
            vecC.push_back(strList.at(i));
            if (!bShowOption)
            {
                return true;
            }
        }

        return true;
    }

    return false;
}

#ifdef IF_ENABLE_TESTABILITY
QString CInputDialogCustom::GetTestInfo()
{
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    // 提供所有function key
    vecButtonLine::iterator iterButton = m_listFunctionButton.begin();
    for (; iterButton != m_listFunctionButton.end(); ++iterButton)
    {
        CCustomButton* pButtonKey = *iterButton;
        if (NULL == pButtonKey || pButtonKey->GetFunctionName().isEmpty())
        {
            continue;
        }

        ptAbsolute = mapToGlobal(pButtonKey->m_rctButton.topLeft());
        testInfo.x_absolute = ptAbsolute.x();
        testInfo.y_absolute = ptAbsolute.y();
        testInfo.width = pButtonKey->m_rctButton.width();
        testInfo.height = pButtonKey->m_rctButton.height();
        testInfo.id = fromQString(pButtonKey->GetFunctionName());
        testInfo.text = fromQString(pButtonKey->GetFunctionName());
        testInfo.type = VIRTUAL_BTN;
        AddNode(nodeRoot, testInfo);
    }

    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);
    return strTestinfo;
}
#endif
