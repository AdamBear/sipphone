#ifndef _DLG_CONF_UI_H_
#define _DLG_CONF_UI_H_

#include "dlgbasetalkui.h"
///////////talkuilogic//////////
#include "talkuilogic/uilogicconfuibase.h"

#include "talkwidget.h"

namespace talkui
{

class CDlgConfUI : public CDlgBaseTalkUI, public CUILogicConfUIBase
{
public:
    explicit CDlgConfUI();
    virtual ~CDlgConfUI();

public:
    virtual void loadChildrenElements(xml_node & node);

private:
    virtual void LoadLayout();

    ////////////////////////////////继承自  dlgbasetalkui /////////////////////
public:
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    // 刷新窗口
    virtual void UpdateWnd(bool bUIReturn = false);

    void showIncomingHint(bool bShow);
    void showStatusHint(bool bShow);
    void showTimer(bool bShow);

public:
    //获取 processor
    virtual CBaseTalkUIProcessor * GetTalkUIProcessor();

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

    ////////////////////////////////继承自  CUILogicConfUIBase /////////////////////
    //设置UILogic 数据
    virtual void SetUILogicData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);

    virtual int GetFocusCallID();

    virtual void ShowSecurityBox(int nAccount);

    virtual void ProcessSecurityCallBack(const yl::string & strResult);

    virtual void ShowACDDispCodeMenu(int nCallID);

    virtual void ShowEmergencyMenu(int nCallID);

    virtual void ShowDealLastNote();

    virtual bool ShowErrorNote(CALL_RECORD_ACTION_RESULT eFailResult);
    ////////////////////////////////继承自  CUILogicConfUIBase 结束 /////////////////////

protected:
    //设置各类提示信息
    void SetContactInfo(CConfUIData * pConfUIData);
    void SetStatusInfo(CConfUIData * pConfUIData);
    void SetIncomingInfo(CConfUIData * pConfUIData);

#if IF_HD_ICON_DEPART
    void SetHDIconInfo(CConfUIData * pConfUIData);
#endif

    void ResetWidget();

    //界面控件
private:
    CTalkWidget m_talkWidget;
};

}       //namespace talkui

#endif
