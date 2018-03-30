#ifndef _C_LINE_SELECT_ADAPTER
#define _C_LINE_SELECT_ADAPTER

#include "xwindow/xlistviewadapter.h"
#include "xwindow/xListView.h"

#include "talknormalitem.h"
#include "talkuilogic/talkuilogiccommon.h"

namespace talkui
{

class CLineSelectAdapter : public xListViewAdapter
{
public:
    CLineSelectAdapter();
    virtual ~CLineSelectAdapter();

    virtual void Init();
    virtual void Uninit();

public:
    //设置数据
    void SetLineInfo(const talkui_vector<yl::string> & vectorLineInfo);

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
    talkui_vector<yl::string> m_vectorLineInfo;
};

}
#endif //_C_LINE_SELECT_ADAPTER
