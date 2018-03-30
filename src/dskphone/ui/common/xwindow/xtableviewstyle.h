#ifndef __X_TABLE_VIEW_STYLE_H__
#define __X_TABLE_VIEW_STYLE_H__

#include <xwindow/xwindowdefine.h>
#include "xlistviewstyle.h"
#include <xwindow/xtableviewdefine.h>
#include "viewdumpcommon.h"

#if IF_TABLE_VIEW_ENABLE

enum
{
    LIST_VIEW_STYLE_TYPE_TABLE_VIEW     = LIST_VIEW_STYLE_TYPE_DEFAULT + 1,
};

class xTableView;
class xTableViewStyle : public xListViewStyle
{
    DECLARE_VIEW_DUMP_CLASS(xTableView)

public:
    explicit xTableViewStyle(xTableView* pTableView);
    virtual ~xTableViewStyle();

    static xListViewStyle* CreateTableViewStyle(xListView* pTableView);
    static void RegisterStyle();

public:
    void SetTableView(xTableView* pTableView);
    xTableView* GetTableView();

    inline void SetColumn(int nColumn)
    {
        m_nColumn = nColumn;
    }

    inline int GetColumn() const
    {
        return m_nColumn;
    }

    inline void SetColumnSpace(int nColSpace)
    {
        m_nColSpace = nColSpace;
    }

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    void SetHorizontalColumn(bool bHorizontal);

    inline bool IsHorizontalColumn() const
    {
        return TAB_COL_ORNT_HORIZONTAL == m_nOrientation;
    }
#endif

#if IF_XTABLEVIEW_COLUMN_SPACE_BY_LINE
    enum
    {
        TAB_BOUND_LINE_TYPE_NONE            = 0,
        // 水平边框线
        TAB_BOUND_LINE_TYPE_HORIZONTAL      = 1,
        // 垂直边框线
        TAB_BOUND_LINE_TYPE_VERTICAL        = 2,
        // 四周边框线
        TAB_BOUND_LINE_TYPE_BOTH            = 3,
    };

    inline void SetBoundLineType(int nBoundLineType)
    {
        m_nBoundLineType = nBoundLineType;
    }

    inline bool IsHorizontalBoundLine() const
    {
        return (TAB_BOUND_LINE_TYPE_HORIZONTAL == (TAB_BOUND_LINE_TYPE_HORIZONTAL & m_nBoundLineType));
    }

    inline bool IsVerticalBoundLine() const
    {
        return (TAB_BOUND_LINE_TYPE_VERTICAL == (TAB_BOUND_LINE_TYPE_VERTICAL & m_nBoundLineType));
    }
#endif

    virtual int GetDataIndexByPos(int nContentPos);
    virtual int GetContentPosByIndex(int nDataIndex);

protected:
    virtual void loadAttributes(xml_node& node);

    virtual void OnFinishInit();

    virtual void AdjustContentPosAndIndex(int& nDataIndex);

    virtual bool GetItemLayoutInfo(int nItemIndex, int nDataIndex,
                                   int& nLeftMargin, int& nTopMargin,
                                   int& nWidth, int& nHeight);

#if IF_XLISTVIEW_SWITCH_ITEM_FOCUS
    virtual void OnItemFocus(xListViewItem* pItem);
#endif

#if IF_XTABLEVIEW_COLUMN_SPACE_BY_LINE
#if IF_XLISTVIEW_SUPPORT_TITLE
    virtual void PaintTitle(xPainter& painter);
#endif
    virtual void PaintContent(xPainter& painter);

    void DrawItemSpaceLine(xPainter& painter, const chRect& rcTableView, const chRect& rcContent, int& nRowWidth, int& nColHeight);
#endif

#if IF_XLISTVIEW_ITEM_CUSTOM_WIDTH
    bool GetItemLayoutInfoVertCol(int nItemIndex, int nDataIndex,
                                  int& nLeftMargin, int& nTopMargin,
                                  int& nWidth, int& nHeight);
#endif

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    bool GetItemLayoutInfoHorzCol(int nItemIndex, int nDataIndex,
                                  int& nLeftMargin, int& nTopMargin,
                                  int& nWidth, int& nHeight);
#endif

#if IF_MOVE_FOCUS_BY_KEY
    bool IsLeftMostItem(int nFocusDataIndex, int nFirstDataIndex);
    bool IsTopMostItem(int nDataIndex);
    bool IsRightMostItem(int nFocusDataIndex, int nFirstDataIndex);
    bool IsBottomMostItem(int nDataIndex);

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    int GetFocusReasonHorzCol(int nFocusReason);
#endif

    int GetColFirstDataIndex(int nDataIndex, int nMaxIndex);
    int GetColLastDataIndex(int nDataIndex, int nMaxIndex);
    int GetRowFirstDataIndex(int nDataIndex, int nMaxIndex);
    int GetRowLastDataIndex(int nDataIndex, int nMaxIndex);

    int GetFocusDataIndex(int nDataIndex, int nFocusReason);
    int GetCycleFocusDataIndex(int nDataIndex, int nFocusReason);
#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    int GetCycleFocusDataIndexHorzCol(int nDataIndex, int nFocusReason);
#endif
#endif

#if IF_XLISTVIEW_SUPPORT_SLIDE
    virtual bool ShiftContentPos(int nOffset, int& nDataIndex, bool bCheckBound);
#endif

protected:
    friend class xTableView;

    // 列的数量
    int m_nColumn;
    // 列的间隔(竖向)
    int m_nColSpace;

#if IF_XTABLE_VIEW_HORIZONTAL_COLUMN
    enum
    {
        // 列是垂直方向（默认值）
        TAB_COL_ORNT_VERTICAL               = 1,
        // 列是水平方向
        TAB_COL_ORNT_HORIZONTAL             = 2,
    };

    // 列的方向
    int m_nOrientation;
#endif

#if IF_XTABLEVIEW_COLUMN_SPACE_BY_LINE
    // 边框线
    int m_nBoundLineType;
#endif
};
typedef xTableViewStyle* xTableViewStylePtr;
#endif

#endif
