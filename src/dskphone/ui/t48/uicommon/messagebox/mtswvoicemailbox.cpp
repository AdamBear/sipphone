#include "mtswvoicemailbox.h"
#if IF_FEATURE_METASWITCH_VOICEMAIL
#include "commonunits/modcommonunits.h"
#include "keymap.h"
#include "globalinputhook/modinputhook.h"
#include "imagemanager/modimagemanager.h"
#include "inputmethod/inputmethod.h"
#include "mainwnd/mainwnd.h"
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uikernel/languagehelper.h"
#include "uikernel/quipushbutton.h"
#include "uikernel/qline.h"
#include "uikernel/qcheckbox.h"
#include "uicommon.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "baseui/t4xpicpath.h"
#include <QtGui>

#define MTSWVOICEMAIL_BOX_LEFT 222
#define MTSWVOICEMAIL_BOX_TOP 46
#define MTSWVOICEMAIL_BOX_WIDTH 357
#define MTSWVOICEMAIL_BOX_HEIGHT 364
#define MTSWVOICEMAIL_BOX_HEIGHT_2 303

#define MTSWVOICEMAIL_BOX_TITLE_HEIGHT 50

#define MTSWVOICEMAIL_BOX_BUTTON_LEFT 65
#define MTSWVOICEMAIL_BOX_BUTTON_WIDTH 227
#define MTSWVOICEMAIL_BOX_BUTTON_HEIGHT 53

#define MTSWVOICEMAIL_BOX_BUTTON_TOP_1 58
#define MTSWVOICEMAIL_BOX_BUTTON_TOP_2 117
#define MTSWVOICEMAIL_BOX_BUTTON_TOP_3 178
#define MTSWVOICEMAIL_BOX_BUTTON_TOP_4 239
#define MTSWVOICEMAIL_BOX_BUTTON_TOP_5 300

#define MTSWVOICEMAIL_BOX_FONT 16

#define RECT_POPUP_MTSWVOICEMAIL                QRect(MTSWVOICEMAIL_BOX_LEFT, MTSWVOICEMAIL_BOX_TOP, MTSWVOICEMAIL_BOX_WIDTH, MTSWVOICEMAIL_BOX_HEIGHT)
#define RECT_POPUP_MTSWVOICEMAIL_2              QRect(MTSWVOICEMAIL_BOX_LEFT, MTSWVOICEMAIL_BOX_TOP, MTSWVOICEMAIL_BOX_WIDTH, MTSWVOICEMAIL_BOX_HEIGHT_2)
#define RECT_POPUP_MTSWVOICEMAIL_BTN_1          QRect(MTSWVOICEMAIL_BOX_BUTTON_LEFT, MTSWVOICEMAIL_BOX_BUTTON_TOP_1, MTSWVOICEMAIL_BOX_BUTTON_WIDTH, MTSWVOICEMAIL_BOX_BUTTON_HEIGHT)
#define RECT_POPUP_MTSWVOICEMAIL_BTN_2          QRect(MTSWVOICEMAIL_BOX_BUTTON_LEFT, MTSWVOICEMAIL_BOX_BUTTON_TOP_2, MTSWVOICEMAIL_BOX_BUTTON_WIDTH, MTSWVOICEMAIL_BOX_BUTTON_HEIGHT)
#define RECT_POPUP_MTSWVOICEMAIL_BTN_3          QRect(MTSWVOICEMAIL_BOX_BUTTON_LEFT, MTSWVOICEMAIL_BOX_BUTTON_TOP_3, MTSWVOICEMAIL_BOX_BUTTON_WIDTH, MTSWVOICEMAIL_BOX_BUTTON_HEIGHT)
#define RECT_POPUP_MTSWVOICEMAIL_BTN_4          QRect(MTSWVOICEMAIL_BOX_BUTTON_LEFT, MTSWVOICEMAIL_BOX_BUTTON_TOP_4, MTSWVOICEMAIL_BOX_BUTTON_WIDTH, MTSWVOICEMAIL_BOX_BUTTON_HEIGHT)
#define RECT_POPUP_MTSWVOICEMAIL_BTN_5          QRect(MTSWVOICEMAIL_BOX_BUTTON_LEFT, MTSWVOICEMAIL_BOX_BUTTON_TOP_5, MTSWVOICEMAIL_BOX_BUTTON_WIDTH, MTSWVOICEMAIL_BOX_BUTTON_HEIGHT)

CMTSWVoiceMailBox::CMTSWVoiceMailBox(CMaskFrame * pMaskFrame /* = 0 */)
    : CMessageBoxBase(pMaskFrame)
    , m_pBtnDelete(NULL)
    , m_pBtnMark(NULL)
    , m_pBtnCallBack(NULL)
    , m_pBtnAddToLocal(NULL)
    , m_pBtnAddToNetwork(NULL)
    , m_pLabelTitle(NULL)
    , m_bRead(true)
    , m_bAddToNetwork(true)
{

}

CMTSWVoiceMailBox::~CMTSWVoiceMailBox()
{
}

/*************************************************/
/*  按照msgbox的类型设置不同的msgbox
/*  lpszText为正文内容
/*  timeTodie为生存时间，单位为s，-1表示无限制
/*************************************************/
void CMTSWVoiceMailBox::SetMessageBoxInfo(const QString & lpszText /* = "" */,
        int timeTodie /* = -1 */)
{

    QRect rect = RECT_POPUP_MTSWVOICEMAIL;
    if (!m_bAddToNetwork)
    {
        rect = RECT_POPUP_MTSWVOICEMAIL_2;
    }
    this->setGeometry(rect);

    //设置背景
    setAutoFillBackground(true);
    SetMessageBoxBackground(PIC_BLUE_POPUP);

    m_pBtnDelete = new qUIPushButton(this);
    m_pBtnCallBack = new qUIPushButton(this);
    m_pBtnMark = new qUIPushButton(this);
    m_pBtnAddToLocal = new qUIPushButton(this);
    m_pBtnAddToNetwork = new qUIPushButton(this);
    m_pLabelTitle = new QLabel(this);

    if (NULL == m_pBtnDelete || NULL == m_pBtnCallBack
            || NULL == m_pBtnMark || NULL == m_pBtnAddToLocal
            || NULL == m_pBtnAddToNetwork || NULL == m_pLabelTitle)
    {
        return;
    }

    m_pBtnDelete->setObjectName("BtnDelete");
    m_pBtnCallBack->setObjectName("BtnCallBack");
    m_pBtnMark->setObjectName("BtnMark");
    m_pBtnAddToLocal->setObjectName("BtnAddToLocal");
    m_pBtnAddToNetwork->setObjectName("BtnAddToNetwork");
    m_pLabelTitle->setObjectName("LabelTitle");

    QFont font;
    font.setPointSize(MTSWVOICEMAIL_BOX_FONT);
    font.setWeight(QFont::Light);
    font.setBold(false);

    // set title
    m_pLabelTitle->setText(LANG_TRANSLATE(TRID_OPTION));
    m_pLabelTitle->setAlignment(Qt::AlignCenter);       //设置文字显示方式，正文居中显示
    m_pLabelTitle->setWordWrap(false);
    QPalette palette = m_pLabelTitle->palette();
    palette.setColor(QPalette::WindowText, Qt::white);
    m_pLabelTitle->setPalette(palette);

    if ("" != lpszText)
    {
        m_pLabelTitle->setText(lpszText);
        QFontMetrics fm(m_pLabelTitle->font());
        int nDiff = MTSWVOICEMAIL_BOX_WIDTH - fm.width(lpszText) - MTSWVOICEMAIL_BOX_FONT;
        if (nDiff > 0)
        {
            m_pLabelTitle->resize(fm.width(lpszText) + MTSWVOICEMAIL_BOX_FONT, m_pLabelTitle->height());
        }
        else
        {
            m_pLabelTitle->resize(MTSWVOICEMAIL_BOX_WIDTH - MTSWVOICEMAIL_BOX_FONT, m_pLabelTitle->height());
        }
    }

    m_pLabelTitle->setGeometry((MTSWVOICEMAIL_BOX_WIDTH - m_pLabelTitle->width()) >> 1,
                               (MTSWVOICEMAIL_BOX_TITLE_HEIGHT - m_pLabelTitle->height()) >> 1,
                               m_pLabelTitle->width(),
                               m_pLabelTitle->height());

    //CallBack
    m_pBtnCallBack->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    m_pBtnCallBack->setText(LANG_TRANSLATE(TRID_SEND));
    QObject::connect(m_pBtnCallBack, SIGNAL(clicked()), this, SLOT(OnBtnCallBackClick()));
    m_pBtnCallBack->setGeometry(RECT_POPUP_MTSWVOICEMAIL_BTN_1);
    m_pBtnCallBack->show();
    m_pBtnCallBack->setFont(font);

    //Mark
    m_pBtnMark->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    if (m_bRead)
    {
        m_pBtnMark->setText(LANG_TRANSLATE(TRID_MTSW_VOICEMAIL_OPTION_MARK_NEW));
    }
    else
    {
        m_pBtnMark->setText(LANG_TRANSLATE(TRID_MTSW_VOICEMAIL_OPTION_MARK_HEARD));
    }
    QObject::connect(m_pBtnMark, SIGNAL(clicked()), this, SLOT(OnBtnMarkClick()));
    m_pBtnMark->setGeometry(RECT_POPUP_MTSWVOICEMAIL_BTN_2);
    m_pBtnMark->show();
    m_pBtnMark->setFont(font);

    //Add To Local
    m_pBtnAddToLocal->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    m_pBtnAddToLocal->setText(LANG_TRANSLATE(TRID_ADD_TO_PHONEBOOK));
    QObject::connect(m_pBtnAddToLocal, SIGNAL(clicked()), this, SLOT(OnBtnAddToLocalClick()));
    m_pBtnAddToLocal->setGeometry(RECT_POPUP_MTSWVOICEMAIL_BTN_3);
    m_pBtnAddToLocal->show();
    m_pBtnAddToLocal->setFont(font);

    //Delete
    m_pBtnDelete->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    m_pBtnDelete->setText(LANG_TRANSLATE(TRID_DELETE));
    QObject::connect(m_pBtnDelete, SIGNAL(clicked()), this, SLOT(OnBtnDeleteClick()));
    m_pBtnDelete->show();
    m_pBtnDelete->setFont(font);

    //Add To Network
    if (m_bAddToNetwork)
    {
        m_pBtnAddToNetwork->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
        m_pBtnAddToNetwork->setText(LANG_TRANSLATE(TRID_ADD_TO_MTSW_CONTACT));
        QObject::connect(m_pBtnAddToNetwork, SIGNAL(clicked()), this, SLOT(OnBtnAddToNetworkClick()));
        m_pBtnAddToNetwork->setGeometry(RECT_POPUP_MTSWVOICEMAIL_BTN_4);
        m_pBtnAddToNetwork->show();
        m_pBtnAddToNetwork->setFont(font);
        m_pBtnDelete->setGeometry(RECT_POPUP_MTSWVOICEMAIL_BTN_5);
    }
    else
    {
        m_pBtnDelete->setGeometry(RECT_POPUP_MTSWVOICEMAIL_BTN_4);
    }

    SetExitTime(timeTodie);
}

//设置弹出框背景
void CMTSWVoiceMailBox::SetMessageBoxBackground(const QString & strPic)
{
    //设置背景图片
    QPalette pal = this->palette();
    QPixmap pix(THEME_GET_BMP(strPic.toUtf8().data()));
    pix.scaled(this->size());
    pal.setBrush(QPalette::Window, QBrush(pix));
    this->setPalette(pal);
    this->setMask((THEME_GET_BMP(strPic.toUtf8().data())).mask());
}

void CMTSWVoiceMailBox::OnBtnDeleteClick()
{
    ExitMessageBox(MessageBox_Delete);
}

void CMTSWVoiceMailBox::OnBtnMarkClick()
{
    if (m_bRead)
    {
        ExitMessageBox(MessageBox_MarkAsNew);
    }
    else
    {
        ExitMessageBox(MessageBox_MarkAsHeard);
    }
}

void CMTSWVoiceMailBox::OnBtnCallBackClick()
{
    ExitMessageBox(MessageBox_CallBack);
}
void CMTSWVoiceMailBox::OnBtnAddToLocalClick()
{
    ExitMessageBox(MessageBox_AddToLocal);
}
void CMTSWVoiceMailBox::OnBtnAddToNetworkClick()
{
    ExitMessageBox(MessageBox_AddToNetwork);
}

//设置过滤器
bool CMTSWVoiceMailBox::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (!bPress)
    {
        return false;
    }

    switch (nKeyCode)
    {

    }
    return CMessageBoxBase::ProcessKeyEvent(nKeyCode, bPress);
}

#endif