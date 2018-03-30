#ifndef _DLG_RTPSTATUS_UI_H_
#define _DLG_RTPSTATUS_UI_H_

#include "dlgbasetalkui.h"
#include "rtpstatusadapter.h"

///////////talkuilogic//////////
#include "talkuilogic/uilogicrtpstatusuibase.h"

#include "xwindow/xlistviewadapter.h"
#include "xwindow/xListView.h"

namespace talkui
{

class CDlgRTPStatusUI : public CDlgBaseTalkUI, public CUILogicRTPStatusUIBase
{
public:
    explicit CDlgRTPStatusUI();
    virtual ~CDlgRTPStatusUI();

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


    ////////////////////////////////继承自  CUILogicRTPStatusUIBase /////////////////////
public:
    //设置UILogic 数据
    virtual void SetUILogicData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);

public:
    //更新RTPStatus数据
    virtual void UpdateUIRTPStatusData(const talkui_vector<CRTPStatusData> & vectorRTPStatusData);

    ////////////////////////////////继承自  CUILogicRTPStatusUIBase 结束 /////////////////////

protected:
    void ResetWidget();

    //界面控件
private:
    xListView* m_pRTPStatusList;
    xScrollBar* m_pScrollBar;

    CRTPStatusAdapter   m_objRTPStatusAdapter;

};


}       //namespace talkui



#endif
