#ifndef _C_CONF_MANAGER_ADAPTER
#define _C_CONF_MANAGER_ADAPTER

#include "xwindow/xlistviewadapter.h"
#include "xwindow/xListView.h"

#include "talkuilogic/talkuilogiccondition.h"
#include "confmanageritem.h"


namespace talkui
{

class CConfManagerAdapter : public xListViewAdapter
{
public:
    CConfManagerAdapter();
    virtual ~CConfManagerAdapter();

    virtual void Init();
    virtual void Uninit();

public:
    //获取焦点item的ID
    int GetFocusCallID();

    //设置数据
    void SetConfManagerData(talkui_vector<CContactInfo> & listContactInfo, bool bRefreshAll = true);

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

public:
    void OnFlushList();

private:
    //保存数据
    talkui_vector<CConfManangerData> m_vectorConfManangerData;
};

}
#endif //_C_CONF_MANAGER_ADAPTER
