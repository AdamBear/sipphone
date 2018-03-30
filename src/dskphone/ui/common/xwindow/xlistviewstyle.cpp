#include "xlistviewstyle.h"
#include "xListView.h"
#include "xWindowManager.h"
#include "dsklog/log.h"
#if IF_XLISTVIEW_SUPPORT_SLIDE
#include "xlistslideview.h"
#endif


namespace
{
#define LISTVIEW_ITEM_DEFAULT_SPACE 1
#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
#define LISTVIEW_DEFAULT_ITEM_LINE_COLOR xColor(0xa8, 0xaa, 0xb5)
#endif
#if IF_XLISTVIEW_SUPPORT_GROUP
#define LISTVIEW_SECTION_DEFAULT_BG_COLOR xColor(0xdd, 0xde, 0xe0)
#define LISTVIEW_SECTION_DEFAULT_TEXT_COLOR xColor(0x65, 0x76, 0x86)
#define LISTVIEW_SECTION_DEFAULT_TEXT_FONT_SIZE 38
#endif
}

#if IF_XLISTVIEW_SUPPORT_GROUP
////////////////////////////////////////////////////////////////////////////////////////////////////
xListViewSection::xListViewSection()
    : m_nSectionPos(0)
    , m_nSectionHeight(0)
    , m_nSectionSpace(0)
#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE
    , m_nSectionDrift(0)
#endif
    , m_nTitleHeight(0)
    , m_nBeginIndex(INVALID_ITEM_INDEX)
    , m_nEndIndex(INVALID_ITEM_INDEX)
    , m_strTitle("")
{
}

xListViewSection::~xListViewSection()
{

}

void xListViewSection::SetTitle(const yl::string& strTitle)
{
    if (!g_WorkStation.SetupTextTran(this, strTitle))
    {
        SetTranText(strTitle);
    }
}

void xListViewSection::SetTranText(const yl::string& strText)
{
    m_strTitle = strText;
}

yl::string xListViewSection::GetShowText(xFont& fontText, int nShowWidth)
{
    return CutOffText(m_strTitle, fontText, nShowWidth);
}

bool xListViewSection::IsTitleShow(int nContentPos, int nContentHeight)
{
    return (GetTitleYPos(nContentPos) + m_nTitleHeight <= nContentHeight);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
xListViewSectionStyle::xListViewSectionStyle()
    : m_brushTitleBG(LISTVIEW_SECTION_DEFAULT_BG_COLOR)
    , m_clrText(LISTVIEW_SECTION_DEFAULT_TEXT_COLOR)
    , m_nTextAlign(align_center)
    , m_nTextMargin(0)
{
    m_fontText.setPointSize(LISTVIEW_SECTION_DEFAULT_TEXT_FONT_SIZE);
}

xListViewSectionStyle::~xListViewSectionStyle()
{
}

void xListViewSectionStyle::loadAttributes(xml_node& node)
{
    PopupAndroidBrush(node, XWINDOW_NAME_SECTION_TITLE_BG, m_brushTitleBG);

    PopupAndroidColor(node, XWINDOW_NAME_SECTION_TEXT_COLOR, m_clrText);

    int nFontSize = LISTVIEW_SECTION_DEFAULT_TEXT_FONT_SIZE;

    if (PopupAndroidFont(node, XWINDOW_NAME_SECTION_TEXT_SIZE, nFontSize))
    {
        m_fontText.setPointSize(nFontSize);
    }

    bool bTextBold = false;

    if (PopupAndroidBool(node, XWINDOW_NAME_SECTION_TEXT_BOLD, bTextBold))
    {
        m_fontText.setBold(bTextBold);
    }

    PopupAndroidGravity(node, XWINDOW_NAME_SECTION_TEXT_ALIGN, m_nTextAlign);
    PopupAndroidInt(node, XWINDOW_NAME_SECTION_TEXT_MARGIN, m_nTextMargin);
}

void xListViewSectionStyle::PaintSection(xPainter& painter, xListViewSection* pSection, xListViewSection* pNextSection,
        xListViewStyle* pStyle, const chRect& rcContent)
{
    if (NULL == pSection || NULL == pStyle)
    {
        return;
    }

    chRect rcListView;

    pStyle->GetContainerGeometry(rcListView);

    // 画Title
    int nTitleYPos = pSection->GetTitleYPos(pStyle->m_nContentPos);

#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE
    if (pStyle->IsSectionDrift())
    {
        nTitleYPos += pSection->m_nSectionDrift;
    }
#endif

    if (pSection->m_nTitleHeight > 0 && nTitleYPos >= 0)
    {
        int nTitleTop = rcContent.top + nTitleYPos - rcListView.top;
        int nTitleBottom = nTitleTop + pSection->m_nTitleHeight;
        int nContentBottom = rcContent.bottom - rcListView.top;

        // 非第一行，要盖掉前面的线
        if (nTitleYPos > 0)
        {
            nTitleTop -= pStyle->m_nItemSpace;
        }

        if (nTitleBottom <= nContentBottom)
        {
            int nTitleLeft = rcContent.left - rcListView.left;
            int nTitleRight = rcContent.right - rcListView.left;
            chRect rcTitle(nTitleLeft, nTitleTop, nTitleRight, nTitleBottom);

            painter.blendRect(rcTitle, m_brushTitleBG);

            chRect rcText(rcTitle.left + m_nTextMargin, nTitleTop, rcTitle.right - m_nTextMargin, nTitleBottom);
            yl::string strShowTitle = pSection->GetShowText(m_fontText, rcText.Width());

            if (NULL != pListView->m_pHostStation)
            {
                pListView->m_pHostStation->GetRTLShowText(strShowTitle, strShowTitle);
            }

            painter.drawText(rcText, strShowTitle.c_str(), m_nTextAlign, m_fontText, m_clrText);
        }
    }

#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
    // 画Item的分隔线
    PaintItemSpaceLine(painter, pSection, pNextSection, pStyle, rcContent);
#endif
}

void xListViewSectionStyle::PaintItemSpaceLine(xPainter& painter, xListViewSection* pSection, xListViewSection* pNextSection,
        xListViewStyle* pStyle, const chRect& rcContent)
{
    if (NULL == pSection || NULL == pStyle)
    {
        return;
    }

    xListView* pListView = pStyle->m_pListView;

    if (NULL == pListView)
    {
        return;
    }

    int nIndex = pSection->m_nBeginIndex;
    xListViewItemPtr pFirstItem = pListView->GetFirstItem();

    if (NULL == pFirstItem)
    {
        return;
    }

    int nFirstDataIndex = pFirstItem->GetDataIndex();

    if (nIndex < nFirstDataIndex)
    {
        nIndex = nFirstDataIndex;
    }

    int nEndIndex = pSection->m_nEndIndex;
    bool bForceShowEndLine = false;

    if (!pStyle->m_bSpaceLineIgnoreEnd && (NULL == pNextSection
                                           || !pNextSection->IsTitleShow(pStyle->m_nContentPos, rcContent.Height())))
    {
        bForceShowEndLine = true;
    }

    if (bForceShowEndLine || (NULL != pNextSection && !pNextSection->IsHadSectionTitle()))
    {
        nEndIndex += 1;
    }

    chRect rcListView;

    pStyle->GetContainerGeometry(rcListView);

    for (; nIndex < nEndIndex; ++nIndex)
    {
        xListViewItemPtr pItem = pListView->GetItemByDataIndex(nIndex);

        if (NULL == pItem)
        {
            break;
        }

        chRect rcItem = pItem->geometry();

        if (rcItem.bottom < rcContent.top)
        {
            continue;
        }

        if (rcItem.bottom > rcContent.bottom)
        {
            break;
        }

#if XLIST_VIEW_DEBUG
        XWINDOW_INFO("Draw Item Line, Index:%d Y:%d X1:%d X2:%d", nIndex, rcItem.bottom, rcItem.left, rcItem.right);
#endif

        painter.drawLineX(rcItem.bottom - rcListView.top, rcItem.left - rcListView.left,
                          rcItem.right - rcListView.left, pStyle->m_clrSpaceLine);
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
IMPLEMENT_RUNTIME_FAMILY(xItemBGPainter);

xItemBGPainter::xItemBGPainter()
    : m_nType(ITEM_BG_PAINTER_TYPE_NONE)
{

}

xItemBGPainter::~xItemBGPainter()
{

}

yl::string xItemBGPainter::GetPainterName()
{
    return "";
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
yl::string xListViewStyle::s_strItemStyle;
#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
xItemBGPainterPtr xListViewStyle::s_pDefaultItemBGPainter = NULL;
#endif

xListViewStyle::xListViewStyle(xListView* pListView, int nType/* = LIST_VIEW_STYLE_TYPE_DEFAULT*/)
    : m_pListView(pListView)
    , m_nType(nType)
#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
#if IF_XLISTVIEW_SHOW_ITEMS_BY_PAGE
    , m_nScrollType(SCROLL_VIEW_TYPE_PAGE | SCROLL_VIEW_TYPE_ITEM)
#else
    , m_nScrollType(SCROLL_VIEW_TYPE_ITEM)
#endif // IF_XLISTVIEW_SHOW_ITEMS_BY_PAGE
#else
    , m_nScrollType(SCROLL_VIEW_TYPE_PAGE)
#endif // IF_XLISTVIEW_SWITCH_ITEM_FOCUS
#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
    , m_nFocusType(ITEM_FOCUS_TYPE_AUTO)
#else
    , m_nFocusType(ITEM_FOCUS_TYPE_NONE)
#endif
    , m_nPageItemCount(0)
    , m_nContentPos(0)
    , m_nContentHeight(0)
    , m_nPageHeight(0)
    , m_nItemTopMargin(0)
    , m_nItemBottomMargin(0)
    , m_nItemLeftMargin(0)
    , m_nItemRightMargin(0)
    , m_nItemWidth(0)
    , m_nItemHeight(0)
    , m_nItemSpace(LISTVIEW_ITEM_DEFAULT_SPACE)
    , m_nScrollCtrlTopMargin(0)
    , m_nScrollCtrlBottomMargin(0)
    , m_nScrollCtrlRightMargin(0)
#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
    , m_pItemBGPainter(NULL)
#endif
#if IF_XLISTVIEW_SUPPORT_TITLE
    , m_nTitleSpace(LISTVIEW_ITEM_DEFAULT_SPACE)
    , m_nTitleLineType(LIST_TITLE_LINE_TYPE_ALWAYS)
    , m_nTitleLinePosType(LIST_TITLE_LINE_POS_TYPE_IN_SPACE)
#endif
    , m_nFixPosFlag(LIST_VIEW_FIX_POS_NONE)
#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
    , m_clrSpaceLine(LISTVIEW_DEFAULT_ITEM_LINE_COLOR)
    , m_nSpaceLineStyle(LINE_TYPE_NONE)
    , m_bSpaceLineIgnoreEnd(false)
#endif
#if IF_XLISTVIEW_SUPPORT_GROUP
    , m_pSectionStyle(NULL)
    , m_nItemsStyle(LIST_VIEW_STYLE_ITEMS_STYLE_NORMAL)
    , m_nSectionCount(0)
#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE
    , m_bSectionDrift(false)
#endif
#endif
#if IF_XLISTVIEW_SUPPORT_SLIDE
    , m_nItemOffset(0)
#endif
{
}

xListViewStyle::~xListViewStyle()
{
#if IF_XLISTVIEW_SUPPORT_GROUP
    ClearGroupSection();
    DeleteSectionStyle();
#endif
}

xListViewStyle* xListViewStyle::CreateListViewStyle(xListView* pListView)
{
    xListViewStyle* pStyle = new xListViewStyle(pListView);

    return pStyle;
}

void xListViewStyle::loadAttributes(xml_node& node)
{
    PopupAndroidInt(node, XWINDOW_NAME_SCROLL_TYPE, m_nScrollType);
    PopupAndroidInt(node, XWINDOW_NAME_FOCUS_TYPE, m_nFocusType);
    PopupAndroidInt(node, XWINDOW_NAME_PAGE_ITEM_COUNT, m_nPageItemCount);
    PopupAndroidInt(node, XWINDOW_NAME_CONTENT_HEIGHT, m_nContentHeight);
    PopupAndroidInt(node, XWINDOW_NAME_PAGE_HEIGHT, m_nPageHeight);
    PopupAndroidInt(node, XWINDOW_NAME_IM_TOP, m_nItemTopMargin);
    PopupAndroidInt(node, XWINDOW_NAME_IM_BOTTOM, m_nItemBottomMargin);
    PopupAndroidInt(node, XWINDOW_NAME_IM_LEFT, m_nItemLeftMargin);
    PopupAndroidInt(node, XWINDOW_NAME_IM_RIGHT, m_nItemRightMargin);
    PopupAndroidInt(node, XWINDOW_NAME_ITEM_WIDTH, m_nItemWidth);
    PopupAndroidInt(node, XWINDOW_NAME_ITEM_HEIGHT, m_nItemHeight);
    PopupAndroidInt(node, XWINDOW_NAME_ITEM_SPACE, m_nItemSpace);
    PopupAndroidInt(node, XWINDOW_NAME_SCM_TOP, m_nScrollCtrlTopMargin);
    PopupAndroidInt(node, XWINDOW_NAME_SCM_BOTTOM, m_nScrollCtrlBottomMargin);
    PopupAndroidInt(node, XWINDOW_NAME_SCM_RIGHT, m_nScrollCtrlRightMargin);
    PopupAndroidInt(node, XWINDOW_NAME_FIX_POS, m_nFixPosFlag);

#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
    PopupAndroidColor(node, XWINDOW_NAME_SPACE_LINE_COLOR, m_clrSpaceLine);
    PopupAndroidInt(node, XWINDOW_NAME_SPACE_LINE_STYLE, m_nSpaceLineStyle);
    PopupAndroidBool(node, XWINDOW_NAME_SPACE_LINE_IGNORE_END, m_bSpaceLineIgnoreEnd);
#endif

    PopupAndroidString(node, XWINDOW_NAME_ITEM_STYLE, m_strItemStyle);

#if IF_XLISTVIEW_SUPPORT_GROUP
    PopupAndroidInt(node, XWINDOW_NAME_ITEMS_STYLE, m_nItemsStyle);
    PopupAndroidInt(node, XWINDOW_NAME_SECTION_COUNT, m_nSectionCount);

    int nSectionStyle = LIST_VIEW_SECTION_STYLE_DEFAULT;

    if (PopupAndroidInt(node, XWINDOW_NAME_SECTION_STYLE, nSectionStyle) || LIST_VIEW_STYLE_ITEMS_STYLE_GROUP == m_nItemsStyle)
    {
        SetGroupSectionStyle(nSectionStyle);
    }

    if (NULL != m_pSectionStyle)
    {
        m_pSectionStyle->loadAttributes(node);
    }
#endif

#if IF_XLISTVIEW_SUPPORT_TITLE
    PopupAndroidInt(node, XWINDOW_NAME_TITLE_SPACE, m_nTitleSpace);
    PopupAndroidInt(node, XWINDOW_NAME_TITLE_LINE_TYPE, m_nTitleLineType);
    PopupAndroidInt(node, XWINDOW_NAME_TITLE_LINE_POS_TYPE, m_nTitleLinePosType);
#endif

#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
    yl::string strFocusItemBG;

    PopupAndroidString(node, XWINDOW_NAME_FOCUS_ITEM_BG, strFocusItemBG);

    SetItemBGPainter(strFocusItemBG);
#endif
}

void xListViewStyle::SetPageItemCount(int nPageCount)
{
    if (nPageCount < 0)
    {
        nPageCount = 0;
    }

    m_nPageItemCount = nPageCount;
}

void xListViewStyle::SetContentHeight(int nHeight)
{
    m_nContentHeight = nHeight;

#if IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
    if (NULL != m_pListView)
    {
        m_pListView->OnContentChange(m_nPageHeight, m_nContentHeight, m_nContentPos);
    }
#endif
}

void xListViewStyle::SetContentPos(int nContentPos)
{
#if IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
    bool bChanged = (nContentPos != m_nContentPos);
#endif

    m_nContentPos = nContentPos;

#if IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
    if (bChanged && NULL != m_pListView)
    {
        m_pListView->OnContentPosChange(m_nContentPos);
    }
#endif
}

void xListViewStyle::SetScrollCtrlMargin(int nTopMargin, int nBottomMargin, int nRightMargin)
{
    m_nScrollCtrlTopMargin = nTopMargin;
    m_nScrollCtrlBottomMargin = nBottomMargin;
    m_nScrollCtrlRightMargin = nRightMargin;
}

#if IF_XLISTVIEW_SUPPORT_TITLE

void xListViewStyle::SetTitleLineType(int nLineType)
{

}

void xListViewStyle::SetTitleLinePosType(int nLinePosType)
{

}
#endif

void xListViewStyle::SetGlobalItemStyle(const yl::string& strItemStyle)
{
    s_strItemStyle = strItemStyle;
}

yl::string xListViewStyle::GetItemStyle() const
{
    if (m_strItemStyle.empty())
    {
        return s_strItemStyle;
    }

    return m_strItemStyle;
}

#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
void xListViewStyle::SetDefaultItemBGPainter(const yl::string& strPainter)
{
    s_pDefaultItemBGPainter = xItemBGPainter::getObjectByName(strPainter);
}

void xListViewStyle::SetItemBGPainter(const yl::string& strPainter)
{
    if (strPainter.empty())
    {
        m_pItemBGPainter = s_pDefaultItemBGPainter;
    }
    else
    {
        m_pItemBGPainter = xItemBGPainter::getObjectByName(strPainter);
    }
}
#endif

#if IF_XLISTVIEW_SUPPORT_GROUP
void xListViewStyle::SetItemsStyle(int nItemsStyle)
{
    m_nItemsStyle = nItemsStyle;

    if (LIST_VIEW_STYLE_ITEMS_STYLE_NORMAL == nItemsStyle)
    {
        ClearGroupSection();
        DeleteSectionStyle();
    }
}

void xListViewStyle::SetGroupSectionStyle(int nStyle)
{
    ClearGroupSection();

    m_pSectionStyle = currentStyleFactory.CreateSectionStyle(nStyle);
}

void xListViewStyle::InitGroupSection()
{
    if (LIST_VIEW_STYLE_ITEMS_STYLE_GROUP != m_nItemsStyle || m_nSectionCount <= 0)
    {
        return;
    }

    IListViewAdapterPtr pAdapter = NULL;

    if (NULL != m_pListView)
    {
        pAdapter = m_pListView->GetAdapter();
    }

    if (NULL == pAdapter)
    {
        return;
    }

    int nSize = m_vecSection.size();
    int i = 0;

    // 重用原有Section
    for (; i < m_nSectionCount && i < nSize; ++i)
    {
        xListViewSection* pSection = m_vecSection[i];

        if (!pAdapter->PrepareSection(m_pListView, i, pSection))
        {
            break;
        }
    }

    if (i < nSize)
    {
        for (int j = i; i < nSize; ++i)
        {
            xListViewSection* pSection = m_vecSection[j];

            m_vecSection[j] = NULL;
            m_vecSection.removeAt(j);

            if (NULL != pSection)
            {
                delete pSection;
            }
        }

        return;
    }

    for (; i < m_nSectionCount; ++i)
    {
        xListViewSection* pSection = new xListViewSection;

        if (pAdapter->PrepareSection(m_pListView, i, pSection))
        {
            m_vecSection.push_back(pSection);
            continue;
        }

        if (NULL != pSection)
        {
            delete pSection;
        }

        break;
    }
}

void xListViewStyle::ClearGroupSection()
{
    VEC_LIST_VIEW_SECTION::iterator iter = m_vecSection.begin();

    for (; iter != m_vecSection.end(); ++iter)
    {
        xListViewSectionPtr pSection = *iter;

        if (NULL != pSection)
        {
            delete pSection;
        }
    }

    m_vecSection.clear();
}

void xListViewStyle::DeleteSectionStyle()
{
    if (NULL != m_pSectionStyle)
    {
        delete m_pSectionStyle;

        m_pSectionStyle = NULL;
    }
}

xListViewSectionPtr xListViewStyle::GetSectionByIndex(int nDataIndex)
{
    int nSize = m_vecSection.size();

    if (nSize <= 0)
    {
        return NULL;
    }

    if (1 == nSize)
    {
        return m_vecSection[0];
    }

    int nBeginIndex = 0;
    int nEndIndex = nSize - 1;

    while (nBeginIndex != nEndIndex)
    {
        xListViewSectionPtr pEndSection = m_vecSection[nEndIndex];

        if (NULL == pEndSection)
        {
            return NULL;
        }

        if (nDataIndex >= pEndSection->m_nBeginIndex && nDataIndex <= pEndSection->m_nEndIndex)
        {
            return pEndSection;
        }

        int nMidIndex = (nBeginIndex + nEndIndex) >> 1;
        xListViewSectionPtr pMidSection = m_vecSection[nMidIndex];

        if (NULL == pMidSection)
        {
            return NULL;
        }

        if (nDataIndex > pMidSection->m_nEndIndex)
        {
            if (nBeginIndex == nMidIndex)
            {
                break;
            }

            nBeginIndex = nMidIndex;
            continue;
        }

        if (nDataIndex < pMidSection->m_nBeginIndex)
        {
            if (nEndIndex == nMidIndex)
            {
                break;
            }

            nEndIndex = nMidIndex;
            continue;
        }

        return pMidSection;
    }

    return NULL;
}

xListViewSectionPtr xListViewStyle::GetSectionByPos(int nSectionPos)
{
    int nSize = m_vecSection.size();

    if (nSize <= 0)
    {
        return NULL;
    }

    int nBeginIndex = 0;
    int nEndIndex = nSize - 1;

    while (nBeginIndex != nEndIndex)
    {
        xListViewSectionPtr pEndSection = m_vecSection[nEndIndex];

        if (NULL == pEndSection)
        {
            return NULL;
        }

        if (pEndSection->m_nSectionPos <= nSectionPos)
        {
            return pEndSection;
        }

        int nMidIndex = (nBeginIndex + nEndIndex) >> 1;
        xListViewSectionPtr pMidSection = m_vecSection[nMidIndex];

        if (NULL == pMidSection)
        {
            return NULL;
        }

        if (pMidSection->m_nSectionPos > nSectionPos)
        {
            if (nEndIndex == nMidIndex)
            {
                break;
            }

            nEndIndex = nMidIndex;
            continue;
        }

        if (nBeginIndex != nMidIndex && pMidSection->m_nSectionPos < nSectionPos)
        {
            if (nBeginIndex == nMidIndex)
            {
                break;
            }

            nBeginIndex = nMidIndex;
            continue;
        }

        return pMidSection;
    }

    return NULL;
}

#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE
void xListViewStyle::SetSectionDrift(bool bDrift)
{
    if (bDrift && m_vecSection.size() > 0)
    {
        m_bSectionDrift = bDrift;
    }
    else
    {
        m_bSectionDrift = false;
    }
}

void xListViewStyle::ResetSectionDrift()
{
    int nSize = m_vecSection.size();

    for (int i = 0; i < nSize; ++i)
    {
        xListViewSectionPtr pSection = m_vecSection[i];

        if (NULL != pSection)
        {
            pSection->m_nSectionDrift = 0;
        }
    }
}

void xListViewStyle::DriftSection(int nBeginIndex, int nEndIndex, int nOffset)
{
    if (0 == nOffset || nBeginIndex > nEndIndex || m_vecSection.size() <= 0)
    {
        return;
    }

    xListViewSectionPtr pSection = GetSectionByIndex(nBeginIndex);
    int nLoop = 0;

    while (NULL != pSection)
    {
        DEAD_LOOP_BREAK(nLoop)

        pSection->m_nSectionDrift = nOffset;
        nBeginIndex = pSection->GetEndIndex() + 1;

        if (nBeginIndex > nEndIndex)
        {
            break;
        }

        pSection = GetSectionByIndex(nBeginIndex);
    }
}

void xListViewStyle::AddUpNextSecDrift(int nBeginIndex, int nEndIndex, int nOffset)
{
    if (0 == nOffset || nBeginIndex > nEndIndex || m_vecSection.size() <= 0)
    {
        return;
    }

    xListViewSectionPtr pSection = GetSectionByIndex(nBeginIndex);
    bool bBeginSection = true;
    int nLoop = 0;

    while (NULL != pSection)
    {
        DEAD_LOOP_BREAK(nLoop)

        bool bAddup = true;

        if (bBeginSection)
        {
            bAddup = false;
            bBeginSection = false;
        }

        if (bAddup)
        {
            pSection->m_nSectionDrift += nOffset;
        }

        nBeginIndex = pSection->GetEndIndex() + 1;

        if (nBeginIndex > nEndIndex)
        {
            break;
        }

        pSection = GetSectionByIndex(nBeginIndex);
    }
}
#endif
#endif

void xListViewStyle::OnFinishInit()
{
    if (NULL == m_pListView || m_nItemHeight <= 0)
    {
        return;
    }

    m_nPageHeight = m_nPageItemCount * (m_nItemHeight + m_nItemSpace);

#if IF_XLISTVIEW_SUPPORT_GROUP
    int nSectionCount = m_vecSection.size();

    if (nSectionCount > 0)
    {
        int nSectionPos = 0;

        for (int i = 0; i < nSectionCount; ++i)
        {
            xListViewSectionPtr pSection = m_vecSection[i];

            if (NULL == pSection)
            {
                continue;
            }

            pSection->m_nSectionPos = nSectionPos;

            int nItemCount = pSection->m_nEndIndex - pSection->m_nBeginIndex + 1;
            int nSectionHeight = nItemCount * (m_nItemHeight + m_nItemSpace);

            pSection->m_nSectionHeight = nSectionHeight;

#if XLIST_VIEW_DEBUG
            XWINDOW_INFO("Section(%d %d) Pos:%d Height:%d TitleSpace:%d TitleHeight:%d",
                         pSection->m_nBeginIndex, pSection->m_nEndIndex,
                         pSection->m_nSectionPos, pSection->m_nSectionHeight,
                         pSection->m_nSectionSpace, pSection->m_nTitleHeight);
#endif

            nSectionPos = nSectionPos + nSectionHeight + pSection->GetTotalTitleHeight();
        }

        m_nContentHeight = nSectionPos;

        if (m_nContentHeight > m_nPageHeight && m_nContentHeight <= m_pListView->contentGeometry().Height())
        {
            m_nPageHeight = m_nContentHeight;
        }

#if IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
        m_pListView->OnContentChange(m_nPageHeight, m_nContentHeight, m_nContentPos);
#endif
        return;
    }
#endif

    int nTotalCount = m_pListView->GetTotalDataCount();

    m_nContentHeight = nTotalCount * (m_nItemHeight + m_nItemSpace);

#if IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
    m_pListView->OnContentChange(m_nPageHeight, m_nContentHeight, m_nContentPos);
#endif
}

void xListViewStyle::AdjustContentPosAndIndex(int& nDataIndex)
{
    if (m_nPageHeight <= 0)
    {
        return;
    }

    if (IsScrollByPage())
    {
        if (m_nPageItemCount <= 0)
        {
            return;
        }

        nDataIndex = nDataIndex - (nDataIndex % m_nPageItemCount);

        SetContentPos(GetContentPosByIndex(nDataIndex));
        return;
    }

    int nContentPos = GetContentPosByIndex(nDataIndex);
    int nMaxPos = GetMaxContentPos();

    if (nContentPos > nMaxPos)
    {
        nDataIndex = GetDataIndexByPos(nMaxPos);
        SetContentPos(nMaxPos);

        return;
    }

#if IF_XLISTVIEW_SUPPORT_GROUP
    if (m_nSectionCount > 0)
    {
        xListViewSectionPtr pSection = GetSectionByPos(m_nContentPos);

        if (NULL != pSection && nDataIndex == pSection->m_nBeginIndex
                && m_nContentPos == pSection->m_nSectionPos)
        {
            return;
        }
    }
#endif

    SetContentPos(nContentPos);
}

// 根据xListView的数据重新排列xScrollBar、xTextView
void xListViewStyle::Relayout()
{
    if (NULL == m_pListView || m_pListView->m_bClosed)
    {
        return;
    }

    chRect rcContent = m_pListView->contentRect();

    rcContent.OffsetRect(m_pListView->m_rectLayout.left, m_pListView->m_rectLayout.top);

#if IF_XLISTVIEW_SUPPORT_TITLE
    LayoutTitle(rcContent);
#endif

    LayoutScrollCtrl(rcContent);
    LayoutNote(rcContent);

#if IF_XLISTVIEW_SUPPORT_SLIDE
    LayoutSlideView(rcContent);
#endif
}

#if IF_XLISTVIEW_SUPPORT_TITLE
bool xListViewStyle::LayoutTitle(chRect& rcContent, bool bInLayout/* = false*/)
{
    if (NULL == m_pListView)
    {
        return false;
    }

    xListViewTitlePtr pTitle = m_pListView->GetTitle();

    if (NULL == pTitle)
    {
        return false;
    }

    int nTitleHeight = pTitle->GetTitleHeight();
    chRect rcTitle(rcContent.left, rcContent.top, rcContent.right, rcContent.top + nTitleHeight);
    chRect rcOldTitle = pTitle->geometry();

    rcContent.top += nTitleHeight;

    if (bInLayout)
    {
        pTitle->LayoutInBoundRect(rcTitle);
    }
    else
    {
        pTitle->ownerStation().LayoutViewInBoundRect(pTitle, rcTitle);
    }

    if (rcOldTitle.Height() != nTitleHeight)
    {
        return true;
    }

    return false;
}
#endif

void xListViewStyle::LayoutScrollCtrl(const chRect& rcContent)
{
    if (NULL == m_pListView)
    {
        return;
    }

    xScrollCtrlPtr pScrollCtrl = m_pListView->GetScrollCtrl();

    if (NULL == pScrollCtrl)
    {
        return;
    }

    if (!IsScrollCtrlFixPos())
    {
        int nRight = rcContent.right - m_nScrollCtrlRightMargin;
        int nBottom = rcContent.bottom - m_nScrollCtrlBottomMargin;
        int nTop = rcContent.top + m_nScrollCtrlTopMargin;

        chRect rcScrollCtrl(nRight - pScrollCtrl->layout_width, nTop, nRight, nBottom);

        pScrollCtrl->SetRect(rcScrollCtrl);
    }
}

void xListViewStyle::LayoutNote(const chRect& rcContent)
{
    if (NULL == m_pListView)
    {
        return;
    }

    if (IsHintFixPos())
    {
        return;
    }

#if IF_XLISTVIEW_NOTE_SHOW_IN_ITEM
    int nBottom = rcContent.bottom;

    if (m_nItemHeight > 0)
    {
        nBottom = rcContent.top + m_nItemHeight;
    }

    chRect rcNote(rcContent.left, rcContent.top, rcContent.right, nBottom);
#else
    chRect rcNote(rcContent.left, rcContent.top, rcContent.right, rcContent.bottom);
#endif

    m_pListView->SetHintRect(rcNote);
}
#if IF_XLISTVIEW_SUPPORT_SLIDE
void xListViewStyle::LayoutSlideView(const chRect& rcContent)
{
    if (NULL == m_pListView)
    {
        return;
    }

    xListSlideView* pListSlideView = m_pListView->GetListSlideView();

    if (NULL != pListSlideView)
    {
        pListSlideView->LayoutInBoundRect(rcContent);
    }
}

bool xListViewStyle::ShiftContentPos(int nOffset, int& nDataIndex, bool bCheckBound)
{
    if (bCheckBound && m_nContentHeight <= m_nPageHeight)
    {
        return false;
    }

    // 一次最多只能滚动1页
    if (nOffset > m_nPageHeight)
    {
        nOffset = m_nPageHeight;
    }
    else if (nOffset < (-m_nPageHeight))
    {
        nOffset = -m_nPageHeight;
    }

    int nContentPos = m_nContentPos + nOffset;

    if (bCheckBound)
    {
        int nMaxContentPos = GetMaxContentPos();

        if (nContentPos < 0)
        {
            nOffset = 0 - m_nContentPos;
            nContentPos = 0;
        }
        else if (nContentPos > nMaxContentPos)
        {
            nOffset = nMaxContentPos - m_nContentPos;
            nContentPos = nMaxContentPos;
        }
    }

    if (0 == nOffset)
    {
        return false;
    }

    // 不等高、分段暂时不支持
    if (0 == m_nItemHeight)
    {
        return false;
    }
#if IF_XLISTVIEW_SUPPORT_GROUP
    else if (m_nSectionCount > 0 && GetSectionShiftPos(nContentPos, nDataIndex, m_nItemOffset))
    {
        SetContentPos(nContentPos);
        return true;
    }
#endif

    int nItemTotalHeight = m_nItemHeight + m_nItemSpace;

    if (nItemTotalHeight <= 0)
    {
        return false;
    }

    nDataIndex = nContentPos / nItemTotalHeight;
    // Y轴坐标与nContentPos反向-(m_nContentPos - nDataIndex * nItemTotalHeight)
    m_nItemOffset = nDataIndex * nItemTotalHeight - nContentPos;
    SetContentPos(nContentPos);

    return true;
}

#if IF_XLISTVIEW_SUPPORT_GROUP
bool xListViewStyle::GetSectionShiftPos(int nContentPos, int& nDataIndex, int& nItemOffset)
{
    xListViewSectionPtr pSection = GetSectionByPos(nContentPos);

    if (NULL == pSection)
    {
        return false;
    }

    if (nContentPos == pSection->m_nSectionPos)
    {
        nDataIndex = pSection->m_nBeginIndex;
        nItemOffset = 0;

        return true;
    }

    int nTitleHeight = pSection->GetTotalTitleHeight();
    int nItemPos = pSection->m_nSectionPos + nTitleHeight;

    if (nContentPos > pSection->m_nSectionPos && nContentPos < nItemPos)
    {
        nDataIndex = pSection->m_nBeginIndex;
        nItemOffset = nItemPos - nContentPos;
        return true;
    }

    int nTotalItemHeight = m_nItemHeight + m_nItemSpace;
    int nIndex = pSection->m_nBeginIndex;

    for (; nIndex <= pSection->m_nEndIndex; ++nIndex)
    {
        if ((nItemPos + m_nItemHeight) >= nContentPos)
        {
            nDataIndex = nIndex;

            // - (nContentPos - nItemPos)
            nItemOffset = nItemPos - nContentPos;

            return true;
        }

        nItemPos += nTotalItemHeight;
    }

    return false;
}
#endif

bool xListViewStyle::IsItemShowFully(xListViewItem* pItem)
{
    if (NULL == pItem)
    {
        return false;
    }

    chRect rcContainer;

    if (!GetContainerGeometry(rcContainer))
    {
        return false;
    }

    chRect rcItem = pItem->geometry();

    if (rcItem.top < rcContainer.top || rcItem.bottom > rcContainer.bottom)
    {
        return false;
    }

    return true;
}
#endif

void xListViewStyle::SetScrollCtrlFixPos(bool bFixPos)
{
    if (bFixPos)
    {
        m_nFixPosFlag |= LIST_VIEW_FIX_POS_SCROLL_CTRL;
    }
    else
    {
        m_nFixPosFlag &= (~LIST_VIEW_FIX_POS_SCROLL_CTRL);
    }
}

void xListViewStyle::SetHintFixPos(bool bFixPos)
{
    if (bFixPos)
    {
        m_nFixPosFlag |= LIST_VIEW_FIX_POS_HINT;
    }
    else
    {
        m_nFixPosFlag &= (~LIST_VIEW_FIX_POS_HINT);
    }
}

int xListViewStyle::GetMaxContentPos() const
{
    int nMaxPos = m_nContentHeight - m_nPageHeight;

    if (nMaxPos < 0)
    {
        nMaxPos = 0;
    }

    return nMaxPos;
}

bool xListViewStyle::GetItemLayoutInfo(int nItemIndex, int nDataIndex,
                                       int& nLeftMargin, int& nTopMargin,
                                       int& nWidth, int& nHeight)
{
    if (NULL == m_pListView)
    {
        return false;
    }

    chRect rcContainer;
    chRect rcItems;

    if (!GetContainerGeometry(rcContainer) || !GetContentGeometry(rcItems))
    {
        return false;
    }

    nLeftMargin = rcItems.left - rcContainer.left;
    nTopMargin = rcItems.top - rcContainer.top;
    nWidth = rcItems.Width();

#if IF_XLISTVIEW_SUPPORT_SLIDE
    xListSlideView* pSlideView = m_pListView->GetListSlideView();
#endif
    int nPreItemIndex = nItemIndex - 1;
    xListViewItemPtr pPreItem = NULL;

    if (INVALID_ITEM_INDEX != nPreItemIndex)
    {
        pPreItem = m_pListView->GetItemByIndex(nPreItemIndex);
    }

    if (NULL != pPreItem)
    {
        nTopMargin = pPreItem->layout_marginTop;

        if (xView::wrap_content == pPreItem->layout_height)
        {
            nTopMargin += pPreItem->wrapFrameHeight();
        }
        else
        {
            nTopMargin += pPreItem->layout_height;
        }
    }
#if IF_XLISTVIEW_SUPPORT_SLIDE
    else if (NULL != pSlideView)
    {
        nTopMargin += m_nItemOffset;
    }
#endif

#if IF_XLISTVIEW_SUPPORT_GROUP
    if (LIST_VIEW_STYLE_ITEMS_STYLE_GROUP == m_nItemsStyle)
    {
        xListViewSection* pSection = GetSectionByIndex(nDataIndex);

        if (NULL != pSection && nDataIndex == pSection->m_nBeginIndex
                && pSection->m_nSectionPos >= m_nContentPos)
        {
            nTopMargin += pSection->m_nTitleHeight;

            if (0 != nItemIndex)
            {
                nTopMargin += pSection->m_nSectionSpace;
            }
        }
    }
#endif

    if (0 != nItemIndex)
    {
        nTopMargin += m_nItemSpace;
    }

    nHeight = m_nItemHeight;

#if IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
    // 如果是非固定长度，则向Adapter请求Geometry
    if (nHeight <= 0)
    {
        IListViewAdapterPtr pAdapter = m_pListView->GetAdapter();

        if (NULL != pAdapter && (!pAdapter->GetItemHeight(m_pListView, nDataIndex, nHeight)))
        {
            return false;
        }
    }
#endif

#if IF_XLISTVIEW_SUPPORT_SLIDE
    if (NULL != pSlideView && pSlideView->IsSliding())
    {
#if IF_XLIST_VIEW_SLIDE_DEBUG
        XWINDOW_INFO("Item Geometry:%d %d %d %d Content:%d %d %d %d",
                     nLeftMargin, nTopMargin, nLeftMargin + nWidth, nTopMargin + nHeight,
                     rcItems.left, rcItems.top, rcItems.right, rcItems.bottom);
#endif

        // 滑动时xListView已经布局好，可以直接跟rcContent进行比较
        return ((rcContainer.top + nTopMargin + nHeight) >= rcItems.top
                && (rcContainer.top + nTopMargin) < rcItems.bottom);
    }
#endif

    return ((rcContainer.top + nTopMargin + nHeight) <= (rcItems.top + m_nPageHeight));
}

bool xListViewStyle::GetContentGeometry(chRect& rcContent)
{
    if (NULL == m_pListView)
    {
        return false;
    }

    rcContent = m_pListView->contentGeometry();

#if IF_XLISTVIEW_SUPPORT_TITLE
    xListViewTitlePtr pTitle = m_pListView->GetTitle();

    if (NULL != pTitle)
    {
        int nItemTop = rcContent.top + pTitle->GetTitleHeight();

        nItemTop += m_nTitleSpace;

        rcContent.top = nItemTop;
    }
    else
    {
        rcContent.top += m_nItemTopMargin;
    }
#else
    rcContent.top += m_nItemTopMargin;
#endif

    rcContent.bottom = rcContent.top + m_nPageHeight;

    rcContent.left += m_nItemLeftMargin;
    rcContent.right -= m_nItemRightMargin;

    return true;
}

bool xListViewStyle::GetContentRect(chRect& rcContent)
{
    if (NULL == m_pListView)
    {
        return false;
    }

#if IF_XLISTVIEW_SUPPORT_SLIDE
    xListSlideView* pSlideView = m_pListView->GetListSlideView();

    if (NULL != pSlideView)
    {
        rcContent = pSlideView->contentRect();

        return true;
    }
#endif

    rcContent = m_pListView->contentRect();

    return true;
}

bool xListViewStyle::GetContainerGeometry(chRect& rcContainer)
{
    if (NULL == m_pListView)
    {
        return false;
    }

#if IF_XLISTVIEW_SUPPORT_SLIDE
    xListSlideView* pSlideView = m_pListView->GetListSlideView();

    if (NULL != pSlideView)
    {
        rcContainer = pSlideView->geometry();

        return true;
    }
#endif

    rcContainer = m_pListView->geometry();

    return true;
}

int xListViewStyle::GetDataIndexByPos(int nContentPos)
{
    if (NULL == m_pListView)
    {
        return INVALID_ITEM_INDEX;
    }

    IListViewAdapterPtr pAdapter = m_pListView->GetAdapter();

    if (NULL == pAdapter)
    {
        return INVALID_ITEM_INDEX;
    }

    // 有带段的列表
#if IF_XLISTVIEW_SUPPORT_GROUP
    xListViewSectionPtr pSection = GetSectionByPos(nContentPos);

    if (NULL != pSection)
    {
        int nSectionPos = pSection->m_nSectionPos + pSection->GetTotalTitleHeight();
        int nIndex = pSection->m_nBeginIndex;

        for (; nIndex <= pSection->m_nEndIndex; ++nIndex)
        {
            if (nSectionPos >= nContentPos)
            {
                return nIndex;
            }

            int nItemHeight = m_nItemHeight;

            if (0 == nItemHeight)
            {
                pAdapter->GetItemHeight(m_pListView, nIndex, nItemHeight);
            }

            nSectionPos = nSectionPos + nItemHeight + m_nItemSpace;
        }

        XWINDOW_ERR("GetDataIndexByPos(%d) can't find item Section:%d %d %d %d %d %d",
                    nContentPos, pSection->m_nSectionPos, pSection->m_nSectionHeight, pSection->m_nSectionSpace,
                    pSection->m_nTitleHeight, pSection->m_nBeginIndex, pSection->m_nEndIndex);

        return INVALID_ITEM_INDEX;
    }
#endif

    // 等高列表项
    if (0 != m_nItemHeight)
    {
        int nItemTotalHeight = m_nItemHeight + m_nItemSpace;

        if (nItemTotalHeight <= 0)
        {
            XWINDOW_ERR("GetDataIndexByPos(%d) can't find item ItemTotalHeight:%d",
                        nContentPos, nItemTotalHeight);

            return INVALID_ITEM_INDEX;
        }

        return (nContentPos / nItemTotalHeight);
    }

#if IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
    // 非等高列表项
    int nTotalDataCount = m_pListView->GetTotalDataCount();
    int nItemHeight = 0;
    int nSectionPos = 0;
    int nIndex = 0;

    for (; nIndex < nTotalDataCount; ++nIndex)
    {
        if (nSectionPos >= nContentPos)
        {
            return nIndex;
        }

        if (!pAdapter->GetItemHeight(m_pListView, nIndex, nItemHeight))
        {
            XWINDOW_ERR("GetDataIndexByPos(%d) can't find item GetItemHeight:%d %d",
                        nContentPos, nIndex, nItemHeight);

            return INVALID_ITEM_INDEX;
        }

        nSectionPos = nSectionPos + nItemHeight + m_nItemSpace;
    }

    XWINDOW_ERR("GetDataIndexByPos(%d) can't find item nTotalDataCount:%d", nContentPos, nTotalDataCount);
#else
    XWINDOW_ERR("GetDataIndexByPos(%d) not support disunion item height");
#endif

    return INVALID_ITEM_INDEX;
}

int xListViewStyle::GetContentPosByIndex(int nDataIndex)
{
    if (NULL == m_pListView)
    {
        return 0;
    }

    IListViewAdapterPtr pAdapter = m_pListView->GetAdapter();

    if (NULL == pAdapter)
    {
        return 0;
    }

    // 有带段的列表
#if IF_XLISTVIEW_SUPPORT_GROUP
    xListViewSectionPtr pSection = GetSectionByIndex(nDataIndex);

    if (NULL != pSection)
    {
        int nSectionPos = pSection->m_nSectionPos + pSection->GetTotalTitleHeight();
        int nIndex = pSection->m_nBeginIndex;

        for (; nIndex <= pSection->m_nEndIndex; ++nIndex)
        {
            if (nIndex == nDataIndex)
            {
                return nSectionPos;
            }

            int nItemHeight = m_nItemHeight;

#if IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
            if (0 == nItemHeight)
            {
                pAdapter->GetItemHeight(m_pListView, nIndex, nItemHeight);
            }
#endif

            nSectionPos = nSectionPos + nItemHeight + m_nItemSpace;
        }

        return nSectionPos;
    }
#endif

    // 等高列表项
    if (0 != m_nItemHeight)
    {
        int nItemTotalHeight = m_nItemHeight + m_nItemSpace;

        return nDataIndex * nItemTotalHeight;
    }

#if IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
    // 非等高列表项
    int nTotalDataCount = m_pListView->GetTotalDataCount();
    int nItemHeight = 0;
    int nSectionPos = 0;
    int nIndex = 0;

    for (; nIndex < nTotalDataCount; ++nIndex)
    {
        if (nIndex == nDataIndex)
        {
            return nSectionPos;
        }

        if (!pAdapter->GetItemHeight(m_pListView, nIndex, nItemHeight))
        {
            return INVALID_ITEM_INDEX;
        }

        nSectionPos = nSectionPos + nItemHeight + m_nItemSpace;
    }

    return nSectionPos;
#else
    return 0;
#endif
}

bool xListViewStyle::OnPrePage(bool bCycle/* = true*/)
{
    if (NULL == m_pListView || m_nContentHeight <= 0 || m_nPageHeight <= 0)
    {
        return false;
    }

    int nPageNo = GetCurPage();

    if (INVALID_FRAME_LIST_PAGE_NO == nPageNo)
    {
        return false;
    }

    // 有前一页数据
    if (0 != nPageNo)
    {
        SetContentPos(m_nContentPos - m_nPageHeight);

        int nIndex = GetDataIndexByPos(m_nContentPos);

        m_pListView->LoadListItem(nIndex);
        return true;
    }

    IListViewAdapterPtr pAdapter = m_pListView->GetAdapter();

    // 处理完第一页翻页
    if (NULL != pAdapter && pAdapter->OnFirstItemPrevious())
    {
        return true;
    }

    if (!bCycle)
    {
        return false;
    }

    // 只有一页，不需要重新加载数据
    if (m_nContentHeight <= m_nPageHeight)
    {
        return false;
    }

    int nContentPos = m_nContentHeight - (m_nContentHeight % m_nPageHeight);

    // 循环到最后一页
    SetContentPos(nContentPos);

    int nIndex = GetDataIndexByPos(m_nContentPos);

    m_pListView->LoadListItem(nIndex);

    return true;
}

bool xListViewStyle::OnNextPage(bool bCycle/* = true*/)
{
    if (NULL == m_pListView || m_nContentHeight <= 0 || m_nPageHeight <= 0)
    {
        return false;
    }

    int nPageNo = GetCurPage();

    if (INVALID_FRAME_LIST_PAGE_NO == nPageNo)
    {
        return false;
    }

    // 只有一页，不需要重新加载数据
    if (0 == nPageNo && m_nContentHeight <= m_nPageHeight)
    {
        return false;
    }

    int nContentPos = m_nContentPos + m_nPageHeight;

    // 有下一页数据
    if (nContentPos < m_nContentHeight)
    {
        int nIndex = GetDataIndexByPos(nContentPos);

        SetContentPos(nContentPos);
        m_pListView->LoadListItem(nIndex);

        return true;
    }

    IListViewAdapterPtr pAdapter = m_pListView->GetAdapter();

    // 处理完最后一页翻页
    if (NULL != pAdapter && pAdapter->OnLastItemNext())
    {
        return true;
    }

    if (!bCycle)
    {
        return false;
    }

    // 循环到第一页
    SetContentPos(0);
    m_pListView->LoadListItem(0);

    return true;
}

int xListViewStyle::GetCurPage()
{
    if (m_nContentHeight <= 0 || m_nPageHeight <= 0)
    {
        return INVALID_FRAME_LIST_PAGE_NO;
    }

    int nPageNo = (m_nContentPos + m_nPageHeight) / m_nPageHeight - 1;

    return nPageNo;
}

bool xListViewStyle::OnPreFullPage(bool bCycle/* = true*/)
{
    if (NULL == m_pListView || m_nContentHeight <= 0 || m_nPageHeight <= 0)
    {
        return false;
    }

    // 只有一页，不需要重新加载数据
    if (m_nContentHeight <= m_nPageHeight)
    {
        return false;
    }

    // 已经是第一页，并且不循环切换
    if (!bCycle && m_nContentPos <= 0)
    {
        return false;
    }

    int nFocusDataIndex = m_pListView->GetFocusItemDataIndex();
    int nFocusIndex = m_pListView->GetFocusItemIndex();
    int nContentPos = m_nContentPos - m_nPageHeight;

    if (nContentPos < 0 && m_nContentPos > 0)
    {
        nContentPos = 0;
    }

    // 有前一页数据
    if (nContentPos >= 0)
    {
        SetContentPos(nContentPos);

        int nIndex = 0;

        if (nContentPos > 0)
        {
            nIndex = GetDataIndexByPos(nContentPos);
        }

        m_pListView->LoadListItem(nIndex);

        FocusPriorItem(nFocusDataIndex, nFocusIndex);

        return true;
    }

    // 循环到最后一页
    SetContentPos(GetMaxContentPos());

    int nIndex = GetDataIndexByPos(m_nContentPos);

    m_pListView->LoadListItem(nIndex);

    FocusPriorItem(nFocusDataIndex, nFocusIndex);

    return true;
}

bool xListViewStyle::OnNextFullPage(bool bCycle/* = true*/)
{
    if (NULL == m_pListView || m_nContentHeight <= 0 || m_nPageHeight <= 0)
    {
        return false;
    }

    // 只有一页，不需要重新加载数据
    if (m_nContentHeight <= m_nPageHeight)
    {
        return false;
    }

    int nFocusDataIndex = m_pListView->GetFocusItemDataIndex();
    int nFocusIndex = m_pListView->GetFocusItemIndex();
    int nContentPos = m_nContentPos + m_nPageHeight;
    int nMaxPos = GetMaxContentPos();

    if (nContentPos > nMaxPos && m_nContentPos < nMaxPos)
    {
        nContentPos = nMaxPos;
    }

    // 有下一页数据
    if (nContentPos <= nMaxPos)
    {
        int nIndex = GetDataIndexByPos(nContentPos);

        SetContentPos(nContentPos);
        m_pListView->LoadListItem(nIndex);

        FocusPriorItem(nFocusDataIndex, nFocusIndex);

        return true;
    }

    if (!bCycle)
    {
        return false;
    }

    // 循环到第一页
    SetContentPos(0);
    m_pListView->LoadListItem(0);

    FocusPriorItem(nFocusDataIndex, nFocusIndex);

    return true;
}

void xListViewStyle::FocusPriorItem(int nDataIndex, int nIndex)
{
    if (NULL == m_pListView)
    {
        return;
    }

    xListViewItemPtr pItem = m_pListView->GetItemByDataIndex(nDataIndex);

    if (NULL != pItem)
    {
        m_pListView->FocusItemByDataIndex(nDataIndex);

        return;
    }

    pItem = m_pListView->GetItemByIndex(nIndex);

    if (NULL != pItem)
    {
        m_pListView->FocusItemByIndex(nIndex);

        return;
    }

    m_pListView->FocusLastItem();
}

bool xListViewStyle::OnPreItem(bool bScrollByPage, bool bCycle/* = true*/)
{
    if (NULL == m_pListView || m_nContentHeight <= 0 || m_nPageHeight <= 0)
    {
        return false;
    }

    xListViewItemPtr pFocusItem = m_pListView->GetFocusItem();
    int nFocusDataIndex = INVALID_ITEM_INDEX;

    if (NULL != pFocusItem)
    {
        nFocusDataIndex = pFocusItem->GetDataIndex();
    }

#if IF_XLISTVIEW_SUPPORT_SLIDE
    xListSlideView* pSlideView = m_pListView->GetListSlideView();

    if (INVALID_ITEM_INDEX == nFocusDataIndex && NULL != pSlideView)
    {
        nFocusDataIndex = pSlideView->GetFocusDataIndex();
    }
#endif

    if (INVALID_ITEM_INDEX == nFocusDataIndex)
    {
        return false;
    }

    IListViewAdapterPtr pAdapter = m_pListView->GetAdapter();
    int nPreDataIndex = nFocusDataIndex - 1;

#if IF_XLISTVIEW_CUSTOM_SWITCH_FOCUS
    if (NULL != pAdapter)
    {
        int nRealDataIndex = nPreDataIndex;

        if (nRealDataIndex < 0)
        {
            nRealDataIndex = m_pListView->GetTotalDataCount() + nRealDataIndex;
        }

        int nRealFocusDataIndex = pAdapter->GetFocusDataIndex(nRealDataIndex, false);

        if (nRealDataIndex != nRealFocusDataIndex)
        {
            nPreDataIndex += (nRealFocusDataIndex - nRealDataIndex);
        }
    }
#endif

    int nPreItemIndex = m_pListView->GetFocusItemIndex() + (nPreDataIndex - nFocusDataIndex);

    // 非第一项的前一项时，往前移一项
    if (nPreItemIndex >= 0)
    {
#if IF_XLISTVIEW_SUPPORT_GROUP
        xListViewSectionPtr pCurSection = GetSectionByIndex(nPreDataIndex);

        // 当前焦点在第一行，且Title没有显示
        if (NULL != pCurSection && nPreDataIndex == pCurSection->m_nBeginIndex
                && m_nContentPos > pCurSection->m_nSectionPos)
        {
            SetContentPos(m_nContentPos - pCurSection->GetTotalTitleHeight());

            m_pListView->LoadListItem(nPreDataIndex);
            m_pListView->FocusItemByIndex(0);

            return true;
        }
#endif

        m_pListView->FocusItemByIndex(nPreItemIndex);
        return true;
    }

    // 已经是数据的第一项
    if (nPreDataIndex < 0)
    {
        if (NULL != pAdapter && pAdapter->OnFirstItemPrevious())
        {
            return true;
        }

        if (!bCycle)
        {
            return false;
        }

        bool bReload = false;

        // 有多页数据，重新加载最后一页
        if (m_nContentHeight > m_nPageHeight)
        {
            SetContentPos(GetMaxContentPos());

            int nIndex = GetDataIndexByPos(m_nContentPos);

            m_pListView->LoadListItem(nIndex);

            bReload = true;
        }

        int nLastDataIndex = m_pListView->GetTotalDataCount() + nPreDataIndex;

        m_pListView->FocusItemByDataIndex(nLastDataIndex);

        if (bReload)
        {
            m_pListView->update(true);
        }

        return true;
    }

    if (bScrollByPage)
    {
        OnPrePage();

        m_pListView->FocusItemByDataIndex(nPreDataIndex);

        return true;
    }

    // 前移N项
    int nContentPos = m_nContentPos;

    for (int i = nFocusDataIndex - 1; i >= nPreDataIndex; --i)
    {
        if (NULL != m_pListView->GetItemByDataIndex(i))
        {
            continue;
        }

        if (!GetPreItemContentPos(i, nContentPos))
        {
            return false;
        }
    }

#if XLIST_VIEW_DEBUG
    XWINDOW_INFO("OnPreItem CurrentPos:%d FocusDatIndex:%d PreContentPos:%d PreDataIndex:%d",
                 m_nContentPos, nFocusDataIndex, m_nContentPos, nPreDataIndex);
#endif

    SetContentPos(nContentPos);

    m_pListView->LoadListItem(nPreDataIndex);
    m_pListView->FocusItemByIndex(0);

    return true;
}

bool xListViewStyle::GetPreItemContentPos(int nPreDataIndex, int& nContentPos)
{
    int nPreItemHeight = m_nItemHeight;

#if IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
    if (0 == nPreItemHeight)
    {
        if (NULL == m_pListView)
        {
            return false;
        }

        IListViewAdapterPtr pAdapter = m_pListView->GetAdapter();

        if (NULL == pAdapter || !pAdapter->GetItemHeight(m_pListView, nPreDataIndex, nPreItemHeight))
        {
            return false;
        }
    }
#endif

    nContentPos = nContentPos - nPreItemHeight - m_nItemSpace;

#if IF_XLISTVIEW_SUPPORT_GROUP
    xListViewSectionPtr pPreSection = GetSectionByIndex(nPreDataIndex);

    // 段第一行，默认显示段头
    if (NULL != pPreSection && nPreDataIndex == pPreSection->m_nBeginIndex)
    {
        nContentPos -= pPreSection->GetTotalTitleHeight();
    }
#endif

    return true;
}

bool xListViewStyle::OnNextItem(bool bScrollByPage, bool bCycle/* = true*/)
{
    if (NULL == m_pListView || m_nContentHeight <= 0 || m_nPageHeight <= 0)
    {
        return false;
    }

    xListViewItemPtr pFocusItem = m_pListView->GetFocusItem();
    int nFocusDataIndex = INVALID_ITEM_INDEX;

    if (NULL != pFocusItem)
    {
        nFocusDataIndex = pFocusItem->GetDataIndex();
    }

#if IF_XLISTVIEW_SUPPORT_SLIDE
    xListSlideView* pSlideView = m_pListView->GetListSlideView();

    if (INVALID_ITEM_INDEX == nFocusDataIndex && NULL != pSlideView)
    {
        nFocusDataIndex = pSlideView->GetFocusDataIndex();
    }
#endif

    if (INVALID_ITEM_INDEX == nFocusDataIndex)
    {
        return false;
    }

    IListViewAdapterPtr pAdapter = m_pListView->GetAdapter();
    int nTotalDataCount = m_pListView->GetTotalDataCount();
    int nNextDataIndex = nFocusDataIndex + 1;

#if IF_XLISTVIEW_CUSTOM_SWITCH_FOCUS
    if (NULL != pAdapter)
    {
        int nRealDataIndex = nNextDataIndex;

        if (nRealDataIndex >= nTotalDataCount)
        {
            nRealDataIndex = 0;
        }

        int nRealFocusDataIndex = pAdapter->GetFocusDataIndex(nRealDataIndex, true);

        if (nRealDataIndex != nRealFocusDataIndex)
        {
            nNextDataIndex += (nRealFocusDataIndex - nRealDataIndex);
        }
    }
#endif

    int nNextItemIndex = m_pListView->GetFocusItemIndex() + (nNextDataIndex - nFocusDataIndex);
    int nItemCount = m_pListView->GetTotalItemCount();

    // 非最后项的后一项时，往后移一项
    if (nNextItemIndex < nItemCount)
    {
        m_pListView->FocusItemByIndex(nNextItemIndex);
        return true;
    }

    // 已经是数据的最后项
    if (nNextDataIndex >= nTotalDataCount)
    {
        if (NULL != pAdapter && pAdapter->OnLastItemNext())
        {
            return true;
        }

        if (!bCycle)
        {
            return false;
        }

        // 有多页数据，重新加载第一页
        if (m_nContentHeight > m_nPageHeight)
        {
            SetContentPos(0);
            m_pListView->LoadListItem(0);
        }

        m_pListView->FocusItemByIndex(nNextDataIndex - nTotalDataCount);

        return true;
    }

    if (bScrollByPage)
    {
        OnNextPage();

        m_pListView->FocusItemByDataIndex(nNextDataIndex);

        return true;
    }

    int nItemHeight = m_nItemHeight;
#if IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
    if (NULL != pAdapter && 0 == nItemHeight)
    {
        pAdapter->GetItemHeight(m_pListView, nNextDataIndex, nItemHeight);
    }
#endif

    // 后移N项
    int nMaxContentPos = GetContentPosByIndex(nNextDataIndex) + nItemHeight + m_nItemSpace - m_nPageHeight;
    int nContentPos = m_nContentPos;

    for (int i = nFocusDataIndex; i < nNextDataIndex; ++i)
    {
        int nTmpNextIndex = i + 1;

        if (NULL != m_pListView->GetItemByDataIndex(nTmpNextIndex))
        {
            continue;
        }

        if (!GetNextItemContentPos(i, nTmpNextIndex, nContentPos))
        {
            return false;
        }

        if (nContentPos >= nMaxContentPos)
        {
            break;
        }
    }

#if XLIST_VIEW_DEBUG
    XWINDOW_INFO("OnNextItem CurrentPos:%d FocusDatIndex:%d", m_nContentPos, nFocusDataIndex);
#endif

    SetContentPos(nContentPos);

    // 定位第一项Index
    int nFirstDataIndex = GetDataIndexByPos(m_nContentPos);

#if XLIST_VIEW_DEBUG
    XWINDOW_INFO("OnNextItem NextBeginIndex:%d NewContentPos:%d NewDataIndex:%d",
                 nFirstDataIndex, m_nContentPos, nNextDataIndex);
#endif

    m_pListView->LoadListItem(nFirstDataIndex);
    m_pListView->FocusLastItem();

    return true;
}

bool xListViewStyle::GetNextItemContentPos(int nDataIndex, int nNextDataIndex, int& nContentPos)
{
    int nNextItemHeight = m_nItemHeight;

#if IF_XLISTVIEW_DISUNION_ITEM_HEIGHT
    if (0 == nNextItemHeight)
    {
        if (NULL == m_pListView)
        {
            return false;
        }

        IListViewAdapterPtr pAdapter = m_pListView->GetAdapter();

        if (NULL == pAdapter || !pAdapter->GetItemHeight(m_pListView, nDataIndex, nNextItemHeight))
        {
            return false;
        }
    }
#endif

    nContentPos = nContentPos + nNextItemHeight + m_nItemSpace;

#if IF_XLISTVIEW_SUPPORT_GROUP
    xListViewSectionPtr pNextSection = GetSectionByIndex(nNextDataIndex);
    xListViewSectionPtr pCurSection = GetSectionByIndex(nDataIndex);

    // 有跨越段，则加上新段头
    if (pNextSection != pCurSection && NULL != pNextSection && pNextSection->IsHadSectionTitle())
    {
        nContentPos += pNextSection->GetTotalTitleHeight();
    }
#endif

    return true;
}

#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
void xListViewStyle::OnItemFocus(xListViewItem* pItem)
{
#if !IF_XLISTVIEW_SCROLL_BAR_DENOTE_POSTION
    if (NULL == m_pListView || NULL == pItem)
    {
        return;
    }

    xScrollCtrlPtr pScrollCtrl = m_pListView->GetScrollCtrl();

    if (NULL == pScrollCtrl || !pScrollCtrl->isVisible())
    {
        return;
    }

    pScrollCtrl->SetScrollPos(pItem->GetDataIndex());
#endif
}
#endif

#if IF_XLISTVIEW_SUPPORT_TITLE
void xListViewStyle::PaintTitle(xPainter& painter)
{
    DrawTitleSpaceLine(painter);
}
#endif

void xListViewStyle::PaintContent(xPainter& painter)
{
#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
    bool bDrawItemLine = false;

    if (LINE_TYPE_NONE != m_nSpaceLineStyle)
    {
        painter.setLineType(m_nSpaceLineStyle);
        painter.setLineWidth(m_nItemSpace);
        bDrawItemLine = true;
    }
#endif

    // 画组样式
#if IF_XLISTVIEW_SUPPORT_GROUP
    int nContentEndPos = m_nContentPos + m_nPageHeight;
    int nSectionCount = m_vecSection.size();
    chRect rcContent;

    if (GetContentGeometry(rcContent) && NULL != m_pSectionStyle)
    {
        for (int i = 0; i < nSectionCount; ++i)
        {
            xListViewSectionPtr pSection = m_vecSection[i];

            if (NULL == pSection)
            {
                continue;
            }

            if ((pSection->m_nSectionPos + pSection->m_nSectionHeight + pSection->GetTotalTitleHeight()) <= m_nContentPos)
            {
                continue;
            }

            if (pSection->m_nSectionPos > nContentEndPos)
            {
                break;
            }

            xListViewSectionPtr pNextSection = NULL;
            int nNextIndex = i + 1;

            if (nNextIndex < nSectionCount)
            {
                pNextSection = m_vecSection[nNextIndex];
            }

            m_pSectionStyle->PaintSection(painter, pSection, pNextSection, this, rcContent);
        }
    }

#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
    if (bDrawItemLine)
    {
        bDrawItemLine = (nSectionCount <= 0);
    }
#endif
#endif

    // 画item分隔线
#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
    if (bDrawItemLine)
    {
        DrawItemSpaceLine(painter);
    }
#endif

#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
    DrawFocusItemBG(painter);
#endif
}

#if IF_XLISTVIEW_FOCUS_INVERT_EFFECT
void xListViewStyle::PaintMask(xPainter& painter)
{
    if (NULL == m_pListView)
    {
        return;
    }

    xListViewItemPtr pFocusItem = m_pListView->GetFocusItem();
    chRect rcFocus;

    if (NULL != pFocusItem)
    {
#if IF_XLISTVIEW_SUPPORT_ITEM_CUSTOM_DECIDE_FOCUS_BG
        if (!pFocusItem->IsShowFocusBG())
        {
            return;
        }
#endif

        rcFocus = pFocusItem->clientGeometry();
    }
#if IF_XLISTVIEW_NOTE_SHOW_IN_ITEM
    else
    {
        if (!m_pListView->hasFocus())
        {
            return;
        }

        xTextView* pHintView = m_pListView->GetHintView();

        if (NULL == pHintView)
        {
            return;
        }

        rcFocus = pHintView->clientGeometry();
    }
#else
    else
    {
        return;
    }
#endif

    chRect rcListView = m_pListView->clientGeometry();

    rcFocus.OffsetRect(-rcListView.left, -rcListView.top);

    painter.invertRect(rcFocus);
}
#endif

#if IF_XLISTVIEW_SUPPORT_TITLE
void xListViewStyle::DrawTitleSpaceLine(xPainter& painter)
{
    if (NULL == m_pListView || LIST_TITLE_LINE_TYPE_NONE == m_nTitleLineType)
    {
        return;
    }

    xListViewTitlePtr pTitle = m_pListView->GetTitle();

    if (NULL == pTitle)
    {
        return;
    }

    if (LIST_TITLE_LINE_TYPE_EXIST_WITH_ITEM == m_nTitleLineType && m_pListView->GetTotalItemCount() <= 0)
    {
        return;
    }

    int nLineWidth = 0;
    bool bLineInSpace = false;

    if (LIST_TITLE_LINE_POS_TYPE_IN_SPACE == (LIST_TITLE_LINE_POS_TYPE_IN_SPACE & m_nTitleLinePosType))
    {
        nLineWidth = m_nTitleSpace;
        bLineInSpace = true;
    }
    else
    {
        nLineWidth = m_nItemSpace;
    }

    if (nLineWidth <= 0)
    {
        return;
    }

    painter.setLineType(m_nSpaceLineStyle);
    painter.setLineWidth(nLineWidth);

    chRect rcListView = m_pListView->geometry();
    chRect rcTitle = pTitle->geometry();
    int nY = rcTitle.bottom - rcListView.top;
    int nLeft = 0;
    int nRight = 0;

    if (LIST_TITLE_LINE_POS_TYPE_BY_TITLE == (LIST_TITLE_LINE_POS_TYPE_BY_TITLE & m_nTitleLinePosType))
    {
        nLeft = rcTitle.left - rcListView.left;
        nRight = rcTitle.right - rcListView.left;
    }
    else
    {
        chRect rcContent = m_pListView->contentRect();

        nLeft = rcContent.left + m_nItemLeftMargin;
        nRight = rcContent.right - m_nItemRightMargin;
    }

    if (!bLineInSpace)
    {
        if (LIST_TITLE_LINE_POS_TYPE_IN_TITLE == (LIST_TITLE_LINE_POS_TYPE_IN_TITLE & m_nTitleLinePosType))
        {
            nY -= nLineWidth;
        }
        else if (LIST_TITLE_LINE_POS_TYPE_IN_ITEM == (LIST_TITLE_LINE_POS_TYPE_IN_ITEM & m_nTitleLinePosType))
        {
            nY += m_nTitleSpace;
        }
    }

    painter.drawLineX(nY, nLeft, nRight, m_clrSpaceLine);
}
#endif

#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
void xListViewStyle::DrawItemSpaceLine(xPainter& painter)
{
    if (NULL == m_pListView)
    {
        return;
    }

    int nTotalItemCount = m_pListView->GetTotalItemCount();

    if (nTotalItemCount <= 0)
    {
        return;
    }

    int nLastIndex = m_nPageItemCount - 1;
    chRect rcListView;
    chRect rcContent;

    if (!GetContainerGeometry(rcListView) || !GetContentRect(rcContent))
    {
        return;
    }

    int nLeft = rcContent.left + m_nItemLeftMargin;
    int nRight = rcContent.right - m_nItemRightMargin;

    for (int i = 0; i < nTotalItemCount; ++i)
    {
        xListViewItemPtr pItem = m_pListView->GetItemByIndex(i);

        if (NULL == pItem)
        {
            continue;
        }

        if (m_bSpaceLineIgnoreEnd && nLastIndex == i)
        {
            break;
        }

        chRect rcItem = pItem->geometry();

        painter.drawLineX(rcItem.bottom - rcListView.top, nLeft, nRight, m_clrSpaceLine);
    }
}
#endif

#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
void xListViewStyle::DrawFocusItemBG(xPainter& painter)
{
    if (NULL == m_pItemBGPainter || NULL == m_pListView
            || (m_pItemBGPainter == s_pDefaultItemBGPainter && !GetItemStyle().empty()))
    {
        return;
    }

    int nFocusItemIndex = m_pListView->GetFocusItemIndex();
    xListViewItemPtr pFocusItem = m_pListView->GetFocusItem();

    if (NULL != pFocusItem
#if IF_XLISTVIEW_SUPPORT_ITEM_CUSTOM_DECIDE_FOCUS_BG
            && pFocusItem->IsShowFocusBG()
#endif
       )
    {
        chRect rcItem = pFocusItem->m_rectLayout;
        chRect rcListView;

        GetContainerGeometry(rcListView);

        rcItem.OffsetRect(-rcListView.left, -rcListView.top);

        if (0 != m_nItemSpace)
        {
            rcListView.top -= m_nItemSpace;
            rcListView.bottom += m_nItemSpace;
        }

        m_pItemBGPainter->DrawItemBG(painter, this, pFocusItem, rcItem, nFocusItemIndex);
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
xListViewStyleFactory::xListViewStyleFactory()
{

}

xListViewStyleFactory::~xListViewStyleFactory()
{
    m_mapListStyleFun.clear();
}

xListViewStyleFactory& xListViewStyleFactory::GetCurrentStyleFactory()
{
#if IF_MULTI_XKERNEL_ENABLE
    MULTI_KERNEL_INSTANCE(xListViewStyleFactory)
#else
    SINGLE_KERNEL_INSTANCE(xListViewStyleFactory)
#endif
}

void xListViewStyleFactory::RegisterListStyle(int nStyleType, FUN_CREATE_LIST_STYLE pFunCreateListStyle)
{
    m_mapListStyleFun[nStyleType] = pFunCreateListStyle;
}

xListViewStyle* xListViewStyleFactory::CreateListStyle(int nStyleType, xListView* pListView)
{
    if (LIST_VIEW_STYLE_TYPE_DEFAULT == nStyleType)
    {
        return xListViewStyle::CreateListViewStyle(pListView);
    }

    MAP_CREATE_LIST_STYLE_FUN::iterator iter = m_mapListStyleFun.find(nStyleType);
    FUN_CREATE_LIST_STYLE pFunCreateStyle = NULL;

    if (iter != m_mapListStyleFun.end())
    {
        pFunCreateStyle = iter->second;
    }

    if (NULL != pFunCreateStyle)
    {
        return (*pFunCreateStyle)(pListView);
    }

    return NULL;
}

#if IF_XLISTVIEW_SUPPORT_GROUP
void xListViewStyleFactory::RegisterSectionStyle(int nStyle, FUN_CREATE_SECTION_STYLE pFunCreateSectionStyle)
{
    m_mapSectionStyleFun[nStyle] = pFunCreateSectionStyle;
}

xListViewSectionStyle* xListViewStyleFactory::CreateSectionStyle(int nStyle)
{
    if (LIST_VIEW_STYLE_TYPE_DEFAULT == nStyle)
    {
        xListViewSectionStyle* pStyle = new xListViewSectionStyle;

        return pStyle;
    }

    MAP_CREATE_SECTION_STYLE_FUN::iterator iter = m_mapSectionStyleFun.find(nStyle);
    FUN_CREATE_SECTION_STYLE pFunCreateStyle = NULL;

    if (iter != m_mapSectionStyleFun.end())
    {
        pFunCreateStyle = iter->second;
    }

    if (NULL != pFunCreateStyle)
    {
        return (*pFunCreateStyle)();
    }

    return NULL;
}
#endif
