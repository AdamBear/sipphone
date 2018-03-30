#ifndef __BROWSE_ADAPTER_H__
#define __BROWSE_ADAPTER_H__

#include "directorycommon.h"
#include "directoryadapter.h"


class CBrowseAdapter : public CDirectoryAdapter
{
public:
    explicit CBrowseAdapter();
    virtual ~CBrowseAdapter();

    static CBrowseAdapter * GetBrowserAdapter(CDirectoryAdapterPtr & pAdapter);

    typedef struct
    {
        int m_nGroupId;
        int m_nType;
        int m_nTotalCount;
        int m_nPageDataIndex;
        int m_nFocusIndex;
    } ST_BROWSER_PAGE_INFO;
    typedef YLVector<ST_BROWSER_PAGE_INFO> VEC_BROWSER_PAGE_INFO;

public:
    // 初始化
    virtual bool Init(int nDirType, int nGroupId = knInvalidRId);

    virtual void OnReactive();

    virtual bool IsShowPageInfo();

    virtual bool GetTitle(yl::string & strTitle);

    virtual bool GetStateSoftKey(xListViewItem * pFocusItem, CArraySoftKeyBarData & vecSoftKey);

    // 处理普通按键
    virtual bool ProcessKeyPress(int nKey);
    // 处理数字键
    virtual bool ProcessDigitKeyPress(int nKey);
    // 处理LineKey
    virtual bool ProcessLineKeyPress(int nKey);

    // 切换群组
    bool OnChangeGroup(int nGroupId, bool bRefresh = true);

    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView * pListView);

    // 进行数据切换时进行数据索引校验，如果nDataIndex不是有效的数据，则返回false,并且将nDataIndex改为正确的值
    virtual bool PrecheckDataIndex(int & nDataIndex);

    virtual bool ProcessAction(int nAction);

    void ClearHistoryPage();

    bool IsMenuPage() const;
#if IF_DIR_UI_SWITCH_GROUP_IN_BROWSE
    bool IsGroupSwitchPage() const;
#endif
    bool IsNormalPage() const;
    bool IsPageHidden(int nPageType);
    int GetGroupId() const;

    bool EnterGroup(int nDirType, int nGroupId);
    bool EnterGroupByIndex(int nIndex);

    // 回退处理（如果处理了回退则返回true，否则返回false）
    bool OnBack();
    // 回退到Menu（如果成功回退到Menu则返回true,否则返回false）
    bool OnBackToMenu(bool bCheckCondition = false);

    void AdjustMenuIndex(int & nPageIndex, int & nFocusIndex);

protected:
    bool PopupPageType(int & nTotalCount, int & nPageDataIndex, int & nFocusIndex);
    bool PopupToMenu(int & nTotalCount, int & nPageDataIndex, int & nFocusIndex, bool bCheckCondition = false);
    bool ProcessPageBack();
    void UpdateUI(int nPageDataIndex, int nFocusIndex);
    bool ProcessRedialKey();

protected:
    int m_nPageType;
    // 记录从浏览状态切换到其他状态时的群组Id，以便回退时还原
    int m_nGroupId;
    VEC_BROWSER_PAGE_INFO m_vecPage;
};

#endif
