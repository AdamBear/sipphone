#ifndef _DLG_DIALING_UI_H
#define _DLG_DIALING_UI_H

#include "xwindow-kernel/include/xsignal.h"
#include "xwindow/xWindow.h"
#include "imeedit/ximeedit.h"
#include "dlgbasetalkui.h"
#include "talkuisearchadapter.h"
///////////talkuilogic//////////
#include "talkuilogic/uilogicdialuibase.h"

namespace talkui
{
class CDlgDialingUI : public CDlgBaseTalkUI, public CUILogicDialUIBase, public xSlotHandler, public xTimerHandler
{
public:
    explicit CDlgDialingUI();
    virtual ~CDlgDialingUI();

////////////////////////////////继承自  dlgbasetalkui /////////////////////
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

    //是否显示音量条
    virtual bool IsShowVolumeBar(int & iVolumeType, int & iMin, int & iMax, yl::string & strBGPath);

    virtual void OnFocusChanged(bool bFocused);

    virtual void OnReturnToIdleAction();

public:
    // 功能键处理
    virtual bool OnFunckeyPress(int iKey);

    // 数字键处理
    virtual bool OnDigitkeyPress(int iKey);

    //按键弹起处理
    virtual bool OnKeyRelease(int iKey);

    //内部控件焦点改变
    virtual bool onFocusMove(xView * pNewFocus, xView * pOldFocus);

public:
    //获取 processor
    virtual CBaseTalkUIProcessor * GetTalkUIProcessor();

public:
    //拨号界面特殊需求，控制softkey的显示
    //获取Softkey数据
    virtual void GetSoftkey(CArraySoftKeyBarData & objSoftkeyData);

public:
    //是否显示DTMF条
    virtual bool IsShowDtmfLine() const;
    // 是否显示切换话路的滚动条
    virtual bool IsShowSessionScrollBar();
    // 能否切换Session
    virtual bool IsChangeSession(int iKey = PHONE_KEY_UP);

public:
#if IF_TALKUI_BG_PICTURE
    ////////////////背景///////////////////////////
    //设置整个大背景的图片（不是界面自身的背景）
    virtual xBrush GetBackGroundBrush(bool& bResult);
    //界面自身背景
    virtual yl::string getTalkUIBackGround();
    ///////////////////////////////////////////////
#endif

    ////////////////////////////////////////////////
    virtual xView * GetFocusWidget();
    ////////////////////////////////////////////////

public:
    //刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
    void RefreshTalkUIData();

    virtual bool NeedReUpdate();
////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////


    ////////////////////////////////继承自  CUILogicDialUIBase /////////////////////
public:
    //设置UILogic 数据
    virtual void SetUILogicData(void * pData, UPDATE_REASON eReason = UPDATE_DEFAULT);
    //从界面获取数据
    virtual bool GetInputContent(yl::string & strNum, yl::string & strDisplayname);
    //从搜索框中获取选择数据
    virtual bool GetSearchTableContent(yl::string & strNum, yl::string & strDisplayname);
    //把选中的搜索结果赋值到编辑框中
    virtual void SetLineEditDataBySearchTable();
    //输入预编辑字符
    virtual void InputLineEditData();
    //是否有选中搜索结果
    virtual bool IsSearchTableSelectItem();
    //编辑框是否为空
    virtual bool IsEditLineEmpty();
    //获取穿梭数据callID
    virtual int GetShuttleCallID();
    //获取编辑框中的数据
    virtual yl::string GetLineEditData();
    //处理IME输入法
    virtual void DealIMEAction();
    //处理delete按键
    virtual void DealDeleteAction();
    //处理Pool的softkey
    virtual void DealPool();
    //进入历史记录
    virtual void DealHistory();

    //处理ST_Line 按键
    virtual void DealLineAction();

public:
    virtual void OpenLineSelectPage(vecLineSelectData & vecLineData, yl::string strTitle,
                                    bool bShowIndex, int nSelectID);

    virtual void OnLineSelectResult(LINE_SELECT_RESULT eResult, int nSelectIndex, int nSelectID);

    ////////////////////////////////继承自  CUILogicDialUIBase 结束 /////////////////////

protected:
    //编辑框内容变化
    void slotTextChanged(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

    void slotSearchResultChange(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes,
                                LPVOID pData);

#ifdef IF_FEATURE_LONG_PRESS_DIGIT
public:
    //定时函数
    void OnLongPressTimer(xThreadTimer* timer, LPVOID pExtraData);

private:
    //获取长按数字键
    yl::string GetStringByDigitKeyLongPress(char chKey);
#endif

protected:
    //编辑框控件
    xEditTextView * m_pLineEdit;
    //搜索框控件
    xListView * m_pSearchList;

    CTalkUISearchAdapter    m_objSearchAdapter;

#ifdef IF_FEATURE_LONG_PRESS_DIGIT
    xThreadTimer m_objLongPressTimer;
#endif
};



}       //namespace talkui


#endif

