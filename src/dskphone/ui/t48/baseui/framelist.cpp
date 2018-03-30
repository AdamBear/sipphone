#include <QtGui>
#include "framelist.h"
#include "uikernel/qwidgetutility.h"
#include "imagemanager/modimagemanager.h"

#ifndef FL_SUPPORT_CUSTOM_FRAME_STYLE
#include "t4xpicpath.h"
#endif

namespace
{
#define DEFAULT_PAGE_CTRL_FONT_SIZE 14
}

CFrameList::CFrameList(QWidget * parent, CScrollCtrl * pScrollCtrl/* = NULL*/)
    : CFrameListBase(parent)
#ifdef FL_SUPPORT_TITLE
    , m_pTitle(NULL)
#endif
    , m_pScrollCtrl(pScrollCtrl)
#ifdef FL_SUPPORT_SCROLL_PAGE
    , m_nScrollType(SCROLL_VIEW_TYPE_PAGE)
#else
    , m_nScrollType(SCROLL_VIEW_TYPE_PAGE | SCROLL_VIEW_TYPE_ITEM)
#endif
    , m_nScrollShow(SCROLL_SHOW_MULTI_PAGE)
#ifdef FL_SUPPORT_CUSTOM_FRAME_STYLE
    , m_bGridLine(false)
    , m_bLineByContent(false)
    , m_bOnlyTitleLine(false)
    , m_bCustomTitleLine(false)
    , m_bOneItemBGBottom(false)
    , m_bAdjustForFkb(true)
    , m_nLinePosY(1)
    , m_clrGridLine(QColor(166, 166, 166))
    , m_strItemSelTopBG("")
    , m_strItemSelMidBG("")
    , m_strItemSelBottomBG("")
    , m_strItemLineImage("")
    , m_nLineHeight(1)
#endif
{
    if (NULL == m_pScrollCtrl)
    {
        m_pScrollCtrl = new CScrollCtrl(this, this);
    }
    else
    {
        m_pScrollCtrl->setObjectName("ScrollCtrl");
        m_pScrollCtrl->setParent(this);
        m_pScrollCtrl->SetScrollCtrlNotify(this);
    }

    if (NULL != m_pScrollCtrl)
    {
        m_pScrollCtrl->hide();
    }

#ifdef FL_SUPPORT_SCROLL_PAGE
    SetPageCtrlFontSize(DEFAULT_PAGE_CTRL_FONT_SIZE, QFont::Normal);
#endif

    setObjectName("CFrameList");
}

CFrameList::~CFrameList()
{
}

void CFrameList::AdjustChildWidget()
{
    QRect rcFrame = rect();
    int nItemTop = rcFrame.top();
    int nItemLeft = rcFrame.left();
    int nItemWidth = rcFrame.width();

#ifdef FL_SUPPORT_CUSTOM_FRAME_STYLE
    nItemTop += m_nTopMargin;
    nItemLeft += m_nLeftMargin;
    nItemWidth -= (m_nLeftMargin + m_nRightMargin);
#endif

#ifdef FL_SUPPORT_TITLE
    if (NULL != m_pTitle)
    {
        QRect rcTitle = m_pTitle->geometry();
        int nTitleHeight = rcTitle.height();

        m_pTitle->setGeometry(nItemLeft, nItemTop, nItemWidth, nTitleHeight);
        nItemTop += nTitleHeight;

#ifdef FL_SUPPORT_CUSTOM_FRAME_STYLE
        if (m_bCustomTitleLine)
        {
            nItemTop += m_nItemSpace;
        }
#endif
    }
#endif

    int nPageCtrlWidth = CScrollCtrl::GetDefaultWidth();
    int nRightMargin = CScrollCtrl::GetRightMargin();

    if (NULL != m_pScrollCtrl &&  m_pScrollCtrl->isVisible())
    {
        QRect rcPageCtrl = m_pScrollCtrl->rect();

        nItemWidth -= (nPageCtrlWidth + nRightMargin);

        rcPageCtrl.setLeft(nItemLeft + nItemWidth);
        rcPageCtrl.setWidth(nPageCtrlWidth);
        rcPageCtrl.setTop(nItemTop + m_pScrollCtrl->GetTopMargin());
        rcPageCtrl.setBottom(rcFrame.bottom() - m_pScrollCtrl->GetBottomMargin());

        m_pScrollCtrl->SetRect(rcPageCtrl);
    }

    if (NULL != m_pLabNote && !m_bNoteFixPos)
    {
        m_pLabNote->setGeometry(nItemLeft, nItemTop, nItemWidth, (rcFrame.height() - nItemTop));
    }

    AdjustChildItem(nItemLeft, nItemTop, nItemWidth);
}

void CFrameList::OnClearContent()
{
    if (NULL != m_pScrollCtrl)
    {
        m_pScrollCtrl->SetScrollInfo(0, 0);

        if (m_pScrollCtrl->isVisible())
        {
            m_pScrollCtrl->hide();
        }
    }

    CFrameListBase::OnClearContent();
}

void CFrameList::OnDataCountChange()
{
    if (NULL == m_pScrollCtrl)
    {
        return;
    }

    int nDataCount = GetTotalDataCount();
    int nPageItemCount = GetMaxPageItemCount();
    int nPageCount = 0;
    int nPageNo = 0;

    if (nPageItemCount > 0)
    {
        nPageCount = (nDataCount + nPageItemCount - 1) / nPageItemCount;

        CListItemPtr pItem = GetItemByIndex(0);

        if (NULL != pItem)
        {
            nPageNo = (pItem->GetDataIndex() + nPageItemCount) / nPageItemCount - 1;
        }
    }

#ifdef FL_SUPPORT_SCROLL_PAGE
    m_pScrollCtrl->SetScrollInfo(nPageCount, nPageNo);
#else
#ifdef FL_SUPPORT_SCROLL_SLIDE
    m_pScrollCtrl->SetScrollInfo(nDataCount, 0);
#endif
#endif

    if (nPageCount <= 1 && SCROLL_SHOW_MULTI_PAGE == m_nScrollShow)
    {
        if (m_pScrollCtrl->isVisible())
        {
            m_pScrollCtrl->hide();
            AdjustChildWidget();
        }
    }
    else if (!m_pScrollCtrl->isVisible())
    {
        m_pScrollCtrl->raise();
        m_pScrollCtrl->show();
        AdjustChildWidget();
    }
}

#ifdef FL_SUPPORT_SCROLL_PAGE
void CFrameList::OnDataIndexChange(CListItem * pItem)
{
    if (NULL == pItem || NULL == m_pScrollCtrl || !m_pScrollCtrl->isVisible())
    {
        return;
    }

    CListItemPtr pFristItem = GetItemByIndex(0);

    if (pFristItem != pItem)
    {
        return;
    }

    int nPageItemCount = GetMaxPageItemCount();

    if (nPageItemCount <= 0)
    {
        return;
    }

    int nDataIndex = pItem->GetDataIndex();
    int nPageNo = (nDataIndex + nPageItemCount) / nPageItemCount - 1;

    if (nPageNo < 0)
    {
        nPageNo = 0;
    }

    if (m_pScrollCtrl->GetPageNo() != nPageNo)
    {
        m_pScrollCtrl->SetScrollPos(nPageNo);
    }
}
#endif

#ifdef FL_SUPPORT_SCROLL_SLIDE
void CFrameList::OnItemFocusChange(CListItem * pItem, bool bPreFocus)
{
    CFrameListBase::OnItemFocusChange(pItem, bPreFocus);

    if (NULL == m_pScrollCtrl || NULL == pItem || !pItem->IsFocus())
    {
        return;
    }

    m_pScrollCtrl->SetScrollPos(pItem->GetDataIndex());
}
#endif

const char * CFrameList::GetFocusBg(int nDataIndex)
{
#ifdef FL_SUPPORT_CUSTOM_FRAME_STYLE
    int nItemIndex = GetIndexByDataIndex(nDataIndex);

    if (0 == nItemIndex)
    {
        if (m_bOneItemBGBottom && !m_strItemSelBottomBG.empty() && 1 == GetTotalItemCount())
        {
            return m_strItemSelBottomBG.c_str();
        }

        if (!m_strItemSelTopBG.empty())
        {
            return m_strItemSelTopBG.c_str();
        }
    }
    else if ((nItemIndex + 1) == GetMaxPageItemCount())
    {
        if (!m_strItemSelBottomBG.empty())
        {
            return m_strItemSelBottomBG.c_str();
        }
    }

    return m_strItemSelMidBG.c_str();
#else
    static yl::string s_strItemBg = PIC_THEME1_ITEMBACKGROUND;

    return s_strItemBg.c_str();
#endif
}

const char * CFrameList::GetClickBg(int nDataIndex)
{
#ifdef FL_SUPPORT_CUSTOM_FRAME_STYLE
    int nItemIndex = GetIndexByDataIndex(nDataIndex);

    if (0 == nItemIndex)
    {
        if (m_bOneItemBGBottom && !m_strItemClickBottomBG.empty() && 1 == GetTotalItemCount())
        {
            return m_strItemClickBottomBG.c_str();
        }

        if (!m_strItemClickTopBG.empty())
        {
            return m_strItemClickTopBG.c_str();
        }
    }
    else if ((nItemIndex + 1) == GetMaxPageItemCount())
    {
        if (!m_strItemClickBottomBG.empty())
        {
            return m_strItemClickBottomBG.c_str();
        }
    }

    return m_strItemClickMidBG.c_str();
#else
    return NULL;
#endif
}

void CFrameList::SetScrollViewType(int nType)
{
    m_nScrollType = nType;
}

void CFrameList::SetScrollShow(int nType)
{
    m_nScrollShow = nType;
}

bool CFrameList::IsScrollByItem() const
{
    return (SCROLL_VIEW_TYPE_ITEM == (SCROLL_VIEW_TYPE_ITEM & m_nScrollType));
}

bool CFrameList::IsScrollByPage() const
{
    return (SCROLL_VIEW_TYPE_PAGE == (SCROLL_VIEW_TYPE_PAGE & m_nScrollType));
}

bool CFrameList::eventFilter(QObject * pObject, QEvent * pEvent)
{
    if (NULL == pObject || NULL == pEvent)
    {
        return false;
    }

    if (NULL != m_pScrollCtrl)
    {
#ifdef FL_SUPPORT_SCROLL_PAGE
        if (m_pScrollCtrl->isVisible() && m_pScrollCtrl->eventFilter(pObject, pEvent))
        {
            return true;
        }
#else
        if (m_pScrollCtrl->eventFilter(pObject, pEvent))
        {
            return true;
        }
#endif
    }

#ifdef QT_KEYBOARD_SUPPORT
    if (QEvent::KeyPress == pEvent->type())
    {
        QKeyEvent * pKeyEvent = (QKeyEvent *)pEvent;
        CListItemPtr pFocusItem = GetFocusItem();

        if (NULL != pFocusItem)
        {
            if (PHONE_KEY_OK == pKeyEvent->key())
            {
                pFocusItem->SelectItem(true);
            }
            else if (pFocusItem->eventFilter(pObject, pEvent))
            {
                return true;
            }
        }
    }
#endif

    return CFrameListBase::eventFilter(pObject, pEvent);
}

void CFrameList::resizeEvent(QResizeEvent * pEvent)
{
    AdjustChildWidget();
}

void CFrameList::OnNotifyScrollPrevious()
{
    if (IsScrollByItem())
    {
        OnPreItem(IsScrollByPage());
    }
    else if (IsScrollByPage())
    {
        OnPrePage();
    }
}

void CFrameList::OnNotifyScrollNext()
{
    if (IsScrollByItem())
    {
        OnNextItem(IsScrollByPage());
    }
    else if (IsScrollByPage())
    {
        OnNextPage();
    }
}

#ifdef FL_SUPPORT_SCROLL_PAGE
void CFrameList::SetPageCtrlFontSize(int nFontSize, int nFontWeight/* = QFont::Normal*/)
{
    if (NULL != m_pScrollCtrl)
    {
        qWidgetUtility::setFontStyle(m_pScrollCtrl, nFontSize, nFontWeight, false);
    }
}
#endif

#ifdef FL_SUPPORT_TITLE
void CFrameList::SetFrameListTitle(CFrameListTitlePtr pFrameListTitle)
{
    if (NULL != m_pTitle)
    {
        m_pTitle->hide();
        m_pTitle->setParent(NULL);
        m_pTitle->deleteLater();
        m_pTitle = NULL;
    }

    if (NULL == pFrameListTitle)
    {
        AdjustChildWidget();
        return;
    }

    m_pTitle = pFrameListTitle;
    m_pTitle->setParent(this);
    m_pTitle->show();
    m_pTitle->raise();

    AdjustChildWidget();
}

CFrameListTitlePtr CFrameList::GetFrameListTitle() const
{
    return m_pTitle;
}

void CFrameList::ResetAction()
{
    CFrameListBase::ResetAction();

    if (NULL != m_pTitle)
    {
        m_pTitle->SetAction(NULL);
    }
}
#endif

#ifdef FL_SUPPORT_CUSTOM_FRAME_STYLE
void CFrameList::SetGridLine(bool bGridLine, bool bLineByContent/* = false*/)
{
    m_bGridLine = bGridLine;
    m_bLineByContent = bLineByContent;
}

void CFrameList::SetOnlyTitleLine(bool bOnlyTitleLine)
{
    m_bOnlyTitleLine = bOnlyTitleLine;
}

void CFrameList::SetCustomTitleLine(bool bCustom)
{
    m_bCustomTitleLine = bCustom;
}

void CFrameList::SetGridLineColor(QColor & clrLine)
{
    m_clrGridLine = clrLine;
}

void CFrameList::SetItemSelectBG(const char * pszBG)
{
    if (NULL == pszBG)
    {
        m_strItemSelTopBG = "";
        m_strItemSelMidBG = "";
        m_strItemSelBottomBG = "";
        return;
    }

    m_strItemSelTopBG = "";
    m_strItemSelMidBG = pszBG;
    m_strItemSelBottomBG = "";
}

void CFrameList::SetItemSelectBG(const char * pszTopBG/* = NULL*/, const char * pszMidBG/* = NULL*/,
                                 const char * pszBottomBG/* = NULL*/)
{
    if (NULL != pszTopBG)
    {
        m_strItemSelTopBG = pszTopBG;
    }
    else
    {
        m_strItemSelTopBG = "";
    }

    if (NULL != pszMidBG)
    {
        m_strItemSelMidBG = pszMidBG;
    }
    else
    {
        m_strItemSelMidBG = "";
    }

    if (NULL != pszBottomBG)
    {
        m_strItemSelBottomBG = pszBottomBG;
    }
    else
    {
        m_strItemSelBottomBG = "";
    }
}

void CFrameList::SetItemClickBG(const char * pszBG)
{
    if (NULL == pszBG)
    {
        m_strItemClickTopBG = "";
        m_strItemClickMidBG = "";
        m_strItemClickBottomBG = "";
        return;
    }

    m_strItemClickTopBG = "";
    m_strItemClickMidBG = pszBG;
    m_strItemClickBottomBG = "";
}

void CFrameList::SetItemClickBG(const char * pszTopBG, const char * pszMidBG,
                                const char * pszBottomBG)
{
    if (NULL != pszTopBG)
    {
        m_strItemClickTopBG = pszTopBG;
    }
    else
    {
        m_strItemClickTopBG = "";
    }

    if (NULL != pszMidBG)
    {
        m_strItemClickMidBG = pszMidBG;
    }
    else
    {
        m_strItemClickMidBG = "";
    }

    if (NULL != pszBottomBG)
    {
        m_strItemClickBottomBG = pszBottomBG;
    }
    else
    {
        m_strItemClickBottomBG = "";
    }
}

void CFrameList::SetOneItemBGBottom(bool bBottom)
{
    m_bOneItemBGBottom = bBottom;
}

void CFrameList::SetLineImage(const char * pszImgLine)
{
    if (NULL == pszImgLine)
    {
        m_strItemLineImage = "";
    }
    else
    {
        m_strItemLineImage = pszImgLine;
    }
}

void CFrameList::SetLineHeight(int nLineHeight)
{
    m_nLineHeight = nLineHeight;
}

void CFrameList::SetLinePosY(int nLinePosY)
{
    m_nLinePosY = nLinePosY;
}
#endif

#ifdef FL_SUPPORT_CUSTOM_FRAME_STYLE
void CFrameList::paintEvent(QPaintEvent * pEvent)
{
    QStylePainter stylePainter(this);

    if (m_bGridLine && m_strItemLineImage.empty())
    {
        stylePainter.setPen(m_clrGridLine);
    }

    QRect rcList = rect();
    int nListWidth = rcList.width();

    // 画标题栏分隔线
    if (NULL != m_pTitle && !m_bCustomTitleLine)
    {
        QRect rcTitle = m_pTitle->geometry();

        DrawGridLine(stylePainter, rcTitle.left(), rcTitle.bottom(), rcTitle.width(), nListWidth);
    }

    // 画列表项分隔线（最后一项不画分隔线）
    int i = 0;
    int nPageMaxItemCount = GetMaxPageItemCount();
    VEC_LIST_ITEM & vecItem = GetListItem();
    VEC_LIST_ITEM::iterator iter = vecItem.begin();

    for (; i < nPageMaxItemCount && iter != vecItem.end(); ++iter, ++i)
    {
        CListItemPtr pItem = *iter;

        if (NULL == pItem)
        {
            continue;
        }

        QRect rcItem = pItem->geometry();
        int nItemWidth = rcItem.width();

        if (pItem->IsDown())
        {
            rcItem.setWidth(nListWidth);
            DrawClickedGrid(stylePainter, rcItem, i);
        }
        else if (pItem->IsFocus())
        {
            rcItem.setWidth(nListWidth);
            DrawFocusGrid(stylePainter, rcItem, i);
        }

        if (m_bOnlyTitleLine)
        {
            continue;
        }

        if ((i + 1) != nPageMaxItemCount)
        {
            DrawGridLine(stylePainter, rcItem.left(), rcItem.bottom(), nItemWidth, nListWidth);
        }
    }
}

void CFrameList::DrawGridLine(QStylePainter & stylePainter, int nX, int nY, int nItemWidth,
                              int nListWidth)
{
    if (!m_bGridLine)
    {
        return;
    }

    // 画普通线
    if (m_strItemLineImage.empty())
    {
        int nRight = nX + nItemWidth;

        if (m_bLineByContent)
        {
            nX += m_nContentLeft;
            nRight -= m_nContentRight;
        }

        nY += m_nLinePosY;
        stylePainter.drawLine(nX, nY, nRight, nY);
        return;
    }

    // 根据切图画线
    QPixmap pmLine = THEME_GET_BMP(m_strItemLineImage.c_str());
    QRect rcLine(nX, nY, nListWidth, m_nLineHeight);

    if (!pmLine.isNull())
    {
        QRect rcSrc = pmLine.rect();

        if (rcSrc.width() > nListWidth)
        {
            rcSrc.setWidth(nListWidth);
        }

        // 线暂时画在两个格子中
        if (rcSrc.height() < m_nLineHeight)
        {
            rcSrc.setHeight(m_nLineHeight);
        }

        if (rcLine.height() < rcSrc.height())
        {
            rcLine.setHeight(rcSrc.height());
        }

        stylePainter.drawPixmap(rcLine, pmLine, rcSrc);
    }
}

void CFrameList::DrawFocusGrid(QStylePainter & stylePainter, const QRect & rcItem, int nItemIndex)
{
    yl::string strSelBG;

    if (0 == nItemIndex)
    {
        if (m_bOneItemBGBottom && !m_strItemSelBottomBG.empty() && 1 == GetTotalItemCount())
        {
            strSelBG = m_strItemSelBottomBG;
        }
        else if (NULL == m_pTitle && (!m_strItemSelTopBG.empty()))
        {
            strSelBG = m_strItemSelTopBG;
        }
    }
    else if ((1 + nItemIndex) == GetMaxPageItemCount())
    {
        if (!m_strItemSelBottomBG.empty())
        {
            strSelBG = m_strItemSelBottomBG;
        }
    }

    if (strSelBG.empty())
    {
        strSelBG = m_strItemSelMidBG.c_str();
    }


    QPixmap pmBg = THEME_GET_BMP(strSelBG.c_str());

    stylePainter.drawPixmap(rcItem, pmBg);
}

void CFrameList::DrawClickedGrid(QStylePainter & stylePainter, const QRect & rcItem, int nItemIndex)
{
    yl::string strClickBG;

    if (0 == nItemIndex)
    {
        if (m_bOneItemBGBottom && !m_strItemClickBottomBG.empty() && 1 == GetTotalItemCount())
        {
            strClickBG = m_strItemClickBottomBG;
        }
        else if (NULL == m_pTitle && (!m_strItemClickTopBG.empty()))
        {
            strClickBG = m_strItemClickTopBG;
        }
    }
    else if ((1 + nItemIndex) == GetMaxPageItemCount())
    {
        if (!m_strItemClickBottomBG.empty())
        {
            strClickBG = m_strItemClickBottomBG;
        }
    }

    if (strClickBG.empty())
    {
        strClickBG = m_strItemClickMidBG;
    }

    QPixmap pmBg = THEME_GET_BMP(strClickBG.c_str());

    stylePainter.drawPixmap(rcItem, pmBg);
}
#endif
