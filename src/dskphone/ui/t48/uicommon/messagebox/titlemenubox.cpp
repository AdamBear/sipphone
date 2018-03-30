#include "titlemenubox.h"

#include "baseui/ipopupmenudelegate.h"
#include "baseui/framelist.h"

#include "baseui/t4xpicpath.h"
#include "imagemanager/modimagemanager.h"
#include "uikernel/qwidgetutility.h"

#define TITLE_POPUP_MENU_ITEM_HEIGHT 50
#if IF_BUG_26252 || IF_FEATURE_STATION_NAME
#define TITLE_POPUP_MENU_ITEM_WIDTH 223
#else
#define TITLE_POPUP_MENU_ITEM_WIDTH 152
#endif
#define TITLE_POPUP_MENU_BOTTOM_BG_HEIGHT 50

#define TITLE_POPUP_MENU_ITEM_SPACE 1
#define TITLE_POPUP_MENU_LINE_HEIGHT 1
#define TITLE_POPUP_MENU_TITLE_HEIGHT 43

// #define TITLE_POPUP_MENU_FONT_SIZE 16

#define TITLE_POPUP_MENU_TITLE_LINE_COLOR QColor(215, 215, 215)

CTitleMenuBox::CTitleMenuBox(CMaskFrame * pMaskFrame/* = NULL*/,
                             IPopupMenuDelegate * pDelegate/* = NULL*/,
                             const QPoint & ptMenu/* = QPoint()*/)
    : CMessageBoxBase(pMaskFrame)
    , m_pFrameList(NULL)
{
    Init(pDelegate, ptMenu);
}

CTitleMenuBox::~CTitleMenuBox()
{
    if (NULL != m_pFrameList)
    {
        m_pFrameList->SetDelegate(NULL);
    }
}

void CTitleMenuBox::Init(IPopupMenuDelegate * pDelegate, const QPoint & ptMenu)
{
//  qWidgetUtility::setFontStyle(this, TITLE_POPUP_MENU_FONT_SIZE);

    if (NULL == pDelegate)
    {
        return;
    }

    m_pFrameList = new CFrameList(this);

    if (NULL == m_pFrameList)
    {
        return;
    }

    int nTotalItemCount = pDelegate->GetTotalMenuCount();
    int nHeight = TITLE_POPUP_MENU_ITEM_HEIGHT * nTotalItemCount + TITLE_POPUP_MENU_ITEM_SPACE *
                  (nTotalItemCount - 1);

    nHeight += TITLE_POPUP_MENU_TITLE_HEIGHT;

    setGeometry(ptMenu.x(), ptMenu.y(), TITLE_POPUP_MENU_ITEM_WIDTH, nHeight);

    m_pFrameList->setGeometry(0, 0, TITLE_POPUP_MENU_ITEM_WIDTH, nHeight);
    m_pFrameList->SetGridLine(true);
    m_pFrameList->SetCustomTitleLine(true);
    m_pFrameList->SetOneItemBGBottom(true);
    m_pFrameList->SetLinePosY(TITLE_POPUP_MENU_LINE_HEIGHT);
    m_pFrameList->SetLineHeight(TITLE_POPUP_MENU_LINE_HEIGHT);
    m_pFrameList->SetItemSpace(TITLE_POPUP_MENU_ITEM_SPACE);
    m_pFrameList->SetLineImage(PIC_THEME1_POPUP_MENU_LINE);
    m_pFrameList->SetItemSelectBG(NULL, PIC_THEME1_POPUP_MENU_MID_SELECT,
                                  PIC_THEME1_POPUP_MENU_BOTTOM_SELECT);
    m_pFrameList->SetItemClickBG(NULL, PIC_THEME1_POPUP_MENU_MID_CLICK,
                                 PIC_THEME1_POPUP_MENU_BOTTOM_CLICK);

    m_pFrameList->SetMaxPageItemCount(nTotalItemCount);
    m_pFrameList->SetDelegate(pDelegate);

    m_pFrameList->RefreshData(0);

}

void CTitleMenuBox::PrintMsgBoxInfo(bool bAdd)
{
    if (bAdd)
    {
        POPUPBOX_INFO("Add CTitleMenuBox::PrintMsgBoxInfo");
    }
    else
    {
        POPUPBOX_INFO("Del CTitleMenuBox::PrintMsgBoxInfo");
    }
}

void CTitleMenuBox::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    QRect rcDlg = rect();
    int nHeight = rcDlg.height();
    int nWidth = rcDlg.width();
    int nLeft = rcDlg.left();
    int nTop = rcDlg.top();

    // 画标题选中框
    QRect rcTitle(nLeft, nTop, nWidth, TITLE_POPUP_MENU_TITLE_HEIGHT);
    QPixmap pmTitle = THEME_GET_BMP(PIC_THEME1_POPUP_MENU_TITLE);

    stylePainter.drawPixmap(rcTitle, pmTitle);
    nTop += TITLE_POPUP_MENU_TITLE_HEIGHT;

    // 画标题分隔线
    stylePainter.setPen(TITLE_POPUP_MENU_TITLE_LINE_COLOR);
    stylePainter.drawLine(nLeft, nTop, nLeft + nWidth, nTop);
    nTop += TITLE_POPUP_MENU_LINE_HEIGHT;

    // 画Item中间背景
    int nMidHeight = rcDlg.top() + nHeight - nTop - TITLE_POPUP_MENU_BOTTOM_BG_HEIGHT;

    if (nMidHeight > 0)
    {
        QRect rcMid(nLeft, nTop, nWidth, nMidHeight);
        QPixmap pmMid = THEME_GET_BMP(PIC_THEME1_POPUP_MENU_MID_BG);

        stylePainter.drawPixmap(rcMid, pmMid);
        nTop += nMidHeight;
    }

    // 画Item底部圆角背景
    QRect rcBottom(nLeft, nTop, nWidth, TITLE_POPUP_MENU_BOTTOM_BG_HEIGHT);
    QPixmap pmBottom = THEME_GET_BMP(PIC_THEME1_POPUP_MENU_BOTTOM_BG);

    stylePainter.drawPixmap(rcBottom, pmBottom);
}

