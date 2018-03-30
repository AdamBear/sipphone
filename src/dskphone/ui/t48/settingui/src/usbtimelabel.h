#ifndef _USB_TIME_LABEL_H
#define _USB_TIME_LABEL_H

#include <QLabel>

class CUSBTimeLabel : public QLabel
{
    Q_OBJECT

public:
    CUSBTimeLabel(QWidget * parent = 0);
    virtual ~CUSBTimeLabel();

    inline void SetIsShowHour(bool bIsShowHour)
    {
        m_bIsShowHour = bIsShowHour;
    }

    void SetTimeText(long nSec);

private:
    QString m_strShowText;
    bool m_bIsShowHour;

};

#endif // _USB_TIME_LABEL_H
