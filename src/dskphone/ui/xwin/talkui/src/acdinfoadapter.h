#ifndef _C_ACD_INFO_ADAPTER
#define _C_ACD_INFO_ADAPTER

#include "xwindow/xlistviewadapter.h"
#include "xwindow/xListView.h"

#include "talknormalitem.h"
#include "talkuilogic/talkuilogiccommon.h"

namespace talkui
{

class CACDInfoAdapter : public xListViewAdapter
{
public:
    CACDInfoAdapter();
    virtual ~CACDInfoAdapter();

    virtual void Init();
    virtual void Uninit();

public:
    //设置数据
    void SetACDInfo(const yl::string & strWaitTime, const yl::string & strCCName,
                    const yl::string & strCCUID, const yl::string & strQueue, const yl::string & strCallsLongest);

public:
    virtual void InitListViewLayout(xListView * pListView);

    void SetScrollBar(xScrollBar * pScrollBar)
    {
        m_pScrollBar = pScrollBar;
    }

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
    talkui_vector<yl::string> m_vectorACDInfoData;
    xScrollBar   *  m_pScrollBar;

};

}
#endif //_C_ACD_INFO_ADAPTER
