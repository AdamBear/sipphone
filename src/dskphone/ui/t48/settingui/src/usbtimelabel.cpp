#include "usbtimelabel.h"

#include "usbuicommon.h"

CUSBTimeLabel::CUSBTimeLabel(QWidget * parent)
    : QLabel(parent)
    , m_bIsShowHour(true)
{

}

CUSBTimeLabel::~CUSBTimeLabel()
{

}

void CUSBTimeLabel::SetTimeText(long nSec)
{
    long nHours = 0;
    long nMinutes = 0;
    long nSeconds = nSec;

    m_strShowText = "";

    if (m_bIsShowHour)
    {
        //计算总小时数
        nHours = nSeconds / 3600;
        nSeconds = nSeconds % 3600;
    }

    //计算总分钟数，并且剩下的就是秒数
    nMinutes = nSeconds / 60;
    nSeconds = nSeconds % 60;

    if (m_bIsShowHour)
    {
        //小于10则补一个0
        if (nHours < 10)
        {
            m_strShowText += QString::number(0);
        }
        m_strShowText += QString::number(nHours);

        m_strShowText += ":";
    }

    //小于10则补一个0
    if (nMinutes < 10)
    {
        m_strShowText += QString::number(0);
    }
    m_strShowText += QString::number(nMinutes);

    m_strShowText += ":";

    //小于10则补一个0
    if (nSeconds < 10)
    {
        m_strShowText += QString::number(0);
    }
    m_strShowText += QString::number(nSeconds);

    setText(m_strShowText);
}
