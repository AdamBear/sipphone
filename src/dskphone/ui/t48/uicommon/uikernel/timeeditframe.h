#ifndef  TimeEditFrame_H
#define  TimeEditFrame_H

#include <QtGui>

class TimeEditFrame : public QFrame
{
    Q_OBJECT
public:
    QLineEdit * m_plineHour;
    QLineEdit * m_plineMinute;
    QLabel * m_plabel_2;
    QLabel * m_plabel;
    QLineEdit * m_plineSecond;
public:
    explicit TimeEditFrame(QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~TimeEditFrame(void);
    void checkTime();

public slots:
    void setHour();
    void setMinute();
    void setSecond();
    void setFocusOutHour();
    void setFocusOutMinute();
    void setFocusOutSecond();
//  void FocusOutsetHour();
//  void FocusOutsetMinute();
};
TimeEditFrame * qReplaceTimeEditFrame(QWidget * pUIWidget);
//BOOL IsLeapyear(int year);
#endif
