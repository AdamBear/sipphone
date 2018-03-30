//mod-this(TalkUI)
#include "ctalkframetopbar.h"

//mod-UIKernel
#include "qtcommon/qmisc.h"
#include "baseui/fontcolor.h"
#include "customtheme/modcustomtheme.h"
#include "uikernel/languagehelper.h"
#include "uikernel/qwidgetlist.h"
#include "uikernel/qwidgetutility.h"
#include "uikernel/qdatetimelabel.h"

// mod-MainWnd
#include "mainwnd/accountmenuselector.h"

//mod-ImageManager
#include "imagemanager/modimagemanager.h"
// #include "account/accountmanager.h"
#include "account/include/modaccount.h"

// PicPath
#ifdef _T3X
#include "T3XStyleUI/T3XPicPath.h"
#endif

#ifdef _T4X
#include "baseui/t4xpicpath.h"
#endif

// 控件位置定义
const QRect LAB_CHANNEL_ICON_GEOMETRY(4, 4, 34, 34);
const QRect LAB_TITLE_GEOMETRY(300, 0, 200, 43);

CTalkFrameTopbar::CTalkFrameTopbar(QWidget * parent/* = 0*/)
    : QFrame(parent)
    , m_iAccountID(-1)
    , m_strChannelIconPath("")
    , m_pDateTime(NULL)
    , m_pLineBtn(NULL)
{
    InitData();
}

CTalkFrameTopbar::~CTalkFrameTopbar()
{

}

// 初始化数据
void CTalkFrameTopbar::InitData()
{
    m_labChannelIcon.setParent(this);
    m_labChannelIcon.setGeometry(LAB_CHANNEL_ICON_GEOMETRY);

    /*QFont font;
    font.setPointSize(18);
    font.setWeight(75);
    font.setBold(true);

    m_labTitle.setFont(font);*/

    CustomTheme_SetWidgetFontProperty(&m_labTitle, CUSTOM_FONT_ID_TITLEBAR_NORMAL, true, QFont::Bold);
    m_labTitle.setParent(this);
    m_labTitle.setGeometry(LAB_TITLE_GEOMETRY);
    m_labTitle.setAlignment(Qt::AlignCenter);

    m_pDateTime = new qDateTimeLabel(this);
    if (NULL != m_pDateTime)
    {
        m_pDateTime->setGeometry(568, 0, 224, 43);
        m_pDateTime->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        // 设置lab上的字的颜色
        CustomTheme_SetWidgetFontProperty(m_pDateTime, CUSTOM_FONT_ID_TITLEBAR_TIME, true, QFont::Bold);
    }

    m_pLineBtn = new qUIPushButton(this);
    if (NULL != m_pLineBtn)
    {
        m_pLineBtn->setGeometry(41, 0, 115, 43);
        m_pLineBtn->setTextColor(Qt::white);
        m_pLineBtn->setTextAlign(Qt::AlignLeft | Qt::AlignVCenter);
        connect(m_pLineBtn, SIGNAL(clicked()), this, SLOT(OnLineBtnClick()));
    }
}

void CTalkFrameTopbar::SetChannelIcon(const QString & strPath)
{
    if (m_strChannelIconPath != strPath)
    {
        m_strChannelIconPath = strPath;
        m_labChannelIcon.setPixmap(THEME_GET_BMP(m_strChannelIconPath.toUtf8().data()));
    }
}

void CTalkFrameTopbar::SetTitle(const yl::string & strTitle)
{
    m_labTitle.setText(LANG_TRANSLATE(strTitle.c_str()));
}

void CTalkFrameTopbar::SetLineBtnText(const QString & strText)
{
    if (NULL != m_pLineBtn)
    {
        m_pLineBtn->setText(strText);
    }
}

void CTalkFrameTopbar::SetLineBtnActive(bool bActive)
{
    if (NULL == m_pLineBtn)
    {
        return;
    }

    (bActive) ? m_pLineBtn->show() : m_pLineBtn->hide();
}

void CTalkFrameTopbar::SetAccountID(int iAccountID)
{
    m_iAccountID = iAccountID;
}

void CTalkFrameTopbar::OnLineBtnClick()
{
    m_accountSelector.Init();

    // 设置Index
    m_accountSelector.SetSelectIndexByAccountID(m_iAccountID);

    if (m_accountSelector.GetAccountSize() <= 0)
    {
        return;
    }

    QPoint ptMenu(0, 0);

    MessageBox_ShowTitleMenuBox(this, &m_accountSelector, ptMenu);
}

void CTalkFrameTopbar::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    int nAccountId = m_accountSelector.GetAccountId();

    if (-1 != nAccountId)
    {
        emit SelectUserLine(nAccountId);
    }
}


