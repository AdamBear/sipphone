#ifndef _C_PICKUP_ADAPTER
#define _C_PICKUP_ADAPTER

#include "xwindow/xlistviewadapter.h"
#include "xwindow/xListView.h"

#include "talknormalitem.h"
#include "talkuilogic/talkuilogiccommon.h"
#include "talkuilogic/pickupmenuprocessor.h"

namespace talkui
{

class CPickUpAdapter : public xListViewAdapter
{
public:
    CPickUpAdapter();
    virtual ~CPickUpAdapter();

    virtual void Init();
    virtual void Uninit();

public:
    //设置数据
    void SetPickUpInfo(const CPickUPMenuData * pPickUPMenuData);

public:
    virtual void InitListViewLayout(xListView * pListView);

public:
    // 根据位置获取列表项
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);
    // 刷新列表项数据
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);
    // 数据清空
    virtual void OnCleanContent(xListView * pListView);

    // IListViewItemAction
public:
    virtual void OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction);

private:
    //保存数据
    talkui_vector<yl::string> m_vectorPickUpData;
};

}
#endif //_C_PICKUP_ADAPTER
