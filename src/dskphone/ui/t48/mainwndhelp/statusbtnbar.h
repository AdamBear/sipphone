#ifndef __STATUS_BTN_BAR_H__
#define __STATUS_BTN_BAR_H__

#include "uikernel/btnbarwrapper.h"

class CStatusBtnBar : public CBtnBarWrapper
{
public:
    explicit CStatusBtnBar(QWidget * pWidget, bool bAutoDelBtn);
    virtual ~CStatusBtnBar(void);

public:
    void SetAlign(int nAlign);

    void SetRect(const QRect & rect);
    void SetRect(int nX, int nY, int nWidth, int nHeight);

    const QRect & GetRect() const;

    void ReLayoutButton();
    void PaintStatusBtnBar(QStylePainter & stylePainter, const char * pszBtnClick);

    void SetBtnShowCount(int nCount);

    enum
    {
        STATUS_BTN_ALIGN_LEFT           = 0,
        STATUS_BTN_ALIGN_CENTER         = 1,
        STATUS_BTN_ALIGN_RIGHT          = 2,
    };

private:
    int m_nAlgin;
    QRect m_rcBar;
    int     m_iBtnShowCount;
};

#endif
