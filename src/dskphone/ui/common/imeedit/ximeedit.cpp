#include "ximeedit.h"
#include "ylime.h"
#include "translateiddefine.h"

#include <common/common_data_define.h>
#include <xwindow/xWindowManager.h>
#if IF_EDIT_TEXT_VIEW_NEED_SOFTKEY
#include <xwindow/xSoftkeyBar.h>
#endif
#include <keymap.h>
#include <ylstringalgorithm.h>
#include "imecommon.h"
#if IF_EDITTEXTVIEW_MULTI_EDIT
#include "xtextdocument.h"
#include "multitextcommonfunc.h"
#include <xwindow/xwindowdefine.h>
#endif
#if IF_EDIT_TEXT_VIEW_BIND_KEYBOARD
#include "keyboard/include/modkeyboard.h"
#endif

#include "dsklog/log.h"
#include "xwindow/activefonttext.h"
#include "xwindow/modviewdump.h"

using namespace yl;

namespace
{
#define MAX_LOOP_TIME       9999

#define mapLines            m_pTextDocument->GetTextLineMap()

const int DEFAULT_CURSOR_HEIGHT = 26;
// 内容提示语文字颜色
#define NOTE_TEXT_COLOR     (xColor(0x68, 0x68, 0x68))
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
EXPORT_VIEW_CLASS(xEditTextView);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
DECLARE_VIEW_DUMP_BEGIN(xEditTextView)
DECLARE_VIEW_DUMP_PROPERTY_DATA(enable, m_bEnabled, VIEW_DUMP_DATA_TYPE_BOOL)
DECLARE_VIEW_DUMP_PROPERTY_DATA(password, m_bPassword, VIEW_DUMP_DATA_TYPE_BOOL)
DECLARE_VIEW_DUMP_PROPERTY_DATA(suddenpassword, m_bSuddenPassword, VIEW_DUMP_DATA_TYPE_BOOL)
DECLARE_VIEW_DUMP_PROPERTY_METHOD(ime, GetImeName())
DECLARE_VIEW_DUMP_END

IMPLEMENT_VIEW_DUMP_BEGIN(xEditTextView, xTextView)
IMPLEMENT_VIEW_DUMP_PROPERTY(enable)
IMPLEMENT_VIEW_DUMP_PROPERTY(password)
IMPLEMENT_VIEW_DUMP_PROPERTY(suddenpassword)
IMPLEMENT_VIEW_DUMP_PROPERTY(ime)
IMPLEMENT_VIEW_DUMP_END(xEditTextView)

FUN_INSTALL_IME xEditTextView::s_pFunInstallIME = NULL;

xEditTextView::xEditTextView()
    : m_bEnabled(true)
    , m_bShowCursor(false)
    , m_bPreInput(false)
#if IF_EDIT_TEXT_VIEW_SCROLL
    , m_bInScroll(false)
#endif
    , m_iCursorPos(0)
    , m_nImeSoftkeyIndex(-1)
    , m_nDeleteSoftkeyIndex(-1)
    , m_bPassword(false)
    , m_bSuddenPassword(false)
    , m_bShowingHideChar(false)
    , m_iPartitionPwdLength(0)
#if IF_EDITTEXTVIEW_PRELINE_ENABLE
    , m_PreLineColor(xColor(0, 0, 0))
#endif
#if IF_EDITTEXTVIEW_SELECT_ENABLE
    , m_SelectBgColor(xColor(0, 0, 238))
    , m_SelectTextColor(xColor(255, 255, 255))
    , m_nSelectIndex(0)
    , m_nSelectNum(0)
#endif
#if IF_DATETIMEEDIT_FOCUS_SWITCH_ENABLE
    , m_bKeyPressedEnable(true)
#endif
    , m_nMaxByte(-1)
    , m_nMaxLength(-1)
#if IF_EDITTEXTVIEW_MULTI_EDIT
    , m_pTextDocument(NULL)
#endif
#if IF_EDIT_TEXT_VIEW_HINT_EMPTY
    , m_strHintText("")
    , m_nHintWidth(0)
    , m_nHintFontSize(0)
    , m_clrHintText(xColor(255, 255, 255))
#endif
#if IF_EDIT_TEXT_VIEW_BIND_KEYBOARD
    , m_bKeyBoardShow(false)
#endif
{
    REGISTER_VIEW_DUMP_CLASS(xEditTextView)

    SLOT_CONNECT(this, signalTextChanged, this, xEditTextView::slotOnTextChange);
    SLOT_CONNECT(this, signalAppear, this, xEditTextView::slotOnAppear);

    //默认不做超长处理
    overlength_style = OVERLENGTH_TYPE_DIRECT_CUT;

#if IF_EDIT_TEXT_VIEW_NEED_SOFTKEY
    SetImeSoftkeyIndex(1);
    SetDeleteSoftkeyIndex(2);
#endif

#if IF_EDITTEXTVIEW_MULTI_EDIT
    m_pTextDocument = new CTextDocument(this);
    if (GetMultiLineEnable() && NULL != m_pTextDocument)
    {
        m_pTextDocument->SetViewportWidth(contentRect().Width());
        m_pTextDocument->UpdateCursorData(0);
    }
#endif
}

xEditTextView::~xEditTextView()
{
    m_tmCursor.KillTimer();
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL != m_pTextDocument)
    {
        delete m_pTextDocument;
        m_pTextDocument = NULL;
    }
#endif
}

void xEditTextView::SetFunInstallIME(FUN_INSTALL_IME pFunInstallIME)
{
    s_pFunInstallIME = pFunInstallIME;
}

void xEditTextView::loadAttributes(xml_node& node)
{
    xTextView::loadAttributes(node);

    //只允许自动换行和直接裁剪，多行效果由多行属性支持，所以为直接裁剪
    if (OVERLENGTH_TYPE_DIRECT_CUT != overlength_style)
    {
        SetOverLengthStyle(OVERLENGTH_TYPE_DIRECT_CUT);
    }

    PopupAndroidBool(node, IME_ATTR_ENABLED, m_bEnabled);

    //softkey位置
    PopupAndroidInt(node, IME_ATTR_IME_SOFTKEY, m_nImeSoftkeyIndex);
    PopupAndroidInt(node, IME_ATTR_DELETE_SOFTKEY, m_nDeleteSoftkeyIndex);

    SetEnabled(m_bEnabled);

    //ime
    yl::string ime;

    PopupAndroidString(node, IME_ATTR_IME, ime);

    if (!ime.empty() && NULL != s_pFunInstallIME)
    {
        (*s_pFunInstallIME)(this, ime.c_str());
    }

    //设定初始文本
    SetInitText(m_strText);
    //密码
    PopupAndroidBool(node, IME_ATTR_PASSWORD, m_bPassword);

    //字符输入立即显示密码
    PopupAndroidBool(node, IME_ATTR_SUDDEN_PASSWORD, m_bSuddenPassword);

    PopupAndroidInt(node, IME_ATTR_MAX_LENGTH, m_nMaxLength);
    PopupAndroidInt(node, IME_ATTR_MAX_BYTE, m_nMaxByte);

#if IF_EDITTEXTVIEW_SELECT_ENABLE
    PopupAndroidColor(node, IME_ATTR_SELECT_BG_COLOR, m_SelectBgColor);
    PopupAndroidColor(node, IME_ATTR_SELECT_TEXT_COLOR, m_SelectTextColor);
#endif

#if IF_EDIT_TEXT_VIEW_HINT_EMPTY
    PopupAndroidInt(node, IME_ATTR_HINT_SIZE, m_nHintFontSize);
    PopupAndroidColor(node, IME_ATTR_HINT_COLOR, m_clrHintText);
    yl::string strHint = "";
    if (PopupAndroidString(node, IME_ATTR_HINT_TEXT, strHint))
    {
        SetHintText(strHint);
    }
#endif

    // 加载完成后，调整大小
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL != m_pTextDocument)
    {
        m_pTextDocument->AdjustWidgetSize();;
    }
#endif
}

bool xEditTextView::SetOverLengthStyle(OVERLENGTH_TYPE eType)
{
    return xTextView::SetOverLengthStyle(OVERLENGTH_TYPE_DIRECT_CUT);
}

#if IF_EDIT_TEXT_VIEW_SCROLL
void xEditTextView::OnScrollFinish()
{
    m_bInScroll = false;

    SetOverLengthStyle(OVERLENGTH_TYPE_DIRECT_CUT);

    SetCursorStatus(true);
}

void xEditTextView::StartScroll(int nScrollCount /* = 1 */)
{
    if (!IsOverLength() || GetMultiLineEnable())
    {
        m_bInScroll = false;
        return;
    }

    m_tmCursor.KillTimer();
    m_bShowCursor = false;
    m_bInScroll = true;

    if (m_bEnabled)
    {
        COverLengthScrollEffectBase* pScrollEffect = (COverLengthScrollEffectBase*)GetEffectByType(OVERLENGTH_TYPE_SCROLL);
        if (NULL != pScrollEffect)
        {
            pScrollEffect->SetScrollCount(nScrollCount);
        }
    }

    xTextView::SetOverLengthStyle(OVERLENGTH_TYPE_SCROLL);
}

void xEditTextView::StopScroll()
{
    if (!m_bInScroll)
    {
        return;
    }

    if (NULL != m_pOverLengthEffect)
    {
        m_pOverLengthEffect->onEndEffect();
    }

    m_bInScroll = false;
    SetOverLengthStyle(OVERLENGTH_TYPE_DIRECT_CUT);

    SetCursorStatus(hasFocus());
}
#endif

bool xEditTextView::onPaintContent(xPainter& p)
{
#if IF_EDIT_TEXT_VIEW_SCROLL
    if (m_bInScroll)
    {
        return xTextView::onPaintContent(p);
    }
#endif

#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL != m_pTextDocument)
    {
        return m_pTextDocument->onPaintMultiEditText(p);
    }
    return false;
#else

    chRect rcViewPort = GetViewPort();

    if (!m_strText.empty())
    {
        // draw text
#if IF_EDITTEXTVIEW_SELECT_ENABLE
        if ((m_nSelectIndex + m_nSelectNum) > m_strText.length() || m_nSelectNum == 0)
        {
            chRect rcTextBound(chPoint(0, 0), CachePaintTextSize());
            adjustRectByGravity(contentRect(), gravity, rcTextBound);
            yl::string strtext = GetStringToPaint();
            DrawNormalText(p, rcTextBound, strtext);
        }
        else
        {
            int iPre = GetNumOfAutoReturnAdd(0, m_nSelectIndex);
            int iSel = GetNumOfAutoReturnAdd(m_nSelectIndex + iPre, m_nSelectNum + m_nSelectIndex + iPre);
            int iLength = GetStringToPaint().length();
            int nNewSelectIndex = m_nSelectIndex + iPre;
            int nNewSelectNum = m_nSelectNum + iSel;
            //中间选中区域
            yl::string strSelect = GetStringToPaint().substr(nNewSelectIndex, nNewSelectNum);
            chSize selectTextSize = font.textBound(strSelect);
            chRect rcSelectText(chPoint(0, 0), selectTextSize);

            if (0 < nNewSelectIndex)
            {
                yl::string strFirst = GetStringToPaint().substr(0, nNewSelectIndex);
                chSize Firsttextsize = font.textBound(strFirst);
                chRect rcFirstText(chPoint(0, 0), Firsttextsize);
                adjustRectByGravity(contentRect(), gravity, rcFirstText);
                //绘制第一部分
                DrawNormalText(p, rcFirstText, strFirst);

                rcSelectText.OffsetRect((rcFirstText.left + rcFirstText.Width()), rcFirstText.top);
            }
            else
            {
                adjustRectByGravity(contentRect(), gravity, rcSelectText);
            }
            //绘制选中区域
            DrawSelcetText(p, rcSelectText, strSelect);
            if ((m_nSelectIndex + m_nSelectNum) < m_strText.length())
            {
                yl::string strTail = GetStringToPaint().substr(nNewSelectIndex + nNewSelectNum, (iLength - nNewSelectIndex - nNewSelectNum));
                chSize Tailtextsize = font.textBound(strTail);
                chRect rcTailText(chPoint(0, 0), Tailtextsize);
                rcTailText.OffsetRect((rcSelectText.left + rcSelectText.Width()), rcSelectText.top);
                //绘制最后部分
                DrawNormalText(p, rcTailText, strTail);
            }
        }
#else
        chRect rcTextBound(chPoint(0, 0), CachePaintTextSize());
        chPoint ptViewPort = rcViewPort.TopLeft();

        adjustRectByGravity(contentRect(), gravity, rcTextBound);

        if (rcTextBound.right > contentRect().right)
        {
            rcTextBound.right = contentRect().right;
        }
        if (rcTextBound.bottom > contentRect().bottom)
        {
            rcTextBound.bottom = contentRect().bottom;
        }
        p.drawTextEx(rcTextBound, GetStringToPaint().c_str(), align, ptViewPort, font, textColor);
#endif
    }

    if (m_bEnabled)
    {
        chRect rcContent = contentRect();

        //drawRect，invertRect没有ViewPort相关接口，先这里处理viewPort位移
        if (m_bPreInput)
        {
            chRect rectImeChar = GetImeCharRect();

            rectImeChar.OffsetRect(-rcViewPort.left, -rcViewPort.top);
            rectImeChar.OffsetRect(rcContent.left, rcContent.top);

#if IF_EDITTEXTVIEW_PRELINE_ENABLE
            int nY = rectImeChar.bottom;
            int nFirX = rectImeChar.left;
            int nSecX = rectImeChar.left + rectImeChar.Width() - 2;
            p.setLineType(LINE_TYPE_DASH);
            p.drawLineX(nY, nFirX, nSecX, m_PreLineColor);
#else
#if IF_INVERT_EFFECT
            p.invertRect(rectImeChar);
#endif
#endif
        }
#if IF_INVERT_EFFECT
        else if (m_bShowCursor)
        {
            //光标本体
            chRect rectCursor = GetCursorRect();

            rectCursor.OffsetRect(-rcViewPort.left, -rcViewPort.top);
            rectCursor.OffsetRect(rcContent.left, rcContent.top);

            p.invertRect(rectCursor);
        }
#endif
    }

    return true;
#endif // IF_EDITTEXTVIEW_MULTI_EDIT
}

bool xEditTextView::onFocusEvent(bool bFocusIn)
{
    bool bResult = xTextView::onFocusEvent(bFocusIn);
    if (bFocusIn)
    {
        if (m_bEnabled)
        {
            OnEditEnable();
        }
    }
    else
    {
        OnEditDisable();
        TargetFocusChange(bFocusIn);
    }

    return bResult;
}

chRect xEditTextView::GetCursorRect()
{
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL == m_pTextDocument)
    {
        return rectLTWH(0, 0, 1, DEFAULT_CURSOR_HEIGHT);
    }

    if (m_bContainRTLChar)
    {
        return m_pTextDocument->GetRTLCursorRect();
    }
    else
    {
        return m_pTextDocument->GetCursorRect();
    }
#else
    chRect rectImeChar = GetImeCharRect();
    unsigned currentChar = 0;

    if (m_iCursorPos != 0)
    {
        currentChar = CacheWText().at(m_iCursorPos - 1);
    }
    else if (!CacheWText().empty())
    {
        currentChar = CacheWText().at(m_iCursorPos);
    }

    if (currentChar != 0)   //LTR时光标在非0位，显示在右侧
    {
        return rectLTWH(rectImeChar.right - 1, rectImeChar.top, 1, rectImeChar.Height());
    }
    else    //输入框没有文本，显示在左侧
    {
        return rectLTWH(rectImeChar.left, rectImeChar.top, 1, rectImeChar.Height());
    }
#endif // IF_EDITTEXTVIEW_MULTI_EDIT
}

chRect xEditTextView::GetCursorRectToGlobal()
{
    // 计算光标真实位置
    chRect rcCursor = GetCursorRect();
    chRect rcContent = contentRect();
    chRect rcViewport = GetViewPort();

    rcCursor.OffsetRect(rcContent.left, rcContent.top);
    rcCursor.OffsetRect(-rcViewport.left, -rcViewport.top);
    return rcCursor;
}

int xEditTextView::GetCursorPos()
{
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL == m_pTextDocument)
    {
        return 0;
    }

    CTextCursor* pCursor = m_pTextDocument->GetCurCursor();

    if (NULL == pCursor)
    {
        return 0;
    }

    int iCurPos = pCursor->GetCursorPos();

    return iCurPos;
#else
    return m_iCursorPos;
#endif // IF_EDITTEXTVIEW_MULTI_EDIT
}

chRect xEditTextView::GetImeCharRect()
{
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL == m_pTextDocument)
    {
        return rectLTWH(0, 0, 1, DEFAULT_CURSOR_HEIGHT);
    }

    if (m_bContainRTLChar)
    {
        return m_pTextDocument->GetRTLImeCharRect();
    }
    else
    {
        return m_pTextDocument->GetImeCharRect();
    }
#else
    //chString接口不熟悉，暂用yl::string处理
    chASSERT(m_iCursorPos <= CacheWText().size());

    chSize preTextSize;
    chSize imeCharSize;
    chRect rectLineTextBound;
    chRect rectAllTextBound;
    chRect rcContent = contentRect();

    rcContent.OffsetRect(-rcContent.left, -rcContent.top);

    int iCursorPos = GetCursorPos();
    if (iCursorPos == 0)
    {
        yl::wstring wstr = CacheWText();
        if (wstr.empty())
        {
            imeCharSize = font.textBound(" ");
        }
        else
        {
            yl::string firstChar = yl::to_utf8(wstr.substr(0, 1));
            imeCharSize = font.textBound(firstChar);
        }

        //获取调整后的位置
        chRect rectTextBound(chPoint(0, 0), imeCharSize);
        adjustRectByGravity(rcContent, gravity, rectTextBound);

        return rectLTWH(rectTextBound.left, rectTextBound.top, imeCharSize.cx, imeCharSize.cy);
    }
    else
    {
        yl::string result;
        typedef YLVector<yl::wstring> LINES;
        YLVector<yl::wstring> vecLines
            = yl::string_split<yl::wstring::value_type>(yl::to_utf16(GetStringToPaint()), L"\n", true);

        int nCount = 0;
        int nYOffset = 0;
        int ii = 0;

        // 修正因字符串中'\n'被去掉导致偏移
        int iNum = m_vecLineBreakLocation.size();
        int iMinus = 0;
        for (int i = 0; i < iNum; ++i)
        {
            // 前置边界判断要修改
            if (iBidiCursorPos >= m_vecLineBreakLocation[i])
            {
                ++iMinus;
            }
            else
            {
                break;
            }
        }
        iCursorPos -= iMinus;

        for (LINES::iterator begin = vecLines.begin(); begin != vecLines.end(); ++begin)
        {
            const yl::wstring& u16text = *begin;
            //如果不在本行则加上本行的高度，继续查找以下行
            if (nCount + u16text.size() < iCursorPos)
            {
                // qfontmetrics会加上leading()，行间隙
                nYOffset += font.textBound(yl::to_utf8(u16text)).cy - 2;
                nCount += u16text.size();
                ii ++;
            }
            else
            {
                imeCharSize = font.textBound(yl::to_utf8(u16text.substr(iBidiCursorPos - nCount - 1, 1)));
                //之前字符的大小
                preTextSize = font.textBound(yl::to_utf8(u16text.substr(0, iBidiCursorPos - nCount - 1)));

                //获取调整后的位置
                rectAllTextBound = chRect(chPoint(0, 0), font.textBound(GetStringToPaint()));
                adjustRectByGravity(rcContent, gravity, rectAllTextBound);
                //当前行位置
                rectLineTextBound = chRect(chPoint(0, 0), font.textBound(yl::to_utf8(u16text)));
                adjustRectByGravity(rectAllTextBound, align, rectLineTextBound);

                break;
            }

        }

        return rectLTWH(rectLineTextBound.left + preTextSize.cx, rectAllTextBound.top + nYOffset, imeCharSize.cx - 1, rectLineTextBound.Height());
    }
#endif // IF_EDITTEXTVIEW_MULTI_EDIT
}

bool xEditTextView::onKeyPressedEvent(int key)
{
#if IF_DATETIMEEDIT_FOCUS_SWITCH_ENABLE
    if (!m_bKeyPressedEnable)
    {
        return false;
    }
#endif

#if IF_EDIT_TEXT_VIEW_BIND_KEYBOARD
    if (m_bKeyBoardShow)
    {
        bool bResult = false;

        if (ProcessKeyboardPress(key, bResult))
        {
            return bResult;
        }
    }
#endif

    if (KEY_REPEAT_MASK == (key & KEY_REPEAT_MASK))
    {
        key = GET_KEY_VALUE(key);
    }

#if IF_EDITTEXTVIEW_SELECT_ENABLE
    //当前选取状态时输入字符清除原选取的字符
    if (m_nSelectNum != 0 && ((PHONE_KEY_0 <= key && PHONE_KEY_9 >= key)
                              || PHONE_KEY_STAR == key || PHONE_KEY_POUND == key))
    {
        DeleteSelectChar(false);
    }
#endif

    if (SendImeKey(key))
    {
        return true;
    }

    switch (key)
    {
    case PHONE_KEY_CANCEL:
    {
        // 单控件测试用代码
        if (this->m_pParent == NULL
                && (m_strText.empty() || !m_bEnabled))
        {
            m_pHostStation->close_helper(this, true);
            return true;
        }
        break;
    }
    case PHONE_KEY_LEFT:
    {
        if (TargetKeyPress(key))
        {
            break;
        }
        else
        {
            return MoveCursorPrevious();
        }
    }
    break;
    case PHONE_KEY_RIGHT:
    {
        if (TargetKeyPress(key))
        {
            break;
        }
        else
        {
            return MoveCursorNext();
        }
    }
    break;
#if IF_EDITTEXTVIEW_MULTI_EDIT
    case PHONE_KEY_UP:
    {
        if (!GetMultiLineEnable())
        {
            return false;
        }
        return MoveCursor(GetMoveStep((int)CTextDocument::CURSOR_OP_MOVE_UP));
    }
    break;
    case PHONE_KEY_DOWN:
    {
        if (!GetMultiLineEnable())
        {
            return false;
        }
        return MoveCursor(GetMoveStep((int)CTextDocument::CURSOR_OP_MOVE_DOWN));
    }
    break;
#endif
    case PHONE_KEY_BACKSPACE:
    {
        return DoDeleteAction();
    }
    break;
    default:
        TargetKeyPress(key);
        break;
    }

    return false;

}

#if IF_MULTI_TOUCH_ENABLE
bool xEditTextView::onTouchBeganEvent(xView* pAnchorView, LIST_RAW_TOUCH_POINT& lsTouchPoint)
{
    if (pAnchorView == this && hasFocused() && lsTouchPoint.size() > 0)
    {
        ST_TOUCH_POINT* pTouchPoint = lsTouchPoint.back();

        if (NULL == pTouchPoint)
        {
            return false;
        }

        chPoint ptTouch(pTouchPoint->nPosX, pTouchPoint->nPosY);

        ptTouch.OffsetPoint(-m_rectLayout.left, -m_rectLayout.top);

        MoveCursorToPos(ptTouch);
    }

    return false;
}

void xEditTextView::MoveCursorToPos(const chPoint& ptPos)
{
    chRect rcContent = contentRect();

    if (!rcContent.PtInRect(ptPos) || NULL == m_pTextDocument)
    {
        return;
    }

    CTextCursor* pCursor = m_pTextDocument->GetCurCursor();

    if (NULL == pCursor)
    {
        return;
    }

    int nTextLen = m_pTextDocument->TextLength();

    if (nTextLen <= 0)
    {
        return;
    }

    chRect rcViewport = GetViewPort();
    chRect rcCursor = GetCursorRect();

    rcCursor.OffsetRect(-rcViewport.left, -rcViewport.top);
    rcCursor.OffsetRect(rcContent.left, rcContent.top);

    int nCursorPos = pCursor->GetCursorPos();

    int nNewCursorPos = nCursorPos;
    int nCursorLeft = rcCursor.left;

    if (rcCursor.left > ptPos.x)
    {
        // 光标在触点右边，向前查找
        for (; nNewCursorPos >= 0; --nNewCursorPos)
        {
            yl::wstring strPreChar = m_pTextDocument->GetStringByPos(nNewCursorPos - 1, 1);
            int nCharWidth = font.textBound(yl::to_utf8(strPreChar)).cx;
            int nHalfCharWidth = nCharWidth >> 1;

            if ((nCursorLeft - nHalfCharWidth) <= ptPos.x)
            {
                break;
            }

            nCursorLeft -= nCharWidth;
        }

        if (nNewCursorPos < 0)
        {
            nNewCursorPos = 0;
        }
    }
    else if (rcCursor.right < ptPos.x)
    {
        // 光标在触点左边，向后查找
        for (; nNewCursorPos < nTextLen; ++nNewCursorPos)
        {
            yl::wstring strPreChar = m_pTextDocument->GetStringByPos(nNewCursorPos, 1);
            int nCharWidth = font.textBound(yl::to_utf8(strPreChar)).cx;
            int nHalfCharWidth = nCharWidth >> 1;

            if ((nCursorLeft + nHalfCharWidth) > ptPos.x)
            {
                break;
            }

            nCursorLeft += nCharWidth;
        }
    }

    if (nNewCursorPos != nCursorPos)
    {
        MoveCursor(nNewCursorPos - nCursorPos);
    }
}
#endif

bool xEditTextView::MoveCursorNext()
{
#if IF_EDITTEXTVIEW_SELECT_ENABLE
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL != m_pTextDocument)
    {
        m_pTextDocument->SetLeftHorizontalPos(-1);

        CTextCursor* pCursor = m_pTextDocument->GetCurCursor();
        if (NULL != pCursor)
        {
            int iMoveStep = 1;
            if (m_nSelectNum != 0)
            {
                iMoveStep = pCursor->GetAnchorEnPos() - pCursor->GetCursorPos();
                SetSelectInvalid();
            }
            return MoveCursor(iMoveStep) != 0;
        }
    }

    return false;
#else
    if (m_nSelectNum != 0)
    {
        SetSelectInvalid();
    }
    return MoveCursor(1) != 0;
#endif
#else
    return MoveCursor(1) != 0;
#endif
}

bool xEditTextView::MoveCursorPrevious()
{
#if IF_EDITTEXTVIEW_SELECT_ENABLE
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL != m_pTextDocument)
    {
        m_pTextDocument->SetLeftHorizontalPos(-1);

        CTextCursor* pCursor = m_pTextDocument->GetCurCursor();
        if (NULL != pCursor)
        {
            int iMoveStep = -1;
            if (m_nSelectNum != 0)
            {
                iMoveStep = pCursor->GetAnchorStPos() - pCursor->GetCursorPos();
                SetSelectInvalid();
            }
            return MoveCursor(iMoveStep) != 0;
        }
    }

    return false;
#else
    int iMoveStep = -1;
    if (m_nSelectNum != 0)
    {
        iMoveStep -= m_iCursorPos - m_nSelectIndex;
        SetSelectInvalid();
    }
    return MoveCursor(iMoveStep) != 0;
#endif
#else
    return MoveCursor(-1) != 0;
#endif
}

#if IF_EDIT_TEXT_VIEW_BIND_KEYBOARD
bool xEditTextView::onKeyReleasedEvent(int key)
{
    if (m_bKeyBoardShow && KB_IsKeyboardFocus(this))
    {
        if (KB_ProcessKeyRelease(this, key))
        {
            return true;
        }

        if (KB_IsKeyboardFocus(this))
        {
            return true;
        }
    }

    return xTextView::onKeyReleasedEvent(key);
}

bool xEditTextView::ProcessKeyboardPress(int key, bool& bResult)
{
    if (!KB_IsKeyboardShow(this))
    {
        return false;
    }

    if (KB_IsKeyboardFocus(this))
    {
        bool bProcess = KB_ProcessKeyPress(this, key);

        if (!bProcess && KB_IsKeyboardFocus(this))
        {
            return false;
        }

        if (bProcess || (PHONE_KEY_UP == GET_KEY_VALUE(key) && KB_IsKeyboardShow(this)))
        {
            bResult = true;
        }
        else
        {
            bResult = false;
        }

        return true;
    }
    else if (PHONE_KEY_DOWN == key)
    {
        KB_FocusKeyboard(this, xView::FOCUS_REASON_DOWN);

        bResult = true;

        return true;
    }

    return false;
}
#endif

#if IF_EDITTEXTVIEW_MULTI_EDIT
int xEditTextView::GetMoveStep(int eOp)
{
#if IF_EDITTEXTVIEW_SELECT_ENABLE
    SetSelectInvalid();
#endif

    int iMoveStep = 0;
    if (NULL != m_pTextDocument)
    {
        iMoveStep = m_pTextDocument->GetMoveStep(eOp);
    }

    return iMoveStep;
}
#endif

void xEditTextView::slotOnTextChange(xSignal* sender, WPARAM wParam, LPARAM lParam, int /*nDataBytes*/, LPVOID /*pData*/)
{
    if (NULL != m_pTextDocument)
    {
        m_pTextDocument->AdjustWidgetSize();
    }
}

void xEditTextView::slotOnAppear(xSignal* sender, WPARAM wParam, LPARAM lParam, int /*nDataBytes*/, LPVOID /*pData*/)
{
    if (0 == wParam)
    {
        TargetFocusChange(false);
    }
}

void xEditTextView::OnFlushCursor(xThreadTimer* timer, LPVOID pExtraData)
{
    chRect rcCursor = GetCursorRect();
    chRect rcContent = contentRect();
    chRect rcViewport = GetViewPort();

    rcCursor.OffsetRect(rcContent.left, rcContent.top);
    rcCursor.OffsetRect(-rcViewport.left, -rcViewport.top);
    rcCursor.OffsetRect(m_rectLayout.left, m_rectLayout.top);

    m_bShowCursor = !m_bShowCursor;

    update(rcCursor);
}

void xEditTextView::SetInitText(const yl::string& text)
{
    m_iCursorPos = yl::to_utf16(text).size();
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL != m_pTextDocument)
    {
        m_pTextDocument->SetViewportWidth(contentRect().Width());
        m_pTextDocument->InitText(yl::to_utf16(text));
    }
#endif // IF_EDITTEXTVIEW_MULTI_EDIT

    SetText(text);

    EnsureCursorVisiable();

    signalTextConfirmChange.emitSignal(0, 0, 0, NULL);
}

bool xEditTextView::DeleteChar(bool bSignalNotify/* = true*/)
{
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL != m_pTextDocument && !m_pTextDocument->IsWTextEmpty())
    {
        CTextCursor* pCursor = m_pTextDocument->GetCurCursor();
        if (NULL != pCursor && 0 != pCursor->GetCursorPos())
        {
            int iPreCursorPos = pCursor->GetCursorPos() - 1;
            // 调整水平、竖直偏移，以及TextLine
            yl::string strDelChar = yl::to_utf8(m_pTextDocument->GetStringByPos(iPreCursorPos));
            // 更新行字符数量
            int iIndex = mapLines.findNode(iPreCursorPos);
            if (0 != iIndex)
            {
                mapLines.setSize(iIndex, mapLines.size(iIndex) - 1);
            }
            m_pTextDocument->RemoveString(iPreCursorPos);
            SetText(yl::to_utf8(m_pTextDocument->GetWTextString()));
#if IF_IME_EDIT_DEBUG
            XWINDOW_INFO("----------- del pos[%d], string[%s], index[%d] ======", iPreCursorPos, strDelChar.c_str(), iIndex);
#endif

            m_pTextDocument->UpdateLineText(iPreCursorPos);

            // 更新光标位置
            m_pTextDocument->UpdateCursorData(iPreCursorPos);

            EnsureCursorVisiable();

            PasswordHideChar();

            if (bSignalNotify)
            {
                signalTextConfirmChange.emitSignal(0, 0, 0, NULL);
            }

            return true;
        }
    }

    return false;
#else

    if (m_strText.empty() || m_iCursorPos == 0)
    {
        return false;
    }

    chRect rcCursor = GetCursorRect();
    chPoint ptCursor(rcCursor.left, rcCursor.top);

    --m_iCursorPos;

    rcCursor = GetCursorRect();

    yl::wstring newText = yl::to_utf16(m_strText);
    yl::string strDelChar = yl::to_utf8(newText.substr(m_iCursorPos, 1));
    newText.erase(m_iCursorPos, 1);
    SetText(yl::to_utf8(newText));
    PasswordHideChar();
    //signalTextConfirmChange.emitSignal(0, 0, 0, NULL);

    int nOffsetX = ptCursor.x - rcCursor.left;
    int nOffsetY = ptCursor.y - rcCursor.top;

    if (nOffsetX > m_LastViewPort.left)
    {
        nOffsetX = m_LastViewPort.left;
    }

    if (nOffsetY > m_LastViewPort.top)
    {
        nOffsetY = m_LastViewPort.top;
    }

    m_LastViewPort.OffsetRect(-nOffsetX, -nOffsetY);

    return true;
#endif // IF_EDITTEXTVIEW_MULTI_EDIT
}

#if IF_EDITTEXTVIEW_PRELINE_ENABLE
void xEditTextView::SetPreLineColor(const xColor& color)
{
    m_PreLineColor = color;
}
#endif

#if IF_EDIT_TEXT_VIEW_HINT_EMPTY
yl::string xEditTextView::GetHintText() const
{
    return m_strHintText;
}

void xEditTextView::SetHintText(const yl::string& strHintText)
{
    if (!g_WorkStation.SetupTextTran(this, strHintText))
    {
        SetTranText(strHintText);
    }
}

void xEditTextView::SetTranText(const yl::string& strTextHint)
{
    if (m_strHintText != strTextHint)
    {
        m_strHintText = strTextHint;

        xFont fontHint = GetHintFont();

        m_nHintWidth = fontHint.textBound(m_strHintText).cx;
    }
}

xFont xEditTextView::GetHintFont()
{
    if (m_nHintFontSize <= 0)
    {
        return font;
    }

    xFont fontHint = font;
    fontHint.setPointSize(m_nHintFontSize);

    return fontHint;
}

#endif

void xEditTextView::SetText(const yl::string& src)
{
#if IF_EDIT_TEXT_VIEW_SCROLL
    if (m_strText != src && m_bInScroll)
    {
        StopScroll();
    }
#endif

    xTextView::SetTranText(src);

#if IF_TEXTVIEW_SUPPORT_ACTIVE_FONT
    SetFontFitSize();
#endif
}

#if IF_EDITTEXTVIEW_SELECT_ENABLE
bool xEditTextView::DeleteSelectChar(bool bSignalNotify/* = true*/)
{
    if (m_strText.empty() || m_nSelectNum == 0
            || (m_nSelectIndex + m_nSelectNum) > m_strText.length())
    {
        return false;
    }

    bool bDelete = true;

#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL != m_pTextDocument)
    {
        CTextCursor* pCursor = m_pTextDocument->GetCurCursor();
        if (NULL != pCursor)
        {
            // 调整光标位置，删除前先更新位置，后面更新会失败
            m_pTextDocument->UpdateCursorData(pCursor->GetAnchorStPos());
            // 计算
            int iSelStIndex = mapLines.findNode(pCursor->GetAnchorStPos());
            int iSelEnIndex = mapLines.findNode(pCursor->GetAnchorEnPos());
            if (!m_pTextDocument->IsWTextEmpty()
                    && m_pTextDocument->TextLength() == pCursor->GetAnchorEnPos())
            {
                iSelEnIndex = mapLines.findNode(pCursor->GetAnchorEnPos() - 1);
            }
            int iNewSize = 0;
            if (0 != iSelStIndex && 0 != iSelEnIndex)
            {
                int iNumSt = pCursor->GetAnchorStPos() - mapLines.position(iSelStIndex);
                int iNumEn = mapLines.size(iSelEnIndex) - (pCursor->GetAnchorEnPos() - mapLines.position(iSelEnIndex));
                iNewSize = iNumSt + iNumEn;
            }
            // 删除字符
            m_pTextDocument->RemoveString(pCursor->GetAnchorStPos(), pCursor->GetAnchorEnPos());
            SetText(yl::to_utf8(m_pTextDocument->GetWTextString()));
            // 删除选中开始也结束中间的行
            for (int iIdx = iSelStIndex; 0 != iIdx && 0 != iSelEnIndex && iIdx != iSelEnIndex;)
            {
                int iNext = mapLines.next(iIdx);
                mapLines.erase_single(iIdx);
                iIdx = iNext;
            }
            // 合并始终行
            if (0 != iSelStIndex && 0 != iSelEnIndex)
            {
                mapLines.setSize(iSelStIndex, iNewSize);
                if (iSelStIndex != iSelEnIndex)
                {
                    mapLines.erase_single(iSelEnIndex);
                }
            }
            // 更新
            m_pTextDocument->UpdateLineText(pCursor->GetAnchorStPos());
            // 更新光标位置
            m_pTextDocument->UpdateCursorData(pCursor->GetAnchorStPos());
            EnsureCursorVisiable();
        }
        else
        {
            bDelete = false;
        }
    }
    else
    {
        bDelete = false;
    }
#else
    yl::wstring newText = yl::to_utf16(m_strText);
    newText.erase(m_nSelectIndex, m_nSelectNum);
    SetText(yl::to_utf8(newText));
    m_iCursorPos = m_nSelectIndex;
#endif

    PasswordHideChar();

    if (bDelete && bSignalNotify)
    {
        signalTextConfirmChange.emitSignal(0, 0, 0, NULL);
    }

    SetSelectInvalid();

    return bDelete;
}

void xEditTextView::SetSelectRange(int nindex, int num)
{
    //设置范围超出文本长度
    if (nindex + num > m_strText.length())
    {
        return;
    }

    m_nSelectIndex = nindex;
    m_nSelectNum = num;
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL != m_pTextDocument)
    {
        CTextCursor* pCursor = m_pTextDocument->GetCurCursor();
        if (NULL != pCursor)
        {
            pCursor->SetSelectArea(nindex, nindex + num);
        }
    }
#endif
}

void xEditTextView::SetSelectInvalid()
{
    m_nSelectNum = 0;
    m_nSelectIndex = 0;
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL != m_pTextDocument)
    {
        CTextCursor* pCursor = m_pTextDocument->GetCurCursor();
        if (NULL != pCursor)
        {
            pCursor->SetSelectArea(0, 0);
        }
    }
#endif
}

void xEditTextView::SetAllSelectStatus()
{
    m_nSelectIndex = 0;
    m_nSelectNum = m_strText.length();
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL != m_pTextDocument)
    {
        CTextCursor* pCursor = m_pTextDocument->GetCurCursor();
        if (NULL != pCursor)
        {
            pCursor->SetSelectArea(0, m_pTextDocument->TextLength());
        }
    }
#endif
}

void xEditTextView::SetSelectTextBgColor(const xColor& color)
{
    m_SelectBgColor = color;
}

void xEditTextView::SetSelectTextColor(const xColor& color)
{
    m_SelectTextColor = color;
}

void xEditTextView::DrawSelcetText(xPainter& painter, chRect &rcText, yl::string &text)
{
    if (text.empty())
    {
        return;
    }

    if (rcText.right > contentRect().right)
    {
        rcText.right = contentRect().right;
    }

    if (rcText.bottom > contentRect().bottom)
    {
        rcText.bottom = contentRect().bottom;
    }

    painter.fillRect(rcText, m_SelectBgColor);

    chRect rcViewPort = GetViewPort();
    chPoint ptViewPort = rcViewPort.TopLeft();
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (GetMultiLineEnable())
    {
        rcText.left -= ptViewPort.x;
        rcText.top -= ptViewPort.y;
        ptViewPort.x = 0;
        ptViewPort.y = 0;
    }
#endif

    painter.drawTextEx(rcText, text.c_str(), align, ptViewPort, font, m_SelectTextColor);
}

#endif

void xEditTextView::DrawNormalText(xPainter& painter, chRect &rcText, yl::string &text)
{
    if (text.empty())
    {
        return;
    }

    if (rcText.right > contentRect().right)
    {
        rcText.right = contentRect().right;
    }

    if (rcText.bottom > contentRect().bottom)
    {
        rcText.bottom = contentRect().bottom;
    }

    chRect rcViewPort = GetViewPort();
    chPoint ptViewPort = rcViewPort.TopLeft();
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (GetMultiLineEnable())
    {
        rcText.left -= ptViewPort.x;
        rcText.top -= ptViewPort.y;
        ptViewPort.x = 0;
        ptViewPort.y = 0;
    }
#endif
#if IF_IME_EDIT_DEBUG
    XWINDOW_INFO("--------- paint text[%s]\n--------- topleft viewport(%d, %d), after(%d, %d), rc(%d, %d), h[%d], ---align[%d] ==="
                 , text.c_str()
                 , rcViewPort.left, rcViewPort.top, rcText.left, rcText.top
                 , rcText.left + rcViewPort.left, rcText.top + rcViewPort.top, rcText.Height()
                 , align);
#endif

    painter.drawTextEx(rcText, text.c_str(), align, ptViewPort, font, textColor);
}

#if IF_EDIT_TEXT_VIEW_HINT_EMPTY
void xEditTextView::DrawHintText(xPainter& painter, chRect &rcText, yl::string &text)
{
    if (text.empty())
    {
        return;
    }

    if (rcText.right > contentRect().right)
    {
        rcText.right = contentRect().right;
    }

    if (rcText.bottom > contentRect().bottom)
    {
        rcText.bottom = contentRect().bottom;
    }

    chRect rcViewPort = GetViewPort();
    chPoint ptViewPort = rcViewPort.TopLeft();
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (GetMultiLineEnable())
    {
        rcText.left -= ptViewPort.x;
        rcText.top -= ptViewPort.y;
        ptViewPort.x = 0;
        ptViewPort.y = 0;
    }
#endif

    xFont fontHint = GetHintFont();
    painter.drawTextEx(rcText, text.c_str(), align, ptViewPort, fontHint, m_clrHintText);
}
#endif

chRect xEditTextView::GetViewPort()
{
#if IF_EDIT_TEXT_VIEW_SCROLL
    if (m_bInScroll)
    {
        return xTextView::GetViewPort();
    }
#endif

#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (m_LastViewPort.IsEmpty())
    {
        chSize viewSize = contentRect().Size();
        m_LastViewPort = chRect(0, 0, viewSize.cx, viewSize.cy);
    }
    else if (contentRect().Width() > m_LastViewPort.Width())
    {
        m_LastViewPort.ResizeWidth(contentRect().Width());
    }

    return m_LastViewPort;
#else

    chSize viewSize = contentRect().Size();
    chSize textSize = CachePaintTextSize();

    if (m_LastViewPort.IsEmpty())
    {
        m_LastViewPort = chRect(0, 0, viewSize.cx, viewSize.cy);
    }
    //容得下，返回原始ViewPort
    if ((viewSize.cx > textSize.cx
            && viewSize.cy > textSize.cy)
            || m_bEnabled == false)
    {
        m_LastViewPort = chRect(0, 0, viewSize.cx, viewSize.cy);
    }
    else
    {
        //判断光标位置是否显示
        chRect& rectViewPort = m_LastViewPort;
        chRect rectCursor = GetCursorRect();

        if (rectCursor.right > m_LastViewPort.right)
        {
            rectViewPort.OffsetRect(rectCursor.right - rectViewPort.right, 0);
        }

        if (rectCursor.bottom > m_LastViewPort.bottom)
        {
            rectViewPort.OffsetRect(0, rectCursor.bottom - rectViewPort.bottom);
        }

        if (rectCursor.left < m_LastViewPort.left)
        {
            rectViewPort.OffsetRect(rectCursor.left - m_LastViewPort.left, 0);
        }

        if (rectCursor.top < m_LastViewPort.top)
        {
            rectViewPort.OffsetRect(0, rectCursor.top - m_LastViewPort.top);
        }
    }

    return m_LastViewPort;
#endif // IF_EDITTEXTVIEW_MULTI_EDIT
}

int xEditTextView::MoveCursor(int offset)
{
    if (!m_bEnabled)
    {
        return 0;
    }

    //保证移动中光标可见
    m_bShowCursor = true;

#if IF_EDITTEXTVIEW_MULTI_EDIT
    // 更新光标信息和Viewport偏移量
    int iCurPos = 0;
    int iPreCur = 0;
    if (NULL != m_pTextDocument)
    {
        CTextCursor* pCursor = m_pTextDocument->GetCurCursor();
        if (NULL != pCursor)
        {
            iCurPos = pCursor->GetCursorPos();
            iPreCur = iCurPos;
            iCurPos += offset;
            if (iCurPos < 0)
            {
                iCurPos = 0;
            }
            else if (iCurPos > m_pTextDocument->TextLength())
            {
                iCurPos = m_pTextDocument->TextLength();
            }

            m_pTextDocument->UpdateCursorData(iCurPos);
        }
    }
    EnsureCursorVisiable();

    PasswordHideChar();

    update();

    return (iPreCur - iCurPos);
#else

    int prePos = m_iCursorPos;
    m_iCursorPos += offset;

    int iOp = offset > 0 ? 1 : -1;
    for (int i = 0; i < MAX_LOOP_TIME; ++i)
    {
        if (m_iCursorPos < 0 || m_iCursorPos == 0)
        {
            m_iCursorPos = 0;
            break;
        }
        else if (m_iCursorPos > TextLength())
        {
            m_iCursorPos = TextLength();
            break;
        }

        if (0 != yl::to_utf8(CacheWText().substr(m_iCursorPos - 1, 1)).compare("\n"))
        {
            break;
        }
        m_iCursorPos += iOp;
    }

    PasswordHideChar();

    update();

    //返回实际移动的位移
    return prePos - m_iCursorPos;
#endif // IF_EDITTEXTVIEW_MULTI_EDIT
}

void xEditTextView::SetCursorPosition(int nPosition)
{
#if IF_EDITTEXTVIEW_MULTI_EDIT
    // 更新光标信息和Viewport偏移量
    if (NULL != m_pTextDocument)
    {
        nPosition = nPosition < 0 ? 0 : nPosition;
        nPosition = nPosition > m_pTextDocument->TextLength() ? m_pTextDocument->TextLength() : nPosition;
        m_pTextDocument->UpdateCursorData(nPosition);
        EnsureCursorVisiable();
    }
    return;
#else

    m_iCursorPos = nPosition;

    if (m_iCursorPos < 0)
    {
        m_iCursorPos = 0;
    }
    else if (m_iCursorPos > TextLength())
    {
        m_iCursorPos = TextLength();
    }
#endif // IF_EDITTEXTVIEW_MULTI_EDIT
}

bool xEditTextView::OnChar(IMECHAR ch)
{
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL != m_pTextDocument)
    {
        m_pTextDocument->SetLeftHorizontalPos(-1);
    }
#endif
    if (InsertChar(ch))
    {
        m_bPreInput = false;
        signalTextConfirmChange.emitSignal(0, 0, 0, NULL);
        return true;
    }
    return false;
}

bool xEditTextView::OnPreChar(IMECHAR ch)
{
    if (!m_bEnabled)
    {
        return false;
    }

#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL != m_pTextDocument)
    {
        m_pTextDocument->SetLeftHorizontalPos(-1);
    }
#endif

    if (m_bPreInput)
    {
#if IF_EDITTEXTVIEW_MULTI_EDIT
        if (NULL != m_pTextDocument)
        {
            CTextCursor* pCursor = m_pTextDocument->GetCurCursor();
            if (NULL != pCursor)
            {
                int iPreCur = pCursor->GetCursorPos();
                m_pTextDocument->ReplaceString(iPreCur - 1, 1, 1, ch);
                SetText(yl::to_utf8(m_pTextDocument->GetWTextString()));
                signalTextPreChange.emitSignal(0, 0, 0, NULL);
                m_pTextDocument->UpdateLineText(iPreCur - 1);
                EnsureCursorVisiable();
                PasswordShowChar();
            }
        }
#else

        IMESTRING newText = CacheWText();
        newText.replace(m_iCursorPos - 1, 1, 1, ch);
        PasswordShowChar();
        setWText(newText);
#endif // IF_EDITTEXTVIEW_MULTI_EDIT
    }
    else
    {

#if IF_EDITTEXTVIEW_MULTI_EDIT
        if (NULL != m_pTextDocument)
        {
            yl::wstring strWText = m_pTextDocument->GetWTextString();
            strWText += ch;
            if (!CheckLength(strWText))
            {
                return false;
            }
        }
#endif // IF_EDITTEXTVIEW_MULTI_EDIT

        //预输入到确认时才检查长度
        if (InsertChar(ch, false))
        {
            signalTextPreChange.emitSignal(0, 0, 0, NULL);
            m_bPreInput = true;
            update();
        }
    }

    return true;
}

bool xEditTextView::OnPreInputConfirm()
{
    if (!m_bEnabled || !m_bPreInput)
    {
        return false;
    }

    TargetConfirm();

    m_bPreInput = false;

    bool bResult = true;

    //检查长度，超长后需要取消输入
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL == m_pTextDocument)
    {
        return false;
    }
    m_pTextDocument->SetLeftHorizontalPos(-1);
    // 调整预输入后光标偏移问题
    CTextCursor* pCursor = m_pTextDocument->GetCurCursor();
    if (NULL != pCursor)
    {
        m_pTextDocument->UpdateCursorData(pCursor->GetCursorPos(), true);
    }

    if (!CheckLength(m_pTextDocument->GetWTextString()))
    {
        DeleteChar(false);

        bResult = false;
    }
#else
    if (!CheckLength(CacheWText()))
    {
        DeleteChar(false);

        bResult = false;
    }
#endif // IF_EDITTEXTVIEW_MULTI_EDIT

    CheckImeInputText();

    update();

    if (bResult)
    {
        signalTextConfirmChange.emitSignal(0, 0, 0, NULL);
    }

    return bResult;
}

void xEditTextView::CheckImeInputText()
{
    yl::wstring strNewWText;

#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL == m_pTextDocument)
    {
        return;
    }

    const yl::wstring& strWText = m_pTextDocument->GetWTextString();
#else
    const yl::string& strWText = CacheWText();
#endif

    if (CheckImeText(strWText, strNewWText) && strNewWText != strWText)
    {
#if IF_EDITTEXTVIEW_MULTI_EDIT
        m_pTextDocument->WTextString(strNewWText);
#endif

        SetText(yl::to_utf8(strNewWText));
    }
}

//在当前光标处插入字符
bool xEditTextView::InsertChar(IMECHAR ch, bool bCheckLength/* = true*/)
{
    if (!m_bEnabled)
    {
        return false;
    }

#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL == m_pTextDocument)
    {
        return false;
    }

    //长度检查
    yl::wstring strWText = m_pTextDocument->GetWTextString();

    strWText += ch;

    bool bLegalLength = CheckLength(strWText);

    if (!bCheckLength || bLegalLength)
    {
        CTextCursor* pCursor = m_pTextDocument->GetCurCursor();
        if (NULL != pCursor)
        {
            int iCursorPos = pCursor->GetCursorPos();
            ++iCursorPos;
            PasswordShowChar();
            // 更新行字符数量
            if (m_pTextDocument->GetWTextString().empty())
            {
                mapLines.insert_single(0, 1);
            }
            else
            {
                int iIndex = mapLines.findNode(iCursorPos - 1);
                if (iCursorPos > 1
                        && iCursorPos - 1 == m_pTextDocument->TextLength())
                {
                    iIndex = mapLines.findNode(iCursorPos - 2);
                }
                if (0 != iIndex)
                {
                    mapLines.setSize(iIndex, mapLines.size(iIndex) + 1);
                }
            }

            m_pTextDocument->InsertString(iCursorPos - 1, ch);
            SetText(yl::to_utf8(m_pTextDocument->GetWTextString()));

            if (L'\n' == ch)
            {
                // 分块，更新两块信息
                m_pTextDocument->Split(iCursorPos);
                m_pTextDocument->UpdateLineText(iCursorPos);
                m_pTextDocument->UpdateLineText(iCursorPos + 1);
            }
            else
            {
                // 更新块信息
                m_pTextDocument->UpdateLineText(iCursorPos);
            }

            // 更新光标位置并显示出来
            m_pTextDocument->UpdateCursorData(iCursorPos);
            EnsureCursorVisiable();

            return true;
        } // end (NULL != pCursor)
    }
#else

    IMESTRING newText = CacheWText();
    newText.insert(m_iCursorPos, 1, ch);
    //长度检查
    if (!bCheckLength || CheckLength(newText))
    {
        m_iCursorPos++;
        PasswordShowChar();
        setWText(newText);

        return true;
    }
#endif // IF_EDITTEXTVIEW_MULTI_EDIT

    return false;
}

bool xEditTextView::DirectInserChar(IMECHAR ch, bool bCheckLength /*= true*/)
{
    bool bResult = InsertChar(ch, bCheckLength);
    if (bResult)
    {
        signalTextConfirmChange.emitSignal(0, 0, 0, NULL);
    }
    return bResult;
}

//检查长度限制
bool xEditTextView::CheckLength(const yl::wstring& wStr)
{
    if ((m_nMaxLength != -1 && wStr.size() > m_nMaxLength)
            || (m_nMaxByte != -1 && yl::to_utf8(wStr).size() > m_nMaxByte))
    {
        return false;
    }
    return true;
}

#if IF_DATETIMEEDIT_FOCUS_SWITCH_ENABLE
void xEditTextView::SetKeyPressedEnable(bool benable)
{
    m_bKeyPressedEnable = benable;
}
#endif

bool xEditTextView::OnImeChange(const yl::string& strImeName)
{
#if IF_EDIT_TEXT_VIEW_NEED_SOFTKEY
    if (m_nImeSoftkeyIndex != -1)
    {
        setupViewSoftKey(this, m_nImeSoftkeyIndex, strImeName, ST_IMECHANGE, SLOT_METHOD(this, xEditTextView::slotOnSoftkeypress));
    }

    signalImeChanged.emitSignal((WPARAM)this, 0, 0, NULL);
#endif

    return true;
}

void xEditTextView::slotOnSoftkeypress(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    if (ST_IMECHANGE == wParam)
    {
        SwitchIme();
    }
    else if (ST_DELETE == wParam)
    {
        DoDeleteAction();
    }
}

bool xEditTextView::DoDeleteAction()
{
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL != m_pTextDocument)
    {
        m_pTextDocument->SetLeftHorizontalPos(-1);
    }
#endif

    bool bDelete = false;

#if IF_EDITTEXTVIEW_SELECT_ENABLE
    if (0 != m_nSelectNum)
    {
        bDelete = DeleteSelectChar(false);
    }
    else
    {
        bDelete = DeleteChar(false);
    }
#else
    bDelete = DeleteChar(false);
#endif

    CheckImeInputText();

    if (bDelete)
    {
        signalTextConfirmChange.emitSignal(0, 0, 0, NULL);
    }

    return bDelete;
}


void xEditTextView::slotOnSoftKeyLonngPress(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    if (wParam == ST_DELETE)
    {
        SetInitText("");
    }
}

#if IF_EDIT_TEXT_VIEW_NEED_SOFTKEY
void xEditTextView::SetImeSoftkeyIndex(int nIndex)
{
    if (-1 != m_nImeSoftkeyIndex && m_nImeSoftkeyIndex != nIndex)
    {
        removeViewSoftKey(this, m_nImeSoftkeyIndex);
    }

    m_nImeSoftkeyIndex = nIndex;
}

void xEditTextView::SetDeleteSoftkeyIndex(int nIndex)
{
    if (-1 != m_nDeleteSoftkeyIndex && m_nDeleteSoftkeyIndex != nIndex)
    {
        removeViewSoftKey(this, m_nDeleteSoftkeyIndex);
    }

    m_nDeleteSoftkeyIndex = nIndex;
}

bool xEditTextView::SetupImeSoftkey()
{
    if (!m_bEnabled || -1 == m_nImeSoftkeyIndex)
    {
        return false;
    }

    setupViewSoftKey(this, m_nImeSoftkeyIndex, GetImeName(), ST_IMECHANGE, SLOT_METHOD(this, xEditTextView::slotOnSoftkeypress));

    return true;
}

bool xEditTextView::SetupDeleteSoftkey()
{
    if (!m_bEnabled || -1 == m_nDeleteSoftkeyIndex)
    {
        return false;
    }

    yl::string strDelete = g_WorkStation.Lang(TRID_DELETE);

    setupViewSoftKey(this, m_nDeleteSoftkeyIndex, strDelete, ST_DELETE,
                     SLOT_METHOD(this, xEditTextView::slotOnSoftkeypress),
                     SLOT_METHOD(this, xEditTextView::slotOnSoftKeyLonngPress));

    return true;
}

bool xEditTextView::ReleteImeSoftkey()
{
    if (-1 == m_nImeSoftkeyIndex)
    {
        return false;
    }

    removeViewSoftKey(this, m_nImeSoftkeyIndex);

    return true;
}

bool xEditTextView::ReleteDeleteSoftkey()
{
    if (-1 == m_nDeleteSoftkeyIndex)
    {
        return false;
    }

    removeViewSoftKey(this, m_nDeleteSoftkeyIndex);

    return true;
}
#endif

#if IF_EDIT_TEXT_VIEW_BIND_KEYBOARD
void xEditTextView::SetKeyboardShow(bool bShow, bool bSwitch)
{
    m_bKeyBoardShow = bShow;

    signalKeyboardShow.emitSignal((bShow ? 1 : 0), (bSwitch ? 1 : 0), 0, this);
}
#endif

bool xEditTextView::SetEnabled(bool bEnabled /*= true*/)
{
    m_bEnabled = bEnabled;

    if (m_bEnabled && hasFocus())
    {
        OnEditEnable();
    }
    else if (!m_bEnabled)
    {
        OnEditDisable();
    }

    return true;
}

bool xEditTextView::SetPassword(bool bPassword /*= true*/)
{
    m_bPassword = bPassword;
    return true;
}

bool xEditTextView::SetSuddenPassword(bool bPassword /*= true*/)
{
    m_bSuddenPassword = bPassword;
    return true;
}

yl::string xEditTextView::GetStringToPaint()
{
    if (CacheWText().empty())
    {
        return "";
    }

    if (m_strCachePaintString.empty())
    {
        //判断是否为前缀密码
        yl::wstring& pwdPrefix = m_strPartitionPwdPrefix;
        bool bPartitionPwdEnabled = !pwdPrefix.empty();
        bPartitionPwdEnabled = bPartitionPwdEnabled
                               && CacheWText().find(pwdPrefix, 0) == 0;
        //密码
        if (m_bPassword || bPartitionPwdEnabled)
        {
            yl::wstring strWText = CacheWText();
            //全密码
            if (m_bPassword)
            {
                yl::wstring::size_type pos = 0;
                int line_count = 0;
                while ((pos - line_count) < m_iCursorPos)
                {
                    pos = strWText.find(L"\n", pos);
                    if (pos != yl::wstring::npos)
                    {
                        line_count++;
                    }
                }

                yl::wstring::size_type chPos = m_iCursorPos + line_count - 1;

                //所有ch变为*
                for (yl::wstring::size_type i = 0; i < strWText.size(); ++i)
                {
                    //当立即隐藏属性为true时，不显示刚输入的字符
                    //当前字符正在输入时不隐藏
                    if (m_bSuddenPassword
                            || i != chPos
                            || !m_bShowingHideChar)
                    {
                        strWText[i] = L'*';
                    }
                }
            }
            //部分密码
            else if (bPartitionPwdEnabled)
            {
                yl::wstring::size_type pwdLength = min(m_iPartitionPwdLength, strWText.size() - pwdPrefix.size());
                strWText.replace(pwdPrefix.size(), pwdLength, pwdLength, '*');
            }

            m_strCachePaintString = strWText;
            m_cachePaintTextSize = chSize(-1, -1);

            //超长才根据风格确定显示方式
            if (CachePaintTextSize().cx > contentRect().Width()
                    && GetMultiLineEnable())
            {
                m_strCachePaintString = GetAutoReturnText(strWText);
                m_cachePaintTextSize = chSize(-1, -1);
            }
        }
        else
        {
            return xTextView::GetStringToPaint();
        }
    }

    return yl::to_utf8(m_strCachePaintString);
}

bool xEditTextView::PasswordShowChar()
{
    if (m_bPassword && m_bSuddenPassword)
    {
        if (m_bShowingHideChar)
        {
            m_tmHideChar.KillTimer();
        }

        m_bShowingHideChar = true;
        m_tmHideChar.SingleShot(1000, TIMER_METHOD(this, xEditTextView::OnPasswordHideCharTimer));
        return true;
    }

    return false;
}

bool xEditTextView::PasswordHideChar()
{
    if (m_bShowingHideChar)
    {
        m_bShowingHideChar = false;
        m_strCachePaintString.clear();
        m_tmHideChar.KillTimer();
        update();
        return true;
    }
    return false;
}

void xEditTextView::OnPasswordHideCharTimer(xThreadTimer* timer, LPVOID pExtraData)
{
    PasswordHideChar();
}

bool xEditTextView::SetPartitionPwdPrefix(yl::string strPrefix)
{
    yl::wstring wstr = yl::to_utf16(strPrefix);
    if (m_strPartitionPwdPrefix != wstr)
    {
        m_strPartitionPwdPrefix = wstr;
        m_strCachePaintString.clear();
    }
    return true;
}

bool xEditTextView::SetPartitionPwdLength(size_t iLength)
{
    if (m_iPartitionPwdLength != iLength)
    {
        m_iPartitionPwdLength = iLength;
        m_strCachePaintString.clear();
    }
    return true;
}

bool xEditTextView::SetMaxLength(int iLength)
{
    m_nMaxLength = iLength;
    return true;
}

int xEditTextView::GetMaxLength()
{
    return m_nMaxLength;
}

bool xEditTextView::SetMaxByte(int iSize)
{
    m_nMaxByte = iSize;
    return true;
}

void xEditTextView::SetMultiLineEnable(bool bMultiLineEnable)
{
    xTextView::SetMultiLineEnable(bMultiLineEnable);

    if (m_bMultiLineEnable)
    {
        // 多行，输入框只支持自动换行，设置多行后要更新超长效果必定是自动换行
        SetOverLengthStyle(OVERLENGTH_TYPE_DIRECT_CUT);
    }
}

void xEditTextView::EnsureCursorVisiable()
{
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (m_LastViewPort.IsEmpty())
    {
        chSize viewSize = contentRect().Size();
        m_LastViewPort = chRect(0, 0, viewSize.cx, viewSize.cy);
    }
    else if (contentRect().Width() > m_LastViewPort.Width())
    {
        m_LastViewPort.ResizeWidth(contentRect().Width());
    }
    if (NULL == m_pTextDocument)
    {
        return;
    }

    bool bChange = false;
    //判断光标位置是否显示
    chRect& rectViewPort = m_LastViewPort;
    // 调整最后一行
    int iContentBottom = mapLines.length(1) * m_pTextDocument->GetLineHeight();
    if (iContentBottom < rectViewPort.bottom)
    {
        int iOff = rectViewPort.bottom - iContentBottom;
        if (iOff > rectViewPort.top)
        {
            iOff = rectViewPort.top;
        }
        rectViewPort.OffsetRect(0, -iOff);
        bChange = true;
    }

    // 调整截断X轴
    if (!GetMultiLineEnable())
    {
        yl::wstring strWText = m_pTextDocument->GetStringByPos(0, mapLines.length());
        int iWidth = font.textBound(yl::to_utf8(strWText)).cx;
        if (rectViewPort.left > 0 && iWidth < rectViewPort.right)
        {
            int iOff = rectViewPort.right - iWidth;
            if (iOff > rectViewPort.left)
            {
                iOff = rectViewPort.left;
            }
            rectViewPort.OffsetRect(-iOff, 0);
            bChange = true;
        }
    }

    // 光标可见
    chRect rectCursor = GetCursorRect();

    if (rectCursor.right > m_LastViewPort.right)
    {
        rectViewPort.OffsetRect(rectCursor.right - rectViewPort.right, 0);
        bChange = true;
    }

    if (rectCursor.bottom > m_LastViewPort.bottom)
    {
        rectViewPort.OffsetRect(0, rectCursor.bottom - rectViewPort.bottom);
        bChange = true;
    }

    if (rectCursor.left < m_LastViewPort.left)
    {
        rectViewPort.OffsetRect(rectCursor.left - m_LastViewPort.left, 0);
        bChange = true;
    }

    if (rectCursor.top < m_LastViewPort.top)
    {
        rectViewPort.OffsetRect(0, rectCursor.top - m_LastViewPort.top);
        bChange = true;
    }

    if (bChange)
    {
        m_pTextDocument->AdjustVerticalHorizonOff();
    }
#endif
}

void xEditTextView::onContentChanged()
{
    xTextView::onContentChanged();

    UpdateCursorData();
}

bool xEditTextView::onRectChanged(const chRect& rectOld)
{
    bool bResult = xTextView::onRectChanged(rectOld);

    m_LastViewPort.SetEmpty();

    EnsureCursorVisiable();

    UpdateCursorData();

    return bResult;
}

void xEditTextView::UpdateCursorData()
{
#if IF_EDITTEXTVIEW_MULTI_EDIT
    if (NULL == m_pTextDocument)
    {
        return ;
    }

    CTextCursor* pCursor = m_pTextDocument->GetCurCursor();
    if (NULL != pCursor)
    {
        m_pTextDocument->InitLines();
        m_pTextDocument->UpdateCursorData(pCursor->GetCursorPos(), true);
    }
#endif
}

void xEditTextView::OnEditDisable()
{
#if IF_EDIT_TEXT_VIEW_NEED_SOFTKEY
    ReleteDeleteSoftkey();
    ReleteImeSoftkey();
#endif

    EndIme();

#if IF_MULTI_TOUCH_ENABLE
    touchable = false;
#endif

    SetCursorStatus(false);

    if (m_bContainRTLChar)
    {
        CheckRTLDirection();
    }
}

void xEditTextView::OnEditEnable()
{
#if IF_EDIT_TEXT_VIEW_NEED_SOFTKEY
    SetupImeSoftkey();
    SetupDeleteSoftkey();
#endif

#if IF_MULTI_TOUCH_ENABLE
    touchable = true;
#endif

#if IF_EDIT_TEXT_VIEW_SCROLL
    if (!m_bInScroll)
#endif
    {
        SetCursorStatus(true);

        if (m_bContainRTLChar)
        {
            CheckRTLDirection();
        }
    }
}

void xEditTextView::CheckRTLDirection()
{
    if (m_bContainRTLChar)
    {
        if (gravity_left == (gravity & gravity_left))
        {
            gravity &= ~gravity_left;
        }

        gravity |= gravity_right;

        if (align_left == (align & align_left))
        {
            align &= ~align_left;
        }

        align |= align_right;
    }
    else
    {
        if (gravity_right == (gravity & gravity_right))
        {
            gravity &= ~gravity_right;
        }

        gravity |= gravity_left;

        if (align_right == (align & align_right))
        {
            align &= ~align_right;
        }

        align |= align_left;
    }
}

void xEditTextView::OnDirectionChange()
{
    CheckRTLDirection();
}

void xEditTextView::SetCursorStatus(bool bShowCursor)
{
    m_bShowCursor = bShowCursor;
    SetCursorPosition(TextLength());

    if (m_tmCursor.IsTimerRuning())
    {
        m_tmCursor.KillTimer();
    }

    if (bShowCursor)
    {
        m_tmCursor.SetTimer(500, TIMER_METHOD(this, xEditTextView::OnFlushCursor));
    }

    update();
}

int xEditTextView::GetTextLineHeight()
{
    yl::string strText = m_strText;
    if (strText.empty())
    {
        strText = " ";
    }
    return font.textBound(strText).cy;
}
