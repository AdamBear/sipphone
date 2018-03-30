#include "exppainterblack.h"
#include "exp/src/expdrive/expdriveheader/exp_drv.h"
#include "exp_struct.h"
#include "devicelib/phonedevice.h"

namespace
{

}


EXPPainterBlack::EXPPainterBlack()
    : m_nHandle(-1)
{

}

EXPPainterBlack::~EXPPainterBlack()
{

}

void EXPPainterBlack::Init()
{

}

void EXPPainterBlack::Uninit()
{

}

void EXPPainterBlack::SetMmapInfo(int nHandle)
{
    m_nHandle = nHandle;
}

void EXPPainterBlack::PaintEXPLcdItem(int hop, int screen, int nCurrentPage, int row, int col,
                                      int icon_number, int flags, int str_len, char * str_buff)
{
    yl::wstring strDisplayW = chUTF82W(str_buff);
    unsigned short * pString = (unsigned short *)malloc((strDisplayW.length() + 1) * 2);
    if (pString == NULL)
    {
        return ;
    }
    int i = 0;
    while (i < strDisplayW.length())
    {
        // 将4字节的WCHAR转换为2字节的unsigned short
        pString[i] = (unsigned short)strDisplayW[i];
        ++i;
    }
    char tempBuf[255] = "\0";
    memcpy(tempBuf, pString, strDisplayW.length() * 2);
    //最后一行需要设置页数图标
    int nType;
    exp_helper_get_hoptype(m_nHandle, hop, &nType);
    if (EXP_TYPE_38 != nType)
    {
        exp_helper_set_lcd_item(m_nHandle, hop, screen,
                                row, col, icon_number, flags, str_len, tempBuf);
    }

    if (EXP_TYPE_40 == nType)
    {
        if (row == 9)
        {
            exp_helper_set_lcd_icon(m_nHandle, hop, nCurrentPage, 9, 1, nCurrentPage == 0 ? 15 : 16);
        }
    }

    if (NULL != pString)
    {
        free(pString);
        pString = NULL;
    }
}

void EXPPainterBlack::PaintLcdIcon(int expIndex, int iPage, int iRow, int iCol, int iIcon)
{

    exp_helper_set_lcd_icon(m_nHandle, expIndex, iPage, iRow, iCol, iIcon);
}

void EXPPainterBlack::PaintLedStatus(int hop, int row, int col, int led_status)
{

    exp_helper_set_led_status(m_nHandle, hop, row, col, led_status);
}

void EXPPainterBlack::SwitchPage(int expIndex, int nPage)
{
    int nType;
    exp_helper_get_hoptype(m_nHandle, expIndex, &nType);
    EXP_INFO("RjExp switch page:expIndex[%d] pageIndex[%d] expType[%d]", expIndex, nPage, nType);
    exp_helper_set_lcd_screen(m_nHandle, expIndex, nPage);

    if (nType == EXP_TYPE_40)
    {
        // EXP40翻页，需要绘制图标
        if (nPage == 0)
        {
            exp_helper_set_lcd_icon(m_nHandle, expIndex, nPage, 9, 1, EIT_PAGE_1);
        }
        else if (nPage == 1)
        {
            exp_helper_set_lcd_icon(m_nHandle, expIndex, nPage, 9, 1, EIT_PAGE_2);
        }
    }
}

void EXPPainterBlack::SetExpBackLight(int expIndex, int iBackLight)
{
    exp_helper_set_lcd_bklight(m_nHandle, expIndex, iBackLight);
}

void EXPPainterBlack::SetExpLCDContrast(int expIndex, int iContrastLevel)
{
    exp_helper_set_lcd_contrast(m_nHandle, expIndex, iContrastLevel);
}

