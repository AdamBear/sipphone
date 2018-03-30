#include "bluetoothbox.h"

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

#define BLUETOOTH_BOX_LEFT 222
#define BLUETOOTH_BOX_TOP 46
#define BLUETOOTH_BOX_WIDTH 357
#define BLUETOOTH_BOX_HEIGHT 357

#define BLUETOOTH_BOX_TITLE_HEIGHT 51

#define BLUETOOTH_BOX_FRAMELIST_HEIGHT 50
#define BLUETOOTH_BOX_TITLE_OFFSET 3

#define BLUETOOTH_BOX_LABEL_LEFT_MARGIN 16
#define BLUETOOTH_BOX_LABEL_WIDTH 162

#define BLUETOOTH_BOX_CHECKBOX_LEFT_MARGIN 178

#define BLUETOOTH_BOX_BUTTON_LEFT 65
#define BLUETOOTH_BOX_BUTTON_WIDTH 227
#define BLUETOOTH_BOX_BUTTON_HEIGHT 53

#define BLUETOOTH_BOX_BUTTON_TOP_1 111
#define BLUETOOTH_BOX_BUTTON_TOP_2 172
#define BLUETOOTH_BOX_BUTTON_TOP_3 233
#define BLUETOOTH_BOX_BUTTON_TOP_4 294


#define BLUETOOTH_BOX_LINE_LEFT 0
#define BLUETOOTH_BOX_LINE_TOP 101
#define BLUETOOTH_BOX_LINE_WIDTH 1

#define BLUETOOTH_BOX_FONT 16

#define RECT_POPUP_BLUETOOTH                                QRect(BLUETOOTH_BOX_LEFT, BLUETOOTH_BOX_TOP, BLUETOOTH_BOX_WIDTH, BLUETOOTH_BOX_HEIGHT)
#define RECT_POPUP_BLUETOOTH_BTN_MOVEUP     QRect(BLUETOOTH_BOX_BUTTON_LEFT, BLUETOOTH_BOX_BUTTON_TOP_1, BLUETOOTH_BOX_BUTTON_WIDTH, BLUETOOTH_BOX_BUTTON_HEIGHT)
#define RECT_POPUP_BLUETOOTH_BTN_MOVEDOWN   QRect(BLUETOOTH_BOX_BUTTON_LEFT, BLUETOOTH_BOX_BUTTON_TOP_2, BLUETOOTH_BOX_BUTTON_WIDTH, BLUETOOTH_BOX_BUTTON_HEIGHT)
#define RECT_POPUP_BLUETOOTH_BTN_DELETE         QRect(BLUETOOTH_BOX_BUTTON_LEFT, BLUETOOTH_BOX_BUTTON_TOP_3, BLUETOOTH_BOX_BUTTON_WIDTH, BLUETOOTH_BOX_BUTTON_HEIGHT)
#define RECT_POPUP_BLUETOOTH_BTN_CANCEL         QRect(BLUETOOTH_BOX_BUTTON_LEFT, BLUETOOTH_BOX_BUTTON_TOP_4, BLUETOOTH_BOX_BUTTON_WIDTH, BLUETOOTH_BOX_BUTTON_HEIGHT)
#define RECT_POPUP_BLUETOOTH_LINE                       QRect(BLUETOOTH_BOX_LINE_LEFT, BLUETOOTH_BOX_LINE_TOP, BLUETOOTH_BOX_WIDTH, BLUETOOTH_BOX_LINE_WIDTH)

CBluetoothBox::CBluetoothBox(CMaskFrame * pMaskFrame /* = 0 */)
    : CMessageBoxBase(pMaskFrame)
    , m_pBtnCancel(NULL)
    , m_pBtnDelete(NULL)
    , m_pBtnMoveDown(NULL)
    , m_pBtnMoveUp(NULL)
    , m_bAutoConnect(true)
    , m_pLabelTitle(NULL)
    , m_pLine(NULL)
    , m_pLabelList(NULL)
    , m_pCheckBox(NULL)
{

}

CBluetoothBox::~CBluetoothBox()
{
}

/*************************************************/
/*  按照msgbox的类型设置不同的msgbox
/*  lpszText为正文内容
/*  timeTodie为生存时间，单位为s，-1表示无限制
/*************************************************/
void CBluetoothBox::SetMessageBoxInfo(const QString & lpszText /* = "" */, int timeTodie /* = -1 */)
{

    QRect rect = RECT_POPUP_BLUETOOTH;
    this->setGeometry(rect);

    //设置背景
    setAutoFillBackground(true);
    SetMessageBoxBackground(PIC_BLUE_POPUP);

    m_pBtnCancel = new qUIPushButton(this);
    m_pBtnDelete = new qUIPushButton(this);
    m_pBtnMoveUp = new qUIPushButton(this);
    m_pBtnMoveDown = new qUIPushButton(this);
    m_pLabelTitle = new QLabel(this);
    m_pLine = new qLine(RECT_POPUP_BLUETOOTH_LINE, this);
    m_pLabelList = new QLabel(this);
    m_pCheckBox = new qCheckBox(this);

    if (NULL == m_pBtnMoveDown || NULL == m_pBtnMoveUp
            || NULL == m_pBtnCancel || NULL == m_pBtnDelete
            || NULL == m_pLabelTitle || NULL == m_pLabelList
            || NULL == m_pLine || NULL == m_pCheckBox)
    {
        return;
    }

    m_pBtnCancel->setObjectName("BtnCancel");
    m_pBtnDelete->setObjectName("BtnDelete");
    m_pBtnMoveUp->setObjectName("BtnMoveUp");
    m_pBtnMoveDown->setObjectName("BtnMoveDown");
    m_pLabelTitle->setObjectName("LabelTitle");
    m_pLine->setObjectName("Line");
    m_pLabelList->setObjectName("LabelList");
    m_pCheckBox->setObjectName("CheckBox");

    QFont font;
    font.setPointSize(BLUETOOTH_BOX_FONT);
    font.setWeight(QFont::Light);
    font.setBold(false);

    // set title
    m_pLabelTitle->setText(LANG_TRANSLATE(TRID_MORE));
    if ("" != lpszText)
    {
        m_pLabelTitle->setText(lpszText);
    }
    m_pLabelTitle->setAlignment(Qt::AlignCenter);       //设置文字显示方式，正文居中显示
    m_pLabelTitle->setWordWrap(true);
    QPalette palette = m_pLabelTitle->palette();
    palette.setColor(QPalette::WindowText, Qt::white);
    m_pLabelTitle->setPalette(palette);
    m_pLabelTitle->setGeometry((BLUETOOTH_BOX_WIDTH - m_pLabelTitle->width()) >> 1,
                               (BLUETOOTH_BOX_TITLE_HEIGHT - m_pLabelTitle->height()) >> 1,
                               m_pLabelTitle->width(),
                               m_pLabelTitle->height());

    // set framelist
    m_pLabelList->setText(LANG_TRANSLATE(TRID_AUTO_CONNECT));
    m_pLabelList->setAlignment(Qt::AlignLeft);
    m_pLabelList->setFont(font);
    //m_pLabelList->setWordWrap(true);
    m_pLabelList->setGeometry(BLUETOOTH_BOX_LABEL_LEFT_MARGIN,
                              BLUETOOTH_BOX_TITLE_HEIGHT + BLUETOOTH_BOX_TITLE_OFFSET + ((BLUETOOTH_BOX_FRAMELIST_HEIGHT -
                                      m_pLabelList->height()) >> 1),
                              BLUETOOTH_BOX_LABEL_WIDTH,
                              m_pLabelList->height());

    m_pCheckBox->SetData(m_bAutoConnect);
    m_pCheckBox->setGeometry(BLUETOOTH_BOX_CHECKBOX_LEFT_MARGIN,
                             BLUETOOTH_BOX_TITLE_HEIGHT + ((BLUETOOTH_BOX_FRAMELIST_HEIGHT - m_pCheckBox->height()) >> 1),
                             m_pCheckBox->width(),
                             m_pCheckBox->height());
    m_pCheckBox->setFont(font);
    QObject::connect(m_pCheckBox->GetRadioButtonOn(), SIGNAL(toggled(bool)), this,
                     SLOT(OnSwitchAutoStatus(bool)));

    // set the btn
    m_pBtnDelete->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    m_pBtnDelete->setText(LANG_TRANSLATE(TRID_DELETE));
    QObject::connect(m_pBtnDelete, SIGNAL(clicked()), this, SLOT(OnBtnDeleteClick()));
    m_pBtnDelete->setGeometry(RECT_POPUP_BLUETOOTH_BTN_DELETE);
    m_pBtnDelete->show();
    m_pBtnDelete->setFont(font);

    m_pBtnCancel->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    m_pBtnCancel->setText(LANG_TRANSLATE(TRID_CANCEL));
    QObject::connect(m_pBtnCancel, SIGNAL(clicked()), this, SLOT(OnBtnCancelClick()));
    m_pBtnCancel->setGeometry(RECT_POPUP_BLUETOOTH_BTN_CANCEL);
    m_pBtnCancel->show();
    m_pBtnCancel->setFont(font);

    m_pBtnMoveUp->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    m_pBtnMoveUp->setText(LANG_TRANSLATE(TRID_MOVE_UP));
    QObject::connect(m_pBtnMoveUp, SIGNAL(clicked()), this, SLOT(OnBtnMoveUpClick()));
    m_pBtnMoveUp->setGeometry(RECT_POPUP_BLUETOOTH_BTN_MOVEUP);
    m_pBtnMoveUp->show();
    m_pBtnMoveUp->setFont(font);

    m_pBtnMoveDown->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    m_pBtnMoveDown->setText(LANG_TRANSLATE(TRID_MOVE_DOWN));
    QObject::connect(m_pBtnMoveDown, SIGNAL(clicked()), this, SLOT(OnBtnMoveDownClick()));
    m_pBtnMoveDown->setGeometry(RECT_POPUP_BLUETOOTH_BTN_MOVEDOWN);
    m_pBtnMoveDown->show();
    m_pBtnMoveDown->setFont(font);

    SetExitTime(timeTodie);
}

void CBluetoothBox::SetAutoConnect(bool & bAutoConnect)
{
    m_bAutoConnect = bAutoConnect;
    if (NULL != m_pCheckBox)
    {
        m_pCheckBox->SetData(bAutoConnect);
    }
}

qCheckBox * CBluetoothBox::GetCheckBox() const
{
    return m_pCheckBox;
}

void CBluetoothBox::PrintMsgBoxInfo(bool bAdd)
{
    if (NULL == m_pLabelTitle)
    {
        return;
    }

    if (bAdd)
    {
        POPUPBOX_INFO("Add CBluetoothBox::PrintMsgBoxInfo: %s", m_pLabelTitle->text().toUtf8().data());
    }
    else
    {
        POPUPBOX_INFO("Del CBluetoothBox::PrintMsgBoxInfo: %s", m_pLabelTitle->text().toUtf8().data());
    }
}

//设置弹出框背景
void CBluetoothBox::SetMessageBoxBackground(const QString & strPic)
{
    //设置背景图片
    QPalette pal = this->palette();
    QPixmap pix(THEME_GET_BMP(strPic.toUtf8().data()));
    pix.scaled(this->size());
    pal.setBrush(QPalette::Window, QBrush(pix));
    this->setPalette(pal);
    this->setMask((THEME_GET_BMP(strPic.toUtf8().data())).mask());
}

void CBluetoothBox::OnSwitchAutoStatus(bool bOn)
{
    SetAutoConnect(bOn);

    //此处不应作退出处理
    if (bOn)
    {
        ExitMessageBox(MessageBox_On);
    }
    else
    {
        ExitMessageBox(MessageBox_Off);
    }
}


void CBluetoothBox::OnBtnCancelClick()
{
    ExitMessageBox(MessageBox_Cancel);
}

void CBluetoothBox::OnBtnDeleteClick()
{
    ExitMessageBox(MessageBox_Delete);
}

void  CBluetoothBox::OnBtnMoveDownClick()
{
    ExitMessageBox(MessageBox_MoveDown);
}

void  CBluetoothBox::OnBtnMoveUpClick()
{
    ExitMessageBox(MessageBox_MoveUp);
}

//设置过滤器
bool CBluetoothBox::ProcessKeyEvent(int nKeyCode, bool bPress)
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


