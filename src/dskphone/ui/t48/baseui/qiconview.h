#ifndef __QICONVIEW_H__
#define __QICONVIEW_H__

#include "uikernel/qswitchpagelistview.h"
#include "setting/include/common.h"
#include <QtGui>

//前向声明
class MenuItemData;
class MenuItemListData;

class CIconViewItem : public CSwitchPageListViewItem
{
    Q_OBJECT
public:
    //默认构造函数
    CIconViewItem();

    //带参数构造函数
    CIconViewItem(MenuItemData * pItemData);

    //析构函数
    ~CIconViewItem();
    //private:
    //virtual QIcon get3StatesIcon(const QString& strFileName, const QString& strSelectedFileName);
};

class CIconView : public CSwitchPageListView
{
    Q_OBJECT
public:
    //构造函数
    explicit CIconView(QWidget * pParent = 0);
    //析构函数
    ~CIconView();

public:
    //加载IconView的菜单项
    bool LoadMenuItemData(MenuItemListData * pMenuItemListData);

    // 取消Item的选择效果
    void CancelItemSelect();
    //菜单项点击时的处理
    virtual void OnItemEntered();
    void SetDefaultSelect(bool bSelelct);

    QSize GetItemSize() const
    {
        return m_itemSize;
    }

protected:
    virtual void OnItemClicked(QListWidgetItem * pItem);

public:
    //Item的默认点击有效区域
    //QSize m_sizeValidClick;
    //当前页面最大的菜单数
    int m_nMenuCount;
signals:
    void ItemEntered();
private:
    bool m_bDefaultSelect;//是否默认选中第一项
};

#endif // __QICONVIEW_H__
