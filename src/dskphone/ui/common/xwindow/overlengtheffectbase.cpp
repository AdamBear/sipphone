#include "overlengtheffectbase.h"
#include "xwindow/xWindow.h"


COverLengthEffectBase::COverLengthEffectBase(int nType)
    : m_nOverLengthEffectType(nType)
    , m_pTextView(NULL)
{
}

COverLengthEffectBase::~COverLengthEffectBase()
{

}

yl::wstring COverLengthEffectBase::GetStringToPaint(const chRect& rcText, const yl::wstring& strText)
{
    return strText;
}

int COverLengthEffectBase::GetGravity()
{
    return gravity_undefined;
}

int COverLengthEffectBase::GetAlign()
{
    return gravity_undefined;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
COverLengthEllipsisEffect:: COverLengthEllipsisEffect()
    : COverLengthEffectBase(OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS)
{

}

COverLengthEllipsisEffect::~COverLengthEllipsisEffect()
{

}

COverLengthEffectBase* COverLengthEllipsisEffect::CreateOverLengthEllipsisEffect()
{
    COverLengthEllipsisEffect* pEllipsis = new COverLengthEllipsisEffect;
    return pEllipsis;
}

COverLengthEllipsisEffect* COverLengthEllipsisEffect::GetOverLengthEllipsisEffect(COverLengthEffectBase* pOverLengthEffectBase)
{
    if (NULL == pOverLengthEffectBase || GetOverLengthEllipsisEffectType() != pOverLengthEffectBase->GetType())
    {
        return NULL;
    }

    return static_cast<COverLengthEllipsisEffect*>(pOverLengthEffectBase);
}

int COverLengthEllipsisEffect::GetOverLengthEllipsisEffectType()
{
    return OVERLENGTH_TYPE_CUT_WITH_ELLIPSIS;
}

bool COverLengthEllipsisEffect::OnStartEffect()
{
    return false;
}

void COverLengthEllipsisEffect::onEndEffect()
{

}

yl::wstring COverLengthEllipsisEffect::GetStringToPaint(const chRect& rcText, const yl::wstring& strText)
{
    if (NULL == m_pTextView || strText.empty())
    {
        return strText;
    }

    if (m_pTextView->GetMultiLineEnable())
    {
        return GetMultiEllipsosEffectText(rcText, strText);
    }
    else
    {
        return GetEllipsisEffectText(rcText, strText, m_pTextView->GetFont());
    }
}

yl::wstring COverLengthEllipsisEffect::GetEllipsisEffectText(const chRect& rcText, const yl::wstring& wstr, const xFont& font)
{
    if (wstr.empty())
    {
        return wstr;
    }

    int nEllipsisSize = font.textBound("...").cx;
    yl::wstring result;

    if (nEllipsisSize >= rcText.Width())
    {
        result = L"...";

        return result;
    }

    typedef YLVector<yl::wstring> LINES;
    YLVector<yl::wstring> vecLines = yl::string_split<yl::wstring::value_type>(wstr, L"\n", true);

    //处理每一行
    for (LINES::iterator begin = vecLines.begin(); begin != vecLines.end(); ++begin)
    {
        yl::wstring& str = *begin;
        if (rcText.Width() < font.textBound(yl::to_utf8(str)).cx)
        {
            yl::wstring::size_type upper_pos = str.size();
            yl::wstring::size_type lower_pos = 0;
            yl::wstring::size_type mid_pos = (upper_pos + lower_pos) >> 1;
            yl::string paintText;
            //二分查找可以显示的字符数量
            int nLoop = 0;
            while (lower_pos != mid_pos)
            {
                DEAD_LOOP_BREAK(nLoop);

                paintText = yl::to_utf8(str.substr(0, mid_pos));
                int nTextSize = font.textBound(paintText).cx;

                if (nTextSize + nEllipsisSize <= rcText.Width())
                {
                    lower_pos = mid_pos;
                }
                else
                {
                    upper_pos = mid_pos;
                }

                mid_pos = (upper_pos + lower_pos) >> 1;
            }

            result.append(str.substr(0, lower_pos));
            result.append(L"...");
        }
        else
        {
            result.append(str);
        }

        result.push_back('\n');
    }

    //去掉最后一行的\n
    if (!result.empty() && result.back() == '\n')
    {
        result.pop_back();
    }

    return result;
}

yl::wstring COverLengthEllipsisEffect::GetMultiEllipsosEffectText(const chRect& rcText, const yl::wstring& wstr)
{
    if (NULL == m_pTextView || wstr.empty())
    {
        return wstr;
    }

    const xFont& font = m_pTextView->GetFont();
    yl::wstring strResult;

    typedef YLVector<yl::wstring> LINES;
    YLVector<yl::wstring> vecLines = yl::string_split<yl::wstring::value_type>(wstr, L"\n", true);

    if (vecLines.size() <= 0)
    {
        return wstr;
    }

    //先计算出文本区域能显示几行的内容
    int nHeight = rcText.Height();
    int nLimitHeight = m_pTextView->GetWrapLimitedHeight();
    if (-1 != nLimitHeight && nLimitHeight < nHeight)
    {
        nHeight = nLimitHeight;
    }

    int nShowLine = 0;
    int nLineHeight = 0;
    for (LINES::iterator begin = vecLines.begin(); begin != vecLines.end(); ++begin)
    {
        yl::wstring& str = *begin;
        nLineHeight += font.textBound(yl::to_utf8(str)).cy;
        if (nHeight >= nLineHeight)
        {
            nShowLine++;
        }
        else
        {
            break;
        }
    }

    if (nShowLine > vecLines.size() || nShowLine <= 0)
    {
        return wstr;
    }

    for (int i = 0; i < nShowLine - 1; ++i)
    {
        strResult.append(vecLines[i]);
        strResult.push_back('\n');
    }

    yl::wstring strEllipsis = vecLines[nShowLine - 1];
    if (nShowLine == vecLines.size())
    {
        strResult.append(strEllipsis);
    }
    else
    {
        yl::wstring strEllipsisResult = GetLineEllipsisText(rcText, strEllipsis);

        strResult.append(strEllipsisResult);
    }

    return strResult;
}

yl::wstring COverLengthEllipsisEffect::GetLineEllipsisText(const chRect& rcText, const yl::wstring& wstrEllipsis)
{
    if (NULL == m_pTextView)
    {
        return wstrEllipsis;
    }

    const xFont& font = m_pTextView->GetFont();

    int nEllipsisSize = font.textBound("...").cx;

    yl::wstring strEllipsisResult;
    if (wstrEllipsis.empty() || nEllipsisSize >= rcText.Width())
    {
        strEllipsisResult = L"...";
    }
    //如果后面能加3个省略号，则直接加上3个省略号
    else if (font.textBound(yl::to_utf8(wstrEllipsis)).cx + nEllipsisSize <= rcText.Width())
    {
        strEllipsisResult.append(wstrEllipsis);
        strEllipsisResult.append(L"...");
    }
    else
    {
        //如果不能，则预留3个省略号空间
        yl::wstring::size_type upper_pos = wstrEllipsis.size();
        yl::wstring::size_type lower_pos = 0;
        yl::wstring::size_type mid_pos = (upper_pos + lower_pos) >> 1;
        yl::string paintText;
        int nLoop = 0;
        //二分查找可以显示的字符数量
        while (lower_pos != mid_pos)
        {
            DEAD_LOOP_BREAK(nLoop);
            paintText = yl::to_utf8(wstrEllipsis.substr(0, mid_pos));
            int nTextSize = font.textBound(paintText).cx;

            if (nTextSize + nEllipsisSize <= rcText.Width())
            {
                lower_pos = mid_pos;
            }
            else
            {
                upper_pos = mid_pos;
            }

            mid_pos = (upper_pos + lower_pos) >> 1;
        }

        strEllipsisResult.append(wstrEllipsis.substr(0, lower_pos));
        strEllipsisResult.append(L"...");
    }

    return strEllipsisResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////
static const int DEFAULT_SCROLL_TICK_TIME          = 300;
static const int DEFAULT_SCROLL_OFFSET_PER_TICK    = 2;
static const int DEFAULT_SCROLL_END_STOP_TIME      = 2000;
static const int DEFAULT_SCROLL_STOP_TIME_PRELINE  = 1000;

/////////////////////////////////////////////////////////////////////////////////
COverLengthScrollEffectBase::COverLengthScrollEffectBase(OVERLENGTH_TYPE enType)
    : COverLengthEffectBase(enType)
    , m_bScrollEnabled(true)
    , m_nScrollDirection(SCROLL_DIRECTION_LEFT)
    , m_nScrollTickTime(DEFAULT_SCROLL_TICK_TIME)
    , m_nScrollShift(DEFAULT_SCROLL_OFFSET_PER_TICK)
    , m_nScrollRoundDuration(DEFAULT_SCROLL_END_STOP_TIME)
    , m_nScrollLineDuration(DEFAULT_SCROLL_STOP_TIME_PRELINE)
    , m_nScrollOffset(0)
    , m_nTotalScrollCount(0)
{

}

COverLengthScrollEffectBase::~COverLengthScrollEffectBase()
{
    m_tmScroll.KillTimer();
    m_tmScrollEnd.KillTimer();
}

void COverLengthScrollEffectBase::SetScrollEnable(bool bEnable)
{
    if (bEnable != m_bScrollEnabled)
    {
        m_bScrollEnabled = bEnable;
        m_nScrollOffset = 0;
    }
}

void COverLengthScrollEffectBase::SetScrollTickTime(int nTickTime)
{
    if (nTickTime >= 0)
    {
        m_nScrollTickTime = nTickTime;
    }
}

void COverLengthScrollEffectBase::SetScrollShift(int nShift)
{
    if (nShift >= 0)
    {
        m_nScrollShift = nShift;
    }
}

void COverLengthScrollEffectBase::SetScrollRoundDuration(int nRoundDuration)
{
    if (nRoundDuration >= 0)
    {
        m_nScrollRoundDuration = nRoundDuration;
    }
}

void COverLengthScrollEffectBase::SetScrollLineDuration(int nLineDuration)
{
    if (nLineDuration >= 0)
    {
        m_nScrollLineDuration = nLineDuration;
    }
}

void COverLengthScrollEffectBase::SetScrollCount(int nScrollCount)
{
    m_nTotalScrollCount = nScrollCount > 0 ? nScrollCount : -1;
}

void COverLengthScrollEffectBase::ResetScrollInfo()
{
    m_tmScrollEnd.KillTimer();
    m_tmScroll.KillTimer();
    m_nScrollOffset = 0;
}

void COverLengthScrollEffectBase::OnScrollEndTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    if (m_nTotalScrollCount > 0)
    {
        --m_nTotalScrollCount;
    }

    RestartScroll();
}

bool COverLengthScrollEffectBase::RestartScroll()
{
    return false;
}

void COverLengthScrollEffectBase::onEndEffect()
{
    ResetScrollInfo();
}

void COverLengthScrollEffectBase::loadAttributes(xml_node& node)
{
    PopupAndroidInt(node, XWINDOW_NAME_SCROLL_DIRECTION, m_nScrollDirection);
    PopupAndroidInt(node, XWINDOW_NAME_SCROLL_TICK, m_nScrollTickTime);
    PopupAndroidInt(node, XWINDOW_NAME_SCROLL_SHIFT, m_nScrollShift);
    PopupAndroidInt(node, XWINDOW_NAME_ROUND_DURATION, m_nScrollRoundDuration);
    PopupAndroidInt(node, XWINDOW_NAME_LINE_DURATION, m_nScrollLineDuration);

}

//////////////////////////////////////////////////////////////////////////////////////////////
CHorizontalScrollEffect::CHorizontalScrollEffect(OVERLENGTH_TYPE enType)
    : COverLengthScrollEffectBase(enType)
#if IF_TEXTVIEW_SCROLL_BY_CHAR
    , m_nScrollIndex(-1)
#endif
    , m_bScrollLeft(true)
{

}

CHorizontalScrollEffect::~CHorizontalScrollEffect()
{

}

int CHorizontalScrollEffect::GetHorizontalScrollEffectType()
{
    return OVERLENGTH_TYPE_SCROLL;
}


CHorizontalScrollEffect* CHorizontalScrollEffect::GetHorizontalScrollEffect(COverLengthEffectBase* pOverLengthEffectBase)
{
    if (NULL == pOverLengthEffectBase || GetHorizontalScrollEffectType() != pOverLengthEffectBase->GetType())
    {
        return NULL;
    }

    return static_cast<CHorizontalScrollEffect*>(pOverLengthEffectBase);
}

COverLengthEffectBase* CHorizontalScrollEffect::CreateHorizontalScrollEffect()
{
    CHorizontalScrollEffect* pScrollEffect = new CHorizontalScrollEffect(OVERLENGTH_TYPE_SCROLL);

    return pScrollEffect;
}

bool CHorizontalScrollEffect::OnStartEffect()
{
    ResetScrollInfo();

    m_bScrollLeft = true;

    if (SCROLL_DIRECTION_BOTH == m_nScrollDirection && m_nTotalScrollCount > 0)
    {
        m_nTotalScrollCount *= 2;
    }

    return RestartScroll();
}

yl::wstring CHorizontalScrollEffect::GetStringToPaint(const chRect& rcText, const yl::wstring& strText)
{
    return strText;
}

int CHorizontalScrollEffect::GetGravity()
{
    if (NULL != m_pTextView && !m_pTextView->IsOverLength())
    {
        return gravity_undefined;
    }

    if (SCROLL_DIRECTION_LEFT == m_nScrollDirection || (SCROLL_DIRECTION_BOTH == m_nScrollDirection && m_bScrollLeft))
    {
        return gravity_left;
    }
    else if (SCROLL_DIRECTION_RIGHT == m_nScrollDirection || (SCROLL_DIRECTION_BOTH == m_nScrollDirection && !m_bScrollLeft))
    {
        return gravity_right;
    }
    else
    {
        return gravity_undefined;
    }
}

int CHorizontalScrollEffect::GetAlign()
{
    if (NULL != m_pTextView && !m_pTextView->IsOverLength())
    {
        return gravity_undefined;
    }

    return gravity_left;
}

void CHorizontalScrollEffect::ResetScrollInfo()
{
    COverLengthScrollEffectBase::ResetScrollInfo();
#if IF_TEXTVIEW_SCROLL_BY_CHAR
    m_nScrollIndex = -1;
#endif
}

bool CHorizontalScrollEffect::RestartScroll()
{
    if (NULL == m_pTextView || m_pTextView->GetMultiLineEnable())
    {
        return false;
    }


    const chRect& rcText = m_pTextView->GetTextRect();
    if (rcText.Width() <= 0 || !m_bScrollEnabled || OVERLENGTH_TYPE_SCROLL != m_nOverLengthEffectType)
    {
        ResetScrollInfo();

        return false;
    }

    chSize szPaintString = m_pTextView->CachePaintTextSize();

    int nTextWidth = szPaintString.cx;

    if (rcText.Width() >= nTextWidth)
    {
        ResetScrollInfo();

        return false;
    }

    if (SCROLL_DIRECTION_BOTH == m_nScrollDirection && !m_bScrollLeft)
    {
        // 双向滚动到最左边，开始滚动
        m_nScrollOffset = nTextWidth - rcText.Width();
    }
    else if (SCROLL_DIRECTION_RIGHT == m_nScrollDirection)
    {
        m_bScrollLeft = false;
        m_nScrollOffset = nTextWidth - rcText.Width();
    }
    else
    {
        ResetScrollInfo();
    }

    // 文本超长，启动滚动定时器
    // 刚开始滚动先停止一段较长的时间
    m_tmScrollEnd.KillTimer();
    m_tmScroll.KillTimer();

#if IF_TEXTVIEW_SCROLL_BY_CHAR
    m_nScrollIndex = 0;
#endif

    m_tmScroll.SingleShot(m_nScrollRoundDuration, TIMER_METHOD(this, CHorizontalScrollEffect::OnScrollTimer));

    m_pTextView->update();

    return true;
}

void CHorizontalScrollEffect::OnScrollTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    UpdateScrollOffset();

    if (IsHorizontalScrollEnd())
    {
        if (SCROLL_DIRECTION_BOTH == m_nScrollDirection)
        {
            m_bScrollLeft = !m_bScrollLeft;
        }

        m_tmScroll.KillTimer();

        if (1 == m_nTotalScrollCount)
        {
            onEndEffect();
            if (NULL != m_pTextView)
            {
                m_pTextView->OnScrollFinish();
            }
            m_nTotalScrollCount = -1;
            return;
        }

        m_tmScrollEnd.SingleShot(m_nScrollRoundDuration, TIMER_METHOD(this, CHorizontalScrollEffect::OnScrollEndTimer));
    }
    else
    {
        if (!m_tmScroll.IsTimerRuning())
        {
            //开始连续滚动
            m_tmScroll.SetTimer(m_nScrollTickTime, TIMER_METHOD(this, CHorizontalScrollEffect::OnScrollTimer));
        }
#if IF_TEXTVIEW_SCROLL_BY_CHAR
        ++m_nScrollIndex;
#endif
    }

    m_pTextView->update();
}

bool CHorizontalScrollEffect::IsHorizontalScrollEnd()
{
    if (NULL == m_pTextView)
    {
        return true;
    }

#if IF_TEXTVIEW_SCROLL_BY_CHAR
    yl::wstring strPaintString = yl::to_utf16(m_pTextView->GetStringToPaint());
    int nLength = strPaintString.length();
    if (m_nScrollIndex >= nLength || strPaintString.empty())
    {
        return true;
    }

    yl::string StrOffsetText = "";
    if (m_bScrollLeft)
    {
        StrOffsetText = yl::to_utf8(strPaintString.substr(0, m_nScrollIndex));
    }
    else
    {
        StrOffsetText = yl::to_utf8(strPaintString.substr(nLength - m_nScrollIndex, nLength));
    }

    xFont font = m_pTextView->GetFont();
    const chRect& rcText = m_pTextView->GetTextRect();

    if (font.textBound(StrOffsetText).cx + rcText.Width() >= font.textBound(yl::to_utf8(strPaintString)).cx)
    {
        return true;
    }

    return false;
#else
    const chRect& rcText = m_pTextView->GetTextRect();
    chSize szText = m_pTextView->CachePaintTextSize();

    //向左滚动判断偏移是否小于0
    if (m_bScrollLeft)
    {
        return (m_nScrollOffset + rcText.Width() >= szText.cx);
    }
    else
    {
        return m_nScrollOffset <= 0;
    }
#endif
}

#if IF_TEXTVIEW_SCROLL_BY_CHAR
int CHorizontalScrollEffect::GetCharWidthByIndex(int nIndex)
{
    if (NULL == m_pTextView)
    {
        return -1;
    }

    yl::wstring strPaintString = yl::to_utf16(m_pTextView->GetStringToPaint());
    const chRect& rcText = m_pTextView->GetTextRect();
    int nLength = strPaintString.length();

    if (nIndex < 0 || nIndex > nLength || strPaintString.empty())
    {
        return -1;
    }

    xFont font = m_pTextView->GetFont();
    if (m_bScrollLeft)
    {
        yl::string strText = yl::to_utf8(strPaintString.substr(0, nIndex));
        return font.textBound(strText).cx;
    }
    else
    {
        yl::string strText = yl::to_utf8(strPaintString.substr(0, nLength - nIndex));
        return font.textBound(strText).cx - rcText.Width();
    }
}

#endif

bool CHorizontalScrollEffect::UpdateScrollOffset()
{
    if (NULL == m_pTextView)
    {
        ResetScrollInfo();
        return false;
    }

#if IF_TEXTVIEW_SCROLL_BY_CHAR
    if (m_nScrollIndex >= 0)
    {
        int nCharWidth = GetCharWidthByIndex(m_nScrollIndex);
        m_nScrollOffset = nCharWidth;
    }
#else
    int nUnit = m_bScrollLeft ? 1 : -1;
    m_nScrollOffset += nUnit * m_nScrollShift;
#endif

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if IF_TEXTVIEW_SUPPORT_VERTICAL_SCROLL
CVerticalScrollEffect::CVerticalScrollEffect(OVERLENGTH_TYPE enType)
    : COverLengthScrollEffectBase(enType)
    , m_nScrollTargetLine(0)
{

}

CVerticalScrollEffect::~CVerticalScrollEffect()
{
    m_vecLineVPos.clear();
}

int CVerticalScrollEffect::GetVerticalScrollEffectType()
{
    return OVERLENGTH_TYPE_VSCROLL;
}

CVerticalScrollEffect* CVerticalScrollEffect::GetVerticalScrollEffect(COverLengthEffectBase* pOverLengthEffectBase)
{
    if (NULL == pOverLengthEffectBase || GetVerticalScrollEffectType() != pOverLengthEffectBase->GetType())
    {
        return NULL;
    }

    return static_cast<CVerticalScrollEffect*>(pOverLengthEffectBase);
}

COverLengthEffectBase* CVerticalScrollEffect::CreateVerticalScrollEffect()
{
    CVerticalScrollEffect* pScrollEffect = new CVerticalScrollEffect(OVERLENGTH_TYPE_VSCROLL);

    return pScrollEffect;
}

bool CVerticalScrollEffect::OnStartEffect()
{
    ResetScrollInfo();
    return RestartScroll();
}

yl::wstring CVerticalScrollEffect::GetStringToPaint(const chRect& rcText, const yl::wstring& strText)
{
    if (NULL != m_pTextView)
    {
        return m_pTextView->GetAutoReturnText(strText);
    }

    return strText;
}

int CVerticalScrollEffect::GetGravity()
{
    return gravity_top;
}

int CVerticalScrollEffect::GetAlign()
{
    return gravity_top;
}

void CVerticalScrollEffect::ResetScrollInfo()
{
    COverLengthScrollEffectBase::ResetScrollInfo();
}

bool CVerticalScrollEffect::RestartScroll()
{
    if (NULL == m_pTextView)
    {
        return false;
    }

    const chRect& rcText = m_pTextView->GetTextRect();
    if (rcText.Width() <= 0 || !m_bScrollEnabled || OVERLENGTH_TYPE_VSCROLL != m_nOverLengthEffectType)
    {
        ResetScrollInfo();

        return false;
    }

    chSize szPaintString = m_pTextView->CachePaintTextSize();

    int nTextHeight = szPaintString.cy;

    if (rcText.Height() >= nTextHeight)
    {
        ResetScrollInfo();

        return false;
    }

    m_nScrollOffset = m_vecLineVPos[0];
    m_nScrollTargetLine = 1;

    // 文本超长，启动滚动定时器
    // 刚开始滚动先停止一段较长的时间
    m_tmScrollEnd.KillTimer();
    m_tmScroll.KillTimer();

    m_tmScroll.SingleShot(m_nScrollRoundDuration, MSG_METHOD(this, CVerticalScrollEffect::OnScrollTimer));

    m_pTextView->update();

    return true;
}

BOOL CVerticalScrollEffect::OnScrollTimer(msgObject& msg)
{
    if (OVERLENGTH_TYPE_VSCROLL != m_nOverLengthEffectType || NULL == m_pTextView)
    {
        ResetScrollInfo();
        return FALSE;
    }

    //开始滚动
    m_nScrollOffset += m_nScrollShift;
    const chRect& rcText = m_pTextView->GetTextRect();
    chSize szPaintString = m_pTextView->CachePaintTextSize();

    int nVscrol = m_vecLineVPos.size() > 0 ? m_vecLineVPos[0] : 0;

    if (m_nScrollOffset + rcText.Height() - nVscrol >= szPaintString.cy)
    {
        //显示到底部，重新滚动
        m_nScrollOffset = szPaintString.cy - rcText.Height() + nVscrol;
        m_tmScroll.KillTimer();
        m_tmScrollEnd.SingleShot(m_nScrollRoundDuration, MSG_METHOD(this, CVerticalScrollEffect::OnScrollEndTimer));
    }
    else if (m_nScrollOffset >= m_vecLineVPos[m_nScrollTargetLine])
    {
        //每行停止一小段时间
        m_nScrollTargetLine++;
        m_tmScroll.KillTimer();
        m_tmScroll.SingleShot(m_nScrollLineDuration, MSG_METHOD(this, CVerticalScrollEffect::OnScrollTimer));
    }
    else if (!m_tmScroll.IsTimerRuning())
    {
        //开始连续滚动
        m_tmScroll.SetTimer(m_nScrollTickTime, MSG_METHOD(this, CVerticalScrollEffect::OnScrollTimer));
    }

    m_pTextView->update();
    return TRUE;
}

void CVerticalScrollEffect::PrepareLineSize(const yl::wstring& strCachePaintString, int nGravity)
{
    if (NULL == m_pTextView)
    {
        return;
    }

    const xFont& font = m_pTextView->GetFont();
    chSize szPaintText = m_pTextView->CachePaintTextSize();
    const chRect& rcText = m_pTextView->GetTextRect();

    typedef YLVector<yl::wstring> LINES;
    YLVector<yl::wstring> vecLines = yl::string_split<yl::wstring::value_type>(strCachePaintString, L"\n", true);

    m_vecLineVPos.clear();
    //获取第一行的位置
    chRect rectTextBound(chPoint(0, 0), szPaintText);
    adjustRectByGravity(rcText, nGravity, rectTextBound);
    int nLinePos = rectTextBound.top;
    m_vecLineVPos.push_back(nLinePos);

    //获取后几行的位置
    for (LINES::iterator begin = vecLines.begin(); begin != vecLines.end(); ++begin)
    {
        yl::string str = yl::to_utf8(*begin);
        nLinePos += font.textBound(str).cy;
        m_vecLineVPos.push_back(nLinePos);
    }
}
#endif
