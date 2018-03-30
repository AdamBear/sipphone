#ifndef _C_NETCONF_LIST_ADAPTER
#define _C_NETCONF_LIST_ADAPTER

#include "xwindow/xlistviewadapter.h"
#include "xwindow/xListView.h"

#include "talknormalitem.h"
#include "talkuilogic/talkuilogiccondition.h"

namespace talkui
{

class CNetConfListAdapter : public xListViewAdapter
{
public:
    CNetConfListAdapter();
    virtual ~CNetConfListAdapter();

    virtual void Init();
    virtual void Uninit();

public:
    //获取焦点item的ID
    int GetFocusCallID();

    //设置数据
    void SetNetConfListData(talkui_vector<yl::string> & listContactInfo);

    void SetTextAlign(int nAlign);

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
    talkui_vector<yl::string> m_vectorNetConfList;
    //item文字对齐方式
    int m_nAlign;
};

}
#endif //_C_NETCONF_LIST_ADAPTER
