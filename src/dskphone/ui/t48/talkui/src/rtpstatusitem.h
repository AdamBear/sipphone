#ifndef RTPSTATUSUI_H
#define RTPSTATUSUI_H

#include "uikernel/listitem.h"

enum
{
    LIST_ITEM_TYPE_RTP      = 0x09000000,
};

class CRTPStatusItem : public CListItem
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    QString GetTestInfo();
    QRect m_rcTag;
    QRect m_rcValue;
#endif

public:
    explicit CRTPStatusItem(QWidget * parent = NULL);
    ~CRTPStatusItem();

    void SetTag(QString strTag)
    {
        m_strTag = strTag;
    }
    void SetValue(QString strText)
    {
        m_strValue = strText;
    }

    void SetFontSize(int iFontSize);

    void SetFontColor(QColor clr);

    bool IsFontSizeValid(int iFontSize);
protected:
    void paintEvent(QPaintEvent * pEvent);
private:
    QString m_strTag;
    QString m_strValue;
    QColor m_fontColor;
};


#endif
