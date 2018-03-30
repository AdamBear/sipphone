
#ifndef __GROUP_EDIT_TREE_ITEM_H__
#define __GROUP_EDIT_TREE_ITEM_H__

#include "grouptreeitem.h"
#include "uikernel/btnbarwrapper.h"

enum
{
    LIST_ITEM_SUB_TYPE_GROUP_EDIT_ITEM          = 0x00000100,
};

enum
{
    LIST_ITEM_ACTION_GROUP_DEL          = LIST_ITEM_ACTION_CUSTOM_BEGIN,        // 删除群组
    LIST_ITEM_ACTION_GROUP_MODIFY       = LIST_ITEM_ACTION_CUSTOM_BEGIN + 1,    // 修改群组
    LIST_ITEM_ACTION_GROUP_SET_RING     = LIST_ITEM_ACTION_CUSTOM_BEGIN + 2,    // 设置群组铃声
};

class QLineEdit;
class CGroupEditTreeItem : public CGroupTreeItem
{
    Q_OBJECT
#ifdef IF_ENABLE_TESTABILITY
    Q_PROPERTY(QString testinfo READ GetTestInfo)
public:
    virtual QString GetTestInfo();

#endif
public:
    CGroupEditTreeItem(QWidget * parent = NULL, IListItemAction * pAction = NULL);
    virtual ~CGroupEditTreeItem(void);

    static int GetGroupEditTreeItemType();

    static CGroupEditTreeItem* GetGroupEditTreeItem(CListItem* pItem);

public:
    // 清除内容
    virtual void ClearContent();

    // 设置选中状态
    virtual void SetFocus(bool bFocus);

    // 设置是否可以编辑
    void SetEditable(bool bEditable);

    // 设置编辑中
    void SetEditing(bool bEditing);

#if IF_FEATURE_GBBOOK
    void SetShowBtnRing(bool bShow);
#endif

    // 是否可以编辑
    bool IsEditable() const;

    // 是否编辑中
    bool IsEditing() const;

    bool SaveGroup();

public:
    virtual bool eventFilter(QObject * pObject, QEvent * pEvent);

protected:
    virtual void mousePressEvent(QMouseEvent * pEvent);
    virtual void mouseReleaseEvent(QMouseEvent * pEvent);

    virtual void paintEvent(QPaintEvent * pEvent);

    // 重新布局
    virtual void Relayout();

    // 初始化群组名称编辑框
    void InitGroupLineEdit();
    // 释放群组名称编辑框
    void ReleaseGroupLineEdit();
    // 完成群组修改
    void OnFinishEdit();
    // 通知父窗口焦点变化
    void NotifyFocusChange();

protected:
    // 删除按钮
    CButtonWrapper m_btnDel;
    // 更改或确认按钮
    CButtonWrapper m_btnModify;
    // 铃声设置按钮
    CButtonWrapper m_btnRing;
    // 按钮工具条
    CBtnBarWrapper m_btnBar;
    // 群组名编辑框
    QLineEdit * m_pEdit;

    // 是否可编辑
    bool m_bEditable;
    // 是否编辑中
    bool m_bEditing;

#if IF_FEATURE_GBBOOK
    bool m_bShowBtnRing;
#endif

};
typedef CGroupEditTreeItem * CGroupEditTreeItemPtr;

#endif
