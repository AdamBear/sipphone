#ifndef _DLG_ACDCALLINFO_H_
#define _DLG_ACDCALLINFO_H_

#include "dlgbasetalkui.h"
#include "acdinfoadapter.h"

///////////talkuilogic//////////
#include "talkuilogic/uilogicacdcallinfouibase.h"

#include "xwindow/xlistviewadapter.h"
#include "xwindow/xListView.h"

namespace talkui
{

class CDlgACDCallInfoUI : public CDlgBaseTalkUI, public CUILogicACDCallInfoUIBase
{
public:
    explicit CDlgACDCallInfoUI();
    virtual ~CDlgACDCallInfoUI();

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

    virtual bool GetIsFullScreen();
#if IF_TALKUI_BG_PICTURE
    virtual yl::string getTalkUIBackGround();
#endif

    // 刷新窗口
    virtual void UpdateWnd(bool bUIReturn = false);

public:
    // 功能键处理
    virtual bool OnFunckeyPress(int iKey);

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
    ////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////

    ////////////////////////////////继承自  CUILogicACDCallInfoUIBase /////////////////////
    //设置UILogic 数据
    virtual void SetUILogicData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);
    ////////////////////////////////继承自  CUILogicACDCallInfoUIBase 结束 /////////////////////

protected:
    void ResetWidget();

    //界面控件
private:
    xListView* m_pACDInfoList;                      //列表
    xScrollBar* m_pScrollBar;                       //滚动条

    CACDInfoAdapter m_objACDInfoAdapter;

};

}       //namespace talkui


#endif
