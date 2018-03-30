#ifndef _CUSTOM_TIME_H
#define _CUSTOM_TIME_H


class  QCustomTime
{
public:
    QCustomTime();
    ~QCustomTime();

    QCustomTime(int h, int m, int s = 0);

    void Reset();
    int hour() const;
    int minute() const;
    int second() const;

    // Overwrite operator=.
    QCustomTime & operator= (const QCustomTime & rhs);

    // Overwrite the operator ==.
    bool operator== (const QCustomTime & rhs);

    // Overwrite the operator !=.
    bool operator!= (const QCustomTime & rhs);
private:
    int m_iHour;
    int m_iMinute;
    int m_iSecond;
};

#endif
