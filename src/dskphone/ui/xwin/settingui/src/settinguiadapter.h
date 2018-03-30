#ifndef __SETTINGUI_ADAPTER_H__
#define __SETTINGUI_ADAPTER_H__

#include "xwindow/xlistviewadapter.h"
#include "uimanager/dialogviewcontroller.h"
#include <settinguilogic/include/settingdefine.h>
#include "ETLLib/ETLMsgSystem.h"
#include "xwindow-kernel/include/xsignal.h"
#include <baseui/include/baseuidefine.h>

class CSettingUIDelegateBase;
class CSettingUIItemDataBase;

class CSettingUIAdapter : public xListViewAdapter, public xSlotHandler
{
public:
    CSettingUIAdapter();
    virtual ~CSettingUIAdapter();

public:
    virtual void BindDelegate(CSettingUIDelegateBase * pDelegate);
    virtual void UnBindDelegate();

    // 刷新Softkey
    virtual void RefreshSoftkey();
    void RefreshTitle();
    // 初始化Tree结点
    virtual void Init();

public:
    // 根据位置获取列表项
    virtual xListViewItem * PrepareItem(xListView * pListView, int nDataIndex, xListViewItem * pItem);
    // 刷新列表项数据
    virtual bool RefreshItem(xListView * pListView, xListViewItem * pItem);
    // 数据清空
    virtual void OnCleanContent(xListView * pListView);

    // 初始化列表框的布局
    virtual void InitListViewLayout(xListView * pListView);
    // 刷新列表框
    virtual void RefreshListView();

    void AdjustFirstShowItemIndex();
    // 自定义Softkey
    virtual void GetPrivateSoftkey(CArraySoftKeyBarData & vecSoftkey);

    // 获取序号为nDataIndex项的Value
    virtual yl::string GetItemValueByDataIndex(int nDataIndex);

#if IF_XLISTVIEW_CUSTOM_SWITCH_FOCUS
    // 获取真正有焦点的数据索引
    virtual int GetFocusDataIndex(int nDataIndex, bool bNext);
#endif

    virtual bool OnNumberKeyPress(int nKey);

    void OnIMEChanged(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

    // IListViewItemAction
public:
    // Item事件
    virtual void OnItemAction(xListView * pListView, xListViewItem * pItem, int nAction);

protected:

    //获取配置文件配置的softkey
    void GetPageXmlSoftkey(CArraySoftKeyBarData & vecSoftKey);
    // 获取公共Softkey
    void GetCommonSoftkey(CArraySoftKeyBarData & vecSoftKey);
    // 设置Item数据
    virtual void SetItemData(xListViewItem * pItem, CSettingUIItemDataBase * pItemData, int nDataIndex);
    // 更新Item显示
    virtual void UpdataItemValue(xListViewItem * pItem);
    // 设置Item文本字体大小
    void SetItemTextFontSize(xListViewItem * pItem, int nTextFontSize);
    // 设置combobox style
    void SetComboBoxStyle(xListViewItem * pItem, const yl::string & strStyle);

#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE && IF_PAGEDATA_SUPPORT_ITEM_PROPERTY
    void OnChangeListViewParam(bool bInit);
#endif

protected:
    CSettingUIDelegateBase   *  m_pSettingUIDelegate;       //
    int                         m_iPreviousFocusIndex;      // 上次焦点项序号
#if IF_EDIT_COMBOBOX_SHOW_TWO_LINE && IF_PAGEDATA_SUPPORT_ITEM_PROPERTY
    int         m_nOldItemHeight;
#endif
};


#endif
