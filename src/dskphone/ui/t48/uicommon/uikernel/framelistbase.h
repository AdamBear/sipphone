#ifndef __FRAME_LIST_BASE_H__
#define __FRAME_LIST_BASE_H__

#include <ylstring.h>
#include <QString>
#include "ilistitemdelegate.h"
#include "listitem.h"

#define DEFAULT_PAGE_ITEM_COUNT 6
#define INVALID_FRAME_LIST_PAGE_NO -1

class QLabel;
class IFrameListDelegate;

class CFrameListBase : public QWidget, public IListItemDelegate
{
    Q_OBJECT

public:
    CFrameListBase(QWidget * parent);
    virtual ~CFrameListBase();

public:
    // 设置FrameList位置
    void SetRect(int nX, int nY, int nWidth, int nHeight);
    void SetRect(const QRect & rect);

    // 设置列表项起始边矩
    void SetItemMargin(int nLeft, int nTop, int nRight);

    // 设置列表项间隔
    void SetItemSpace(int nSpace);

    // 设置列表项内容位置
    void SetContentPos(int nLeftMargin, int nRightMargin);

    // 设置默认选中
    void SetDefaultFocus(bool bFocus);

    // 设置提示信息
    void SetNote(const yl::string & strNote);
    void SetNote(const QString & strNode);
    // 设置提示信息颜色
    void SetNoteFontColor(const QColor & color);
    // 设置提示信息字体大小
    void SetNoteFontSize(int nFontSize, int nFontWeight = QFont::Normal);
    // 设置固定位置
    void SetNoteFixRect(int nLeft, int nTop, int nWidth, int nHeight);

    // 设置列表数据代理(会通知原有的delegate解除绑定、并通知新代理绑定)
    void SetDelegate(IFrameListDelegate * pDelegate, bool bLoadData = false);

    // 重置代理(只是清空代理，不通知代理解除绑定)
    void ResetDelegate();

    // 重置Action代理
    virtual void ResetAction();

    // 刷新数据
    void RefreshData(int nDataIndex);

    // 设置每页列表项数量
    void SetMaxPageItemCount(int nMaxCount);
    // 获得每页列表项数量
    int GetMaxPageItemCount();

    // 获得当前显示的列表项数量
    int GetTotalItemCount() const;

    // 设置数据总数量
    void SetTotalDataCount(int nTotalCount);
    // 获得数据总数量
    int GetTotalDataCount() const;

    // 获得选中行号
    int GetFocusItemIndex() const;
    // 根据行号选中一行，传INVALID_ITEM_INDEX时将取消选中(会通知外部处理选中)
    void SelectItemByIndex(int nIndex);
    // 根据行号设置焦点
    void FocusItemByIndex(int nIndex);
    // 设置选中行（不会通知外部处理选中）
    void SetItemFocusByIndex(int nIndex);

    // 获取选中行的数据索引
    int GetFocusItemDataIndex();
    // 根据数据索引选中一行，传INVALID_ITEM_INDEX时将取消选中
    void SelectItemByDataIndex(int nIndex);
    // 根据数据索引设置焦点
    void FocusItemByDataIndex(int nIndex);

    // Detach后，标识自动清除
    void SetItemAttach();

    bool IsItemAttach() const;

    // 添加列表项（注意判断返回值，为false时要注意释放指针）
    virtual bool AddItem(CListItemPtr pListItem, bool bRefresh = false);

    // 清理所有的内容
    virtual void ClearContent(bool bRefresh = false);

    // 清除所有的内容，但不释放列表项(用于外部托管列表项的情况)
    void DetachAllItem(bool bRefresh = false);

    // 根据行号删除一行（成功返回true，失败返回false）
    virtual bool DelItemByIndex(int nIndex, bool bRefresh = false);
    // 根据数据索引删除一行（成功返回true，失败返回false）
    virtual bool DelItemByDataIndex(int nIndex, bool bRefresh = false);

    // 根据行号获得相应的列表项
    CListItemPtr GetItemByIndex(int nIndex);
    // 根据数据索引获得相应的列表项
    CListItemPtr GetItemByDataIndex(int nIndex);
    // 获得焦点行的列表项
    CListItemPtr GetFocusItem();

    // 由列表项获得对应的行号
    int GetIndexByItem(CListItemPtr pListItem);

    // 由列表项数据索引获得对应的行号
    int GetIndexByDataIndex(int nDataIndex);

    // 获取列表框代理
    IFrameListDelegate * GetFrameDelegate() const;

    bool IsCanReuseItem(int nItemType);

    bool IsAllSameItem(int nItemType);

    bool IsHadConflictItem(int nItemType, int nMaskType);

    // 清除列表
    void ClearAllItem();

    void SwitchPage(int nPageNo);

// IListItemDelegate
public:
    // 数据索引更改时通知处理类
    virtual void OnDataIndexChange(CListItem * pItem);
    // 选中状态更改时通知处理类
    virtual void OnItemFocusChange(CListItem * pItem, bool bPreFocus);
    // 获取内容的位置信息
    virtual void GetContentPos(int & nLeftMargin, int & nRightMargin);

protected:
    // 重置数据
    void ResetListData();
    // 取消行焦点
    void DefocusItem();
    // 焦点设置到最后一行
    void FocusLastItem();
    // 处理最后一项的下一个项交互
    bool OnLastItemNext();
    // 处理第一项的上一个项交互
    bool OnFirstItemPrevious();
    // 向前翻页
    bool OnPrePage();
#if IF_FEATURE_FAVORITE_DIR
public:
#endif
    // 向后翻页
    bool OnNextPage();
#if IF_FEATURE_FAVORITE_DIR
protected:
#endif
    // 获取当前页码(从0开始)
    int GetCurPage();
    // 按项向前滚动一项（只要有按项滚动都在这函数处理，如：T46中单页内按项滚动，翻页时按页滚动）
    bool OnPreItem(bool bScrollByPage);
    // 按项向后滚动一项（只要有按项滚动都在这函数处理，如：T46中单页内按项滚动，翻页时按页滚动）
    bool OnNextItem(bool bScrollByPage);

    // 调整子控件布局
    virtual void AdjustChildWidget();
    // 刷新提示
    virtual void RefreshNoteLabel();
    // 总数据更新
    virtual void OnDataCountChange();
    // 响应清空数据
    virtual void OnClearContent();

    void AdjustChildItem(int nLeft, int nTop, int nWidth);

    VEC_LIST_ITEM & GetListItem();

protected:
    // 提示Label
    QLabel * m_pLabNote;

    // 列表项左边距
    int m_nLeftMargin;
    // 列表项上边距
    int m_nTopMargin;
    // 列表项右边距
    int m_nRightMargin;
    // 列表项间隔
    int m_nItemSpace;
    // 列表项内容左边距位置
    int m_nContentLeft;
    // 列表项内容右边距位置
    int m_nContentRight;
    // 默认焦点
    bool m_bDefaultFocus;
    // 是否Note固定位置显示
    bool m_bNoteFixPos;

    // 数据代理
    IFrameListDelegate * m_pDelegate;

private:
    // 焦点所在的行号
    int m_nFocusIndex;
    // 每页可容纳的行数
    int m_nPageItemCount;
    // 总的数据
    int m_nDataCount;
    bool m_bAttachItem;

    // 列表项
    VEC_LIST_ITEM m_vecItem;
};

#endif // __FRAME_LIST_BASE_H__
