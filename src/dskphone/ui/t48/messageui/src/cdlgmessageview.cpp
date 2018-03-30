#include "cdlgmessageview.h"
#include "setting/include/common.h"
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qcomboboxutility.h"
#include "messagebox/modmessagebox.h"
#include "messagebox/messageboxbase.h"
#include "uimanager/uimanager_inc.h"
#include "setting/src/textmessagecontroller.h"
#include "inputmethod/inputmethod.h"
#include "mainwnd/mainwnd.h"
#include "cmsguilogic.h"
#include <QtGui>
#include <QtCore>

#include "uicommon.h"
#include "keymap.h"
#include "baseui/t4xpicpath.h"

namespace
{
//显示发件人、收件人、时间的标签的位置
const QRect RECT_TEXT_FROM_LOCATION(17, 10, 500, 27);
const QRect RECT_TEXT_TO_LOCATION(17, 37, 500, 27);
const QRect RECT_TEXT_TIME_LOCATION(17, 64, 500, 22);

//消息内容的位置
const QRect RECT_DLG_LOCATION(17, 98, 501, 200);

//中间的线的位置
#define LINE_LEFT           15
#define LINE_TOP            96//111
#define LINE_LENGTH 501

#define MSGBOX_ID_DELETE                    81
}

CDlgMessageView::CDlgMessageView(QWidget * parent)
    : CDlgBaseSubPage(parent)
    , m_pLabelFromAddr(NULL)
    , m_pLabelToAddr(NULL)
    , m_pLabelTime(NULL)
    , m_iMsgIdx(-1)
{
    //窗体类型
    SetWindowType(eWT_Message);

    InitData();
}

CDlgMessageView::~CDlgMessageView()
{
    bool bOnePage = g_pMsgUILogic->GetIfOnePage();
    if (bOnePage)
    {
        //如果从MsgUILogic得知该窗口是通过dsskey或idle弹出框回调开启的，则退出时需要清除box类型信息
        g_pMsgUILogic->ExitMessageDetail();
        g_pMsgUILogic->SetOnePage(false);
    }
}

void CDlgMessageView::InitData()
{
    if (NULL != m_pGeneralFrame)
    {
        m_pGeneralFrame->hide();
    }

    m_pLabelFromAddr = new QLabel(this);
    m_pLabelToAddr = new QLabel(this);
    m_pLabelTime = new QLabel(this);
    m_pTextEditor = new QTextEdit(this);

    if (NULL == m_pLabelFromAddr
            || NULL == m_pLabelToAddr
            || NULL == m_pLabelTime
            || NULL == m_pTextEditor)
    {
        return;
    }

    m_pLabelFromAddr->setGeometry(RECT_TEXT_FROM_LOCATION);
    m_pLabelFromAddr->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    m_pLabelToAddr->setGeometry(RECT_TEXT_TO_LOCATION);
    m_pLabelToAddr->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    m_pLabelTime->setGeometry(RECT_TEXT_TIME_LOCATION);
    m_pLabelTime->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    QPalette pal = m_pTextEditor->palette();
    pal.setColor(QPalette::Base, QColor(0, 0, 0, 0));
    pal.setColor(QPalette::Text, THEME_TEXT_COLOR);
    pal.setColor(QPalette::Highlight, QColor(0, 0, 0, 0));
    pal.setColor(QPalette::HighlightedText, THEME_TEXT_COLOR);
    m_pTextEditor->setPalette(pal);
    m_pTextEditor->setGeometry(RECT_DLG_LOCATION);
    m_pTextEditor->setReadOnly(true);
}

void CDlgMessageView::Uninit()
{

}

void CDlgMessageView::Init()
{
    //获取当前box类型和焦点消息
    m_iMsgIdx = g_pMsgUILogic->GetCurrentMsgId();
    m_strCurrentBox = g_pMsgUILogic->GetBoxAction();

    TextMsgItemDetailData pageData;
    //获取消息
    if (g_pMsgUILogic->GetMessageDetail(&pageData, m_strCurrentBox, m_iMsgIdx))
    {
        //显示消息
        SetMessageDetail(pageData);
    }
}

void CDlgMessageView::SetMessageDetail(const TextMsgItemDetailData & sDetail)
{
    if (NULL == m_pLabelFromAddr || NULL == m_pLabelToAddr || NULL == m_pLabelTime
            || NULL == m_pTextEditor)
    {
        return;
    }

    yl::string strShowFrom;
    yl::string strShowTo;
    yl::string strShowTime;

    strShowFrom = LANG_TRANSLATE(TRID_FROM).toUtf8().data();
    strShowFrom += " : ";
    strShowFrom += sDetail.m_strFromName.empty() ? sDetail.m_strFromUserName : sDetail.m_strFromName;
    m_pLabelFromAddr->setText(QString::fromUtf8(strShowFrom.c_str()));

    strShowFrom = sDetail.m_strFromName.empty() ? sDetail.m_strFromUserName : sDetail.m_strFromName;
    SetWindowTitle(strShowFrom.data());

    strShowTo = LANG_TRANSLATE(TRID_TO).toUtf8().data();
    strShowTo += " : ";
    strShowTo += sDetail.m_strToName.empty() ? sDetail.m_strToUserName : sDetail.m_strToName;
    m_pLabelToAddr->setText(QString::fromUtf8(strShowTo.c_str()));

    strShowTime = LANG_TRANSLATE(TRID_TIME).toUtf8().data();
    strShowTime += " : ";
    strShowTime += sDetail.m_strTime;
    m_pLabelTime->setText(QString::fromUtf8(strShowTime.c_str()));

    m_pTextEditor->setText(QString::fromUtf8(sDetail.m_strText.c_str()));
}

bool CDlgMessageView::GetSoftkeyData(CArraySoftKeyBarData & objWndData)
{
    objWndData[0].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_DELETE);
    objWndData[0].m_strSoftkeyAction = DM_SOFTKEY_CLICK_DELETE;
    objWndData[0].m_strIconPath = PIC_SOFTKEY_REMOVE;

    if (kszCAInBox == m_strCurrentBox)
    {
        objWndData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_REPLAY);
        objWndData[1].m_strSoftkeyAction = DM_SOFTKEY_CLICK_REPLY;
        objWndData[1].m_strIconPath = PIC_SOFTKEY_MSG_REPLY;
    }
    else if (kszCASentBox == m_strCurrentBox || kszCAOutBox == m_strCurrentBox)
    {
        objWndData[1].m_strSoftkeyTitle = LANG_TRANSLATE(TRID_FORWARD);
        objWndData[1].m_strSoftkeyAction = DM_SOFTKEY_CLICK_FORWARD;
        objWndData[1].m_strIconPath = PIC_SOFTKEY_MSG_FORWARD;
    }
    else
    {
        //
    }

    objWndData[2].m_strSoftkeyTitle = "";
    objWndData[2].m_strSoftkeyAction = "";

    objWndData[3].m_strSoftkeyTitle = "";
    objWndData[3].m_strSoftkeyAction = "";

    return true;
}

bool CDlgMessageView::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (pObj == NULL
            || pEvent == NULL)
    {
        return true;
    }

    //判断事件类型
    switch (pEvent->type())
    {
    case QEvent::Show:
        {
            emit RefreshSoftkey();
        }
    //键盘输入事件
    case  QEvent::KeyPress:
        {
            QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
            switch (pKeyEvent->key())
            {
            case PHONE_KEY_CANCEL:
                {
                    OnBtnBackClick();
                    return true;
                }
                break;
            default:
                break;
            }
        }
    }

    return CDlgBaseSubPage::eventFilter(pObj, pEvent);
}

//打开编辑窗口
void CDlgMessageView::OpenEditPage()
{
    SubMenuData subData;
    subData.strMenuAction = "OpenSubPage(text_message_edit_message)";
    emit openpage(subData);
}

bool CDlgMessageView::OnAction(const QString & strAction)
{
    if (strAction == DM_SOFTKEY_CLICK_REPLY)
    {
        //回复
        OnReplyBtnClicked();
        return true;
    }
    else if (strAction == DM_SOFTKEY_CLICK_FORWARD)
    {
        //转发
        OnForwardBtnClicked();
        return true;
    }
    else if (strAction == DM_SOFTKEY_CLICK_DELETE)
    {
        //删除该条短信
        OnDeleteBtnClicked();
        return true;
    }
    else
    {
        return false;
    }
}

//回复
void CDlgMessageView::OnReplyBtnClicked()
{
    OpenEditPage();
}

//转发
void CDlgMessageView::OnForwardBtnClicked()
{
    OpenEditPage();
}

//删除
void CDlgMessageView::OnDeleteBtnClicked()
{
    MessageBox_ShowCommonMsgBox(this,
                                LANG_TRANSLATE(TRID_IF_DELETE_THIS_MESSAGE),
                                MESSAGEBOX_OkCancel, -1,  "", "",
                                MSGBOX_ID_DELETE);
}

void CDlgMessageView::OnBtnBackClick()
{
    emit BtnBackClick();
}

void CDlgMessageView::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox)
    {
        return;
    }

    int nResult = pMessageBox->GetResult();
    switch (pMessageBox->GetID())
    {
    case MSGBOX_ID_DELETE:
        {
            if (MessageBox_OK == nResult)
            {
                g_pMsgUILogic->DeleteMessage(m_iMsgIdx);

                OnBtnBackClick();
            }
        }
        break;

    default:
        break;
    }
}

void CDlgMessageView::paintEvent(QPaintEvent * event)
{
    //画分割线
    QStylePainter stylePainter(this);
    stylePainter.setPen(Qt::lightGray);
    stylePainter.drawLine(LINE_LEFT, LINE_TOP, LINE_LENGTH, LINE_TOP);
}

