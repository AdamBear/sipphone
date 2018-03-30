
#ifndef __DIRECTORY_ADAPTER_H__
#define __DIRECTORY_ADAPTER_H__

#include <ETLLib.hpp>
#include <ylsmartptr.h>
#include "directorycommon.h"
#include "xwindow/xlistviewadapter.h"
#include "directorycontroler.h"
#include "diruihelper.h"
#include <baseui/include/baseuidefine.h>

class CDirectoryAdapter : public xListViewAdapter
{
public:
    explicit CDirectoryAdapter(int nState);
    virtual ~CDirectoryAdapter();

public:
    // 初始化
    virtual bool Init(int nDirType, int nGroupId = knInvalidRId);
    // 恢复初始化
    virtual void UnInit();

    virtual void OnReactive();

    virtual bool IsShowPageInfo();

    virtual bool GetTitle(yl::string & strTitle);

    virtual bool GetStateSoftKey(xListViewItem * pFocusItem, CArraySoftKeyBarData & vecSoftKey);

    // 处理普通按键
    virtual bool ProcessKeyPress(int nKey);
    // 处理数字键、#、*
    virtual bool ProcessDigitKeyPress(int nKey);
    // 处理LineKey
    virtual bool ProcessLineKeyPress(int nKey);
    // 处理SoftKey
    virtual bool ProcessAction(int nAction);

    CDirectoryControlerPtr GetControler();
    IDirUIHelperPtr GetUIHelper();

    void SetCache(bool bCache);
    bool IsInCache() const;

    // 获取代理状态
    int GetState() const;
    // 获取联系人类型
    int GetDirType();
    // 获取联系人群组Id
    int GetGroupId();

    yl::string GetPageInfo();

    void RefreshUI(int nRefreshType = DIR_UI_REFRESH_TYPE_ALL);

    void OnFocusChanged();

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
    virtual void InitListView();
    virtual void UnInitListView();

    void RefreshTitle();
    void RefreshTitlePageInfo();
    void RefreshSoftKey();

    bool ProcessCtrlAction(CDirUIHelper * pUIHelper, int nAction);
    bool CheckPageIndex(int nTotalCount, int & nDataIndex);
    bool OnShowOption();
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE || IF_HIDE_TITLE_BUT_EDIT
    void OnChangeListViewParam(bool bEnter);
#endif
protected:
    // 联系人业务逻辑控制器
    CDirectoryControlerPtr m_pControler;
    // 联系人UI表现管理器
    IDirUIHelperPtr m_pUIHelper;

    // 当前状态
    int m_nState;

    bool m_bRefreshTitle;
    bool m_bRefreshSoftKey;
    bool m_bInCache;
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE || IF_HIDE_TITLE_BUT_EDIT
    int m_nOldPageItemCount;
    int m_nOldItemHeight;
#endif
};

#ifndef CDirectoryAdapterPtr
typedef YLSmartPtr<CDirectoryAdapter> CDirectoryAdapterPtr;
#endif

#endif
