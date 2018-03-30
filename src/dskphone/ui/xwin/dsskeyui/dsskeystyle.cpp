#include "xwindow/xWindow.h"
#include "xwindow/xWindowManager.h"
#include "dsskeystyle.h"
////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
#define BTN_HALF_LINE_T54 4
#define BTN_HALF_LINE_T52 3
#define DSSKEY_CUTLINE_HORIZONTAL       6
}

#if IS_COLOR
EXPORT_STYLE_CLASS(CDsskeyFrameBGStyle);
CDsskeyFrameBGStyle::CDsskeyFrameBGStyle()
{
    m_ePhoneType = devicelib_GetPhoneType();
}

CDsskeyFrameBGStyle::~CDsskeyFrameBGStyle()
{
}

EXPORT_STYLE_CLASS(CDsskeyFrameLeftBGStyle);
CDsskeyFrameLeftBGStyle::CDsskeyFrameLeftBGStyle()
{
}

CDsskeyFrameLeftBGStyle::~CDsskeyFrameLeftBGStyle()
{
}

bool CDsskeyFrameLeftBGStyle::onViewPaintBackground(xView * pView, xPainter & p)
{
    if (NULL == pView)
    {
        return false;
    }

    int nBtnCount = m_ePhoneType == PT_T54S ? BTN_HALF_LINE_T54 : BTN_HALF_LINE_T52;
    chRect rcClient = pView->clientRect();
    p.blendRect(rcClient, pView->background);

    if (m_ePhoneType == PT_T52)
    {
        int iHeight = rcClient.Height() / (nBtnCount + 1);
        for (int i = 0; i != nBtnCount; ++i)
        {
            p.drawLineX((i + 1) * iHeight, 0, rcClient.Width() - DSSKEY_CUTLINE_HORIZONTAL, xColor(52, 255, 255, 255));
        }
    }
    else if (m_ePhoneType == PT_T54S)
    {
        int iHeight = rcClient.Height() / (nBtnCount + 1);
        for (int i = 0; i != nBtnCount; ++i)
        {
            p.drawLineX((i + 1) * iHeight, 0, rcClient.Width() - DSSKEY_CUTLINE_HORIZONTAL, xColor(52, 255, 255, 255));
        }
    }



    return true;
}

//
EXPORT_STYLE_CLASS(CDsskeyFrameRightBGStyle);
CDsskeyFrameRightBGStyle::CDsskeyFrameRightBGStyle()
    : m_bHavePageBtn(false)
{
}

CDsskeyFrameRightBGStyle::~CDsskeyFrameRightBGStyle()
{
}

bool CDsskeyFrameRightBGStyle::onViewPaintBackground(xView * pView, xPainter & p)
{
    if (NULL == pView)
    {
        return false;
    }

    int nBtnCount = m_ePhoneType == PT_T54S ? BTN_HALF_LINE_T54 : BTN_HALF_LINE_T52;
    chRect rcClient = pView->clientRect();
    if (m_bHavePageBtn)
    {
        rcClient = rcClient.ResizeHeight(rcClient.Height() - rcClient.Height() / (nBtnCount + 1));
        nBtnCount--;
    }
    p.blendRect(rcClient, pView->background);

    if (m_ePhoneType == PT_T52)
    {
        int iHeight = rcClient.Height() / (nBtnCount + 1);
        for (int i = 0; i != nBtnCount; ++i)
        {
            p.drawLineX((i + 1) * iHeight, DSSKEY_CUTLINE_HORIZONTAL, rcClient.Width(), xColor(52, 255, 255, 255));
        }
    }
    else if (m_ePhoneType == PT_T54S)
    {
        int iHeight = rcClient.Height() / (nBtnCount + 1);
        for (int i = 0; i != nBtnCount; ++i)
        {
            p.drawLineX((i + 1) * iHeight, DSSKEY_CUTLINE_HORIZONTAL, rcClient.Width(), xColor(52, 255, 255, 255));
        }
    }

    return true;
}

void CDsskeyFrameRightBGStyle::SetPageBtn(bool bHavePageBtn)
{
    m_bHavePageBtn = bHavePageBtn;
}

#endif
