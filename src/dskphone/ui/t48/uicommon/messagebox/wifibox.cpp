#include "wifibox.h"

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
#include "uicommon.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "baseui/t4xpicpath.h"
#include <QtGui>

#define WIFI_BOX_LEFT 222
#define WIFI_BOX_TOP 46
#define WIFI_BOX_WIDTH 357
#define WIFI_BOX_HEIGHT 357

#define WIFI_BOX_FONT 16

#define RECT_POPUP_WIFI             QRect(WIFI_BOX_LEFT, WIFI_BOX_TOP, WIFI_BOX_WIDTH, WIFI_BOX_HEIGHT)

namespace
{
//const QRect RECT_POPUP_WIFI(221, 46, 358, 340);
const QRect RECT_POPUP_WIFI_FRAMELIST(0, 51, 357, 50);
const QRect RECT_POPUP_WIFI_BTN_MOVEUP(70, 83, 227, 53);
const QRect RECT_POPUP_WIFI_BTN_MOVEDOWN(70, 144, 227, 53);
const QRect RECT_POPUP_WIFI_BTN_DELETE(70, 205, 227, 53);
const QRect RECT_POPUP_WIFI_BTN_CANCEL(70, 266, 227, 53);
const QRect RECT_POPUP_WIFI_BTN_EDIT(70, 22, 227, 53);
}

CWifiDetailBox::CWifiDetailBox(CMaskFrame * pMaskFrame /* = 0 */)
    : CMessageBoxBase(pMaskFrame)
    , m_pBtnCancel(NULL)
    , m_pBtnDelete(NULL)
    , m_pBtnMoveDown(NULL)
    , m_pBtnMoveUp(NULL)
    , m_pBtnEdit(NULL)
{
}

CWifiDetailBox::~CWifiDetailBox()
{
}

/*************************************************/
/*  按照msgbox的类型设置不同的msgbox
/*  lpszText为正文内容
/*  timeTodie为生存时间，单位为s，-1表示无限制
/*************************************************/
void CWifiDetailBox::SetMessageBoxInfo(const QString & lpszText /* = "" */,
                                       int timeTodie /* = -1 */)
{

    QRect rect = RECT_POPUP_WIFI;
    this->setGeometry(rect);

    //设置背景
    setAutoFillBackground(true);
    SetMessageBoxBackground(PIC_WIFI_POP_BG);

    m_pBtnCancel = new qUIPushButton(this);
    m_pBtnDelete = new qUIPushButton(this);
    m_pBtnMoveUp = new qUIPushButton(this);
    m_pBtnMoveDown = new qUIPushButton(this);
    m_pBtnEdit = new qUIPushButton(this);

    if (NULL == m_pBtnMoveDown || NULL == m_pBtnMoveUp
            || NULL == m_pBtnCancel || NULL == m_pBtnDelete
            || NULL == m_pBtnEdit)
    {
        return;
    }

    m_pBtnCancel->setObjectName("BtnCancel");
    m_pBtnDelete->setObjectName("BtnDelete");
    m_pBtnMoveUp->setObjectName("BtnMoveUp");
    m_pBtnMoveDown->setObjectName("BtnMoveDown");
    m_pBtnEdit->setObjectName("BtnEdit");

    QFont font;
    font.setPointSize(WIFI_BOX_FONT);
    font.setWeight(QFont::Light);
    font.setBold(false);

    // BtnEdit  位置
    m_pBtnEdit->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    m_pBtnEdit->setText(LANG_TRANSLATE(TRID_EDIT));
    QObject::connect(m_pBtnEdit, SIGNAL(clicked()), this, SLOT(OnBtnEditClick()));
    m_pBtnEdit->setGeometry(RECT_POPUP_WIFI_BTN_EDIT);
    m_pBtnEdit->show();
    m_pBtnEdit->setFont(font);
    // set the btn
    m_pBtnDelete->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    m_pBtnDelete->setText(LANG_TRANSLATE(TRID_DELETE));
    QObject::connect(m_pBtnDelete, SIGNAL(clicked()), this, SLOT(OnBtnDeleteClick()));
    m_pBtnDelete->setGeometry(RECT_POPUP_WIFI_BTN_DELETE);
    m_pBtnDelete->show();
    m_pBtnDelete->setFont(font);

    m_pBtnCancel->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    m_pBtnCancel->setText(LANG_TRANSLATE(TRID_CANCEL));
    QObject::connect(m_pBtnCancel, SIGNAL(clicked()), this, SLOT(OnBtnCancelClick()));
    m_pBtnCancel->setGeometry(RECT_POPUP_WIFI_BTN_CANCEL);
    m_pBtnCancel->show();
    m_pBtnCancel->setFont(font);

    m_pBtnMoveUp->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    m_pBtnMoveUp->setText(LANG_TRANSLATE(TRID_MOVE_UP));
    QObject::connect(m_pBtnMoveUp, SIGNAL(clicked()), this, SLOT(OnBtnMoveUpClick()));
    m_pBtnMoveUp->setGeometry(RECT_POPUP_WIFI_BTN_MOVEUP);
    m_pBtnMoveUp->show();
    m_pBtnMoveUp->setFont(font);

    m_pBtnMoveDown->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    m_pBtnMoveDown->setText(LANG_TRANSLATE(TRID_MOVE_DOWN));
    QObject::connect(m_pBtnMoveDown, SIGNAL(clicked()), this, SLOT(OnBtnMoveDownClick()));
    m_pBtnMoveDown->setGeometry(RECT_POPUP_WIFI_BTN_MOVEDOWN);
    m_pBtnMoveDown->show();
    m_pBtnMoveDown->setFont(font);

    SetExitTime(timeTodie);
}

void CWifiDetailBox::PrintMsgBoxInfo(bool bAdd)
{
    if (bAdd)
    {
        POPUPBOX_INFO("Add CCommonMessageBox::PrintMsgBoxInfo");
    }
    else
    {
        POPUPBOX_INFO("Del CCommonMessageBox::PrintMsgBoxInfo");
    }
}

//设置弹出框背景
void CWifiDetailBox::SetMessageBoxBackground(const QString & strPic)
{
    //设置背景图片
    QPalette pal = this->palette();
    QPixmap pix(THEME_GET_BMP(strPic.toUtf8().data()));
    pix.scaled(this->size());
    pal.setBrush(QPalette::Window, QBrush(pix));
    this->setPalette(pal);
    this->setMask((THEME_GET_BMP(strPic.toUtf8().data())).mask());
}


void CWifiDetailBox::OnBtnCancelClick()
{
    ExitMessageBox(MessageBox_Cancel);
}

void CWifiDetailBox::OnBtnDeleteClick()
{
    ExitMessageBox(MessageBox_Delete);
}

void  CWifiDetailBox::OnBtnMoveDownClick()
{
    ExitMessageBox(MessageBox_MoveDown);
}

void  CWifiDetailBox::OnBtnMoveUpClick()
{
    ExitMessageBox(MessageBox_MoveUp);
}

void CWifiDetailBox::OnBtnEditClick()
{
    ExitMessageBox(MessageBox_Edit);
}

//设置过滤器
bool CWifiDetailBox::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (!bPress)
    {
        return false;
    }

    return CMessageBoxBase::ProcessKeyEvent(nKeyCode, bPress);
}


