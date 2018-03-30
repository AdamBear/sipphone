#include "xtextdocument.h"
#include <ylstringalgorithm.h>
#include <ylalgorithm.h>
#include <xwindow/xwindowdefine.h>
#include "xwindow/xWindow.h"
#include "xwindow/xWindowManager.h"

#include "dsklog/log.h"

#if IF_EDITTEXTVIEW_MULTI_EDIT

using namespace yl;

namespace
{
const int DEFAULT_MAX_CHAR_ONE_LINE = 50;
const int DEFAULT_N                 = 2;
const int DEFAULT_LINE_HEIGHT       = 24;

const int g_iDashLineWidth = 2;

static const yl::wstring HIDEABLE_CHARS = L" \t\n";
static const yl::wstring CUTABLE_CHARS = HIDEABLE_CHARS + L"-,.;)}]!?>";
}

CTextDocument::CTextDocument(xEditTextView* pEditView /*= NULL*/)
    : m_fHVar(0)
    , m_fVVar(0)
    , m_fViewportV2VVarRate(0)
    , m_fViewportH2HVarRate(0)
    , m_fVVar2ViewportVRate(0)
    , m_fHVar2ViewportHRate(0)
    , m_pCursor(NULL)
    , m_iTopLeftLine(0)
    , m_nLineHeight(DEFAULT_LINE_HEIGHT)
    , m_nViewportWidth(0)
    , m_iLeftHorizontalPos(-1)
    , m_pEditView(pEditView)
    , m_bHasInit(false)
{
    m_pCursor = new CTextCursor();
    if (NULL != m_pEditView)
    {
        m_nViewportWidth = m_pEditView->contentRect().Width();
        // 设置行高
        SetLineHeight(m_pEditView->GetTextLineHeight());
    }
}

CTextDocument::~CTextDocument()
{
    m_lines.clear();
    if (NULL != m_pEditView)
    {
        m_pEditView = NULL;
    }
    if (NULL != m_pCursor)
    {
        delete m_pCursor;
        m_pCursor = NULL;
    }
}

void CTextDocument::SetEditView(xEditTextView* pEditView)
{
    m_pEditView = pEditView;
    if (NULL != m_pEditView)
    {
        m_nViewportWidth = m_pEditView->contentRect().Width();
        // 设置行高
        SetLineHeight(m_pEditView->GetTextLineHeight());
    }
}

///////////////////////////////////// 初始化 /////////////////////////////////////
void CTextDocument::InitText(const yl::wstring& strWText)
{
    m_strCacheWText = strWText;
    InitLines();
    // 更新光标到最后1个字符后面
    if (NULL != m_pCursor)
    {
        m_pCursor->SetCursorPos(strWText.size());
//      UpdateCursorData(strWText.size());
    }
}

void CTextDocument::InitLines()
{
    if (NULL == m_pEditView)
    {
        return;
    }

    m_lines.clear();

    if (!m_pEditView->GetMultiLineEnable())
    {
        int iSize = m_strCacheWText.size();
        if (0 != iSize)
        {
            m_lines.insert_single(0, iSize);
        }
        return;
    }

    m_nViewportWidth = m_pEditView->contentRect().Width();
    if (0 >= m_nViewportWidth)
    {
        return;
    }
    m_bHasInit = true;

    int iRow = 0;
    int iFirstCharPos = 0;
    bool bFirstLine = true;
    int iTextLength = m_strCacheWText.size();

    typedef YLVector<yl::wstring> LINES;
    YLVector<yl::wstring> vecLines = yl::string_split<yl::wstring::value_type>(m_strCacheWText, L"\n", true);

    for (LINES::iterator begin = vecLines.begin(); begin != vecLines.end(); ++begin)
    {
        yl::wstring& str = *begin;
        if (iFirstCharPos + str.size() < iTextLength)
        {
            str.push_back('\n');
        }

        // 开始一行行初始化
        while (!str.empty())
        {
            int nInsertIdx = 0;
            if (str.size() > DEFAULT_MAX_CHAR_ONE_LINE
                    || m_nViewportWidth < m_pEditView->font.textBound(yl::to_utf8(str)).cx)
            {
                int iLength = str.size();
                iLength = iLength > DEFAULT_MAX_CHAR_ONE_LINE ? DEFAULT_MAX_CHAR_ONE_LINE : iLength;
                // 二分查找合适位置
                yl::wstring::size_type lower_pos = BinarySearch(str, GetViewportWidth(), iLength);

                //分词换行
                yl::wstring strLine(str, 0, lower_pos);

                //一个字符都显示不下
                if (strLine.empty())
                {
                    break;
                }

                //如果结尾的字符不能换行
                if (CUTABLE_CHARS.find_first_of(strLine.back()) == yl::wstring::npos)
                {
                    yl::wstring::size_type cutPos = strLine.find_last_of(CUTABLE_CHARS);
                    if (cutPos != yl::wstring::npos)
                    {
                        strLine.resize(cutPos + 1);
                    }
                }

                str.erase(0, strLine.size());
                // record line data
                nInsertIdx = m_lines.insert_single(iFirstCharPos, strLine.size());
                // update count
                iFirstCharPos += strLine.size();

                // 记录行字符宽度
                m_lines.fragment(nInsertIdx)->m_iLineTextWidth = m_pEditView->font.textBound(yl::to_utf8(strLine)).cx;
#if IF_TEXT_DOC_DEBUG
                XWINDOW_INFO("----- firstCharPos[%d], numChars[%d], str[%s] ======"
                             , iFirstCharPos - strLine.size(), strLine.size(), strLine.c_str());
#endif
            }
            else
            {
                // record line data
                nInsertIdx = m_lines.insert_single(iFirstCharPos, str.size());
                // update count
                iFirstCharPos += str.size();

                // 记录行字符宽度
                m_lines.fragment(nInsertIdx)->m_iLineTextWidth = m_pEditView->font.textBound(yl::to_utf8(str)).cx;
#if IF_TEXT_DOC_DEBUG
                XWINDOW_INFO("----- firstCharPos[%d], numChars[%d], str[%s] ======"
                             , iFirstCharPos - str.size(), str.size(), str.c_str());
#endif

                // 设置首行
                if (bFirstLine)
                {
                    bFirstLine = false;
                    SetTopLeftLine(nInsertIdx);
#if IF_TEXT_DOC_DEBUG
                    XWINDOW_INFO("----- firstBlockIndex[%d] ======", nInsertIdx);
#endif
                }
                break;
            }
        }
    }
}

///////////////////////////////////// 水平、竖直偏移相关 /////////////////////////////////////
int CTextDocument::VerticalOffset()
{
    return m_fVVar;
}

void CTextDocument::SetVerticalOffset(float fVOff)
{
    m_fVVar = fVOff;
}

int CTextDocument::HorizoncalOffset()
{
    return (int)m_fHVar;
}

void CTextDocument::SetHorizoncalOffset(float fHOff)
{
    m_fHVar = fHOff;
}

float CTextDocument::GetViewportV2VVarRate()
{
    return 1.0;
}

float CTextDocument::GetVVar2ViewportVRate()
{
    return 1.0;
}

float CTextDocument::GetViewportH2HVarRate()
{
    return 1.0;
}

float CTextDocument::GetHVar2ViewportHRate()
{
    return 1.0;
}

const yl::wstring& CTextDocument::GetWStringToPaint()
{
    if (NULL != m_pEditView)
    {
        if (m_pEditView->m_strCachePaintString.empty())
        {
            UpdateCacheText();
        }

        return m_pEditView->m_strCachePaintString;
    }

    return m_strCacheWText;
}

void CTextDocument::UpdateCacheText()
{
    if (NULL != m_pEditView)
    {
        if (m_pEditView->m_strCachePaintString.empty())
        {
            m_pEditView->m_strCachePaintString = m_strCacheWText;

            xWorkStation* pHostStation = m_pEditView->m_pHostStation;

            if (NULL != pHostStation && m_pEditView->IsContainRTLChar())
            {
                pHostStation->GetRTLShowWText(m_pEditView->m_strCachePaintString);
            }

            yl::wstring strWText = m_strCacheWText;

            yl::wstring& pwdPrefix = m_pEditView->m_strPartitionPwdPrefix;
            bool bPartitionPwdEnabled = !pwdPrefix.empty();
            bPartitionPwdEnabled = bPartitionPwdEnabled
                                   && strWText.find(pwdPrefix, 0) == 0;
            //密码
            if (m_pEditView->m_bPassword || bPartitionPwdEnabled)
            {
                //全密码
                if (m_pEditView->m_bPassword)
                {
                    int iCur = -1;
                    CTextCursor* pCursor = GetCurCursor();
                    if (NULL != pCursor)
                    {
                        iCur = pCursor->GetCursorPos();
                    }

                    //所有ch变为*
                    for (yl::wstring::size_type i = 0; i < strWText.size(); ++i)
                    {
                        //当立即隐藏属性为true时，不显示刚输入的字符
                        //当前字符正在输入时不隐藏
                        bool bCurHideChar = (!m_pEditView->m_bPreInput && i != iCur)
                                            || (m_pEditView->m_bPreInput && i != iCur - 1);
                        if (m_pEditView->m_bSuddenPassword
                                && (L'\n' != strWText[i])
                                && bCurHideChar
                                || !m_pEditView->m_bShowingHideChar)
                        {
                            strWText[i] = L'*';
                        }
                    }
                }
                //部分密码
                else if (bPartitionPwdEnabled)
                {
                    yl::wstring::size_type pwdLength = min(m_pEditView->m_iPartitionPwdLength, strWText.size() - pwdPrefix.size());
                    strWText.replace(pwdPrefix.size(), pwdLength, pwdLength, '*');
                }

                m_pEditView->m_strCachePaintString = strWText;

                if (NULL != m_pCursor)
                {
                    UpdateCursorData(m_pCursor->GetCursorPos(), true);
                }
            }
        }
    }
}

bool CTextDocument::onPaintMultiEditText(xPainter& p)
{
    if (NULL == m_pEditView)
    {
        return false;
    }

    if (!m_bHasInit)
    {
        InitLines();
    }

    yl::wstring strPaintWString = GetWStringToPaint();
    chRect rcViewPort = m_pEditView->GetViewPort();
    bool bRTL = m_pEditView->IsContainRTLChar();

    if (bRTL)
    {
        DrawRTLContent(p, strPaintWString, rcViewPort);
    }
    else
    {
        DrawLTRContent(p, strPaintWString, rcViewPort);
    }

    if (m_pEditView->m_bEnabled)
    {
        chRect rcContent = m_pEditView->contentRect();

        //drawRect，invertRect没有ViewPort相关接口，先这里处理viewPort位移
        if (m_pEditView->m_bPreInput)
        {
            chRect rectImeChar = m_pEditView->GetImeCharRect();

            rectImeChar.OffsetRect(-rcViewPort.left, -rcViewPort.top);
            rectImeChar.OffsetRect(rcContent.left, rcContent.top);

#if IF_EDITTEXTVIEW_PRELINE_ENABLE
            int nY = rectImeChar.bottom;
            int nFirX = rectImeChar.left;
            int nSecX = rectImeChar.left + rectImeChar.Width() - 2;
            p.setLineType(LINE_TYPE_DASH);
            p.setLineWidth(g_iDashLineWidth);
            p.drawLineX(nY, nFirX, nSecX, m_pEditView->m_PreLineColor);
#else
#if IF_INVERT_EFFECT
            p.invertRect(rectImeChar);
#endif
#endif
        }
        else if (m_pEditView->m_bShowCursor)
        {
            //光标本体
            chRect rectCursor = m_pEditView->GetCursorRect();

            rectCursor.OffsetRect(-rcViewPort.left, -rcViewPort.top);
            rectCursor.OffsetRect(rcContent.left, rcContent.top);

            // 光标画跟文字同一个颜色
            p.drawLineY(rectCursor.left, rectCursor.top, rectCursor.bottom, m_pEditView->textColor);
        }
    }

    return true;
}


int CTextDocument::GetRTLCursorIndex(const yl::wstring& strPaintWString)
{
    if (NULL == m_pEditView || strPaintWString.empty())
    {
        return 0;
    }

    xWorkStation* pHostStation = m_pEditView->m_pHostStation;

    if (NULL == pHostStation)
    {
        return 0;
    }

    int nLen = strPaintWString.length();

    for (int i = 0; i < nLen; ++i)
    {
        if (pHostStation->IsRTLChar(strPaintWString[i]))
        {
            return i;
        }
    }

    return nLen - 1;
}

void CTextDocument::DrawLTRContent(xPainter& p, const yl::wstring& strPaintWString, const chRect& rcViewPort)
{
    if (NULL == m_pEditView)
    {
        return;
    }

    if (!m_pEditView->m_strText.empty())
    {
        int iSelPosSt = 0;
        int iSelPosEn = 0;

        CTextCursor* pCursor = GetCurCursor();
        if (NULL != pCursor)
        {
            pCursor->GetSelectArea(iSelPosSt, iSelPosEn);
#if IF_TEXT_DOC_DEBUG
            XWINDOW_INFO("------------- after get sel st[%d],en[%d] =====", iSelPosSt, iSelPosEn);
#endif
        }
        yl::string strNormal;
        yl::string strNormalTwo;
        yl::string strSelect;

        // 计算显示的第一行
        int iLineH = GetLineHeight();
        iLineH = (0 == iLineH) ? 1 : iLineH;
        int iFirstVisiableLine = rcViewPort.top / iLineH;
        // 开始绘制
        bool bVisiable = true;

        for (int iIndex = m_lines.findNode(iFirstVisiableLine, 1)
                          ; bVisiable && 0 != iIndex
                ; iIndex = m_lines.next(iIndex))
        {
            int iLineTop = iLineH * m_lines.position(iIndex, 1);
            // 计算画的字符是否超出viewport，超出则不再画
            if (iLineTop >= rcViewPort.bottom)
            {
                bVisiable = false;
                break;
            }
            // 获取一行字符绘制
            strNormal = "";
            strNormalTwo = "";
            strSelect = "";
            int iSt = m_lines.position(iIndex);
            int iEn = iSt + m_lines.size(iIndex);
#if IF_TEXT_DOC_DEBUG
            XWINDOW_INFO("---- line pos[%d], num[%d] ==========", iSt, iEn - iSt);
#endif
            if (iEn <= iSt)
            {
                continue;
            }
#if IF_EDITTEXTVIEW_SELECT_ENABLE
            // 计算选中和非选中区域
            if (iSt >= iSelPosEn)
            {
                strNormal = yl::to_utf8(strPaintWString.substr(iSt, iEn - iSt));
            }
            else if (iSt >= iSelPosSt)
            {
                int iMin = iEn > iSelPosEn ? iSelPosEn : iEn;
                strSelect = yl::to_utf8(strPaintWString.substr(iSt, iMin - iSt));
                if (iEn > iMin)
                {
                    strNormal = yl::to_utf8(strPaintWString.substr(iMin, iEn - iMin));
                }
            }
            else if (iSelPosEn <= iEn)
            {
                strNormal = yl::to_utf8(strPaintWString.substr(iSt, iSelPosSt - iSt));
                strSelect = yl::to_utf8(strPaintWString.substr(iSelPosSt, iSelPosEn - iSelPosSt));
                strNormalTwo = yl::to_utf8(strPaintWString.substr(iSelPosEn, iEn - iSelPosEn));
            }
            else
            {
                int iMin = iSelPosSt > iEn ? iEn : iSelPosSt;
                strNormal = yl::to_utf8(strPaintWString.substr(iSt, iMin - iSt));
                if (iMin < iEn)
                {
                    strSelect = yl::to_utf8(strPaintWString.substr(iMin, iEn - iMin));
                }
            }
#if IF_TEXT_DOC_DEBUG
            XWINDOW_INFO("---------- nor[%s], sel[%s], nor2[%s] ======="
                         , strNormal.c_str(), strSelect.c_str(), strNormalTwo.c_str());
#endif
            // 画文字
            int iLeft = GetLeftDistance(m_pEditView->contentRect().Width(), m_lines.fragment(iIndex)->m_iLineTextWidth);
            int iWidth = m_pEditView->font.textBound(strNormal).cx;
            int iContentWidth = m_pEditView->contentRect().Width();
#if IF_TEXT_DOC_DEBUG
            XWINDOW_INFO("----------------- textW[%d], contentW[%d] ======", iWidth, iContentWidth);
#endif
            iWidth = (iLeft + iWidth > iContentWidth) ? (iContentWidth - iLeft) : iWidth;
            if (!m_pEditView->GetMultiLineEnable())
            {
                chRect rcTemp(0, iLineTop, 0, iLineH);
                adjustRectByGravity(m_pEditView->contentRect(), m_pEditView->gravity, rcTemp);
                iLineTop = rcTemp.top;
            }
            chRect rcText = rectLTWH(iLeft, iLineTop, iWidth, iLineH);
#if IF_TEXT_DOC_DEBUG
            XWINDOW_INFO("----------------- text[%s] textRect(%d, %d, %d, %d) ======"
                         , strNormal.substr(0, strNormal.size() - 1).c_str(), iLeft, iLineTop, iWidth, iLineH);
#endif
            m_pEditView->DrawNormalText(p, rcText, strNormal);

            iLeft += iWidth;
            iWidth = m_pEditView->font.textBound(strSelect).cx;
            iWidth = (iLeft + iWidth > iContentWidth) ? (iContentWidth - iLeft) : iWidth;
            rcText = rectLTWH(iLeft, iLineTop, iWidth, iLineH);
            m_pEditView->DrawSelcetText(p, rcText, strSelect);

            iLeft += iWidth;
            iWidth = m_pEditView->font.textBound(strNormalTwo).cx;
            iWidth = (iLeft + iWidth > iContentWidth) ? (iContentWidth - iLeft) : iWidth;
            rcText = rectLTWH(iLeft, iLineTop, iWidth, iLineH);
            m_pEditView->DrawNormalText(p, rcText, strNormalTwo);
#else
            strNormal = yl::to_utf8(strPaintWString.substr(iSt, iEn - iSt));
            if (!m_pEditView->GetMultiLineEnable())
            {
                chRect rcTemp(0, iLineTop, 0, iLineH);
                adjustRectByGravity(m_pEditView->contentRect(), m_pEditView->gravity, rcTemp);
                iLineTop = rcTemp.top;
            }
            // 画文字
            int iWidth = m_pEditView->font.textBound(strNormal).cx;
            chRect rcText = rectLTWH(0, iLineTop, iWidth, iLineH);

            m_pEditView->DrawNormalText(p, rcText, strNormal);
#endif
        } // end for
    }
#if IF_EDIT_TEXT_VIEW_HINT_EMPTY
    else if (NULL != m_pEditView && !(m_pEditView->m_strHintText.empty()))  // 画提示文字
    {
        chRect rcNoteText(0, 0, m_pEditView->m_nHintWidth, GetLineHeight());
        yl::string strHint = m_pEditView->m_strHintText;
        if (m_pEditView->contentRect().Width() < m_pEditView->m_nHintWidth)
        {
            xFont font = m_pEditView->GetHintFont();
            yl::wstring strWHint = m_pEditView->GetEllipsisTextByFont(yl::to_utf16(strHint), font);
            strHint = yl::to_utf8(strWHint);
            rcNoteText.right = m_pEditView->contentRect().Width();
        }
        adjustRectByGravity(m_pEditView->contentRect(), m_pEditView->gravity, rcNoteText);
#if IF_TEXT_DOC_DEBUG
        XWINDOW_INFO("----------------- noteTextRect(%d, %d, %d, %d), gravity[%d] yC[%d], left[%d] ======"
                     , rcNoteText.left, rcNoteText.top, rcNoteText.Width(), rcNoteText.Height()
                     , m_pEditView->gravity, gravity_y_center, gravity_left);
#endif
        m_pEditView->DrawHintText(p, rcNoteText, strHint);
    }
#endif
}

void CTextDocument::DrawRTLContent(xPainter& p, const yl::wstring& strPaintWString, const chRect& rcViewPort)
{
    if (NULL == m_pEditView)
    {
        return;
    }

    chRect rcTextBound(chPoint(0, 0), m_pEditView->CachePaintTextSize());
    chRect rcContent = m_pEditView->contentRect();
    chPoint ptViewPort = rcViewPort.TopLeft();

    int nGravity = m_pEditView->gravity;
    int nAlign = m_pEditView->align;

    if (rcTextBound.Width() >= rcContent.Width())
    {
        nAlign = (nAlign & gravity_y_mask) | align_left;
        nGravity = (nGravity & gravity_y_mask) | gravity_left;
    }

    adjustRectByGravity(rcContent, nGravity, rcTextBound);

    rcTextBound = rcContent & rcTextBound;

    yl::string strContent = yl::to_utf8(strPaintWString);

    p.drawTextEx(rcTextBound, strContent.c_str(), nAlign, ptViewPort, m_pEditView->font, m_pEditView->textColor);
}

///////////////////////////////////// 属性相关 /////////////////////////////////////
void CTextDocument::SetLineHeight(int nLineHeight)
{
    m_nLineHeight = nLineHeight;
}

int CTextDocument::GetLineHeight()
{
    return m_nLineHeight;
}

void CTextDocument::SetViewportWidth(int nViewportWidth)
{
    m_nViewportWidth = nViewportWidth;
}

int CTextDocument::GetViewportWidth()
{
    return m_nViewportWidth;
}

void CTextDocument::SetTopLeftLine(int iTopLeftLine)
{
    m_iTopLeftLine = iTopLeftLine;
}

int CTextDocument::GetTopLeftLine()
{
    return m_iTopLeftLine;
}

void CTextDocument::SetLeftHorizontalPos(int iLeftHorizontalPos)
{
    m_iLeftHorizontalPos = iLeftHorizontalPos;
}

int CTextDocument::GetLeftHorizontalPos()
{
    return m_iLeftHorizontalPos;
}

void CTextDocument::SetContentRect(chRect& rcContent)
{
    m_rcContent = rcContent;
}

chRect CTextDocument::GetContentRect()
{
    return m_rcContent;
}

void CTextDocument::WTextString(const yl::wstring& strText)
{
    m_strCacheWText = strText;
}

const yl::wstring& CTextDocument::GetWTextString()
{
    return m_strCacheWText;
}

yl::wstring CTextDocument::GetStringByPos(int iPos, int iNum /*= 1*/)
{
    if (GetWTextString().empty())
    {
        return yl::to_utf16("");
    }
    else if (iPos > GetWTextString().size() - iNum)
    {
        iPos = GetWTextString().size() - iNum;
    }
    else if (iPos < 0)
    {
        iPos = 0;
    }

    return GetWTextString().substr(iPos, iNum);
}

bool CTextDocument::RemoveString(int iPos, int iNum /*= 1*/)
{
    if (m_strCacheWText.empty())
    {
        return false;
    }
    if (iPos < 0)
    {
        iPos = 0;
    }
    if (iPos > m_strCacheWText.size() - iNum)
    {
        iPos = m_strCacheWText.size() - iNum;
    }

    m_strCacheWText.erase(iPos, iNum);

    return true;
}

bool CTextDocument::InsertString(int iPos, yl::wstring& strInsertString)
{
    if (m_strCacheWText.empty())
    {
        m_strCacheWText = strInsertString;
    }
    else if (iPos > m_strCacheWText.size())
    {
        m_strCacheWText += strInsertString;
    }
    else
    {
        m_strCacheWText.insert(iPos, strInsertString);
    }

    return true;
}

bool CTextDocument::InsertString(int iPos, IMECHAR ch)
{
    int nSize = m_strCacheWText.size();
    if (iPos < 0 || iPos > nSize)
    {
        XWINDOW_INFO("[CTextDocument::InsertString] iPos[%d], nSize[%d]", iPos, nSize);
    }
    if (iPos > nSize)
    {
        iPos = nSize;
    }
    else if (iPos < 0)
    {
        iPos = 0;
    }
    m_strCacheWText.insert(iPos, 1, ch);
#if IF_TEXT_DOC_DEBUG
    XWINDOW_INFO("------------ after insert [%s] =========="
                 , yl::to_utf8(m_strCacheWText).c_str());
#endif
    return true;
}

bool CTextDocument::ReplaceString(int iPos, int iCnt, int iCntTwo, IMECHAR ch)
{
    m_strCacheWText.replace(iPos, iCnt, iCntTwo, ch);
    return true;
}

bool CTextDocument::IsWTextEmpty()
{
    return m_strCacheWText.empty();
}

int CTextDocument::TextLength()
{
    return m_strCacheWText.size();
}

///////////////////////////////////// 行操作 /////////////////////////////////////
// 更新显示的首块
void CTextDocument::UpdateFirstVisiableLine(int iYOff)
{
    int iLineIndex = (iYOff + GetLineHeight()) / GetLineHeight();
    SetTopLeftLine(m_lines.findNode(iLineIndex, 1));
}

void CTextDocument::Unite(int iPos)
{
    int iCur = m_lines.findNode(iPos);
    if (0 != iCur)
    {
        int iNext = m_lines.next(iCur);
        if (0 != iNext)
        {
            m_lines.setSize(iCur, m_lines.size(iCur) + m_lines.size(iNext));
            m_lines.erase_single(iNext);
        }
    }
}

void CTextDocument::Split(int iPos)
{
    int iCur = m_lines.findNode(iPos);
    if (0 != iCur)
    {
        int iLineStartPos = m_lines.position(iCur);
        if (iLineStartPos != iPos)
        {
            int nOldSize = m_lines.fragment(iCur)->size_array[0];
            m_lines.setSize(iCur, iPos - iLineStartPos);
            // 添加新的一行
            m_lines.insert_single(iPos, nOldSize - (iPos - iLineStartPos));
        }
    }
}

void CTextDocument::UpdateLineText(int iCharPos)
{
    // 焦点所在行的前一行开始更新，因为操作可能会影响上、下两行断行
    // eg:
    // Good good
    // studyd
    // （一行只能容纳15个字符，删除study后面的d，study会缩进到上一行中，减少一行）
    // Good good study

    // 不是多行，不需要更新
    if (NULL == m_pEditView || !m_pEditView->GetMultiLineEnable())
    {
        return;
    }

    int iOffset = 0;
    // 获取删除字符的前一行，开始更新
    int iIndex = m_lines.findNode(iCharPos);        // 当前行Map中ID
    if (iCharPos > 0 && iCharPos == TextLength())
    {
        iIndex = m_lines.findNode(iCharPos - 1);
    }
    if (0 >= iIndex)
    {
#if IF_TEXT_DOC_DEBUG
        XWINDOW_INFO("---------- pos[%d], no line exist ========", iCharPos);
#endif
        return;
    }
    int iNextIndex = m_lines.next(iIndex);          // 下一行Map中ID
    int nTotLines = m_lines.length(1);              // 总行数
    yl::wstring strWText;
    // 判断是否从前1行开始更新（若前1行有换行行、换段符，则前1行不用更新）
    if (0 != iIndex && m_lines.position(iIndex, 1) < nTotLines)
    {
        int iCurLine = m_lines.position(iIndex, 1);

        if (0 != iCurLine)
        {
            int iPreIndex = m_lines.previous(iIndex);
            if (0 != iPreIndex)
            {
                yl::wstring strWText = GetStringByPos(m_lines.position(iPreIndex), m_lines.size(iPreIndex));
                if (strWText.find(L"\n") == yl::wstring::npos)
                {
                    iNextIndex = iIndex;
                    iIndex = iPreIndex;
                }
            }
        }
    }
    // 开始更新
    yl::wstring strTextString = GetWTextString();
    bool bFirstLineUpdate = true;
    while (0 != iIndex)
    {
        int iSt = m_lines.position(iIndex);
        int iNum = m_lines.size(iIndex);                                        // 要计算偏移量
        int iFirstLineLength = m_lines.size(iIndex) + iOffset;                  // 首行字符数
        bool bCurLineSep = false;                                               // 当前行是否有换行符('\n')
#if IF_TEXT_DOC_DEBUG
        XWINDOW_INFO("------------- st[%d], size[%d], offset[%d], line[%d] ======"
                     , iSt, m_lines.size(iIndex), iOffset, m_lines.position(iIndex, 1));
#endif
        chASSERT(0 <= iFirstLineLength);
        strWText = strTextString.substr(iSt, iFirstLineLength);

        // 换行符查找
        int iSecondLineLength = 0;
        int iLength = iFirstLineLength;
        yl::wstring::size_type typePos = strWText.find_first_of(L"\n");
#if IF_TEXT_DOC_DEBUG
        XWINDOW_INFO("--------- string first line len[%d], findPos[%d] =====", iFirstLineLength, typePos);
#endif
        if (yl::wstring::npos != typePos)
        {
            iLength = (int)typePos + 1;     // 要包含'\n'字符
            bCurLineSep = true;
        }
        else if (0 != iNextIndex)
        {
            iSecondLineLength = m_lines.size(iNextIndex);
            if (0 == iSecondLineLength)
            {
                m_lines.erase_single(iNextIndex);
                iNextIndex = 0;
            }
            else
            {
                strWText += strTextString.substr(iSt + iFirstLineLength, iSecondLineLength);
#if IF_TEXT_DOC_DEBUG
                XWINDOW_INFO("--------------- second line str[%s]\noff[%d], size[%d], st[%d] ======="
                             , yl::to_utf8(strWText).c_str(), iOffset, m_lines.size(iNextIndex), m_lines.position(iNextIndex));
#endif
                typePos = strWText.find_first_of(L"\n");
                if (yl::wstring::npos != typePos)
                {
                    iLength = (int)typePos + 1;     // 要包含'\n'字符
                }
                else
                {
                    iLength = strWText.size();
                }
            }
        }

        // 二分查找断行位置
        yl::wstring::size_type lower_pos = BinarySearch(strWText, GetViewportWidth(), iLength);

        //分词换行
        yl::wstring strLine(strWText, 0, lower_pos);

        //一个字符都显示不下
        if (strLine.empty())
        {
            break;
        }
        int iNewNum = strLine.size();

        //如果结尾的字符不能换行
        if (iSt + lower_pos < TextLength()
                && CUTABLE_CHARS.find_first_of(strLine.back()) == yl::wstring::npos)
        {
            yl::wstring::size_type cutPos = strLine.find_last_of(CUTABLE_CHARS);
            if (cutPos != yl::wstring::npos)
            {
                strLine.resize(cutPos + 1);
                iNewNum = cutPos + 1;
            }
        }

        // 更新行信息
        m_lines.setSize(iIndex, iNewNum);
        // 更新行字符宽
        m_lines.fragment(iIndex)->m_iLineTextWidth = m_pEditView->font.textBound(yl::to_utf8(strLine)).cx;
        if (yl::wstring::npos != typePos)
        {
            // 新增1行
            if (bCurLineSep && iNewNum < iLength)
            {
#if IF_TEXT_DOC_DEBUG
                XWINDOW_INFO("##################### 1 add new line st[%d], num[%d] return ************"
                             , iSt + iNewNum, iLength - iNewNum);
#endif
                int iNewIndex = m_lines.insert_single(iSt + iNewNum, iLength - iNewNum);
                // 更新行字符宽
                if (0 != iNewNum)
                {
                    m_lines.fragment(iNewIndex)->m_iLineTextWidth = m_pEditView->font.textBound(yl::to_utf8(GetStringByPos(iSt + iNewNum, iLength - iNewNum))).cx;
                }
                break;
            }
            // 减少1行
            else if (iNewNum == iLength && 0 != iNextIndex && 0 != iSecondLineLength)
            {
#if IF_TEXT_DOC_DEBUG
                XWINDOW_INFO("##################### 2 erease line[%d] len[%d] ************"
                             , m_lines.position(iNextIndex) + 1, iSecondLineLength);
#endif
                m_lines.erase_single(iNextIndex);
                break;
            }
        }
        // 补差刚好，更新结束
        if (!bFirstLineUpdate && iNewNum == iFirstLineLength)
        {
#if IF_TEXT_DOC_DEBUG
            XWINDOW_INFO("##################### 3 iNew[%d] == iNum return ************"
                         , iNewNum);
#endif
            break;
        }
        // 最后一行
        if (m_lines.position(iIndex, 1) == nTotLines - 1 && iNewNum < iFirstLineLength)
        {
            // 增加1行
            int iNewIndex = m_lines.insert_single(iSt + iNewNum, iFirstLineLength - iNewNum);
            // 更新行字符宽
            if (0 != iNewNum)
            {
                m_lines.fragment(iNewIndex)->m_iLineTextWidth = m_pEditView->font.textBound(yl::to_utf8(GetStringByPos(iSt + iNewNum, iFirstLineLength - iNewNum))).cx;
            }
            break;
        }
        // 减少1行
        if (!bCurLineSep && iNewNum == iLength && 0 != iNextIndex && 0 != iSecondLineLength)
        {
#if IF_TEXT_DOC_DEBUG
            XWINDOW_INFO("##################### 4 erease line[%d] len[%d] ************"
                         , m_lines.position(iNextIndex) + 1, iSecondLineLength);
#endif
            m_lines.erase_single(iNextIndex);
            break;
        }
        // 更新信息
        bFirstLineUpdate = false;
        iOffset += iNum - iNewNum;
        iIndex = iNextIndex;
        if (0 != iIndex)
        {
            iNextIndex = m_lines.next(iIndex);
        }
    }
}

///////////////////////////////////// 光标操作 /////////////////////////////////////
CTextCursor* CTextDocument::GetCurCursor()
{
    return m_pCursor;
}

bool CTextDocument::SetCurCursor(CTextCursor& textCursor)
{
    if (NULL != m_pCursor)
    {
        int nTextLength = TextLength();
        int iNewCursorPos = (textCursor.GetCursorPos() > nTextLength) ? nTextLength : textCursor.GetCursorPos();
        m_pCursor->SetCursorPos(iNewCursorPos);
        m_pCursor->SetCursorLineIndex(textCursor.GetCursorLineIndex());
        m_pCursor->SetCursorCoordinate(textCursor.GetCursorCoordinate());
        return true;
    }
    return false;
}

void CTextDocument::UpdateCursorData(int iNewCursorPos, bool bForce /*= false*/)
{
    if (NULL != m_pCursor && NULL != m_pEditView
            && (bForce || iNewCursorPos != m_pCursor->GetCursorPos()))
    {
        int nTextLength = TextLength();
        if (0 == nTextLength)
        {
            m_pCursor->SetCursorPos(0);
            m_pCursor->SetCursorCoordinate(chPoint(0, 0));
            m_pCursor->SetCursorLineIndex(0);
            m_pCursor->SetPreCursorCharXPos(0);
            return;
        }

        if (iNewCursorPos > nTextLength)
        {
            XWINDOW_INFO("[CTextDocument::UpdateCursorData] newCursorPos[%d] more than cacheSize[%d]", iNewCursorPos, nTextLength);
            iNewCursorPos = nTextLength;
        }
        UpdateCacheText();
        m_pCursor->SetCursorPos(iNewCursorPos);

        uint iCurLine = 0;
        if (iNewCursorPos == nTextLength)
        {
            iCurLine = m_lines.findNode(iNewCursorPos - 1);
        }
        else
        {
            iCurLine = m_lines.findNode(iNewCursorPos);
        }

        if (0 != iCurLine)
        {
            m_pCursor->SetCursorLineIndex(m_lines.position(iCurLine, 1));
            int iFirstLineCharPos = m_lines.position(iCurLine);
            if (iFirstLineCharPos <= iNewCursorPos && TextLength() >= iNewCursorPos)
            {
                yl::string strTemp = yl::to_utf8(GetWStringToPaint().substr(iFirstLineCharPos, iNewCursorPos - iFirstLineCharPos));
                chPoint posCursor;
                posCursor.x = m_pEditView->font.textBound(strTemp).cx;
                posCursor.y = m_pCursor->GetCursorLineIndex() * GetLineHeight();
                m_pCursor->SetCursorCoordinate(posCursor);
                // 更新光标前一字符X坐标
                int iXPos = posCursor.x;
                if (iNewCursorPos > iFirstLineCharPos)
                {
                    iXPos -= m_pEditView->font.textBound(yl::to_utf8(GetWStringToPaint().substr(iNewCursorPos - iFirstLineCharPos - 1, 1))).cx;
                }
                m_pCursor->SetPreCursorCharXPos(iXPos);
            }
        }
    }
}

int CTextDocument::GetMoveStep(int eOp)
{
    int iMoveStep = 0;

    CTextCursor* pCursor = GetCurCursor();
    if (NULL == pCursor || NULL == m_pEditView)
    {
        return iMoveStep;
    }

    int iCurLineIndex = pCursor->GetCursorLineIndex();
    int iIndex = m_lines.findNode(iCurLineIndex, 1);
    if (0 == iIndex)
    {
        return iMoveStep;
    }

    // 获取当前偏移值
    int iContentWidth = m_pEditView->contentRect().Width();
    int iXOff = m_iLeftHorizontalPos;
    if (0 > iXOff)
    {
        iXOff = pCursor->GetCursorCoordinate().x;
        iXOff += GetLeftDistance(iContentWidth, m_lines.fragment(iIndex)->m_iLineTextWidth);
        m_iLeftHorizontalPos = iXOff;
    }

    if (eOp == CURSOR_OP_MOVE_DOWN)
    {
        if (m_lines.length(1) - 1 == iCurLineIndex) // 最后一行，行数从0行开始数
        {
            // 移到最后一个字符
            iMoveStep = TextLength() - pCursor->GetCursorPos();
            m_iLeftHorizontalPos = -1;
        }
        else
        {
            // 下一行处理
            iIndex = m_lines.next(iIndex);
            if (0 != iIndex)
            {
                int iLeftBalance = GetLeftDistance(iContentWidth, m_lines.fragment(iIndex)->m_iLineTextWidth);
                iMoveStep = GetMoveStep(m_lines.position(iIndex), m_lines.size(iIndex), iXOff
                                        , pCursor->GetCursorPos(), iLeftBalance);
            }
        }
    }
    else if (eOp == CURSOR_OP_MOVE_UP)
    {
        if (0 == iCurLineIndex)
        {
            iMoveStep = -pCursor->GetCursorPos();
            m_iLeftHorizontalPos = -1;
        }
        else
        {
            // 上一行处理
            iIndex = m_lines.previous(iIndex);
            if (0 != iIndex)
            {
                int iLeftBalance = GetLeftDistance(iContentWidth, m_lines.fragment(iIndex)->m_iLineTextWidth);
                iMoveStep = GetMoveStep(m_lines.position(iIndex), m_lines.size(iIndex), iXOff
                                        , pCursor->GetCursorPos(), iLeftBalance);
            }
        }
    }

    return iMoveStep;
}

int CTextDocument::GetMoveStep(int iCharPos, int iCharNum, int iXOff, int iCursorPos, int iLeftBalance)
{
    if (NULL == m_pEditView)
    {
        return 0;
    }
    // 不是最后一行的话，计算到最后第二个字符（避免最后一个字符会跑到第二行去）
    if (TextLength() != iCharPos + iCharNum)
    {
        iCharNum = iCharNum > 0 ? (iCharNum - 1) : iCharNum;
    }
    yl::wstring strLineText = GetStringByPos(iCharPos, iCharNum);
    //二分查找合适位置最左位置
    yl::wstring::size_type lower_pos = BinarySearch(strLineText, iXOff, iCharNum, iLeftBalance);

    return (iCharPos + lower_pos - iCursorPos);
}

chRect CTextDocument::GetCursorRect()
{
    CTextCursor* pCursor = GetCurCursor();
    if (NULL == pCursor || NULL == m_pEditView)
    {
        return rectLTWH(0, 0, 1, GetLineHeight());
    }

    int iLeft = pCursor->GetCursorCoordinate().x;
    int iCharPos = pCursor->GetCursorPos();
    if (iCharPos == TextLength() && 0 != iCharPos)
    {
        -- iCharPos;
    }
    int iCurIndex = m_lines.findNode(iCharPos);
    if (0 != iCurIndex)
    {
        int iSt = m_lines.position(iCurIndex);
        int iEn = iSt + m_lines.size(iCurIndex);
        iLeft += GetLeftDistance(m_pEditView->contentRect().Width()
                                 , m_pEditView->font.textBound(GetPaintStringByPos(iSt, iEn - iSt)).cx);
    }

    iLeft = iLeft - m_pEditView->paddingLeft;


    chRect rcCursor = rectLTWH(iLeft, pCursor->GetCursorCoordinate().y, 1, GetLineHeight());
    if (!m_pEditView->GetMultiLineEnable())
    {
        chRect rcTemp = rcCursor;
        adjustRectByGravity(m_pEditView->contentRect(), m_pEditView->gravity, rcTemp);
        rcCursor.top = rcTemp.top;
        rcCursor.bottom = rcTemp.bottom;
    }

#if IF_TEXT_DOC_DEBUG
    XWINDOW_INFO("-------- getCursor pos[%d], left[%d], top[%d] ==========="
                 , pCursor->GetCursorPos(), rcCursor.left, rcCursor.top);
#endif
    return rcCursor;
}

chRect CTextDocument::GetRTLCursorRect()
{
    if (NULL == m_pEditView)
    {
        return rectLTWH(0, 0, 1, GetLineHeight());
    }

    chRect rcContent = m_pEditView->contentRect();
    chRect rcCursor = rectLTWH(0, 0, 1, GetLineHeight());

    adjustRectByGravity(rcContent, m_pEditView->gravity, rcCursor);

    yl::wstring strPaintWString = GetWStringToPaint();
    int nLength = strPaintWString.length();

    if (nLength <= 0)
    {
        return rcCursor;
    }

    chSize szText = m_pEditView->font.textBound(yl::to_utf8(strPaintWString));
    int nContentWidth = rcContent.Width();

    if (nContentWidth > szText.cx)
    {
        rcCursor.OffsetRect(-szText.cx, 0);
    }
    else
    {
        rcCursor.OffsetRect(rcContent.left - rcCursor.left, 0);
    }

    int nCursorPos = GetRTLCursorIndex(strPaintWString);

    if (nCursorPos <= 0)
    {
        return rcCursor;
    }

    if (nCursorPos >= nLength)
    {
        nCursorPos = nLength - 1;
    }

    yl::string strRTL = yl::to_utf8(strPaintWString.substr(0, nCursorPos));
    chSize szRTL = m_pEditView->font.textBound(strRTL);

    rcCursor.OffsetRect(szRTL.cx, 0);

    return rcCursor;
}

chRect CTextDocument::GetImeCharRect()
{
    CTextCursor* pCursor = GetCurCursor();
    if (NULL == pCursor || NULL == m_pEditView)
    {
        return rectLTWH(0, 0, 1, GetLineHeight());
    }
    chASSERT(pCursor->GetCursorPos() <= m_strCacheWText.size());

    chSize imeCharSize;
    chRect rcContent = m_pEditView->contentRect();

    rcContent.OffsetRect(-rcContent.left, -rcContent.top);

    int iCursorPos = m_pEditView->GetCursorPos();

    int iLeft = pCursor->GetCursorCoordinate().x;
    int iTop = pCursor->GetCursorCoordinate().y;

    imeCharSize = m_pEditView->font.textBound(" ");
    if (!IsWTextEmpty())
    {
        int iPos = iCursorPos;
        int iCurIndex = (iPos == TextLength() && iPos > 0) ? m_lines.findNode(iPos - 1) : m_lines.findNode(iPos);
        int iFirstCharPos = -1;
        if (0 != iCurIndex)
        {
            iFirstCharPos = m_lines.position(iCurIndex);
            iPos = (iPos > iFirstCharPos) ? (iPos - 1) : iPos;
        }
        yl::string firstChar = GetPaintStringByPos(iPos, 1);
        imeCharSize = m_pEditView->font.textBound(firstChar);
        if (-1 != iFirstCharPos)
        {
            iLeft = m_pEditView->font.textBound(GetPaintStringByPos(iFirstCharPos, iPos - iFirstCharPos)).cx;
            int iSt = m_lines.position(iCurIndex);
            int iEn = iSt + m_lines.size(iCurIndex);
            iLeft += GetLeftDistance(m_pEditView->contentRect().Width()
                                     , m_pEditView->font.textBound(GetPaintStringByPos(iSt, iEn - iSt)).cx);
        }
    }

#if IF_TEXT_DOC_DEBUG
    XWINDOW_INFO("-------- pos[%d], size(%d, %d), left[%d], top[%d] ==========="
                 , pCursor->GetCursorPos(), imeCharSize.cx, imeCharSize.cy, iLeft, iTop);
#endif

    iLeft = (iLeft > 0) ? (iLeft - 1) : iLeft;

    chRect rcImeChar = rectLTWH(iLeft, iTop, imeCharSize.cx, imeCharSize.cy);//GetLineHeight());
    if (!m_pEditView->GetMultiLineEnable())
    {
        chRect rcTemp = rcImeChar;
        adjustRectByGravity(m_pEditView->contentRect(), m_pEditView->gravity, rcTemp);
        rcImeChar.top = rcTemp.top;
        rcImeChar.bottom = rcTemp.bottom;
    }
    return rcImeChar;
}

chRect CTextDocument::GetRTLImeCharRect()
{
    yl::wstring strPaintWString = GetWStringToPaint();
    int nCursorIndex = GetRTLCursorIndex(strPaintWString);
    int nLength = strPaintWString.length();

    if (nCursorIndex >= nLength || NULL == m_pEditView)
    {
        return rectLTWH(0, 0, 1, GetLineHeight());
    }

    yl::string strChar = GetPaintStringByPos(nCursorIndex, 1);
    chSize szIMEChar;

    if (strChar.empty())
    {
        szIMEChar = m_pEditView->font.textBound(" ");
    }
    else
    {
        szIMEChar = m_pEditView->font.textBound(strChar);
    }

    chRect rcCursor = GetRTLCursorRect();
    chRect rcImeChar(rcCursor.left, 0, rcCursor.left + szIMEChar.cx, szIMEChar.cy);

    return rcImeChar;
}

//////////////////////////////////////////////////////////////////////////////////
void CTextDocument::AdjustVerticalHorizonOff()
{
    if (NULL != m_pEditView)
    {
        chRect rcViewport = m_pEditView->GetViewPort();

        //      float fV = rcViewport.top;//(rcViewport.top * m_pTextDocument->GetViewportV2VVarRate());
        //      m_pTextDocument->SetVerticalOffset(fV);
        //      float fH = rcViewport.left;//(rcViewport.left * m_pTextDocument->GetViewportV2VVarRate());
        //      m_pTextDocument->SetHorizoncalOffset(fH);
        //      m_pTextDocument->UpdateFirstVisiableLine(rcViewport.top);
    }
}

void CTextDocument::SetVerticalHorizonOff(int iV, int iH)
{
    SetVerticalOffset(iV);
    SetHorizoncalOffset(iH);
    AdjustViewportOff();
}

void CTextDocument::AdjustViewportOff()
{
    //      int iV = (int)m_pTextDocument->VerticalOffset();//(m_pTextDocument->GetVVar2ViewportVRate() * m_pTextDocument->VerticalOffset());
    //      int iH = (int)m_pTextDocument->HorizoncalOffset();//(m_pTextDocument->GetHVar2ViewportHRate() * m_pTextDocument->HorizoncalOffset());
    //      m_LastViewPort.OffsetRect(iH - m_LastViewPort.left, iV - m_LastViewPort.top);
    //      m_pTextDocument->UpdateFirstVisiableLine(m_LastViewPort.top);
}

int CTextDocument::GetLeftDistance(int iContentWidth, int iTextWidth)
{
    if (iContentWidth <= iTextWidth || NULL == m_pEditView)
    {
        if (NULL != m_pEditView)
        {
            return m_pEditView->paddingLeft;
        }
        return 0;
    }

    switch (m_pEditView->gravity & gravity_x_mask)
    {
    case gravity_left:
    {
        if (NULL != m_pEditView)
        {
            return m_pEditView->paddingLeft;
        }
        return 0;
    }
    break;
    case gravity_x_center:
    {
        // 1 pixel offset to right with odd cols
        int diff = iContentWidth - iTextWidth;
        return (diff / 2 + diff % 2);
    }
    break;
    case gravity_right:
    {
        return (iContentWidth - iTextWidth);
    }
    break;
    default:
    {
        if (NULL != m_pEditView)
        {
            return m_pEditView->paddingLeft;
        }
        return 0;
    }
    }

    if (NULL != m_pEditView)
    {
        return m_pEditView->paddingLeft;
    }
    return 0;
}

// 二分查找靠左位置
int CTextDocument::BinarySearch(const yl::wstring& strWText, int iRegisterWidth, int iUpperLimit, int iLeftBlank /*= 0*/)
{
    yl::wstring::size_type upper_pos = iUpperLimit;
    yl::wstring::size_type lower_pos = 0;
    yl::wstring::size_type mid_pos = 0;
    yl::string paintText;
    // 二分查找合适位置
    while (lower_pos != (mid_pos = (upper_pos + lower_pos) / 2))
    {
        paintText = yl::to_utf8(strWText.substr(0, mid_pos));
        int nTextSize = m_pEditView->font.textBound(paintText).cx + iLeftBlank;

        if (nTextSize <= iRegisterWidth)
        {
            lower_pos = mid_pos;
        }
        else
        {
            upper_pos = mid_pos;
        }
    }
    if (upper_pos > lower_pos
            && m_pEditView->font.textBound(yl::to_utf8(strWText.substr(0, upper_pos))).cx <= iRegisterWidth)
    {
        lower_pos = upper_pos;
    }

    return lower_pos;
}

void CTextDocument::AdjustWidgetSize()
{
    if (NULL != m_pEditView)
    {
        m_nViewportWidth = m_pEditView->clientRect().Width();
        // 设置行高
        SetLineHeight(m_pEditView->GetTextLineHeight());
    }
}

yl::string CTextDocument::GetPaintStringByPos(int iPos, int iNum /*= 1*/)
{
    yl::wstring strWPaintString = GetWStringToPaint();
    if (strWPaintString.empty())
    {
        return "";
    }
    else if (iPos + iNum > strWPaintString.size())
    {
        iPos = (strWPaintString.size() < iNum) ? 0 : (strWPaintString.size() - iNum);
    }
    else if (iPos < 0)
    {
        iPos = 0;
    }

    return yl::to_utf8(strWPaintString.substr(iPos, iNum));
}

#endif // IF_EDITTEXTVIEW_MULTI_EDIT
