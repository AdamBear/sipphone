#ifndef _DLG_NETCONF_UI_H
#define _DLG_NETCONF_UI_H

#include "dlgbasetalkui.h"
///////////talkuilogic//////////
#include "talkuilogic/uilogicnetconfuibase.h"

#include "talkwidget.h"
#include "netconflistadapter.h"

namespace talkui
{

class CDlgNetConfUI : public CDlgBaseTalkUI, public CUILogicNetConfUIBase
{
public:
    explicit CDlgNetConfUI();
    virtual ~CDlgNetConfUI();

public:
    virtual void loadAttributes(xml_node& node);
    virtual void loadChildrenElements(xml_node & node);

private:
    virtual void LoadLayout();

public:
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    // 刷新窗口
    virtual void UpdateWnd(bool bUIReturn = false);

public:
    //获取 processor
    virtual CBaseTalkUIProcessor * GetTalkUIProcessor();

    void showIncomingHint(bool bShow);
#if IF_NETCONF_UI_STYLE
    void showStatusHint(bool bShow);
#else
    void showMemberHint(bool bShow);
#endif
    void showTimer(bool bShow);

public:
    //是否显示DTMF条
    virtual bool IsShowDtmfLine() const;
    // 是否显示切换话路的滚动条
    virtual bool IsShowSessionScrollBar();
    // 能否切换Session
    virtual bool IsChangeSession(int iKey = PHONE_KEY_UP);

    // 按键处理
    virtual bool OnKeyPreProcess(int iKey);

    virtual void OnTalkVolumnVisibleChange(bool bVisible);

public:
    //刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
    virtual void RefreshTalkUIData();

    //刷新通话时间显示
    virtual void UpdateTalkUITime(uint iHour, uint iMin, uint iSecond);
    ////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////

    ////////////////////////////////继承自  CUILogicNetConfUIBase /////////////////////
    //设置UILogic 数据
    virtual void SetUILogicData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);

    virtual void ShowSecurityBox(int nAccount);

    virtual void ProcessSecurityCallBack(const yl::string & strResult);

    virtual void ShowACDDispCodeMenu(int nCallID);

    virtual void ShowEmergencyMenu(int nCallID);

    //处理Pool的softkey
    virtual void DealPool();

    virtual void ShowDealLastNote();

    virtual bool ShowErrorNote(CCallInfo * pCallInfo);

    ////////////////////////////////继承自  CUILogicNetConfUIBase 结束 /////////////////////

protected:
#if IF_NETCONF_UI_STYLE
    //设置各类提示信息
    void SetConfInfo(CNetConfUIData * pNetConfUIData);
#else
    void SetConfList(CNetConfUIData * pNetConfUIData);
    void SetMemberHint(CNetConfUIData * pNetConfUIData);
#endif

    void SetStatusInfo(CNetConfUIData * pNetConfUIData);

    void SetIncomingInfo(CNetConfUIData * pNetConfUIData);

#if IF_HD_ICON_DEPART
    void SetHDIconInfo(CNetConfUIData * pNetConfUIData);
#endif

    void ResetWidget();

protected:
    yl::string GetMemberHint(CNetConfUIData * pNetConfUIData);

    //界面控件
private:
    CTalkWidget m_talkWidget;

#if !IF_NETCONF_UI_STYLE
    xTextView* m_pMemberHint;

    xListView* m_pNetConfList;
    CNetConfListAdapter m_objNetConfListAdapter;

    int m_nItemAlign;
    int m_nListPageCount;

    xView* m_pMemberHintFrame;
#endif
};

}       //namespace talkui

#endif
