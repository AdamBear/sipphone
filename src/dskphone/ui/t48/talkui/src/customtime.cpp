#include "customtime.h"

QCustomTime::QCustomTime()
    : m_iHour(0)
    , m_iMinute(0)
    , m_iSecond(0)
{
    Reset();
}

QCustomTime::QCustomTime(int h, int m, int s)
    : m_iHour(h)
    , m_iMinute(m)
    , m_iSecond(s)
{

}

QCustomTime::~QCustomTime()
{

}

void QCustomTime::Reset()
{
    m_iHour = 0;
    m_iMinute = 0;
    m_iSecond = 0;
}

int QCustomTime::hour() const
{
    return m_iHour;
}

int QCustomTime::minute() const
{
    return m_iMinute;
}

int QCustomTime::second() const
{
    return m_iSecond;
}

// Overwrite operator=.
QCustomTime & QCustomTime::operator= (const QCustomTime & rhs)
{
    if (this != &rhs)
    {
        m_iHour = rhs.m_iHour;
        m_iMinute = rhs.m_iMinute;
        m_iSecond = rhs.m_iSecond;
    }
    return *this;
}

// Overwrite the operator ==.
bool QCustomTime::operator== (const QCustomTime & rhs)
{
    return (m_iHour == rhs.m_iHour
            && m_iMinute == rhs.m_iMinute
            && m_iSecond == rhs.m_iSecond);
}

// Overwrite the operator !=.
bool QCustomTime::operator!= (const QCustomTime & rhs)
{
    return !(this->operator ==(rhs));
}
