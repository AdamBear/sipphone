#ifndef TITLEMENUBOX_H
#define TITLEMENUBOX_H

#include "messageboxbase.h"

class CFrameList;
class IPopupMenuDelegate;

class CTitleMenuBox : public CMessageBoxBase
{
public:
    CTitleMenuBox(CMaskFrame * pMaskFrame = NULL, IPopupMenuDelegate * pDelegate = NULL,
                  const QPoint & ptMenu = QPoint());
    ~CTitleMenuBox();

public:
    void Init(IPopupMenuDelegate * pDelegate, const QPoint & ptMenu);

    virtual void PrintMsgBoxInfo(bool bAdd);

    virtual bool IfClickOutExit()
    {
        return true;
    }

protected:
    virtual void paintEvent(QPaintEvent * pEvent);

private:
    CFrameList * m_pFrameList;

};

#endif // TITLEMENUBOX_H
