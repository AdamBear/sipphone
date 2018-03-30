#ifndef _C_RTP_STATUS_ADAPTER
#define _C_RTP_STATUS_ADAPTER

#include "xwindow/xlistviewadapter.h"
#include "xwindow/xListView.h"

#include "talknormalitem.h"
#include "talkuilogic/talkuilogiccommon.h"

#include "talkuilogic/rtpstatusprocessor.h"

namespace talkui
{

class CRTPStatusAdapter : public xListViewAdapter
{
public:
    CRTPStatusAdapter();
    virtual ~CRTPStatusAdapter();

    virtual void Init();
    virtual void Uninit();

public:
    //设置数据
    void SetRTPStatusData(const talkui_vector<CRTPStatusData> & vectorRTPStatusData);

    void SetScrollBar(xScrollBar * pScrollBar)
    {
        m_pScrollBar = pScrollBar;
    }
    void SetCurrentPageIndex(int nIndex)
    {
        m_nPageIndex = nIndex;
    }

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
    talkui_vector<CRTPStatusData> m_vectorRTPStatusData;
    xScrollBar   *  m_pScrollBar;
    int                 m_nPageIndex;
};

}
#endif //_C_RTP_STATUS_ADAPTER
