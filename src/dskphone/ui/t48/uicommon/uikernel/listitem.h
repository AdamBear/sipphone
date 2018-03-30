#ifndef LISTITEM_H
#define LISTITEM_H

#include <QWidget>
#include <ylvector.h>
#include "ilistitemaction.h"

#define INVALID_ITEM_INDEX -1

class QMouseEvent;
class QMoveEvent;
class QResizeEvent;
class QStylePainter;
class IListItemDelegate;

enum
{
    // 列表项类型(每个继承类使用一个字节来区分)
    LIST_ITEM_TYPE_BASE     = 0x00000000,
    LIST_ITEM_TYPE_MARK     = 0xFF000000,
};

class CListItem : public QWidget
{
    Q_OBJECT

public:
    explicit CListItem(int nType, QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CListItem();

    // 释放
    virtual void Release();

    // 设置通知更改处理对象
    virtual void SetDelegate(IListItemDelegate * pDelegate);

    // 设置Action处理对象
    void SetAction(IListItemAction * pAction);

    // 清除内容
    virtual void ClearContent();

    // 选中或取消选中此选项(会有选中事件通知)
    void SelectItem(bool bSelect);

    // 设置焦点(会有选中事件通知)
    void FocusItem(bool bFocus, int nAction = LIST_ITEM_ACTION_FOCUS);

    // 设置焦点状态
    virtual void SetFocus(bool bFocus);

    // 获取类型
    int GetType() const;
    int GetListType() const;

    // 获取数据索引
    int GetDataIndex() const;

    // 设置索引
    void SetDataIndex(int nIndex);

    // 是否被选中或者有焦点
    bool IsFocus() const;

    // 是否被按下
    bool IsDown() const;

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    bool ProcessMousePress(QMouseEvent * pEvent);
    bool ProcessMouseRelease(QMouseEvent * pEvent, bool bReSelect = false);

    // 响应移动事件
    virtual void moveEvent(QMoveEvent * pEvent);
    // 响应改变大小事件
    virtual void resizeEvent(QResizeEvent * pEvent);

    // 重新布局
    virtual void Relayout();

    void RefreshUI();

    // 通知处理Action
    void DoAction(int nAction);
    // 画按下效果图
    void PaintDownBg(QStylePainter & stylePainter, const char * pszDownBg = NULL,
                     const QRect & rcBg = QRect(0, 0, 0, 0));
    // 画焦点效果图
    void PaintFocusBg(QStylePainter & stylePainter, const char * pszFocusBg = NULL,
                      const QRect & rcBg = QRect(0, 0, 0, 0));

protected:
    // 列表项代理接口
    IListItemDelegate * m_pDelegate;
    // 列表项用户操作接口
    IListItemAction * m_pAction;

    // 列表项类型
    int m_nType;

private:
    // 列表项索引，从0开始
    int m_nDataIndex;
    // 是否有焦点
    bool m_bFocus;
    // 是否被按下
    bool m_bDown;
};

typedef CListItem * CListItemPtr;
typedef YLVector<CListItem *> VEC_LIST_ITEM;

#endif // LISTITEM_H
