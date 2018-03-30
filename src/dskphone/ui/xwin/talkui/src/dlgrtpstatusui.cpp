#include "dlgrtpstatusui.h"
#include "talkuixmlitemname.h"
#include "conflictmanager.h"
#include "talkuixmlpath.h"

////////////talkuilogic///////////////
#include "talkuilogic/talkuilogicdelegate.h"

namespace talkui
{
CDlgRTPStatusUI::CDlgRTPStatusUI()
    : CDlgBaseTalkUI(DLG_RTPStatusUI, eWT_Talk)
    , m_pRTPStatusList(NULL)
    , m_pScrollBar(NULL)
{
    m_eTitleType = TALKUI_TITLE_TYPE_MENU;
}

CDlgRTPStatusUI::~CDlgRTPStatusUI()
{

}

void CDlgRTPStatusUI::LoadLayout()
{
    loadContent(TALKUI_DLGRTPSTATUSUI_XML);
}

void CDlgRTPStatusUI::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);

    m_pRTPStatusList = static_cast<xListView *>(getViewById(TALKUI_RTPSTATUS_LIST));
    m_pScrollBar = static_cast<xScrollBar *>(getViewById(TALKUI_RTP_SCROLLBAR));

    if (NULL != m_pScrollBar)
    {
        m_objRTPStatusAdapter.SetScrollBar(m_pScrollBar);
    }
}

void CDlgRTPStatusUI::ResetWidget()
{
    if (NULL != m_pRTPStatusList)
    {
        m_pRTPStatusList->ClearContent();
    }

    if (NULL != m_pScrollBar)
    {
        m_pScrollBar->hide(true);
    }
}

// 初始化
void CDlgRTPStatusUI::Init()
{
    CDlgBaseTalkUI::Init();

    m_objRTPStatusAdapter.Init();

    if (NULL != m_pRTPStatusList)
    {
        m_pRTPStatusList->SetAdapter(&m_objRTPStatusAdapter);
    }
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgRTPStatusUI::Uninit()
{
    CDlgBaseTalkUI::Uninit();

    ResetWidget();

    if (NULL != m_pRTPStatusList)
    {
        m_pRTPStatusList->SetAdapter(NULL);
    }

    m_objRTPStatusAdapter.Uninit();
}

bool CDlgRTPStatusUI::GetIsFullScreen()
{
    return true;
}

#if IF_TALKUI_BG_PICTURE
yl::string CDlgRTPStatusUI::getTalkUIBackGround()
{
    return "";
}
#endif

// 刷新窗口
void CDlgRTPStatusUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    CDlgBaseTalkUI::UpdateWnd(bUIReturn);
}

// 功能键处理
bool CDlgRTPStatusUI::OnFunckeyPress(int iKey)
{
    if ((PHONE_KEY_UP == iKey) || (PHONE_KEY_DOWN == iKey))
    {
        //先丢给控件处理
        if (NULL == m_pRTPStatusList
                || NULL == m_pScrollBar)
        {
            return true;
        }

        if (PHONE_KEY_UP == iKey)
        {
            m_pRTPStatusList->OnNotifyScrollPrevious();
        }
        else if (PHONE_KEY_DOWN == iKey)
        {
            m_pRTPStatusList->OnNotifyScrollNext();
        }

        int nIndex = m_pRTPStatusList->GetFocusItemDataIndex();
        m_objRTPStatusAdapter.SetCurrentPageIndex(nIndex);
        m_pScrollBar->SetScrollPos(nIndex / m_pRTPStatusList->GetStyle()->GetPageItemCount());
    }

    return CDlgBaseTalkUI::OnFunckeyPress(iKey);
}

//获取 processor
CBaseTalkUIProcessor * CDlgRTPStatusUI::GetTalkUIProcessor()
{
    return GetBindUIProcessor();
}

//是否显示DTMF条
bool CDlgRTPStatusUI::IsShowDtmfLine() const
{
    return false;
}

// 是否显示切换话路的滚动条
bool CDlgRTPStatusUI::IsShowSessionScrollBar()
{
    return false;
}

// 能否切换Session
//内部切换 显示数据
bool CDlgRTPStatusUI::IsChangeSession(int iKey)
{
    return false;
}

//刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
void CDlgRTPStatusUI::RefreshTalkUIData()
{
    CBaseTalkUIProcessor * pProcessor = GetTalkUIProcessor();
    if (NULL != pProcessor)
    {
        pProcessor->SetTalkUIData(UPDATE_DEFAULT);
    }

    //rtp界面不显示ccstatus
    _TalkConflictManager.setConflictUnitEnable(CONFLICT_PRIORITY_CCSTATUS, false);
}

void CDlgRTPStatusUI::UpdateUIRTPStatusData(const talkui_vector<CRTPStatusData> &
        vectorRTPStatusData)
{
    m_objRTPStatusAdapter.SetRTPStatusData(vectorRTPStatusData);
}
////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////

////////////////////////////////继承自  CUILogicRTPStatusUIBase /////////////////////
//设置UILogic 数据
void CDlgRTPStatusUI::SetUILogicData(void * pData, UPDATE_REASON eReason)
{

}
////////////////////////////////继承自  CUILogicRTPStatusUIBase 结束 /////////////////////

}       //namespace talkui



