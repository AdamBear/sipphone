#ifndef _EXP_PAINTER_BLACK_H
#define _EXP_PAINTER_BLACK_H

#include "../expbase/expdefine.h"

class EXPPainterBlack
{
public:
    EXPPainterBlack();
    virtual ~EXPPainterBlack();

public:
    virtual void Init();
    virtual void Uninit();

    void SetMmapInfo(int nHandle);

public:
    virtual void PaintEXPLcdItem(int hop, int screen, int nCurrentPage, int row, int col,
                                 int icon_number, int flags, int str_len, char * str_buff);
    virtual void PaintLcdIcon(int expIndex, int iPage, int iRow, int iCol, int iIcon);
    virtual void PaintLedStatus(int hop, int row, int col, int led_status);

    virtual void SwitchPage(int expIndex, int nPage);

    virtual void SetExpBackLight(int expIndex, int iBackLight);
    virtual void SetExpLCDContrast(int expIndex, int iContrastLevel);

private:

    int m_nHandle;

};

#endif //_EXP_PAINTER_H
