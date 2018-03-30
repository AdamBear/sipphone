#ifndef _DLG_PUBLIC_WIDGET
#define _DLG_PUBLIC_WIDGET

#include "talkuilogic/talkuilogiccondition.h"
#include "talkuilogic/talkuilogiccommon.h"
#include "dlgbasetalkui.h"

#include "talkuidtmf.h"

namespace talkui
{

class CPublicWidget : public xSlotHandler
{
public:
    explicit CPublicWidget();
    virtual ~CPublicWidget();

    void Init();
    void Uninit();

    //加载控件
    void LoadWidget(xView * pLayout);
    //重置控件，例如话路切换的时候调用
    void ResetWidget();
    //重置可用、显示状态
    void RefreshWidget();

    void SetFrameFocus();

public:
    void addTalkUIChild(CDlgBaseTalkUI* pBaseTalkUI);
    void popTalkUIChild(CDlgBaseTalkUI* pBaseTalkUI);
#if IF_TALKUI_BG_PICTURE
    void setTalkUIBackGround(CDlgBaseTalkUI* pBaseTalkUI);
    void setTalkUIGeometry(int nLeft, int nTop, int nWidth, int nHeight);
#endif

    void ShowScrollBar(bool bShow);
    void SetScrollBar(int nPageRange, int nRange, int nPos);

    void ShowDtmfEdit(bool bShow);
    void ResetDtmf();

    void ShowNetworkHint(bool bShow);
    void SetNetworkHint(TALK_NETWORK_STATUS eStatus);

    void ShowCCStatusHint(bool bShow);
    void RefreshCCStatus();

public:
    //硬按键事件
    void OnKeyEvent(CDlgBaseTalkUI* pBaseTalkUI, int iKey, bool bPress = true);

    //编辑框内容变化
    void slotDtmfVisibleChange(xSignal* sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData);

private:
    bool m_bNetWorkAvailable;

private:
    xLinearLayout * m_pTalkUIFrame;

    xFrameLayout * m_pTalkUIBackground;

    xFrameLayout * m_pTalkUIFrameNormal;

    xFrameLayout * m_pTalkUIFrameFullScreen;

    xScrollBar * m_pScrollBar;          //滚动条

    CTalkUIDtmf m_objDtmfLine;

    xTextView* m_pNetWorkHint;

    xTextView* m_pCCStatusHint;
};

}       //namespace talkui

#endif
