
#ifndef __STATUS_BUTTON_H__
#define __STATUS_BUTTON_H__

#include "uikernel/buttonwrapper.h"
#include <ylstring.h>
#include <ylvector.h>

#include <QObject>
#include <QTimer>

class QString;
typedef YLVector<yl::string> vecIconString;

class CStatusButton : public QObject, public CButtonWrapper
{
    Q_OBJECT

public:
    explicit CStatusButton(QWidget * pWidget);
    virtual ~CStatusButton(void);

    static int GetStatusBtnWidth();

    void SetStatus(int nId, const yl::string & strIcon, const yl::string & strHint);
    void SetStatus(int nId, vecIconString & vecstrIcon, const yl::string & strHint);
    int GetId() const;

    void PaintStatusButton(QStylePainter & stylePainter, const char * pszClick);

public:
    void StartFlash();
    void StopFlash();

    bool ProcessMousePressPoint(QPoint & PressPoint);
    bool ProcessMouseReleasePoint(QPoint & PressPoint);

private Q_SLOTS:
    void OnFlashTimeOut();

signals:
    void NeedRepaint();

private:
    int             m_nId;
    vecIconString   m_vecstrIcon;
    QString         m_strHint;
    QTimer          m_FlashTimer;
    bool            m_bIsFlashUp;
    int             m_nFlashIndex;
    bool            m_bPointDonw;
};

#endif
