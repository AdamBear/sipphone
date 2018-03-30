#ifndef _C_SCA_MENU_ADAPTER
#define _C_SCA_MENU_ADAPTER

#include "xwindow/xlistviewadapter.h"
#include "xwindow/xListView.h"

#include "talknormalitem.h"
#include "talkuilogic/talkuilogiccommon.h"
#include "talkuilogic/scamenuprocessor.h"

namespace talkui
{

class CSCAMenuAdapter : public xListViewAdapter
{
public:
    CSCAMenuAdapter();
    virtual ~CSCAMenuAdapter();

    virtual void Init();
    virtual void Uninit();

public:
    //设置数据
    void SetSCAMenuInfo(const CSCAMenuUIData * pSCAMenuUIData);

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
    talkui_vector<yl::string> m_vectorSCAMenuData;
};

}
#endif //_C_SCA_MENU_ADAPTER
