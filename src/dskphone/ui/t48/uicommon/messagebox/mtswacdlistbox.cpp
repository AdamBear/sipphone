#if IF_FEATURE_METASWITCH
#include "mtswacdlistbox.h"

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

#define MTSWACDLIST_BOX_TITLE_HEIGHT 51

#define MTSWACDLIST_BOX_FONT 16

#define RECT_POPUP_MTSWACDLIST                  QRect(222, 66, 357, 312)

#define POPUP_MTSWACDLIST_BTN_LEFT              65
#define POPUP_MTSWACDLIST_BTN_TOP               61
#define POPUP_MTSWACDLIST_BTN_WIDTH             227
#define POPUP_MTSWACDLIST_BTN_HEIGHT            53

CMTSWAcdListBox::CMTSWAcdListBox(CMaskFrame * pMaskFrame /* = 0 */)
    : CMessageBoxBase(pMaskFrame)
    , m_pBtnLogin(NULL)
    , m_pBtnDetail(NULL)
    , m_pBtnMember(NULL)
    , m_pLabelTitle(NULL)
    , m_eShowLoggedMode(EMTSWACDLIST_SHOW_NONE)
{
}

CMTSWAcdListBox::~CMTSWAcdListBox()
{
}

/*************************************************/
//  按照msgbox的类型设置不同的msgbox
//  lpszText为正文内容
//  timeTodie为生存时间，单位为s，-1表示无限制
/*************************************************/
void CMTSWAcdListBox::SetMessageBoxInfo(const QString & lpszText /* = "" */,
                                        int timeTodie /* = -1 */)
{
    QRect rect = RECT_POPUP_MTSWACDLIST;
    if (m_eShowLoggedMode == EMTSWACDLIST_SHOW_NONE)
    {
        rect.setHeight(rect.height() - POPUP_MTSWACDLIST_BTN_TOP);
    }
    this->setGeometry(rect);

    //设置背景
    setAutoFillBackground(true);
    SetMessageBoxBackground(PIC_BLUE_POPUP);

    m_pLabelTitle = new QLabel(this);
    m_pBtnLogin = new qUIPushButton(this);
    m_pBtnDetail = new qUIPushButton(this);
    m_pBtnMember = new qUIPushButton(this);
    m_pBtnCancel = new qUIPushButton(this);

    if (NULL == m_pBtnLogin || NULL == m_pBtnDetail
            || NULL == m_pBtnMember || NULL == m_pLabelTitle
            || NULL == m_pBtnCancel)
    {
        return;
    }

    m_pLabelTitle->setObjectName("LabelTitle");
    m_pBtnLogin->setObjectName("BtnLogin");
    m_pBtnDetail->setObjectName("BtnDetail");
    m_pBtnMember->setObjectName("BtnMember");
    m_pBtnCancel->setObjectName("BtnCancel");

    QFont font;
    font.setPointSize(MTSWACDLIST_BOX_FONT);
    font.setWeight(QFont::Light);
    font.setBold(false);

    // set title
    m_pLabelTitle->setText(LANG_TRANSLATE(TRID_MORE));
    if ("" != lpszText)
    {
        m_pLabelTitle->setText(lpszText);
    }
    m_pLabelTitle->setAlignment(Qt::AlignCenter);       //设置文字显示方式，正文居中显示
    //m_pLabelTitle->setWordWrap(true);
    QPalette palette = m_pLabelTitle->palette();
    palette.setColor(QPalette::WindowText, Qt::white);
    m_pLabelTitle->setPalette(palette);
    m_pLabelTitle->setGeometry(10,
                               (MTSWACDLIST_BOX_TITLE_HEIGHT - m_pLabelTitle->height()) >> 1,
                               RECT_POPUP_MTSWACDLIST.width() - 20,
                               m_pLabelTitle->height());

    int nBtnTop = POPUP_MTSWACDLIST_BTN_TOP;

    //Login
    m_pBtnLogin->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    QObject::connect(m_pBtnLogin, SIGNAL(clicked()), this, SLOT(OnBtnLoginClick()));
    m_pBtnLogin->setFont(font);
    m_pBtnLogin->setGeometry(POPUP_MTSWACDLIST_BTN_LEFT, nBtnTop,
                             POPUP_MTSWACDLIST_BTN_WIDTH, POPUP_MTSWACDLIST_BTN_HEIGHT);

    if (m_eShowLoggedMode == EMTSWACDLIST_SHOW_LOGOUT)
    {
        m_pBtnLogin->setText(LANG_TRANSLATE(TRID_MTSW_ACD_LOGOUT));
        m_pBtnLogin->show();
        nBtnTop += POPUP_MTSWACDLIST_BTN_TOP;
    }
    else if (m_eShowLoggedMode == EMTSWACDLIST_SHOW_LOGIN)
    {
        m_pBtnLogin->setText(LANG_TRANSLATE(TRID_MTSW_ACD_LOGIN));
        m_pBtnLogin->show();
        nBtnTop += POPUP_MTSWACDLIST_BTN_TOP;
    }
    else
    {
        m_pBtnLogin->hide();
    }


    //Detail
    m_pBtnDetail->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    m_pBtnDetail->setText(LANG_TRANSLATE(TRID_SOFTKEY_MTSW_ACD_DETAIL));

    QObject::connect(m_pBtnDetail, SIGNAL(clicked()), this, SLOT(OnBtnDetailClick()));
    m_pBtnDetail->setGeometry(POPUP_MTSWACDLIST_BTN_LEFT, nBtnTop,
                              POPUP_MTSWACDLIST_BTN_WIDTH, POPUP_MTSWACDLIST_BTN_HEIGHT);
    m_pBtnDetail->show();
    nBtnTop += POPUP_MTSWACDLIST_BTN_TOP;
    m_pBtnDetail->setFont(font);

    //Member
    m_pBtnMember->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    m_pBtnMember->setText(LANG_TRANSLATE(TRID_SOFTKEY_MTSW_ACD_MEMBER));

    QObject::connect(m_pBtnMember, SIGNAL(clicked()), this, SLOT(OnBtnMemberClick()));
    m_pBtnMember->setGeometry(POPUP_MTSWACDLIST_BTN_LEFT, nBtnTop,
                              POPUP_MTSWACDLIST_BTN_WIDTH, POPUP_MTSWACDLIST_BTN_HEIGHT);
    nBtnTop += POPUP_MTSWACDLIST_BTN_TOP;
    m_pBtnMember->show();
    m_pBtnMember->setFont(font);

    m_pBtnCancel->set3StatePic(PIC_BLUE_BTN, PIC_BLUE_BTN_CLICK, "");
    m_pBtnCancel->setText(LANG_TRANSLATE(TRID_CANCEL));
    QObject::connect(m_pBtnCancel, SIGNAL(clicked()), this, SLOT(OnBtnCancelClick()));
    m_pBtnCancel->setGeometry(POPUP_MTSWACDLIST_BTN_LEFT, nBtnTop,
                              POPUP_MTSWACDLIST_BTN_WIDTH, POPUP_MTSWACDLIST_BTN_HEIGHT);
    m_pBtnCancel->show();
    m_pBtnCancel->setFont(font);

    SetExitTime(timeTodie);
}

//设置弹出框背景
void CMTSWAcdListBox::SetMessageBoxBackground(const QString & strPic)
{
    //设置背景图片
    QPalette pal = this->palette();
    QPixmap pix(THEME_GET_BMP(strPic.toUtf8().data()));
    pix.scaled(this->size());
    pal.setBrush(QPalette::Window, QBrush(pix));
    this->setPalette(pal);
    this->setMask((THEME_GET_BMP(strPic.toUtf8().data())).mask());
}

//响应点击Login/Logout事件
void CMTSWAcdListBox::OnBtnLoginClick()
{
    if (m_eShowLoggedMode == EMTSWACDLIST_SHOW_LOGOUT)
    {
        ExitMessageBox(MessageBox_MTSWACDList_Logout);
    }
    else
    {
        ExitMessageBox(MessageBox_MTSWACDList_Login);
    }
}

//响应点击Detail事件
void CMTSWAcdListBox::OnBtnDetailClick()
{
    ExitMessageBox(MessageBox_MTSWACDList_Detail);
}

//响应点击Member事件
void CMTSWAcdListBox::OnBtnMemberClick()
{
    ExitMessageBox(MessageBox_MTSWACDList_Member);
}

void CMTSWAcdListBox::OnBtnCancelClick()
{
    ExitMessageBox(MessageBox_Cancel);
}
#endif

