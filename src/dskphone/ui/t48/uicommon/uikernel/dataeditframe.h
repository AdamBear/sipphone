#ifndef  DataEditFrame_H
#define  DataEditFrame_H

#include <QtGui>
class DataEditFrame : public QFrame
{

    Q_OBJECT
public:
    QLineEdit * m_plineMonth;
    QLineEdit * m_plineDay;
    QLabel * m_plabel_2;
    QLabel * m_plabel;
    QLineEdit * m_plineYear;
public:
    explicit DataEditFrame(QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~DataEditFrame(void);
    void checkDate();

public slots:
    void setDay();
    void setYear();
    void setMounth();
    void FocusOutsetYear();
    void FocusOutsetMounth();
    void FocusOutSetDay();
};
DataEditFrame * qReplaceDataEditFrame(QWidget * pUIWidget);
bool IsLeapyear(int year);
#endif
