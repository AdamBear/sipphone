#ifndef __EDIT_TEXT_DOCUMENT_H__
#define __EDIT_TEXT_DOCUMENT_H__

#include "xtextcursor.h"
#include "ylstring.h"
#include "xwindow/xGraphic.h"
#include "xfragmentmap.h"

#include "xwindow/xGraphic.h"

#include "ximeedit.h"

#if IF_EDITTEXTVIEW_MULTI_EDIT

class xEditTextView;

typedef yl::wstring::value_type   IMECHAR;

// 修改xFragment<2>中数字时，需要同步修改cpp中的 DEFAULT_N 变量值
class CTextLineData : public xFragment<2>
{
public:
    inline void initialize()
    {
        m_iLineTextWidth = 0;
    }
    void invalidate() const {}
    inline void free()
    {
        m_iLineTextWidth = 0;
    }

    int m_iLineTextWidth;
};

typedef xFragmentMap<CTextLineData> TextLineMap;

class CTextDocument
{
public:
    // 光标移动类型
    enum
    {
        CURSOR_OP_MOVE_UP,
        CURSOR_OP_MOVE_DOWN,
    };

public:
    explicit CTextDocument(xEditTextView* pEditView = NULL);
    ~CTextDocument();

public:
    // 初始化
    void InitText(const yl::wstring& strWText);
    void InitLines();

public:
    // 水平、竖直偏移相关
    int VerticalOffset();
    void SetVerticalOffset(float fVOff);
    int HorizoncalOffset();
    void SetHorizoncalOffset(float fHOff);

    float GetViewportV2VVarRate();
    float GetVVar2ViewportVRate();
    float GetViewportH2HVarRate();
    float GetHVar2ViewportHRate();

public:
    const yl::wstring& GetWStringToPaint();
    bool onPaintMultiEditText(xPainter& p);

public:
    // 属性相关
    void SetLineHeight(int nLineHeight);
    int GetLineHeight();
    void SetViewportWidth(int nViewportWidth);
    int GetViewportWidth();
    void SetTopLeftLine(int iTopLeftLine);
    int GetTopLeftLine();
    void SetLeftHorizontalPos(int iLeftHorizontalPos);
    int GetLeftHorizontalPos();

    void SetContentRect(chRect& rcContent);
    chRect GetContentRect();

    void WTextString(const yl::wstring& strText);
    const yl::wstring& GetWTextString();
    yl::wstring GetStringByPos(int iPos, int iNum = 1);
    bool RemoveString(int iPos, int iNum = 1);
    bool InsertString(int iPos, yl::wstring& strInsertString);
    bool InsertString(int iPos, IMECHAR ch);
    bool ReplaceString(int iPos, int iCnt, int iCntTwo, IMECHAR ch);
    bool IsWTextEmpty();
    int TextLength();

    void SetEditView(xEditTextView* pEditView);

    // 调整宽度和高度
    void AdjustWidgetSize();

public:
    const TextLineMap &GetTextLineMap() const
    {
        return m_lines;
    }
    TextLineMap &GetTextLineMap()
    {
        return m_lines;
    }

public:
    void AdjustVerticalHorizonOff();
    void SetVerticalHorizonOff(int iV, int iH);
    void AdjustViewportOff();

    int GetLeftDistance(int iContentWidth, int iTextWidth);

public:
    // 更新显示的首行序号
    void UpdateFirstVisiableLine(int iYOff);
    // 合并、分离行
    void Unite(int iPos);
    void Split(int iPos);

    void UpdateLineText(int iCharPos);

    // 光标操作
    CTextCursor* GetCurCursor();
    void UpdateCursorData(int iNewCursorPos, bool bForce = false);

    int GetMoveStep(int eOp);
    int GetMoveStep(int iCharPos, int iCharNum, int iXOff, int iCursorPos, int iLeftBalance);

    //获取当前字符的光标位置和输入法位置
    chRect GetCursorRect();
    chRect GetRTLCursorRect();

    chRect GetImeCharRect();
    chRect GetRTLImeCharRect();

    int GetRTLCursorIndex(const yl::wstring& strPaintWString);

private:
    bool SetCurCursor(CTextCursor& textCursor);

    // 二分查找靠左位置
    int BinarySearch(const yl::wstring& strWText, int iRegisterWidth, int iUpperLimit, int iLeftBlank = 0);

    void UpdateCacheText();

    yl::string GetPaintStringByPos(int iPos, int iNum = 1);

    void DrawLTRContent(xPainter& p, const yl::wstring& strPaintWString, const chRect& rcViewPort);
    void DrawRTLContent(xPainter& p, const yl::wstring& strPaintWString, const chRect& rcViewPort);

private:
    // 偏移量
    float                       m_fHVar;
    float                       m_fVVar;
    float                       m_fViewportV2VVarRate;
    float                       m_fVVar2ViewportVRate;
    float                       m_fViewportH2HVarRate;
    float                       m_fHVar2ViewportHRate;
    // 光标
    CTextCursor*                m_pCursor;
    // 行
    int                         m_iTopLeftLine;
    TextLineMap                 m_lines;
    // 文本
    yl::wstring                 m_strCacheWText;
    // 属性
    int                         m_nLineHeight;              // 行高
    int                         m_nViewportWidth;           // 显示屏宽度
    chRect                      m_rcContent;
    int                         m_iLeftHorizontalPos;       // 上下切光标时，当前光标相对左边距距离

    xEditTextView*              m_pEditView;
    bool                        m_bHasInit;
};

#endif // if IF_EDITTEXTVIEW_MULTI_EDIT

#endif


