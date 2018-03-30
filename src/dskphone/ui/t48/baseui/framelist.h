
#ifndef __FRAME_LIST_H__
#define __FRAME_LIST_H__

#include "framelistdefine.h"
#include "uikernel/framelistbase.h"
#ifdef FL_SUPPORT_TITLE
#include "baseui/framelisttitle.h"
#endif
#include "scrollctrl.h"

class QStylePainter;
class CFrameList : public CFrameListBase, public IScrollNotify
{
    Q_OBJECT

public:
    CFrameList(QWidget * parent, CScrollCtrl * pScrollCtrl = NULL);
    virtual ~CFrameList();

// IListItemDelegate
public:
#ifdef FL_SUPPORT_SCROLL_PAGE
    virtual void OnDataIndexChange(CListItem * pItem);
#endif

#ifdef FL_SUPPORT_SCROLL_SLIDE
    // 某行获得焦点
    virtual void OnItemFocusChange(CListItem * pItem, bool bPreFocus);
#endif

    virtual const char * GetFocusBg(int nDataIndex);

    virtual const char * GetClickBg(int nDataIndex);

public:
    void SetScrollViewType(int nType);

    void SetScrollShow(int nType);

    bool IsScrollByItem() const;
    bool IsScrollByPage() const;

public:
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

    virtual void resizeEvent(QResizeEvent * pEvent);

// IScrollNotify
public:
    // 通知向前滚动
    virtual void OnNotifyScrollPrevious();
    // 通知向后滚动
    virtual void OnNotifyScrollNext();

    // 列表项滚动类型
    // 可以是多个类型的|组合，如T46设置为3
    enum
    {
        SCROLL_VIEW_TYPE_NONE       = 0,
        SCROLL_VIEW_TYPE_PAGE       = 1,        // 数据按页滚动
        SCROLL_VIEW_TYPE_ITEM       = 2,        // 数据按项滚动
    };

    // 1页数据是否显示滚动条
    enum
    {
        SCROLL_SHOW_ONE_PAGE        = 0,
        SCROLL_SHOW_MULTI_PAGE      = 1,
    };

// CFrameListBase
protected:
    // 调整子控件布局
    virtual void AdjustChildWidget();
    // 响应清空数据
    virtual void OnClearContent();
    // 总数据更新
    virtual void OnDataCountChange();

#ifdef FL_SUPPORT_SCROLL_PAGE
public:
    // 设置滚动条字体大小
    void SetPageCtrlFontSize(int nFontSize, int nFontWeight = QFont::Normal);
#endif

#ifdef FL_SUPPORT_TITLE
public:
    // 设置标题栏
    void SetFrameListTitle(CFrameListTitlePtr pFrameListTitle);
    // 获取标题栏
    CFrameListTitlePtr GetFrameListTitle() const;

    // 重置Action代理
    virtual void ResetAction();
private:
    CFrameListTitlePtr m_pTitle;
#endif

#ifdef FL_SUPPORT_CUSTOM_FRAME_STYLE
public:
    // 设置分隔线
    void SetGridLine(bool bGridLine, bool bLineByContent = false);

    // 设置是否只有标题有分隔线
    void SetOnlyTitleLine(bool bOnlyTitleLine);

    // 设置标题栏与列表项是否用不同的线间隔(例：CDlgTitlePopupMenu)
    void SetCustomTitleLine(bool bCustom);

    // 设置分隔线颜色
    void SetGridLineColor(QColor & clrLine);

    // 设置列表项选中效果
    void SetItemSelectBG(const char * pszBG);

    // 设置列表项选中效果
    void SetItemSelectBG(const char * pszTopBG, const char * pszMidBG, const char * pszBottomBG);

    // 设置点击效果背景
    void SetItemClickBG(const char * pszBG);

    // 设置点击效果背景
    void SetItemClickBG(const char * pszTopBG, const char * pszMidBG, const char * pszBottomBG);

    // 设置只有一项时，是否使用最底下背景
    void SetOneItemBGBottom(bool bBottom);

    // 设置线的图片
    void SetLineImage(const char * pszImgLine);

    // 设置线的高度（目前只支持图片）
    void SetLineHeight(int nLineHeight);

    void SetLinePosY(int nLinePosY);

    virtual void paintEvent(QPaintEvent * pEvent);

    void DrawGridLine(QStylePainter & stylePainter, int nX, int nY, int nItemWidth, int nListWidth);
    void DrawFocusGrid(QStylePainter & stylePainter, const QRect & rcItem, int nItemIndex);
    void DrawClickedGrid(QStylePainter & stylePainter, const QRect & rcItem, int nItemIndex);

    void setEnableAdjust(bool bAdjust)
    {
        m_bAdjustForFkb = bAdjust;
    }
    bool enableAdjust()
    {
        return m_bAdjustForFkb;
    }
#endif

private:
    CScrollCtrl * m_pScrollCtrl;
    int m_nScrollType;
    int m_nScrollShow;
#ifdef FL_SUPPORT_CUSTOM_FRAME_STYLE
    bool m_bGridLine;
    bool m_bLineByContent;
    bool m_bOnlyTitleLine;
    bool m_bCustomTitleLine;
    bool m_bOneItemBGBottom;

    // 是否允许根据全键盘调整位置
    bool m_bAdjustForFkb;

    int m_nLinePosY;
    int m_nLineHeight;
    QColor m_clrGridLine;
    yl::string m_strItemSelTopBG;
    yl::string m_strItemSelMidBG;
    yl::string m_strItemSelBottomBG;
    yl::string m_strItemClickTopBG;
    yl::string m_strItemClickMidBG;
    yl::string m_strItemClickBottomBG;
    yl::string m_strItemLineImage;
#endif
};

#endif // FRAMELIST_H
