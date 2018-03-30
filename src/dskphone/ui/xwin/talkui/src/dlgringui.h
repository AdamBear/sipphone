#ifndef _DLG_RING_UI_H
#define _DLG_RING_UI_H

#include "dlgbasetalkui.h"
///////////talkuilogic//////////
#include "talkuilogic/uilogicringuibase.h"

#include "xwindow/xControls.h"
#include "talkwidget.h"

namespace talkui
{

class CDlgRingUI : public CDlgBaseTalkUI, public CUILogicRingUIBase
{
public:
    explicit CDlgRingUI();
    virtual ~CDlgRingUI();

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

    //是否显示音量条
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, yl::string & strBGPath);

    virtual void OnTalkVolumnValueChange(int iValue);

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

    void showRingingHint(bool bShow);

public:
    //刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
    virtual void RefreshTalkUIData();

    //停止界面的动画、文字滚动
    virtual void StopUIAnimation();
    ////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////

    ////////////////////////////////继承自  CUILogicRingUIBase /////////////////////
    //设置UILogic 数据
    virtual void SetUILogicData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);

    ////////////////////////////////继承自  CUILogicRingUIBase 结束 /////////////////////

protected:
    virtual void SetContactInfo(CRingUIData * pRingUIData);
    virtual void SetRingAnimation();

    void SetMuteInfo();

    void ResetWidget();

    //界面控件
private:
    xView*  m_pRingingInfoFrame;
    xImageView* m_pRingingArrow;
    xTextView* m_pRingingTarget;
#if IF_TALKUI_RINGUI_TARGET_SPLIT
    xTextView* m_pRingingTargetNumber;
#endif
    xView*  m_pRingingArrowRight;

    CTalkWidget m_talkWidget;

    xAnimationBox* m_pRingingContactAnimation;
    xAnimationBox* m_pRingingAnimation;

    xImageView* m_pMuteIcon;
};



}       //namespace talkui


#endif
