#ifndef SCROLL_PAGE_H
#define SCROLL_PAGE_H

#include <QScrollArea>
#include <QScrollBar>
#include "baseui/pagectrl.h"

// 设置ScrollPage子页面时需要预留的高度
// 即ScrollPage的高度 = 子页面显示高度 + ScrollPageFrameHeight
const int ScrollPageFrameHeight = 4;

//T3X风格所用的ScrollPage
class CT3XScrollPage : public QScrollArea
{
    Q_OBJECT

public:
    explicit CT3XScrollPage(QWidget * parent = 0);
    ~CT3XScrollPage();

    // 设置切页控件的子页面
    // nHeight[in] 重设控件子页面的高度(即控件的高度) 当nHeight大于0的时候设置生效
    // bAutoResetGeometry[in] 是否根据子控件的位置自动设置控件的位置和宽度
    // 注意高度不能用bAutoResetGeometry设置必须要参数nHeight设置
    bool SetChildPage(QWidget * pChildPage, int nHeight = 0, bool bAutoResetGeometry = false);

    // 按键处理
    bool ProcessPhonekey(int iKey);

    // 设置滚动条的滚动步长
    // 即按上下键滚动的距离
    void SetSingleStep(int iSingleStep);

    // 设置滚动条的切页步长
    void SetPageStep(int iPageStep);

    // 根据最大高度和每行的高度自动调整自身的高度
    // 使自身的高度 = n * iRowHeight(n 为正整数)
    void AutoResizeByHeight(int iMaxHeight, int iRowHeight);

private:
    int     m_iSingleStep;
    int     m_iPageStep;
};


// 切页控件
#ifdef _T48
class CScrollPage : public QWidget, public IScrollNotify
#else
class CScrollPage : public QScrollArea
#endif
{
    Q_OBJECT
public:
    explicit CScrollPage(QWidget * parent = 0);
    ~CScrollPage();

    // 设置切页控件的子页面
    // nHeight[in] 重设控件子页面的高度(即控件的高度) 当nHeight大于0的时候设置生效
    // bAutoResetGeometry[in] 是否根据子控件的位置自动设置控件的位置和宽度
    // 注意高度不能用bAutoResetGeometry设置必须要参数nHeight设置
    bool SetChildPage(QWidget * pChildPage, int nHeight = 0, bool bAutoResetGeometry = false);

    // 移除之前的子页面
    // 并返回移除子页面的指针
    QWidget * TakeChildPage();


public slots:
    void OnChildPageFocusWgtChange(QRect wgtRect);
#ifndef _T48
    void OnScrollBarClickUp();
    void OnScrollBarClickDown();

#else

    void OnScrollRangeChanged(int min, int max);
public:
    void SetSingleStep(int iStep);
    void SetPageStep(int iStep);
    bool eventFilter(QObject * pObj, QEvent * pEvent);
    // 上下翻页
    void OnNextPage();
    void OnPrePage();

    void RefreshCurPage();

protected:
    // 通知向前滚动
    virtual void OnNotifyScrollPrevious();
    // 通知向后滚动
    virtual void OnNotifyScrollNext();
    //刷新切页控件
    void RefreshPageCtrlInfo();
#endif

protected:
    virtual void resizeEvent(QResizeEvent * event);// 重写的resizeEvent

signals:
    void PageChange(int iPageIndex);

private:
    void SetPage(int iIndex);
    int GetMinPage();
    int GetMaxPage();
    int GetCurrentPage();
#ifdef _T48
    CPageCtrl * m_pScrollBar;
    QScrollArea * m_pScrollArea;
    int m_iSingleStep;
    int m_iPageStep;
#endif
};

// 滚动条的事件过滤器
// 作用：屏蔽滚动条的滑块和滚动区域操作，只剩按钮的操作
class ScrollBarFilter : public QObject
{
    Q_OBJECT
public:
    explicit ScrollBarFilter(QScrollBar * pWidget, QObject * parent = 0);
    ~ScrollBarFilter();

    virtual bool eventFilter(QObject * target, QEvent * event);

    // 设置滚动条图片的高度
    void SetScrollIconHeight(int nIconHeight);

signals:
    //对点击和松开操作进行判断并发出信号
    void ClickUp();
    void ClickDown();
    void ReleaseClick();

private:
    QScrollBar * m_pScrollBar;// 保存父控件的指针
    int          m_nScrollIconHeight;// 图片的高度
};

// ComboBox滚动条的事件过滤器
// 作用：屏蔽滚动条的滑块和滚动区域操作，只剩按钮的操作
class ViewScrollBarFilter : public QObject
{
    Q_OBJECT
public:
    explicit ViewScrollBarFilter(QScrollBar * pScrollBar, QObject * parent = 0);
    ~ViewScrollBarFilter();
    virtual bool eventFilter(QObject * target, QEvent * event);
    void SetSingleStep(int iStep);// 设置最小的步长

signals:
    //对点击和松开操作进行判断并发出信号
    void ClickUp();
    void ClickDown();
    void ReleaseClick();

private:
    QScrollBar * m_pScrollBar;// 保存父控件的指针
    int          m_iSingStep; // 最小步长
};


// 自己的滚动条
class CMyScrollBar : public QScrollBar
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    QString GetTestInfo();

    QString m_strInfo;
    QRect m_rcInfo;
    QRect m_rcBtnUp;
    QRect m_rcBtnDown;
#endif
public:
    enum ARROWS_DISPLAR_TYPE
    {
        ARROWS_ALWAYS_ON,
        ARROWS_ALWAYS_OFF,
        ARROWS_ALWAYS_AS_NEEDED,
    };

public:
    explicit CMyScrollBar(QWidget * parent = 0);
    ~CMyScrollBar();

    // 设置背景的画刷
    void SetBackgroundBrush(const QBrush & brushBG);

    // 显示页面的信息
    void DisplayPageInfo(bool bOpen);

    // 设置页面背景的高度偏移值
    // 为了解决Scrollpage滚动条显示过长而设置
    void SetBackgroundHeightOffSet(int iOffset);

    // 设置滚动条中间显示的文字
    // pageInfo Open的时候优先显示pageInfo
    void SetScrollText(const QString & strText);

    // 设置文字的颜色
    void SetTextColor(const QColor & objColor);

    // 设置上下箭头的显示模式
    void SetArrowsDisplayType(ARROWS_DISPLAR_TYPE eArrowsDisplayType);

    void EmitSignal(const QString & type);

signals:
    ////过滤器中对点击和松开操作进行判断并发出的信号引发相应的信号
    void ClickUp();
    void ClickDown();
    void ReleaseClick();

protected slots:
    //收到点击和松开操作的信号后，进行相应处理，实现图标的变化
    void OnClickUp();
    void OnClickDown();
    void OnReleaseClick();
protected:
    virtual void paintEvent(QPaintEvent * event); // 重画函数

protected:
    // 根据显示的模式获取上下箭头的图标
    void GetArrowsImageByDisplayType(QPixmap & pixmapArrowUp, QPixmap & pixmapArrowsDown);

    QFont GetFontByText(QString & strText);

protected:
    QString     m_strText;
    QBrush      m_brushBG;
    QColor      m_TextColor;
    int         m_BackgroundHeightOffSet;
    ARROWS_DISPLAR_TYPE m_eArrowsDisplayType;
    bool        m_bDisplayPageInfo;

    bool m_bIsClickUp;      //Up是否处于被按下状态，默认为false
    bool m_bIsClickDown;    //Down是否处理被按下状态，默认为false
};

class CCmbScrollBar : public CMyScrollBar
{
    Q_OBJECT

public:
    explicit CCmbScrollBar(QWidget * parent = 0);
    ~CCmbScrollBar();

protected:
    virtual void paintEvent(QPaintEvent * event); // 重画函数
    // 绘制滚动条背景
    void PaintBG(QPainter & painter, QRect rect);
};

#endif //SCROLL_PAGE_H
