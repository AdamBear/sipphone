#include "dlginput.h"
#include "xmlbrowserui/xmlwidgetpool.h"
#include <QEvent>
#include "uimanager/rectdefine.h"
#include "inputmethod/inputmethod.h"
#include "xbpositiondefine.h"

DlgInput::DlgInput(QWidget * pParent/* = 0*/)
    : QFrame(pParent)
    , m_pScrollPage(NULL)
    , m_pInputFrame(NULL)
{
    setObjectName("DlgInput");
    // 初始化滚动条ScrollPage
    m_pScrollPage = new CScrollPage(this);
    if (m_pScrollPage != NULL)
    {
        m_pScrollPage->setObjectName("ScrollPage");
        //设置滚动条坐标、大小
        m_pScrollPage->setGeometry(0, 0, RECT_XMLBrowser.width()
                                   , RECT_XMLBrowser.height() - TITLE_HEIGHT);

        // 设置ScrollPage背景透明
        QPalette pl = m_pScrollPage->palette();
        pl.setBrush(QPalette::Background, QBrush(QColor(255, 0, 0, 0)));
        m_pScrollPage->setPalette(pl);
    }
    this->installEventFilter(this);

    m_pInputFrame = new CXMLInputFrame(m_pScrollPage);
    if (NULL != m_pInputFrame && NULL != m_pScrollPage)
    {
        m_pInputFrame->setObjectName("InputFrame");
        m_pScrollPage->SetChildPage(m_pInputFrame);
        //连接焦点改变信号
        connect(m_pInputFrame, SIGNAL(FocusWgtChange()), this, SIGNAL(FrameFocusChange()));
        //为滚动条连接信号，实现自动翻页
        connect(m_pInputFrame, SIGNAL(currentFocusWgtChange(QRect)), m_pScrollPage,
                SLOT(OnChildPageFocusWgtChange(QRect)));
    }
}

int DlgInput::GetFrameCurrentItem()
{
    //调用接口，返回当前焦点所在序号
    return m_pInputFrame->GetItemIndex();
}

DlgInput::~DlgInput()
{

}

void  DlgInput::resizeEvent(QResizeEvent * pEvent)
{
    int iHeight = pEvent->size().height();
    int iMore = iHeight % (LINEEDIT_HEIGHT + INPUT_FRAME_ROW_SPACING);
    iHeight -= iMore;
    this->setGeometry(this->geometry().x(), this->geometry().y() + iMore, this->width(), iHeight);
    if (NULL != m_pScrollPage)
    {
        m_pScrollPage->setGeometry(0, 0, this->width(), this->height());
    }
}

bool DlgInput::eventFilter(QObject * pObj, QEvent * pEvent)
{
    if (pEvent == NULL)
    {
        return false;
    }

    if (pEvent->type() == QEvent::Show)
    {
        QRect rect = this->geometry();
        if (NULL != m_pScrollPage)
        {
            rect = m_pScrollPage->geometry();
        }

        if (NULL != m_pInputFrame)
        {
            rect = m_pInputFrame->geometry();
        }
    }

    if (pEvent->type() == QEvent::KeyPress)
    {
        QKeyEvent * keyEvent = (QKeyEvent *)pEvent;
        if (NULL != m_pInputFrame)
        {
            m_pInputFrame->keyPressEvent(keyEvent);
        }

        return false;
    }

    return !FullKeyboardEnabled();
}

void DlgInput::InitInputFrame(std::list<InputField *> List_Input_Field)
{
    //调整滚动区大小，使其刚好容纳整数行
    int iHeight = this->size().height();
    iHeight -= iHeight % (LINEEDIT_HEIGHT + INPUT_FRAME_ROW_SPACING);
    m_pScrollPage->setGeometry(0, 0, this->width(), iHeight);

    if (NULL != m_pInputFrame)
    {
        //调用接口，传入参数生成input界面
        m_pInputFrame->AddItem(List_Input_Field);
    }
}

void DlgInput::SetFrameDisplayMode(INPUT_DISPLAY_MODE itemStyle)
{
    if (NULL != m_pInputFrame)
    {
        m_pInputFrame->SetDisplayMode(itemStyle);   //调用接口，设置显示类型
    }
}

bool  DlgInput::GetFrameLineEditValue(YLHashMap<int, yl::string> & PageData)
{
    if (NULL != m_pInputFrame)
    {
        return m_pInputFrame->GetLineEditValue(
                   PageData); //调用接口，返回map型的所有lineEdit数据
    }

    return false;
}

void DlgInput::SetFocusItem(int iIndex)
{
    if (NULL != m_pInputFrame)
    {
        m_pInputFrame->SetFrameFocusItem(iIndex);   //调用接口，设置默认焦点
    }
}

void DlgInput::OnKeyboardShow()
{
    if (m_pInputFrame != NULL)
    {
        m_pInputFrame->OnKeyboardShow();
    }
}

void DlgInput::OnKeyboardHide()
{
    if (m_pInputFrame != NULL)
    {
        m_pInputFrame->OnKeyboardHide();
    }
}
