#include "dlgacdcallinfoui.h"
#include "talkuixmlitemname.h"
#include "conflictmanager.h"
#include "talkuixmlpath.h"

////////////talkuilogic///////////////
#include "talkuilogic/talkuilogicdelegate.h"

namespace talkui
{
CDlgACDCallInfoUI::CDlgACDCallInfoUI()
    : CDlgBaseTalkUI(DLG_ACDCallInfoUI, eWT_Talk)
    , m_pACDInfoList(NULL)
    , m_pScrollBar(NULL)
{
    m_eTitleType = TALKUI_TITLE_TYPE_MENU;
}

CDlgACDCallInfoUI::~CDlgACDCallInfoUI()
{

}

void CDlgACDCallInfoUI::LoadLayout()
{
    loadContent(TALKUI_DLGACDINFOUI_XML);
}

void CDlgACDCallInfoUI::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);

    m_pACDInfoList = static_cast<xListView *>(getViewById(TALKUI_ACDINFO_LIST));
    m_pScrollBar = static_cast<xScrollBar *>(getViewById(TALKUI_ACDINFO_SCROLLBAR));

    if (NULL != m_pScrollBar)
    {
        m_objACDInfoAdapter.SetScrollBar(m_pScrollBar);
    }
}

void CDlgACDCallInfoUI::ResetWidget()
{
    if (NULL != m_pACDInfoList)
    {
        m_pACDInfoList->ClearContent();
    }

    if (NULL != m_pScrollBar)
    {
        m_pScrollBar->hide(true);
    }
}

// 初始化
void CDlgACDCallInfoUI::Init()
{
    CDlgBaseTalkUI::Init();

    m_objACDInfoAdapter.Init();

    if (NULL != m_pACDInfoList)
    {
        m_pACDInfoList->SetAdapter(&m_objACDInfoAdapter);
    }
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgACDCallInfoUI::Uninit()
{
    CDlgBaseTalkUI::Uninit();

    ResetWidget();

    if (NULL != m_pACDInfoList)
    {
        m_pACDInfoList->SetAdapter(NULL);
    }

    m_objACDInfoAdapter.Uninit();
}

bool CDlgACDCallInfoUI::GetIsFullScreen()
{
    return true;
}

#if IF_TALKUI_BG_PICTURE
yl::string CDlgACDCallInfoUI::getTalkUIBackGround()
{
    return "";
}
#endif

// 刷新窗口
void CDlgACDCallInfoUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    CDlgBaseTalkUI::UpdateWnd(bUIReturn);
}

bool CDlgACDCallInfoUI::OnFunckeyPress(int iKey)
{
    if ((PHONE_KEY_UP == iKey) || (PHONE_KEY_DOWN == iKey))
    {
        if (NULL == m_pACDInfoList
                || NULL == m_pScrollBar)
        {
            return false;
        }

        if (PHONE_KEY_UP == iKey)
        {
            m_pACDInfoList->OnNotifyScrollPrevious();
        }
        else if (PHONE_KEY_DOWN == iKey)
        {
            m_pACDInfoList->OnNotifyScrollNext();
        }

        int nIndex = m_pACDInfoList->GetFocusItemDataIndex();
        m_pScrollBar->SetScrollPos(nIndex / m_pACDInfoList->GetStyle()->GetPageItemCount());
    }

    return CDlgBaseTalkUI::OnFunckeyPress(iKey);
}

//获取 processor
CBaseTalkUIProcessor * CDlgACDCallInfoUI::GetTalkUIProcessor()
{
    return GetBindUIProcessor();
}

//是否显示DTMF条
bool CDlgACDCallInfoUI::IsShowDtmfLine() const
{
    return false;
}

// 是否显示切换话路的滚动条
bool CDlgACDCallInfoUI::IsShowSessionScrollBar()
{
    return false;
}

// 能否切换Session
bool CDlgACDCallInfoUI::IsChangeSession(int iKey)
{
    return false;
}

//刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
void CDlgACDCallInfoUI::RefreshTalkUIData()
{
    CACDCallInfoUIData * pACDCallInfoUIData = NULL;

    if (NULL != m_pProcessor)
    {
        pACDCallInfoUIData = m_pProcessor->GetACDCallInfoUIData();
    }

    if (NULL == pACDCallInfoUIData)
    {
        return ;
    }

    //设置数据
    m_objACDInfoAdapter.SetACDInfo(pACDCallInfoUIData->strWaitTime
                                   , pACDCallInfoUIData->strCCName
                                   , pACDCallInfoUIData->strCCUID
                                   , pACDCallInfoUIData->strQueue
                                   , pACDCallInfoUIData->strCallsLongest);

    _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_CCSTATUS, true);
//     if (pACDCallInfoUIData->strQueue.empty()
//             && pACDCallInfoUIData->strCallsLongest.empty())
//     {
//         //callslongest为空 那么全局可以显示 ccstatus 信息
//         _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_CCSTATUS, true);
//     }
//     else
//     {
//         //callslongest不为空 那么全局可以显示 ccstatus 信息
//         _TalkConflictManager.setConflictUnitShow(CONFLICT_PRIORITY_CCSTATUS, false);
//     }
}

////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////

////////////////////////////////继承自  CUILogicACDCallInfoUIBase /////////////////////
//设置UILogic 数据
void CDlgACDCallInfoUI::SetUILogicData(void * pData, UPDATE_REASON eReason)
{

}

////////////////////////////////继承自  CUILogicACDCallInfoUIBase 结束 /////////////////////

}       //namespace talkui



