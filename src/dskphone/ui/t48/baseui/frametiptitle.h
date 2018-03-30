
#ifndef __FRAME_TIP_TITLE_H__
#define __FRAME_TIP_TITLE_H__

#include "framelisttitle.h"
#include <ylstring.h>

enum
{
    FRAME_LIST_TITLE_TYPE_TIP_TITLE         = FRAME_LIST_TITLE_TYPE_NONE + 3,
};

class QStylePainter;

class CFrameTipTitle : public CFrameListTitle
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    virtual QString GetTestInfo();

    QRect m_rcTip;
#endif
public:
    explicit CFrameTipTitle(CFrameList * pFrameList = NULL, IFrameTitleAction * pAction = NULL);
    virtual ~CFrameTipTitle(void);

    static bool IsTipTitle(CFrameListTitlePtr pTitle);

public:
    void SetTip(const yl::string & strTip);
    void SetTip(const QString & strTip);

protected:
    virtual void paintEvent(QPaintEvent * pEvent);

    void PaintTip(QStylePainter & stylePainter, int nRight = 0);

private:
    QString m_strTip;
};
typedef CFrameTipTitle * CFrameTipTitlePtr;

#endif
