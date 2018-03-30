#include "qtimepiece.h"
#include "qtcommon/qmisc.h"
#include "commonapi/stringhelperapi.h"
#include <QtGui>

QTimePiece::QTimePiece(QWidget * pParent, Qt::WindowFlags flag)
    : QLabel(pParent, flag)
{
    m_uStartTime = 0;
    m_uTickTime = 0;
    m_uPauseTime = 0;

    m_timerFlash = new QTimer(this);
    if (m_timerFlash != NULL)
    {
        connect(m_timerFlash, SIGNAL(timeout()), this, SLOT(OnFlashTimer()));
    }

    setText("00 : 00");
    setAlignment(Qt::AlignCenter);
}

QTimePiece::~QTimePiece()
{
    StopTimer();
}

void QTimePiece::StartTimer()
{
    m_timerFlash->start(1000);
    if (m_uPauseTime == 0)
    {
        m_uStartTime = etlGetTickCount();
        m_uTickTime = 0;
    }
    else
    {
        UINT64 uNowTime = etlGetTickCount();
        if (uNowTime < m_uStartTime || uNowTime < m_uPauseTime)
        {
            chWARNING2(FALSE, commonAPI_FormatString("Start Time[%d], Now Time[%d], Pause Time[%d]",
                       m_uStartTime, uNowTime, m_uPauseTime).c_str());
        }
        chASSERT(uNowTime >= m_uStartTime && uNowTime >= m_uPauseTime);

        //adjust start time
        m_uStartTime += (uNowTime - m_uPauseTime);
        m_uPauseTime = 0;
    }
}

void QTimePiece::StopTimer()
{
    if (m_timerFlash->isActive())
    {
        m_timerFlash->stop();
    }
    m_uStartTime = 0;
    m_uPauseTime = 0;
    setText("00 : 00");
}

void QTimePiece::PauseTimer()
{
    if (m_uPauseTime == 0)
    {
        m_uPauseTime = etlGetTickCount();
        if (m_timerFlash->isActive())
        {
            m_timerFlash->stop();
        }
    }
}

BOOL QTimePiece::IsTimerStart()
{
    return (BOOL)(m_timerFlash->isActive());
}

BOOL QTimePiece::IsTimerPause()
{
    return (BOOL)(m_uPauseTime != 0);
}

void QTimePiece::GetTime(UINT & uHour, UINT & uMinute, UINT & uSecond)
{
    uHour = m_uTickTime / 3600;
    uMinute = (m_uTickTime / 60) % 60;
    uSecond = m_uTickTime % 60;
}

void QTimePiece::SetTime(UINT uHour, UINT uMinute, UINT uSecond)
{
    if (uMinute > 59)
    {
        uMinute = 59;
    }

    if (uSecond > 59)
    {
        uSecond = 59;
    }

    yl::string strTime;
    if (uHour > 0)
    {
        strTime = commonAPI_FormatString("%.2d : %.2d : %.2d", uHour, uMinute, uSecond);
    }
    else
    {
        strTime = commonAPI_FormatString("%.2d : %.2d", uMinute, uSecond);
    }
    setText(toQString(strTime.c_str()));

    UINT64 uTempTime = (uHour * 3600 + uMinute * 60 + uSecond) * 1000;
    m_uStartTime = etlGetTickCount() - uTempTime;
}

void QTimePiece::OnFlashTimer()
{
    UINT64 uNowTime = etlGetTickCount();
    if (uNowTime < m_uStartTime)
    {
        chWARNING2(FALSE, commonAPI_FormatString("Start Time[%d], Now Time[%d]", m_uStartTime,
                   uNowTime).c_str());
        return;
    }

    UINT64 uNewTime = (uNowTime - m_uStartTime) / 1000;
    if (m_uTickTime != uNewTime)
    {
        m_uTickTime = uNewTime;
        UINT uHour = m_uTickTime / 3600;
        UINT uMinute = (m_uTickTime / 60) % 60;
        UINT uSecond = m_uTickTime % 60;

        yl::string strTime;
        if (uHour > 0)
        {
            strTime = commonAPI_FormatString("%.2d : %.2d : %.2d", uHour, uMinute, uSecond);
        }
        else
        {
            strTime = commonAPI_FormatString("%.2d : %.2d", uMinute, uSecond);
        }

        setText(toQString(strTime.c_str()));
    }
}

bool QTimePiece::event(QEvent * pEvent)
{
    switch (pEvent->type())
    {
    case QEvent::WindowActivate:
        {
            //StartTimer();
        }
        break;
    case QEvent::WindowDeactivate:
        {
            //为防止通话时通过Linekey进入xml browser导致的暂停，移到talking的uninit中
            //PauseTimer();
        }
        break;
    default:
        break;
    }
    return QLabel::event(pEvent);
}
