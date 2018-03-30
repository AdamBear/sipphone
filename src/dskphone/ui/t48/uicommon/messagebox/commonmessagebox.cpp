#include "commonmessagebox.h"
#include "commonunits/modcommonunits.h"
#include "commonapi/systemhelperapi.h"
#include "keymap.h"
#include "globalinputhook/modinputhook.h"
#include "imagemanager/modimagemanager.h"
#include "inputmethod/inputmethod.h"
#include "mainwnd/mainwnd.h"
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uikernel/languagehelper.h"
#include "uikernel/quipushbutton.h"
#include "uikernel/qwidgetutility.h"
#include "uicommon.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"
#include "baseui/t4xpicpath.h"
#include "include/picpath.h"
#include <QtGui>
#include "etlmsghandler/modetlmsghandler.h"

#define MESSAGEBOX_RECT_LEFT 171
#define MESSAGEBOX_RECT_TOP 160
#define MESSAGEBOX_RECT_WIDTH 458
#define MESSAGEBOX_RECT_HEIGHT 160

#define MESSAGEBOX_NOTE_FONT_SIZE 18

#define MESSAGEBOX_TEXT_LEFT_SPACING 4

#define MESSAGEBOX_BTN_SET_LEFT_MARGIN 8
#define MESSAGEBOX_BTN_SET_RIGHT_MARGIN 7
#define MESSAGEBOX_BTN_SET_BOTTOM_MARGIN 8

#define MESSAGEBOX_BTN_HEIGHT 53

CCommonMessageBox::CCommonMessageBox(CMaskFrame * pMaskFrame /* = 0 */,
                                     bool bIsShowCountDown/* = false*/)
    : CMessageBoxBase(pMaskFrame)
    , m_bIsShowCountDown(bIsShowCountDown)
    , m_pRemainTimer(NULL)
    , m_nTimeRemain(0)
    , m_bHaveButtons(true)
{
    setAutoFillBackground(true);
    SetMessageBoxBackground(PIC_MSGBOX_BG1);

    m_LabText.setParent(this);
    m_LabText.setObjectName("LabText");
    qWidgetUtility::setFontStyle(&m_LabText, MESSAGEBOX_NOTE_FONT_SIZE);

    //设置位置和大小，note类型的提示框没有按钮，大小不同其它类型
    //弹出框的图片是固定的，因此只是改变note标签的位置，并不改变弹出框的大小
    //注释掉的这段为改变弹出框大小的设置
    //  QRect rect = QRect(MESSAGEBOX_RECT_LEFT,
    //              MESSAGEBOX_NOTE ==  GetType() ? (MESSAGEBOX_RECT_TOP + MESSAGEBOX_BTN_HEIGHT/2) : MESSAGEBOX_RECT_TOP,
    //              MESSAGEBOX_RECT_WIDTH,
    //              MESSAGEBOX_NOTE ==  GetType() ? (MESSAGEBOX_RECT_HEIGHT - MESSAGEBOX_BTN_HEIGHT) : MESSAGEBOX_RECT_HEIGHT);
    QRect rect = QRect(MESSAGEBOX_RECT_LEFT, MESSAGEBOX_RECT_TOP, MESSAGEBOX_RECT_WIDTH,
                       MESSAGEBOX_RECT_HEIGHT);
    this->setGeometry(rect);

    if (m_bIsShowCountDown)
    {
        m_pRemainTimer = new QTimer();
        if (NULL != m_pRemainTimer)
        {
            m_pRemainTimer->setParent(this);
            connect(m_pRemainTimer, SIGNAL(timeout()), this, SLOT(OnCountTimeOut()));
        }
    }
}

CCommonMessageBox::~CCommonMessageBox()
{
}

void CCommonMessageBox::OnCountTimeOut()
{
    if (m_bIsShowCountDown && NULL != m_pRemainTimer)
    {
        --m_nTimeRemain;
        m_LabText.setText(m_RemainTimeHint + " " + QString::number(m_nTimeRemain) + " " + LANG_TRANSLATE(
                              TRID_TIME_SECOND));

        if (m_nTimeRemain == 0 && m_pRemainTimer->isActive())
        {
            m_pRemainTimer->stop();
        }

        update();
    }
}

void CCommonMessageBox::SetMessageBoxCommonInfo(const QString & lpszText, int timeTodie /* = -1 */,
        int iID /* = -1 */)
{
    // 设置note
    m_LabText.setGeometry(MESSAGEBOX_TEXT_LEFT_SPACING, 0,
                          width() - MESSAGEBOX_BTN_SET_RIGHT_MARGIN - MESSAGEBOX_BTN_SET_LEFT_MARGIN,
                          GetType() == MESSAGEBOX_NOTE ? (height() - 0) : (height() - 0 - MESSAGEBOX_BTN_HEIGHT));
    m_LabText.setAlignment(Qt::AlignCenter);        //设置文字显示方式，正文居中显示
    m_LabText.setWordWrap(true);
    if (m_bIsShowCountDown && NULL != m_pRemainTimer)
    {
        m_nTimeRemain = timeTodie / 1000;

        if (m_pRemainTimer->isActive())
        {
            m_pRemainTimer->stop();
        }

        if (m_nTimeRemain > 0)
        {
            m_pRemainTimer->start(1000);
        }

        m_RemainTimeHint = lpszText;

        m_LabText.setText(m_RemainTimeHint + " " + QString::number(m_nTimeRemain) + " " + LANG_TRANSLATE(
                              TRID_TIME_SECOND));
    }
    else
    {
        m_LabText.setText(lpszText);
    }

    SetWgtFontColor(&m_LabText, FONT_TYPE_TEXT, Qt::white);

    SetExitTime(timeTodie);

    SetID(iID);
}

void CCommonMessageBox::SetMessageBoxSpecialInfo(VecMsgBoxSoftkey & vecSoftkey)
{
    if (MESSAGEBOX_NOTE == GetType())
    {
        m_bHaveButtons = false;
        return;
    }

    m_vecSoftkey = vecSoftkey;
    QRect rect = QRect(MESSAGEBOX_RECT_LEFT, MESSAGEBOX_RECT_TOP, MESSAGEBOX_RECT_WIDTH,
                       MESSAGEBOX_RECT_HEIGHT);
    this->setGeometry(rect);

    //自定义softkey
    m_BtnSet.setGeometry(MESSAGEBOX_BTN_SET_LEFT_MARGIN,
                         height() - MESSAGEBOX_BTN_SET_BOTTOM_MARGIN - MESSAGEBOX_BTN_HEIGHT,
                         width() - (MESSAGEBOX_BTN_SET_LEFT_MARGIN + MESSAGEBOX_BTN_SET_RIGHT_MARGIN),
                         MESSAGEBOX_BTN_HEIGHT);

    m_BtnSet.SetMaxBtnCount(vecSoftkey.size());

    m_BtnSet.show();

    int iCount = m_BtnSet.GetMaxBtnCount();

    ItMsgBoxSoftkey it = m_vecSoftkey.begin();
    for (int i = 0; it != m_vecSoftkey.end() && i < iCount; ++it, ++i)
    {
        qUIPushButton * pButton = new qUIPushButton();
        pButton->setObjectName(QString("Button%1").arg(i));
        //设置图片和文字
        pButton->set3StatePic("", PIC_BTN_MSG_PRESS, "");
        pButton->setText(it->m_title);
        pButton->setBtnAction(it->m_title);

        if (MessageBox_OK == it->m_result)
        {
            m_bAllowOkButton = true;
        }

        //连接槽
        connect(pButton, SIGNAL(clicked(const QString &)), this, SLOT(OnButtonPressed(const QString &)));
        m_BtnSet.AddButton(pButton);
    }
}

//设置弹出框背景
void CCommonMessageBox::SetMessageBoxBackground(const QString & strPic)
{
    //设置背景图片
    QPalette pal = this->palette();
    QPixmap pix(THEME_GET_BMP(strPic.toUtf8().data()));
    pix.scaled(this->size());
    pal.setBrush(QPalette::Window, QBrush(pix));
    this->setPalette(pal);
    this->setMask((THEME_GET_BMP(strPic.toUtf8().data())).mask());
}

bool CCommonMessageBox::ProcessKeyEvent(int nKeyCode, bool bPress)
{
    if (!bPress)
    {
        return false;
    }

    switch (GetType())
    {
    case MESSAGEBOX_NOTE:
        {
            //通道键不锁
            if (PHONE_KEY_HANDFREE == nKeyCode
                    || PHONE_KEY_HANDSET == nKeyCode
                    || PHONE_KEY_HANDSET_ON_HOOK == nKeyCode
                    || PHONE_KEY_HANDSET_OFF_HOOK == nKeyCode
                    || PHONE_KEY_HEADSET_CTRL == nKeyCode
                    || PHONE_KEY_EHS_OPEN == nKeyCode
                    || PHONE_KEY_EHS_CLOSE == nKeyCode
                    || PHONE_KEY_VOLUME_DECREASE == nKeyCode
                    || PHONE_KEY_VOLUME_INCREASE == nKeyCode)
            {
                return false;
            }
            //note类型不响应按键事件
            return true;
        }
        break;
    case MESSAGEBOX_Cancel:
        {
            //Cancel类型不响应ok键
            if (nKeyCode == PHONE_KEY_OK)
            {
                return true;
            }
        }
        break;
    default:
        break;
    }

    //只有当前弹出框在顶层激活状态时才接受事件
    switch (nKeyCode)
    {
    case PHONE_KEY_OK:
        {
            // 只有此弹出框有确认按钮时，按ok才有效果
            if (GetType() == MESSAGEBOX_OkCancel
                    || GetType() == MESSAGEBOX_OkBlock)
//              || GetType() == MESSAGEBOX_VoiceMail)
            {
                ExitMessageBox(MessageBox_OK);
                return true;
            }
        }
        break;
    case PHONE_KEY_CANCEL:
        {
            ExitMessageBox(MessageBox_Cancel);
            return true;
        }
        break;
    default:
        break;
    }

    return CMessageBoxBase::ProcessKeyEvent(nKeyCode, bPress);
}

void CCommonMessageBox::PrintMsgBoxInfo(bool bAdd)
{
    if (bAdd)
    {
        POPUPBOX_INFO("Add CCommonMessageBox::PrintMsgBoxInfo: %s", m_LabText.text().toUtf8().data());
    }
    else
    {
        POPUPBOX_INFO("Del CCommonMessageBox::PrintMsgBoxInfo: %s", m_LabText.text().toUtf8().data());
    }
}
