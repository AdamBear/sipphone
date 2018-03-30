#include "publicwidget.h"
#include "talkuipath.h"
#include "talkuixmlpath.h"
#include "talkuixmlitemname.h"

#include "talkuicommonfunction.h"
#include "talkuilogic/talkuilogicdelegate.h"
#include "talkuiframemanager.h"
#include "conflictmanager.h"

namespace talkui
{

void publicShowDtmf(bool bShow)
{
    CBaseDialog * pDialog = UIManager_GetPrivateDlg(DLG_TalkUIFrame);
    if (NULL != pDialog)
    {
        CDlgTalkUIFrame* pTalkUIFrame = dynamic_cast<CDlgTalkUIFrame*>(pDialog);
        if (NULL != pTalkUIFrame)
        {
            CPublicWidget& PublicWidget = pTalkUIFrame->getPublicWidget();
            PublicWidget.ShowDtmfEdit(bShow);
        }
    }
}

void publicShowCCStatus(bool bShow)
{
    CBaseDialog * pDialog = UIManager_GetPrivateDlg(DLG_TalkUIFrame);
    if (NULL != pDialog)
    {
        CDlgTalkUIFrame* pTalkUIFrame = dynamic_cast<CDlgTalkUIFrame*>(pDialog);
        if (NULL != pTalkUIFrame)
        {
            CPublicWidget& PublicWidget = pTalkUIFrame->getPublicWidget();
            PublicWidget.ShowCCStatusHint(bShow);
        }
    }
}

void publicShowNetworkHint(bool bShow)
{
    CBaseDialog * pDialog = UIManager_GetPrivateDlg(DLG_TalkUIFrame);
    if (NULL != pDialog)
    {
        CDlgTalkUIFrame* pTalkUIFrame = dynamic_cast<CDlgTalkUIFrame*>(pDialog);
        if (NULL != pTalkUIFrame)
        {
            CPublicWidget& PublicWidget = pTalkUIFrame->getPublicWidget();
            PublicWidget.ShowNetworkHint(bShow);
        }
    }
}

CPublicWidget::CPublicWidget()
    : m_bNetWorkAvailable(true)
    , m_pTalkUIFrame(NULL)
    , m_pTalkUIBackground(NULL)
    , m_pTalkUIFrameNormal(NULL)
    , m_pTalkUIFrameFullScreen(NULL)
    , m_pScrollBar(NULL)
    , m_pNetWorkHint(NULL)
    , m_pCCStatusHint(NULL)
{

}

CPublicWidget::~CPublicWidget()
{
    m_objDtmfLine.FreeWidget();

    SLOT_DISCONNECT(&m_objDtmfLine, signalVisiableChange, this, talkui::CPublicWidget::slotDtmfVisibleChange);
}

void CPublicWidget::Init()
{
    ResetWidget();

    CONFLICT_UNIT stUnit;

    stUnit.m_ShowFunction = publicShowDtmf;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_DTMF, stUnit);

    stUnit.m_ShowFunction = publicShowCCStatus;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_CCSTATUS, stUnit);

    stUnit.m_ShowFunction = publicShowNetworkHint;
    _TalkConflictManager.addConflictUnit(CONFLICT_PRIORITY_NETWORK, stUnit);
}

void CPublicWidget::Uninit()
{
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_NETWORK);
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_DTMF);
    _TalkConflictManager.removeConflictUnit(CONFLICT_PRIORITY_CCSTATUS);
}

void CPublicWidget::LoadWidget(xView * pLayout)
{
    if (NULL == pLayout)
    {
        return;
    }

    m_pTalkUIFrame = static_cast<xLinearLayout*>((pLayout->getViewById(TALKUI_UI_FRAME)));

    m_pTalkUIBackground = static_cast<xFrameLayout*>((pLayout->getViewById(TALKUI_UI_BACKGROUND)));

    m_pTalkUIFrameNormal = static_cast<xFrameLayout*>((pLayout->getViewById(TALKUI_UIFRAME_NORMAL)));

    m_pTalkUIFrameFullScreen = static_cast<xFrameLayout*>((pLayout->getViewById(TALKUI_UIFRAME_FULLSCREEN)));

    //赋值滚动条
    m_pScrollBar = static_cast<xScrollBar*>((pLayout->getViewById(TALKUI_FRAME_SCROLLBAR)));

    xEditTextView* pDtmfEdit = static_cast<xEditTextView*>((pLayout->getViewById(TALKUI_DTMF)));
    m_objDtmfLine.SetDtmfWidget(pDtmfEdit);

    SLOT_CONNECT(&m_objDtmfLine, signalVisiableChange, this, talkui::CPublicWidget::slotDtmfVisibleChange);

    m_pNetWorkHint = static_cast<xTextView*>((pLayout->getViewById(TALKUI_NETWORK_HINT)));
    m_pCCStatusHint = static_cast<xTextView*>((pLayout->getViewById(TALKUI_CCSTATUS_HINT)));
}

void CPublicWidget::ResetWidget()
{
    m_objDtmfLine.ClearData();

    if (NULL != m_pCCStatusHint)
    {
        m_pCCStatusHint->SetText("");
    }
}

void CPublicWidget::RefreshWidget()
{
    _TalkConflictManager.setConflictUnitEnable(CONFLICT_PRIORITY_DTMF, false);
    _TalkConflictManager.setConflictUnitEnable(CONFLICT_PRIORITY_NETWORK, false);
    _TalkConflictManager.setConflictUnitEnable(CONFLICT_PRIORITY_CCSTATUS, false);

    _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_DTMF, false);
    _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_NETWORK, false);
    _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_CCSTATUS, false);

    RefreshCCStatus();
}

void CPublicWidget::SetFrameFocus()
{
    if (NULL != m_pTalkUIFrame)
    {
        m_pTalkUIFrame->setFocus();
    }
}

void CPublicWidget::addTalkUIChild(CDlgBaseTalkUI * pBaseTalkUI)
{
    if (NULL == m_pTalkUIBackground
            || NULL == m_pTalkUIFrameNormal
            || NULL == m_pTalkUIFrameFullScreen
            || NULL == pBaseTalkUI)
    {
        return;
    }

    xFrameLayout* pTargetParent = NULL;
    if (pBaseTalkUI->GetIsFullScreen())
    {
        m_pTalkUIBackground->hide(true);
        m_pTalkUIFrameFullScreen->show();
        pTargetParent = m_pTalkUIFrameFullScreen;
    }
    else
    {
        m_pTalkUIFrameFullScreen->hide(true);
        m_pTalkUIBackground->show();
        pTargetParent = m_pTalkUIFrameNormal;
    }

    if (pBaseTalkUI->parent() == NULL)
    {
//         for (xViewIterator itView = pTargetParent->xViewTree::child_head(); itView.hasData(); ++itView)
//         {
//             xView* pChildView = itView.operator ->();
//             pTargetParent->popupChildView(pChildView);
//         }

        pTargetParent->addChildView(pBaseTalkUI);
    }
}

void CPublicWidget::popTalkUIChild(CDlgBaseTalkUI * pBaseTalkUI)
{
    if (NULL == m_pTalkUIFrameNormal
            || NULL == m_pTalkUIFrameFullScreen
            || NULL == pBaseTalkUI)
    {
        return;
    }

    if (pBaseTalkUI->parent() == m_pTalkUIFrameNormal)
    {
        m_pTalkUIFrameNormal->popupChildView(pBaseTalkUI);
    }
    else if (pBaseTalkUI->parent() == m_pTalkUIFrameFullScreen)
    {
        m_pTalkUIFrameFullScreen->popupChildView(pBaseTalkUI);
    }
}

#if IF_TALKUI_BG_PICTURE
void CPublicWidget::setTalkUIBackGround(CDlgBaseTalkUI * pBaseTalkUI)
{
    if (NULL != m_pTalkUIBackground
            && NULL != pBaseTalkUI)
    {
        xPixmap picBG = GetPixmapByString(pBaseTalkUI->getTalkUIBackGround());

        if (!picBG.isNull())
        {
            m_pTalkUIBackground->setBackground(picBG);
        }
        else
        {
            m_pTalkUIBackground->setBackground(xColor(0, 0, 0, 0));
        }
    }
}

void CPublicWidget::setTalkUIGeometry(int nLeft, int nTop, int nWidth, int nHeight)
{
    if (NULL == m_pTalkUIFrame
            || NULL == m_pTalkUIBackground)
    {
        return;
    }

    m_pTalkUIBackground->layout_marginLeft = nLeft;
    m_pTalkUIBackground->layout_marginTop = nTop;
    m_pTalkUIBackground->layout_width = nWidth;
    m_pTalkUIBackground->layout_height = nHeight;

    m_pTalkUIFrame->onChildrenLayoutChanged();
}
#endif

void CPublicWidget::ShowScrollBar(bool bShow)
{
    if (NULL == m_pScrollBar)
    {
        return;
    }

    if (bShow)
    {
        m_pScrollBar->show();
    }
    else
    {
        m_pScrollBar->hide(false);
    }
}

void CPublicWidget::SetScrollBar(int nPageRange, int nRange, int nPos)
{
    if (NULL != m_pScrollBar)
    {
        m_pScrollBar->SetScrollInfo(nPageRange, nRange, nPos);
    }
}

void CPublicWidget::ShowDtmfEdit(bool bShow)
{
    if (bShow)
    {
        m_objDtmfLine.SetDtmfLineVisible(true);
    }
    else
    {
        m_objDtmfLine.SetDtmfLineVisible(false);
    }
}

void CPublicWidget::ResetDtmf()
{
    m_objDtmfLine.SetDtmfLineVisible(false);
    m_objDtmfLine.ClearData();
}

void CPublicWidget::ShowNetworkHint(bool bShow)
{
    if (NULL == m_pNetWorkHint)
    {
        return;
    }

    if (bShow)
    {
        m_pNetWorkHint->show();
    }
    else
    {
        m_pNetWorkHint->hide(true);
    }
}

void CPublicWidget::SetNetworkHint(TALK_NETWORK_STATUS eStatus)
{
    if (NULL == m_pNetWorkHint)
    {
        return;
    }

#warning
//     eStatus = TALK_NETWORK_STATUS_FAIL;

    bool bResult = false;
    //判断网络是掉线
    if (TALK_NETWORK_STATUS_FAIL == eStatus)
    {
        //finish界面的 网络掉线提示由 finish界面显示了
        //          if (m_pLastTalkUI->GetTalkUIClassName() != DLG_FinishUI
        //结合黑白屏的实现规则，外加pstn不能使用networkisready来判断，因此只有通话界面和会议界面需要显示网络不可用，其他界面不需要提示
        bResult = true;
        m_pNetWorkHint->SetText(_UILOGIC_LANG_TRAN(TRID_NETWORK_UNAVAILABLE));
    }
#if IF_TALKUI_PSTN
    else if (TALK_NETWORK_STATUS_PSTN_FAIL == eStatus)
    {
        bResult = true;
        m_pNetWorkHint->SetText(_UILOGIC_LANG_TRAN(TRID_PSTN_ACCOUNT_UNAVAILABLE));
    }
#endif

    if (bResult)
    {
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_NETWORK, true);
    }
    else
    {
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_NETWORK, false);
    }
    _TalkConflictManager.checkShowUnit();
}

void CPublicWidget::ShowCCStatusHint(bool bShow)
{
    if (NULL == m_pCCStatusHint)
    {
        return;
    }

    if (bShow)
    {
        m_pCCStatusHint->show();
    }
    else
    {
        m_pCCStatusHint->hide(true);
    }
}

void CPublicWidget::RefreshCCStatus()
{
    if (NULL == m_pCCStatusHint)
    {
        return;
    }

#ifdef IF_FEATURE_BROADSOFT_CALL_CENTER
    //暂时测试
    ////设置成需要显示 ccstatus
    //m_bIsShowCCStatus = true;
    //m_pLabCCStatus->SetText("fdasfadsfdasfd");

    //return ;

    //非通话界面不更新
    if (!UIManager_IsInTalk())
    {
        return;
    }

    yl::string strDisplay;

    //获取queuestatus数据
    strDisplay = _TalkUILogicDelegate.GetQueueStatus();

    //有获取到数据
    if (!strDisplay.empty())
    {
        //设置成需要显示 ccstatus
        m_pCCStatusHint->SetText(strDisplay);
    }

    //有获取到数据
    if (!strDisplay.empty())
    {
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_CCSTATUS, true);
    }
    else
    {
        _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_CCSTATUS, false);
    }
    _TalkConflictManager.checkShowUnit();
#endif
}

void CPublicWidget::OnKeyEvent(CDlgBaseTalkUI * pBaseTalkUI, int iKey, bool bPress /*= true*/)
{
    //不在通话中不处理
    if (NULL == pBaseTalkUI
            || !UIManager_IsInTalk()
            //非按下按键不处理
            || !bPress
            //如果当前处于网络不可用的状态下 那么不处理  因为这个时候发dtmf 也无效
            || !m_bNetWorkAvailable)
    {
        return;
    }

    //如果是数字键  先要判断是否需要显示dtmf 条
    if (IsDigitKey(iKey))
    {
        //数字键或ABCD键
        if (!m_objDtmfLine.GetDtmfVisible())
        {
            //Dtmf条不可见情况，若为数字键，询问子窗口，若需要显示则直接显示
            if (pBaseTalkUI->IsShowDtmfLine())
            {
                //把dtmf条的处理放到这边来  因为 所有的key按键dtmf条都需要处理的， 数字键需要输入， 非数字键 需要隐藏
                //而在数字键的处理函数中
                //Dtmf条可见情况，交由Dtmf条处理
                m_objDtmfLine.SetDtmfLineVisible(true);
            }
        }

        //dtmf 处理完成后 不返回   因为 还是要传给 talk 中进行处理的， 现在 dtmf 都是有 对话框  传给 逻辑层
        //交给dtmf处理
        m_objDtmfLine.OnKeyPress(iKey);
    }
    else        //非数字按键 隐藏dtmf条
    {
        //隐藏dtmf条
        m_objDtmfLine.SetDtmfLineVisible(false);
    }
}

void CPublicWidget::slotDtmfVisibleChange(xSignal * sender, WPARAM wParam, LPARAM lParam, int nDataBytes, LPVOID pData)
{
    bool bVisible = (bool)wParam;

    _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_DTMF, bVisible);
    _TalkConflictManager.checkShowUnit();
}

}       //namespace talkui


