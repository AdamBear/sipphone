#ifndef QMYLABLE_H
#define QMYLABLE_H

#include <QLabel>
//#include "PhoneQT.h"
#include <ETLLib/ETLLib.hpp>
#include <QTimer>
#include <QEvent>

class qScrollTextLabel : public QLabel
{
    Q_OBJECT
public:
    explicit qScrollTextLabel(QWidget * parent = 0, Qt::WindowFlags flag = 0);
    ~qScrollTextLabel();
    bool eventFilter(QObject * pObj, QEvent * pEvent);
    void setText(const QString & text); // 设置Text,用于覆盖父类的text函数
    QString text() const
    {
        return m_strRealText;   // 取回Text,用于覆盖父类的text函数
    }
    void setFocus(Qt::FocusReason
                  reason);//解决刚进入界面时，设置了焦点事件，实际上又没获取焦点问题。
    void setScroolTimer(int iTimer);
    void resetScroolTimer();
    int getCountStop();
    void setScroll(BOOL bScroll);
protected:
    void ResetScrollStatus();
    void OnReshowEvent();

private slots:
    void UpdateText();

public:
    void StartScrollTimer();
    //停止计时器，bEllipsis[in]: 是否自动添加省略号
    void StopScrollTimer(bool bEllipsis = false);

private:
    QTimer  m_timerScrollText;
    QString m_strShowText;  // 显示的文字
    QString m_strRealText;  // 真实的文字
    int m_nCount;
    BOOL m_bGoAhead;
    int m_nDiffLength;
    int     m_iScroolTimer;
    Qt::Alignment m_alignLast;
    BOOL m_bScroll;
};

#endif  // QMYLABLE_H

