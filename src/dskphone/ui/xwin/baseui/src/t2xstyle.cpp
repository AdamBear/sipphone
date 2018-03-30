#include "t2xstyle.h"
#include "xwindow/xWindow.h"
#include "xwindow/xcombobox.h"
#include "xwindow/xWindowManager.h"
#include "t2xpicpath.h"
#include "baseuicommon.h"
#include "xwindow/xListView.h"

void InitCommonStyle()
{
    xComboBoxStyle::Init();
    xComboBox::SetGlobalStyle("ComboBoxStyle");
}

void UnInitCommonStyle()
{
    xComboBoxStyle::UnInit();
}

////////////////////////////////////////////////////////////////////////////////////////////
EXPORT_STYLE_CLASS(xComboBoxStyle);
xComboBoxStyle::xComboBoxStyle()
{
}

xComboBoxStyle::~xComboBoxStyle()
{
}

void xComboBoxStyle::Init()
{
    xStyle * pStyle = xStyle::getObjectByName("ComboBoxStyle");
    xComboBoxStyle * pComboBoxStyle = static_cast<xComboBoxStyle *>(pStyle);

    if (NULL != pComboBoxStyle)
    {
        g_WorkStation.GetImageByPath(PIC_LEFT_RIGHT_ARROR, pComboBoxStyle->m_pmArrow);
    }
}

void xComboBoxStyle::UnInit()
{
    xStyle * pStyle = xStyle::getObjectByName("ComboBoxStyle");
    xComboBoxStyle * pComboBoxStyle = static_cast<xComboBoxStyle *>(pStyle);

    if (NULL != pComboBoxStyle)
    {
        pComboBoxStyle->m_pmArrow.setNull();
    }
}

bool xComboBoxStyle::onViewPaintContent(xView * pView, xPainter & p)
{
    if (NULL == pView)
    {
        return false;
    }

    xComboBox * pComboBox = static_cast<xComboBox *>(pView);
    chRect rcContent = pView->contentRect();

    if (!m_pmArrow.isNull() && pComboBox->GetItemCount() > 1 && pComboBox->IsEnable())
    {
        chRect rcArrow(0, 0, m_pmArrow.width(), m_pmArrow.height());
        adjustRectByGravity(rcContent, (gravity_right | gravity_y_center), rcArrow);

        p.drawItemPixmap(rcArrow, m_pmArrow);
    }

    pComboBox->DrawText(p, pComboBox->GetTextColor());

    return true;
}

bool xComboBoxStyle::onViewRectChanged(xView * pView, const chRect & rectOld)
{
    xComboBox * pComboBox = static_cast<xComboBox *>(pView);

    if (NULL == pComboBox)
    {
        return false;
    }

    chRect rcText = pComboBox->contentRect();

    if (pComboBox->GetItemCount() > 1 && pComboBox->IsEnable())
    {
        rcText.right = rcText.right - m_pmArrow.width() - 1;
    }

    pComboBox->SetTextRect(rcText);

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
EXPORT_STYLE_CLASS(CRectFrameBGStyle);
CRectFrameBGStyle::CRectFrameBGStyle()
{
}

CRectFrameBGStyle::~CRectFrameBGStyle()
{
}

bool CRectFrameBGStyle::onViewPaintBackground(xView * pView, xPainter & p)
{
    if (NULL == pView)
    {
        return false;
    }

    chRect rcClient = pView->clientRect();

    p.fillRect(rcClient, g_WorkStation.profile().m_clrViewDefaultBG);
    p.drawRect(rcClient, T2X_SYSTEM_COLOR_BLACK);

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
#if !IS_COLOR
namespace
{
#define ROUND_BTN_BOUND_PIXEL 3
const char g_cTilde = '~';
}

void InitT2xStyle()
{
    CMsgBoxCommonTitleStyle::Init();
}

void UnInitT2xStyle()
{

}

////////////////////////////////////////////////////////////////////////////////////////////
EXPORT_STYLE_CLASS(xRoundBtnStyle);
xRoundBtnStyle::xRoundBtnStyle()
{

}

xRoundBtnStyle::~xRoundBtnStyle()
{

}

bool xRoundBtnStyle::onViewPaintBackground(xView * pView, xPainter & p)
{
    if (NULL == pView)
    {
        return false;
    }

    p.fillRoundedRect(pView->clientRect(), ROUND_BTN_BOUND_PIXEL - 1, pView->background.GetColor());

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
EXPORT_STYLE_CLASS(xRoundFrameBtnStyle);
xRoundFrameBtnStyle::xRoundFrameBtnStyle()
{

}

xRoundFrameBtnStyle::~xRoundFrameBtnStyle()
{

}

bool xRoundFrameBtnStyle::onViewPaintBackground(xView * pView, xPainter & p)
{
    if (NULL == pView)
    {
        return false;
    }
    p.drawRoundedRect(pView->clientRect(), ROUND_BTN_BOUND_PIXEL, xColor(0, 0, 0));
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
EXPORT_STYLE_CLASS(xNormalTitleStyle);
xNormalTitleStyle::xNormalTitleStyle()
{

}

xNormalTitleStyle::~xNormalTitleStyle()
{

}

bool xNormalTitleStyle::onViewPaintBackground(xView * pView, xPainter & p)
{
    if (NULL == pView)
    {
        return false;
    }

    chRect rcClient = pView->clientRect();
    int nY = rcClient.top + (rcClient.Height() >> 1);

    p.drawLineX(nY, rcClient.left, rcClient.right, T2X_SYSTEM_COLOR_BLACK);

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////
EXPORT_STYLE_CLASS(CMsgBoxCommonTitleStyle);
CMsgBoxCommonTitleStyle::CMsgBoxCommonTitleStyle()
    : m_nTildeWidth(0)
    , m_nTildeHeight(0)
{
}

CMsgBoxCommonTitleStyle::~CMsgBoxCommonTitleStyle()
{
}

void CMsgBoxCommonTitleStyle::Init()
{
    xStyle * pStyle = xStyle::getObjectByName("CMsgBoxCommonTitleStyle");
    CMsgBoxCommonTitleStyle * pTitleStyle = static_cast<CMsgBoxCommonTitleStyle *>(pStyle);

    if (NULL == pTitleStyle)
    {
        return;
    }

    yl::string strTilde(1, g_cTilde);
    xFont fontDefault;

    chSize szTilde = fontDefault.textBound(strTilde);

    pTitleStyle->m_nTildeWidth = szTilde.cx;
    pTitleStyle->m_nTildeHeight = szTilde.cy;
}

bool CMsgBoxCommonTitleStyle::onViewPaintContent(xView * pView, xPainter & p)
{
    xTextView * pTextView = static_cast<xTextView *>(pView);

    if (NULL == pTextView || m_nTildeWidth <= 0)
    {
        return false;
    }

    chRect rcContent = pTextView->contentRect();
    int nTextWidth = pTextView->cacheTextSize().cx;
    int nTildeWidth = (rcContent.Width() - nTextWidth) >> 1;
    int nTildeCount = nTildeWidth / m_nTildeWidth;
    const xFont & fontText = pTextView->GetFont();
    const xColor & clrText = pTextView->GetTextColor();

    if (nTildeCount > 0)
    {
        yl::string strTilde(nTildeCount, g_cTilde);
        chRect rcTilde(0, 0, nTildeCount * m_nTildeWidth, m_nTildeHeight);
        chRect rcLeftTilde(rcContent.left, rcContent.top, rcContent.left + nTildeWidth, rcContent.bottom);

        adjustRectByGravity(rcLeftTilde, (gravity_right | gravity_y_center), rcTilde);

        p.drawText(rcTilde, strTilde.c_str(), (align_right | align_v_center), fontText, clrText);

        chRect rcRightTilde(rcContent.right - nTildeWidth, rcContent.top, rcContent.right,
                            rcContent.bottom);

        adjustRectByGravity(rcRightTilde, (gravity_left | gravity_y_center), rcTilde);

        p.drawText(rcTilde, strTilde.c_str(), (align_left | align_v_center), fontText, clrText);
    }

    pTextView->DrawText(p, clrText);

    return true;
}

#if IF_OPTION_BOX_SUPPORT_RECT_BG
EXPORT_STYLE_CLASS(COptionRectFrameBGStyle);
COptionRectFrameBGStyle::COptionRectFrameBGStyle()
{
}

COptionRectFrameBGStyle::~COptionRectFrameBGStyle()
{
}

bool COptionRectFrameBGStyle::onViewPaintBackground(xView * pView, xPainter & p)
{
    xListViewPtr pListView = static_cast<xListView*>(pView);
    if (NULL == pListView)
    {
        return false;
    }

    xScrollCtrlPtr pScroll = pListView->GetScrollCtrl();
    if (NULL == pScroll)
    {
        return false;
    }

    int nWidth = pScroll->width();
    chRect rcClient = pView->clientRect();
    rcClient.right = rcClient.right - pScroll->width() - 1;

    p.fillRect(rcClient, g_WorkStation.profile().m_clrViewDefaultBG);
    p.drawRect(rcClient, T2X_SYSTEM_COLOR_BLACK);

    return true;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////
EXPORT_STYLE_CLASS(CMsgBoxRoundBGStyle);
CMsgBoxRoundBGStyle::CMsgBoxRoundBGStyle()
{
}

CMsgBoxRoundBGStyle::~CMsgBoxRoundBGStyle()
{
}

bool CMsgBoxRoundBGStyle::onViewPaintBackground(xView * pView, xPainter & p)
{
    if (NULL == pView)
    {
        return false;
    }

    chRect rcClient = pView->clientRect();

    PaintRoundFrame(p, rcClient, g_WorkStation.profile().m_clrViewDefaultBG);

    return true;
}
#endif
