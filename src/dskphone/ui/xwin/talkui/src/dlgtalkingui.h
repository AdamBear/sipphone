#ifndef _DLG_TALKING_UI_H
#define _DLG_TALKING_UI_H

#include "dlgbasetalkui.h"
///////////talkuilogic//////////
#include "talkuilogic/uilogictalkinguibase.h"

#include "talkwidget.h"

namespace talkui
{

class CDlgTalkingUI : public CDlgBaseTalkUI, public CUILogicTalkingUIBase
{
public:
    explicit CDlgTalkingUI();
    virtual ~CDlgTalkingUI();

public:
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
    void showTransFailHint(bool bShow);
    void showStatusHint(bool bShow);
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

    ////////////////////////////////继承自  CUILogicTalkingUIBase /////////////////////
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

    ////////////////////////////////继承自  CUILogicTalkingUIBase 结束 /////////////////////

protected:
    //设置各类提示信息
    void SetContactInfo(CTalkingUIData * pTalkingUIData);
    void SetStatusInfo(CTalkingUIData * pTalkingUIData);
    void SetIncomingInfo(CTalkingUIData * pTalkingUIData);

#if IF_HD_ICON_DEPART
    void SetHDIconInfo(CTalkingUIData * pTalkingUIData);
#endif

    void ResetWidget();

    //界面控件
private:
    CTalkWidget m_talkWidget;

#if IF_TRANSFER_FAIL_HINT_STYLE
    xTextView*  m_pTransFailHint;
#endif
};

}       //namespace talkui

#endif
