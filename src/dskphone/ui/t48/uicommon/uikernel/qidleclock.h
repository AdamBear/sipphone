#ifndef __Q_IDLE_CLOCK_H__
#define  __Q_IDLE_CLOCK_H__

#include <QtGui>
#include "qdatetimelabel.h"

class qIdleClock : public qDateTimeLabel
{
public:
    qIdleClock(QWidget * p);
    virtual ~qIdleClock();

    inline void SetHMFont(const QFont & hmfont)
    {
        m_HMFont = hmfont;
    }
    inline void SetHMColor(const QColor & hmColor)
    {
        m_HMColor = hmColor;
    }

    inline void SetDateFont(const QFont & datefont)
    {
        m_DateFont = datefont;
    }
    inline void SetDateColor(const QColor & dateColor)
    {
        m_DateColor = dateColor;
    }

    inline void SetAmPmFont(const QFont & ampmfont)
    {
        m_AmPmFont = ampmfont;
    }
    inline void SetAmPmColor(const QColor & ampmColor)
    {
        m_AmPmColor = ampmColor;
    }

    inline void SetShadowColor(const QColor & shadowColor)
    {
        m_ShadowColor = shadowColor;
    }
    inline void SetNeedShadow(bool bNeedShadow)
    {
        m_bNeedShadow = bNeedShadow;
    }

    inline void SetIntervalOfHMAndDate(int nInterval)
    {
        m_nIntervalOfHMAndDate = nInterval;
    }

protected:
    void paintEvent(QPaintEvent * pe);

private:
    QFont   m_HMFont;       //显示小时分钟字体
    QColor  m_HMColor;      //显示小时分钟颜色

    QFont   m_DateFont;     //显示日期字体
    QColor  m_DateColor;    //显示日期颜色

    QFont   m_AmPmFont;     //显示上午下午的字体
    QColor  m_AmPmColor;    //显示上午下午的颜色

    QColor  m_ShadowColor;  //阴影颜色
    bool    m_bNeedShadow;

    int     m_nIntervalOfHMAndDate; // 时间和日期间的间隔
    bool    m_bDateShowBefore;
};

#endif // #ifndef __Q_IDLE_CLOCK_H__
