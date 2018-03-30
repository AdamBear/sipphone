#include "xGraphic.h"
#include "xgraphicproess.h"
#include "xWindowManager.h"
#include "imageformatdefine.h"
#include "imageformatfactory.h"
#include "bmpformathandle.h"
#include "jpegformathandle.h"
#include "pngformathandle.h"
#include "path_define.h"

#if IF_USE_FREETYPE_LIB
#include "xwindow/modfreetype.h"
#endif

#if IF_USE_DOB_FONT
#include "gDobFont.h"
#endif

// 根据传入的字体、显示宽度,将字符串strText进行省略处理
yl::string CutOffText(const yl::string& strText, xFont& objFont, int nShowWidth)
{
    int nTextWidth = objFont.textBound(strText).cx;

    // 当字符串为空或显示长度大于文字长度时返回原串
    if (strText.empty() || nShowWidth >= nTextWidth)
    {
        return strText;
    }

    yl::string strEndSymbol("...");

    int nShowTextWidth = nShowWidth - objFont.textBound(strEndSymbol).cx;

    // 当显示宽度小于末端省略字符时，返回省略字符
    if (nShowTextWidth <= 0)
    {
        return strEndSymbol;
    }

    // 二分法确定截断位置的范围
    int nLen = strText.length();
    int nMinIndex = 0;
    int nMaxIndex = nLen;
    int nMidIndex = (nMinIndex + nMaxIndex) >> 1;

    // 循环确定截断的范围
    while (nMaxIndex > nMinIndex + 1 && nMidIndex >= 0 && nMidIndex < nLen)
    {
        if (nShowTextWidth >= objFont.textBound(strText.substr(0, nMidIndex)).cx)
        {
            nMinIndex = nMidIndex;
        }
        else
        {
            nMaxIndex = nMidIndex;
        }

        nMidIndex = (nMinIndex + nMaxIndex) >> 1;
    }

    if (nMidIndex >= 0 && nMidIndex < nLen)
    {
        return strText.substr(0, nMidIndex) + strEndSymbol;
    }

    return strEndSymbol;
}

xRegion xRegion::OffsetRegion(const xRegion& region, int x, int y)
{
    xRegion result;
    const typeRectList& listRect = region.m_listRect;

    for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
    {
        result.m_listRect.push_back(OffsetRect(*it, x, y));
    }
    result.m_rectRound = OffsetRect(region.m_rectRound, x, y);
    return result;
}

//////////////////////////////////////////////////////////////////////////
// class xRegion
xRegion::xRegion()
{
}

xRegion::xRegion(const xRegion& rgn)
    : m_listRect(rgn.m_listRect)
    , m_rectRound(rgn.m_rectRound)
{
}

xRegion::xRegion(const chRect& rect)
{
    m_listRect.push_back(rect);
    m_rectRound = rect;
}

bool xRegion::empty() const
{
    return m_rectRound.IsEmpty();
}

const chRect& xRegion::bound() const
{
    return m_rectRound;
}

bool xRegion::operator == (const xRegion& src) const
{
    return m_rectRound == src.bound() && (*this - src).empty() && (src - *this).empty();
}

xRegion& xRegion::operator = (const chRect& src)
{
    m_listRect.clear();
    m_listRect.push_back(src);
    m_rectRound = src;
    return *this;
}

xRegion& xRegion::operator = (const xRegion& src)
{
    m_listRect = src.m_listRect;
    m_rectRound = src.m_rectRound;

    return *this;
}

xRegion xRegion::operator - (const chRect& src) const
{
    xRegion result;
    chRect andRect = m_rectRound & src;
    if (andRect.IsEmpty())
    {
        result = *this;
    }
    else if (andRect != m_rectRound)
    {
        for (typeRectList::const_iterator it = m_listRect.begin(); it != m_listRect.end(); ++it)
        {
            const chRect& rectChild = *it;
            andRect = rectChild & src;
            if (andRect.IsEmpty())
            {
                result.push_rect(rectChild);
            }
            else if (andRect != rectChild)
            {
                result.push_rect(chRect(rectChild.left, rectChild.top, rectChild.right, andRect.top));
                result.push_rect(chRect(rectChild.left, andRect.top, andRect.left, andRect.bottom));
                result.push_rect(chRect(andRect.right, andRect.top, rectChild.right, andRect.bottom));
                result.push_rect(chRect(rectChild.left, andRect.bottom, rectChild.right, rectChild.bottom));
            }
        }
    }
    return result;
}

xRegion& xRegion::operator -= (const chRect& src)
{
    chRect andRect = m_rectRound & src;
    if (andRect == m_rectRound)
    {
        m_listRect.clear();
        m_rectRound.SetEmpty();
    }
    else if (!andRect.IsEmpty())
    {
        chRect bound;
        for (typeRectList::iterator it = m_listRect.begin(); it != m_listRect.end();)
        {
            chRect& rectChild = *it;
            andRect = rectChild & src;
            if (andRect.IsEmpty())
            {
                bound |= rectChild;
                ++it;
            }
            else if (andRect == rectChild)
            {
                it = m_listRect.erase(it);
            }
            else
            {
                chRect sub;

                sub.SetRect(rectChild.left, rectChild.top, rectChild.right, andRect.top);

                if (!sub.IsEmpty())
                {
                    bound |= sub;
                    it = m_listRect.insert(it, sub);
                    ++it;
                }

                sub.SetRect(rectChild.left, andRect.top, andRect.left, andRect.bottom);

                if (!sub.IsEmpty())
                {
                    bound |= sub;
                    it = m_listRect.insert(it, sub);
                    ++it;
                }

                sub.SetRect(andRect.right, andRect.top, rectChild.right, andRect.bottom);
                if (!sub.IsEmpty())
                {
                    bound |= sub;
                    it = m_listRect.insert(it, sub);
                    ++it;
                }

                sub.SetRect(rectChild.left, andRect.bottom, rectChild.right, rectChild.bottom);
                if (!sub.IsEmpty())
                {
                    bound |= sub;
                    it = m_listRect.insert(it, sub);
                    ++it;
                }

                it = m_listRect.erase(it);
            }
        }
        m_rectRound = bound;
    }
    return *this;
}

xRegion xRegion::operator & (const chRect& src) const
{
    xRegion result;
    chRect andRect = m_rectRound & src;
    if (andRect == m_rectRound)
    {
        result = *this;
    }
    else if (!andRect.IsEmpty())
    {
        for (typeRectList::const_iterator it = m_listRect.begin(); it != m_listRect.end(); ++it)
        {
            andRect = *it & src;
            if (!andRect.IsEmpty())
            {
                result.push_rect(andRect);
            }
        }
    }
    return result;
}

xRegion& xRegion::operator &= (const chRect& src)
{
    chRect andRect = m_rectRound & src;
    if (andRect.IsEmpty())
    {
        m_listRect.clear();
    }
    else if (andRect != m_rectRound)
    {
        chRect bound;
        for (typeRectList::iterator it = m_listRect.begin(); it != m_listRect.end();)
        {
            chRect& rectChild = *it;
            andRect = rectChild & src;
            if (!andRect.IsEmpty())
            {
                rectChild = andRect;
                bound |= rectChild;
                ++it;
            }
            else
            {
                it = m_listRect.erase(it);
            }
        }
        m_rectRound = bound;
    }
    return *this;
}

xRegion xRegion::operator | (const chRect& src) const
{
    xRegion rgnResult = *this;

    rgnResult |= src;

    return rgnResult;
}

void CombineRectInOrOperator(typeRectList& listSelf, typeRectList::iterator& itSelf,
                             typeRectList& listExclusive, typeRectList::iterator& itExclusive,
                             const chRect& rcSelf, const chRect& rcExclusive, bool& bDelete)
{
    chRect rcCombine = rcSelf | rcExclusive;

    itExclusive = listExclusive.erase(itExclusive);
    if (itExclusive != listExclusive.end())
    {
        listExclusive.insert(itExclusive, rcCombine);
    }
    else
    {
        listExclusive.push_back(rcCombine);
    }

    // 有区域合并，重头开始遍历
    listSelf.erase(itSelf);
    itSelf = listSelf.begin();

    bDelete = true;
}

void ExclusiveRectInOrOperator(typeRectList& listExclusive, typeRectList::iterator& itExclusive,
                               const chRect& rcInter, const chRect& rcExclusive)
{
    xRegion rgnExclusive(rcExclusive);

    rgnExclusive -= rcInter;

    itExclusive = listExclusive.erase(itExclusive);

    if (!rgnExclusive.empty())
    {
        const typeRectList& listTmp = rgnExclusive.GetRectList();
        typeRectList::const_iterator itTmp = listTmp.begin();

        for (; itTmp != listTmp.end(); ++itTmp)
        {
            const chRect& rcTmp = *itTmp;

            if (itExclusive != listExclusive.end())
            {
                itExclusive = listExclusive.insert(itExclusive, rcTmp);
                ++itExclusive;
            }
            else
            {
                listExclusive.push_back(rcTmp);
                itExclusive = listExclusive.end();
            }
        }
    }
}

xRegion& xRegion::operator |= (const chRect& src)
{
    if (src.IsEmpty())
    {
        return *this;
    }

    if ((m_rectRound & src) == m_rectRound)
    {
        m_listRect.clear();
        m_listRect.push_back(src);
        m_rectRound = src;

        return *this;
    }

    typeRectList::iterator it = m_listRect.begin();
    typeRectList listExclusive;

    listExclusive.push_back(src);

    while (it != m_listRect.end())
    {
        typeRectList::iterator itExclusive = listExclusive.begin();
        const chRect& rcSelf = *it;
        bool bDelete = false;

        while (itExclusive != listExclusive.end())
        {
            const chRect& rcExclusive = *itExclusive;
            chRect rcInter = rcSelf & rcExclusive;

            if (rcInter.IsEmpty())
            {
                // rcSelf与rcExclusive刚好连接，并且高或宽一样，可以合并成一块
                bool bAdjoinH = (rcInter.left == rcInter.right && rcSelf.top == rcExclusive.top && rcSelf.bottom == rcExclusive.bottom);
                bool bAdjoinV = (rcInter.top == rcInter.bottom && rcSelf.left == rcExclusive.left && rcSelf.right == rcExclusive.right);

                if (bAdjoinH || bAdjoinV)
                {
                    CombineRectInOrOperator(m_listRect, it, listExclusive, itExclusive, rcSelf, rcExclusive, bDelete);

                    break;
                }

                ++itExclusive;
                continue;
            }

            // rcSelf <= rcExclusive
            if (rcInter == rcSelf)
            {
                it = m_listRect.erase(it);
                bDelete = true;

                break;
            }

            // rcExclusive <= rcSelf
            if (rcInter == rcExclusive)
            {
                itExclusive = listExclusive.erase(itExclusive);
                continue;
            }

            // rcSelf与rcExclusive高或宽相等，并且有共同区域，合并在一起
            if ((rcSelf.left == rcExclusive.left && rcSelf.right == rcExclusive.right)
                    || (rcSelf.top == rcExclusive.top && rcSelf.bottom == rcExclusive.bottom))
            {
                CombineRectInOrOperator(m_listRect, it, listExclusive, itExclusive, rcSelf, rcExclusive, bDelete);

                break;
            }

            // rcSelf与rcExclusive有交集，又不可以合并，将交集从rcExclusive删除掉
            // ExclusiveRectInOrOperator函数中会改掉itExclusive，不会引起死循环
            ExclusiveRectInOrOperator(listExclusive, itExclusive, rcInter, rcExclusive);
        }

        if (!bDelete)
        {
            ++it;
        }
    }

    for (it = listExclusive.begin(); it != listExclusive.end(); ++it)
    {
        push_rect(*it);
    }

    m_rectRound |= src;

    return *this;
}

xRegion xRegion::operator - (const xRegion& src) const
{
    if ((src.m_rectRound & m_rectRound).IsEmpty())
    {
        return *this;
    }
    else
    {
        xRegion result = *this;
        const typeRectList& listRect = src.m_listRect;

        for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
        {
            result -= (*it);
        }

        return result;
    }
}

xRegion& xRegion::operator -= (const xRegion& src)
{
    if (!(src.m_rectRound & m_rectRound).IsEmpty())
    {
        const typeRectList& listRect = src.m_listRect;

        for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
        {
            this->operator -=(*it);
        }
    }
    return *this;
}

xRegion xRegion::operator & (const xRegion& src) const
{
    if ((src.m_rectRound & m_rectRound).IsEmpty())
    {
        return xRegion();
    }

    const typeRectList& listRect = src.m_listRect;
    xRegion result;

    for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
    {
        xRegion tmp = this->operator &(*it);
        typeRectList& listTmpRect = tmp.m_listRect;

        for (typeRectList::iterator itTmp = listTmpRect.begin(); itTmp != listTmpRect.end(); ++itTmp)
        {
            result.push_rect(*itTmp);
        }
    }

    return result;
}

xRegion& xRegion::operator &= (const xRegion& src)
{
    if ((src.m_rectRound & m_rectRound).IsEmpty())
    {
        m_listRect.clear();
    }
    else
    {
        xRegion result;

        for (typeRectList::const_iterator it = src.m_listRect.begin(); it != src.m_listRect.end(); ++it)
        {
            xRegion tmp = this->operator &(*it);
            typeRectList& listTmpRect = tmp.m_listRect;

            for (typeRectList::iterator itTmp = listTmpRect.begin(); itTmp != listTmpRect.end(); ++itTmp)
            {
                result.push_rect(*itTmp);
            }
        }
        *this = result;
    }
    return *this;
}

xRegion xRegion::operator | (const xRegion& src) const
{
    xRegion result = *this;
    const typeRectList& listRect = src.m_listRect;

    for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
    {
        result |= (*it);
    }
    return result;
}

xRegion& xRegion::operator |= (const xRegion& src)
{
    const typeRectList& listRect = src.m_listRect;

    for (typeRectList::const_iterator it = listRect.begin(); it != listRect.end(); ++it)
    {
        this->operator |=(*it);
    }
    return *this;
}

xRegion& xRegion::moveRegion(int x, int y)
{
    for (typeRectList::iterator it = m_listRect.begin(); it != m_listRect.end(); ++it)
    {
        chRect& rcChild = *it;
        rcChild.OffsetRect(x, y);
    }
    m_rectRound.OffsetRect(x, y);
    return *this;
}

int xRegion::percent() const
{
    int nSum = 0;

    for (typeRectList::const_iterator it = m_listRect.begin(); it != m_listRect.end(); ++it)
    {
        const chRect& rcChild = *it;
        nSum += rcChild.Width() * rcChild.Height();
    }

    int nTotal = m_rectRound.Width() * m_rectRound.Height();

    if (nTotal <= 0)
    {
        return 0;
    }

    chASSERT(nSum <= nTotal);
    return nSum * 100 / nTotal;
}

void xRegion::clear()
{
    m_rectRound = chRect(0, 0, 0, 0);
    m_listRect.clear();
}

bool xRegion::hasIntersect(const chRect& rect) const
{
    if (!(rect & m_rectRound).IsEmpty())
    {
        for (typeRectList::const_iterator it = m_listRect.begin(); it != m_listRect.end(); ++it)
        {
            if (!(*it & rect).IsEmpty())
            {
                return true;
            }
        }
    }
    return false;
}

bool xRegion::hasIntersect(const xRegion& src) const
{
    if (!(src.m_rectRound & m_rectRound).IsEmpty())
    {
        for (typeRectList::const_iterator it = src.m_listRect.begin(); it != src.m_listRect.end(); ++it)
        {
            if (hasIntersect(*it))
            {
                return true;
            }
        }
    }
    return false;
}

const typeRectList& xRegion::GetRectList() const
{
    return m_listRect;
}

void xRegion::push_rect(const chRect& rect)
{
    if (!rect.IsEmpty())
    {
        m_rectRound |= rect;

        typeRectList::iterator iter = m_listRect.begin();

        for (; iter != m_listRect.end(); ++iter)
        {
            const chRect& rcTmp = *iter;

            if (rcTmp.top > rect.top)
            {
                break;
            }
            else if (rcTmp.top == rect.top && rcTmp.left > rect.left)
            {
                break;
            }
        }

        if (iter != m_listRect.end())
        {
            m_listRect.insert(iter, rect);
        }
        else
        {
            m_listRect.push_back(rect);
        }
    }
}

#if IF_XGRAPHIC_DEBUG
void xRegion::Dump() const
{
    XWINDOW_INFO("==Region:%p is:");

    DumpRectList(m_listRect);

    XWINDOW_INFO("==");
}

void xRegion::DumpRectList(const typeRectList& listRect)
{
    typeRectList::const_iterator iter = listRect.begin();

    for (int i = 0; iter != listRect.end(); ++iter, ++i)
    {
        const chRect& rcTmp = *iter;

        XWINDOW_INFO("==Rect:%d is:%d %d %d %d Width:%d Height:%d", i,
                     rcTmp.left, rcTmp.top, rcTmp.right, rcTmp.bottom,
                     rcTmp.Width(), rcTmp.Height());
    }
}
#endif

//////////////////////////////////////////////////////////////////////////
xColor::xColor()
#if IF_ONLY_USE_GRADATION
    : gray(0)
#else
    : b(0)
    , g(0)
    , r(0)
#if IF_PIXEL_ALPHA
    , a(0)
#endif
#endif
{
}

xColor::xColor(BYTE _r, BYTE _g, BYTE _b)
#if IF_ONLY_USE_GRADATION
    : gray((BYTE)(((int)_r + (int)_g + (int)_b) / 3))
#else
    : b(_b)
    , g(_g)
    , r(_r)
#if IF_PIXEL_ALPHA
    , a(0xFF)
#endif
#endif
{
}

#if IF_ONLY_USE_GRADATION
xColor::xColor(BYTE byGray)
    : gray(byGray)
{
}
#else
#if IF_PIXEL_ALPHA
xColor::xColor(BYTE _a, BYTE _r, BYTE _g, BYTE _b)
    : b(_b)
    , g(_g)
    , r(_r)
    , a(_a)
{
}
#endif

xColor::operator DWORD () const
{
#if IF_PIXEL_ALPHA
    return *(DWORD*)this;
#else
    DWORD dwRed = r;
    DWORD dwGreen = g;

    return ((dwRed << 16) + (dwGreen << 8) + b);
#endif
}
#endif

#if IF_USE_GRADATION
xColor::operator BYTE() const
{
#if IF_ONLY_USE_GRADATION
    return gray;
#else
    return (BYTE)(((int)pPixel->r + (int)pPixel->g + (int)pPixel->b) / 3)
#endif
}
#endif

//////////////////////////////////////////////////////////////////////////
// class xGraphic
int xGraphic::s_nDefaultFormat = GRAPHIC_DEFAULT_FORMAT;
#if IF_PIXEL_ALPHA
int xGraphic::s_nDefaultTransparentFormat = GRAPHIC_DEFAULT_TRANPARENT_FORMAT;
#endif
#if IF_GRAPHIC_SUPPORT_COLOR_KEY
xColor xGraphic::s_clrKey = xColor(224, 224, 224);
u16int xGraphic::s_u16Key = 0xE71C;
#endif

xGraphic::xGraphic()
    : m_nFormat(s_nDefaultFormat)
    , m_pPixmap(NULL)
    , m_nWidthPixels(0)
    , m_nHeightPixels(0)
#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
    , m_nFlag(false)
#endif
#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
    , m_pScreenGraphic(NULL)
#endif
{
}

xGraphic::~xGraphic()
{
    destroyGraphic();
}

xGraphic::xGraphic(const xGraphic& src)
    : m_pPixmap(NULL)
    , m_nWidthPixels(src.m_nWidthPixels)
    , m_nHeightPixels(src.m_nHeightPixels)
    , m_nFormat(src.m_nFormat)
#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
    , m_pScreenGraphic(NULL)
#endif
{
    if (m_nWidthPixels > 0 && m_nHeightPixels > 0
            && createGraphic(m_nWidthPixels, m_nHeightPixels))
    {
        chRect rcSrc(0, 0, src.m_nWidthPixels, src.m_nHeightPixels);

#if IF_ONLY_USE_GRADATION
        xGraphicOperate::tranGraphic(src, rcSrc, *this, rcSrc);
#else
        int nDataLen = xGraphicOperate::getDataLength(src.m_nFormat, m_nWidthPixels * m_nHeightPixels);

        xGraphicOperate::quickCopy(src, *this, rcSrc, nDataLen);
#endif
    }
}

xGraphic & xGraphic::operator = (const xGraphic& src)
{
    if (&src == this)
    {
        return *this;
    }

    destroyGraphic();

    m_nFormat = src.m_nFormat;

    m_nWidthPixels = src.m_nWidthPixels;
    m_nHeightPixels = src.m_nHeightPixels;

    if (m_nWidthPixels <= 0 || m_nHeightPixels <= 0)
    {
        return *this;
    }

    if (!createGraphic(m_nWidthPixels, m_nHeightPixels))
    {
        return *this;
    }

    chRect rcSrc(0, 0, src.m_nWidthPixels, src.m_nHeightPixels);

#if IF_ONLY_USE_GRADATION
    xGraphicOperate::tranGraphic(src, rcSrc, *this, rcSrc);
#else
    int nDataLen = xGraphicOperate::getDataLength(src.m_nFormat, m_nWidthPixels * m_nHeightPixels);

    xGraphicOperate::quickCopy(src, *this, rcSrc, nDataLen);
#endif

    return *this;
}

int xGraphic::GetDefaultFormat()
{
    return s_nDefaultFormat;
}

void xGraphic::SetDefaultFormat(int nFormat)
{
    s_nDefaultFormat = nFormat;
}

#if IF_PIXEL_ALPHA
int xGraphic::GetDefaultTranparentFormat()
{
    return s_nDefaultTransparentFormat;
}

void xGraphic::SetDefaultTranparentFormat(int nFormat)
{
    s_nDefaultTransparentFormat = nFormat;
}
#endif

bool xGraphic::createGraphic(int nWidth, int nHeight)
{
    chASSERT(NULL == m_pPixmap);

#if IF_ONLY_USE_GRADATION
    int nSize = nWidth * nHeight;
#else
    int nSize = xGraphicOperate::getDataLength(m_nFormat, nWidth * nHeight);
#endif

    if (nSize <= 0)
    {
        return false;
    }

    m_pPixmap = new BYTE[nSize];

    if (NULL == m_pPixmap)
    {
        return false;
    }

    m_nWidthPixels = nWidth;
    m_nHeightPixels = nHeight;

    return true;
}

void xGraphic::destroyGraphic()
{
    if (NULL != m_pPixmap)
    {
        delete[] m_pPixmap;

        m_pPixmap = NULL;
        m_nWidthPixels = 0;
        m_nHeightPixels = 0;
    }

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
    DetachScreenGraphic();
#endif
}

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
bool xGraphic::AttachScreenGraphic(xScreenGraphic* pScreenGraphic)
{
    if (NULL == pScreenGraphic)
    {
        return false;
    }

    m_pScreenGraphic = pScreenGraphic;

    m_pScreenGraphic->OnAttachGraphic(this);

    return true;
}

bool xGraphic::DetachScreenGraphic()
{
    if (NULL == m_pScreenGraphic)
    {
        return false;
    }

    m_pScreenGraphic->OnDetachGraphic(this);

    m_pScreenGraphic = NULL;

    return true;
}
#endif

void xGraphic::SetFormat(int nFormat)
{
    // 已经有数据了，不能变更图片格式
    if (NULL != m_pPixmap)
    {
        return;
    }

    m_nFormat = nFormat;
}

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
void xGraphic::setAlphaFlag(bool bOpacity, bool bFullTransparent)
{
    int nAlphaFlag = xGraphic::GRAPHIC_FLAG_OPACITY;

    if (!bOpacity)
    {
        nAlphaFlag = xGraphic::GRAPHIC_FLAG_TRANSPARENT;

        if (bFullTransparent)
        {
            nAlphaFlag |= xGraphic::GRAPHIC_FLAG_FULL_TRANSPARENT;
        }
    }

    setAlphaFlag(nAlphaFlag);
}

void xGraphic::setAlphaFlag(int nAlphaFlag)
{
    m_nFlag &= (~GRAPHIC_FLAG_ALPHA_MASK);
    m_nFlag |= nAlphaFlag;
}
#endif

#if IF_GRAPHIC_MARK_ROTATE
void xGraphic::setRotateFlag(int nAngle)
{
    int nRotateFlag = GRAPHIC_FLAG_NONE;

    switch (nAngle)
    {
    case 90:
    case -270:
        nRotateFlag = GRAPHIC_FLAG_ROTATE_90;
        break;
    case 180:
    case -180:
        nRotateFlag = GRAPHIC_FLAG_ROTATE_180;
        break;
    case 270:
    case -90:
        nRotateFlag = GRAPHIC_FLAG_ROTATE_270;
        break;
    default:
        break;
    }

    m_nFlag &= (~GRAPHIC_FLAG_ROTATE_MASK);
    m_nFlag |= nRotateFlag;
}
#endif

//////////////////////////////////////////////////////////////////////////
// class xPixmap
xPixmap::xPixmap()
    : m_pGraphic(NULL)
{
}

xPixmap::xPixmap(const xPixmap& src)
{
    m_pGraphic = src.m_pGraphic;
#if IF_PIXMAP_PARTLY_SHARED
    m_rcPixmap = src.m_rcPixmap;
#endif
#if IF_PIXMAP_SUPPORT_DOT_9
    m_pDot9Info = src.m_pDot9Info;
#endif
}

xPixmap::xPixmap(const yl::string& strFile)
{
    g_WorkStation.GetImageByPath(strFile, *this);
}

xPixmap::~xPixmap()
{

}

xPixmap & xPixmap::operator = (const xPixmap& src)
{
    if (m_pGraphic.Get() != src.m_pGraphic.Get())
    {
        m_pGraphic = src.m_pGraphic;
#if IF_PIXMAP_PARTLY_SHARED
        m_rcPixmap = src.m_rcPixmap;
#endif
#if IF_PIXMAP_SUPPORT_DOT_9
        m_pDot9Info = src.m_pDot9Info;
#endif
    }

    return *this;
}

bool xPixmap::Create(int nWidth, int nHeight, int nFormat/* = GRAPHIC_DEFAULT_FORMAT*/)
{
    if (!isNull())
    {
        setNull();
    }

    xGraphic* pGraphic = new xGraphic;

    if (NULL == pGraphic)
    {
        return false;
    }

    pGraphic->SetFormat(nFormat);

    if (!pGraphic->createGraphic(nWidth, nHeight))
    {
        delete pGraphic;
        return false;
    }

    m_pGraphic = xGraphicPtr(pGraphic);

#if IF_PIXMAP_PARTLY_SHARED
    m_rcPixmap.SetRect(0, 0, nWidth, nHeight);
#endif

    return true;
}

#if IF_ONLY_USE_GRADATION
bool xPixmap::LoadFromDobData(BYTE* pData, int nWidth /*= 0*/, int nHeight /*= 0*/)
{
    if (NULL == pData)
    {
        return false;
    }

    int w = 0;
    int h = 0;

    if (0 == nWidth || 0 == nHeight)
    {
        w = pData[0];
        h = pData[1];
        pData += 2;
    }
    else
    {
        w = nWidth;
        h = nHeight;
    }

    //转换为xPixel
    if (!Create(w, h))
    {
        return false;
    }

    xGraphic* pGraphic = m_pGraphic.Get();

    if (NULL == pGraphic)
    {
        return false;
    }

    BYTE* pixDst = pGraphic->m_pPixmap;

    for (int iY = 0; iY < h; ++iY)
    {
        for (int iX = 0; iX < w; ++iX)
        {
            int iIndexPix = iY * w + iX;
            BYTE byBit4 = pData[iIndexPix / 2];

            if (iIndexPix % 2)
            {
                //RGB值为亮度，DOB所存为灰度，所以应该取反
                //需要将4位映射到8位上, 乘16, 与原先右移操作刚好抵消
                pixDst[iIndexPix] = ~(byBit4 & 0xf0);
            }
            else
            {
                pixDst[iIndexPix] = ~((byBit4 & 0x0f) << 4);
            }
        }
    }
    return true;
}
#endif

bool xPixmap::load(const yl::string& strFile)
{
    m_pGraphic.Reset();

#if IF_ONLY_USE_GRADATION
    if (NULL == strstr(strFile.c_str(), ".dob") && NULL == strstr(strFile.c_str(), ".1b"))
    {
        return false;
    }

    //移植dotWindow Dob读取代码
    dobFile file;

    if (!file.Open(strFile.c_str(), chFileStdio::modeRead | chFileStdio::typeBinary)
            || file.GetBufferSize() <= 0)
    {
        return false;
    }

    BYTE* fileBuffer = file.GetBuffer();

    return LoadFromDobData(fileBuffer, 0, 0);
#else
    FILE* pFile = fopen(strFile.c_str(), "rb");

    if (NULL == pFile)
    {
        XWINDOW_ERR("Can Not Open File[%s]", strFile.c_str());
        return false;
    }

    CImageFormatHandleBase* pHandle = g_ImageFormatFactory.GetImageHandleByFileName(strFile);

    if (NULL == pHandle || !pHandle->IsFileHandleCorrect(pFile))
    {
        pHandle = g_ImageFormatFactory.GetImageHandleByFileHead(pFile, pHandle);
    }

    if (NULL == pHandle)
    {
        fclose(pFile);
        return false;
    }

    int nErroeCode = -1;

    if (!pHandle->LoadFile(pFile, *this, nErroeCode))
    {
        XWINDOW_INFO("Load image(%s) fail! ErrorCode=%d", strFile.c_str(), nErroeCode);

        fclose(pFile);
        setNull();
        return false;
    }

    fclose(pFile);

    xGraphic* pGraphic = m_pGraphic.Get();

    if (NULL == pGraphic)
    {
        return false;
    }

#if IF_PIXMAP_SUPPORT_DOT_9
    if (yl::string::npos != strFile.find(".9.") && GRAPHIC_FORMAT_BGRA32 == pGraphic->m_nFormat)
    {
        bool bTile = true;

        if (yl::string::npos != strFile.find(".s.9."))
        {
            bTile = false;
        }

        InitDot9Info(bTile);
    }
#endif

    return true;
#endif
}

bool xPixmap::load(const char* pFlow, int iFlowSize)
{
    m_pGraphic.Reset();

#if IF_ONLY_USE_GRADATION
    return LoadFromDobData((BYTE*)pFlow, 0, 0);
#else
    return false;
#endif
}

#if IF_ONLY_USE_GRADATION
bool xPixmap::save(const yl::string& strFile, chSize picSize)
#else
bool xPixmap::save(const yl::string& strFile, chSize picSize, const yl::string& strFormat /*= ""*/)
#endif
{
    xGraphic* pGraphic = m_pGraphic.Get();

    if (NULL == pGraphic)
    {
        return false;
    }

    int nWidth = pGraphic->width();
    int nHeight = pGraphic->height();

    if (nWidth <= 0 || nHeight <= 0)
    {
        return false;
    }

#if IF_ONLY_USE_GRADATION
    FILE* pFile = fopen(strFile.c_str(), "wb");

    if (NULL == pFile)
    {
        return false;
    }

    BYTE byWidth = (BYTE)nWidth;
    BYTE byHeight = (BYTE)nHeight;

    fwrite((LPVOID)&byWidth, 1, 1, pFile);
    fwrite((LPVOID)&byHeight, 1, 1, pFile);

    int nLineBytes = nWidth >> 1;

    if (nLineBytes <= 0)
    {
        nLineBytes = 1;
    }

    BYTE* pPixel = pGraphic->m_pPixmap;
    BYTE* pLine = new BYTE[nLineBytes];

    for (int i = 0; i < nHeight; ++i)
    {
        BYTE* pTarget = pLine;

        for (int j = 0; j < nLineBytes; ++j, ++pTarget)
        {
            *pTarget = ~(*pPixel) & 0xf0;// *NOPAD*

            ++pPixel;

            *pTarget |= ((~(*pPixel) >> 4) & 0x0f);// *NOPAD*

            ++pPixel;
        }

        fwrite((LPVOID)pLine, nLineBytes, 1, pFile);
    }

    delete [] pLine;

    fclose(pFile);

    return true;
#else
    chRect rcSrc(0, 0, nWidth, nHeight);

#if IF_GRAPHIC_ZOOM
    xGraphicPtr pScaleGraphic(NULL);

    if (nWidth != picSize.cx || nHeight != picSize.cy)
    {
#if IF_GRAPHIC_SUPPORT_RGB24_SCALE
        pScaleGraphic = xGraphicOperate::zoomGraphic(*pGraphic, rcSrc, picSize.cx, picSize.cy);
#else
        pScaleGraphic = xGraphicScale::ScaleGraphic(*pGraphic, rcSrc, picSize.cx, picSize.cy);
#endif

        pGraphic = pScaleGraphic.Get();

        if (NULL == pGraphic)
        {
            return false;
        }

        rcSrc.right = picSize.cx;
        rcSrc.top = picSize.cy;
    }
#endif

    CImageFormatHandleBase* pHandle = g_ImageFormatFactory.GetFitSaveImageFormatHandle(strFile, strFormat);

    if (NULL == pHandle)
    {
        return false;
    }

    int nTargetFormat = pHandle->GetSaveFormatBitDepth();
    xGraphicPtr pConvertGraphic(NULL);

    if (nTargetFormat != pGraphic->m_nFormat)
    {
        pConvertGraphic = xGraphicOperate::convertGraphic(*pGraphic, rcSrc, nTargetFormat);

        pGraphic = pConvertGraphic.Get();

        if (NULL == pGraphic)
        {
            XWINDOW_INFO("xPixmap::save, Convert Format Error");
            return false;
        }
    }

    int nErrorCode = -1;

    if (!pHandle->SaveFile(strFile, pGraphic, nErrorCode))
    {
        XWINDOW_INFO("xPixmap::save(%s) fail, ErrorCode:%d", strFile.c_str(), nErrorCode);
        return false;
    }

    return true;
#endif
}

#if IF_GRAPHIC_ZOOM
bool xPixmap::ScaleCopy(const xPixmap& src)
{
    xGraphicPtr pSrcGraphic = src.m_pGraphic;

    if (m_pGraphic.IsEmpty() || pSrcGraphic.IsEmpty()
            || pSrcGraphic->m_nFormat != m_pGraphic->m_nFormat)
    {
        return false;
    }

    chRect rcSrc(0, 0, src.width(), src.height());

#if IF_PIXEL_ALPHA && IF_GRAPHIC_SUPPORT_RGB24_SCALE
    return xGraphicScale::ScaleGraphic(*pSrcGraphic.Get(), rcSrc, *m_pGraphic.Get(), true);
#else
    return xGraphicScale::ScaleGraphic(*pSrcGraphic.Get(), rcSrc, *m_pGraphic.Get());
#endif
}
#endif

bool xPixmap::Copy(xGraphic* pGraphic)
{
    if (NULL == pGraphic || m_pGraphic.IsEmpty()
            || pGraphic->m_nFormat != m_pGraphic->m_nFormat)
    {
        return false;
    }

    int nWidth = m_pGraphic->width();
    int nHeight = m_pGraphic->height();

    if (pGraphic->width() != nWidth || pGraphic->height() != nHeight)
    {
        return false;
    }

#if IF_ONLY_USE_GRADATION
    int nDataLen = nWidth * nHeight;
#else
    int nDataLen = xGraphicOperate::getDataLength(pGraphic->m_nFormat, nWidth * nHeight);
#endif

    if (nDataLen <= 0)
    {
        return false;
    }

    memcpy((LPVOID)m_pGraphic->m_pPixmap, (LPVOID)pGraphic->pixels(), nDataLen);

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL || IF_GRAPHIC_MARK_ROTATE
    m_pGraphic->m_nFlag = pGraphic->m_nFlag;
#endif

    return true;
}

#if IF_SCREEN_SUPPORT_DEVICE_GRAPHIC
bool xPixmap::AttachScreenGraphic(xScreenGraphic* pScreenGraphic, const chRect& rcPixmap)
{
    if (m_pGraphic.IsEmpty() || !m_pGraphic->AttachScreenGraphic(pScreenGraphic))
    {
        return false;
    }

#if IF_PIXMAP_PARTLY_SHARED
    m_rcPixmap = rcPixmap;
#endif

    return true;
}

bool xPixmap::DetachScreenGraphic()
{
    return (m_pGraphic.IsEmpty() ? false : m_pGraphic->DetachScreenGraphic());
}

xScreenGraphic * xPixmap::GetScreenGraphic() const
{
    return (m_pGraphic.IsEmpty() ? NULL : m_pGraphic->GetScreenGraphic());
}
#endif

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
bool xPixmap::hasAlphaChannel() const
{
    if (m_pGraphic.IsEmpty())
    {
        return false;
    }

    return m_pGraphic->hasAlphaPixel();
}
#endif

#if IF_PIXMAP_SUPPORT_DOT_9
void xPixmap::InitDot9Info(bool bTile)
{
    m_pDot9Info.Reset();

    if (m_pGraphic.IsEmpty())
    {
        return;
    }

    xPixel* pData = (xPixel*)m_pGraphic->pixels();

    if (NULL == pData)
    {
        return;
    }

    int nBeginH = INVALID_DOT_9_VALUE;
    int nEndH = INVALID_DOT_9_VALUE;
    int nBeginV = INVALID_DOT_9_VALUE;
    int nEndV = INVALID_DOT_9_VALUE;
    int nStride = m_pGraphic->width();

    bool bHorizontal = GetDot9HorizontalInfo(pData, nStride, nStride, nBeginH, nEndH);
    bool bVertical = GetDot9VerticalInfo(pData, nStride, m_pGraphic->height(), nBeginV, nEndV);

    if (!bHorizontal && !bVertical)
    {
        return;
    }

    xDot9Info* pInfo = new xDot9Info;

    if (NULL == pInfo)
    {
        return;
    }

    pInfo->m_nBeginH = nBeginH;
    pInfo->m_nEndH = nEndH;
    pInfo->m_nBeginV = nBeginV;
    pInfo->m_nEndV = nEndV;
    pInfo->m_bTile = bTile;

    m_pDot9Info = xDot9InfoPtr(pInfo);
}

bool xPixmap::GetDot9HorizontalInfo(const xPixel* pPixel, int nStride, int nWidth, int& nBeginH, int& nEndH)
{
    if (NULL == pPixel)
    {
        return false;
    }

    nBeginH = INVALID_DOT_9_VALUE;
    nEndH = INVALID_DOT_9_VALUE;

#if IF_PIXMAP_PARTLY_SHARED
    pPixel = pPixel + m_rcPixmap.left + m_rcPixmap.top * nStride;
    nWidth = m_rcPixmap.Width();
#else
    pPixel = pPixel;
#endif

    for (int i = 0; i < nWidth; ++i)
    {
        if (0xff == pPixel->a)
        {
            if (INVALID_DOT_9_VALUE == nBeginH)
            {
                nBeginH = i;
            }

            nEndH = i + 1;
        }
        else if (INVALID_DOT_9_VALUE != nEndH)
        {
            break;
        }

        ++pPixel;
    }

    return (INVALID_DOT_9_VALUE != nBeginH);
}

bool xPixmap::GetDot9VerticalInfo(const xPixel* pPixel, int nStride, int nHeight, int& nBeginV, int& nEndV)
{
    if (NULL == pPixel)
    {
        return false;
    }

    nBeginV = INVALID_DOT_9_VALUE;
    nEndV = INVALID_DOT_9_VALUE;

#if IF_PIXMAP_PARTLY_SHARED
    pPixel = pPixel + m_rcPixmap.left + m_rcPixmap.top * nStride;
    nHeight = m_rcPixmap.Height();
#else
    pPixel = pPixel;
#endif

    for (int i = 0; i < nHeight; ++i)
    {
        if (0xff == pPixel->a)
        {
            if (INVALID_DOT_9_VALUE == nBeginV)
            {
                nBeginV = i;
            }

            nEndV = i + 1;
        }
        else if (INVALID_DOT_9_VALUE != nEndV)
        {
            break;
        }

        pPixel += nStride;
    }

    return (INVALID_DOT_9_VALUE != nBeginV);
}
#endif

void xPixmap::setNull()
{
    m_pGraphic.Reset();
#if IF_PIXMAP_PARTLY_SHARED
    m_rcPixmap.SetEmpty();
#endif
}

int xPixmap::width() const
{
#if IF_PIXMAP_PARTLY_SHARED
    return m_rcPixmap.Width();
#else
    if (m_pGraphic.IsEmpty())
    {
        return 0;
    }

    return m_pGraphic->width();
#endif
}

int xPixmap::height() const
{
#if IF_PIXMAP_PARTLY_SHARED
    return m_rcPixmap.Height();
#else
    if (m_pGraphic.IsEmpty())
    {
        return 0;
    }

    return m_pGraphic->height();
#endif
}

int xPixmap::GetRealWidth()
{
    int nWidth = width();

#if IF_PIXMAP_SUPPORT_DOT_9
    xDot9Info* pDot9 = GetDot9Info();
    if (NULL != pDot9 && nWidth >= 2)
    {
        nWidth -= 2;
    }
#endif

    return nWidth;
}

int xPixmap::GetRealHeight()
{
    int nHeight = height();

#if IF_PIXMAP_SUPPORT_DOT_9
    xDot9Info* pDot9 = GetDot9Info();
    if (NULL != pDot9 && nHeight >= 2)
    {
        nHeight -= 2;
    }
#endif

    return nHeight;
}

#if !IF_ONLY_USE_GRADATION
void xPixmap::InitImageFormatHandle()
{
#if IF_SUPPORT_BMP
    g_ImageFormatFactory.RegisterImageFormatFactory(kszImageFormatBMP, CBMPFormatHandle::CreateBMPFormatHandle);
#endif
#if IF_SUPPORT_JPEG
    g_ImageFormatFactory.RegisterImageFormatFactory(kszImageFormatJPEG, CJPEGFormatHandle::CreateJPEGFormatHandle);
#endif
#if IF_SUPPORT_PNG
    g_ImageFormatFactory.RegisterImageFormatFactory(kszImageFormatPNG, CPNGFormatHandle::CreatePNGFormatHandle);
#endif
}

bool xPixmap::GetImageSize(const yl::string&strFileName, chSize& szImage)
{
    return g_ImageFormatFactory.GetImageSize(strFileName, szImage);
}

bool xPixmap::convertFormat(int nFormat)
{
    if (m_pGraphic.IsEmpty())
    {
        return false;
    }

    if (nFormat == m_pGraphic->m_nFormat)
    {
        return true;
    }

    int nWidth = m_pGraphic->m_nWidthPixels;
    int nHeight = m_pGraphic->m_nHeightPixels;

    if (nWidth <= 0 || nHeight <= 0)
    {
        return false;
    }

    chRect rcPM(0, 0, nWidth, nHeight);

    xGraphicPtr pNewGraphic = xGraphicOperate::convertGraphic(*m_pGraphic.Get(), rcPM, nFormat);

    if (pNewGraphic.IsEmpty())
    {
        return false;
    }

    pNewGraphic->m_nFlag = m_pGraphic->m_nFlag;

    if (4 != xGraphicOperate::getDataLength(nFormat, 1)
            || 4 != xGraphicOperate::getDataLength(m_pGraphic->m_nFormat, 1))
    {
        pNewGraphic->setAlphaFlag(xGraphic::GRAPHIC_FLAG_OPACITY);
    }

    m_pGraphic = pNewGraphic;

    return true;
}

bool xPixmap::convertDefaultFormat()
{
    xGraphic* pGraphic = m_pGraphic.Get();

    if (NULL == pGraphic)
    {
        return false;
    }

    int nFormat = pGraphic->m_nFormat;

    if (GRAPHIC_FORMAT_BGRA32 == pGraphic->m_nFormat && pGraphic->hasAlphaPixel())
    {
        if (pGraphic->IsAlphaFullTransparent())
        {
            nFormat = xGraphic::s_nDefaultTransparentFormat;
        }
    }
    else if (xGraphic::s_nDefaultFormat != pGraphic->m_nFormat)
    {
        nFormat = xGraphic::s_nDefaultFormat;
    }

    if (nFormat != pGraphic->m_nFormat)
    {
        return convertFormat(nFormat);
    }

    return false;
}
#endif

//////////////////////////////////////////////////////////////////////////
// class xFont
int xFont::s_nDefaultSize = DEFAULT_FONT_SIZE;

xFont::xFont()
{
#if IF_USE_DOB_FONT
#if IF_USE_FREETYPE_LIB
    if (g_WorkStation.IsUseDotLib())
    {
#endif
        m_pDobFont = new dobFont;

        setPointSize(s_nDefaultSize);
#if IF_USE_FREETYPE_LIB
        return;
    }
#endif
#endif

#if IF_USE_FREETYPE_LIB
#if IF_USE_DOB_FONT
    m_pDobFont = NULL;
#endif
    setPointSize(s_nDefaultSize);
#endif
}

xFont::xFont(const yl::string& family, int pointSize, bool bold)
{
#if IF_USE_DOB_FONT
#if IF_USE_FREETYPE_LIB
    if (g_WorkStation.IsUseDotLib())
    {
#endif
        m_pDobFont = new dobFont;

        setPointSize(pointSize);
        setFamily(family);
#if IF_USE_FREETYPE_LIB
        return;
    }
#endif
#endif

#if IF_USE_FREETYPE_LIB
#if IF_USE_DOB_FONT
    m_pDobFont = NULL;
#endif

    setPointSize(pointSize);
    setFamily(family);
    setBold(bold);
#endif
}

xFont::xFont(const xFont& font)
{
#if IF_USE_DOB_FONT
#if IF_USE_FREETYPE_LIB
    if (g_WorkStation.IsUseDotLib())
    {
#endif
        m_pDobFont = new dobFont;

        if (NULL != m_pDobFont && NULL != font.m_pDobFont)
        {
            *m_pDobFont = *font.m_pDobFont;
        }

#if IF_USE_FREETYPE_LIB
        return;
    }
#endif
#endif

#if IF_USE_FREETYPE_LIB
#if IF_USE_DOB_FONT
    m_pDobFont = NULL;
#endif

    m_nPointSize = font.m_nPointSize;
    m_strFamily = font.m_strFamily;
    m_bBold = font.m_bBold;
#endif
}

xFont::~xFont()
{
#if IF_USE_DOB_FONT
    if (NULL != m_pDobFont)
    {
        delete m_pDobFont;
        m_pDobFont = NULL;
    }
#endif
}

xFont & xFont::operator = (const xFont& src)
{
#if IF_USE_DOB_FONT
    if (NULL != m_pDobFont && NULL != src.m_pDobFont)
    {
        *m_pDobFont = *src.m_pDobFont;
    }
#endif

#if IF_USE_FREETYPE_LIB
    m_nPointSize = src.m_nPointSize;
    m_strFamily = src.m_strFamily;
    m_bBold = src.m_bBold;
#endif

    return *this;
}

void xFont::SetDefaultSize(int nDefaultSize)
{
    s_nDefaultSize = nDefaultSize;
}

void xFont::setPointSize(int pointSize)
{
#if IF_USE_DOB_FONT
    if (NULL != m_pDobFont)
    {
        m_pDobFont->setPixelSize(pointSize);

        return;
    }
#endif

#if IF_USE_FREETYPE_LIB
    m_nPointSize = pointSize;
#endif
}

void xFont::setFamily(const yl::string& family)
{
#if IF_USE_DOB_FONT
    if (NULL != m_pDobFont)
    {
        m_pDobFont->setFamily(family);
    }
#endif

#if IF_USE_FREETYPE_LIB
    m_strFamily = family;
#endif
}

void xFont::setBold(bool bBold)
{
#if IF_USE_FREETYPE_LIB
    m_bBold = bBold;
#endif
}

int xFont::getPointSize() const
{
#if IF_USE_DOB_FONT
    if (NULL != m_pDobFont)
    {
        return m_pDobFont->getPixelSize();
    }

    return 0;
#endif

#if IF_USE_FREETYPE_LIB
    return m_nPointSize;
#endif
}

chSize xFont::textBound(const yl::string& text) const
{
    chSize szText(0, 0);

    if (text.empty())
    {
        return szText;
    }

#if IF_USE_DOB_FONT
    if (NULL != m_pDobFont)
    {
        return m_pDobFont->textBound(text.c_str());
    }
#endif

#if IF_USE_FREETYPE_LIB
    szText = FreeType_TextOutBound(*this, text);
#endif

    return szText;
}

//////////////////////////////////////////////////////////////////////////
// class xBrush
xBrush::xBrush()
{
    m_bIsNull = true;
}

xBrush::~xBrush()
{
    m_bIsNull = true;
}

xBrush::xBrush(const xPixmap& pixmap)
{
    setPixmap(pixmap);
}

xBrush::xBrush(const xColor& color)
{
    setColor(color);
}

xBrush::xBrush(const xBrush& src)
    : m_objPixmap(src.m_objPixmap)
    , m_objColor(src.m_objColor)
    , m_bIsNull(src.m_bIsNull)
{
}

xBrush & xBrush::operator = (const xBrush& src)
{
    m_objPixmap = src.m_objPixmap;
    m_objColor = src.m_objColor;
    m_bIsNull = src.m_bIsNull;

    return *this;
}

bool xBrush::isNull() const
{
    return m_bIsNull;
}

bool xBrush::isColor() const
{
    if (m_bIsNull)
    {
        return false;
    }

    return m_objPixmap.isNull();
}

#if IF_GRAPHIC_MARK_ALPHA_CHANNEL
bool xBrush::isTransparent() const
{
    return m_bIsNull || (!m_objPixmap.isNull() && m_objPixmap.hasAlphaChannel());
}
#endif

void xBrush::setPixmap(const xPixmap& pixmap)
{
    m_objPixmap = pixmap;
    m_bIsNull = m_objPixmap.isNull();
}

void xBrush::setColor(const xColor& color)
{
    m_objColor = color;
    m_objPixmap.setNull();
    m_bIsNull = false;
}

const xColor & xBrush::GetColor() const
{
    return m_objColor;
}

xPixmap & xBrush::pixmap()
{
    return m_objPixmap;
}

//////////////////////////////////////////////////////////////////////////
// class xPainter
xPainter::xPainter(xScreen* pScreen, const chPoint& ptOffset, const xRegion& region)
    : m_pScreen(pScreen)
    , m_ptViewOffset(ptOffset)
    , m_regionClip(region)
{
#if IF_DEBUG_PAINTER
    chRect rcClip = m_regionClip.bound();
    XWINDOW_INFO("xPainter ClipRegion:%d %d %d %d ViewOffset:%d %d.",
                 rcClip.left, rcClip.top, rcClip.right, rcClip.bottom,
                 m_ptViewOffset.x, m_ptViewOffset.y);
#endif

    if (NULL != m_pScreen)
    {
        m_pScreen->SetClipRegion(region, true);
    }
}

xPainter::xPainter(xScreen* pScreen, xView* pView, const xRegion& region)
{
    m_pScreen = pScreen;

    xRegion rgnClip;

    if (NULL != pView)
    {
        chRect rect = pView->m_rectLayout;

        rgnClip = region & OffsetRect(rect, -rect.left, -rect.top);
    }

    m_regionClip = rgnClip;

#if IF_DEBUG_PAINTER
    chRect rcClip = rgnClip.bound();
    XWINDOW_INFO("xPainter View:%p ClipRegion:%d %d %d %d ViewOffset is zero.",
                 pView, rcClip.left, rcClip.top, rcClip.right, rcClip.bottom);
#endif

    if (NULL != pScreen)
    {
        pScreen->SetClipRegion(rgnClip, true);
    }

    m_ptViewOffset = chPoint(0, 0);
}

xPainter::~xPainter()
{
    if (NULL != m_pScreen)
    {
        m_pScreen = NULL;
    }
}

void xPainter::setClipRect(const chRect& rect, int nClipOperation, bool bMarkDirty/* = false*/)
{
    if (NULL == m_pScreen)
    {
        return;
    }

    switch (nClipOperation)
    {
    case CHIP_OPERATION_NO_CHIP:
        {
            m_regionClip = chRect(0, 0, m_pScreen->Width(), m_pScreen->Height());
        }
        break;
    case CHIP_OPERATION_REPLACE:
        {
            m_regionClip = rect;
        }
        break;
    case CHIP_OPERATION_INTERSECT:
        {
            m_regionClip &= rect;
        }
        break;
    case CHIP_OPERATION_UNITE:
        {
            m_regionClip |= rect;
        }
        break;
    default:
        {
            return;
        }
        break;
    }

    m_pScreen->SetClipRegion(m_regionClip, bMarkDirty);
}

const xRegion & xPainter::GetClipRegion() const
{
    return m_regionClip;
}

void xPainter::SetClipRegion(const xRegion& rgnClip)
{
    if (NULL == m_pScreen)
    {
        return;
    }

    m_regionClip = rgnClip;

    m_pScreen->SetClipRegion(m_regionClip);
}

void xPainter::setLineType(int nType)
{
    if (NULL == m_pScreen)
    {
        return;
    }

    m_pScreen->SetLineType(nType);
}

void xPainter::setLineWidth(int nWidth)
{
    if (NULL != m_pScreen)
    {
        m_pScreen->SetLineWidth(nWidth);
    }
}

bool xPainter::IsInvalidDrawRgn(const chRect& rcDraw)
{
    xRegion rgnValid = m_regionClip & rcDraw;

    return rgnValid.empty();
}

void xPainter::drawItemPixmap(const chRect& rect, xPixmap& pixmap)
{
    if (pixmap.isNull())
    {
        return;
    }

#if IF_PIXMAP_PARTLY_SHARED
    drawItemPixmapEx(rect, pixmap, pixmap.GetPixmapRect());
#else
    chRect rcPixmap(0, 0, pixmap.width(), pixmap.height());

    drawItemPixmapEx(rect, pixmap, rcPixmap);
#endif
}

void xPainter::drawItemPixmapEx(const chRect& rect, xPixmap& pixmap, const chRect& srcRect)
{
    if (pixmap.isNull() || NULL == m_pScreen)
    {
        return;
    }

    chRect rcScreen = OffsetRect(rect, m_ptViewOffset.x, m_ptViewOffset.y);

    if (IsInvalidDrawRgn(rcScreen))
    {
        return;
    }

    m_pScreen->DrawPixmap(rcScreen, pixmap, srcRect);
}

void xPainter::fillRoundPixmap(const chRect& rect, xPixmap& src)
{

}

void xPainter::fillExpandPixmap(const chRect& rect, xPixmap& src)
{

}

void xPainter::drawText(const chRect& rcTextBound, const char* pszText, int nAlign,
                        const xFont& font, const xColor& color)
{
    if (NULL == pszText || NULL == m_pScreen)
    {
        return;
    }

    chRect rcScreen = OffsetRect(rcTextBound, m_ptViewOffset.x, m_ptViewOffset.y);

    if (IsInvalidDrawRgn(rcScreen))
    {
        return;
    }

    m_pScreen->DrawText(rcScreen, pszText, nAlign, font, color);
}

void xPainter::drawTextEx(const chRect& rcTextBound, const char* pszText, int nAlign,
                          chPoint& ptOffset, const xFont& font, const xColor& color)
{
    if (NULL == pszText || NULL == m_pScreen)
    {
        return;
    }

    chRect rcScreen = OffsetRect(rcTextBound, m_ptViewOffset.x, m_ptViewOffset.y);

    if (IsInvalidDrawRgn(rcScreen))
    {
        return;
    }

    m_pScreen->DrawTextEx(rcScreen, pszText, nAlign, ptOffset, font, color);
}

void xPainter::fillRect(const chRect& rect, xBrush& brush)
{
    if (brush.isNull())
    {
        return;
    }

    if (!brush.m_objPixmap.isNull())
    {
        fillRect(rect, brush.m_objPixmap);
    }
    else
    {
        fillRect(rect, brush.m_objColor);
    }
}

#if IF_PIXEL_ALPHA
void xPainter::blendRect(const chRect& rect, xBrush& brush)
{
    if (brush.isNull())
    {
        return;
    }

    if (!brush.m_objPixmap.isNull())
    {
        blendRect(rect, brush.m_objPixmap);
    }
    else
    {
        blendRect(rect, brush.m_objColor);
    }
}

void xPainter::blendRect(const chRect& rect, xPixmap& pixmap)
{
    if (pixmap.isNull() || NULL == m_pScreen)
    {
        return;
    }

    chRect rcScreen = OffsetRect(rect, m_ptViewOffset.x, m_ptViewOffset.y);

    if (IsInvalidDrawRgn(rcScreen))
    {
        return;
    }

    m_pScreen->BlendRect(rcScreen, pixmap);
}

void xPainter::blendRect(const chRect& rect, const xColor& color)
{
    if (NULL == m_pScreen)
    {
        return;
    }

    chRect rcScreen = OffsetRect(rect, m_ptViewOffset.x, m_ptViewOffset.y);

    if (IsInvalidDrawRgn(rcScreen))
    {
        return;
    }

    m_pScreen->BlendRect(rcScreen, color);
}
#endif

void xPainter::fillRect(const chRect& rect, xPixmap& pixmap)
{
    if (pixmap.isNull() || NULL == m_pScreen)
    {
        return;
    }

    chRect rcScreen = OffsetRect(rect, m_ptViewOffset.x, m_ptViewOffset.y);

    if (IsInvalidDrawRgn(rcScreen))
    {
        return;
    }

    m_pScreen->FillRect(rcScreen, pixmap);
}

void xPainter::fillRect(const chRect& rect, const xColor& color)
{
    if (NULL == m_pScreen)
    {
        return;
    }

    chRect rcScreen = OffsetRect(rect, m_ptViewOffset.x, m_ptViewOffset.y);

    if (IsInvalidDrawRgn(rcScreen))
    {
        return;
    }

    m_pScreen->FillRect(rcScreen, color);
}

#if IF_ROUND_RECTANGLE
void xPainter::fillRoundedRect(const chRect& rect, int nRoundPixel, const xColor& color,
                               int nCornerType/* = CORNER_TYPE_ALL*/)
{
    if (NULL == m_pScreen)
    {
        return;
    }

    chRect rcScreen = OffsetRect(rect, m_ptViewOffset.x, m_ptViewOffset.y);

    if (IsInvalidDrawRgn(rcScreen))
    {
        return;
    }

    m_pScreen->FillRoundedRect(rcScreen, nRoundPixel, color, nCornerType);
}
#endif

void xPainter::drawRect(const chRect& rect, const xColor& color)
{
    if (NULL == m_pScreen)
    {
        return;
    }

    chRect rcScreen = OffsetRect(rect, m_ptViewOffset.x, m_ptViewOffset.y);

    if (IsInvalidDrawRgn(rcScreen))
    {
        return;
    }

    int nLineWidth = m_pScreen->GetLineWidth();

    chPoint ptTopLeft(rcScreen.left, rcScreen.top);
    chPoint ptTopRight(rcScreen.right - nLineWidth, rcScreen.top);
    chPoint ptBottomLeft(rcScreen.left, rcScreen.bottom - nLineWidth);
    chPoint ptBottomRight(rcScreen.right - nLineWidth, rcScreen.bottom - nLineWidth);

    m_pScreen->DrawLine(ptTopLeft, ptTopRight, color);
    m_pScreen->DrawLine(ptTopLeft, ptBottomLeft, color);
    m_pScreen->DrawLine(ptTopRight, ptBottomRight, color);

    ptBottomRight.x += nLineWidth;
    m_pScreen->DrawLine(ptBottomLeft, ptBottomRight, color);
}

#if IF_ROUND_RECTANGLE
void xPainter::drawRoundedRect(const chRect& rect, int nRoundPixel, const xColor& color,
                               int nCornerType/* = CORNER_TYPE_ALL*/)
{
    if (NULL == m_pScreen)
    {
        return;
    }

    chRect rcScreen = OffsetRect(rect, m_ptViewOffset.x, m_ptViewOffset.y);

    if (IsInvalidDrawRgn(rcScreen))
    {
        return;
    }

    m_pScreen->DrawRoundedRect(rcScreen, nRoundPixel, color, nCornerType);
}
#endif

#if IF_INVERT_EFFECT
void xPainter::invertRect(const chRect& rect)
{
    if (NULL == m_pScreen)
    {
        return;
    }

    chRect rcScreen = OffsetRect(rect, m_ptViewOffset.x, m_ptViewOffset.y);

    if (IsInvalidDrawRgn(rcScreen))
    {
        return;
    }

    m_pScreen->InvertRect(rcScreen);
}
#endif

void xPainter::drawLineX(int y, int x1, int x2, const xColor& color)
{
    if (NULL == m_pScreen)
    {
        return;
    }

    int nY = y + m_ptViewOffset.y;
    int nX1 = chMIN(x1, x2) + m_ptViewOffset.x;
    int nX2 = chMAX(x1, x2) + m_ptViewOffset.x;
    chPoint ptBegin(nX1, nY);
    chPoint ptEnd(nX2, nY);

    m_pScreen->DrawLine(ptBegin, ptEnd, color);
}

void xPainter::drawLineY(int x, int y1, int y2, const xColor& color)
{
    if (NULL == m_pScreen)
    {
        return;
    }

    int nX = x + m_ptViewOffset.x;
    int nY1 = chMIN(y1, y2) + m_ptViewOffset.y;
    int nY2 = chMAX(y1, y2) + m_ptViewOffset.y;
    chPoint ptBegin(nX, nY1);
    chPoint ptEnd(nX, nY2);

    m_pScreen->DrawLine(ptBegin, ptEnd, color);
}

#if IF_SCREEN_SET_PIXEL
void xPainter::drawPoint(int x, int y, const xColor& color)
{
    if (NULL != m_pScreen)
    {
        m_pScreen->drawPoint(x + m_ptViewOffset.x, y + m_ptViewOffset.y, color);
    }
}
#endif
