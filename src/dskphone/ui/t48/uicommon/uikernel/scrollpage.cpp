#include "scrollpage.h"
#include "imagemanager/modimagemanager.h"
#include <QScrollBar>
#include <QMouseEvent>
#include <QStylePainter>
#include <QStyleOptionSlider>
#include "keymap.h"
#include "baseui/t4xpicpath.h"
#include "baseui/pagectrl.h"
#include "qtcommon/qmisc.h"
#include "qtcommon/xmlhelper.h"

// 滚动条按钮图片的高
const int SCROLL_ICON_HEIGHT = 34;

// 滚动条按钮图片的宽
const int SCROLL_ICON_WIDTH  = 49;

// 绘制向上向下按钮的偏移值
const int DRAW_ARROW_UP_OFFSET = 6;
const int DRAW_ARROW_DOWN_OFFSET = 4;

// 扩大的滚动条上下按钮点击范围
const int INCREASE_CLICK_RANGE = 40;

const int SCROLL_BAR_LEFT_SPACING = 3;
const int SCROLL_BAR_TOP_SPACING = 0;
const int SCROLL_BAR_BOTTOM_SPACING = 4;

#define SCROLL_CTRL_DEFAULT_WIDTH 48
#define SCROLL_CTRL_BG_TOP_HEIGHT 28
#define SCROLL_CTRL_BG_MID_HEIGHT 26
#define SCROLL_CTRL_BG_BOTTOM_HEIGHT 28

CT3XScrollPage::CT3XScrollPage(QWidget * parent)
    : QScrollArea(parent)
    , m_iSingleStep(30)
{
    setFrameShape(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);
    setFocusPolicy(Qt::NoFocus);

    QScrollBar * pScrollBar = new QScrollBar(this);
    if (NULL != pScrollBar)
    {
        // 设置滚动条名字
        pScrollBar->setObjectName(QString::fromUtf8("QScrollBar"));

        // 设置为ScrollPage的滚动条
        this->setVerticalScrollBar(pScrollBar);
    }

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);// 设置横向滚动条永远关闭
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);// 设置竖向滚动条永远开启
}

CT3XScrollPage::~CT3XScrollPage()
{

}

bool CT3XScrollPage::SetChildPage(QWidget * pChildPage, int nHeight/* = 0*/,
                                  bool bAutoResetGeometry/* = false*/)
{
    if (NULL == pChildPage)
    {
        return false;
    }

    // 判断是否已有子页面
    if (this->widget() != NULL)
    {
        // 移除之前的子页面
        this->takeWidget();
    }

    bool bResetGeometry = false;// 是否重设控件位置
    QRect rect = this->geometry();// 获得控件的当前位置

    // 判断是否自动设置控件位置
    if (bAutoResetGeometry)
    {
        bResetGeometry = true;
        QRect rectChidlPage = pChildPage->geometry();// 获得子控件的位置
        rect.setX(rectChidlPage.x());
        rect.setY(rectChidlPage.y());
        rect.setWidth(rect.width() + 15);// 增加宽度以预留滚动条的宽度
    }

    // 判断是否重设子页面的高度(即控件的高度)
    if (nHeight > 0)
    {
        bResetGeometry = true;
        rect.setHeight(nHeight);// 重设控件的高度
    }

    // 判断是否需要重设控件位置
    if (bResetGeometry)
    {
        this->setGeometry(rect);
    }

    pChildPage->setAutoFillBackground(true);// 添加进QScrollArea的页面必须有这个属性
    this->setWidget(pChildPage);// 设置新的子页面
    return true;
}

// 按键处理
bool CT3XScrollPage::ProcessPhonekey(int iKey)
{
    bool bHandled = true;

    if (iKey == PHONE_KEY_UP)
    {
        verticalScrollBar()->setValue(verticalScrollBar()->value() - m_iSingleStep);
    }
    else if (iKey == PHONE_KEY_DOWN)
    {
        verticalScrollBar()->setValue(verticalScrollBar()->value() + m_iSingleStep);
    }
    else if (iKey == PHONE_KEY_LEFT)
    {
        verticalScrollBar()->setValue(verticalScrollBar()->value() - m_iPageStep);
    }
    else if (iKey == PHONE_KEY_RIGHT)
    {
        verticalScrollBar()->setValue(verticalScrollBar()->value() + m_iPageStep);
    }
    else
    {
        bHandled = false;
    }

    return bHandled;
}

void CT3XScrollPage::SetSingleStep(int iSingleStep)
{
    if (iSingleStep > 0)
    {
        m_iSingleStep = iSingleStep;
    }
}

void CT3XScrollPage::SetPageStep(int iPageStep)
{
    if (iPageStep > 0)
    {
        m_iPageStep = iPageStep;
    }
}

void CT3XScrollPage::AutoResizeByHeight(int iMaxHeight, int iRowHeight)
{
    if (iMaxHeight > 0 && iRowHeight > 0)
    {
        this->resize(this->width(), (iMaxHeight / iRowHeight) * iRowHeight);
    }
}
/************************************************************************/
/*                             about scrollPage                         */
/************************************************************************/
#ifdef _T48
CScrollPage::CScrollPage(QWidget * parent)
    : QWidget(parent)
    , IScrollNotify()
    , m_iSingleStep(0)
    , m_iPageStep(0)
{
    this->setObjectName("CScrollPage");
    m_pScrollBar = new CPageCtrl(this, this);

    if (NULL != m_pScrollBar)
    {
        m_pScrollBar->setObjectName("ScrollPageBar");
    }

    m_pScrollArea = new QScrollArea(this);
    if (NULL != m_pScrollArea)
    {
        m_pScrollArea->setFocusPolicy(Qt::NoFocus);
        m_pScrollArea->setHorizontalScrollBarPolicy(
            Qt::ScrollBarAlwaysOff);// 设置横向滚动条永远关闭
        m_pScrollArea->setVerticalScrollBarPolicy(
            Qt::ScrollBarAlwaysOn);// 设置竖向滚动条永远开启
        QScrollBar * pVscroll = m_pScrollArea->verticalScrollBar();
        if (NULL != pVscroll)
        {
            pVscroll->hide();
            connect(pVscroll, SIGNAL(rangeChanged(int, int)), this, SLOT(OnScrollRangeChanged(int, int)));
        }
    }
    this->installEventFilter(this);
}

CScrollPage::~CScrollPage()
{

}

QWidget * CScrollPage::TakeChildPage()
{
    QWidget * pWidget = NULL;
    if (NULL != m_pScrollArea)
    {
        pWidget = m_pScrollArea->widget();
        if (pWidget != NULL)
        {
            disconnect(pWidget, SIGNAL(currentFocusWgtChange(QRect)));
        }

        m_pScrollArea->takeWidget();
        RefreshPageCtrlInfo();
    }

    return pWidget;
}

bool CScrollPage::SetChildPage(QWidget * pChildPage, int nHeight/* = 0*/,
                               bool bAutoResetGeometry/* = false*/)
{
    if (NULL == pChildPage || NULL == m_pScrollArea)
    {
        return false;
    }

    TakeChildPage();

    bool bResetGeometry = false;// 是否重设控件位置
    QRect rect = m_pScrollArea->geometry();// 获得控件的当前位置

    // 判断是否自动设置控件位置
    if (bAutoResetGeometry)
    {
        bResetGeometry = true;
        QRect rectChidlPage = pChildPage->geometry();// 获得子控件的位置
        rect.setX(rectChidlPage.x());
        rect.setY(rectChidlPage.y());
        rect.setWidth(rect.width());// 增加宽度以预留滚动条的宽度
    }

    // 判断是否重设子页面的高度(即控件的高度)
    if (nHeight > 0)
    {
        bResetGeometry = true;
        rect.setHeight(nHeight);// 重设控件的高度
    }

    // 判断是否需要重设控件位置
    if (bResetGeometry)
    {
        m_pScrollArea->setGeometry(rect);
        if (NULL != m_pScrollArea->verticalScrollBar())
        {
            m_pScrollArea->verticalScrollBar()->setPageStep(m_iPageStep > 0 ? m_iPageStep :
                    m_pScrollArea->height());
            m_pScrollArea->verticalScrollBar()->setSingleStep(m_iSingleStep);
            RefreshPageCtrlInfo();
        }
    }

    pChildPage->setAutoFillBackground(true);// 添加进QScrollArea的页面必须有这个属性
    m_pScrollArea->setWidget(pChildPage);// 设置新的子页面

    RefreshPageCtrlInfo();
    return true;
}

void CScrollPage::resizeEvent(QResizeEvent * event)
{
    if (m_pScrollArea != NULL && m_pScrollBar != NULL)
    {
        QSize size = event->size();
        QSize Osize = event->oldSize();

        m_pScrollBar->SetRect(width() - SCROLL_ICON_WIDTH - SCROLL_BAR_LEFT_SPACING, SCROLL_BAR_TOP_SPACING,
                              SCROLL_ICON_WIDTH, this->height() - SCROLL_BAR_BOTTOM_SPACING - SCROLL_BAR_TOP_SPACING);

        m_pScrollArea->setGeometry(0, 0, width(), height());
        m_pScrollBar->raise();

        QScrollBar * pVscroll = m_pScrollArea->verticalScrollBar();
        if (NULL != pVscroll)
        {
            pVscroll->setPageStep(m_iPageStep > 0 ? m_iPageStep :
                                  m_pScrollArea->height());// 设置滚动条的最小步长
            pVscroll->setSingleStep(m_iSingleStep);
            RefreshPageCtrlInfo();
        }
    }

    /*if (event != NULL)
    {
        // 重新设置滚动条点击按钮移动的长度
        QSize objSize = event->size();
        int iHeight = objSize.height();
        this->verticalScrollBar()->setPageStep(iHeight);

        // 调用父控件的resizeEvent函数处理剩下操作
        QScrollArea::resizeEvent(event);
        RefreshPageCtrlInfo();
    }*/
}

// 全键盘隐藏 重置界面
void CScrollPage::RefreshCurPage()
{
    QScrollBar* pScrollBar = m_pScrollArea->verticalScrollBar();
    if (NULL != m_pScrollBar && NULL != pScrollBar)
    {
        int iCurrentPage = pScrollBar->value() / pScrollBar->pageStep() + 1;// 计算当前所在页面
        QWidget* pWgt = m_pScrollArea->takeWidget();
        m_pScrollArea->setWidget(pWgt);
        SetPage(iCurrentPage);
    }
}

void CScrollPage::RefreshPageCtrlInfo()
{
    if (m_pScrollArea == NULL)
    {
        return;
    }

    QScrollBar * pScrollBar = m_pScrollArea->verticalScrollBar();
    if (NULL != m_pScrollBar && NULL != pScrollBar)
    {
        int iMaxPage = (pScrollBar->maximum() - 1) / pScrollBar->pageStep() + 1;// 计算总页数
        int iCurrentPage = pScrollBar->value() / pScrollBar->pageStep() + 1;// 计算当前所在页面
        m_pScrollBar->SetScrollInfo(iMaxPage, iCurrentPage - 1);
        m_pScrollBar->update();
        this->update();
        /*if (iMaxPage <= 1)
        {
            m_pScrollBar->hide();
        }
        else
        {
            m_pScrollBar->show();
        }*/
    }
}

void CScrollPage::OnScrollRangeChanged(int min, int max)
{
    QScrollBar * pScrollBar = m_pScrollArea->verticalScrollBar();
    if (NULL != m_pScrollBar && NULL != pScrollBar && m_pScrollArea->widget() != NULL)
    {
        pScrollBar->setMaximum(m_pScrollArea->widget()->height());
        RefreshPageCtrlInfo();
    }
}

void CScrollPage::SetSingleStep(int iStep)
{
    m_iSingleStep = iStep;
    if (m_pScrollArea != NULL && m_pScrollArea->verticalScrollBar())
    {
        m_pScrollArea->verticalScrollBar()->setSingleStep(m_iSingleStep);
    }
}

void CScrollPage::SetPageStep(int iStep)
{
    m_iPageStep = iStep;
    if (m_pScrollArea != NULL && m_pScrollArea->verticalScrollBar() && m_iSingleStep > 0)
    {
        m_pScrollArea->verticalScrollBar()->setPageStep(m_iSingleStep);
        RefreshPageCtrlInfo();
    }
}

// 通知向前滚动
void CScrollPage::OnNextPage()
{
    OnNotifyScrollNext();
}

// 通知向后滚动
void CScrollPage::OnPrePage()
{
    OnNotifyScrollPrevious();
}

// 通知翻到前一页
void CScrollPage::OnNotifyScrollPrevious()
{
    QScrollBar * pScrollBar = m_pScrollArea->verticalScrollBar();
    QWidget  *  pWidget    = m_pScrollArea->widget();
    if (pScrollBar != NULL)
    {
        if (pScrollBar->value() == pScrollBar->minimum())
        {
            // 若scroll已经到达最大值，则设置其为最小值实现循环滚动
            int iMaxPage = (pScrollBar->maximum() - 1) / pScrollBar->pageStep() + 1;// 计算总页数
            SetPage(iMaxPage);
        }
        else
        {
            int iCurrentPage = pScrollBar->value() / pScrollBar->pageStep() + 1;// 计算当前所在页面
            SetPage(iCurrentPage - 1);
        }
    }
}

// 通知翻到下一页
void CScrollPage::OnNotifyScrollNext()
{
    if (m_pScrollArea == NULL)
    {
        return;
    }

    QScrollBar * pScrollBar = m_pScrollArea->verticalScrollBar();
    QWidget  *  pWidget    = m_pScrollArea->widget();
    if (pScrollBar != NULL && pWidget != NULL)
    {
        int iMaxPage = (pScrollBar->maximum() - 1) / pScrollBar->pageStep() + 1;// 计算总页数
        int iCurrentPage = pScrollBar->value() / pScrollBar->pageStep() + 1;// 计算当前所在页面
        if (iMaxPage == iCurrentPage /*pScrollBar->value() == pScrollBar->maximum()*/)
        {
            // 若scroll已经到达最小值，则设置其为最小值实现循环滚动
            SetPage(1);
        }
        else
        {
            SetPage(iCurrentPage + 1);
        }
    }
}

void CScrollPage::SetPage(int iIndex)
{
    QScrollBar * pScrollBar = m_pScrollArea->verticalScrollBar();
    if (NULL != pScrollBar)
    {
        pScrollBar->setPageStep(m_iPageStep > 0 ? m_iPageStep : m_pScrollArea->height());
        pScrollBar->setSingleStep(m_iSingleStep);
        int iMaxPage = (pScrollBar->maximum() - 1) / pScrollBar->pageStep() + 1;// 计算总页数

        if (iIndex > 0 && iIndex <= iMaxPage)
        {
            int iCurrentPage = pScrollBar->value() / pScrollBar->pageStep() + 1;// 计算当前所在页面
            if (iIndex != iCurrentPage)
            {
                pScrollBar->setValue((iIndex - 1) * pScrollBar->pageStep());
                emit PageChange(iIndex);
                RefreshPageCtrlInfo();
            }
        }
    }
}

bool CScrollPage::eventFilter(QObject * pObj, QEvent * pEvent)
{
    return QWidget::eventFilter(pObj, pEvent);
}

void CScrollPage::OnChildPageFocusWgtChange(QRect wgtRect)
{
    if (NULL == m_pScrollArea)
    {
        return;
    }

    QWidget * pWgt = m_pScrollArea->widget();
    QScrollBar * pScrollBar = m_pScrollArea->verticalScrollBar();
    if (NULL != pWgt && NULL != pScrollBar)
    {
        int iY = wgtRect.y();
        int iWgtPage = iY / pScrollBar->pageStep() + 1;// 计算控件当前所在页面
        SetPage(iWgtPage);
    }
}
#else //_T48

CScrollPage::CScrollPage(QWidget * parent) : QScrollArea(parent)
{
    this->setObjectName("CScrollPage");

#ifdef _VPX
    CMyScrollBar * pScrollBar = new CMyScrollBar();
#else
    QScrollBar * pScrollBar = new QScrollBar();
#endif

    if (NULL != pScrollBar)
    {
#ifdef _VPX
        pScrollBar->DisplayPageInfo(true);
        pScrollBar->SetBackgroundHeightOffSet(ScrollPageFrameHeight);
#endif

        this->setObjectName("CScrollPage");
        this->setFocusPolicy(Qt::NoFocus);

        this->setVerticalScrollBar(pScrollBar);// 将控件内的竖直的滚动条

#ifdef _VPX
        pScrollBar->setSingleStep(this->height());// 设置滚动条的最小步长
        ScrollBarFilter * pFilter = new ScrollBarFilter(pScrollBar, pScrollBar);
        if (NULL != pFilter)
        {
            pScrollBar->installEventFilter(pFilter);// 给滚动条安装过滤器
        }
#endif
        // 连接相关的信号和槽
        connect(pScrollBar, SIGNAL(ClickUp()), this, SLOT(OnScrollBarClickUp()));
        connect(pScrollBar, SIGNAL(ClickDown()), this, SLOT(OnScrollBarClickDown()));
    }

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);// 设置横向滚动条永远关闭
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);// 设置竖向滚动条永远开启
}

CScrollPage::~CScrollPage()
{

}

QWidget * CScrollPage::TakeChildPage()
{
    QWidget * pWidget = this->widget();

    // 判断是否已有子页面
    if (pWidget != NULL)
    {
        if (pWidget->inherits("CDlgBaseSubPage"))
        {
            disconnect(pWidget, SIGNAL(currentFocusWgtChange(QRect)));
        }

        // 移除之前的子页面
        this->takeWidget();
    }

    return pWidget;
}

bool CScrollPage::SetChildPage(QWidget * pChildPage, int nHeight/* = 0*/,
                               bool bAutoResetGeometry/* = false*/)
{
    if (NULL == pChildPage)
    {
        return false;
    }

    TakeChildPage();

    // 链接相关的信号和槽
    if (pChildPage->inherits("CDlgBaseSubPage"))
    {
        connect(pChildPage, SIGNAL(currentFocusWgtChange(QRect)), this,
                SLOT(OnChildPageFocusWgtChange(QRect)));
    }

    bool bResetGeometry = false;// 是否重设控件位置
    QRect rect = this->geometry();// 获得控件的当前位置

    // 判断是否自动设置控件位置
    if (bAutoResetGeometry)
    {
        bResetGeometry = true;
        QRect rectChidlPage = pChildPage->geometry();// 获得子控件的位置
        rect.setX(rectChidlPage.x());
        rect.setY(rectChidlPage.y());
        rect.setWidth(rect.width() + SCROLL_ICON_WIDTH);// 增加宽度以预留滚动条的宽度
    }

    // 判断是否重设子页面的高度(即控件的高度)
    if (nHeight > 0)
    {
        bResetGeometry = true;
        rect.setHeight(nHeight);// 重设控件的高度
    }

    // 判断是否需要重设控件位置
    if (bResetGeometry)
    {
        this->setGeometry(rect);
    }

    pChildPage->setAutoFillBackground(true);// 添加进QScrollArea的页面必须有这个属性
    this->setWidget(pChildPage);// 设置新的子页面
    return true;
}

void CScrollPage::resizeEvent(QResizeEvent * event)
{
    if (event != NULL)
    {
        // 重新设置滚动条点击按钮移动的长度
        int iHeight = this->height() - ScrollPageFrameHeight;
        this->verticalScrollBar()->setSingleStep(iHeight);

        // 调用父控件的resizeEvent函数处理剩下操作
        QScrollArea::resizeEvent(event);
    }
}

void CScrollPage::OnChildPageFocusWgtChange(QRect wgtRect)
{
    QWidget * pWgt = this->widget();
    QScrollBar * pScrollBar = verticalScrollBar();
    if (NULL != pWgt && NULL != pScrollBar)
    {
        int iY = wgtRect.y();
        int iWgtPage = iY / pScrollBar->singleStep() + 1;// 计算控件当前所在页面
        SetPage(iWgtPage);
    }
}

void CScrollPage::SetPage(int iIndex)
{
    QScrollBar * pScrollBar = verticalScrollBar();
    if (NULL != pScrollBar)
    {
        int iMaxPage = pScrollBar->maximum() / pScrollBar->singleStep() + 1;// 计算总页数

        if (iIndex > 0 && iIndex <= iMaxPage)
        {
            int iCurrentPage = pScrollBar->value() / pScrollBar->singleStep() + 1;// 计算当前所在页面
            if (iIndex != iCurrentPage)
            {
                pScrollBar->setValue((iIndex - 1) * pScrollBar->singleStep());
                emit PageChange(iIndex);
            }
        }
    }
}

void CScrollPage::OnScrollBarClickUp()
{
    QScrollBar * pScrollBar = verticalScrollBar();
    QWidget  *  pWidget    = this->widget();
    if (pScrollBar != NULL)
    {
        if (pScrollBar->value() == pScrollBar->minimum())
        {
            // 若scroll已经到达最大值，则设置其为最小值实现循环滚动
            int iMaxPage = pScrollBar->maximum() / pScrollBar->singleStep() + 1;// 计算总页数
            SetPage(iMaxPage);
        }
        else
        {
            int iCurrentPage = pScrollBar->value() / pScrollBar->singleStep() + 1;// 计算当前所在页面
            SetPage(iCurrentPage - 1);
        }
        //设置按下效果
    }
}

void CScrollPage::OnScrollBarClickDown()
{
    QScrollBar * pScrollBar = verticalScrollBar();
    QWidget  *  pWidget    = this->widget();
    if (pScrollBar != NULL && pWidget != NULL)
    {
        if (pScrollBar->value() == pScrollBar->maximum())
        {
            // 若scroll已经到达最小值，则设置其为最小值实现循环滚动
            SetPage(1);
        }
        else
        {
            int iCurrentPage = pScrollBar->value() / pScrollBar->singleStep() + 1;// 计算当前所在页面
            SetPage(iCurrentPage + 1);
        }
    }
}
#endif //_T48

/************************************************************************/
/*                             about ScrollBarFilter                    */
/************************************************************************/
ScrollBarFilter::ScrollBarFilter(QScrollBar * pScrollBar, QObject * parent/* = 0*/)
    : QObject(parent)
{
    // 保存父控件指针
    if (NULL != pScrollBar)
    {
        m_pScrollBar = pScrollBar;
        connect(this, SIGNAL(ClickDown()), m_pScrollBar, SIGNAL(ClickDown()));
        connect(this, SIGNAL(ClickUp()), m_pScrollBar, SIGNAL(ClickUp()));
        connect(this, SIGNAL(ReleaseClick()), m_pScrollBar, SIGNAL(ReleaseClick()));
    }

    // 设置默认的图片高度
    m_nScrollIconHeight = SCROLL_ICON_HEIGHT;
}

ScrollBarFilter::~ScrollBarFilter()
{

}

void ScrollBarFilter::SetScrollIconHeight(int nIconHeight)
{
    m_nScrollIconHeight = nIconHeight;
}

bool ScrollBarFilter::eventFilter(QObject * target, QEvent * event)
{
    // 入口参数判断
    if (target == NULL || m_pScrollBar == NULL || event == NULL)
    {
        return true;
    }

    if (event->type() == QEvent::MouseMove
            || event->type() == QEvent::MouseButtonPress
            || event->type() == QEvent::MouseButtonRelease
            || event->type() == QEvent::MouseButtonDblClick)
    {
        // 只处理点击事件
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent * pMouseEvent = (QMouseEvent *)event;

            // 获得点击事件在控件内的位置
            QPoint p = pMouseEvent->pos();

            // 判断位置是否在滚动条的按钮上
            if (p.y() < DRAW_ARROW_UP_OFFSET + SCROLL_ICON_HEIGHT + INCREASE_CLICK_RANGE
                    || p.y() > m_pScrollBar->height() - DRAW_ARROW_DOWN_OFFSET - SCROLL_ICON_HEIGHT -
                    INCREASE_CLICK_RANGE)
            {
                if (p.y() > m_pScrollBar->height() / 2)// 点击了向下的按钮
                {
                    emit ClickDown();
                }
                else// 点击了向上的按钮
                {
                    emit ClickUp();
                }

                return true;
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            // 不用判断位置是否在滚动条的按钮上
            emit ReleaseClick();

            return true;
        }

        return true;
    }

    return false;
}

/************************************************************************/
/*                             about ViewScrollBarFilter                */
/************************************************************************/
// View类的滚动条过滤器(QTabWidget、QCombobox等)
// 用来实现点击向下跳跃N个Item
ViewScrollBarFilter::ViewScrollBarFilter(QScrollBar * pScrollBar, QObject * parent/* = 0*/)
    : QObject(parent)
    , m_pScrollBar(NULL)
    , m_iSingStep(1)
{
    m_pScrollBar = pScrollBar;
    connect(this, SIGNAL(ClickDown()), m_pScrollBar, SIGNAL(ClickDown()));
    connect(this, SIGNAL(ClickUp()), m_pScrollBar, SIGNAL(ClickUp()));
    connect(this, SIGNAL(ReleaseClick()), m_pScrollBar, SIGNAL(ReleaseClick()));
}

void ViewScrollBarFilter::SetSingleStep(int iStep)
{
    if (iStep > 0 && iStep != m_iSingStep)
    {
        m_iSingStep = iStep;
    }
}

ViewScrollBarFilter::~ViewScrollBarFilter()
{

}

bool ViewScrollBarFilter::eventFilter(QObject * target, QEvent * event)
{
    // 入口参数判断
    if (target == NULL || m_pScrollBar == NULL)
    {
        return false;
    }

    // 勾住所有的鼠标事件
    if (event->type() == QEvent::MouseMove
            || event->type() == QEvent::MouseButtonPress
            || event->type() == QEvent::MouseButtonRelease
            || event->type() == QEvent::MouseButtonDblClick)
    {
        // 只处理点击事件
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent * pMouseEvent = (QMouseEvent *)event;

            // 获得点击事件在控件内的位置
            QPoint p = pMouseEvent->pos();

            // 判断位置是否在滚动条的按钮上
            if (p.y() < DRAW_ARROW_UP_OFFSET + SCROLL_ICON_HEIGHT + INCREASE_CLICK_RANGE
                    || p.y() > m_pScrollBar->height() - DRAW_ARROW_DOWN_OFFSET - SCROLL_ICON_HEIGHT -
                    INCREASE_CLICK_RANGE)
            {
                int nValue = m_pScrollBar->value();
                int nSetValue = nValue;

                if (p.y() > m_pScrollBar->height() / 2)// 点击了向下的按钮
                {
                    //  nSetValue = min(nValue + m_iSingStep, m_pScrollBar->maximum());
                    if (nSetValue == m_pScrollBar->maximum())
                    {
                        nSetValue = m_pScrollBar->minimum();
                    }
                    else
                    {
                        nSetValue = min(nValue + m_iSingStep, m_pScrollBar->maximum());
                    }
                    emit ClickDown();
                }
                else// 点击了向上的按钮
                {
                    // nSetValue = max(nValue - m_iSingStep, m_pScrollBar->minimum());
                    if (nSetValue == m_pScrollBar->minimum())
                    {
                        nSetValue = m_pScrollBar->maximum();
                    }
                    else
                    {
                        nSetValue = max(nValue - m_iSingStep, m_pScrollBar->minimum());
                    }
                    emit ClickUp();
                }
                m_pScrollBar->setValue(nSetValue);
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            // 不用判断位置是否在滚动条的按钮上
            emit ReleaseClick();
        }

        return true;
    }

    return false;
}

/************************************************************************/
/*                             about CMyScrollBar                       */
/************************************************************************/
CMyScrollBar::CMyScrollBar(QWidget * parent/* = 0*/)
    : QScrollBar(parent)
    , m_brushBG(QColor(72, 76, 81))
    , m_bDisplayPageInfo(true)
    , m_BackgroundHeightOffSet(0)
    , m_eArrowsDisplayType(ARROWS_ALWAYS_AS_NEEDED)
    , m_TextColor(Qt::white)
    , m_bIsClickUp(false)
    , m_bIsClickDown(false)
#ifdef IF_ENABLE_TESTABILITY
    , m_rcInfo(QRect())
    , m_rcBtnUp(QRect())
    , m_rcBtnDown(QRect())
#endif
{
    this->setObjectName("CMyScrollBar");

    connect(this, SIGNAL(ClickUp()), this, SLOT(OnClickUp()));
    connect(this, SIGNAL(ClickDown()), this, SLOT(OnClickDown()));
    connect(this, SIGNAL(ReleaseClick()), this, SLOT(OnReleaseClick()));
}

CMyScrollBar::~CMyScrollBar()
{

}

void CMyScrollBar::OnClickUp()
{
    m_bIsClickUp = true;
}

void CMyScrollBar::OnClickDown()
{
    m_bIsClickDown = true;
}

void CMyScrollBar::OnReleaseClick()
{
    m_bIsClickUp = false;
    m_bIsClickDown = false;
    this->update();
}

void CMyScrollBar::SetBackgroundBrush(const QBrush & brushBG)
{
    m_brushBG = brushBG;
}

void CMyScrollBar::DisplayPageInfo(bool bOpen)
{
    m_bDisplayPageInfo = bOpen;
}

void CMyScrollBar::SetBackgroundHeightOffSet(int iOffset)
{
    m_BackgroundHeightOffSet = iOffset;
}

void CMyScrollBar::SetScrollText(const QString & strText)
{
    m_strText = strText;
}

void CMyScrollBar::SetTextColor(const QColor & objColor)
{
    m_TextColor = objColor;
}

void CMyScrollBar::SetArrowsDisplayType(ARROWS_DISPLAR_TYPE eArrowsDisplayType)
{
    m_eArrowsDisplayType = eArrowsDisplayType;
}

void CMyScrollBar::GetArrowsImageByDisplayType(QPixmap & pixmapArrowUp, QPixmap & pixmapArrowsDown)
{
    LPCSTR lpszArrowsDown;
    LPCSTR lpszArrowUp;

    if (m_eArrowsDisplayType == ARROWS_ALWAYS_ON)
    {
        // 根据按下状态设置图片
        if (m_bIsClickUp)
        {
            lpszArrowUp = PIC_ARROW_UP1;
        }
        else
        {
            lpszArrowUp = PIC_ARROW_UP2;
        }
        if (m_bIsClickDown)
        {
            lpszArrowsDown = PIC_ARROW_DOWN1;
        }
        else
        {
            lpszArrowsDown = PIC_ARROW_DOWN2;
        }
    }
    else if (m_eArrowsDisplayType == ARROWS_ALWAYS_OFF)
    {
        // 设置两个图标置灰
        lpszArrowsDown = PIC_ARROW_DOWN1;
        lpszArrowUp = PIC_ARROW_UP1;
    }
    else
    {
        if (this->minimum() == this->maximum())
        {
            // 但子页面只有一页的时候两个图标置灰
            lpszArrowsDown = PIC_ARROW_DOWN1;
            lpszArrowUp = PIC_ARROW_UP1;
        }
        else
        {
            // 根据按下状态设置图片
            if (m_bIsClickUp)
            {
                lpszArrowUp = PIC_ARROW_UP1;
            }
            else
            {
                lpszArrowUp = PIC_ARROW_UP2;
            }
            if (m_bIsClickDown)
            {
                lpszArrowsDown = PIC_ARROW_DOWN1;
            }
            else
            {
                lpszArrowsDown = PIC_ARROW_DOWN2;
            }
        }
    }

    pixmapArrowUp = THEME_GET_BMP(lpszArrowUp);
    pixmapArrowsDown = THEME_GET_BMP(lpszArrowsDown);
}

void CMyScrollBar::EmitSignal(const QString & type)
{
    if (type == QString("down"))
    {
        emit ClickDown();
    }
    else if (type == QString("up"))
    {
        emit ClickUp();
    }
    else
    {

    }
}

QFont CMyScrollBar::GetFontByText(QString & strText)
{
    // 设置字体基本属性
    QFont objFont;
    objFont.setWeight(75);
    objFont.setBold(true);
    objFont.setFamily("Tahoma");

    // 计算'/'后面数字的长度
    int iNumLen = strText.mid(strText.lastIndexOf('/') + 1).size();

    if (iNumLen == 0)
    {
        return objFont;
    }

    // 根据text长度设置字体大小
    if (iNumLen == 1)
    {
        objFont.setPointSize(18);
    }
    else if (iNumLen == 2)
    {
        objFont.setPointSize(14);
    }
    else if (iNumLen == 3)
    {
        objFont.setPointSize(10);
    }
    else
    {
        objFont.setPointSize(18);
    }

    // 计算需要补0的位数
    int iZeroLen = iNumLen + iNumLen + 1 - strText.size();

    // 补0
    if (iZeroLen > 0)
    {
        strText = QString(iZeroLen, '0') + strText;
    }

    return objFont;
}

void CMyScrollBar::paintEvent(QPaintEvent * event)
{
    QStyleOptionSlider option;
    initStyleOption(&option);// 初始化style Option的数据

    // 重设滚动条的显示高度
    option.rect.setHeight(option.rect.height() - m_BackgroundHeightOffSet);

    // 创建一个空的图片，在上面绘制内容
    QPixmap objPixmap(option.rect.size());
    QPainter PixmapPainter(&objPixmap);

    // 获取上下箭头的图片
    QPixmap pixmapDown; // 向下箭头图片
    QPixmap pixmapUp;   // 向上箭头图片
    GetArrowsImageByDisplayType(pixmapUp, pixmapDown);

    // 设置背景颜色为透明
    PixmapPainter.fillRect(option.rect, m_brushBG);
    QString strPageInfo;
    if (m_bDisplayPageInfo)// 判断是否自动绘制page的信息
    {
        // 计算当前页面的Index
        int iCurrentPage = this->value() / this->singleStep() + 1;

        // 计算总页数
        int iMaxPage = this->maximum() / this->singleStep() + 1;
        strPageInfo  =  QString::number(iCurrentPage) + "/" + QString::number(iMaxPage);
    }
    else if (!m_strText.isEmpty())
    {
        strPageInfo = m_strText;
    }
    else
    {
        // to do
    }

    PixmapPainter.setPen(m_TextColor);// 设置绘制内容的颜色
    PixmapPainter.setFont(GetFontByText(strPageInfo));// 设置绘制内容的字体
    PixmapPainter.drawText(option.rect, Qt::AlignCenter, strPageInfo);
#ifdef IF_ENABLE_TESTABILITY
    m_strInfo = strPageInfo;
    m_rcInfo = option.rect;
#endif
    // 画向上按钮背景图片
    if (!pixmapUp.isNull())
    {
        // 为了美观，X轴留几像素,Y轴留几像素
        PixmapPainter.drawPixmap(option.rect.x() + 3, option.rect.y() + DRAW_ARROW_UP_OFFSET, pixmapUp);
#ifdef IF_ENABLE_TESTABILITY
        m_rcBtnUp.setRect(option.rect.x() + 3, option.rect.y() + DRAW_ARROW_UP_OFFSET, pixmapUp.width(),
                          pixmapUp.height());
#endif
    }

    // 画向下按钮背景图片
    if (!pixmapDown.isNull())
    {
        // 为了美观，X轴留几像素,Y轴留几像素
        PixmapPainter.drawPixmap(option.rect.x() + 3,
                                 option.rect.bottom() - SCROLL_ICON_HEIGHT - DRAW_ARROW_DOWN_OFFSET, pixmapDown);
#ifdef IF_ENABLE_TESTABILITY
        m_rcBtnDown.setRect(option.rect.x() + 3,
                            option.rect.bottom() - SCROLL_ICON_HEIGHT - DRAW_ARROW_DOWN_OFFSET, pixmapDown.width(),
                            pixmapDown.height());
#endif
    }

    // 将在绘制好的图片,画在滚动条上
    QStylePainter ScrollBarPainter(this);
    ScrollBarPainter.drawPixmap(0, 0, objPixmap);
}

#ifdef IF_ENABLE_TESTABILITY
QString CMyScrollBar::GetTestInfo()
{
    xml_document doc;
    xml_node nodeRoot = doc.append_child("testinfo");

    TestInfo testInfo;
    QPoint ptAbsolute;

    // info
    ptAbsolute = mapToGlobal(m_rcInfo.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcInfo.width();
    testInfo.height = m_rcInfo.height();
    testInfo.id = LABEL_INFO;
    testInfo.text = fromQString(m_strInfo);
    testInfo.type = VIRTUAL_LABEL;
    AddNode(nodeRoot, testInfo);

    // btn
    ptAbsolute = mapToGlobal(m_rcBtnUp.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcBtnUp.width();
    testInfo.height = m_rcBtnUp.height();
    testInfo.id = BTN_UP;
    testInfo.text = "";
    testInfo.type = VIRTUAL_BTN;
    AddNode(nodeRoot, testInfo);

    // btn
    ptAbsolute = mapToGlobal(m_rcBtnDown.topLeft());
    testInfo.x_absolute = ptAbsolute.x();
    testInfo.y_absolute = ptAbsolute.y();
    testInfo.width = m_rcBtnDown.width();
    testInfo.height = m_rcBtnDown.height();
    testInfo.id = BTN_DOWN;
    testInfo.text = "";
    testInfo.type = VIRTUAL_BTN;
    AddNode(nodeRoot, testInfo);

    QString strTestinfo;
    GetStringFromXml(doc, strTestinfo);

    return strTestinfo;
}
#endif

/************************************************************************/
/*                            about CCmbScrollBar                       */
/************************************************************************/
CCmbScrollBar::CCmbScrollBar(QWidget * parent/* = 0*/)
    : CMyScrollBar(parent)
{
    m_bDisplayPageInfo = false;
}

CCmbScrollBar::~CCmbScrollBar()
{

}

void CCmbScrollBar::paintEvent(QPaintEvent * event)
{
    QStyleOptionSlider option;
    initStyleOption(&option);// 初始化style Option的数据

    QRect rect, rectUp, rectDown;
    // 重设滚动条的显示高度,上、下、右方各减一像素
    rect = option.rect;
    rect.setY(rect.top() + 1);
    rect.setHeight(option.rect.height());

    // 创建一个空的图片，在上面绘制内容
    QPixmap objPixmap(option.rect.size());
    QPainter PixmapPainter(&objPixmap);

    PaintBG(PixmapPainter, option.rect);

//  GetArrowsImageByDisplayType(pixmapUp, pixmapDown);

    // 计算上下翻页2个按钮的区域大小
    rectUp = rect;
    rectUp.setHeight(rect.height() / 2);
    rectDown = rect;
    rectDown.setY(rect.height() / 2 + 1);
    rectDown.setHeight(rect.height() / 2);

    QRect rcUp, rcDown;
    // 根据按下状态设置图片
    if (m_bIsClickUp)
    {
        QPixmap pixmapUpClick = THEME_GET_BMP(PIC_GENERAL_COMBO_SCROLL_UP_CLICK);
        rcUp.setRect(rectUp.x() + 2, rectUp.y() + 1, pixmapUpClick.width(), pixmapUpClick.height());
        if (!pixmapUpClick.isNull())
        {
            PixmapPainter.drawPixmap(rcUp, pixmapUpClick);
        }
    }
    // 根据按下状态设置图片
    if (m_bIsClickDown)
    {
        QPixmap pixmapDownClick = THEME_GET_BMP(PIC_GENERAL_COMBO_SCROLL_DOWN_CLICK);
        rcDown.setRect(rectDown.x() + 2, rectDown.y() + 1, pixmapDownClick.width(),
                       pixmapDownClick.height());
        if (!pixmapDownClick.isNull())
        {
            PixmapPainter.drawPixmap(rcDown, pixmapDownClick);
        }
    }

    // 绘制按钮上、下箭头图标
    // 获取上下箭头的图片
    // 向上箭头图片
    QPixmap pixmapUp = THEME_GET_BMP(PIC_GENERAL_PAGE_CTRL_UP_ARROW);
    // 向下箭头图片
    QPixmap pixmapDown = THEME_GET_BMP(PIC_GENERAL_PAGE_CTRL_DOWN_ARROW);
    rectUp.setRect(rectUp.left() + (rectUp.width() - pixmapUp.width()) / 2,
                   rectUp.top() + (rectUp.height() - pixmapUp.height()) / 2, pixmapUp.width(), pixmapUp.height());
    if (!pixmapUp.isNull())
    {
        PixmapPainter.drawPixmap(rectUp, pixmapUp);
    }
    rectDown.setRect(rectDown.left() + (rectDown.width() - pixmapDown.width()) / 2,
                     rectDown.top() + (rectDown.height() - pixmapDown.height()) / 2, pixmapDown.width(),
                     pixmapDown.height());
    if (!pixmapDown.isNull())
    {
        PixmapPainter.drawPixmap(rectDown, pixmapDown);
    }

    QString strPageInfo;
    // 自动绘制page的信息
    {
        // 计算当前页面的Index
        // 每页只递增3个选项，因为第一个要保持上页最后一个选项
        int pagestep = this->pageStep() - 1;
        int icurIndex = this->value();
        int iCurrentPage = (this->value() + pagestep - 1) / pagestep + 1;

        // 计算总页数
        int iMaxPage = (this->maximum() + pagestep - 1) / pagestep + 1;
        strPageInfo  =  QString::number(iCurrentPage) + "/" + QString::number(iMaxPage);
        PixmapPainter.setPen(Qt::black);// 设置绘制内容的颜色
        PixmapPainter.setFont(GetFontByText(strPageInfo));// 设置绘制内容的字体
        PixmapPainter.drawText(option.rect, Qt::AlignCenter, strPageInfo);
    }

#ifdef IF_ENABLE_TESTABILITY
    m_rcInfo = option.rect;
    m_strInfo = strPageInfo;
#endif
    // 将在绘制好的图片,画在滚动条上
    QStylePainter ScrollBarPainter(this);
    ScrollBarPainter.drawPixmap(0, 0, objPixmap);
}

void CCmbScrollBar::PaintBG(QPainter & painter, QRect rect)
{
    QRect rcCtrl = rect;
    QRect rcTop(rcCtrl.left(), rcCtrl.top(), SCROLL_CTRL_DEFAULT_WIDTH, SCROLL_CTRL_BG_TOP_HEIGHT);
    QRect rcBottom(rcCtrl.left(), rcCtrl.bottom() + 1 - SCROLL_CTRL_BG_BOTTOM_HEIGHT,
                   SCROLL_CTRL_DEFAULT_WIDTH, SCROLL_CTRL_BG_TOP_HEIGHT);
    QRect rcMid(rcCtrl.left(), rcTop.bottom(), SCROLL_CTRL_DEFAULT_WIDTH,
                rcBottom.top() - rcTop.bottom());
    // 顶部
    QPixmap pmTop = THEME_GET_BMP(PIC_GENERAL_PAGE_CTRL_BG_TOP);

    if (!pmTop.isNull())
    {
        painter.drawPixmap(rcTop, pmTop);
    }

    // 中间
    QPixmap pmMid = THEME_GET_BMP(PIC_GENERAL_PAGE_CTRL_BG_MID);

    if (!pmMid.isNull())
    {
        QRect rcSource(0, 0, SCROLL_CTRL_DEFAULT_WIDTH, SCROLL_CTRL_BG_MID_HEIGHT);

        painter.drawPixmap(rcMid, pmMid, rcSource);
    }

    // 底部
    QPixmap pmBottom = THEME_GET_BMP(PIC_GENERAL_PAGE_CTRL_BG_BOTTOM);

    if (!pmBottom.isNull())
    {
        painter.drawPixmap(rcBottom, pmBottom);
    }
#ifdef IF_ENABLE_TESTABILITY
    m_rcBtnDown = rcBottom;
    m_rcBtnUp = rcTop;
#endif
}
