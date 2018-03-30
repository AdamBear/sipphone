#include "numberpad.h"
#include "talkui_define.h"
#include "baseui/t4xpicpath.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/languagehelper.h"
#include "inputmethod/inputmethod.h"

//////////////////////////////////////////////////////////////////////////
qIMEButton::qIMEButton(QWidget * parent/* = 0*/)
    : qUIPushButton(parent)
{

}

qIMEButton::~qIMEButton()
{

}

void qIMEButton::paintEvent(QPaintEvent * paintEvent)
{
    QStylePainter p(this);
    QStyleOptionButton option;
    initStyleOption(&option);
    QRect rcText = option.rect;

    QFont fontTitle = font();
    fontTitle.setPointSize(24);
    p.setFont(fontTitle);
    QRect rcUpdated(option.rect.x(), option.rect.y(), option.rect.width(),
                    option.rect.height() * 2 / 3);
    p.drawItemText(rcUpdated, Qt::AlignCenter, option.palette, this->isEnabled(), this->text());

    // IME按钮下方字体颜色为灰色
    rcText.setY(rcText.y()  + option.rect.height() * 4 / 7);
    fontTitle.setPointSize(16);
    p.setFont(fontTitle);
//  QPen pen = p.pen();
//  pen.setColor(QColor(128, 128, 128));
//  p.setPen(pen);

    // 绘制Item的Text
    p.drawItemText(rcText, Qt::AlignCenter, option.palette, this->isEnabled(),
                   LANG_TRANSLATE(TRID_INPUT_METHOD_SHORT));

    // 所有东西绘制完后,才绘制上遮罩
    if (this->isDown()
            || this->isChecked())
    {
        QPixmap pmMask = THEME_GET_BMP(m_strMaskPic.toUtf8().data());
        if (!pmMask.isNull())
        {
            p.drawPixmap(option.rect.x() + (option.rect.width() - pmMask.width()) / 2,
                         option.rect.y() + (option.rect.height() - pmMask.height()) / 2, pmMask);
        }
    }
}

qKeypadButton::qKeypadButton(QWidget *parent/* = 0*/)
    : qUIPushButton(parent)
{

}

qKeypadButton::~qKeypadButton()
{

}

void qKeypadButton::paintEvent(QPaintEvent *paintEvent)
{
    QStylePainter p(this);
    QStyleOptionButton option;
    initStyleOption(&option);
    QRect rcText = option.rect;

    QRect rcUpdated(option.rect.x(), option.rect.y(), option.rect.width(), option.rect.height() * 2 / 3);
    QPixmap pmKeyPad = THEME_GET_BMP(PIC_WIDGET_DIAL_SHOW_KEYPAD);
    if (!pmKeyPad.isNull())
    {
        p.drawPixmap(rcUpdated.x() + (rcUpdated.width() - pmKeyPad.width()) / 2,
                     option.rect.y() + (rcUpdated.height() - pmKeyPad.height()) / 2, pmKeyPad);
    }

    // IME按钮下方字体颜色为灰色
    rcText.setY(rcText.y() + option.rect.height() * 4 / 7);
    QFont fontTitle = font();
    fontTitle.setPointSize(20);
    p.setFont(fontTitle);
    p.drawItemText(rcText, Qt::AlignCenter, option.palette, this->isEnabled(), LANG_TRANSLATE(TRID_KEY_BOARD));

    // 所有东西绘制完后,才绘制上遮罩

    if (this->isDown()
            || this->isChecked())
    {
        QPixmap pmMask = THEME_GET_BMP(m_strMaskPic.toUtf8().data());
        if (!pmMask.isNull())
        {
            p.drawPixmap(option.rect.x() + (option.rect.width() - pmMask.width()) / 2,
                         option.rect.y() + (option.rect.height() - pmMask.height()) / 2, pmMask);
        }
    }
}

NumberPad::NumberPad(QWidget * parent /* =0 */):
    QWidget(parent)
{

    setObjectName("NumberPad");

    CDialButton ** btn[12] =
    {
        &m_btnOne,
        &m_btnTwo,
        &m_btnThree,
        &m_btnFour,
        &m_btnFive,
        &m_btnSix,
        &m_btnSeven,
        &m_btnEight,
        &m_btnNine,
        &m_btnStar,
        &m_btnZero,
        &m_btnPound
    };

    //QString strs[12][2] = {
    //    {QString("1"), QString()},
    //    {QString("2"), QString("ABC")},
    //    {QString("3"), QString("DEF")},
    //    {QString("4"), QString("GHI")},
    //    {QString("5"), QString("JKL")},
    //    {QString("6"), QString("MNO")},
    //    {QString("7"), QString("PQRS")},
    //    {QString("8"), QString("TUV")},
    //    {QString("9"), QString("WXYZ")},
    //    {QString("*"), QString(".")},
    //    {QString("0"), QString("")},
    //    {QString("#"), QString("SEND")},
    //};

    bool enables[12][2] =
    {
        {false, true},
        {false, true},
        {false, true},
        {false, true},
        {false, true},
        {false, true},
        {false, true},
        {false, true},
        {false, true},
        {false, true},
        {false, true},
        {false, true}
    };

    int key_array[12] =
    {

        PHONE_KEY_1,
        PHONE_KEY_2,
        PHONE_KEY_3,
        PHONE_KEY_4,
        PHONE_KEY_5,
        PHONE_KEY_6,
        PHONE_KEY_7,
        PHONE_KEY_8,
        PHONE_KEY_9,
        PHONE_KEY_STAR,
        PHONE_KEY_0,
        PHONE_KEY_POUND
    };


    int index = 0;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            (*btn[index]) = new CDialButton(this);
            (*btn[index])->setObjectName(QString("DialButton%1").arg(index));
            (*btn[index])->setTextColor(QColor(40, 49, 58));
            (*btn[index])->setGeometry(QRect(j * 129, i * 68, 126, 67));
            /////----优化界面速度暂时注释
            //(*btn[index])->SetText(strs[index][0], strs[index][1]);
            (*btn[index])->SetFontSize(30, 14);
            (*btn[index])->SetMargin(28);
            (*btn[index])->SetMarginBetweenDigitAndLetters(10);
            (*btn[index])->SetMask(PIC_WIDGET_DIAL_NUMBER_MASK);
            (*btn[index])->EnableAutoMargin(enables[index][0], enables[index][1]);
            (*btn[index])->Init();
            (*btn[index])->SetKey(key_array[index]);

            connect((*btn[index]), SIGNAL(BtnPressed(int)), this, SLOT(OnPressedKey(int)));
            connect((*btn[index]), SIGNAL(BtnReleased(int)), this, SLOT(OnReleasedkey(int)));

            index++;
        }
    }

    (*btn[9])->SetFontSize(36, 12);
}

NumberPad::~NumberPad()
{

}

void NumberPad::OnPressedKey(int  key)
{
    TALKUI_INFO("NumberPad::OnPressedKey [%d]", key);
    inputHook_SendKey(key, true);
}

void NumberPad::OnReleasedkey(int  key)
{
    TALKUI_INFO("NumberPad::OnReleasedkey [%d]", key);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
DialPad::DialPad(QWidget * parent /* = 0 */):
    QWidget(parent)
{
    /////----优化界面速度暂时注释
    //qWidgetUtility::SetBackGround(this, PIC_WIDGET_DIAL_PAD);
    setObjectName("DialPad");
    m_NumberPad.setParent(this);
    m_NumberPad.setGeometry(QRect(3, 0, 385, 273));

    m_btnDial = new qUIPushButton(this);
    if (NULL != m_btnDial)
    {
        m_btnDial->SetMask(PIC_WIDGET_DIAL_SEND_BTN_MASK);
        m_btnDial->setGeometry(QRect(390, 0, 126, 138));
        m_btnDial->set3StatePic(PIC_WIDGET_DIAL_SEND_BTN_BACK,
                                PIC_WIDGET_DIAL_SEND_BTN_BACK,
                                PIC_WIDGET_DIAL_SEND_BTN_BACK);
        m_btnDial->setBtnAction(TALK_UI_ACTION_SEND);
        m_btnDial->setTextColor(Qt::white);
        m_btnDial->setIconPicAlign(Qt::AlignTop);
        m_btnDial->setIconTopOffSet(45);
        m_btnDial->setTextYOffSet(18);
        m_btnDial->setObjectName("btnDial");

        QFont fontBtn = m_btnDial->font();
        fontBtn.setPointSize(19);
        m_btnDial->setFont(fontBtn);

        connect(m_btnDial, SIGNAL(clicked(QString)), this, SIGNAL(BtnClicked(QString)));
    }

    m_btnTransSend = new qUIPushButton(this);
    if (NULL != m_btnTransSend)
    {
        m_btnTransSend->SetMask(PIC_WIDGET_DIAL_SEND_BTN_MASK_SMALL);
        m_btnTransSend->setGeometry(QRect(390, 0, 126, 68));
        m_btnTransSend->set3StatePic(PIC_WIDGET_DIAL_SEND_BTN_BACK_SMALL,
                                     PIC_WIDGET_DIAL_SEND_BTN_BACK_SMALL,
                                     PIC_WIDGET_DIAL_SEND_BTN_BACK_SMALL);
        m_btnTransSend->setBtnAction(TALK_UI_ACTION_SEND);
        m_btnTransSend->setTextColor(Qt::white);
        m_btnTransSend->setIconPicAlign(Qt::AlignLeft);
        m_btnTransSend->setTextAlign(Qt::AlignLeft | Qt::AlignVCenter);
        m_btnTransSend->setIconLeftOffSet(23);
        m_btnTransSend->setTextXOffSet(2);
        m_btnTransSend->setObjectName("btnTransfer");
        m_btnTransSend->setVisible(false);

        QFont fontBtn = m_btnTransSend->font();
        fontBtn.setPointSize(19);
        m_btnTransSend->setFont(fontBtn);

        connect(m_btnTransSend, SIGNAL(clicked(QString)), this, SIGNAL(BtnClicked(QString)));
    }

    m_btnIME = new qIMEButton(this);
    if (NULL != m_btnIME)
    {
        m_btnIME->setObjectName("btnIME");
        m_btnIME->set3StatePic("", "", "");
        m_btnIME->setGeometry(QRect(390, 138, 126, 65));
        m_btnIME->setBtnAction(TALK_UI_ACTION_IME);
        m_btnIME->SetMask(PIC_WIDGET_DIAL_NUMBER_MASK);
        m_btnIME->setTextColor(QColor(40, 49, 58));
        connect(m_btnIME, SIGNAL(clicked(QString)), this, SIGNAL(BtnClicked(QString)));
    }

#if IF_BUG_25055
    m_btnExtension = new qUIPushButton(this);
    if (NULL != m_btnExtension)
    {
        m_btnExtension->setObjectName("btnExtension");
        m_btnExtension->set3StatePic("", "", "");
        m_btnExtension->setGeometry(QRect(390, 138, 126, 65));
        m_btnExtension->setBtnAction("");
        m_btnExtension->SetMask(PIC_WIDGET_DIAL_NUMBER_MASK);
        m_btnExtension->setTextColor(QColor(40, 49, 58));
        connect(m_btnExtension, SIGNAL(clicked(QString)), this, SIGNAL(BtnClicked(QString)));
    }
#endif

    m_btnDsskey = new qUIPushButton(this);
    if (NULL != m_btnDsskey)
    {
        m_btnDsskey->setObjectName("btnDsskey");
        m_btnDsskey->setGeometry(QRect(390, 205, 126, 65));
        m_btnDsskey->setTextColor(QColor(40, 49, 58));
        m_btnDsskey->setBtnAction(TALK_UI_ACTION_OPENDSSKEY);
        m_btnDsskey->setIconPic(PIC_DIAL_DSS_PAGE_EXPAND);
        m_btnDsskey->setTextXOffSet(5);
#if IF_BUG_30875
        m_btnDsskey->setText(LANG_TRANSLATE(TRID_MORE));
#else
        m_btnDsskey->setText(LANG_TRANSLATE(TRID_DSS_KEY));
#endif
        m_btnDsskey->setIconLeftOffSet(12);
        m_btnDsskey->setIconPicAlign(Qt::AlignLeft);
        m_btnDsskey->setTextAlign(Qt::AlignLeft | Qt::AlignVCenter);
        m_btnDsskey->SetMask(PIC_WIDGET_DIAL_NUMBER_MASK);
        connect(m_btnDsskey, SIGNAL(clicked(QString)), this, SIGNAL(BtnClicked(QString)));
    }
}

DialPad::~DialPad()
{

}

void DialPad::SetDialBtnIcon(const QString & strIcon)
{
    if (NULL != m_btnDial)
    {
        m_btnDial->setIconPic(strIcon);
    }
}

void DialPad::SetDialBtnAction(const QString & strAction)
{
    if (NULL != m_btnDial)
    {
        m_btnDial->setBtnAction(strAction);
    }
}

void DialPad::SetDialBtnText(const QString & strText)
{
    if (NULL != m_btnDial)
    {
        m_btnDial->setText(strText);
    }
}

void DialPad::SetTransSendBtnIcon(const QString & strIcon)
{
    if (NULL != m_btnTransSend)
    {
        m_btnTransSend->setIconPic(strIcon);
    }
}

void DialPad::SetTransSendBtnAction(const QString & strAction)
{
    if (NULL != m_btnTransSend)
    {
        m_btnTransSend->setBtnAction(strAction);
    }
}

void DialPad::SetTransSendBtnText(const QString & strText)
{
    if (NULL != m_btnTransSend)
    {
        m_btnTransSend->setText(strText);
    }
}

void DialPad::SetIMEBtnText(const QString & strText)
{
    if (NULL != m_btnIME)
    {
        m_btnIME->setText(strText);
    }
}

#if IF_BUG_25055
// 是否需要IME按钮
void DialPad::SetIMEBtnVisible(bool bVisible)
{
    if (NULL != m_btnIME)
    {
        m_btnIME->setVisible(bVisible);
    }
}

void DialPad::SetExtensionBtnText(const QString & strText)
{
    if (NULL != m_btnExtension)
    {
        m_btnExtension->setText(strText);
    }
}

void DialPad::SetExtensionBtnAction(const QString & strAction)
{
    if (NULL != m_btnExtension)
    {
        m_btnExtension->setBtnAction(strAction);
    }
}

void DialPad::SetExtensionBtnVisible(bool bVisible)
{
    if (NULL != m_btnExtension)
    {
        m_btnExtension->setVisible(bVisible);
    }
}
#endif

void DialPad::SetTransferState(bool bBtnVisible)
{
    if (NULL == m_btnDial)
    {
        return;
    }

    // change dialBtn geometry
    if (bBtnVisible)
    {
        m_btnDial->setGeometry(QRect(390, 68, 126, 68));
        m_btnDial->SetMask(PIC_WIDGET_DIAL_SEND_BTN_MASK_SMALL);
        m_btnDial->set3StatePic(PIC_WIDGET_DIAL_SEND_BTN_BACK_SMALL,
                                PIC_WIDGET_DIAL_SEND_BTN_BACK_SMALL,
                                PIC_WIDGET_DIAL_SEND_BTN_BACK_SMALL);
        m_btnDial->setIconPicAlign(Qt::AlignLeft);
        m_btnDial->setTextAlign(Qt::AlignLeft | Qt::AlignVCenter);
        m_btnDial->setIconLeftOffSet(4);
        m_btnDial->setTextXOffSet(2);
    }
    else
    {
        m_btnDial->setGeometry(QRect(390, 0, 126, 138));
        m_btnDial->SetMask(PIC_WIDGET_DIAL_SEND_BTN_MASK);
        m_btnDial->set3StatePic(PIC_WIDGET_DIAL_SEND_BTN_BACK,
                                PIC_WIDGET_DIAL_SEND_BTN_BACK,
                                PIC_WIDGET_DIAL_SEND_BTN_BACK);
        m_btnDial->setIconPicAlign(Qt::AlignTop);
        m_btnDial->setTextAlign(Qt::AlignCenter);
        m_btnDial->setIconTopOffSet(45);
        m_btnDial->setTextYOffSet(18);
    }

    if (NULL != m_btnTransSend)
    {
        m_btnTransSend->setVisible(bBtnVisible);
    }
}

void DialPad::HideIMEBtn()
{
    if (m_btnIME != NULL)
    {
        m_btnIME->hide();
    }
}

KeyPadDialPad::KeyPadDialPad(QWidget * parent/* = 0*/)
    : DialPad(parent)
    , m_btnKeypad(NULL)
{
    if (FullKeyboardEnabled())
    {
        //初始化全键盘按钮
        m_btnKeypad = new qKeypadButton(this);

        if (NULL != m_btnKeypad)
        {
            m_btnKeypad->setObjectName("btnKeypad");
            m_btnKeypad->set3StatePic("", "", "");
            m_btnKeypad->setGeometry(QRect(390, 138, 126, 65));
            m_btnKeypad->setBtnAction(DIALPAD_SHOW_KEYPAD);
            m_btnKeypad->SetMask(PIC_WIDGET_DIAL_NUMBER_MASK);
            m_btnKeypad->setTextColor(QColor(40, 49, 58));
            connect(m_btnKeypad, SIGNAL(clicked(QString)), this, SIGNAL(BtnClicked(QString)));
        }

        //隐藏IME按钮
        HideIMEBtn();
    }
}

KeyPadDialPad::~KeyPadDialPad()
{

}
