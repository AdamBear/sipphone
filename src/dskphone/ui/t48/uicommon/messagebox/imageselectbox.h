#ifndef IMAGESELECTBOX_H
#define IMAGESELECTBOX_H

#include "messageboxbase.h"

class CFrameList;
class IFrameListDelegate;

class CImageSelectBox : public CMessageBoxBase
{
    Q_OBJECT

public:
    explicit CImageSelectBox(int nRowNum, IFrameListDelegate * pDelegate = NULL,
                             CMaskFrame * pMaskFrame = 0);
    ~CImageSelectBox();

    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);
    virtual bool event(QEvent * pEvent);

    virtual void PrintMsgBoxInfo(bool bAdd);

    virtual bool IfClickOutExit()
    {
        return true;
    }

private:
    void Init();

protected:
    int m_nRowNum;
    bool m_bRefresh;
    CFrameList * m_pFrameList;

};

#endif // IMAGESELECTBOX_H
