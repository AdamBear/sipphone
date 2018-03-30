#ifndef __X_LIST_VIEW_STYLE_H__
#define __X_LIST_VIEW_STYLE_H__

#include <xwindow/xlistviewdefine.h>
#include <xmlparser/xmlparser.hpp>
#include <ylhashmap.h>
#if IF_XLISTVIEW_SUPPORT_GROUP
#include <ylvector.h>
#endif
#include "xGraphic.h"
#include "xwindowcommon.h"
#include "xViewHandler.h"
#include "viewdumpcommon.h"


class xListView;
class xListViewItem;
class xListViewStyle;
class xListViewSectionStyle;

#if IF_XLISTVIEW_SUPPORT_GROUP
enum
{
    LIST_VIEW_STYLE_ITEMS_STYLE_NORMAL      = 0,
    LIST_VIEW_STYLE_ITEMS_STYLE_GROUP       = 1,
};

class xListViewSection : public xTextTranslate
{
    friend class xListViewStyle;
    friend class xListViewSectionStyle;

public:
    xListViewSection();
    virtual ~xListViewSection();

public:
    virtual void SetTranText(const yl::string& strText);

    yl::string GetShowText(xFont& fontText, int nShowWidth);

    inline void SetSectionPos(int nPos)
    {
        m_nSectionPos = nPos;
    }

    inline void SetSectionHeight(int nSectionHeight)
    {
        m_nSectionHeight = nSectionHeight;
    }

    inline void SetSectionSpace(int nSpace)
    {
        m_nSectionSpace = nSpace;
    }

    inline void SetTitleHeight(int nHeight)
    {
        m_nTitleHeight = nHeight;
    }

    inline void SetDataIndex(int nBeginIndex, int nEndIndex)
    {
        m_nBeginIndex = nBeginIndex;
        m_nEndIndex = nEndIndex;
    }

    void SetTitle(const yl::string& strTitle);

    inline int GetSectionPos() const
    {
        return m_nSectionPos;
    }

    inline int GetSectionHeight() const
    {
        return m_nSectionHeight;
    }

    inline int GetSectionSpace() const
    {
        return m_nSectionSpace;
    }

    inline int GetTitleHeight() const
    {
        return m_nTitleHeight;
    }

    inline int GetBeginIndex() const
    {
        return m_nBeginIndex;
    }

    inline int GetEndIndex() const
    {
        return m_nEndIndex;
    }

    inline const yl::string& GetTitle() const
    {
        return m_strTitle;
    }

    inline bool IsHadSectionTitle() const
    {
        return (0 != m_nSectionSpace || 0 != m_nTitleHeight);
    }

    inline int GetTotalTitleHeight() const
    {
        return (m_nSectionSpace + m_nTitleHeight);
    }

    inline int GetTitleYPos(int nContentPos) const
    {
        return (m_nSectionPos + m_nSectionSpace - nContentPos);
    }

    bool IsTitleShow(int nContentPos, int nContentHeight);

protected:
    int m_nSectionPos;
    int m_nSectionHeight;
    int m_nSectionSpace;
#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE
    int m_nSectionDrift;
#endif
    int m_nTitleHeight;
    int m_nBeginIndex;
    int m_nEndIndex;
    yl::string m_strTitle;
};
typedef xListViewSection* xListViewSectionPtr;
typedef YLVector<xListViewSectionPtr> VEC_LIST_VIEW_SECTION;

enum
{
    LIST_VIEW_SECTION_STYLE_DEFAULT     = 0,
};

class xListViewSectionStyle
{
public:
    xListViewSectionStyle();
    virtual ~xListViewSectionStyle();

public:
    virtual void loadAttributes(xml_node& node);

    virtual void PaintSection(xPainter& painter, xListViewSection* pSection, xListViewSection* pNextSection,
                              xListViewStyle* pStyle, const chRect& rcContent);

    inline void SetTitleBG(const xBrush& brushTitleBG)
    {
        m_brushTitleBG = brushTitleBG;
    }

    inline void SetTitleBG(const xPixmap& pmTitleBG)
    {
        m_brushTitleBG.setPixmap(pmTitleBG);
    }

    inline void SetTextColor(const xColor& clrText)
    {
        m_clrText = clrText;
    }

    inline void SetTextFont(const xFont& fontText)
    {
        m_fontText = fontText;
    }

    inline void SetTextFontSize(int nFontSize)
    {
        m_fontText.setPointSize(nFontSize);
    }

    inline void SetTextFontBold(bool bBold)
    {
        m_fontText.setBold(bBold);
    }

    inline void SetTextAlign(int nAlign)
    {
        m_nTextAlign = nAlign;
    }

    inline void SetTextMargin(int nMargin)
    {
        m_nTextMargin = nMargin;
    }

    inline const xBrush& GetTitleBG() const
    {
        return m_brushTitleBG;
    }

    inline const xColor& GetTextColor() const
    {
        return m_clrText;
    }

    inline const xFont& GetTextFont() const
    {
        return m_fontText;
    }

    inline int GetTextAlign() const
    {
        return m_nTextAlign;
    }

    inline int GetTextMargin() const
    {
        return m_nTextMargin;
    }

#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
protected:
    void PaintItemSpaceLine(xPainter& painter, xListViewSection* pSection, xListViewSection* pNextSection,
                            xListViewStyle* pStyle, const chRect& rcContent);
#endif

protected:
    xBrush m_brushTitleBG;
    xColor m_clrText;
    xFont m_fontText;
    int m_nTextAlign;
    int m_nTextMargin;
};
typedef xListViewSectionStyle* xListViewSectionStylePtr;
#endif

enum
{
    LIST_VIEW_STYLE_TYPE_DEFAULT        = 0,
};

#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
DECALRE_RUNTIME_FAMILY(xItemBGPainter);
#define EXPORT_ITEM_BG_PAINTER_CLASS(name) \
    static xItemBGPainter* method_of_get_##name() {\
    static name tmp_obj_of_##name; return &tmp_obj_of_##name;\
} DECLARE_EXPORT ClassNode_xItemBGPainter s_Node_Of_##name(#name, method_of_get_##name)

class xListViewStyle;
class xItemBGPainter
{
    DECALRE_GET_OBJECT_METHOD(xItemBGPainter);

public:
    xItemBGPainter();
    virtual ~xItemBGPainter();

    enum
    {
        ITEM_BG_PAINTER_TYPE_NONE           = 0,
    };

public:
    virtual void DrawItemBG(xPainter& painter, xListViewStyle* pStyle, xListViewItem* pFocusItem,
                            const chRect& rcItem, int nItemIndex) = 0;
    virtual yl::string GetPainterName();

public:
    inline void SetType(int nType)
    {
        m_nType = nType;
    }

    inline int GetType() const
    {
        return m_nType;
    }

protected:
    int m_nType;
};
typedef xItemBGPainter* xItemBGPainterPtr;
#endif

class xListViewStyle
{
    DECLARE_VIEW_DUMP_CLASS(xListView)

#if IF_XLISTVIEW_SUPPORT_SLIDE
    friend class xListSlideView;
#endif

public:
    explicit xListViewStyle(xListView* pListView, int nType = LIST_VIEW_STYLE_TYPE_DEFAULT);
    virtual ~xListViewStyle();

    static xListViewStyle* CreateListViewStyle(xListView* pListView);

public:
    // 设置列表框没有Item焦点
    inline void SetNoneFocus()
    {
        m_nFocusType = ITEM_FOCUS_TYPE_NONE;
    }

    // 设置列表框不默认设置焦点
    inline void SetManualFocus()
    {
        m_nFocusType = ITEM_FOCUS_TYPE_MANUAL;
    }

    // 设置列表框中默认设置焦点
    inline void SetAutoFocus()
    {
        m_nFocusType = ITEM_FOCUS_TYPE_AUTO;
    }

    // 是否自动设置焦点
    inline bool IsAutoFocusItem() const
    {
        return (ITEM_FOCUS_TYPE_AUTO == m_nFocusType);
    }

    // 是否有Item焦点
    inline bool IsItemFocusable() const
    {
        return (ITEM_FOCUS_TYPE_NONE != m_nFocusType);
    }

    // 设置每页列表项数量
    void SetPageItemCount(int nPageCount);
    // 获得每页列表项数量
    inline int GetPageItemCount() const
    {
        return m_nPageItemCount;
    }

    void SetContentHeight(int nHeight);
    void SetContentPos(int nContentPos);

    inline void SetPageHeight(int nHeight)
    {
        m_nPageHeight = nHeight;
    }

    inline void SetItemTopMargin(int nTopMargin)
    {
        m_nItemTopMargin = nTopMargin;
    }

    inline void SetItemBottomMargin(int nItemBottomMargin)
    {
        m_nItemBottomMargin = nItemBottomMargin;
    }

    inline void SetItemLeftMargin(int nLeftMargin)
    {
        m_nItemLeftMargin = nLeftMargin;
    }

    inline void SetItemRightMargin(int nRightMargin)
    {
        m_nItemRightMargin = nRightMargin;
    }

    inline void SetItemWidth(int nWidth)
    {
        m_nItemWidth = nWidth;
    }

    inline void SetItemHeight(int nItemHeight)
    {
        m_nItemHeight = nItemHeight;
    }

    inline void SetItemSpace(int nItemSpace)
    {
        m_nItemSpace = nItemSpace;
    }

    void SetScrollCtrlMargin(int nTopMargin, int nBottomMargin, int nRightMargin);

    void SetScrollCtrlFixPos(bool bFixPos);
    void SetHintFixPos(bool bFixPos);

    inline void SetScrollViewType(int nType)
    {
        m_nScrollType = nType;
    }

    inline xListView* GetListView() const
    {
        return m_pListView;
    }

    inline int GetContentHeight() const
    {
        return m_nContentHeight;
    }

    inline int GetPageHeight() const
    {
        return m_nPageHeight;
    }

    inline int GetContentPos() const
    {
        return m_nContentPos;
    }

    inline int GetItemHeight() const
    {
        return m_nItemHeight;
    }

    inline int GetItemSpace() const
    {
        return m_nItemSpace;
    }

    inline int GetItemLeftMargin() const
    {
        return m_nItemLeftMargin;
    }

    inline int GetItemRighMargin() const
    {
        return m_nItemRightMargin;
    }

    int GetMaxContentPos() const;

    inline bool IsScrollCtrlFixPos() const
    {
        return (LIST_VIEW_FIX_POS_SCROLL_CTRL == (LIST_VIEW_FIX_POS_SCROLL_CTRL & m_nFixPosFlag));
    }

    inline bool IsHintFixPos() const
    {
        return (LIST_VIEW_FIX_POS_HINT == (LIST_VIEW_FIX_POS_HINT & m_nFixPosFlag));
    }

    inline bool IsScrollByItem() const
    {
        return (SCROLL_VIEW_TYPE_ITEM == (SCROLL_VIEW_TYPE_ITEM & m_nScrollType));
    }

    bool IsScrollByPage() const
    {
        return (SCROLL_VIEW_TYPE_PAGE == (SCROLL_VIEW_TYPE_PAGE & m_nScrollType));
    }

#if IF_XLISTVIEW_SUPPORT_TITLE
    // 设置Title与Item之间的间隔
    inline void SetTitleSpace(int nTitleSpace)
    {
        m_nTitleSpace = nTitleSpace;
    }

    // 设置Title与Item之间线类型
    inline void SetTitleLineType(int nLineType)
    {
        m_nTitleLineType = nLineType;
    }

    // 设置Title与Item之间线位置类型
    inline void SetTitleLinePosType(int nLinePosType)
    {
        m_nTitleLinePosType = nLinePosType;
    }

    enum
    {
        LIST_TITLE_LINE_POS_TYPE_NONE           = 0,
        LIST_TITLE_LINE_POS_TYPE_IN_SPACE       = 0x01,     // 在间隔中根据间隔大小画线
        LIST_TITLE_LINE_POS_TYPE_IN_TITLE       = 0x02,     // 在Title底部根据Item间隔大小画线
        LIST_TITLE_LINE_POS_TYPE_IN_ITEM        = 0x04,     // 在Item顶部根据Item间隔大小画线
        LIST_TITLE_LINE_POS_TYPE_BY_TITLE       = 0x10,     // 根据Title的长度画线
        LIST_TITLE_LINE_POS_TYPE_BY_CONTENT     = 0x00,     // 根据内容的位置画线
    };

    enum
    {
        LIST_TITLE_LINE_TYPE_NONE               = 0,        // 没有分隔线
        LIST_TITLE_LINE_TYPE_ALWAYS             = 1,        // 一直有Title线
        LIST_TITLE_LINE_TYPE_EXIST_WITH_ITEM    = 2,        // 有Item时才有分隔线
    };
#endif

    static void SetGlobalItemStyle(const yl::string& strItemStyle);

    inline void SetItemStyle(const yl::string& strItemStyle)
    {
        m_strItemStyle = strItemStyle;
    }

    yl::string GetItemStyle() const;

#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
    inline void SetItemLineColor(const xColor& clrLine)
    {
        m_clrSpaceLine = clrLine;
    }

    inline void SetItemLineStyle(int nLineStyle)
    {
        m_nSpaceLineStyle = nLineStyle;
    }

    inline void SetItemLineIgnoreEnd(bool bIgnore)
    {
        m_bSpaceLineIgnoreEnd = bIgnore;
    }

    inline bool IsItemLineIgnoreEnd() const
    {
        return m_bSpaceLineIgnoreEnd;
    }
#endif

#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
    static void SetDefaultItemBGPainter(const yl::string& strPainter);
    void SetItemBGPainter(const yl::string& strPainter);
    inline xItemBGPainter* GetItemBGPainter() const
    {
        return m_pItemBGPainter;
    }
#endif

#if IF_XLISTVIEW_SUPPORT_GROUP
    void SetItemsStyle(int nItemsStyle);
    inline void SetGroupSectionCount(int nCount)
    {
        m_nSectionCount = nCount;
    }

    void SetGroupSectionStyle(int nStyle);

    inline xListViewSectionStylePtr GetGroupSectionStyle() const
    {
        return m_pSectionStyle;
    }
#endif

    virtual int GetDataIndexByPos(int nContentPos);
    virtual int GetContentPosByIndex(int nDataIndex);

protected:
    // 绘制xListView
    // xListView中Item、ScrollCtrl、Title的布局
    virtual void loadAttributes(xml_node& node);

    virtual void OnFinishInit();

    virtual void AdjustContentPosAndIndex(int& nDataIndex);

    virtual void Relayout();

    virtual bool GetItemLayoutInfo(int nItemIndex, int nDataIndex,
                                   int& nLeftMargin, int& nTopMargin,
                                   int& nWidth, int& nHeight);

#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
    virtual void OnItemFocus(xListViewItem* pItem);
#endif

#if IF_XLISTVIEW_SUPPORT_TITLE
    virtual void PaintTitle(xPainter& painter);
#endif

    virtual void PaintContent(xPainter& painter);

#if IF_XLISTVIEW_FOCUS_INVERT_EFFECT
    virtual void PaintMask(xPainter& painter);
#endif

#if IF_XLISTVIEW_SUPPORT_TITLE
    void DrawTitleSpaceLine(xPainter& painter);
#endif

#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
    void DrawItemSpaceLine(xPainter& painter);
#endif

#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
    void DrawFocusItemBG(xPainter& painter);
#endif

    bool GetContentGeometry(chRect& rcContent);
    bool GetContentRect(chRect& rcContent);
    bool GetContainerGeometry(chRect& rcContainer);

#if IF_XLISTVIEW_SUPPORT_TITLE
    bool LayoutTitle(chRect& rcContent, bool bInLayout = false);
#endif

    void LayoutScrollCtrl(const chRect& rcContent);
    void LayoutNote(const chRect& rcContent);

#if IF_XLISTVIEW_SUPPORT_SLIDE
    void LayoutSlideView(const chRect& rcContent);

    virtual bool ShiftContentPos(int nOffset, int& nDataIndex, bool bCheckBound);

#if IF_XLISTVIEW_SUPPORT_GROUP
    bool GetSectionShiftPos(int nContentPos, int& nDataIndex, int& nItemOffset);
#endif
    bool IsItemShowFully(xListViewItem* pItem);
#endif

#if IF_XLISTVIEW_SUPPORT_GROUP
    virtual void InitGroupSection();

    void ClearGroupSection();
    void DeleteSectionStyle();

    xListViewSectionPtr GetSectionByIndex(int nDataIndex);
    xListViewSectionPtr GetSectionByPos(int nSectionPos);

#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE
    inline bool IsSectionDrift() const
    {
        return m_bSectionDrift;
    }

    void SetSectionDrift(bool bDrift);
    void ResetSectionDrift();
    void DriftSection(int nBeginIndex, int nEndIndex, int nOffset);
    void AddUpNextSecDrift(int nBeginIndex, int nEndIndex, int nOffset);
#endif
#endif

    // 向前翻页
    bool OnPrePage(bool bCycle = true);
    // 向后翻页
    bool OnNextPage(bool bCycle = true);
    // 获取当前页码(从0开始)
    int GetCurPage();
    // 向前翻一整页
    bool OnPreFullPage(bool bCycle = true);
    // 向后翻一整页
    bool OnNextFullPage(bool bCycle = true);
    // 按优先级设置焦点
    void FocusPriorItem(int nDataIndex, int nIndex);
    // 按项向前滚动一项（只要有按项滚动都在这函数处理，如：T46中单页内按项滚动，翻页时按页滚动）
    bool OnPreItem(bool bScrollByPage, bool bCycle = true);
    // 按项向后滚动一项（只要有按项滚动都在这函数处理，如：T46中单页内按项滚动，翻页时按页滚动）
    bool OnNextItem(bool bScrollByPage, bool bCycle = true);

    bool GetPreItemContentPos(int nPreDataIndex, int& nContentPos);
    bool GetNextItemContentPos(int nDataIndex, int nNextDataIndex, int& nContentPos);

protected:
    friend class xListView;

    xListView* m_pListView;

    int m_nType;

    // 列表项滚动类型
    // 可以是多个类型的|组合，如T46设置为3
    enum
    {
        SCROLL_VIEW_TYPE_NONE       = 0,
        SCROLL_VIEW_TYPE_PAGE       = 1,        // 数据按页滚动
        SCROLL_VIEW_TYPE_ITEM       = 2,        // 数据按项滚动
    };

    int m_nScrollType;

    enum
    {
        ITEM_FOCUS_TYPE_NONE        = 0,        // 没有焦点
        ITEM_FOCUS_TYPE_MANUAL      = 1,        // 外部设置焦点才有焦点
        ITEM_FOCUS_TYPE_AUTO        = 2,        // 默认焦点规则
    };

    // 焦点类型
    int m_nFocusType;

    // 每页可容纳的行数
    int m_nPageItemCount;

    // 列表框显示内容在所有内容中的位置
    int m_nContentPos;
    // 列表框所有内容的高度
    int m_nContentHeight;
    // 列表框一页所能容纳的高度
    int m_nPageHeight;
    // 列表项上边距
    int m_nItemTopMargin;
    // 底端的边距，用于wrap_content情况下的保持边距
    int m_nItemBottomMargin;
    // 列表项左边距
    int m_nItemLeftMargin;
    // 列表项右边距
    int m_nItemRightMargin;
    // 列表项宽度
    int m_nItemWidth;
    // 列表项高度
    int m_nItemHeight;
    // 列表项间隔
    int m_nItemSpace;
    // 滚动条上边距
    int m_nScrollCtrlTopMargin;
    // 滚动条下边距
    int m_nScrollCtrlBottomMargin;
    // 滚动条右边距
    int m_nScrollCtrlRightMargin;

#if IF_XLISTVIEW_FOCUS_ITEM_BG_PAINTER
    static xItemBGPainterPtr s_pDefaultItemBGPainter;
    xItemBGPainterPtr m_pItemBGPainter;
#endif

#if IF_XLISTVIEW_SUPPORT_TITLE
    int m_nTitleSpace;
    int m_nTitleLineType;
    int m_nTitleLinePosType;
#endif

    enum
    {
        LIST_VIEW_FIX_POS_NONE          = 0,
        LIST_VIEW_FIX_POS_SCROLL_CTRL   = 1,
        LIST_VIEW_FIX_POS_HINT          = 2,
    };

    int m_nFixPosFlag;

#if IF_XLISTVIEW_ITEM_SPACE_BY_LINE
    xColor m_clrSpaceLine;
    int m_nSpaceLineStyle;
    bool m_bSpaceLineIgnoreEnd;
#endif

#if IF_XLISTVIEW_SUPPORT_GROUP
    friend class xListViewSection;
    friend class xListViewSectionStyle;

    xListViewSectionStyle* m_pSectionStyle;
    int m_nItemsStyle;
    int m_nSectionCount;

    VEC_LIST_VIEW_SECTION m_vecSection;
#if IF_XLISTVIEW_SUPPORT_ITEM_FLATE
    bool m_bSectionDrift;
#endif
#endif

    static yl::string s_strItemStyle;

    yl::string m_strItemStyle;

#if IF_XLISTVIEW_SUPPORT_SLIDE
    int m_nItemOffset;
#endif
};
typedef xListViewStyle* xListViewStylePtr;


typedef xListViewStyle* (*FUN_CREATE_LIST_STYLE)(xListView*);
#if IF_XLISTVIEW_SUPPORT_GROUP
typedef xListViewSectionStyle* (*FUN_CREATE_SECTION_STYLE)();
#endif

class xListViewStyleFactory
{
public:
    xListViewStyleFactory();
    ~xListViewStyleFactory();

    static xListViewStyleFactory& GetCurrentStyleFactory();

public:
    void RegisterListStyle(int nStyleType, FUN_CREATE_LIST_STYLE pFunCreateListStyle);
    xListViewStyle* CreateListStyle(int nStyleType, xListView* pListView);

#if IF_XLISTVIEW_SUPPORT_GROUP
    void RegisterSectionStyle(int nStyle, FUN_CREATE_SECTION_STYLE pFunCreateSectionStyle);
    xListViewSectionStyle* CreateSectionStyle(int nStyle);
#endif

protected:
    typedef YLHashMap<int, FUN_CREATE_LIST_STYLE> MAP_CREATE_LIST_STYLE_FUN;

    MAP_CREATE_LIST_STYLE_FUN m_mapListStyleFun;

#if IF_XLISTVIEW_SUPPORT_GROUP
    typedef YLHashMap<int, FUN_CREATE_SECTION_STYLE> MAP_CREATE_SECTION_STYLE_FUN;

    MAP_CREATE_SECTION_STYLE_FUN m_mapSectionStyleFun;
#endif
};

#define currentStyleFactory xListViewStyleFactory::GetCurrentStyleFactory()

#endif
