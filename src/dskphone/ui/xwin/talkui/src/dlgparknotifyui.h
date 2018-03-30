#ifndef _DLG_PARKNOTIFY_UI_H_
#define _DLG_PARKNOTIFY_UI_H_

#include "dlgbasetalkui.h"
///////////talkuilogic//////////
#include "talkuilogic/uilogicparknotifyuibase.h"

#include "talkwidget.h"

namespace talkui
{

class CDlgParkNotifyUI : public CDlgBaseTalkUI, public CUILogicParkNotifyUIBase
{
public:
    explicit CDlgParkNotifyUI();
    virtual ~CDlgParkNotifyUI();

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

public:
    //获取 processor
    virtual CBaseTalkUIProcessor * GetTalkUIProcessor();
    void showStatusHint(bool bShow);

public:
    //是否显示DTMF条
    virtual bool IsShowDtmfLine() const;
    // 是否显示切换话路的滚动条
    virtual bool IsShowSessionScrollBar();
    // 能否切换Session
    virtual bool IsChangeSession(int iKey = PHONE_KEY_UP);

public:
    //刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
    virtual void RefreshTalkUIData();
    ////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////

    ////////////////////////////////继承自  CUILogicParkNotifyUIBase /////////////////////
    //设置UILogic 数据
    virtual void SetUILogicData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);
    ////////////////////////////////继承自  CUILogicParkNotifyUIBase 结束 /////////////////////

protected:
    //设置各类提示信息
    void SetContactInfo(CParkNotifyUIData * pParkNotifyUIData);
    void SetStatusInfo(CParkNotifyUIData * pParkNotifyUIData);

    void ResetWidget();

    //界面控件
private:
    CTalkWidget m_talkWidget;

};

}       //namespace talkui


#endif
