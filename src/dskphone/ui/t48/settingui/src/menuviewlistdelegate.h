#ifndef _SUBMENU_LIST_DELEGATE_H_
#define _SUBMENU_LIST_DELEGATE_H_

#include  "baseui/framelistdelegate.h"
#include "setting/include/common.h"
#include <QObject>
#include "ylstl/ylvector.h"

class CTextMenuItem;

class CMenuviewDelegate : public QObject, public CFrameListDelegate
{
    Q_OBJECT

private:
    typedef YLVector<MenuItemData> ItemDataVec;
    typedef YLVector<CTextMenuItem *> TextMenuItemVec;

public:
    CMenuviewDelegate(QObject * parent);
    virtual void OnItemAction(CFrameListBase * pFrameList, CListItem * pItem, int nAction);

    virtual void RefreshFrameList();

    void ClearAllContent();//清空列表控件，页面数据保留

    bool SetContent(MenuItemListData * pListData);

    //根据dataindex选中行，如果当前没有显示出来则刷新到该页，benter表示是否执行制定行action
    void SelectByIndex(int iDataIndex, bool bEnter = true);

    void SetCurrentItem(const QString & strItemClickAction, bool bEnter = true);

    //获取当前行的dataindex
    int GetCurrentIndex();

    QString GetCurrentAction();

    virtual void InitFrameList();

    bool eventFilter(QObject * pObject, QEvent * pEvent);

    void RollackPreItem();

protected:
    virtual void OnLoadData(CFrameListBase * pFrameList, int nDataIndex, int nDataCount);
    void ClearListData();
    void GoNextPage();
    void GoPrePage();
    void init();

signals:
    void ProcessCurrentItem(const QString & strAction);

private:
    ItemDataVec m_listData;
    int m_iPreSelectIndex;//之前选中的dataindex
    int m_iCurrentIndex;
    TextMenuItemVec m_vecItem;
};

#endif  //_SUBMENU_LIST_DELEGATE_H_
