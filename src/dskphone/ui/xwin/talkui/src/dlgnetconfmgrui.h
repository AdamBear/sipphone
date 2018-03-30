#ifndef _DLG_NETCONFMGR_UI_H
#define _DLG_NETCONFMGR_UI_H

#include "dlgbasetalkui.h"
///////////talkuilogic//////////
#include "talkuilogic/uilogicnetconfmgruibase.h"

#include "confmanageradapter.h"

#include "xwindow/xlistviewadapter.h"
#include "xwindow/xListView.h"

namespace talkui
{
class CDlgNetConfMgrUI : public CDlgBaseTalkUI, public CUILogicNetConfMgrUIBase
{
public:
    explicit CDlgNetConfMgrUI();
    virtual ~CDlgNetConfMgrUI();

public:
    virtual void loadChildrenElements(xml_node & node);

private:
    virtual void LoadLayout();

public:
    // 初始化
    virtual void Init();
    // 释放数据，将窗口清空回复到初始化状态
    virtual void Uninit();

    virtual bool GetIsFullScreen();
#if IF_TALKUI_BG_PICTURE
    virtual yl::string getTalkUIBackGround();
#endif

    // 刷新窗口
    virtual void UpdateWnd(bool bUIReturn = false);

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

public:
    //刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
    virtual void RefreshTalkUIData();

    //更新通话图标
    virtual void UpdateTalkUIIcons();
    ////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////

    ////////////////////////////////继承自  CUILogicNetConfMgrUIBase /////////////////////
    //设置UILogic 数据
public:
    virtual void SetUILogicData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);

public:
    //获取焦点话路callid
    virtual int GetFocusCallID();

    virtual void ShowRemoveMemberBox(yl::string strName);
    ////////////////////////////////继承自  CUILogicNetConfMgrUIBase 结束 /////////////////////

public:
    virtual void MessageBoxCallBack(CMessageBoxBase * pMessageBox);

protected:
    void ResetWidget();

    //界面控件
private:
    xListView* m_pConfManagerList;

    CConfManagerAdapter m_objConfManagerAdapter;

};

}       //namespace talkui

#endif
