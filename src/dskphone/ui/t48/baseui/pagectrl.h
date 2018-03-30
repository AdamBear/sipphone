#ifndef __PAGE_CTRL_H__
#define __PAGE_CTRL_H__

#include <QWidget>
#include "iscrollnotify.h"
#include "uikernel/buttonwrapper.h"
#include "uikernel/btnbarwrapper.h"

class QMouseEvent;
class QKeyEvent;
class QPaintEvent;
class QStylePainter;

class CPageCtrl : public QWidget
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    QString GetTestInfo();
#endif

public:
    explicit CPageCtrl(QWidget * parent, IScrollNotify * pScrollNotify);
    ~CPageCtrl();

    // 设置翻页通知
    void SetScrollCtrlNotify(IScrollNotify * pScrollNotify);

    // 设置控件位置
    void SetRect(int nX, int nY, int nWidth, int nHeight);
    void SetRect(const QRect & rect);

    // 设置字体大小
    void SetFontSize(int nSize);
    // 设置字体颜色
    void SetFontType(int nFontType);

    // 设置页数信息
    void SetScrollInfo(int nRange, int nPos);

    // 设置页码
    void SetScrollPos(int nPos);

    // 获取页码
    int GetPageNo();

    // 获取总页数
    int GetPageCount();

    // 获取默认宽度
    static int GetDefaultWidth();

    // 获取右边距
    static int GetRightMargin();

    // 获取上边距
    int GetTopMargin() const;

    // 获取下边距
    int GetBottomMargin() const;

    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);
    virtual void keyPressEvent(QKeyEvent * pEvent);
    virtual void keyReleaseEvent(QKeyEvent * pEvent);

    virtual void paintEvent(QPaintEvent * pEvent);

    // 画背景
    void PaintBG(QStylePainter & stylePainter);
    // 重排窗口
    void ReLayout();

private:
    int m_nPageNo;
    int m_nPageCount;

    QRect m_rcText;

    IScrollNotify * m_pScrollNotify;
    CButtonWrapper m_btnUpArrow;
    CButtonWrapper m_btnDownArrow;
    CBtnBarWrapper m_btnBar;
    QString m_strPageInfo;
};

#endif // __PAGE_CTRL_H__
