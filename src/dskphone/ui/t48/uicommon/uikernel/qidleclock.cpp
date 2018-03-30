#include "qidleclock.h"
#include "languagehelper.h"
#include <configiddefine.h>
#include "configparser/modconfigparser.h"

namespace
{
static const QColor S_COLOR_SHADOW(0, 0, 0, 100);
}

qIdleClock::qIdleClock(QWidget * p)
    : qDateTimeLabel(p)
    , m_bNeedShadow(false)
    , m_ShadowColor(S_COLOR_SHADOW)
    , m_HMColor(Qt::white)
    , m_DateColor(Qt::white)
    , m_nIntervalOfHMAndDate(0)
{
    setObjectName("qIdleClock");
    m_bDateShowBefore = configParser_GetConfigInt(kszDateShowBefore) == 1;
}

qIdleClock::~qIdleClock()
{
}

void qIdleClock::paintEvent(QPaintEvent * pe)
{
    QString strTime = text();
    QStylePainter pp(this);
    TimeFormatType timeformat = (TimeFormatType)GetTimeFmt();
    int spittimeindex = 0;
    int spitdateindex = 0;
    int sepdistance = 0;
    QString strampm;//PMAM
    QString strHM;
    QString strDate;

    if (TIME_FORMAT_12HOUR == timeformat)
    {
        spittimeindex = 5;
        spitdateindex = strTime.size() - 9;
        if (m_bDateShowBefore)
        {
            strampm = strTime.right(2);
            strHM = strTime.mid(strTime.size() - 8, spittimeindex);
            strDate = strTime.left(spitdateindex);
        }
        else
        {
            strampm = strTime.mid(6, 2);
            strHM = strTime.left(spittimeindex);
            strDate = strTime.right(spitdateindex);
        }
        strampm = LANG_PARSE(strampm);
        sepdistance = 16;
    }
    else if (TIME_FORMAT_24HOUR == timeformat)
    {
        spittimeindex = 5;
        spitdateindex = strTime.size() - 6;
        strampm = ""; //24小时制不需要ampm
        sepdistance = 0;
        if (m_bDateShowBefore)
        {
            strHM = strTime.right(spittimeindex);
            strDate = strTime.left(spitdateindex);
        }
        else
        {
            strHM = strTime.left(spittimeindex);
            strDate = strTime.right(spitdateindex);
        }
    }

    strDate = LANG_PARSE(strDate);
    QFontMetrics HMmetrics(m_HMFont);
    QString strHour = strHM.left(2);//小时
    QString strMinute = strHM.right(strHM.size() - 3); //分钟
    QChar spaceorsym = strHM[2];//为冒号或者空格

    int HWidth = HMmetrics.width(strHour);//小时宽度
    int MWidth = HMmetrics.width(strMinute);//分钟宽度
    int ColonWidth =  HMmetrics.width(':');//冒号宽度

    QFontMetrics AmPmMmetrics(m_AmPmFont);
    int AmPmWidth = AmPmMmetrics.width(strampm) + sepdistance; //ampm和时间间隔16个像素。
    int AmPmHeight = AmPmMmetrics.height();
    int HMWidth = HWidth + MWidth + ColonWidth + AmPmWidth; //总宽度
    int HMHeight = HMmetrics.height();//高度

    //绘制时间
    ////---- 优化使用图片
    QPixmap HMpixmap(HMWidth, HMHeight + 8);
    HMpixmap.fill(QColor(0, 0, 0, 0));
    QPainter pHM(&HMpixmap);
    int textwidth = width();

    //先绘制时间阴影效果
    if (m_bNeedShadow)
    {
        pHM.setFont(m_HMFont);
        pHM.setPen(m_ShadowColor);
        pHM.drawText(0 + 1, HMHeight + 1, strHour);
        pHM.drawText(HWidth + 1, HMHeight + 1, spaceorsym);
        pHM.drawText(HWidth + ColonWidth + 1, HMHeight + 1, strMinute);
        //绘制ampm换字体
        pHM.setFont(m_AmPmFont);
        pHM.drawText(HMWidth - AmPmWidth + sepdistance + 1, HMHeight + 1, strampm);
        pp.drawItemPixmap(QRect(0 + 1, 0 + 1, textwidth, HMHeight), Qt::AlignCenter, HMpixmap);
    }

    pHM.setFont(m_HMFont);
    pHM.setPen(m_HMColor);
    pHM.drawText(0, HMHeight, strHour);
    pHM.drawText(HWidth, HMHeight, spaceorsym);
    pHM.drawText(HWidth + ColonWidth, HMHeight, strMinute);

    //绘制ampm换字体
    pHM.setFont(m_AmPmFont);
    pHM.drawText(HMWidth - AmPmWidth + sepdistance, HMHeight, strampm);
    pp.drawItemPixmap(QRect(0, 0, textwidth, HMHeight), Qt::AlignCenter, HMpixmap);

    //绘制日期阴影
    if (m_bNeedShadow)
    {
        pp.setFont(m_DateFont);
        pp.setPen(m_ShadowColor);
        pp.drawText(QRect(0 + 1, HMHeight + m_nIntervalOfHMAndDate + 1, textwidth, height() - HMHeight),
                    Qt::AlignHCenter | Qt::AlignTop, strDate);
    }

    //绘制日期
    pp.setFont(m_DateFont);
    pp.setPen(m_DateColor);
    pp.drawText(QRect(0, HMHeight + m_nIntervalOfHMAndDate, textwidth, height() - HMHeight),
                Qt::AlignHCenter | Qt::AlignTop, strDate);
}
