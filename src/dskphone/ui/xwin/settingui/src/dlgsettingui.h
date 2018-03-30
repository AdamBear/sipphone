#ifndef __DLG_SETTINGUI_H__
#define __DLG_SETTINGUI_H__

#include "xwindow/xListView.h"
#include "settinguilogic/include/settinguicommondata.h"
#include "settinguilogic/src/settinguidatavalidinfo.h"
#include "messagebox/messageboxbase.h"
#include "basesettingdialog.h"

class CSettingUIDelegateBase;
class CSettingUIAdapter;

enum
{
    SETTTING_DIALOG_TYPE_BASE       = 0x01000000,
};

class CDlgSettingUI : public xLinearLayout, public CSettingDialogBase
{
public:
    static CBaseDialog * CreateInstance();
    static CDlgSettingUI * GetDlgBaseSettingUI(CSettingDialogBase * pDlg);

public:
    explicit CDlgSettingUI(EWindowType eWndType = eWT_PageMenu);
    virtual ~CDlgSettingUI();

public:
    // 加载界面数据，若bReLoadData为true，从配置中读取；否则，从内存中读取
    virtual bool LoadPageData(bool bReLoadData = false);
    // 绑定Delegate
    virtual void BindDelegate(CSettingUIDelegateBase * pDelegate);
    // 解绑Delegate
    virtual void UnBindDelegate();
    // 刷新界面，bReLoadData为true时，重新从配置中读取数据刷新；否则，读取缓存数据刷新
    virtual void RefreshUI(bool bReLoadData = false);
    // 刷新Softkey
    virtual void RefreshSoftkey();
    // 缓存当前界面数据
    virtual void CachePageData();
    // 保存时数据校验，非法数据处理函数
    virtual void DealWithDataValid(CSettingUIDataValidInfo & dataValid);
    // 弹出框回调处理
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);
    // 根据ItemID设置界面焦点项
    virtual void UpdateFocus(const yl::string & strItemId, bool bFocus);
    // 当前状态是否可成为活动窗口
    virtual bool CanBeActivityDialog();
    // 当前状态是否可成为活动窗口
    virtual bool IsShowVolumeBar(int& iVolumeType, int& iMin, int& iMax, yl::string& strPix);

public:
    // 创建界面后初始化
    virtual bool DoInitAfterCreateDialog(const yl::string & strPageID);
    // 删除界面前资源释放
    virtual bool DoUninitBeforeDeleteDialog();

/////////////////////////////////////////// CBaseDialog ///////////////////////////////////////////
public:
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();
    // 刷新界面
    virtual void UpdateWnd(bool bUIReturn = false);

protected:
    bool OnProcessAction(int iType, bool bPress, int nFocusDataIndex);

/////////////////////////////////////////// xLinearLayout ///////////////////////////////////////////
public:
    virtual void loadChildrenElements(xml_node & node);
    virtual void loadAttributes(xml_node& node);
    virtual bool ProcessKeyEvent(int nKeyCode, bool bPress);
    virtual bool onKeyPressedEvent(int key);
    virtual bool OnSoftkeyAction(int iType, bool bLongPress);
    virtual bool onPaintContent(xPainter & p);
    virtual bool onFocusEvent(bool bFocusIn);

#if IS_COLOR
    virtual xBrush GetBackGroundBrush();
#endif

/////////////////////////////////////////// 界面属性 ///////////////////////////////////////////
    // 设置、获取界面是否有标题
    bool HasTitle();
    void SetHasTitle(bool bHasTitle);

    // 获取界面Title控件
    xView * GetTitleView() const;

    // 获取界面列表管理控件
    xListView * GetListView() const;
    CSettingUIAdapter * GetListViewAdapter() const;

    bool InitListViewAdapter(const yl::string & strPageId);
    bool UninitListViewAdapter();

    // 加入大界面后，是否设置为活动窗口
    virtual bool IsSetActiveWhileAddToBigWnd();

/////////////////////////////////////////// 其他函数 ///////////////////////////////////////////

public:
    virtual bool onActiveEvent(bool bActive);

protected:
    xListView           *           m_pListView;                    // 显示列表管理
    CSettingUIAdapter       *       m_pSetAdapter;
    int
    m_nNeedReloadItemIndex;         // 重新显示时，需要重新从配置中加载数据的Item项序号
    int                             m_nValidIndex;
    int                             m_nDefPageItemCount;         // 加载时，列表每页Item数
};


#endif
