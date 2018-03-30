#ifndef _C_SEARCH_ADAPTER
#define _C_SEARCH_ADAPTER

#include "xwindow/xlistviewadapter.h"
#include "xwindow/xListView.h"

#include "searchwidgetitem.h"
#include "talkuilogic/supersearchreceiverbase.h"
#include "talkuilogic/talkuilogiccommon.h"
#include "talkuilogic/supersearchprocessor.h"

namespace talkui
{

class CTalkUISearchAdapter : public xListViewAdapter, public CSuperSearchReceiverBase
{
public:
    CTalkUISearchAdapter();
    virtual ~CTalkUISearchAdapter();

    virtual void Init();
    virtual void Uninit();

public:
    bool GetListHaveData();

    virtual void SetNoFocus();

    //数据变化
    void OnTextChange(const yl::string & strSearchData);

    //获取焦点item的数据, 有焦点数据返回true， 没有焦点数据返回false
    bool GetFocusItemData(yl::string & strDisplayName, yl::string & strNumber, int & iCallID);

public:
    //设置穿梭数据
    void SetTranCallData(YLList<CCallInfo *> & listTranCallInfo);

    //CSuperSearchReceiverBase
    //bResult搜索是否成功， refMessage搜索返回的数据
    virtual void DealSuperSearchResult(bool bResult, msgObject & refMessage);

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

protected:
    //是否两个联系人列表数组 相同
    bool IsContaceInfoVectorEqual(talkui_vector<CSearchItemData> & vectorTranData1,
                                  talkui_vector<CSearchItemData> & vectorTranData2);

public:
    void OnFlushList();

public:
    //焦点变化信号
    xSignal signalSearchResultChange;

private:
    //用户自定义项列表  穿梭数据
    talkui_vector<CSearchItemData> m_vectorTransferData;
    yl::string m_strSearchKey;      //查找联系人的key值

    //搜索逻辑
    CSuperSearchProcessor m_objProcessor;

    bool            m_bSearchResult;
    int         m_nDataCount;
};

}
#endif //_C_SEARCH_ADAPTER
