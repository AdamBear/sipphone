#include "ctagtitle.h"
#include "baseuicommon.h"

namespace
{
const char * TAG_TITLE_ATTR_TEXT_COLOR_NORMAL = "android:normalTagTitleTextColor";//字体正常颜色
const char * TAG_TITLE_ATTR_TEXT_COLOR_SELECT = "android:selectTagTitleTextColor";//选中字体颜色
const char * TAG_TITLE_ATTR_TEXT_BG_NORMAL = "android:normalTagTitleTextBG";//正常字体背景
const char * TAG_TITLE_ATTR_TEXT_BG_SELECT = "android:selectTagTitleTextBG";//选中字体背景
const char * TAG_TITLE_ATTR_CUT_LINE_LEFT = "android:TagTitleCutLineLeft";//分割线左边距
const char * TAG_TITLE_ATTR_CUT_LINE_TOP = "android:TagTitleCutLineTop";//分割线上边距
const char * TAG_TITLE_ATTR_CUT_LINE_WIDTH = "android:TagTitleCutLineWidth";//分割线宽度
const char * TAG_TITLE_ATTR_CUT_LINE_HEIGHT = "android:TagTitleCutLineHeight";//分割线高度
const char * TAG_TITLE_ATTR_CUT_LINE_SPACE = "android:TagTitleCutLineSpace";//分割线之间距离
const char * TAG_TITLE_ATTR_CUT_LINE_BG = "android:TagTitleCutLineBG";//分割线背景
#define INVALID_TAG_INDEX -1
const char * const kszTagTitleID = "TagText";
}


////////////////////////////////////////////////////////////////////////
EXPORT_VIEW_HANDLER_CLASS(CTagTitleHandle);
CTagTitleHandle::CTagTitleHandle()
{
    m_nType = VIEW_HANDLE_TYPE_TAG_TITLE;

    ResetCutLineInfo();
}

CTagTitleHandle::~CTagTitleHandle()
{

}

CTagTitleHandle* CTagTitleHandle::GetTagTitleHandle(xViewHandler* pHandle)
{
    if (NULL == pHandle || VIEW_HANDLE_TYPE_TAG_TITLE != pHandle->m_nType)
    {
        return NULL;
    }

    return (CTagTitleHandle*)pHandle;
}

void CTagTitleHandle::loadAttributes(xml_node & node)
{
    PopupAndroidBrush(node, TAG_TITLE_ATTR_TEXT_BG_NORMAL, m_bruNormalBG);
    PopupAndroidBrush(node, TAG_TITLE_ATTR_TEXT_BG_SELECT, m_bruSelectBG);

    PopupAndroidColor(node, TAG_TITLE_ATTR_TEXT_COLOR_NORMAL, m_clrNormalText);
    PopupAndroidColor(node, TAG_TITLE_ATTR_TEXT_COLOR_SELECT, m_clrSelectText);

    PopupAndroidInt(node, TAG_TITLE_ATTR_CUT_LINE_LEFT, m_stCutLineInfo.m_nCutLineLeft);
    PopupAndroidInt(node, TAG_TITLE_ATTR_CUT_LINE_TOP, m_stCutLineInfo.m_nCutLineTop);
    PopupAndroidInt(node, TAG_TITLE_ATTR_CUT_LINE_WIDTH, m_stCutLineInfo.m_nCutWidth);
    PopupAndroidInt(node, TAG_TITLE_ATTR_CUT_LINE_HEIGHT, m_stCutLineInfo.m_nCutLineHeight);
    PopupAndroidInt(node, TAG_TITLE_ATTR_CUT_LINE_SPACE, m_stCutLineInfo.m_nCutLineSpace);

    PopupAndroidBrush(node, TAG_TITLE_ATTR_CUT_LINE_BG, m_stCutLineInfo.m_bruCutlineBG);
}

const xColor& CTagTitleHandle::GetFontColor(bool bNormal) const
{
    if (bNormal)
    {
        return m_clrNormalText;
    }

    return m_clrSelectText;
}

const xBrush& CTagTitleHandle::GetTextFontBG(bool bNormal) const
{
    if (bNormal)
    {
        return m_bruNormalBG;
    }

    return m_bruSelectBG;
}

const ST_CUTLINE_INFO& CTagTitleHandle::GetCutLineInfo() const
{
    return m_stCutLineInfo;
}

void CTagTitleHandle::ResetCutLineInfo()
{
    m_stCutLineInfo.m_nCutLineLeft = 0;
    m_stCutLineInfo.m_nCutLineTop = 0;
    m_stCutLineInfo.m_nCutWidth = 0;
    m_stCutLineInfo.m_nCutLineHeight = 0;
    m_stCutLineInfo.m_nCutLineSpace = 0;
}

/////////////////////////////////////////////////////////////////
EXPORT_VIEW_CLASS(CTagTitle);
CTagTitle::CTagTitle()
    : m_pTagTextPageInfo(NULL)
    , m_pBtnLeftArrow(NULL)
    , m_pBtnRightArrow(NULL)
    , m_nCurrentTagIndex(INVALID_TAG_INDEX)
{

}

CTagTitle::~CTagTitle()
{

}

void CTagTitle::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);

    m_pBtnLeftArrow = static_cast<xButton*>(getViewById("btnLeftArrow"));
    m_pBtnRightArrow = static_cast<xButton*>(getViewById("btnRightArrow"));

    m_vecTagTextView.clear();
    for (int i = 1; ; ++i)
    {
        xTextView* pTextTag = NULL;
        yl::string strTatTitleTextId = commonAPI_FormatString("%s%d", kszTagTitleID, i);
        pTextTag = static_cast<xTextView*>(getViewById(strTatTitleTextId));
        if (NULL != pTextTag)
        {
            m_vecTagTextView.push_back(pTextTag);
        }
        else
        {
            break;
        }
    }

    m_pTagTextPageInfo = static_cast<xTextView*>(getViewById("TagPageInfo"));
}

bool CTagTitle::onPaintContent(xPainter& p)
{
    bool bResult = xFrameLayout::onPaintContent(p);

#if IS_COLOR
    PaintCutLine(p);
#endif
    return bResult;
}

#if IS_COLOR
void CTagTitle::PaintCutLine(xPainter& p)
{
    CTagTitleHandle* pHandle = CTagTitleHandle::GetTagTitleHandle(m_pHandler);
    if (NULL == pHandle || m_vecTitle.size() <= 1)
    {
        return;
    }

    ST_CUTLINE_INFO stCutLineInfo = pHandle->GetCutLineInfo();
    chRect rcCutLine(stCutLineInfo.m_nCutLineLeft
                     , stCutLineInfo.m_nCutLineTop
                     , stCutLineInfo.m_nCutLineLeft + stCutLineInfo.m_nCutWidth
                     , stCutLineInfo.m_nCutLineHeight);

    bool bColor = stCutLineInfo.m_bruCutlineBG.isColor();

    for (int i = 0; i <= m_vecTagTextView.size(); ++ i)
    {
        if (bColor)
        {
#if IF_PIXEL_ALPHA
            p.blendRect(rcCutLine, stCutLineInfo.m_bruCutlineBG.GetColor());
#else
            p.fillRect(rcCutLine, stCutLineInfo.m_bruCutlineBG.GetColor());
#endif
        }
        else
        {
            p.drawItemPixmap(rcCutLine, stCutLineInfo.m_bruCutlineBG.pixmap());
        }

        rcCutLine.OffsetRect(stCutLineInfo.m_nCutLineSpace, 0);
    }
}
#endif

void CTagTitle::SetTagList(VEC_STRING& vecTitleList)
{
    m_vecTitle.clear();

    m_vecTitle = vecTitleList;
}

void CTagTitle::SetCurrenTagIndex(const yl::string& strTag)
{
    int nTagIndex = GetIndex(strTag);

    SetCurrenTagIndex(nTagIndex);
}

void CTagTitle::SetCurrenTagIndex(int nIndex)
{
    int nTitleCount = GetTagCount();
    if (INVALID_TAG_INDEX == nIndex || nTitleCount <= 0)
    {
        return;
    }

    if (nIndex >= nTitleCount)
    {
        m_nCurrentTagIndex = nTitleCount - 1;
    }
    else
    {
        m_nCurrentTagIndex = nIndex;
    }

    UpdateTitleAttribute();
}

yl::string CTagTitle::GetCurrentTag()
{
    return GetString(m_nCurrentTagIndex);
}

int CTagTitle::GetCurrentTagIndex()
{
    return m_nCurrentTagIndex;
}

int CTagTitle::GetTagCount()
{
    return m_vecTitle.size();
}

void CTagTitle::UpdateTitleAttribute()
{
    UpdateTagText();

    UpdateArrow();

#if IS_COLOR
    UpdatePageInfo();
#endif

    update(true);
}

void CTagTitle::UpdateTagText()
{
    int nPageTagTextShow = m_vecTagTextView.size();
    if (nPageTagTextShow <= 0 || m_nCurrentTagIndex < 0)
    {
        return;
    }
    int nPageTagTextCount = m_vecTitle.size();
    if (m_nCurrentTagIndex >= nPageTagTextCount)
    {
        return;
    }

    CTagTitleHandle* pHandle = CTagTitleHandle::GetTagTitleHandle(m_pHandler);
    int nPageTagTextBegin = m_nCurrentTagIndex - m_nCurrentTagIndex % nPageTagTextShow;
    for (int i = 0; i < nPageTagTextShow; ++i, ++nPageTagTextBegin)
    {
        xTextView* pTextView = m_vecTagTextView[i];
        if (NULL == pTextView)
        {
            continue;
        }

        yl::string strText = GetString(nPageTagTextBegin);
        pTextView->SetText(strText);

        //设置文本颜色和背景
        if (NULL != pHandle)
        {
            bool bNormal = (nPageTagTextBegin != m_nCurrentTagIndex);
            pTextView->SetTextColor(pHandle->GetFontColor(bNormal));
            pTextView->background = pHandle->GetTextFontBG(bNormal);
        }
    }
}

void CTagTitle::UpdateArrow()
{
    CTagTitleHandle* pHandle = CTagTitleHandle::GetTagTitleHandle(m_pHandler);
    if (NULL == pHandle || NULL == m_pBtnLeftArrow || NULL == m_pBtnRightArrow)
    {
        return;
    }

    int nPageTagTextCount = m_vecTitle.size();
    int nPageTagTextShow = m_vecTagTextView.size();
    if (nPageTagTextCount <= 0 || nPageTagTextShow <= 0)
    {
        return;
    }

    bool bHasPreviousPage = (m_nCurrentTagIndex >= nPageTagTextShow);
    bool bHasNextPage = ((m_nCurrentTagIndex - m_nCurrentTagIndex % nPageTagTextShow + nPageTagTextShow) < nPageTagTextCount);

    m_pBtnLeftArrow->setDisable(!bHasPreviousPage);
    m_pBtnRightArrow->setDisable(!bHasNextPage);
}

#if IS_COLOR
void CTagTitle::UpdatePageInfo()
{
    if (NULL == m_pTagTextPageInfo)
    {
        return;
    }

    yl::string strPageInfo = commonAPI_FormatString("%d/%d", m_nCurrentTagIndex + 1, m_vecTitle.size());
    m_pTagTextPageInfo->SetText(strPageInfo);
}
#endif

int CTagTitle::GetIndex(const yl::string& strTagTitle)
{
    for (int i = 0; i < m_vecTitle.size(); ++i)
    {
        if (strTagTitle == m_vecTitle[i])
        {
            return i;
        }
    }

    return INVALID_TAG_INDEX;
}

yl::string CTagTitle::GetString(int nIndex)
{
    yl::string strTitle = "";
    if (nIndex >= 0 && nIndex < m_vecTitle.size())
    {
        strTitle = m_vecTitle[nIndex];
    }

    return strTitle;
}

#if !IS_COLOR
void CTagTitle::SetPageIndexTip(const yl::string& strTip)
{
    if (NULL != m_pTagTextPageInfo)
    {
        m_pTagTextPageInfo->SetText(strTip);
    }
}
#endif
