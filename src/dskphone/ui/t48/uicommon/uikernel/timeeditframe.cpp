#include "timeeditframe.h"
#include "uikernel/qlineeditutility.h"
#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
namespace
{
#define HourMax 23
#define MinuteMax 59
#define LINEEDIT_HEIGHT 36
}

TimeEditFrame::TimeEditFrame(QWidget * parent, Qt::WindowFlags f)
    : QFrame(parent, f)
{
    //QRegExp regExp("[0-9]{2}");
    m_plineHour = new QLineEdit(this);
    m_plineHour->setGeometry(QRect(0, 0, 55, LINEEDIT_HEIGHT));
    m_plineHour->setAlignment(Qt::AlignCenter);

    //m_plineHour->setMaxLength(2);
    //m_plineHour->setValidator(new QRegExpValidator(regExp, this));

    m_plineMinute = new QLineEdit(this);
    m_plineMinute->setGeometry(QRect(75, 0, 55, LINEEDIT_HEIGHT));
    m_plineMinute->setAlignment(Qt::AlignCenter);

    //m_plineMinute->setMaxLength(2);
    //m_plineMinute->setValidator(new QRegExpValidator(regExp, this));

    m_plineSecond = new QLineEdit(this);
    m_plineSecond->setGeometry(QRect(150, 0, 55, LINEEDIT_HEIGHT));
    m_plineSecond->setAlignment(Qt::AlignCenter);

    //m_plineSecond->setMaxLength(2);
    //m_plineSecond->setValidator(new QRegExpValidator(regExp, this));


    m_plabel = new QLabel(this);
    m_plabel->setGeometry(QRect(55, 0, 20, LINEEDIT_HEIGHT));
    m_plabel->setAlignment(Qt::AlignCenter);
    m_plabel->setText(":");

    m_plabel_2 = new QLabel(this);
    m_plabel_2->setGeometry(QRect(130, 0, 20, LINEEDIT_HEIGHT));
    m_plabel_2->setAlignment(Qt::AlignCenter);
    m_plabel_2->setText(":");

    this->resize(190, LINEEDIT_HEIGHT);
    connect(m_plineHour, SIGNAL(textChanged(QString)), this, SLOT(setHour()));
    connect(m_plineMinute, SIGNAL(textChanged(QString)), this, SLOT(setMinute()));
    connect(m_plineSecond, SIGNAL(textChanged(QString)), this, SLOT(setSecond()));
    connect(m_plineHour, SIGNAL(editingFinished()), this, SLOT(setFocusOutHour()));
    connect(m_plineMinute, SIGNAL(editingFinished()), this, SLOT(setFocusOutMinute()));
    connect(m_plineSecond, SIGNAL(editingFinished()), this, SLOT(setFocusOutSecond()));
}

TimeEditFrame::~TimeEditFrame(void)
{
}
void TimeEditFrame::setHour()
{
    QString strHour = m_plineHour->text();
    if (strHour.length() > 2)
    {
        m_plineHour->setText("00");
    }
    int nHour = strHour.toInt();
    if (nHour > HourMax)
    {
        m_plineHour->setText("00");
    }
//   else if(strHour != "0")
//   {
//    strHour.sprintf("%.2d",nHour);
//    //sprintf(strHour,"%.2d",nHour);
//    m_plineHour->setText(strHour);
//   }
}
void TimeEditFrame::setMinute()
{
    QString strMinute = m_plineMinute->text();
    if (strMinute.length() > 2)
    {
        m_plineMinute->setText("00");
    }
    int nMinute = strMinute.toInt();
    if (nMinute > MinuteMax)
    {
        m_plineMinute->setText("00");
    }
//  else if(strMinute != "0")
//  {
//      strMinute.sprintf("%.2d",nMinute);
//      //sprintf(strHour,"%.2d",nHour);
//      m_plineMinute->setText(strMinute);
//  }
}
void TimeEditFrame::setSecond()
{
    QString strSecond = m_plineSecond->text();
    if (strSecond.length() > 2)
    {
        m_plineSecond->setText("00");
    }
    int nSecond = strSecond.toInt();
    if (nSecond > MinuteMax)
    {
        m_plineSecond->setText("00");
    }
//  else if(strSecond != "0")
//  {
//      strSecond.sprintf("%.2d",nSecond);
//
//      //sprintf(strHour,"%.2d",nHour);
//      m_plineSecond->setText(strSecond);
//  }
}

void TimeEditFrame::checkTime()
{
    setFocusOutHour();
    setFocusOutMinute();
    setFocusOutSecond();
}

void TimeEditFrame::setFocusOutHour()
{
    QString strHour = m_plineHour->text();
    if (strHour.length() > 2  || strHour.isEmpty())
    {
        m_plineHour->setText("00");
        return;
    }
    int nHour = strHour.toInt();
    if (nHour > HourMax)
    {
        m_plineHour->setText("00");
    }
    else
    {
        strHour.sprintf("%.2d", nHour);
        //sprintf(strHour,"%.2d",nHour);
        m_plineHour->setText(strHour);
    }
}
void TimeEditFrame::setFocusOutMinute()
{
    QString strMinute = m_plineMinute->text();
    if (strMinute.length() > 2 || strMinute.isEmpty())
    {
        m_plineMinute->setText("00");
        return;
    }
    int nMinute = strMinute.toInt();
    if (nMinute > MinuteMax)
    {
        m_plineMinute->setText("00");
    }
    else
    {
        strMinute.sprintf("%.2d", nMinute);
        m_plineMinute->setText(strMinute);
    }
}
void TimeEditFrame::setFocusOutSecond()
{
    QString strSecond = m_plineSecond->text();
    if (strSecond.length() > 2  || strSecond.isEmpty())
    {
        m_plineSecond->setText("00");
        return;
    }
    int nSecond = strSecond.toInt();
    if (nSecond > MinuteMax)
    {
        m_plineSecond->setText("00");
    }
    else
    {
        strSecond.sprintf("%.2d", nSecond);
        m_plineSecond->setText(strSecond);
    }
}
TimeEditFrame * qReplaceTimeEditFrame(QWidget * pUIWidget)
{
    QRect rect = pUIWidget->geometry();
    QWidget * pParent = (QWidget *)pUIWidget->parent();
    pUIWidget->setAttribute(Qt::WA_DeleteOnClose, true);
    pUIWidget->close();

    TimeEditFrame * pTimeEditFrame = new TimeEditFrame(pParent);
    pTimeEditFrame->setGeometry(rect);
    return pTimeEditFrame;
}
