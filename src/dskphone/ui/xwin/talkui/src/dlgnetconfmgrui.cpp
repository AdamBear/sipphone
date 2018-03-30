#include "dlgnetconfmgrui.h"
#include "talkuicommonfunction.h"
#include "modtalkui.h"
#include "talkuiframemanager.h"
#include "talkuixmlitemname.h"
#include "talkuixmlpath.h"

////////////////// talkuilogic /////////////////////
#include "talkuilogic/talkuilogicdelegate.h"

#include "settingui/include/modsettingui.h"

#include "messagebox/modmessagebox.h"

namespace talkui
{
#define NET_CONF_MGR_REMOVE_ID      100

CDlgNetConfMgrUI::CDlgNetConfMgrUI()
    : CDlgBaseTalkUI(DLG_NetConfMgrUI, eWT_Talk)
{
    m_eTitleType = TALKUI_TITLE_TYPE_MENU;
}

CDlgNetConfMgrUI::~CDlgNetConfMgrUI()
{

}


void CDlgNetConfMgrUI::LoadLayout()
{
    loadContent(TALKUI_DLGCONFMANAGER_XML);
}

void CDlgNetConfMgrUI::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);

    m_pConfManagerList = static_cast<xListView *>(getViewById(TALKUI_CONF_LIST));
}

void CDlgNetConfMgrUI::ResetWidget()
{
    if (NULL != m_pConfManagerList)
    {
        m_pConfManagerList->ClearContent();
    }
}

// 初始化
void CDlgNetConfMgrUI::Init()
{
    CDlgBaseTalkUI::Init();

    m_objConfManagerAdapter.Init();

    if (NULL != m_pConfManagerList)
    {
        m_pConfManagerList->SetAdapter(&m_objConfManagerAdapter);
    }
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgNetConfMgrUI::Uninit()
{
    CDlgBaseTalkUI::Uninit();

    if (NULL != m_pConfManagerList)
    {
        m_pConfManagerList->SetAdapter(NULL);
    }

    ResetWidget();

    m_objConfManagerAdapter.Uninit();
}

bool CDlgNetConfMgrUI::GetIsFullScreen()
{
    return true;
}

#if IF_TALKUI_BG_PICTURE
yl::string CDlgNetConfMgrUI::getTalkUIBackGround()
{
    return "";
}
#endif

// 刷新窗口
void CDlgNetConfMgrUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    CDlgBaseTalkUI::UpdateWnd(bUIReturn);
}

//获取 processor
CBaseTalkUIProcessor * CDlgNetConfMgrUI::GetTalkUIProcessor()
{
    return GetBindUIProcessor();
}

//是否显示DTMF条
bool CDlgNetConfMgrUI::IsShowDtmfLine() const
{
    return false;
}

// 是否显示切换话路的滚动条
bool CDlgNetConfMgrUI::IsShowSessionScrollBar()
{
    return false;
}

// 能否切换Session
bool CDlgNetConfMgrUI::IsChangeSession(int iKey)
{
    return false;
}


//刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
void CDlgNetConfMgrUI::RefreshTalkUIData()
{
    CNetConfMgrUIData * pNetConfMgrUIData = NULL;

    if (NULL != m_pProcessor)
    {
        pNetConfMgrUIData = m_pProcessor->GetNetConfMgrUIData();
    }

    if (NULL == pNetConfMgrUIData)
    {
        return;
    }

    m_objConfManagerAdapter.SetConfManagerData(pNetConfMgrUIData->listContactInfo, true);
}

//更新通话图标
void CDlgNetConfMgrUI::UpdateTalkUIIcons()
{

}


//设置UILogic 数据
void CDlgNetConfMgrUI::SetUILogicData(void * pData, UPDATE_REASON eReason)
{
    //不做处理  统一放到RefreshTalkUIData处理
}


int CDlgNetConfMgrUI::GetFocusCallID()
{
    return m_objConfManagerAdapter.GetFocusCallID();
}

void CDlgNetConfMgrUI::ShowRemoveMemberBox(yl::string strName)
{
    MSGBOX_INFO stMsgInfo;
    stMsgInfo.pAction = this;
    stMsgInfo.eType = MESSAGEBOX_OkCancel;
    stMsgInfo.strTitle = _UILOGIC_LANG_TRAN(TRID_REMOVE_PARTICIPANT);

    yl::string strNote = _UILOGIC_LANG_TRAN(TRID_DO_YOU_WANT_TO_REMOVE) + " " + strName + " " + _UILOGIC_LANG_TRAN(TRID_FROM_THE_CONFERENCE);

    stMsgInfo.strNote = strNote;
    stMsgInfo.nMsTime = -1;
    stMsgInfo.iID = NET_CONF_MGR_REMOVE_ID;

    MessageBox_ShowCommonMsgBox(stMsgInfo);
}

void CDlgNetConfMgrUI::MessageBoxCallBack(CMessageBoxBase * pMessageBox)
{
    if (NULL == pMessageBox
            || MessageBox_OK != pMessageBox->GetResult())
    {
        return;
    }

    if (NET_CONF_MGR_REMOVE_ID == pMessageBox->GetID())
    {
        if (NULL != m_pProcessor)
        {
            m_pProcessor->DealRemoveMember();
        }
    }
}


}       //namespace talkui


