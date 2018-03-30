#include "dlgconfmanagerui.h"
#include "talkuicommonfunction.h"
#include "talkuixmlitemname.h"
#include "talkuixmlpath.h"

////////////talkuilogic///////////////
#include "talkuilogic/talkuilogicdelegate.h"

#include "talk/talklogic/include/modtalklogic.h"

#include "settingui/include/modsettingui.h"

namespace talkui
{
CDlgConfManagerUI::CDlgConfManagerUI()
    : CDlgBaseTalkUI(DLG_ConfManagerUI, eWT_Talk)
    , m_pConfManagerList(NULL)
{
    m_eTitleType = TALKUI_TITLE_TYPE_MENU;
}

CDlgConfManagerUI::~CDlgConfManagerUI()
{

}


void CDlgConfManagerUI::LoadLayout()
{
    loadContent(TALKUI_DLGCONFMANAGER_XML);
}

void CDlgConfManagerUI::loadChildrenElements(xml_node & node)
{
    xFrameLayout::loadChildrenElements(node);

    m_pConfManagerList = static_cast<xListView *>(getViewById(TALKUI_CONF_LIST));
}

void CDlgConfManagerUI::ResetWidget()
{
    if (NULL != m_pConfManagerList)
    {
        m_pConfManagerList->ClearContent();
    }
}

// 初始化
void CDlgConfManagerUI::Init()
{
    CDlgBaseTalkUI::Init();

    m_objConfManagerAdapter.Init();

    if (NULL != m_pConfManagerList)
    {
        m_pConfManagerList->SetAdapter(&m_objConfManagerAdapter);
    }
}

// 释放数据，将窗口清空回复到初始化状态
void CDlgConfManagerUI::Uninit()
{
    CDlgBaseTalkUI::Uninit();

    if (NULL != m_pConfManagerList)
    {
        m_pConfManagerList->SetAdapter(NULL);
    }

    ResetWidget();

    m_objConfManagerAdapter.Uninit();
}

bool CDlgConfManagerUI::GetIsFullScreen()
{
    return true;
}

#if IF_TALKUI_BG_PICTURE
yl::string CDlgConfManagerUI::getTalkUIBackGround()
{
    return "";
}
#endif

// 刷新窗口
void CDlgConfManagerUI::UpdateWnd(bool bUIReturn/* = false*/)
{
    CDlgBaseTalkUI::UpdateWnd(bUIReturn);
}

// 功能键处理
bool CDlgConfManagerUI::OnFunckeyPress(int iKey)
{
    return CDlgBaseTalkUI::OnFunckeyPress(iKey);
}

//获取 processor
CBaseTalkUIProcessor * CDlgConfManagerUI::GetTalkUIProcessor()
{
    return GetBindUIProcessor();
}

//是否显示DTMF条
bool CDlgConfManagerUI::IsShowDtmfLine() const
{
    return false;
}

// 是否显示切换话路的滚动条
bool CDlgConfManagerUI::IsShowSessionScrollBar()
{
    return false;
}

// 能否切换Session
bool CDlgConfManagerUI::IsChangeSession(int iKey)
{
    return false;
}


//刷新通话UI数据  这边需要从processor 获取 数据 设置到通话UI子界面上
void CDlgConfManagerUI::RefreshTalkUIData()
{
    CConfManagerUIData * pConfManagerUIData = NULL;

    if (NULL != m_pProcessor)
    {
        pConfManagerUIData = m_pProcessor->GetConfManagerUIData();
    }

    if (NULL == pConfManagerUIData)
    {
        return ;
    }

    m_objConfManagerAdapter.SetConfManagerData(pConfManagerUIData->listContactInfo, true);
}

//更新通话图标
void CDlgConfManagerUI::UpdateTalkUIIcons()
{

}

////////////////////////////////继承自  dlgbasetalkui 结束/////////////////////

////////////////////////////////继承自  CUILogicConfManagerUIBase /////////////////////
//设置UILogic 数据
void CDlgConfManagerUI::SetUILogicData(void * pData, UPDATE_REASON eReason)
{

}

//获取焦点话路callid
int CDlgConfManagerUI::GetFocusCallID()
{
    return m_objConfManagerAdapter.GetFocusCallID();
}

void CDlgConfManagerUI::ShowACDDispCodeMenu(int nCallID)
{
    SettingUI_EnterPage(yl::string("disposition_code"), &nCallID);
}

void CDlgConfManagerUI::ShowEmergencyMenu(int nCallID)
{
    SettingUI_EnterPage(yl::string("emergency_escalation"), &nCallID);
}

////////////////////////////////继承自  CUILogicConfManagerUIBase 结束 /////////////////////

}       //namespace talkui



